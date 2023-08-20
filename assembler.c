/*Created by elifu on 02/08/2023.*/

#include "assembler.h"

int main(int argc, char* argv[]){/*לעשות פונקציה שמחזירה מצביע לקובץ ומקבלת את הפרמטרים של fopen ואת הסיומת*/

    FILE* am;
    int capacity  = INITIAL_SIZE;
    int ld_arr_size = capacity;
    int st_size = 0;
    int i;
    char* as_suffix = ".as";
    char* am_suffix = ".am";
    char* as_file_path;

    for (i = 1; i <= argc; i++) {
        int ic = 100, dc = 0;

        label_object** symbol_table;
        line_data** ld_arr = NULL;
        symbol_table =(label_object**)safe_calloc(capacity,sizeof (label_object*));
        ld_arr = (line_data **) safe_malloc((ld_arr_size) * sizeof(line_data *));

        if (strlen(argv[i]) > FILE_NAME) {
            printf("Filename is too long - %d characters max is allowed\n" , FILE_NAME);
            continue;
        }

        as_file_path= path_string(argv[i],as_suffix);
        pre_as(as_file_path);
        am = safe_fopen (argv[i], "r", am_suffix);
        first_pass (am, &symbol_table,&st_size, &capacity, &ld_arr, &ld_arr_size, &ic, &dc);
        second_pass(&symbol_table , &st_size , &ld_arr , &ic, &dc , ld_arr_size);
        fclose(am);
        free_ld_structs(&ld_arr,ld_arr_size);
        safe_free((void **)symbol_table);
        safe_free((void**)ld_arr);

    }

    return 0;

}

void free_ld_structs(line_data** ld_arr[], int ld_arr_size) {
    int i;
    for (i = 0; i < ld_arr_size; i++) {
        printf("ld_arr[i]  is direction %d i = %d\n", (*ld_arr)[i]->is_direction,i);
        printf("ld_arr[i]  is instruction %d i = %d\n", (*ld_arr)[i]->is_instruction,i);

        if ((*ld_arr)[i]->is_direction) {
            if ((*ld_arr)[i]->dir->d_type == d_string) {
                safe_free((void **)&(*ld_arr)[i]->dir->d_content->string->string);
                safe_free((void **) &(*ld_arr)[i]->dir->d_content->string);
            } else if ((*ld_arr)[i]->dir->d_type == d_data) {
                safe_free((void **) &(*ld_arr)[i]->dir->d_content->d_arr->data_arr);
                safe_free((void **) &(*ld_arr)[i]->dir->d_content->d_arr);

            } else if ((*ld_arr)[i]->dir->d_type == d_entry) {
                safe_free((void **) (*ld_arr)[i]->dir->d_content->en_arr->entry);
                safe_free((void **) &(*ld_arr)[i]->dir->d_content->en_arr);
            } else if ((*ld_arr)[i]->dir->d_type == d_extern) {
                safe_free((void **) (*ld_arr)[i]->dir->d_content->ex_arr->extern_);
                safe_free((void **) &(*ld_arr)[i]->dir->d_content->ex_arr);
            }
            safe_free((void **) &(*ld_arr)[i]->dir->d_content);
            safe_free((void **) &(*ld_arr)[i]->dir);
        } else if ((*ld_arr)[i]->is_instruction) {
         /*printf("dest name is : %s\n", (*ld_arr)[i]->inst->dest_name);*/
          /* safe_free((void **)&((*ld_arr)[i]->inst->dest_name));*/
            safe_free((void **) &(*ld_arr)[i]->inst->op_args_type);
            /*safe_free((void **)&(*ld_arr)[i]->inst->src_name);*/
            safe_free((void **) &(*ld_arr)[i]->inst);
        }
       /* free((void **)(ld_arr)[i]);*/
    }
}

