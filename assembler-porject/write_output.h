/* Output files related functions */
#ifndef _WRITEFILES_H
#define _WRITEFILES_H
#include "globals.h"
#include "table.h"

/**
 * Writes the output files of a single assembly file
 * @param code_img The code image
 * @param icf The final code counter
 * @param dcf The final data counter
 * @param filename The filename (without the extension)
 * @param symbol_table The symbol table
 * @param data The data image
 * @return Whether succeeded
 */
int write_output_files(machine_word** code_img, long icf, long dcf, char* filename, table symbol_table, data_word** data);


#endif