#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


instruction find_instruction_from_index(line_info line, int* index){
  char temp[MAX_LINE_LENGTH];
	int j;
	instruction result;
  SKIP_TO_NOT_WHITE(line.content, *index) /* get index to first not white place */

	if (line.content[*index] != '.'){
    return NONE_INST;
  }
  for (j = 0; line.content[*index] && line.content[*index] != '\t' && line.content[*index] != ' '; (*index)++, j++) {
		temp[j] = line.content[*index];
	}
  temp[j] = '\0'; /* end of string */

  /* check if valid instruction, if not return error */
  if ((result = find_instruction_by_name(temp+1)) != NONE_INST){
    return result;
  }
  print_error(line, "Invalid instruction name: %s", temp);
	return ERROR_INST; /* starts with '.' but not a valid instruction! */
}

bool process_asciz_instruction(line_info line, int index, long* dc, data_word** data){
  char temp_str[MAX_LINE_LENGTH];
	char* last_quote_location = strrchr(line.content, '"');
	SKIP_TO_NOT_WHITE(line.content, index)
  if (line.content[index] != '"') {
		print_error(line, "Missing opening quote of string");
		return FALSE;
  }
  else if (&line.content[index] == last_quote_location) { /* last quote is same as first quote */
		print_error(line, "Missing closing quote of string");
		return FALSE;
  }
  else{
    int i;
		/* copy the string including quotes and everything until end of line */
		for (i = 0;line.content[index] && line.content[index] != '\n' && line.content[index] != EOF; index++,i++) {
				temp_str[i] = line.content[index];
		}
    /* put string terminator instead of last quote */
		temp_str[last_quote_location - line.content] = '\0';

    for(i = 1; temp_str[i] && temp_str[i] != '"'; i++) {
      data[*dc] = (data_word*) malloc_with_check(sizeof(data_word));
      data[*dc]->ins = ASCIZ_INST;
      data[*dc]->data = temp_str[i];
			(*dc)++;
		}

    data[*dc] = (data_word*) malloc_with_check(sizeof(data_word));
    data[*dc]->ins = ASCIZ_INST;
    /* put string terminator */
    data[*dc]->data = '\0';
		(*dc)++;
  }
  return TRUE;
}

bool process_data_instruction(line_info line, int index, long* dc, instruction inst, data_word** data){
  char temp[80], *temp_ptr;
	long value;
	int i;
	SKIP_TO_NOT_WHITE(line.content, index)
  if (line.content[index] == ',') {
		print_error(line, "Unexpected comma after data instruction");
    return FALSE;
	}
  do{
    for (i = 0; line.content[index] && line.content[index] != EOF && line.content[index] != '\t' &&
		     line.content[index] != ' ' && line.content[index] != ',' && line.content[index] != '\n'; index++, i++) {
			      temp[i] = line.content[index];
		}
    temp[i] = '\0'; /* end of string */

    if (!is_int(temp)) {
			print_error(line, "Expected integer for .data instruction, got '%s'", temp);
			return FALSE;
		}
   
    /* write to data buffer */
		value = strtol(temp, &temp_ptr, 10);
    if(!is_num_in_range(value, inst)){
      print_error(line, "The value is out of range for this instruction");
      return FALSE;
    }
    data[*dc] = (data_word*) malloc_with_check(sizeof(data_word));
    data[*dc]->ins = inst;
    data[*dc]->data = value;

    if(inst == DB_INST){
      (*dc)++; /* a word was written right now */
    }
    else if(inst == DW_INST){
      (*dc)+=4;
    }
    else{ /* it's .dh instruction */
      (*dc)+=2;
    }

    SKIP_TO_NOT_WHITE(line.content, index)
    if (line.content[index] == ','){
      index++;
    }
    else if (!line.content[index] || line.content[index] == '\n' || line.content[index] == EOF){
      break;  /* end of line/file/string - nothing to process anymore */
    }
    else{
      print_error(line, "Missing comma.");
      return FALSE;
    }
    /* got comma. skip white chars and check if end of line (if so, there's extraneous comma) */
		SKIP_TO_NOT_WHITE(line.content, index)
    if (line.content[index] == ',') {
			print_error(line, "Multiple consecutive commas.");
			return FALSE;
		}
    else if (line.content[index] == EOF || line.content[index] == '\n' || !line.content[index]){
      print_error(line, "Missing data after comma");
			return FALSE;
    }
  } while(line.content[index] != '\n' && line.content[index] != EOF);

  return TRUE;
}