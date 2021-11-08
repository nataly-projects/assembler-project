#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "utils.h"
#include "code.h"

 #define ERR_OUTPUT stdout 


char* strconcat(char* str1, char* str2){
  char* str = (char *)malloc_with_check(strlen(str1) + strlen(str2) + 1);
	strcpy(str, str1);
	strcat(str, str2);
	return str;
}

void* malloc_with_check(long size) {
	void *ptr = malloc(size);
	if (ptr == NULL) {
		printf("Error: Fatal: Memory allocation failed.\n");
		exit(1);
	}
	return ptr;
}

bool find_label(line_info line, char* symbol_dest) {
	int j, i;
	i = j = 0;

	/* skip white chars at the beginning anyway */
	SKIP_TO_NOT_WHITE(line.content, i)

	/* allocate some memory to the string needed to be returned */
	for ( ; line.content[i] && line.content[i] != ':' && line.content[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
		symbol_dest[j] = line.content[i];
	}
	symbol_dest[j] = '\0'; /* end of string */

	/* if it was a try to define label, print errors if needed. */
	if (line.content[i] == ':') {
		if (!is_valid_label_name(symbol_dest)) {
			print_error(line,"Invalid label name - cannot be longer than 32 chars, may only start with letter be alphanumeric.");
			symbol_dest[0] = '\0';
			return FALSE; /* no valid symbol, and no try to define one */
		}
		return TRUE;
	}
	symbol_dest[0] = '\0';
	return TRUE; /* there was no error */
}

bool is_valid_label_name(char* name) {

	/* check length, first char is alpha and all the others are alphanumeric, and not reserved word */
	return name[0] && strlen(name) <= MAX_LABEL_LENGTH && isalpha(name[0]) && is_alphanumeric(name + 1) && !is_reserved_word(name);
}

bool is_alphanumeric(char* string) {
	int i;
	/*check for every char in string if it is non alphanumeric char if it is function returns true*/
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])){
      return FALSE;
    } 
	}
	return TRUE;
}

bool is_reserved_word(char* name) {
	int func, opc;
	/* check if register or command or instruction */
	get_opcode_and_funct(name, &opc, (funct *) &func);
	if (opc != NONE_OP || get_register_by_name(name) != NONE_REG || find_instruction_by_name(name) != NONE_INST){
    return TRUE;
  } 
	return FALSE;
}

struct instruction_lookup_item {
	char* name;
	instruction value;
};

static struct instruction_lookup_item
		instructions_lookup_table[] = {
		{"asciz", ASCIZ_INST},
		{"dh",   DH_INST},
    {"dw",   DW_INST},
    {"db",   DB_INST},
		{"entry",  ENTRY_INST},
		{"extern", EXTERN_INST},
		{NULL, NONE_INST}
};

instruction find_instruction_by_name(char* name) {
	struct instruction_lookup_item* curr_item;

	for (curr_item = instructions_lookup_table; curr_item->name != NULL; curr_item++) {
		if (strcmp(curr_item->name, name) == 0) {
			return curr_item->value;
		}
	}
	return NONE_INST;
}

bool is_int(char* string) {
	int i = 0;
	if (string[0] == '-' || string[0] == '+') string++; /* if string starts with +/-, it's OK */
	for (; string[i]; i++) { /* just make sure that everything is a digit until the end */
		if (!isdigit(string[i])) {
			return FALSE;
		}
	}
	return i > 0; /* if i==0 then it was an empty string! */
}


bool is_num_in_range(long num, instruction inst){
	int bits = 0;
	long min, max;

	switch (inst){
		case DB_INST:
			bits = BYTE-1;
			break;

		case DH_INST:
			bits = 2*BYTE-1;
			break;

		case DW_INST:
			bits = 4*BYTE-1;
			break;

		default:
			break;
	}
	min = pow(2, bits) * (-1);
	max = pow(2,bits)-1;
	if(num > max || num < min){
		return FALSE;
	}
	return TRUE;
}

int print_error(line_info line, char* message, ...) {
	int result;
	va_list args; /* for formatting */
	/* print file+line */
	fprintf(ERR_OUTPUT,"Error In %s:%ld: ", line.file_name, line.line_number);

	/* use vprintf to call printf from variable argument function with message + format */
	va_start(args, message);
	result = vfprintf(ERR_OUTPUT, message, args);
	va_end(args);
	fprintf(ERR_OUTPUT, "\n");
	return result;
}

void free_code_image(machine_word** code_image, long icf){
  long i;
  /* for each not-null cell (we might have some "holes", so we won't stop on first null) */
  for(i = 0; i < icf; i++){
    machine_word* word = code_image[i];
    if(word != NULL){
      /* free code/data word */
			if (word->length > 0) {
				if(word->word.code->command == 'r'){
					free(word->word.code->commad_type.r);
				}
				else if(word->word.code->command == 'i'){
					free(word->word.code->commad_type.i);
				}
				else{
					free(word->word.code->commad_type.j);
				}
				free(word->word.code);
			} 
			else {
				free(word->word.data);
			}
			/* free the pointer to the union */
			free(word);
			code_image[i] = NULL;
    }
  }
	free(*code_image);
	*code_image = NULL;
}

void free_data_word(data_word** data, long dcf){
  long i;

  for(i = 0; i < dcf; ){
    data_word* curr_data = data[i];

		switch(curr_data->ins){
			case ASCIZ_INST:
			case DB_INST:
				i++;
				break;

			case DH_INST:
				i+=2;
				break;

			case DW_INST:
				i+=4;
				break;
			
			default:
				break;
		}
    free(curr_data);
	}

}
