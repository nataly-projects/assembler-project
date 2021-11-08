#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "table.h"
#include "write_output.h"


/**
 * Writes a symbol table to a file. Each symbol and it's address in line, separated by a single space.
 * @param tab The symbol table
 * @param filename The filename without the extension
 * @param file_extension The extension of the file, including dot before
 * @return Whether succeeded
 */
static bool write_table_to_file(table tab, char* filename, char* file_extension);

/**
 * Writes the code and data image into an .ob file, with lengths on top
 * @param code_img The code image
 * @param icf The final code counter
 * @param dcf The final data counter
 * @param filename The filename, without the extension
 * @param data The data image
 * @return Whether succeeded
 */
static bool write_ob_file(machine_word** code_img, long icf, long dcf, char* filename, data_word** data);


/**
 * convert the value of a number to hexa value
 * @param num The number to convert
 * @param array The destination of the convert number
 * @param length The length of the number in bits
 * @param data_index The index to put in the array
 * @return 
 */ 
static void convert_to_hexa(int num, char* array, int length, int data_index);


int write_output_files(machine_word** code_img, long icf, long dcf, char* filename, table symbol_table, data_word** data){
  bool result;
	table externals = filter_table_by_type(symbol_table, EXTERNAL_REFERENCE);
	table entries = filter_table_by_type(symbol_table, ENTRY_SYMBOL);

  result = write_ob_file(code_img, icf, dcf, filename, data) && 
           write_table_to_file(externals, filename, ".ext") && 
					 write_table_to_file(entries, filename, ".ent");

  free_table(externals);
  free_table(entries);
  return result;
}

static bool write_table_to_file(table tab, char* filename, char* file_extension){
  FILE* file_desc;
	/* concatenate filename & extension, and open the file for writing */
	char* full_filename = strconcat(filename, file_extension);

  /* if table is null, nothing to write */
  if(tab == NULL){
    return TRUE;
  }
	
	file_desc = fopen(full_filename, "w");
	free(full_filename);

  /* if failed, print error and exit */
	if (file_desc == NULL) {
		printf("Can't create or rewrite to file %s\n", full_filename);
		return FALSE;
	}

  /* write first line without \n to avoid extraneous line breaks */
	fprintf(file_desc, "%s %.4ld", tab->key, tab->value);

	/* write the other lines to file */
  while ((tab = tab->next) != NULL) {
		fprintf(file_desc, "\n%s %.4ld", tab->key, tab->value);
	}
  fclose(file_desc);
	return TRUE;
}


static void convert_to_hexa(int num, char* array, int length, int data_index){
	int i;
	unsigned char lower; /* use binary 'and' to mask the lower byte */
	unsigned char upper; /* use binary 'and' to mask upper byte */
	unsigned char space = ' ';
	int index = 0; /* used for storing into array*/

	for(i=0; i<length; i+=8){ /* mask and compare 8 times*/
		int shift = num>>i; /* right shift*/
		lower = shift & 0xF; /* use binary 'and' to mask the lower byte */
		upper = (shift & 0xF0) >> 4; /* use binary 'and' to mask upper byte */
		if(lower >= 10){ /* if lower is in range [10-15], than add a value [0-5] on 'a'. */
			lower = 'A' + (lower - 10);
		}
		else{
			lower = lower + '0'; /* it's in range [0-9], so we have to add it to '0'. */
		}

		if(upper >= 10){ /* same as lower */
			upper = 'A' + (upper - 10);
		}
		else{
			upper = upper + '0';
		}
	
		if(data_index == -1){
			/* code - puts the hexas values in the corrcet index */
			array[index] = upper;
			array[index+1] = lower;
			array[index+2] = space;
			index += 3;
		}
		else{
			/* data - puts the hexas values in the corrcet index */
			array[data_index] = upper;
			array[data_index+1] = lower;
			array[data_index+2] = space;
			data_index += 3;
		}
	}
	if(index == -1){
		array[index] = '\0'; 
	}
	else{
		array[data_index] = '\0'; 
	}
    return;
}


 
static bool write_ob_file(machine_word** code_img, long icf, long dcf, char* filename, data_word** data){
  int i;
	FILE* file_desc;
	int index = 0;
	int val;
	char* hex_arr;
	char temp[12] = {0};
	char* output_filename = strconcat(filename, ".ob"); 	/* add extension of file to open */

	file_desc = fopen(output_filename, "w"); 	/* try to open the file for writing */
	free(output_filename);

  if(file_desc == NULL){
    printf("Can't create or rewrite to file %s.", output_filename);
		return FALSE;
  }

	hex_arr = (char *) malloc(sizeof(char)*dcf*3); 
	if(!hex_arr){
		printf("Error: Fatal: Memory allocation failed.\n");
		return FALSE; 
	}

  /* print data and code word count on top */
	fprintf(file_desc, "\t\t%ld %ld", icf - IC_INIT_VALUE, dcf);

	/* starting from index 0, not IC_INIT_VALUE as icf, so we have to subtract it. */
	for (i = 0; i < icf - IC_INIT_VALUE; i+=4) {
		if (code_img[i]->length > 0) {

			if(code_img[i]->word.code->opcode >= 0 && code_img[i]->word.code->opcode <= MVLO_OP){ /* R command */	
				val = (code_img[i]->word.code->opcode << 26 | code_img[i]->word.code->commad_type.r->rs << 21 | 
						 code_img[i]->word.code->commad_type.r->rt << 16 | code_img[i]->word.code->commad_type.r->rd << 11 |
						code_img[i]->word.code->commad_type.r->funct << 6 | code_img[i]->word.code->commad_type.r->NONE);
				convert_to_hexa(val, hex_arr, 32, -1);
				strncpy(temp, hex_arr, 11);
			}
			
			else if(code_img[i]->word.code->opcode >= ADDI_OP && code_img[i]->word.code->opcode<= SH_OP ){ /* I command */
				val = (code_img[i]->word.code->opcode << 26 | code_img[i]->word.code->commad_type.i->rs << 21 | 
						code_img[i]->word.code->commad_type.i->rt << 16 | code_img[i]->word.code->commad_type.i->immed);
				convert_to_hexa(val, hex_arr, 32, -1);
				strncpy(temp, hex_arr, 11);
			}
			else if(code_img[i]->word.code->opcode >= JMP_OP && code_img[i]->word.code->opcode <= STOP_OP){ /* J command */
				val = (code_img[i]->word.code->opcode << 26 | code_img[i]->word.code->commad_type.j->reg << 25 | 
						code_img[i]->word.code->commad_type.j->address);
				convert_to_hexa(val, hex_arr, 32, -1);
				strncpy(temp, hex_arr, 11);
			}
		}
	
		/* write the value to the file - first */
		fprintf(file_desc, "\n%.4d %s", i + 100, temp);
	}

	/* write data image. dcf starts at 0 so it's fine */
	for (i = 0; i < dcf; ) {

		switch(data[i]->ins){
			case ASCIZ_INST:
			case DB_INST:
			convert_to_hexa(data[i]->data, hex_arr, BYTE, index); 
			index += 3;
				i++;
				break;

			case DH_INST:
			convert_to_hexa(data[i]->data, hex_arr, 2*BYTE, index); 
			index += 6;
				i += 2;
				break;

			case DW_INST:
			convert_to_hexa(data[i]->data, hex_arr, 4*BYTE, index);
			index += 12;
				i += 4;
				break;

			default:
				break;
		}	
	}

	for(i = 0; i<strlen(hex_arr); i+=12){
		strncpy(temp, hex_arr+i, 11);
		fprintf(file_desc, "\n%.4ld %s", icf, temp);
		icf += 4;
	}

  /* close the file */
	fclose(file_desc);
	free(hex_arr);
	return TRUE;
}