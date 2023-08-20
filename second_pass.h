#ifndef ASSEMBLER_OPENU_PROJECT_SECOND_PASS_H
#define ASSEMBLER_OPENU_PROJECT_SECOND_PASS_H

#include "first_pass.h"

#define MEMORY_START_ADDRESS 100
#define MEMORY_MAX_ADDRESS 1023
#define BINARY_LENGTH 12

typedef struct binary_table* binary_table_p;

typedef enum coding_type{
    non_non,
    non_imm,
    non_label,
    non_reg,
    imm_imm,
    imm_label,
    imm_reg,
    label_imm,
    label_label,
    label_reg,
    reg_imm,
    reg_label,
    reg_reg
}coding_type;

int second_pass(label_object **symbol_table[], int* st_size , line_data **ld_arr[], int *ic, int *dc , int lines_count);
binary_table_p new_binary_table(int lines_count , int start_address);
void intToTwosComplement(int num, int* binary_line);
void free_binary_table(binary_table_p b1 , int lines_count);
void entries_and_externals_file(label_object **symbol_table[], int* st_size);
int inst_coding_type(line_data **ld_arr[] , int line_number);
void inst_binary_insert(coding_type type , int* binary_line , line_data **ld_arr[]);
#endif