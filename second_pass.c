
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

int reg_num_bits[8][5] = {
    {0,0,0,0,0},
    {0,0,0,0,1},
    {0,0,0,1,0},
    {0,0,0,1,1},
    {0,0,1,0,0},
    {0,0,1,0,1},
    {0,0,1,1,0},
    {0,0,1,1,1},
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
    int i , j , char_as_ascii , data_count , binary_curr;
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
                    for (j = 0; j <= (*ld_arr)[i]->dir->d_content->string->str_len; j++)
                    {
                        char_as_ascii = (*ld_arr)[i]->dir->d_content->string->string[j];
                        printf("char: %c\n" , (*ld_arr)[i]->dir->d_content->string->string[j]);
                        printf("curr_index direction binary table: %d\n" , *direction_binary_table->curr_index);
                        intToTwosComplement(char_as_ascii, direction_binary_table->lines_as_binary[*(direction_binary_table->curr_index)] , 1);
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
                        intToTwosComplement((*ld_arr)[i]->dir->d_content->d_arr->data_arr[j] , direction_binary_table->lines_as_binary[*(direction_binary_table->curr_index)] , 1);
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
            binary_curr = *(instruction_binary_table->curr_index);
            printf("curr binary index: %d\n" , binary_curr);
            inst_coding_type(ld_arr , i , instruction_binary_table->lines_as_binary , binary_curr , symbol_table , st_size);
            (*instruction_binary_table->curr_index) += (*ld_arr)[i]->inst->inst_line_keeper;
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
afik. method 1: 12 digits , method 2: 10 left digits
*/
void intToTwosComplement(int num, int* binary_line , int method)
{
    int i , digits_count;
    digits_count = (method == 1)? BINARY_LENGTH : A_R_E_INDEX;
    int mask = 1 << (digits_count - 1);
    /* If the number is negative, calculate its two's complement value for 12 bits */
    if (num < 0)
    {
        num = (1 << digits_count) + num;
    }
    for (i = 0; i < digits_count; i++)
    {
        binary_line[i] = (num & mask) ? 1 : 0; /*afik_______________________________________________________________________________________*/
        mask >>= 1;
        if(method == 1)
            printf("%d" , binary_line[i]);
    }
    if(method == 1)
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
    free_or_close(2 , 2 , ent_fptr , ext_fptr);
    delete_null_file(first);
    delete_null_file(second);
    if(first != NULL)
        free(first);
    if(second != NULL)
        free(second);
}

int inst_coding_type(line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index , label_object **symbol_table[], int* st_size)
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
            for(j = 0 ; j < OPCODE_INDEX ; j++) {
                binary_line[binary_curr_index][j] = coding_type_arr[adjust_src_index][j];
            }
            printf("\n");
            for(j = OPCODE_INDEX ; j < DEST_INDEX ; j++)
                binary_line[binary_curr_index][j] = opcode_arr[opcode_number][j - OPCODE_INDEX];
            for(j = DEST_INDEX ; j < A_R_E_INDEX ; j++)
                binary_line[binary_curr_index][j] = coding_type_arr[adjust_dst_index][j - DEST_INDEX];
            binary_line[binary_curr_index][10] = binary_line[binary_curr_index][11] = 0; /*fix error*/
            break;
        }
    }    
    if(coding_type == ERROR)
        printf("Error, wrong coding way.\n");
    else {
        printf("Binary first line: ");
        for(j = 0 ; j < BINARY_LENGTH ; j++)
            printf("%d" , binary_line[binary_curr_index][j]);
        printf("\n");
    }
    binary_curr_index++;
    binary_extra_lines_insert(ld_arr , line_number , binary_line , binary_curr_index , symbol_table, st_size , type[0][i] , type[1][i]  , type[2][i]);
    return coding_type;
}

void binary_extra_lines_insert (line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index , label_object **symbol_table[], int* st_size , int s_type , int d_type , int code_type){
    int j , reg_s_num , reg_d_num , reg_num , first_str_index , operand_type , reg_start_index;
    bool *is_extern_label , extra_print;
    char *operand_str;

    reg_s_num = reg_d_num = -1;
    is_extern_label = malloc(sizeof(bool));
    *is_extern_label = false;

    if(code_type == reg_reg) {
        reg_s_num = ((*ld_arr)[line_number]->inst->src_name)[REG_NUM_STR_INDEX] - '0'; /*convert to int by ascii*/
        reg_d_num = ((*ld_arr)[line_number]->inst->dest_name)[REG_NUM_STR_INDEX] - '0';
        for(j = 0 ; j < SECOND_REG_INDEX ; j++)
                binary_line[binary_curr_index][j] = reg_num_bits[reg_s_num][j];
        for(j = SECOND_REG_INDEX ; j <  A_R_E_INDEX ; j++)
                binary_line[binary_curr_index][j] = reg_num_bits[reg_d_num][j - SECOND_REG_INDEX];
        printf("Binary second line: ");
        for(j = 0 ; j < BINARY_LENGTH ; j++)
            printf("%d" , binary_line[binary_curr_index][j]);
        printf("\n");
    }
    else if(code_type != non_non) {
        int extra_line_num;
        extra_line_num = 2;
        while(extra_line_num <= 3){
            first_str_index = 0;
            extra_print = false;
            if(extra_line_num == 2) {
                operand_str = (*ld_arr)[line_number]->inst->src_name;
                operand_type = s_type;
            }
            else{
                operand_str = (*ld_arr)[line_number]->inst->dest_name;
                operand_type = d_type;
            }
            switch(operand_type) {
                case immediate:
                    int imm = string_to_sign_int(operand_str , &first_str_index);
                    intToTwosComplement(imm , binary_line[binary_curr_index] , 2);
                    break;
                case label:
                    int label_val = label_value_search(operand_str , *symbol_table , *st_size , is_extern_label);
                    if(label_val != ERROR){
                        intToTwosComplement(label_val , binary_line[binary_curr_index] , 2);
                        if(*is_extern_label){
                            binary_line[binary_curr_index][A_R_E_INDEX] = 0;
                            binary_line[binary_curr_index][A_R_E_INDEX + 1] = 1;
                        }
                        else{
                            binary_line[binary_curr_index][A_R_E_INDEX] = 1;
                            binary_line[binary_curr_index][A_R_E_INDEX + 1] = 0;
                        }
                    }
                    else printf("Error: label %s does not exist.\n" , operand_str);
                    break;
                case reg:
                    reg_num = (operand_str)[REG_NUM_STR_INDEX] - '0'; /*convert to int by ascii*/
                    if(extra_line_num == 2)
                        reg_start_index = 0;
                    else reg_start_index = SECOND_REG_INDEX;
                    for(j = reg_start_index ; j < reg_start_index + REG_BINARY_DIGITS ; j++){
                        binary_line[binary_curr_index][j] = reg_num_bits[reg_num][j - reg_start_index];
                    }
                    break;
                default:
                    break; 
            }
            if((extra_line_num == 2 && s_type != none) || (extra_line_num == 3 && s_type == none)){
                printf("Binary second line: ");
                extra_print = true;
            }
            else if(extra_line_num == 3 && s_type != none && d_type != none){
                printf("Binary third line: ");
                extra_print = true;
            }
            if(extra_print){
                for(j = 0 ; j < BINARY_LENGTH ; j++)
                    printf("%d" , binary_line[binary_curr_index][j]);
                printf("\n");
            }
            extra_line_num++;
            binary_curr_index++;
        }
        operand_str = NULL;
    }
    free(is_extern_label);
}

bool delete_null_file(char* path){
    int empty_file;
    bool deleted;
    FILE* fptr;
    deleted = false;

    fptr = fopen(path , "r");
    empty_file = fgetc(fptr);
    if(empty_file == EOF){
        remove(path);
        deleted = true;
    }
    return deleted;  
}

