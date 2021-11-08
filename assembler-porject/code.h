/* Helper functions to process and analyze code */
#ifndef _CODE_H
#define _CODE_H
#include "table.h"
#include "globals.h"



/**
 * Get's the opcode and the funct of a command by it's name
 * @param cmd The command name 
 * @param opcode_des The opcode value destination
 * @param funct_des The funct value destination
 */
void get_opcode_and_funct(char* cmd, opcode* opcode_des, funct* funct_des);

/**
 * Returns the register value by it's name
 * @param name The name of the register
 * @return The value of the register if found. otherwise, returns NONE_REG
 */
int get_register_by_name(char* name);

/**
 * Separates the operands from a certain index, puts each operand into the destination array,
 * and puts the found operand count in operand count argument
 * @param line The current source line info
 * @param i The index to start analyzing from
 * @param destination At least a 3-cell buffer of strings for the extracted operand strings
 * @param operand_count The destination of the detected operands count
 * @param command The current command string
 * @return Whether succeeded
 */
bool get_operands(line_info line, int i, char** destination, int* operand_count, char* command);

/**
 * Validates and Builds a code word by the opcode, funct, operand count and operand strings
 * @param line The current source line info
 * @param curr_opcode The current opcode
 * @param curr_funct The current funct
 * @param op_count The operands count
 * @param operands a 3-cell array of pointers to the operands.
 * @param tab The symbol table
 * @return A pointer to code word struct, which represents the code. if validation fails, returns NULL.
 */
code_word* build_code_word(line_info line, opcode curr_opcode, funct curr_funct, int op_count, char* operands[3], table* tab);

/**
 * Returns the type of an operand
 * @param operand The operand's string
 * @return The type of the operand. otherwise, returns NONE_TYPE
 */
operand_type get_operand_type(char* operand);


#endif