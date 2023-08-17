#include "second_pass.h"

typedef struct binary_table {
    int **lines_as_binary;
    int *curr_index;
    int start_address;
} binary_table;

binary_table_p new_binary_table(int lines_count , int start_address) {
    int i;
	printf("lines count: %d \n" , lines_count);
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

int second_pass(label_object **symbol_table[], line_data **ld_arr[], int *ic, int *dc , int lines_count)
{
    int ld_arr_size , i , j , char_as_ascii , data_count;
    
    ld_arr_size = lines_count;

    binary_table_p instruction_binary_table = new_binary_table(*ic - MEMORY_START_ADDRESS , MEMORY_START_ADDRESS);
    binary_table_p direction_binary_table = new_binary_table(*dc , *ic); 
	printf("ld size: %d\n" , ld_arr_size);
    for (i = 0; i < ld_arr_size; i++)
    {
	printf("number: %d\n" , i + 1);
        if ((*ld_arr)[i]->is_direction)
        {
	printf("direction type: %d\n" , (*ld_arr)[i]->dir->d_type);
            switch ((*ld_arr)[i]->dir->d_type)
            {
            case d_string:
		printf(".string time: %d\n" , ld_arr_size);
                for (j = 0; j < (*ld_arr)[i]->dir->d_content->string->str_len; j++)
                {
                    char_as_ascii = (*ld_arr)[i]->dir->d_content->string->string[j];
                    printf("char: %c\n" , (*ld_arr)[i]->dir->d_content->string->string[j]);
                    printf("curr_index direction binary table: %d\n" , *direction_binary_table->curr_index);
                    intToTwosComplement(char_as_ascii, direction_binary_table->lines_as_binary[*direction_binary_table->curr_index]);
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
                    intToTwosComplement((*ld_arr)[i]->dir->d_content->d_arr->data_arr[j] , direction_binary_table->lines_as_binary[*direction_binary_table->curr_index]);
                    (*direction_binary_table->curr_index)++;
                }
                break;
            case d_entry:      
                break;
            case d_extern:
                break;
            default:
                printf("Error: direction type\n");
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

