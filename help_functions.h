//
// Created by elifu on 02/08/2023.
//

#ifndef ASSEMBLER_OPENU_PROJECT_HELP_FUNCTIONS_H
#define ASSEMBLER_OPENU_PROJECT_HELP_FUNCTIONS_H
#include "line_parser.h"

typedef enum {false, true} bool;

char* copy_word (const char* line, int* index);
void skip_spaces (int* index,const char* line);
int args_counter (char* inst_line);
int string_to_sign_int(char* data_line, int* index);
void skip_commas(int* index, const char* line);
bool is_commas_valid(char* args);
void* safe_malloc(size_t size);
void resize_arr(int** arr, int* size);
#endif //ASSEMBLER_OPENU_PROJECT_HELP_FUNCTIONS_H
