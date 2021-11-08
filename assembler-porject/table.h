/* Implements a dynamically-allocated symbol table */
#ifndef _TABLE_H
#define _TABLE_H

/* A symbol type */
typedef enum symbol_type {
	CODE_SYMBOL,
	DATA_SYMBOL,
	EXTERNAL_SYMBOL,
	EXTERNAL_REFERENCE, 	/* address that contains a reference to the external symbol */
	ENTRY_SYMBOL
} symbol_type;

/* pointer to table entry */
typedef struct entry* table;

/* A single table entry */
typedef struct entry {
	table next; /* next entry in table */
	long value; /* address of the symbol */
	char *key; /* key - the symbol name */
	symbol_type type; /* the symbol type */
} table_entry;

/**
 * Adds the value of the entry
 * @param tab The table, containing the entries
 * @param to_add The value to add
 * @param type The type of symbols to add the value to
 */
void add_value_to_type(table tab, long to_add, symbol_type type);

/**
 * Find entry from the only specified types
 * @param tab The table
 * @param key The key of the entry to find
 * @param symbol_count The count of given types
 * @param ... The types to filter
 * @return The entry if found, NULL if not found
 */
table_entry* find_by_types(table tab, char* key, int symbol_count, ...);

/**
 * Adds an item to the table, keeping it sorted.
 * @param tab A pointer to the table
 * @param key The key of the entry to insert
 * @param value The value of the entry to insert
 * @param type The type of the entry to insert
 */
void add_table_item(table* tab, char* key, long value, symbol_type type);

/**
 * Find entry by the given name
 * @param tab The symbol table
 * @param key The key of the entry to search
 * @return The address of the key or 0 if it's external
 */
long find_by_name(table tab, char* key);

/**
 * Returns all the entries by their type in a new table
 * @param tab The table
 * @param type The type to look for
 * @return A new table, which contains the entries
 */
table filter_table_by_type(table tab, symbol_type type);

/**
 * Deallocates all the memory required by the table.
 * @param tab The table to deallocate
 */
void free_table(table tab);

#endif