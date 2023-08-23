/*Created by elifu on 02/08/2023.*/

#include "assembler.h"

int main(int argc, char* argv[]) {
    int i , j;
    bool pre_valid , first_valid;

    for (i = 1; i < argc; i++) {

        FILE *am;
        char *as_file_path;
        char *as_suffix = ".as";
        char *am_suffix = ".am";

        int capacity = INITIAL_SIZE;
        int ld_arr_size = capacity;
        int st_size = 0;

        int ic = 100, dc = 0;
        line_data* ld = NULL;
        label_object* new_label = NULL;
        line_data** ld_arr = NULL;
        label_object** symbol_table = NULL;
        symbol_table = (label_object **) safe_calloc(capacity, sizeof(label_object*));
        ld_arr = (line_data **) safe_malloc((ld_arr_size) * sizeof(line_data *));

        if (strlen(argv[i]) > FILE_NAME) {
            printf("Filename is too long - %d characters max is allowed\n", FILE_NAME);
            continue;
        }

        as_file_path = path_string(argv[i], as_suffix);
        pre_valid = pre_as(as_file_path);
        pre_as(as_file_path);
        safe_free(as_file_path);
        am = safe_fopen(argv[i], "r", am_suffix);
        
        if(pre_valid) {
            first_valid = first_pass(am, &symbol_table, &st_size, &capacity, &ld_arr, &ld_arr_size, &ic, &dc, ld, new_label);

            if(first_valid) {
                second_pass(&symbol_table, &st_size, &ld_arr, &ic, &dc, ld_arr_size , argv[i]);
            }
        }

        fclose(am);
        free_ld_structs(ld_arr, ld_arr_size);
        for (j = 0; j < st_size ; ++j) {
            safe_free(symbol_table[j]);
        }
        safe_free_double_p((void **) &symbol_table);
        symbol_table = NULL;
        free(symbol_table);
        safe_free_double_p((void **) &new_label);
        new_label = NULL;

        safe_free_double_p((void **) &ld_arr);
        ld_arr = NULL;
        safe_free_double_p((void **) &ld);


    }
    return 0;

    }


void free_ld_structs(line_data **ld_arr, int ld_arr_size) {
    int i , j;
    for (i = 0; i < ld_arr_size; i++) {
        printf("ld_arr[i] is direction %d i = %d\n", ld_arr[i]->is_direction, i);
        printf("ld_arr[i] is instruction %d i = %d\n", ld_arr[i]->is_instruction, i);

        if (ld_arr[i]->is_direction) {
            if (ld_arr[i]->dir->d_type == d_string) {
                safe_free_double_p((void **) &ld_arr[i]->dir->d_content->string->string);
                safe_free_double_p((void **) &ld_arr[i]->dir->d_content->string);

            }
            else if (ld_arr[i]->dir->d_type == d_data) {
                safe_free_double_p((void **) &ld_arr[i]->dir->d_content->d_arr->data_arr);
                safe_free_double_p((void **) &ld_arr[i]->dir->d_content->d_arr);
            } else if (ld_arr[i]->dir->d_type == d_entry) {
                for (j = 0; j < ld_arr[i]->dir->d_content->en_arr->en_size ; ++j) {
                    safe_free(ld_arr[i]->dir->d_content->en_arr->entry[j]);
                }
                safe_free(ld_arr[i]->dir->d_content->en_arr->entry);
                safe_free(ld_arr[i]->dir->d_content->en_arr);
            } else if (ld_arr[i]->dir->d_type == d_extern) {
                for (j = 0; j <ld_arr[i]->dir->d_content->ex_arr->ex_size ; ++j) {
                    safe_free(ld_arr[i]->dir->d_content->ex_arr->extern_[j]);
                }
                safe_free(ld_arr[i]->dir->d_content->ex_arr->extern_);
                safe_free(ld_arr[i]->dir->d_content->ex_arr);
            }
            safe_free_double_p((void **) &ld_arr[i]->dir->d_content);
            safe_free_double_p((void **) &ld_arr[i]->dir);

        }
        else if (ld_arr[i]->is_instruction) {

            /*free (ld_arr[i]->inst->dest_name);
           /* ld_arr[i]->inst->dest_name = NULL;
            safe_free_double_p( (void **)&ld_arr[i]->inst->src_name);
            ld_arr[i]->inst->src_name = NULL;*/
            safe_free_double_p((void **) &ld_arr[i]->inst->op_args_type);
            safe_free_double_p((void **) &ld_arr[i]->inst);
            ld_arr[i] = NULL;

        }
        ld_arr[i] = NULL;
    }
}
