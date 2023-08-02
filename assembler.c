//
// Created by elifu on 02/08/2023.
//

#include "assembler.h"
#include "line_parser.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[]){

    char line [MAX_LINE_SIZE];
    int i = 0;
    FILE* am;
    am = fopen("test1.am","r");
    if (am == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }
    line_data* ld = NULL;
    line_data* ld_arr;/*chang 2*/
    ld_arr = malloc(sizeof (line_data)*10);
    while (fgets(line, MAX_LINE_SIZE, am)) {
        printf("\n%s\n", line);
        ld = create_line_data(line);
        ld_arr[i] = *ld;
        /*printf("%d\n", ld->l_type.dir.dc.data);
        printf("%d\n", ld->l_type.dir.dc.string);
        printf("%d\n", ld->l_type.inst.op_address_method.code);*/
        i++;

        /*if (ld != NULL) {
            printf("\n%s\n", ld->label_name);
            printf("\n%d\n", ld->is_comment);
            printf("\n%d\n", ld->is_instruction);
        }*/
    }

    fclose(am);

    return 0;

}
/*label_object symbol_table[100];*/
/*int i;
    line_data* ld;
    for(i = 1; i < argc; i++){
        pre_as(argv[i]);
        create_line_data(line);
*/
/*const char line [100] = "main: mov @r3, length";*/

