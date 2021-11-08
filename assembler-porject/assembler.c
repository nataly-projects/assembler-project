#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "first_pass.h"
#include "table.h"
#include "utils.h"
#include "second_pass.h"
#include "write_output.h"


/**
 * Processes a single assembly source file, and returns the result status.
 * @param filename The filename
 * @return if succeeded
 */
static bool process_file(char* filename);

int main(int argc, char *argv[]){
  	int i;
		char* extension;

	/* to break line if needed */
	bool succeeded = TRUE;

	if(argc <= 1){
		printf("Missing input files. Please enter at least 1 assembler file.\n");
		return 0;
	}
	if(argc > MAX_FILES_TO_PROCESS+1){
		printf("The maximum files to process is %d.\n", MAX_FILES_TO_PROCESS);
		return 0;
	}

		
  /* process each file by arguments */
	for (i = 1; i < argc; ++i) {
		/* if last process failed and there's another file, break line: */
		if (!succeeded){
      puts(""); 
    }
		extension = strstr(argv[i], ".");
		if(extension == NULL || strcmp(extension, ".as") != 0){ /* the extension is not '.as' */
			printf("Error: cannot open the file with the %s extension. please enter file with .as extension\n", extension);
			return 0;
	} 
		/* foreach argument (file name), send it for full processing. */
		succeeded = process_file(argv[i]);
	
	}
	return 0;
}

static bool process_file(char* filename){
	/* memory address counters */
	int temp_c;
	long ic = IC_INIT_VALUE, dc = DC_INIT_VALUE, icf, dcf;

  bool is_success = TRUE; /* is succeeded so far */
  char* input_filename; 
  char temp_line[MAX_LINE_LENGTH + 2]; /* temporary string for storing line, read from file */
	FILE* file_des; /* current assembly file descriptor to process */
	data_word* data[CODE_ARR_IMG_LENGTH]; 
	machine_word* code_img[CODE_ARR_IMG_LENGTH];
	table symbol_table = NULL; /* our symbol table */
	line_info curr_line_info;

  /* remove the .as extension */
	input_filename = malloc_with_check(strlen(filename)); 
	strncpy(input_filename, filename, strlen(filename)-3);
	 /* get the file name without the extension */
	input_filename[strlen(filename)-3]='\0';

	/* open file, skip on failure */
	file_des = fopen(filename, "r");
	if (file_des == NULL) {
		/* if file couldn't be opened, print error. */
		printf("Error: cannot open the file: %s.\n", filename);
		free(input_filename); /* the only allocated space is for the full file name */
		return FALSE;
	}

  
	/* start first pass */
 	curr_line_info.file_name = input_filename; 
	curr_line_info.content = temp_line; /* use temp_line to read from the file, but it stays at same location. */

	/* read line - stop if read failed (when NULL returned) - usually when EOF. increase line counter for error printing. */
  for (curr_line_info.line_number = 1; fgets(temp_line, MAX_LINE_LENGTH + 2, file_des) != NULL; curr_line_info.line_number++){
          if (strchr(temp_line, '\n') == NULL && !feof(file_des)) {
            /* print message and prevent further line processing, as well as second pass.  */
            print_error(curr_line_info, "Line too long to process. Maximum line length should be %d.",MAX_LINE_LENGTH);
            is_success = FALSE;
            /* skip leftovers */
            do {
              temp_c = fgetc(file_des);
            } while (temp_c != '\n' && temp_c != EOF);
          }
          else {
            if (!process_line_fp(curr_line_info, &ic, &dc, code_img, &symbol_table, data)) {
              if (is_success) {
                icf = -1;
                is_success = FALSE;
							}
            }
		      }
       }

  /* save ICF & DCF */
	icf = ic;
	dcf = dc;

  /* if first pass success */
	if (is_success){
    ic = IC_INIT_VALUE;

    /* add IC to each DC for each of the data symbols in table */
    add_value_to_type(symbol_table, icf, DATA_SYMBOL);

    /*start second pass */
	  rewind(file_des); /* start from the beginning of file again */
    for (curr_line_info.line_number = 1; !feof(file_des); curr_line_info.line_number++) {
      int i = 0;
      fgets(temp_line, MAX_LINE_LENGTH, file_des); /* get line */
      SKIP_TO_NOT_WHITE(temp_line, i)
      if (code_img[ic - IC_INIT_VALUE] != NULL || temp_line[i] == '.'){
        is_success &= process_line_sp(curr_line_info, &ic, code_img, &symbol_table);
      }
	  }
    /* write output files if second pass succeeded */
		if (is_success) {
			is_success = write_output_files(code_img, icf, dcf, input_filename, symbol_table, data);
		}
  }

	fclose(file_des);
	/* free all the pointers: */
	free(input_filename);  /* free current file name */
	free_table(symbol_table); /* free symbol table */
	free_data_word(data, dcf); /* free data image */
	free_code_image(code_img, icf); /* free code image */
  return is_success;
}