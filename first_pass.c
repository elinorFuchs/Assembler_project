//
// Created by elifu on 02/08/2023.
//

#include "first_pass.h"

/*fgets from am file->create line_data for each line and put in ld struct array->create symbol table -> pass to second pass */


bool first_pass (FILE* am) {/*add later the symbol table and ld arr and ld as argument so secont pass can use them too*/
    char line[MAX_LINE_SIZE];
    int i = 0;
    int* ic, *dc;
    label_object* label;
    label_object* symbol_table[50];

    line_data* ld;
    line_data* ld_arr;
    ld_arr = (line_data *) malloc(sizeof(line_data) * 10);


    while (fgets(line, MAX_LINE_SIZE, am)) {
        printf("\n%s\n", line);
        ld = create_line_data(line);
        ld_arr[i] = *ld;
        if (ld->is_instruction)
        i++;
    }
    create_symbol_table (ld_arr, symbol_table, ic, dc);

}



bool create_symbol_table (line_data* ld_arr, label_object* symbol_table[], int* ic, int* dc) {

    /*loop through ld array:*/
    int i;
    label_object* new_label = {0};
    new_label = (label_object*) safe_malloc(sizeof (label_object));
    for (i = 0; i < 50; i++) /*chang to ld arr size*/{

        if (ld_arr[i].label_name != '\0') {/*there is a label definition in the line*/

            if (search_label((ld_arr[i].label_name), symbol_table)) {/*check if the label not already in the lable table*/
                /*label isn't already exist - definition is valid. add to table*/


                if (ld_arr[i].is_instruction) {
                    strcpy(new_label->label_name, ld_arr[i].label_name);
                    new_label->is_code = true;
                    new_label->type = relocatable;
                    new_label->label_value = *ic;
                    *ic += ld_arr[i].inst->inst_line_keeper;

                    add_to_symbol_table(new_label, symbol_table);
                }
                else if(ld_arr[i].is_direction) {/*direction line*/
                    strcpy(new_label->label_name, ld_arr[i].label_name);
                    new_label->is_data = true;
                    new_label->type = relocatable;
                    new_label->label_value = *dc;
                    *dc += ld_arr[i].dir->dir_line_keeper;

                    add_to_symbol_table(new_label, symbol_table);

                }
            }


        }
        /*not a label definition*/
        else{
            if (ld_arr[i].is_direction){
                if(ld_arr[i].dir->d_type == d_extern){
                    new_label->type = external;
                    new_label->label_name = ld_arr[i].dir->d_content->extern_ /*TO SOLVE - there can be few extern*/;
                    /*label value remine empty in purpose*/
                    add_to_extern_file
                    if (!(search_label((new_label->label_name), symbol_table))) {/*if already exist in the table */

                    }
                }

            }
        }


    }
}





void add_to_symbol_table (label_object* label,label_object* symbol_table){}

bool search_label(char* label_name, label_object* symbol_table[]){

    int i;
    /*label_object* which_label;*/
    for(i = 0; i < 50; i++){
        if(strcmp(symbol_table[i]->label_name,label_name) == 0){
            /*strcpy(which_label->label_name, symbol_table[i]->label_name);
            break;*/
            if(symbol_table[i]->type == external){
                printf("extern label can't be defined in the same file.");
                return false;}
            else if(symbol_table[i]->type == relocatable){
                printf("label already defined.");
                return false;
            }
        }
    }
    return true;/*the label define is valid- it's not already in the label table*/

}
