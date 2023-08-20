/*Created by elifu on 02/08/2023.*/

#include "pre_assembler.h"

#define INITIAL_SIZE 10


enum label_type{
    undefined_type,
    external,
    relocatable

};

typedef struct label_object{
    char label_name[31];
    bool is_data;
    bool is_code;
    bool is_entry;
    bool is_extern;
    enum label_type type;
    int label_value;
}label_object;


/*functions declaration*/
/*get pointer to the line data array, and pointer to the empty symbol table and add labels from lines data to the symbol table*/
bool create_symbol_table(line_data *ld_arr[], int ld_arr_size, label_object **symbol_table[], int *st_size, int *capacity, int *ic, int *dc);
/*search a label name string in the symbol table, if found return true, else return false*/
int search_label(char* label_name, label_object *symbol_table[], int s_table_size);
bool first_pass (FILE* am, label_object** symbol_table[],int* st_size, int* capacity, line_data** ld_arr[],int* ld_arr_size, int* ic, int* dc);
void add_to_symbol_table(label_object *label, label_object ***symbol_table, int *size, int *capacity);
void resize_symbol_table(label_object ***symbol_arr, int* capacity) ;
void resize_ld_arr(line_data*** arr, int* capacity);
#endif /*ASSEMBLER_OPENU_PROJECT_FIRST_PASS_H*/
