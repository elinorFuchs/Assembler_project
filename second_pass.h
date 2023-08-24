
#include "first_pass.h"

#define MEMORY_START_ADDRESS 100
#define MEMORY_MAX_ADDRESS 1023
#define BINARY_LENGTH 12
#define BINARY_LAST_INDEX 11
#define RIGHT_BINARY_INDEX 6
#define LEFT_LAST_BINARY_INDEX 5
#define ADDRESS_TYPES 13
#define ERROR -1
#define OPCODE_INDEX 3
#define DEST_INDEX 7
#define A_R_E_INDEX 10
#define SECOND_REG_INDEX 5
#define REG_NUM_STR_INDEX 2
#define REG_BINARY_DIGITS 5
#define ENT_SUFFIX_LEN 5
#define EXT_SUFFIX_LEN 5
#define OB_SUFFIX_LEN 4
#define BINARY_TABLE_COUNT 2
#define BASE_64_DIGITS 64
#define OPCODES_COUNT 16
#define OPCODES_LEN 4
#define CODING_TYPES 4
#define CODING_TYPE_LEN 3
#define A_R_E_TYPES 3
#define A_R_E_DIGITS 2
#define REG_WORD_DIGITS 5
#define DIFF_CODING_OPTIONS 13
#define LINE_CODING_MODES 3
#define BINARY_DIGITS 2
#define SECOND_BINARY_LINE 2
#define THIRD_BINARY_LINE 3
#define MAX_ADDRESS_BIN_DIGITS_STR 11

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

int second_pass(label_object **symbol_table[], int* st_size , line_data **ld_arr[], int *ic, int *dc , int lines_count,\
                char* file_name);
binary_table_p new_binary_table(int lines_count , int start_address);
void intToTwosComplement(int num, int* binary_line , int method);
void free_binary_table(binary_table_p b1 , int lines_count);
void entries_and_externals_file(label_object **symbol_table[], int* st_size , char* file_name);
int inst_coding_type(line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index ,\
                     label_object **symbol_table[], int* st_size);
bool delete_null_file(char* path);
void binary_extra_lines_insert (line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index,\
                                label_object **symbol_table[], int* st_size , int s_type , int d_type , int code_type);
void binary_to_64(int** inst_binary_table , int** dir_binary_table , char* path , int inst_len , int dir_len);
void binary_file_title_insert(FILE* fptr , int inst_len , int dir_len);
void binary_line_convert(FILE* fptr , int time , int*** curr_table , int** inst_binary_table , int** dir_binary_table,\
                         int* curr_len , int inst_len , int dir_len);
void two_reg_binary_line(line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index ,\
                         int* reg_s_num , int* reg_d_num);
void labels_files_creation(char** first , char** second , char* file_name , FILE **ent_fptr , FILE **ext_fptr);
void binary_first_line_insert(line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index,\
                              int found_index , int* coding_type);
void direction_string_convert(line_data **ld_arr[] , int* char_as_ascii , int index ,\
                              binary_table_p direction_binary_table);
void direction_data_convert(line_data **ld_arr[] , int* data_count , int index, binary_table_p direction_binary_table);
void direction_label_dec_valid(label_object **symbol_table[], int* st_size , line_data **ld_arr[] , int index);
void binary_extra_lines_adjustment(line_data **ld_arr[] , int line_number , int extra_line_num , char **operand_str,\
                                   int *operand_type  , int s_type , int d_type);
void binary_label_line(label_object **symbol_table[] , int **binary_line , int binary_curr_index , int *label_val,\
                       char *operand_str , int* st_size , bool *is_extern_label);
void binary_opernd_handle(int **binary_line , label_object **symbol_table[] , int* imm , int operand_type,\
                          char *operand_str , int* first_str_index , int binary_curr_index , int* label_val,\
                          int* st_size, bool *is_extern_label, int* reg_num, int extra_line_num, int *reg_start_index);






