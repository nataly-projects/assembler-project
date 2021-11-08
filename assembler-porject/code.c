#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include "code.h"
#include "utils.h"

/**
 * Validates the operands type, and prints error message if needed.
 * @param line The current line information
 * @param op1_type The current type of the first operand
 * @param op2_type The current type of the second operand
 * @param op3_type The current type of the third operand
 * @param op1_valid_type_count The count of valid types for the first operand
 * @param op2_valid_type_count The count of valid types for the second operand
 * @param op3_valid_type_count The count of valid types for the third operand
 * @param ... The valid types for operand
 * @return Whether the types are valid 
 */
static bool validate_operand_type(line_info line, operand_type op1_type, operand_type op2_type, operand_type op3_type,
		int op1_valid_type_count, int op2_valid_type_count,int op3_valid_type_count,...); 

/**
 * Validates the operands count by the opcode of the command
 * @param line The current source line info
 * @param op1_type The type of the first operand
 * @param op2_type The type of the second operand
 * @param op3_type The type of the third operand
 * @param curr_opcode The opcode of the current command
 * @param op_count The operand count of the current commad
 * @return Whether the operands are valids
 */
static bool validate_operand_by_opcode(line_info line, operand_type op1_type,operand_type op2_type, operand_type op3_type, opcode curr_opcode,
   int op_count); 


/* A single lookup table element */
struct cmd_lookup_element {
	char *cmd;
	opcode opc;
	funct func;
};

/* A lookup table for opcode & funct by command name */
static struct cmd_lookup_element lookup_table[] = {
		{"add", ADD_OP, ADD_FUNCT},
		{"sub",SUB_OP, SUB_FUNCT},
		{"and",AND_OP, AND_FUNCT},
		{"or",OR_OP, OR_FUNCT},
		{"nor",NOR_OP, NOR_FUNCT},
		{"move",MOVE_OP, MOVE_FUNCT},
		{"mvhi",MVHI_OP, MVHI_FUNCT},
		{"mvlo",MVLO_OP, MVLO_FUNCT},
		{"addi",ADDI_OP, NONE_FUNCT},
		{"subi",SUBI_OP, NONE_FUNCT},
    {"andi",ANDI_OP, NONE_FUNCT},
		{"ori",ORI_OP, NONE_FUNCT},
		{"nori",NORI_OP, NONE_FUNCT},
		{"bne",BNE_OP, NONE_FUNCT},
		{"beq",BEQ_OP, NONE_FUNCT},
		{"blt",BLT_OP, NONE_FUNCT},
    {"bgt",BGT_OP, NONE_FUNCT},
		{"lb",LB_OP, NONE_FUNCT},
		{"sb",SB_OP, NONE_FUNCT},
		{"lw",LW_OP, NONE_FUNCT},
		{"sw",SW_OP, NONE_FUNCT},
		{"lh",LH_OP, NONE_FUNCT},
		{"sh",SH_OP, NONE_FUNCT},
		{"jmp",JMP_OP, NONE_FUNCT},
		{"la",LA_OP, NONE_FUNCT},
		{"call",CALL_OP, NONE_FUNCT},
		{"stop",STOP_OP, NONE_FUNCT},
		{NULL, NONE_OP, NONE_FUNCT}
};

void get_opcode_and_funct(char* cmd, opcode* opcode_des, funct* funct_des) {
	struct cmd_lookup_element *e;
	*opcode_des = NONE_OP;
	*funct_des = NONE_FUNCT;

	/* iterate through the lookup table, if commands are same return the opcode of found. */
	for (e = lookup_table; e->cmd != NULL; e++) {
		if (strcmp(e->cmd, cmd) == 0) {
			*opcode_des = e->opc;
			*funct_des = e->func;
			return;
		}
	}
}

int get_register_by_name(char *name) {
  if(strlen(name) == 2){
    if (name[0] == '$' && isdigit(name[1]) &&  name[2] == '\0'){
      int digit = name[1]-'0';
      if(digit >= 0 && digit <= 9){
        return digit;
      }
    }
  }
  else if(strlen(name) == 3){
    if (name[0] == '$' && isdigit(name[1]) && isdigit(name[2]) &&  name[3] == '\0'){
      int digit = (name[1]-'0')*10 +(name[2]-'0');
      if(digit >= 0 && digit <=31){
        return digit;
      }
    }
  }
	return NONE_REG; /* no match */
}

bool get_operands(line_info line, int i, char** destination, int* operand_count, char* command){
  int j;
	*operand_count = 0;
	destination[0] = destination[1] = destination[2] = NULL;
	SKIP_TO_NOT_WHITE(line.content, i)
  if (line.content[i] == ',') {
		print_error(line, "Unexpected comma after command.");
		return FALSE; /* an error occurred */
	}
  for (*operand_count = 0; line.content[i] != EOF && line.content[i] != '\n' && line.content[i]; ) {
    if(*operand_count == 3){
      print_error(line, "Too many operands for operation", *operand_count);
			free(destination[0]);
			free(destination[1]);
      free(destination[2]); 
			return FALSE; /* an error occurred */
    }

    /* allocate memory to save the operand */
		destination[*operand_count] = malloc_with_check(MAX_LINE_LENGTH);
    /* as long we're still on same operand */
		for (j = 0; line.content[i] && line.content[i] != '\t' && line.content[i] != ' ' && line.content[i] != '\n' 
        && line.content[i] != EOF && line.content[i] != ','; i++, j++) {
			    destination[*operand_count][j] = line.content[i];
		}
    destination[*operand_count][j] = '\0';
		(*operand_count)++; /* saved another operand! */
		SKIP_TO_NOT_WHITE(line.content, i)

    if (line.content[i] == '\n' || line.content[i] == EOF || !line.content[i]){
      break;
    }
    else if(line.content[i] != ','){
      /* after operand and after white chars there's something that isn't ',' or end of line.. */
			print_error(line, "Expecting ',' between operands");

      /* release operands dynamically allocated memory */
      while(*operand_count > 0){
        free(destination[(*operand_count) - 1]);
        (*operand_count)--;
      }
			return FALSE;
    }

    i++;
		SKIP_TO_NOT_WHITE(line.content, i)
    /* if there was just a comma, then (optionally) white char(s) and then end of line */
		if (line.content[i] == '\n' || line.content[i] == EOF || !line.content[i]){
      print_error(line, "Missing operand after comma.");
    }
    else if (line.content[i] == ','){
      print_error(line, "Multiple consecutive commas.");
    }
    else{
      continue; /* no errors, continue */
    }
    { /* error found! - didn't continue. release operands dynamically allocated memory */
      while(*operand_count > 0){
        free(destination[(*operand_count) - 1]);
        (*operand_count)--;
      }
			return FALSE;
    }
  }
  return TRUE;
}

code_word* build_code_word(line_info line, opcode curr_opcode, funct curr_funct, int op_count, char* operands[3], table* tab){
  code_word* codeword;
  long value;
  i_command* i_cmd;
  r_command* r_cmd;
  j_command* j_cmd;
  /* get operands types and validate them */
	operand_type op1_type = op_count >= 1 ? get_operand_type(operands[0]) : NONE_TYPE;
	operand_type op2_type = op_count >= 2 ? get_operand_type(operands[1]) : NONE_TYPE;
  operand_type op3_type = op_count == 3 ? get_operand_type(operands[2]) : NONE_TYPE;

  /* validate operands by opcode */
	if (!validate_operand_by_opcode(line, op1_type, op2_type, op3_type, curr_opcode, op_count)) {
		return NULL;
	}
  /* create the code word by the data */
	codeword = (code_word *) malloc_with_check(sizeof(code_word));
  codeword->opcode = curr_opcode;
  /* check if need to set the registers bits */
  if (curr_opcode >= ADD_OP && curr_opcode <= MVLO_OP) { /* R COMMAND */
    codeword->command = 'r';
    r_cmd = (r_command *) malloc_with_check(sizeof(r_command));
    r_cmd->funct = curr_funct; /*if no funct, curr_funct = NONE_FUNCT = 0 */
    r_cmd->NONE = 0;
    /* default values of register bits are 0 */
    r_cmd->rt = r_cmd->rd = r_cmd->rs = 0;
    r_cmd->rs = get_register_by_name(operands[0]);
    if(op_count == 2){
      r_cmd->rd = get_register_by_name(operands[1]);
    }
    else if(op_count == 3){
      r_cmd->rt = get_register_by_name(operands[1]);
      r_cmd->rd = get_register_by_name(operands[2]);
    }
    (codeword->commad_type).r = r_cmd;
  }
  else if( (curr_opcode >= ADDI_OP && curr_opcode <= NORI_OP) || (curr_opcode >= LB_OP && curr_opcode <= SH_OP)){ /* I COMMAND */
    codeword->command = 'i';
    i_cmd = (i_command *) malloc_with_check (sizeof(i_command));
    i_cmd->rs = get_register_by_name(operands[0]);
    i_cmd->immed = atoi(operands[1]);
    i_cmd->rt = get_register_by_name(operands[2]);
    (codeword->commad_type).i = i_cmd;
  }
  else if(curr_opcode >= BNE_OP && curr_opcode <= BGT_OP){ /* I COMMAND */
    codeword->command = 'i';
    i_cmd = (i_command *) malloc_with_check (sizeof(i_command));
    i_cmd->rs = get_register_by_name(operands[0]);
    i_cmd->rt = get_register_by_name(operands[1]);
    
    i_cmd->immed = atoi(operands[2]); 
    (codeword->commad_type).i = i_cmd;
  }
  else if(curr_opcode >= JMP_OP && curr_opcode <= STOP_OP){ /* J COMMAND */
    codeword->command = 'j';
    j_cmd = (j_command *) malloc_with_check(sizeof(j_command));
    if(op1_type == LABEL_TYPE){
      j_cmd->reg = 0;
      value = find_by_name(*tab, operands[0]);
      j_cmd->address = value;
    }
    else if(op1_type == REGISTER_TYPE){
      j_cmd->reg = 1;
      j_cmd->address = get_register_by_name(operands[0]);
    }
    else if(curr_opcode == STOP_OP){
      j_cmd->reg = 0;
      j_cmd->address = 0;
    }
    (codeword->commad_type).j = j_cmd;
  }
  return codeword;
}

operand_type get_operand_type(char* operand){
  int num;
  /* if nothing, just return none */
	if (operand[0] == '\0'){
    return NONE_TYPE;
  }
  /* if first char is '$', and after number between 0-31 and in the end  is end of string, it's a register */
	else if (operand[0] == '$' && operand[1] >= '0' && operand[1] <= '9' && operand[2] == '\0'){
    return REGISTER_TYPE;
  }
  else if(operand[0] == '$' && strlen(operand) == 3){
    num = (operand[1]-'0')*10 + (operand[2]-'0');
    if (num >= 0 && num <= 31 && operand[3] == '\0'){
      return REGISTER_TYPE;
    }
  }
  /* if operand starts with +/- and a number right after that, it's immediately type */
	else if (is_int(operand) || ((operand[0] == '+' || operand[0] == '-')  && is_int(operand+1)) ){
    return IMMEDIATE_TYPE;
  }
  	/* if operand is a valid label name, it's label type */
	else if (is_valid_label_name(operand)){
    return LABEL_TYPE;
  }  
  return NONE_TYPE;
}

static bool validate_operand_by_opcode(line_info line, operand_type op1_type,operand_type op2_type,operand_type op3_type, opcode curr_opcode, int op_count){
  
  if( (curr_opcode >= ADD_OP && curr_opcode <= NOR_OP) || (curr_opcode >= ADDI_OP && curr_opcode <= SH_OP) ){
    /* 3 operands required */
		if (op_count != 3) {
			print_error(line, "Operation requires 3 operands, got %d", op_count);
			return FALSE;
		}
    if(curr_opcode >= ADD_OP && curr_opcode <= NOR_OP){
      return validate_operand_type(line, op1_type, op2_type,op3_type,1,1,1,REGISTER_TYPE,REGISTER_TYPE,REGISTER_TYPE);
    }
    if( (curr_opcode >= ADDI_OP && curr_opcode <= NORI_OP) || (curr_opcode >= LB_OP && curr_opcode <=SH_OP) ){
      return validate_operand_type(line, op1_type, op2_type,op3_type,1,1,1,REGISTER_TYPE,IMMEDIATE_TYPE,REGISTER_TYPE);
    }
    if(curr_opcode >= BNE_OP && curr_opcode <= BGT_OP){
      return validate_operand_type(line, op1_type, op2_type,op3_type,1,1,1,REGISTER_TYPE,REGISTER_TYPE,LABEL_TYPE);
    }
    
  }
  else if(curr_opcode >= MOVE_OP && curr_opcode <= MVLO_OP){
    /* 2 operands required */
		if (op_count != 2) {
			print_error(line, "Operation requires 2 operands, got %d", op_count);
			return FALSE;
		}
    return validate_operand_type(line, op1_type, op2_type,NONE_TYPE,1,1,0,REGISTER_TYPE,REGISTER_TYPE);
  }
  else if (curr_opcode >= JMP_OP && curr_opcode <= CALL_OP) {
		/* 1 operand required */
		if (op_count != 1) {
			print_error(line, "Operation requires 1 operand, got %d", op_count);
			return FALSE;
		}
    if(curr_opcode == JMP_OP){
      return validate_operand_type(line, op1_type, NONE_TYPE,NONE_TYPE,2,0,0,REGISTER_TYPE,LABEL_TYPE);
    }
    if(curr_opcode == LA_OP || curr_opcode == CALL_OP){
      return validate_operand_type(line, op1_type, NONE_TYPE,NONE_TYPE,1,0,0,LABEL_TYPE);
    }
  }
  else if (curr_opcode == STOP_OP) {
		/* no operands required */
		if (op_count != 0) {
			print_error(line, "Operation requires no operands, got %d", op_count);
			return FALSE;
    }
	}
  return TRUE;
}


static bool validate_operand_type(line_info line, operand_type op1_type, operand_type op2_type, operand_type op3_type,
		int op1_valid_type_count, int op2_valid_type_count,int op3_valid_type_count,...){
    
  int i;
	bool is_valid;
	va_list list;
  operand_type op1_valids[4], op2_valids[4], op3_valids[4];
	memset(op1_valids, NONE_TYPE, sizeof(op1_valids));
	memset(op2_valids, NONE_TYPE, sizeof(op2_valids));
	memset(op3_valids, NONE_TYPE, sizeof(op3_valids));

  va_start(list, op3_valid_type_count);
  /* get the variable args and put them in three arrays (op1_valids & op2_valids & op3_valids) */
	for (i = 0; i < op1_valid_type_count && i <= 3 ;i++)
		op1_valids[i] = va_arg(list, int);
	for (; op1_valid_type_count > 5; va_arg(list,int), op1_valid_type_count--); /* Go on with stack until got all (even above limitation of 4) */
	/* again for second operand by the count */
	for (i = 0; i < op2_valid_type_count && i <= 3 ;i++)
		op2_valids[i] = va_arg(list, int);
  	/* again for third operand by the count */
	for (i = 0; i < op3_valid_type_count && i <= 3 ;i++)
		op3_valids[i] = va_arg(list, int);

  va_end(list); /* got all the arguments */

  	/* make the validation itself: check if any of the operand types has match to any of the valid ones */
	is_valid = op1_valid_type_count == 0 && op1_type == NONE_TYPE;
	for (i = 0; i < op1_valid_type_count && !is_valid; i++) {
		if (op1_valids[i] == op1_type) {
			is_valid = TRUE;
		}
	}
	if (!is_valid) {
		print_error(line, "Invalid operand type for first operand.");
		return FALSE;
	}
	/* same for second operand*/
	is_valid = op2_valid_type_count == 0 && op2_type == NONE_TYPE;
	for (i = 0; i < op2_valid_type_count && !is_valid; i++) {
		if (op2_valids[i] == op2_type) {
			is_valid = TRUE;
		}
	}
	if (!is_valid) {
		print_error(line, "Invalid operand type for second operand.");
		return FALSE;
	}

  	/* same for thirs operand */
	is_valid = op3_valid_type_count == 0 && op3_type == NONE_TYPE;
	for (i = 0; i < op3_valid_type_count && !is_valid; i++) {
		if (op3_valids[i] == op3_type) {
			is_valid = TRUE;
		}
	}
	if (!is_valid) {
		print_error(line, "Invalid operand type for third operand.");
		return FALSE;
	}
	return TRUE;

}