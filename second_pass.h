
#include "first_pass.h"

#define MEMORY_START_ADDRESS 100
#define MEMORY_MAX_ADDRESS 1023
#define BINARY_LENGTH 12
#define ADDRESS_TYPES 13
#define ERROR -1
#define OPCODE_INDEX 3
#define DEST_INDEX 7
#define A_R_E_INDEX 10
#define SECOND_REG_INDEX 5
#define REG_NUM_STR_INDEX 2
#define REG_BINARY_DIGITS 5

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

int second_pass(label_object **symbol_table[], int* st_size , line_data **ld_arr[], int *ic, int *dc , int lines_count , char* file_name);
binary_table_p new_binary_table(int lines_count , int start_address);
void intToTwosComplement(int num, int* binary_line , int method);
void free_binary_table(binary_table_p b1 , int lines_count);
void entries_and_externals_file(label_object **symbol_table[], int* st_size , char* file_name);
int inst_coding_type(line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index , label_object **symbol_table[], int* st_size);
bool delete_null_file(char* path);
