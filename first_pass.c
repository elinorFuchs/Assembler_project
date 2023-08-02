//
// Created by elifu on 02/08/2023.
//

#include "first_pass.h"
#include "line_parser.h"

/*fgets from am file->create line_data for each line and put in ld struct array->create symbol table -> pass to second pass */

/*create array of ld struct. fgets from am file.  call: create_line_data(char *line), then put the struct inside the ld array.  call the create_symbol_table func */
bool create_symbol_table (line_data* ld, int* ic, int* dc, label_object symbol_table[]) {

    /*loop through ld array:*/
    if( ld->label_name != NULL) {/*there is a label definition*/

        label_object* label = search_label((ld->label_name), symbol_table);/*check if the label already exist*/

        if (label != NULL) {/*label found in symbol table*/
            /*לבדוק מה עושים, אם התוית נמצאת בטבלה כאקסטרן, זאת בעיה כי אי אפשר להגדיר את הלייבל באותו קובץ שבו הוא הוכרז כאקסטרן
              ואם לא מוגדרת כאקסטרן גם שגיאה כי אי אפשר לבגדיר פעממים אותה תוית*/
        }
        else /*label definition*/
        {
            if(ld->is_instruction){

                strcpy(label->label_name,ld->label_name);
                label->d_or_c =code;
                label->type = relocatable;
                label->label_value = *ic;
                /*בכמה לקדם את ic? לפי קבוצת הפקודות, לבדוק ולהשלים קידום*/
                add_to_symbol_table (label,symbol_table);
            }
            else {/*direction line*/
                strcpy(label->label_name,ld->label_name);
                label->d_or_c =data;
                label->type = relocatable;
                label->label_value = *dc;

            }
        }


    }
    /*not a label definition*/
    /*המשך מעבר ראשון*/



}



void add_to_symbol_table (label_object* label,label_object* symbol_table){}

label_object* search_label(char label_name[],label_object* symbol_table){
/*
    loop the symbpl table, if the name is the same, check if type is external
     * if(type == external){
                   print_error_msg(&line, "Extern label can't be defined in the same file.");
                   return FALSE;
                   else{
                       print_error_msg(&line, "Label already defined.");
                       return FALSE;
                   }*/
    return NULL;
}
