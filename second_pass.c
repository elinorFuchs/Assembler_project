#include "second_pass.h"

typedef struct binary_table {
    int **lines_as_binary; 
    int *curr_index;
    int start_address;
} binary_table;

int opcode_arr[OPCODES_COUNT][OPCODES_LEN] = {
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

int coding_type_arr[CODING_TYPES][CODING_TYPE_LEN] = {
    {0,0,0},
    {0,0,1},
    {0,1,1},
    {1,0,1}
};

int A_R_E_arr[A_R_E_TYPES][A_R_E_DIGITS] = {
    {0,0},
    {0,1},
    {1,0}
};

int reg_num_bits[REGISTERS_NUM][REG_WORD_DIGITS] = {
    {0,0,0,0,0},
    {0,0,0,0,1},
    {0,0,0,1,0},
    {0,0,0,1,1},
    {0,0,1,0,0},
    {0,0,1,0,1},
    {0,0,1,1,0},
    {0,0,1,1,1},
};

char base64_table[BASE_64_DIGITS] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
    };

int type[LINE_CODING_MODES][DIFF_CODING_OPTIONS] = 
{
    {none    , none      , none      , none    , immediate , immediate , immediate , \
                                    label     , label         , label     , reg       , reg       , reg} , \
    {none    , immediate , label     , reg     , immediate , label     , reg       , \
                                    immediate , label         , reg       , immediate , label     , reg} , \
    {non_non , non_imm   , non_label , non_reg , imm_imm   , imm_label , imm_reg   , \
                                    label_imm , label_label   , label_reg , reg_imm   , reg_label , reg_reg}
};

bool second_valid = true;

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

void direction_string_convert(line_data **ld_arr[],int* char_as_ascii,int index,binary_table_p direction_binary_table){
    int j;
    printf(".string time: \n");
    for (j = 0; j <= (*ld_arr)[index]->dir->d_content->string->str_len; j++)
    {
        *char_as_ascii = (*ld_arr)[index]->dir->d_content->string->string[j];
        printf("char: %c\n" , (*ld_arr)[index]->dir->d_content->string->string[j]);
        printf("curr_index direction binary table: %d\n" , *direction_binary_table->curr_index);
        intToTwosComplement(*char_as_ascii,\
                             direction_binary_table->lines_as_binary[*(direction_binary_table->curr_index)] , 1);
        (*direction_binary_table->curr_index)++; 
    }
}

void direction_data_convert(line_data **ld_arr[] , int* data_count , int index ,binary_table_p direction_binary_table){
    int j;
    printf(".data time: \n");
    *data_count = (*ld_arr)[index]->dir->d_content->d_arr->data_arr_size;
    printf("data count: %d\n" , *data_count);
    for (j = 0; j < *data_count; j++) {
        printf("num: %d\n" , (*ld_arr)[index]->dir->d_content->d_arr->data_arr[j]);
        printf("curr_index direction binary table: %d\n" , *direction_binary_table->curr_index);
        intToTwosComplement((*ld_arr)[index]->dir->d_content->d_arr->data_arr[j] ,\
                             direction_binary_table->lines_as_binary[*(direction_binary_table->curr_index)] , 1);
        (*direction_binary_table->curr_index)++;
    }
}

void direction_label_dec_valid(label_object **symbol_table[], int* st_size , line_data **ld_arr[] , int index){
    int j;
    bool label_found;
    if((*ld_arr)[index]->dir->d_type != d_entry && (*ld_arr)[index]->dir->d_type != d_extern){
        printf("Error: direction type\n");
        second_valid = false;
    }
    else if((*ld_arr)[index]->dir->d_type == d_entry) {
        for(j = 0 ; j < (*ld_arr)[index]->dir->d_content->en_arr->en_size ; j++) {
            label_found = search_label((*ld_arr)[index]->dir->d_content->en_arr->entry[j] , *symbol_table ,*st_size);
            if(label_found == LABEL_NOT_EXIST){
                printf("Error: label \"%s\" not defined in this file\n" ,\
                      (*ld_arr)[index]->dir->d_content->en_arr->entry[j]);
                second_valid = false;
            }    
        }  
    }
    else {
        for(j = 0 ; j < (*ld_arr)[index]->dir->d_content->ex_arr->ex_size ; j++) {
            label_found = search_label((*ld_arr)[index]->dir->d_content->ex_arr->extern_[j] , *symbol_table ,*st_size);
            if(label_found == LABEL_NOT_EXIST){
                printf("Error: label \"%s\" not defined. \n" , (*ld_arr)[index]->dir->d_content->en_arr->entry[j]);
                second_valid = false;
            }       
        }
    }
}

int second_pass(label_object **symbol_table[], int* st_size , line_data **ld_arr[], int *ic, int *dc ,\
                int lines_count , char* file_name)
{
    int i , char_as_ascii , data_count , binary_curr , inst_len , dir_len;
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
                    direction_string_convert(ld_arr , &char_as_ascii , i , direction_binary_table);
                    break;
                case d_data:
                    direction_data_convert(ld_arr , &data_count , i , direction_binary_table);
                    break;
                default:
                    direction_label_dec_valid(symbol_table , st_size , ld_arr , i);
                    break;
            }
        }    
        else if ((*ld_arr)[i]->is_instruction){
            binary_curr = *(instruction_binary_table->curr_index);
            printf("curr binary index: %d\n" , binary_curr);
            inst_coding_type(ld_arr,i,instruction_binary_table->lines_as_binary,binary_curr , symbol_table , st_size);
            (*instruction_binary_table->curr_index) += (*ld_arr)[i]->inst->inst_line_keeper;
        }
	    else if (!(*ld_arr)[i]->is_direction && !(*ld_arr)[i]->is_instruction){
            printf("error: NOR type\n");
            second_valid = false;
        }
    }
    inst_len = (*instruction_binary_table->curr_index);
    dir_len = (*direction_binary_table->curr_index);
    binary_to_64(instruction_binary_table->lines_as_binary , direction_binary_table->lines_as_binary , file_name,\
                 inst_len , dir_len);
    entries_and_externals_file(symbol_table, st_size , file_name);
    free_binary_table(instruction_binary_table , *ic - MEMORY_START_ADDRESS);
    if(instruction_binary_table != NULL)
        free(instruction_binary_table);
    free_binary_table(direction_binary_table , *dc);
    if(direction_binary_table != NULL)
        free(direction_binary_table);
    return 1;
}

/* method 1: 12 digits , method 2: 10 left digits */
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
        binary_line[i] = (num & mask) ? 1 : 0;
        mask >>= 1;
        if(method == 1)
            printf("%d" , binary_line[i]);
    }
    if(method == 1)
        printf("\n");
}

void labels_files_creation(char** first , char** second , char* file_name , FILE **ent_fptr , FILE **ext_fptr){
    *first = malloc(sizeof(char) * (ENT_SUFFIX_LEN + strlen(file_name)));
    *second = malloc(sizeof(char) * (EXT_SUFFIX_LEN + strlen(file_name))); 

    strcpy(*first , file_name);
	strcat(*first , ".ent");
    strcpy(*second , file_name);
    strcat(*second , ".ext");
    *ent_fptr = fopen(*first , "w");
    *ext_fptr = fopen(*second , "w");
}

void entries_and_externals_file(label_object **symbol_table[], int* st_size , char* file_name){
    int i;
    char *first , *second , buffer[MAX_ADDRESS_BIN_DIGITS_STR];
    FILE *ent_fptr , *ext_fptr;
	labels_files_creation(&first , &second , file_name , &ent_fptr , &ext_fptr);

    if(is_null_file(ent_fptr , ext_fptr , &first , &second)){
        printf("Error\n");
        second_valid = false;
    } 
    else {   
        for(i = 0 ; i < *st_size ; i++) {
            if((*symbol_table)[i]->is_entry){
                fputs((*symbol_table)[i]->label_name , ent_fptr);
                fputs(": " , ent_fptr);
                sprintf(buffer , "%d", (*symbol_table)[i]->label_value);               
		        fputs(buffer , ent_fptr);
                fputs("\n" , ent_fptr);
            }
            else if((*symbol_table)[i]->is_extern){
                fputs((*symbol_table)[i]->label_name , ext_fptr);
                fputs("\n" , ext_fptr);
            }
        }
    }
    free_or_close(2 , 2 , ent_fptr , ext_fptr);
    delete_null_file(first);
    delete_null_file(second);
    free_or_close(1 , 2 , first , second);
}

void binary_first_line_insert(line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index,\
                              int found_index , int* coding_type){
    int j , adjust_src_index , adjust_dst_index , opcode_number;

    *coding_type = type[2][found_index];
    opcode_number = (*ld_arr)[line_number]->inst->op_args_type->code;
    /*adjustment coding type to table lines index 0->0 , 1->1 , 3->2 , 5->3*/
    adjust_src_index = (type[0][found_index] + 1) / 2; 
    adjust_dst_index = (type[1][found_index] + 1) / 2;
    for(j = 0 ; j < OPCODE_INDEX ; j++) {
        binary_line[binary_curr_index][j] = coding_type_arr[adjust_src_index][j];
    }
    printf("\n");
    for(j = OPCODE_INDEX ; j < DEST_INDEX ; j++)
        binary_line[binary_curr_index][j] = opcode_arr[opcode_number][j - OPCODE_INDEX];
    for(j = DEST_INDEX ; j < A_R_E_INDEX ; j++)
        binary_line[binary_curr_index][j] = coding_type_arr[adjust_dst_index][j - DEST_INDEX];
    binary_line[binary_curr_index][A_R_E_INDEX] = binary_line[binary_curr_index][BINARY_LAST_INDEX] = 0; 
}

int inst_coding_type(line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index ,\
                     label_object **symbol_table[], int* st_size)
{
    int i , j , coding_type;
    bool source_fit , dest_fit;

    coding_type = ERROR;
    for(i = 0 ; i < ADDRESS_TYPES ; i++){
        source_fit = (*ld_arr)[line_number]->inst->op_args_type->src[0] == type[0][i];
        dest_fit = (*ld_arr)[line_number]->inst->op_args_type->dest[0] == type[1][i];
        if(source_fit && dest_fit) {
            binary_first_line_insert(ld_arr , line_number , binary_line , binary_curr_index , i , &coding_type);
            break;
        }
    }    
    if(coding_type == ERROR){
        printf("Error, wrong coding way.\n");
        second_valid = false;
    }
    else {
        printf("Binary first line: ");
        for(j = 0 ; j < BINARY_LENGTH ; j++)
            printf("%d" , binary_line[binary_curr_index][j]);
        printf("\n");
    }
    binary_curr_index++;
    binary_extra_lines_insert(ld_arr , line_number , binary_line , binary_curr_index , symbol_table, st_size,\
                              type[0][i] , type[1][i]  , type[2][i]);
    return coding_type;
}

void two_reg_binary_line(line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index ,\
                         int* reg_s_num , int* reg_d_num){
    int j;

    *reg_s_num = ((*ld_arr)[line_number]->inst->src_name)[REG_NUM_STR_INDEX] - '0'; /*convert to int by ascii*/
    *reg_d_num = ((*ld_arr)[line_number]->inst->dest_name)[REG_NUM_STR_INDEX] - '0';
    for(j = 0 ; j < SECOND_REG_INDEX ; j++)
            binary_line[binary_curr_index][j] = reg_num_bits[*reg_s_num][j];
    for(j = SECOND_REG_INDEX ; j <  A_R_E_INDEX ; j++)
            binary_line[binary_curr_index][j] = reg_num_bits[*reg_d_num][j - SECOND_REG_INDEX];
    printf("Binary second line: ");
    for(j = 0 ; j < BINARY_LENGTH ; j++)
        printf("%d" , binary_line[binary_curr_index][j]);
    printf("\n");
}

void binary_extra_lines_adjustment(line_data **ld_arr[] , int line_number , int extra_line_num , char **operand_str,\
                                   int *operand_type  , int s_type , int d_type){
    if(extra_line_num == 2) {
        *operand_str = (*ld_arr)[line_number]->inst->src_name;
        *operand_type = s_type;
    }
    else{
        *operand_str = (*ld_arr)[line_number]->inst->dest_name;
        *operand_type = d_type;
    }
}

void binary_label_line(label_object **symbol_table[] , int **binary_line , int binary_curr_index , int *label_val ,\
                       char *operand_str , int* st_size , bool *is_extern_label){
    *label_val = label_value_search(operand_str , *symbol_table , *st_size , is_extern_label);
    if(*label_val != ERROR){
        intToTwosComplement(*label_val , binary_line[binary_curr_index] , 2);
        if(*is_extern_label){
            binary_line[binary_curr_index][A_R_E_INDEX] = 0;
            binary_line[binary_curr_index][BINARY_LAST_INDEX] = 1;
        }
        else{
            binary_line[binary_curr_index][A_R_E_INDEX] = 1;
            binary_line[binary_curr_index][BINARY_LAST_INDEX] = 0;
        }
    }
    else {
        printf("Error: label %s does not exist.\n" , operand_str);
        second_valid = false;
    } 
}

void binary_opernd_handle(int **binary_line , label_object **symbol_table[] , int* imm , int operand_type ,\
                          char *operand_str , int* first_str_index , int binary_curr_index , int* label_val ,\
                          int* st_size, bool *is_extern_label, int* reg_num, int extra_line_num, int *reg_start_index){
    int j;
    switch(operand_type) {
        case immediate:
            *imm = string_to_sign_int(operand_str , first_str_index);
            intToTwosComplement(*imm , binary_line[binary_curr_index] , 2);
            break;
        case label:
            binary_label_line(symbol_table,binary_line,binary_curr_index,label_val,operand_str,st_size,is_extern_label);
            break;
        case reg:
            *reg_num = (operand_str)[REG_NUM_STR_INDEX] - '0'; /*convert to int by ascii*/
            if(extra_line_num == 2)
                *reg_start_index = 0;
            else *reg_start_index = SECOND_REG_INDEX;
            for(j = *reg_start_index ; j < *reg_start_index + REG_BINARY_DIGITS ; j++){
                binary_line[binary_curr_index][j] = reg_num_bits[*reg_num][j - *reg_start_index];
            }
            break;
        default:
            break; 
    }
}

void binary_extra_lines_insert (line_data **ld_arr[] , int line_number , int **binary_line , int binary_curr_index ,\
                                label_object **symbol_table[], int* st_size , int s_type , int d_type , int code_type){
    int j , reg_s_num , reg_d_num , reg_num , first_str_index;
    int operand_type , reg_start_index , imm , label_val , extra_line_num;
    bool *is_extern_label , extra_print;
    char *operand_str;

    reg_s_num = reg_d_num = ERROR;
    is_extern_label = malloc(sizeof(bool));
    *is_extern_label = false;

    if(code_type == reg_reg) 
        two_reg_binary_line(ld_arr , line_number , binary_line , binary_curr_index , &reg_s_num , &reg_d_num);
    else if(code_type != non_non) {
        extra_line_num = SECOND_BINARY_LINE;
        while(extra_line_num <= THIRD_BINARY_LINE){
            first_str_index = 0 ,extra_print = false;
            binary_extra_lines_adjustment(ld_arr , line_number , extra_line_num , &operand_str , &operand_type ,\
                                           s_type , d_type);
            binary_opernd_handle(binary_line , symbol_table , &imm , operand_type ,  operand_str , &first_str_index ,\
                                 binary_curr_index , &label_val , st_size, is_extern_label, &reg_num, extra_line_num,\
                                 &reg_start_index);
            if((extra_line_num == SECOND_BINARY_LINE && s_type != none) ||\
               (extra_line_num == THIRD_BINARY_LINE && s_type == none)){
                printf("Binary second line: ");
                extra_print = true;
            }
            else if(extra_line_num == THIRD_BINARY_LINE && s_type != none && d_type != none){
                printf("Binary third line: ");
                extra_print = true;
            }
            if(extra_print){
                for(j = 0 ; j < BINARY_LENGTH ; j++)
                    printf("%d" , binary_line[binary_curr_index][j]);
                printf("\n");
            }
            extra_line_num++;
            if(operand_type != none)
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
    if(fptr == NULL) {  
        printf("couldn't open file %s" , path);
        return deleted;
    }
    empty_file = fgetc(fptr);
    if(empty_file == EOF){
        remove(path);
        deleted = true;
    }
    fclose(fptr);
    return deleted;  
}

void binary_file_title_insert(FILE* fptr , int inst_len , int dir_len){
    fprintf(fptr, "%d", inst_len);
    fputc(' ' , fptr);
    fprintf(fptr, "%d", dir_len);
    fputc('\n' , fptr);
}

void binary_to_64(int** inst_binary_table , int** dir_binary_table , char* path , int inst_len , int dir_len){
    FILE* fptr;
    char *binary_file_name;
    int curr_len , **curr_table , time;
    bool valid;

    binary_file_name = safe_malloc(sizeof(char) * (strlen(path) + OB_SUFFIX_LEN));
    strcpy(binary_file_name , path);
    strcat(binary_file_name , ".ob");
    time = 1;
    valid = true;

    bool_safe_fopen(&fptr , binary_file_name , &valid);  
    binary_file_title_insert(fptr , inst_len , dir_len);

    while(time <= BINARY_TABLE_COUNT && valid){
        binary_line_convert(fptr,time,&curr_table,inst_binary_table,dir_binary_table,&curr_len , inst_len , dir_len);
        time++;
    }
    
    curr_table = NULL;
    free_or_close(2 , 1 , fptr);
    free_or_close(1 , 1 , binary_file_name);
}
 
void binary_line_convert(FILE* fptr , int time , int*** curr_table , int** inst_binary_table , int** dir_binary_table,\
                         int* curr_len , int inst_len , int dir_len){
    int i , j ,  left , right;
    char left_char , right_char;

    left = right = 0;

    if(time < BINARY_TABLE_COUNT) {
        *curr_len = inst_len;
        *curr_table = inst_binary_table;
    }
    else{
        *curr_len = dir_len;
        *curr_table = dir_binary_table;
    }

    for(i = 0 ; i < *curr_len ; i++){
        left = right = 0;
        for(j = LEFT_LAST_BINARY_INDEX ; j >= 0 ; j--)
            left += ((*curr_table)[i][j] * pow(BINARY_DIGITS , LEFT_LAST_BINARY_INDEX - j));
        for(j = RIGHT_BINARY_INDEX ; j < BINARY_LENGTH ; j++)
            right += ((*curr_table)[i][j] * pow(BINARY_DIGITS , BINARY_LAST_INDEX - j));
        left_char = base64_table[left];
        right_char = base64_table[right];
        fputc(left_char , fptr);
        fputc(right_char , fptr);
        fputc('\n' , fptr);
    }
}
