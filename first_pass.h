//
// Created by elifu on 02/08/2023.
//

#ifndef ASSEMBLER_OPENU_PROJECT_FIRST_PASS_H
#define ASSEMBLER_OPENU_PROJECT_FIRST_PASS_H
#include <string.h>
#include <stdio.h>
#include "line_parser.h"
#include "help_functions.h"

enum label_type{
    external,
    relocatable
};



typedef struct label_object{
    char label_name[31];
    bool is_data;
    bool is_code;
    enum label_type type;
    int label_value;
}label_object;



/*functions declaration*/
bool search_label(char* label_name, label_object* symbol_table[]);
bool first_pass (FILE* am);
void add_to_symbol_table (label_object* label,label_object* symbol_table);
bool create_symbol_table (line_data* ld_arr, label_object* symbol_table[], int* ic, int* dc);
/*
 * how to implement the images, and how to represent 12 bits in a "word"
struct line_data instruction_image[?];
struct line_data code_image[1];
*/

#endif //ASSEMBLER_OPENU_PROJECT_FIRST_PASS_H
