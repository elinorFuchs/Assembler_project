/*Created by elifu on 02/08/2023.*/

#ifndef ASSEMBLER_OPENU_PROJECT_LINE_PARSER_H
#define ASSEMBLER_OPENU_PROJECT_LINE_PARSER_H

#include "help_functions.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DIRECTION_NUM 4
#define MAX_LABEL_SIZE 31
#define MAX_LINE_SIZE 82
#define VALID_ADDRSS_MTHDS 3
#define REGISTERS_NUM 8
#define INST_SIZE 16

/*Instruction Line Data Structures:*/

typedef enum opcode_names {
    mov,
    cmp,
    add,
    sub,
    not,
    clr,
    lea,
    inc,
    dec,
    jmp,
    bne,
    red,
    prn,
    jsr,
    rts,
    stop,
    opcode_length,
    invalid
} opcode;

typedef enum {SUCCESS, INVALID_FIRST_WORD,UNDEFINED_DATA_NAME, UNNECESSARY_LABEL,OPCODE_LABEL_NAME ,INVALID_ARGS_AMOUNT
              ,INVALID_ARGS_ADD_METHOD, INVALID_INST_ARGS,COMMA_AT_END_OF_D_WORD,MISSING_COMMA,ILLEGAL_COMMA,MULT_COMMAS,
              EXTERN_DEFINED,CHAR_AFTER_QUOTE,NO_CONTENT_DIRECTION, DATA_RANGE,LONG_LABEL,LABEL_FIRST_CHAR,INVALID_LABEL_NAME,IMMEDIATE_RANGE
              }error_index;/*all errors option */


typedef enum address_type{
    immediate = 1,
    label = 3,
    reg = 5,
    none = 0
}address_type;

typedef struct{
    opcode code;
    address_type src[VALID_ADDRSS_MTHDS];
    address_type dest[VALID_ADDRSS_MTHDS];
}op_args_mthd;

extern op_args_mthd op_args_arr [16];

union arg_name{
    char* label_name;
    char* rgister;
    int immediate;
};

typedef struct instruction {
    op_args_mthd* op_args_type;
    char* src_name;
    char* dest_name;
    int inst_line_keeper;
}instruction;

/*DIRECTIONS Line Data Structures:*/

typedef enum direction_type {
    d_string,
    d_data,
    d_entry,
    d_extern,
    invalid_data
}direction_type;

typedef struct data_arr { 
    int *data_arr;/*if it's a data*/
    size_t data_arr_size;
}data_arr;

typedef struct string_data { 
    char* string; /*if it's a string*/
    int str_len;
}str_d;

typedef struct {
    char** entry;
    int en_size;
}entry_arr;

typedef struct {
    char** extern_;
    int ex_size;
}extern_arr;

typedef union direction_content{
    data_arr* d_arr;
    str_d* string;
    entry_arr* en_arr;
    extern_arr* ex_arr;
}direction_content;

typedef struct direction {
    direction_type d_type;
    direction_content* d_content;
    int dir_line_keeper;
}direction;

/*LINE DATA STRUCTURE:*/

typedef struct line_data{

    char label_name[31];
    int labal_value;
    error_index ei;
    direction* dir;
    instruction* inst;
    bool is_label_def;
    bool is_instruction;
    bool is_direction;


}line_data;


/*function declarations*/
/*get a line string, validate the line syntax and return a structure that hold the line information for the second pass */
line_data* create_line_data(char *line);

bool is_direction (char* word, line_data* ld);
bool is_args_as_expected(op_args_mthd* op_args_to_validate);
bool is_valid_string(char *string_line, line_data *ld);
bool is_label_def(char *word, line_data *ld);
bool is_instruction(char* word);
direction_type which_data_type(char* word);
opcode which_instruction(char* word);
bool is_A_group(opcode code);
bool is_B_group(opcode code);
bool is_C_group(opcode code);
char* copy_string(char* line,int* index);
bool is_more_args(char *line, int *index);
bool string_parser(char* line, line_data* ld, int* index);
char *copy_s_args(char *string_line);
bool data_parser(char* temp_line, line_data* ld, int* index);
void copy_d_args(char* data_line, line_data* ld);
bool is_valid_data(char *data_line, line_data *ld);
bool inst_args_parser(char *temp_line, opcode code, int *index, line_data *ld);
bool a_count_as_expected(opcode op, int args_c);
bool set_op_args(char* data_args, line_data* ld);
void set_src_add (char* arg, line_data* ld);
void set_dest_add (char* args, line_data* ld);
bool is_immediate(char* arg,line_data* ld);
bool is_label(char* arg,line_data* ld);
bool is_register(char* arg, line_data* ld);
bool is_inst_arg_valid(char *argument, line_data *ld);
void set_extern_labels(line_data * ld, char* args);
void set_entry_labels(line_data * ld, char* args);
bool is_commas_valid(char* args, line_data* ld);

#endif /*ASSEMBLER_OPENU_PROJECT_LINE_PARSER_H*/
