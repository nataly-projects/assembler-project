/* Contains general-purposed functions, for both passes and many usages */
#ifndef _UTILS_H
#define _UTILS_H

#include "globals.h"

/** moves the index to the next place in string where the char isn't white */
#define SKIP_TO_NOT_WHITE(string, index) \
        for (;string[(index)] && (string[(index)] == '\t' || string[(index)] == ' '); (++(index)))\
        ;

/**
 * Concatenates both string to a new allocated memory
 * @param str1 The first string
 * @param str2 The second string
 * @return A pointer to the new, allocated string
 */
char* strconcat(char* str1, char* str2);

/**
 * Allocates memory in the required size. Exits the program if failed.
 * @param size The size to allocate in bytes
 * @return A generic pointer to the allocated memory if succeeded
 */
void* malloc_with_check(long size);

/**
 * Finds the defined label in the code if exists, and saves it into the buffer.
 * @param line The source line to find in
 * @param symbol_dest The buffer for copying the found label in
 * @return Whether syntax error found
 */
bool find_label(line_info line, char* symbol_dest);

/**
 * Returns whether a label can be defined with the specified name.
 * @param name The label name
 * @return Whether the specified name is valid,
 */
bool is_valid_label_name(char* name);

/**
 * Check if a string is alphanumeric.
 * @param string The string
 * @return Whether it's alphanumeric
 */
bool is_alphanumeric(char* string);

/**
 * Returns FALSE if name is reserved word
 * @param name The string
 * @return Whether it's reserved word
 * */
bool is_reserved_word(char *name);

/**
 * Returns the instruction enum by the instruction's name, without the '.'
 * @param name The instruction name, without the '.'
 * @return The instruction enum if found, NONE_INST if not found
 */
instruction find_instruction_by_name(char* name);

/**
 * Returns whether the string is a integer
 * @param string The number in string
 * @return Whether is a valid integer
 */
bool is_int(char* string);

/**
 * Prints a detailed error message, including file name and line number by the specified message
 * @param message The error message
 * @param ... The arguments to format into the message
 * @return print result of the message
 */
int print_error(line_info line, char *message, ...);

/**
 * Frees all the dynamically-allocated memory for the code image.
 * @param code_image A pointer to the code images buffer
 * @param icf The final code counter value
 */
void free_code_image(machine_word** code_image, long icf);

/**
 * Frees all the dynamically-allocated memory for the data image.
 * @param data A pointer to the data images buffer
 * @param dcf The final data counter value
 */
void free_data_word(data_word** data, long dcf);

/**
 * Checks if the number is in the range for the specific data instruction 
 * @param num The number to check
 * @param ins The current instruction
 * @return Whether the number is in the range or not
 */
bool is_num_in_range(long num, instruction inst);

#endif 