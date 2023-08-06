//
// Created by elifu on 02/08/2023.
//

#include "assembler.h"
#include "line_parser.h"
#include "first_pass.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[]){



    FILE* am;
    am = fopen("test1.am","r");
    if (am == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }
    first_pass ( am);
    /*
   line_data* ld = NULL;
    line_data* ld_arr;
    ld_arr = malloc(sizeof (line_data)*10);
    while (fgets(line, MAX_LINE_SIZE, am)) {
        printf("\n%s\n", line);
        ld = create_line_data(line);
        ld_arr[i] = *ld;
        i++;

    }
*/
    fclose(am);

    return 0;

}

label_object* label_arr
/*label_object symbol_table[100];*/
/*int i;
    line_data* ld;
    for(i = 1; i < argc; i++){
        pre_as(argv[i]);
        create_line_data(line);
*/
/*const char line [100] = "main: mov @r3, length";*/

