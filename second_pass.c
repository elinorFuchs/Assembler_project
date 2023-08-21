
#include "second_pass.h"

typedef struct binary_table {
    int **lines_as_binary; 
    int *curr_index;
    int start_address;
} binary_table;

int opcode_arr[16][4] = {
    {0,0,0,0},
    {0,0,0,1},
    {0,0,1,0},
    {0,0,1,1},
    {0,1,0,0},
    {0,1,0,1},
    {0,1,1,0},
    {0,1,1,1},
    {1,0,0,0},
    {1,0,0,1},
    {1,0,1,0},
    {1,0,1,1},
    {1,1,0,0},
    {1,1,0,1},
    {1,1,1,0},
    {1,1,1,1}
};

int coding_type_arr[4][3] = {
    {0,0,0},
    {0,0,1},
    {0,1,1},
    {1,0,1}
};

int A_R_E_arr[4][3] = {
    {0,0},
    {0,1},
    {1,0}
};

binary_table_p new_binary_table(int lines_count , int start_address) {
    int i;
    binary_table_p b1;
    b1 = malloc(sizeof(binary_table));
    b1->start_address = start_address;
    b1->curr_index = malloc(sizeof(int));
    *b1->curr_index = 0;
    b1->lines_as_binary = safe_calloc(lines_count , sizeof(int *));
    for (i = 0; i < lines_count; i++)
        b1->lines_as_binary[i] = safe_calloc(BINARY_LENGTH , sizeof(int));
    return b1;
}

void free_binary_table(binary_table_p b1 , int lines_count) {
    int i;
    for (i = 0; i < lines_count ; i++)
    	if(b1->lines_as_binary[i] != NULL)
        	free(b1->lines_as_binary[i]);
    if(b1->lines_as_binary != NULL)
        free(b1->lines_as_binary);
    if(b1->curr_index != NULL)
        free(b1->curr_index);
}

int second_pass(label_object **symbol_table[], int* st_size , line_data **ld_arr[], int *ic, int *dc , int lines_count , char* file_name)
{
    int i , j , char_as_ascii , data_count;
    printf("binary lines instructions: %d\n" , *ic - MEMORY_START_ADDRESS);
    binary_table_p instruction_binary_table = new_binary_table(*ic - MEMORY_START_ADDRESS , MEMORY_START_ADDRESS);
    printf("binary lines direcctions: %d\n" , *dc);
    printf("total lines: %d\n" , lines_count + 1);
    binary_table_p direction_binary_table = new_binary_table(*dc , *ic); 
    /* needed to add last line to the loop!!!!! */
    for (i = 0; i < lines_count + 1 ; i++){
	    printf("___________\nLine number: %d\n" , i + 1);
        if ((*ld_arr)[i]->is_direction) {
	        printf("direction type: %d\n" , (*ld_arr)[i]->dir->d_type);
            switch ((*ld_arr)[i]->dir->d_type) {
                case d_string:
                    printf(".string time: \n");
                    for (j = 0; j < (*ld_arr)[i]->dir->d_content->string->str_len; j++)
                    {
                        char_as_ascii = (*ld_arr)[i]->dir->d_content->string->string[j];
                        printf("char: %c\n" , (*ld_arr)[i]->dir->d_content->string->string[j]);
                        printf("curr_index direction binary table: %d\n" , *direction_binary_table->curr_index);
                        intToTwosComplement(char_as_ascii, direction_binary_table->lines_as_binary[*(direction_binary_table->curr_index)]);
                        (*direction_binary_table->curr_index)++; 
                    }
                    break;
                case d_data:
                    printf(".data time: \n");
                    data_count = (*ld_arr)[i]->dir->d_content->d_arr->data_arr_size;
                    printf("data count: %d\n" , data_count);
                    for (j = 0; j < data_count; j++) {
                        printf("num: %d\n" , (*ld_arr)[i]->dir->d_content->d_arr->data_arr[j]);
                        printf("curr_index direction binary table: %d\n" , *direction_binary_table->curr_index);
                        intToTwosComplement((*ld_arr)[i]->dir->d_content->d_arr->data_arr[j] , direction_binary_table->lines_as_binary[*(direction_binary_table->curr_index)]);
                        (*direction_binary_table->curr_index)++;
                    }
                    break;
                default:
                    if((*ld_arr)[i]->dir->d_type != d_entry && (*ld_arr)[i]->dir->d_type != d_extern)
                        printf("Error: direction type\n");
                    else if((*ld_arr)[i]->dir->d_type == d_entry) {
                        for(j = 0 ; j < (*ld_arr)[i]->dir->d_content->en_arr->en_size ; j++) {
                            if(search_label((*ld_arr)[i]->dir->d_content->en_arr->entry[j] , *symbol_table , *st_size) == -3) 
                                printf("Error: label \"%s\" not defined in this file\n" , (*ld_arr)[i]->dir->d_content->en_arr->entry[j]);      
                        }  
                    }
                    else {
                        for(j = 0 ; j < (*ld_arr)[i]->dir->d_content->ex_arr->ex_size ; j++) {
                            if(search_label((*ld_arr)[i]->dir->d_content->ex_arr->extern_[j] , *symbol_table , *st_size) == -3) 
                                printf("Error: label \"%s\" not defined in this file\n" , (*ld_arr)[i]->dir->d_content->en_arr->entry[j]);
                        }
                    }
                    break;
            }
        }    
        else if ((*ld_arr)[i]->is_instruction){
            inst_coding_type(ld_arr , i , instruction_binary_table->lines_as_binary[*(instruction_binary_table->curr_index)]);
            (*instruction_binary_table->curr_index)++;
        }
	    else if (!(*ld_arr)[i]->is_direction && !(*ld_arr)[i]->is_instruction)
		    printf("error: NOR type\n");
    }
    entries_and_externals_file(symbol_table, st_size , file_name);
    free_binary_table(instruction_binary_table , *ic - MEMORY_START_ADDRESS);
    if(instruction_binary_table != NULL)
        free(instruction_binary_table);
    free_binary_table(direction_binary_table , *dc);
    if(direction_binary_table != NULL)
        free(direction_binary_table);
    return 1;
}

/*
TODO - afik make tests!
*/
void intToTwosComplement(int num, int* binary_line)
{
    int i;
    /*afik*/
    printf("create another binary line:\n");
    int mask = 1 << (BINARY_LENGTH - 1);

    /* If the number is negative, calculate its two's complement value for 12 bits */
    if (num < 0)
    {
        num = (1 << BINARY_LENGTH) + num;
    }
    for (i = 0; i < BINARY_LENGTH; i++)
    {
        binary_line[i] = (num & mask) ? 1 : 0;
        mask >>= 1;
        printf("%d" , binary_line[i]);
    }
    printf("\n");
}

void entries_and_externals_file(label_object **symbol_table[], int* st_size , char* file_name)
{
    int i;
    char *first = malloc(sizeof(char) * (5 + strlen(file_name))); 
	strcpy(first , file_name);
	strcat(first , ".ent");
    char *second = malloc(sizeof(char) * (5 + strlen(file_name)));
    strcpy(second , file_name);
    strcat(second , ".ext");
    FILE *ent_fptr , *ext_fptr;
    ent_fptr = fopen( first , "w");
    ext_fptr = fopen( second , "w");
    char buffer[20]; 

    if(is_null_file(ent_fptr , ext_fptr , &first , &second)) 
        printf("Error\n");
    else {   
        for(i = 0 ; i < *st_size ; i++) {
            if((*symbol_table)[i]->is_entry){
	printf("Here entry: %s\n" , (*symbol_table)[i]->label_name); 
                fputs((*symbol_table)[i]->label_name , ent_fptr);
                fputs(": " , ent_fptr);
                sprintf(buffer , "%d", (*symbol_table)[i]->label_value);               
		fputs(buffer , ent_fptr);
                fputs("\n" , ent_fptr);
            }
            else if((*symbol_table)[i]->is_extern){
printf("Here extern: %s\n" , (*symbol_table)[i]->label_name);
                fputs((*symbol_table)[i]->label_name , ext_fptr);
                fputs("\n" , ext_fptr);
            }
else printf("Here Regular: %s \n" , (*symbol_table)[i]->label_name);
        }
    }
    if(first != NULL)
        free(first);
    if(second != NULL)
        free(second);
    free_or_close(2 , 2 , ent_fptr , ext_fptr);
}

int inst_coding_type(line_data **ld_arr[] , int line_number , int* binary_line)
{
    int type[3][13] =
         {{none    , none      , none      , none    , immediate , immediate , immediate , label     , label         , label     , reg       , reg       , reg} , \
          {none    , immediate , label     , reg     , immediate , label     , reg       , immediate , label         , reg       , immediate , label     , reg} , \
          {non_non , non_imm   , non_label , non_reg , imm_imm   , imm_label , imm_reg   , label_imm , label_label   , label_reg , reg_imm   , reg_label , reg_reg}};
    int i , j , coding_type , adjust_src_index , adjust_dst_index , opcode_number;

    coding_type = ERROR;
    /*change it to method of first line - afik*/
    for(i = 0 ; i < ADDRESS_TYPES ; i++){
        if((*ld_arr)[line_number]->inst->op_args_type->src[0] == type[0][i] && (*ld_arr)[line_number]->inst->op_args_type->dest[0] == type[1][i]) {
            coding_type = type[2][i];
            printf("Coding type: %d\n" , i);
            opcode_number = (*ld_arr)[line_number]->inst->op_args_type->code;
            adjust_src_index = (type[0][i] + 1) / 2;
            adjust_dst_index = (type[1][i] + 1) / 2;
            for(j = 0 ; j < OPCODE_INDEX ; j++)
                binary_line[j] = coding_type_arr[adjust_src_index][j];
            for(j = OPCODE_INDEX ; j < DEST_INDEX ; j++)
                binary_line[j] = opcode_arr[opcode_number][j - OPCODE_INDEX];
            for(j = DEST_INDEX ; j < A_R_E_INDEX ; j++)
                binary_line[j] = coding_type_arr[adjust_dst_index][j - DEST_INDEX];
            binary_line[BINARY_LENGTH - 2] = binary_line[BINARY_LENGTH - 1] = 0;
            break;
        }
    }
    if(coding_type == ERROR)
        printf("Error, wrong coding way.\n");
    else {
        printf("Binary first line: ");
        for(i = 0 ; i < BINARY_LENGTH ; i++)
            printf("%d" , binary_line[i]);
        printf("\n");
    }
    return coding_type;
}

