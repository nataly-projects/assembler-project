/* Second pass line processing functions */
#ifndef _SECOND_PASS_H
#define _SECOND_PASS_H

#include "globals.h"
#include "table.h"

/**
 * Processes a single source line in the second pass
 * @param line The current source line info
 * @param ic A pointer to the current code counter
 * @param code_img The code image
 * @param symbol_table The symbol table
 * @return Whether succeeded
 */
bool process_line_sp(line_info line, long* ic, machine_word** code_img, table* symbol_table);




#endif