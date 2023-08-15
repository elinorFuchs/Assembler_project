/*Created by elifu on 02/08/2023.*/

#include "assembler.h"

int main(int argc, char* argv[]){
#include "line_parser.h"
#include "first_pass.h"
#include "help_functions.h"
#include <stdio.h>


int main(int argc, char* argv[]){/*לעשות פונקציה שמחזירה מצביע לקובץ ומקבלת את הפרמטרים של fopen ואת הסיומת*/

    int ic = 100, dc = 0;
    label_object** symbol_table = NULL;
    line_data** ld_arr = NULL;
    FILE* am;
    int capacity  = INITIAL_SIZE;


    pre_as("test1.as");
    am = fopen("test1.am","r");
    ld_arr = safe_malloc(sizeof (line_data*) *INITIAL_SIZE);
    symbol_table = safe_calloc(capacity,sizeof (label_object*));

    am = fopen("validam.am","r");
    if (am == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    if(first_pass (am, symbol_table, &capacity, ld_arr, &ic, &dc)) {
        second_pass(symbol_table, ld_arr, &ic, &dc);
    }




    fclose(am);

    safe_free((void **)&symbol_table);
    safe_free((void**)&ld_arr);
    return 0;

}


