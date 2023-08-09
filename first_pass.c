/*Created by elifu on 02/08/2023.*/

#include "first_pass.h"


/*fgets from am file->create line_data for each line and put in ld struct array->create symbol table -> pass to second pass */

char* errors[] = {"", "Error: Invalid first word.\n","Error: Invalid data name.\n",
                  "Warning: Unnecessary label definition before .entry or .extern.\n",
                  "Error: Label name can't be an instruction opcode.\n",
                  "Error: Arguments amount is not valid.\n",
                  "Error: Arguments address method is not valid.\n",
                  "Error: Instruction argument is not valid.\n",
                  "Error: Illegal comma at the end of the direction word.\n",
                  "Error: Missing comma.\n",
                  "Error: Illegal comma.\n",
                  "Error: Multiple consecutive commas.\n",
                  "Error: extern label can't be defined in the same file.\n",
                  "Error: invalid char after closing quote.\n",
                  "Error: Direction line without content.\n"
};
bool first_pass (FILE* am, label_object* symbol_table, line_data* ld_arr, int* ic, int* dc) {/*add later the symbol table and ld arr and ld as pointer argument so second pass can use them too*/
    char line[MAX_LINE_SIZE];
    int i = 0;
    line_data* ld;
    int ld_arr_size = INITIAL_SIZE;/*CHECK*/

    ld_arr = (line_data*)safe_malloc(ld_arr_size * sizeof(line_data*));

    while (fgets(line, MAX_LINE_SIZE, am)) {/*put each line as a struct in the line_data array*/
        printf("%s\n", line);
        ld = create_line_data(line);
        ld_arr[i] = *ld;
        printf("%s\n",errors[ld->ei]);
        if (i >= ld_arr_size) /*reallocate memory*/
            resize_ld_arr(&ld_arr, &ld_arr_size);
        i++;
    }
    create_symbol_table (ld_arr,ld_arr_size, symbol_table, ic, dc);/*creating the symbol table of the file*/
}

bool create_symbol_table (line_data* ld_arr, int arr_size, label_object* symbol_table, int* ic, int* dc) {
    /*loop through ld array:*/
    int i;
    label_object* new_label = {0};
    new_label = (label_object*) safe_malloc(sizeof (label_object));
    for (i = 0; i < arr_size; i++){
        if (ld_arr[i].is_label_def) {/*there is a label definition in the line*/
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
            if(ld_arr[i].is_direction) {
                if (ld_arr[i].dir->d_type == d_extern) {
                    int j, ex_arr_size =ld_arr[i].dir->d_content->ex_arr->ex_size;
                    for (j = 0; j < ex_arr_size; ++j) {
                        new_label->type = external;
                        new_label->is_extern = true;
                        strcpy(new_label->label_name,ld_arr[i].dir->d_content->ex_arr->extern_[j]);
                        /*label value remain empty*/
                        add_to_symbol_table(new_label, symbol_table);
                    }
                }
            }
            if(ld_arr[i].is_instruction)
                *ic += ld_arr[i].inst->inst_line_keeper;


        }
    }
}


void add_to_symbol_table (label_object* label,label_object* symbol_table){}

bool search_label(char* label_name, label_object* symbol_table){
    int i;
    /*label_object* which_label;*/
    for(i = 0; i < 50; i++){
        if(strcmp(symbol_table[i].label_name,label_name) == 0){
            /*strcpy(which_label->label_name, symbol_table[i]->label_name);
            break;*/
            if(symbol_table[i].type == external){
                printf("Error: extern label can't be defined in the same file.\n");
                return false;
            }
            else if(symbol_table[i].type == relocatable){
                printf("Error: label already defined.\n");
                return false;
            }
        }
    }
    return true;/*the label define is valid - it's not already in the label table*/
}

void resize_ld_arr(line_data ** arr, int* size) {
    *size *= 2;
    line_data* temp_arr = (line_data *)realloc(*arr, (*size) * sizeof(line_data));
    if (temp_arr == NULL) {
        printf("Memory reallocation failed.\n");
        free(*arr);
        exit(1);
    }
    *arr = temp_arr;
}
