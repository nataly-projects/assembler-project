/* Contains functions that are related to the first pass */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "code.h"
#include "utils.h"
#include "instructions.h"
#include "first_pass.h"

/**
 * Processes a single code line in the first pass.
 * Adds the code build binary structure to the code_img,
 * encodes immediately-addresses operands and leaves required data word that use labels NULL.
 * @param line The code line to process
 * @param i Where to start processing the line from
 * @param ic A pointer to the current code counter
 * @param code_img The code image array
 * @param tab The symbol table
 * @return Whether succeeded or not.
 */
static bool process_code(line_info line, int i, long* ic, machine_word** code_img, table* tab);

bool process_line_fp(line_info line, long* IC, long* DC, machine_word** code_img, table* symbol_table, data_word** data){
  int i=0, j;
	char symbol[MAX_LINE_LENGTH];
	instruction instruction;
  SKIP_TO_NOT_WHITE(line.content, i) /* move to next non-white char */

  if (!line.content[i] || line.content[i] == '\n' || line.content[i] == EOF || line.content[i] == ';'){
    return TRUE; /* empty/Comment line - no errors found */
  }
  /* check if symbol (*:). if tried to define label, but it's invalid, return that an error occurred. */
	if (!find_label(line, symbol)) {
		return FALSE;
	}
  /* if illegal name */
	if (symbol[0] && !is_valid_label_name(symbol)) {
		print_error(line, "Illegal label name: %s", symbol);
		return FALSE;
	}
	if (symbol[0] != '\0') {
		for ( ; line.content[i] != ':'; i++); /* if symbol detected, start analyzing from it's deceleration end */
		i++;
	}
  SKIP_TO_NOT_WHITE(line.content, i) /* move to next not-white char */
  if (line.content[i] == '\n'){ /* label-only line - skip */
    return TRUE;
  }

  /* if already defined as data/external/code and not empty line */
	if (find_by_types(*symbol_table, symbol, 3, EXTERNAL_SYMBOL, DATA_SYMBOL, CODE_SYMBOL)) {
		print_error(line, "Symbol %s is already defined.", symbol);
		return FALSE;
	}
  /* check if it's an instruction (starting with '.') */
	instruction = find_instruction_from_index(line, &i);
	if (instruction == ERROR_INST) { /* syntax error found */
		return FALSE;
	}

  SKIP_TO_NOT_WHITE(line.content, i)
  /* is it's an instruction */
  if (instruction != NONE_INST){
    /* if .asciz or .dh, .dw, .db, and symbol defined, put it into the symbol table */
		if ((instruction == ASCIZ_INST || instruction == DB_INST || instruction == DW_INST || instruction == DH_INST) && symbol[0] != '\0'){
          /* is data or string, add DC with the symbol to the table as data */
			    add_table_item(symbol_table, symbol, *DC, DATA_SYMBOL);
    }
    /* if asciz, encode into data image buffer and increase dc as needed. */
		if (instruction == ASCIZ_INST){
      return process_asciz_instruction(line, i, DC, data);
    }
    /* if data instructions: .db, .dh, .dw, do same but need to parse the numbers. */
		else if (instruction == DB_INST || instruction == DH_INST || instruction == DW_INST){
      return process_data_instruction(line, i,DC, instruction, data);
    }
    /* if .extern, add to externals symbol table */
		else if (instruction == EXTERN_INST){
      SKIP_TO_NOT_WHITE(line.content, i)
      /* if external symbol detected, start analyzing from it's deceleration end */
			for (j = 0; line.content[i] && line.content[i] != '\n' && line.content[i] != '\t' && line.content[i] != ' ' && line.content[i] != EOF; i++, j++) {
				symbol[j] = line.content[i];
			}
      symbol[j] = 0;
      /* if invalid external label name, it's an error */
			if (!is_valid_label_name(symbol)) {
				print_error(line, "Invalid external label name: %s", symbol);
				return TRUE;
			}
      add_table_item(symbol_table, symbol, 0, EXTERNAL_SYMBOL); /* Extern value is defaulted to 0 */
    }
    /* if entry and symbol defined, print error */
    else if(instruction == ENTRY_INST && symbol[0] != '\0'){
      print_error(line, "Can't define a label to an entry instruction.");
			return FALSE;
    } /* .entry is handled in second pass! */
  }
  /* not instruction, it's a command */
  else{
    /* if symbol defined, add it to the table */
		if (symbol[0] != '\0'){
      add_table_item(symbol_table, symbol, *IC, CODE_SYMBOL);
    }
    /* analyze the code */
		return process_code(line, i, IC, code_img, symbol_table);
  }
  return TRUE;
}

static bool process_code(line_info line, int i, long* ic, machine_word** code_img, table* tab){
  char operation[8]; /* stores the string of the current code command */
	char* operands[3]; /* 3 strings, each for operand */
  opcode curr_opcode; /* the current opcode and funct values */
	funct curr_funct;
	code_word* codeword; /* The current code word */
	long ic_before;
	int j, operand_count;
	machine_word* word_to_write;

	/* skip white chars */
	SKIP_TO_NOT_WHITE(line.content, i)
  /* until white char, end of line, or too big command, copy it */
	for (j = 0; line.content[i] && line.content[i] != '\t' && line.content[i] != ' ' && line.content[i] != '\n' 
        && line.content[i] != EOF && j < 6; i++, j++) {
		  operation[j] = line.content[i];
	}
  operation[j] = '\0'; /* end of string */
  /* get opcode & funct by command name into curr_opcode & curr_funct */
	get_opcode_and_funct(operation, &curr_opcode, &curr_funct);

  /* if invalid operation (opcode is NONE_OP=-1), print and skip processing the line. */
	if (curr_opcode == NONE_OP) {
		print_error(line, "Unrecognized command: %s.", operation);
		return FALSE; /* an error occurred */
	}

  /* separate operands and get their count */
	if (!get_operands(line, i, operands, &operand_count, operation))  {
		return FALSE;
	}

  /* build code word struct to store in code image array */
	if ((codeword = build_code_word(line, curr_opcode, curr_funct, operand_count, operands, tab)) == NULL) {
		/* release allocated memory for operands */
    while(operand_count > 0){
      free(operands[operand_count-1]);
      operand_count--;
    }
		return FALSE;
	}
  /* ic in position of new code word */
	ic_before = *ic;
  /* allocate memory for a new word in the code image, and put the code word into it */
	word_to_write = (machine_word *) malloc_with_check(sizeof(machine_word));
  (word_to_write->word).code = codeword;
  code_img[(*ic) - IC_INIT_VALUE] = word_to_write; /* avoid "spending" cells of the array, by starting from initial value of ic */

  (*ic)+=4; /* increase ic to point the next cell */
  /* add the final length (of code word + data words) to the code word struct: */
	code_img[ic_before - IC_INIT_VALUE]->length = (*ic) - ic_before;

  /* release allocated memory for operands */
  while(operand_count > 0){
    free(operands[operand_count-1]);
    operand_count--;
  }
  return TRUE; /* no errors */
}