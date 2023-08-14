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
                  "Error: Direction line without content.\n",
                  "Error: .data argument value is not in range, should be -2048 to 2047.\n",
                  "Error: label name is longer then 31 characters.",
                  "Error: Label first character should be A-Z or a-z character.\n",
                  "Error: Invalid label name (characters should be digits or alphabets.\n",
                  "Error: Immediate number range should be -512 to 511.\n"
};
bool first_pass (FILE* am, label_object* symbol_table[], line_data* ld_arr[], int* ic, int* dc) {/*add later the symbol table and ld arr and ld as pointer argument so second pass can use them too*/
    char line[MAX_LINE_SIZE];
    int i = 0;
    int line_num = 1;
    line_data* ld;
    int ld_arr_size = INITIAL_SIZE;

    ld_arr = (line_data**)safe_malloc(ld_arr_size * sizeof(line_data*));


    while (fgets(line, MAX_LINE_SIZE, am)) {/*put each line as a struct in the line_data array*/
        ld = create_line_data(line);
        /*printf("%s\n", line);*/
        ld_arr[i] = ld;
        if(ld->ei != SUCCESS) {
            printf("In line %d:\n",line_num);
            printf("%s", line);
            printf("%s\n", errors[ld->ei]);
        }
        line_num++;
        /*if (i >= ld_arr_size) /*reallocate memory
            resize_ld_arr(ld_arr, &ld_arr_size);*/
        i++;
    }/*IF NEEDED ADD A RESIZING OG LD ARR TO THE CORRECT SIZE*/
    /*int j;
    for (j = 0; j <300 ; ++j) {
        printf("%s\n", ld_arr[j].label_name);

    }*/
    create_symbol_table (ld_arr,ld_arr_size, symbol_table, ic, dc);/*creating the symbol table of the file*/
    int k;
    printf("label name in symbol table is: \n");
    for (k = 0; k < 24; ++k) {
        printf( "%s\n", symbol_table[k]->label_name);

    }
    return true;
}

bool create_symbol_table (line_data* ld_arr[], int arr_size, label_object* symbol_table[], int* ic, int* dc) {
    /*loop through ld array:*/
    int i;
    label_object* new_label = NULL;
    int s_table_size = 0;
        for (i = 0; i < 24; i++) {

            new_label = (label_object*) safe_malloc(sizeof (label_object));

            *new_label = (label_object){0};
            printf("Debug: at ld_arr[%d]\n", i);
            if(ld_arr[i]->ei != SUCCESS){
                continue;
            }
            else if (ld_arr[i]->is_label_def) {
                /*there is a label definition in the line*/
                printf(" Debug: label name in table arr [%d] is: %s , ic = %d, dc = %d\n", i, ld_arr[i]->label_name , *ic, *dc);

                /*if (search_label((ld_arr[i]->label_name), symbol_table,s_table_size)) {check if the label not already in the lable table*/
                /*label isn't already exist - definition is valid. add to table*/
                if (ld_arr[i]->is_instruction) {
                    strcpy(new_label->label_name, ld_arr[i]->label_name);
                    new_label->is_code = true;
                    new_label->type = relocatable;
                    new_label->label_value = *ic;
                    *ic += ld_arr[i]->inst->inst_line_keeper;
                    add_to_symbol_table(new_label, symbol_table, s_table_size);
                    s_table_size++;
                }
                else if(ld_arr[i]->is_direction) {/*direction line*/
                    strcpy(new_label->label_name, ld_arr[i]->label_name);
                    new_label->is_data = true;
                    new_label->type = relocatable;
                    new_label->label_value = *dc;
                    *dc += ld_arr[i]->dir->dir_line_keeper;
                    add_to_symbol_table(new_label, symbol_table, s_table_size);
                    s_table_size++;
                }

        }
        /*not a label definition*/
        else{
            if(ld_arr[i]->is_direction) {
                if (ld_arr[i]->dir->d_type == d_extern) {
                    int j, ex_arr_size = ld_arr[i]->dir->d_content->ex_arr->ex_size;
                    printf("Debug: external labels for ld_arr[%d]\n", i);
                    for (j = 0; j < ex_arr_size; ++j) {
                        label_object* new_label = (label_object*)safe_malloc(sizeof(label_object));
                        new_label->type = external;
                        new_label->is_extern = true;
                        strcpy(new_label->label_name,ld_arr[i]->dir->d_content->ex_arr->extern_[j]);
                        new_label->label_value = 0;
                        add_to_symbol_table(new_label, symbol_table, s_table_size);
                        s_table_size++;

                    }
                }
            }
            if(ld_arr[i]->is_instruction)
                *ic += ld_arr[i]->inst->inst_line_keeper;
        }
    }
    free(new_label);
return true;
    }


void add_to_symbol_table(label_object *label, label_object* symbol_table[], int size) {
    symbol_table[size] = label;
}


bool search_label(char* label_name, label_object* symbol_table[], int s_table_size){
    int i;
    /*label_object* which_label;*/
    for(i = 0; i < s_table_size; i++){
        if(strcmp(symbol_table[i]->label_name,label_name) == 0){
            if(symbol_table[i]->type == external){
                printf("Error: extern label can't be defined in the same file.\n");
                return true;
            }
            else if(symbol_table[i]->type == relocatable){
                printf("Error: label already defined.\n");
                return true;
            }
        }
    }
    return false;/*the label doesn't exist in the symbol table*/
}

void resize_ld_arr(line_data ** arr, int* size) {
    *size *= 2;
    int i;
    line_data* temp_arr = (line_data *)realloc(*arr, (*size) * sizeof(line_data));
    if (temp_arr == NULL) {
        printf("Memory reallocation failed.\n");
        free(*arr);
        exit(1);
    }

    *arr = temp_arr;
    }
