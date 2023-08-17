#include "second_pass.h"

typedef struct binary_table {
    int **lines_as_binary;
    int *curr_index;
    int start_address;
} binary_table;

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

int second_pass(label_object **symbol_table[], int* st_size , line_data **ld_arr[], int *ic, int *dc , int lines_count)
{
    int i , j , char_as_ascii , data_count;
printf("binary lines instructions: %d\n" , *ic - MEMORY_START_ADDRESS);
    binary_table_p instruction_binary_table = new_binary_table(*ic - MEMORY_START_ADDRESS , MEMORY_START_ADDRESS);
printf("binary lines direcctions: %d\n" , *dc);
printf("total lines: %d\n" , lines_count + 1);
    binary_table_p direction_binary_table = new_binary_table(*dc , *ic); 
/* needed to add last line to the loop!!!!! */
    for (i = 0; i < lines_count + 1 ; i++)
    {
	printf("number: %d\n" , i + 1);
        if ((*ld_arr)[i]->is_direction)
        {
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
        if ((*ld_arr)[i]->is_instruction)
        {
	printf("inst_line_keeper: %d\n" , (*ld_arr)[i]->inst->inst_line_keeper);
            switch ((*ld_arr)[i]->inst->inst_line_keeper)
            {
            case 1:
	printf("another time1:\n");
                break;
            case 2:
	printf("another time2:\n");
                break;
            case 3:
	printf("another time3:\n");
                break;
            default:
                printf("Error: instruction type\n");
            }
        }
	if (!(*ld_arr)[i]->is_direction && !(*ld_arr)[i]->is_instruction)
		printf("error: NOR type\n");
    }
    entries_and_externals_file(symbol_table, st_size);
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

void entries_and_externals_file(label_object **symbol_table[], int* st_size)
{
    int i;
    char *first = malloc(sizeof(char) * 8); 
	strcpy(first , "entries");
    char *second = malloc(sizeof(char) * 10);
    strcpy(second , "externals");
    FILE *ent_fptr , *ext_fptr;
    ent_fptr = fopen( first , "w");
    ext_fptr = fopen( second , "w");
    char buffer[20]; 

    if(is_null_file(ent_fptr , ext_fptr , &first , &second)) {
        printf("Error\n");
    }
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
                fputs(": " , ext_fptr);
                sprintf(buffer , "%d", (*symbol_table)[i]->label_value);
                fputs(buffer , ent_fptr);
                fputs("\n" , ext_fptr);
            }
printf("Here Regular: %s\n" , (*symbol_table)[i]->label_name);
        }
    }
    if(first != NULL)
        free(first);
    if(second != NULL)
        free(second);
    free_or_close(2 , 2 , ent_fptr , ext_fptr);
}

int inst_coding_type(line_data **ld_arr[] , int line_number)
{
    if((*ld_arr)[line_number]->inst->op_args_type->src == none && (*ld_arr)[line_number]->inst->op_args_type->dest == none)
        return non_non;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == none && (*ld_arr)[line_number]->inst->op_args_type->dest == immediate)
        return non_imm;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == none && (*ld_arr)[line_number]->inst->op_args_type->dest == label)
        return non_label;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == none && (*ld_arr)[line_number]->inst->op_args_type->dest == reg)
        return non_reg;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == immediate && (*ld_arr)[line_number]->inst->op_args_type->dest == immediate)
        return imm_imm;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == immediate && (*ld_arr)[line_number]->inst->op_args_type->dest == label)
        return imm_label;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == immediate && (*ld_arr)[line_number]->inst->op_args_type->dest == reg)
        return imm_reg;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == label && (*ld_arr)[line_number]->inst->op_args_type->dest == immediate)
        return label_imm;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == label && (*ld_arr)[line_number]->inst->op_args_type->dest == label)
        return label_label;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == label && (*ld_arr)[line_number]->inst->op_args_type->dest == reg)
        return label_reg;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == reg && (*ld_arr)[line_number]->inst->op_args_type->dest == immediate)
        return reg_imm;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == reg && (*ld_arr)[line_number]->inst->op_args_type->dest == label)
        return reg_label;
    else if((*ld_arr)[line_number]->inst->op_args_type->src == reg && (*ld_arr)[line_number]->inst->op_args_type->dest == reg)
        return reg_reg;
    else printg("Error, wrong coding way.\n");
    return -1;
}

void inst_binary_insert(coding_type type , int* binary_line , line_data **ld_arr[])
{
    switch(type) {
    case non_non:
        break;
    case non_imm:
        break;
    case non_label:
        break;
    case non_reg:
        break;
    case imm_imm:
        break;
    case imm_label:
        break;
    case imm_reg:
        break;
    case label_imm:
        break;
    case label_label:
        break;
    case label_reg:
        break;
    case reg_imm:
        break;
    case reg_label:
        break;
    case reg_reg:
        break;
    default:
        printf("Error: wrong coding type/\n");
        break;
    }
