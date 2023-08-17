/*Created by elifu on 02/08/2023.*/

#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H
#include <stdlib.h>
#include <stdio.h>
#include "line_parser.h"

typedef enum {false, true} bool;

extern char* inst_Arr[16];


char* copy_word (const char* line, int* index);
void skip_spaces (int* index,const char* line);
int args_counter (char* inst_line);
int string_to_sign_int(char* data_line, int* index);
void skip_commas(int* index, const char* line);
void* safe_malloc(size_t size);
void safe_free(void **ptr);

void* safe_calloc(size_t num_elements, size_t element_size);
void resize_int_arr(int** arr, int* size);
#endif /*ASSEMBLER_OPENU_PROJECT_HELP_FUNCTIONS_H*/
