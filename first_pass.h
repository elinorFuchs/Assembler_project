//
// Created by elifu on 02/08/2023.
//

#ifndef ASSEMBLER_OPENU_PROJECT_FIRST_PASS_H
#define ASSEMBLER_OPENU_PROJECT_FIRST_PASS_H
#include <string.h>

enum label_type{
    external,
    relocatable
};

typedef enum data_or_code{
    data,code
}data_or_code;

typedef struct label_object{
    char label_name[31];
    data_or_code d_or_c;
    enum label_type type;
    int label_value;

}label_object;



/*functions declaration*/
label_object* search_label(char label_name[],label_object symbol_table[]);

void add_to_symbol_table (label_object* label,label_object* symbol_table);



/*
 * how to implement the images, and how to represent 12 bits in a "word"
struct line_data instruction_image[?];
struct line_data code_image[1];
*/

#endif //ASSEMBLER_OPENU_PROJECT_FIRST_PASS_H
