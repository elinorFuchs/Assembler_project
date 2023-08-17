#include "first_pass.h"

#define MEMORY_START_ADDRESS 100

#define MEMORY_MAX_ADDRESS 1023

#define BINARY_LENGTH 12



typedef struct binary_table* binary_table_p;



int second_pass(label_object *symbol_table[], line_data *ld_arr[], int *ic, int *dc);

binary_table_p new_binary_table(int lines_count , int start_address);

void intToTwosComplement(int num, int* binary_line);

void free_binary_table(binary_table_p b1 , int lines_count);

