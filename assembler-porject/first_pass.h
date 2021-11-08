/* Processes a code line in first pass */
#ifndef _FIRST_PASS_H
#define _FIRST_PASS_H

#include "globals.h"
#include "table.h"

/**
 * Processes a single line in the first pass
 * @param line The current source line info
 * @param IC A pointer to the current code counter
 * @param DC A pointer to the current data counter
 * @param code_img The code image array
 * @param symbol_table The data symbol table
 * @param data The data image array
 * @return Whether succeeded.
 */
bool process_line_fp(line_info line, long* IC, long* DC, machine_word** code_img, table* symbol_table, data_word** data);

#endif