/*Created by elifu on 02/08/2023.*/

#include "pre_assembler.h"

#define INITIAL_SIZE 10
#define ERROR_VAL -1


enum label_type{
    undefined_type,
    external,
    relocatable

};

typedef struct {
    char label_name[31];
    bool is_data;
    bool is_code;
    bool is_entry;
    bool is_extern;
    enum label_type type;
    int label_value;
}label_object;


/*Functions Declaration*/
/*gets all the data to parse the am file lines, return errors if exist.
 * create a line_data array that hold all the lines info, and a symbol table for the second pass*/
bool
first_pass(FILE *am, label_object **symbol_table[], int *st_size, int *capacity, line_data **ld_arr[], int *ld_arr_size,
           int *ic, int *dc, line_data *ld, label_object *new_label);

/*get pointer to the line data array, pointer to the symbol table. add labels from lines data to the symbol table.
 * return false if there is double declaration of a label or if there is an error in one of the lines in the file*/
bool
create_symbol_table(line_data *ld_arr[], int ld_arr_size, label_object **symbol_table[], int *st_size, int *capacity,
                    int *ic, int *dc, label_object *new_label);

/*search a label name string in the symbol table, if found : if it's an entry return the index to update label value,
 * if it's extern or double definition return -2, if not exist in the table return -3*/
int search_label(char* label_name, label_object *symbol_table[], int s_table_size);

/*add labes to the symbol table with values, type and name of each label*/
void add_to_symbol_table(label_object *label, label_object ***symbol_table, int *size, int *capacity);

/*realloc to the line_data array and the symbol table*/
void resize_symbol_table(label_object ***symbol_arr, int* capacity) ;
void resize_ld_arr(line_data*** arr, int* capacity);
int label_value_search(char* label_name, label_object *symbol_table[], int s_table_size , bool* is_extern);

