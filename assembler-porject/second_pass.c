#include <stdio.h>
#include <stdlib.h>
#include "second_pass.h"
#include "code.h"
#include "utils.h"
#include "string.h"

/**
 * Builds the additional data word for operand in the second pass, if needed.
 * @param line The current source line info
 * @param curr_ic Current code counter pointer of source code line
 * @param ic code counter pointer 
 * @param operand The operand string
 * @param code_img The code image array
 * @param symbol_table The symbol table
 * @return Whether succeeded
 */
static int process_operand(line_info line, long* curr_ic, long* ic, char* operand, machine_word** code_img, table* symbol_table);

/**
 * Find the symbol that need replace in a code line, and replace it by the address in the symbol table.
 * @param line The current source line info
 * @param ic A pointer to the current code counter
 * @param code_img The code image
 * @param symbol_table The symbol table
 * @return Whether succeeded
 */
static bool add_symbol_to_code(line_info line, long* ic, machine_word** code_img, table* symbol_table);

bool process_line_sp(line_info line, long* ic, machine_word** code_img, table* symbol_table){
  char* colonIndex;
	char* token;
	long i = 0;
	SKIP_TO_NOT_WHITE(line.content, i)

  if (line.content[i] == ';' || line.content[i] == '\n'){
    return TRUE;
  }
  colonIndex = strchr(line.content, ':');
  if (colonIndex != NULL) {
		i = colonIndex - line.content;
		i++;
	}
  SKIP_TO_NOT_WHITE(line.content, i)
  /* it's .instruction */
	if (line.content[i] == '.'){
    /* if it's entry - add it to the symbol table */
		if (strncmp(".entry", line.content, 6) == 0){
      i += 6;
      SKIP_TO_NOT_WHITE(line.content, i)
      token = strtok(line.content + i, " \n\t");
      /* if label is already marked as entry, ignore. */
			if (token == NULL) {
				print_error(line, "You have to specify a label name for .entry instruction.");
				return FALSE;
			}
      if (find_by_types(*symbol_table, token, 1, ENTRY_SYMBOL) == NULL){
        table_entry* entry;
				token = strtok(line.content + i, "\n"); /*get name of label*/

        /* if symbol is not defined as data/code */
				if ((entry = find_by_types(*symbol_table, token, 2, DATA_SYMBOL, CODE_SYMBOL)) == NULL){
          /* if defined as external print error */
					if ((entry = find_by_types(*symbol_table, token, 1, EXTERNAL_SYMBOL)) != NULL){
            print_error(line, "The symbol %s can be either external or entry, but not both.", entry->key);
            return FALSE;
          }
          /* otherwise print more general error */
					print_error(line, "The symbol %s for .entry is undefined.", token);
					return FALSE;
        }
        add_table_item(symbol_table, token, entry->value, ENTRY_SYMBOL);
      }
    }
    return TRUE;
  }
  return add_symbol_to_code(line, ic, code_img, symbol_table);
}

static bool add_symbol_to_code(line_info line, long* ic, machine_word** code_img, table* symbol_table){
  char temp[80];
	char* operands[3];
	int i = 0, operand_count;
	bool isvalid = TRUE;
	long curr_ic = *ic; /* using curr_ic as temp index inside the code image, in the current line code+data words */

  /* get the total word length of current code text line in code binary image */
	int length = code_img[(*ic) - IC_INIT_VALUE]->length;
  /* if the length is 1, then there's only the code word, no data. */
  if(length > 1){
    /* skip command and get the operands */
		SKIP_TO_NOT_WHITE(line.content, i)
		find_label(line, temp);
    if (temp[0] != '\0') { /* if symbol is defined */
      /* move i right after it's end */
			for (; line.content[i] && line.content[i] != '\n' && line.content[i] != EOF && line.content[i] != ' ' && line.content[i] != '\t'; i++){
			    i++;
		  }
    }
    SKIP_TO_NOT_WHITE(line.content, i)
		/* skip command */
    for ( ; line.content[i] && line.content[i] != ' ' && line.content[i] != '\t' && line.content[i] != '\n' &&
		       line.content[i] != EOF; i++);
		SKIP_TO_NOT_WHITE(line.content, i)
    /* analyze operands. send NULL as string of command because no error will be printed, and that's the only usage for it there. */
		get_operands(line, i, operands, &operand_count, NULL);

    /* process operands, if needed. if failed return failure. otherwise continue */
    while(operand_count > 0){
      isvalid = process_operand(line, &curr_ic, ic, operands[operand_count-1], code_img, symbol_table);
      free(operands[operand_count-1]);
      if(!isvalid){
        return FALSE;
      }
      operand_count--;
    }
  } 
  /* make the current pass IC as the next line ic */
	(*ic) = (*ic) + length;
	return TRUE;
}

static int process_operand(line_info line, long* curr_ic, long* ic, char* operand, machine_word** code_img, table* symbol_table){
  operand_type tpye = get_operand_type(operand);

  if(tpye == LABEL_TYPE){
    long data_to_add;
    table_entry* entry = find_by_types(*symbol_table, operand, 3, DATA_SYMBOL, CODE_SYMBOL, EXTERNAL_SYMBOL);
    if (entry == NULL) {
			print_error(line, "The symbol %s not found", operand);
			return FALSE;
		} 
    /*found symbol*/
		data_to_add = entry->value;
    
    /* add to externals reference table if it's an external. increase ic because it's the next data word */
    if (entry->type == EXTERNAL_SYMBOL) {
			add_table_item(symbol_table, operand, *curr_ic, EXTERNAL_REFERENCE);
		}
    else{
      if(code_img[(*ic)-IC_INIT_VALUE]->word.code->opcode >= JMP_OP && code_img[(*ic)-IC_INIT_VALUE]->word.code->opcode <= CALL_OP){
        if(code_img[(*ic)-IC_INIT_VALUE]->word.code->commad_type.j->reg == 0){
          code_img[(*ic)-IC_INIT_VALUE]->word.code->commad_type.j->address = data_to_add;
        }
      }
      else if(code_img[(*ic)-IC_INIT_VALUE]->word.code->opcode >= BNE_OP && code_img[(*ic)-IC_INIT_VALUE]->word.code->opcode <= BGT_OP){
        data_to_add = find_by_name(*symbol_table, operand);
        /* calculate the address distance */
        code_img[(*ic)-IC_INIT_VALUE]->word.code->commad_type.i->immed = data_to_add - (*ic);
      }
    }
  }
  return TRUE;
}