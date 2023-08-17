/*Created by elifu on 02/08/2023.*/

#include "assembler.h"

#include "line_parser.h"
#include "first_pass.h"
#include "help_functions.h"

#include <stdio.h>


int main(int argc, char* argv[]){/*לעשות פונקציה שמחזירה מצביע לקובץ ומקבלת את הפרמטרים של fopen ואת הסיומת*/

    int ic = 100, dc = 0;
    label_object** symbol_table;
    line_data** ld_arr = NULL;
    FILE* am;
    int capacity  = INITIAL_SIZE;
    int ld_arr_size = capacity;
    int st_size = 0;

    pre_as("test1.as");
    am = fopen("test1.am","r");

    symbol_table =(label_object**)safe_calloc(capacity,sizeof (label_object*));
    ld_arr = (line_data **) safe_malloc((ld_arr_size) * sizeof(line_data *));


    /* am = fopen("validam.am","r");
     if (am == NULL) {
         printf("Error opening the file.\n");
         return 1;
     }*/

    first_pass (am, &symbol_table,&st_size, &capacity, &ld_arr, &ld_arr_size, &ic, &dc);
    second_pass(&symbol_table , &ld_arr , &ic, &dc , ld_arr_size);




    fclose(am);

    /*safe_free((void **)symbol_table);
    safe_free((void**)ld_arr);*/
    return 0;

}
/*
void free_ld_structs(line_data** ld_arr,*ld_arr_size){
    int i;
    for (i = 0; i < *ld_arr_size ; ++i) {

    }

    if((*ld_arr)[i].is_direction){
        if((*ld_arr)[i].dir->d_type == d_string){
            free((void**)(*ld_arr)[i].dir->d_content->string->string )
        }

    }

}
*/
