/* Implements a basic table ("dictionary") data structure. sorted by value, ascending. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "table.h"
#include "utils.h"

void add_value_to_type(table tab, long to_add, symbol_type type) {
	table curr_entry;
	/* for each entry, add value to_add if same type */
	for (curr_entry = tab; curr_entry != NULL; curr_entry = curr_entry->next) {
		if (curr_entry->type == type) {
			curr_entry->value += to_add;
		}
	}
}

table_entry* find_by_types(table tab,char* key, int symbol_count, ...){
  int i;
	symbol_type* valid_symbol_types = malloc_with_check((symbol_count) * sizeof(int));
  /* build a list of the valid types */
	va_list arglist;
	va_start(arglist, symbol_count);
	for (i = 0; i < symbol_count; i++) {
		valid_symbol_types[i] = va_arg(arglist, symbol_type);
	}
  	va_end(arglist);
	/* if table null, nothing to do */
	if (tab == NULL) {
		free(valid_symbol_types);
		return NULL;
	}
  /* iterate over table and then over array of valid. if type is valid and same key, return the entry. */
	do {
		for (i = 0; i < symbol_count; i++) {
			if (valid_symbol_types[i] == tab->type && strcmp(key, tab->key) == 0) {
				free(valid_symbol_types);
				return tab;
			}
		}
	} while ((tab = tab->next) != NULL);
	/* not found, return NULL */
	free(valid_symbol_types);
	return NULL;
}

void add_table_item(table* tab, char* key, long value, symbol_type type){
  char* temp_key;
	table prev_entry, curr_entry, new_entry;
	/* allocate memory for new entry */
	new_entry = (table) malloc_with_check(sizeof(table_entry));

  /* prevent "Aliasing" of pointers. when free the list, also free these allocated char ptrs*/
  temp_key = (char *) malloc_with_check(strlen(key) + 1);
	strcpy(temp_key, key);
	new_entry->key = temp_key;
	new_entry->value = value;
	new_entry->type = type;

  /* if the table's null, set the new entry as the head. */
	if ((*tab) == NULL || (*tab)->value > value) {
		new_entry->next = (*tab);
		(*tab) = new_entry;
		return;
	}

  /* insert the new table entry */
	curr_entry = (*tab)->next;
	prev_entry = *tab;
	while (curr_entry != NULL && curr_entry->value < value) {
		prev_entry = curr_entry;
		curr_entry = curr_entry->next;
	}
  new_entry->next = curr_entry;
	prev_entry->next = new_entry;
}

long find_by_name(table tab,char* key){
  /* iterate over table and check if the label is exteranl or not, and then return the adress. */
  while( tab != NULL ){
    if(strcmp(key, tab->key) == 0){
      if(tab->type != EXTERNAL_SYMBOL){
        return tab->value;
      }
    }
    tab = tab->next;
  }
  return 0;
}

table filter_table_by_type(table tab, symbol_type type){
  table new_table = NULL;
  /* for each entry, check if has the type. if so, insert to the new table. */
  while (tab != NULL){
    if(tab->type == type){
      add_table_item(&new_table, tab->key, tab->value, tab->type);
    }
    tab = tab->next;
  }
  return new_table; /* it holds a pointer to the first entry, dynamically-allocated */
}

void free_table(table tab) {
	table prev_entry, curr_entry = tab;
	
	while (curr_entry != NULL) {
		prev_entry = curr_entry;
		curr_entry = curr_entry->next;
		free(prev_entry->key); 
		free(prev_entry);
	}
}