/*Created by elifu on 02/08/2023.*/

#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"
#include "help_functions.h"

int main(int argc, char* argv[]){

    int ic = 0, dc = 0;
    label_object* symbol_table[INITIAL_SIZE] = {0};
    line_data* ld_arr[INITIAL_SIZE];
     FILE* am;
    
    *symbol_table = (label_object*) safe_malloc(sizeof (label_object));
    /*ld_arr = safe_malloc(sizeof (*ld_arr));*/

    pre_as("test1.as");
    am = fopen("test1.am","r");
    if (am == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }
    first_pass (am, symbol_table, ld_arr, &ic, &dc);
    /*second_pass(symbol_table, ld_arr, &ic, &dc);*/

    fclose(am);

    return 0;

}


