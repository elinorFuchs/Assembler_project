#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H
#include "line_parser.h"


#define MAX_GET_LEN 100
#define MAX_LINE_LEN 82
#define MAX_MCRO_NAME 30
#define MAX_MACROS_NUM 50
#define DIFF_INSTRUCTION_COUNT 15
#define MCRO_LEN 4
#define ENDMCRO_LEN 7
#define END_LINE_CHARS 2
#define START_MCRO_TABLE_LINES_COUNT 4


typedef struct mcro_table* mcro_table_p;

/** Creates .am file after macro breakdown, and after adjustment to the template of 'line_template'. It is assumed
    that a call to a macro can only be in line that contains nothing except the macro name. */
int pre_as(char* path);
/** If one of the files is NULL, it prints error message, closes both, free the pointer to the name of second file
    and returns true. */
bool is_null_file(FILE *f1 , FILE *f2 , char **first_file_name , char **second_file_name);
/* Returns line in format: no white chars in the beginning, between every two words exactly one regular space. */
char* line_template(char* line);
/* Gets two strings with same size of memory allocated and copies the second content to the first. */
void str_copy(char **copy , char **original , bool start_no_spaces);
void string_copy(char *first , char *copy);
/*Given an optional macro declaration line, it ensures that it is new, valid, and has a legal name.*/
void mcro_dec_line(int* index, int* index_copy, bool* mcro_flag, char** curr_line_index, char* curr_line, 
                   char** mcro_name, mcro_table_p m1, char** line_copy, int line_number);
/* Increases index to the index of the first non white char in the line (till \n) */
void clean_white(char* line , int* index);
/* Method 1: list of strings to free, Method 2: list of file pointers to close. */
void free_or_close(int method , int count, ...);
/*Returns the index of the macro in the macro table or '-1' if not exist.*/
int is_exist_mcro (mcro_table_p m1 , char *mcro_name_to_check);
/*Method 1 to insert macro name and method 2 to insert macro content.*/
void insert_mcro(mcro_table_p m1 , char *mcro_name_to_check , char *line , int method);
/* Copies the first string to the second except one letter change. */
void strings_letter_change(int letter_index , char new_letter , char* first , char **after_change);
/*Checks whether the char is space or one of those: \t\f\v\r */
int is_white(char c);
/*Replaces valid macro call line with the content of the macro or with NULL if not valid*/
void is_mcro_line(mcro_table_p m1 , char* line , char **inside);
/*Ensure that it is not instruction name.*/
bool valid_mcro_name(char* mcro_name);
void mcro_table_line_increase(mcro_table_p m1);
void free_mcro_table(mcro_table_p m1);
mcro_table_p create_mcro_table();
int is_all_white(char* line);

#endif
