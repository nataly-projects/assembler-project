/* Helper functions for instruction analyzing */
#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H
#include "globals.h"

/**
 * Returns the first instruction detected from the index in the string.
 * @param line The current source line info
 * @param index The index to start looking from.
 * @return instruction_type indicates the detected instruction.
 */
instruction find_instruction_from_index(line_info line, int* index);

/**
 * Processes a .asciz instruction from index of source line.
 * @param line The current source line info
 * @param index The index
 * @param dc The current data counter
 * @param data The data image struct
 * @return Whether succeeded
 */
bool process_asciz_instruction(line_info line, int index, long* dc, data_word** data);

/**
 * Processes a data instructions: .db, .dh, .dw from index of source line.
 * copies each number value to data image by dc position
 * @param line The current source line info
 * @param index The index
 * @param dc The current data counter
 * @param inst The instruction
 * @param data The data image
 * @return Whether succeeded
 */
bool process_data_instruction(line_info line, int index, long* dc, instruction inst, data_word** data);

#endif