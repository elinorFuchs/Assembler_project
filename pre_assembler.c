#include "pre_assembler.h"

typedef struct mcro_table {
    char** mcro_name;
    char** line;
    int index;
} mcro_table;
 
mcro_table_p create_mcro_table()
{
    int i;
    mcro_table* m1;

    m1 = malloc(sizeof(mcro_table));
    m1->mcro_name = malloc(sizeof(char*) * MAX_MACROS_NUM);
    m1->line = malloc(sizeof(char*) * MAX_MACROS_NUM);
    m1->index = 0;

    for (i = 0; i < MAX_MACROS_NUM; i ++) {
        m1->mcro_name[i] = malloc(sizeof(char) * MAX_MCRO_NAME);
        m1->line[i] = malloc(sizeof(char) * MAX_LINE_LEN);
        m1->mcro_name[i][0] = '\0';
        m1->line[i][0] = '\0';
    }
    return m1;
}

bool valid_mcro_name(char* mcro_name)
{
    int i;
    bool valid;
    valid = true;
    for(i = 0 ; i < DIFF_INSTRUCTION_COUNT ; i++) {
        if(strcmp(mcro_name , inst_Arr[i]) == 0) {
            valid = false;
            break;
        }
    }
    return valid;    
}

void mcro_table_line_increase(mcro_table_p m1) {
    m1->line[m1->index] = realloc(m1->line[m1->index] , sizeof(char) * MAX_LINE_LEN * (m1->index + 1));
}

void is_mcro_line(mcro_table_p m1 , char* line , char **inside_mcro) {
    int index , exist_index;
    char *name_check , *line2;

    index = 0;

    clean_white(line , &index);
    line2 = malloc(sizeof(char) * ((strlen(&line[index])) + 1));
    if(line2 == NULL) {
        printf("Memory allocation Failed\n");
    }
    line2 = strcpy(line2 , &line[index]);
    name_check = strtok(line2 , " \t\n\r\0\f");
    exist_index = is_exist_mcro(m1 , name_check);
    if(exist_index != -1 && is_all_white(&line[index + strlen(name_check)])) {
        *inside_mcro = malloc(sizeof(char) * (1 + strlen(m1->line[exist_index])));
        if(*inside_mcro != NULL)
        strcpy(*inside_mcro , m1->line[exist_index]);
    }
    else *inside_mcro = NULL;
    free_or_close(1 , 1 , line2);
}

int is_exist_mcro (mcro_table_p m1 , char *mcro_name_to_check)
{
    int mcro_index;
    int i;
    
    mcro_index = -1;
    for(i = 0 ; i < m1->index ; i++)
        if(strcmp(m1->mcro_name[i] , mcro_name_to_check) == 0) {
            mcro_index = i;
            break;
        }
    return mcro_index;
}

void insert_mcro(mcro_table_p m1 , char *mcro_name_to_check , char *line , int method) {
    if(method == 1 && is_exist_mcro(m1 , mcro_name_to_check) == -1 && valid_mcro_name(mcro_name_to_check)) {
        strcpy(m1->mcro_name[m1->index] , mcro_name_to_check);
    }
    else if(method == 2) {
        mcro_table_line_increase(m1);
        strcat(m1->line[m1->index] , line);
    }
}

void free_mcro_table(mcro_table_p m1)
{
    int index;
    for (index = 0; index < MAX_MACROS_NUM; index++) {
        free_or_close(1 , 2 , m1->mcro_name[index] , m1->line[index]);
    }
    if(m1->mcro_name != NULL)
        free(m1->mcro_name);
    if(m1->line != NULL)
        free(m1->line);
}

void str_copy(char **copy , char **original , bool start_no_spaces)
{
    int index;
    *copy = malloc(sizeof(char) * (1 + strlen(*original)));
    index = 0;
    if(*copy != NULL) {
        if(start_no_spaces) {
            clean_white(*original , &index);
            strcpy(*copy , &(*original)[index]);
        }
        else strcpy(*copy , *original);
    }
    else(printf("Error: copy line creation failed."));
}

void mcro_dec_line(int* index, int* index_copy, bool* mcro_flag, char** curr_line_index, char* curr_line, 
                   char** mcro_name, mcro_table_p m1, char** line_copy, int line_number) {
    *mcro_flag = false;
    (*index) += MCRO_LEN;
    *curr_line_index = &curr_line[*index];
    str_copy(line_copy , curr_line_index , true);
    while(!(isspace(curr_line[*index_copy])) && (curr_line[*index_copy] != '\0'))
        (*index_copy)++;
    clean_white(curr_line , index_copy);
    *mcro_name = strtok(&curr_line[*index] , " \f\r\t\n");
    (*mcro_name)[strlen(*mcro_name)] = '\0';
    insert_mcro(m1 , *mcro_name , NULL , 1);
    if(!is_all_white(&(curr_line[*index_copy]))) 
        printf("ERROR: forbidden macro declaration in line %d \"%s\". Only one word allowed.\n" , \
        line_number , strtok(*line_copy , "\n\0"));
    else if(is_exist_mcro(m1 , *mcro_name) != -1) 
        printf("ERROR: macro named \"%s\" declared twice" , *mcro_name);
    else if(!valid_mcro_name(*mcro_name)) 
        printf("ERROR: macro name \"%s\" is name of instruction\n" , *mcro_name);
    else *mcro_flag = true;
    free_or_close(1 , 1 , *line_copy);
}


int pre_as(char* path) {
 int index , index_copy , line_number;
    char *am_file_name , *curr_line , *mcro_name , *line_copy , *corrected_line , *inside_mcro , *curr_line_index;
    FILE *fptr , *fptr2;
    mcro_table* m1;
    bool mcro_flag;

    strings_letter_change(strlen(path) - 1 , 'm' , path , &am_file_name);
    line_copy = NULL , mcro_name = NULL;
    line_number = 0;
    mcro_flag = false;
    fptr = fopen(path , "r");
    fptr2 = fopen(am_file_name , "w");
    m1 = create_mcro_table();
    curr_line = malloc(sizeof(char) * MAX_LINE_LEN);
    
    if(is_null_file(fptr , fptr2 , &path ,  &am_file_name)) {
        free_or_close(1 , 1 , curr_line);
        return -1;
    }

    while(fgets(curr_line , MAX_GET_LEN , fptr) != NULL) {
        index = 0 , line_number++;
        clean_white(curr_line , &index);

        if(strlen(curr_line) > MAX_LINE_LEN) {
            printf("Error: Line %d too long and will be omitted.\n" , line_number);
            continue;
        }  /*too long line check*/
        if(is_all_white(curr_line) || curr_line[index] == ';') {
            continue;
        } /*empty or note line skip*/
        is_mcro_line(m1 , curr_line , &inside_mcro);
        if(inside_mcro != NULL) {
            fputs(inside_mcro , fptr2);
        continue; 
        } /*line to replace with a macro content case. was without mcro flag check */
        
        index_copy = index + MCRO_LEN;
        clean_white(curr_line , &index_copy);
        if(!strncmp(&curr_line[(index)] , "mcro" , MCRO_LEN) && !isspace(curr_line[index_copy]) &&
        !(strlen(&curr_line[index]) > MCRO_LEN && !isspace(curr_line[index + MCRO_LEN]))) { 
            mcro_dec_line(&index, &index_copy , &mcro_flag, &curr_line_index, curr_line, &mcro_name, \
            m1, &line_copy, line_number);
        } /* case: optional macro declaration line */
        else if(!mcro_flag) { 
            if (!is_all_white(curr_line)) {
                corrected_line = line_template(&(curr_line[index]));
                if(corrected_line[index] != '\0')
                    fputs(corrected_line , fptr2);
                free_or_close(1 , 1 , corrected_line);
            }  
        } /* case: regular line (not in macro declaration and not empty) */
        else { 
            clean_white(curr_line , &index);
            if(!strncmp(&curr_line[index] , "endmcro" , ENDMCRO_LEN) && is_all_white(&curr_line[index + ENDMCRO_LEN])){
                mcro_flag = false; 
                mcro_name = NULL;
                (m1->index)++;
            } /* "endmcro" line */
            else {
                corrected_line = line_template(curr_line);
                if(corrected_line[index] != '\0') {
                    insert_mcro(m1 , NULL , corrected_line , 2);
                }
                free_or_close(1 , 1 , corrected_line);
            } /*line of content inside macro declaration */
        } /* case: line inside macro declaration */       
    }
    free_or_close(2 , 2 , fptr , fptr2);
    free_or_close(1 , 3 , am_file_name ,  curr_line , inside_mcro);
    free_mcro_table(m1);
    if(m1 != NULL)
        free(m1);
    return 1;
}

void free_or_close(int method , int count, ...) {
    int i;
    char *str;
    FILE *ptr;
    va_list args;
    va_start(args, count);
    
    for(i = 0 ; i < count ; i++) {
        if(method == 1) {
            str = va_arg(args, char*);
            if(str != NULL)
                free(str);
        }  
        else if(method == 2) {
            ptr = va_arg(args, FILE*);
            if(ptr != NULL)
                fclose(ptr);
        }
    }
    va_end(args);
}

int is_white(char c)
{
    int white = 0;
    if(c == ' ' || c == '\t' || c == '\f' || c == '\v' || c == '\r') 
        white = 1;
    return white;
}

int is_all_white(char* line)
{
    int white_line, i , line_len;
    white_line = true;
    line_len = strlen(line);
    for(i = 0 ; i < line_len ; i++) 
        if (!isspace(line[i]) && line[i] != '\0') 
            return false;
    return white_line;
}

void clean_white(char* line , int* index) {
    while(is_white(line[*index])) 
            (*index)++;
}
  
char* line_template(char* line) {
    int index , template_index, extra_spaces_counter , max_extra;
    char *template_line , temp;

    index = 0 , template_index = 0 , extra_spaces_counter = 0 , max_extra = 2;
    template_line = malloc(sizeof(char) * strlen(line) + END_LINE_CHARS + max_extra);
    
    while(line[index] != '\n' && line[index] != '\0') {
        if(line[index] == ' ' || line[index] == '\t'){
            if(template_index > 0 && template_line[template_index - 1] != ' ') 
                template_line[template_index] = ' ';
            else template_index--;
            index++;
            clean_white(line , &index);
            index--;
        }
        else if (line[index] == ',' && template_index > 0 && template_line[template_index - 1] == ' '){
            temp = template_line[template_index - 1];
            template_line[template_index - 1] = line[index];
            template_line[template_index] = temp;
        }   
        else if(template_index > 0 && template_line[template_index - 1] == ',') {
            template_line[template_index] = ' ';
            extra_spaces_counter++;
            if(extra_spaces_counter == max_extra) {
                max_extra *= 2;
                realloc(template_line , sizeof(char) * strlen(line) + END_LINE_CHARS + max_extra);
            }
	        template_index++;
            template_line[template_index] = line[index];
            clean_white(line , &index);  
        }
        else template_line[template_index] = line[index];
        index++; 
        template_index++; 
    }
    template_line[template_index] = '\n' , template_line[template_index + 1] = '\0';
    return template_line; 
}

void string_copy(char *first , char *copy) {
    int compare_index , path_len;
    path_len = strlen(first);
    for(compare_index = 0 ; compare_index <= path_len ; compare_index++)
            copy[compare_index] = first[compare_index];
}

void strings_letter_change(int letter_index , char new_letter , char* first , char **after_change)
{
    int path_len;
    path_len = strlen(first);
    *after_change = malloc(sizeof(char) * (path_len + 1)); /*&&&& was path_len + 1*/
    string_copy(first , *after_change);
    (*after_change)[letter_index] = new_letter;
}

bool is_null_file(FILE *f1 , FILE *f2 , char **first_file_name , char **second_file_name)
{
    bool null_file;
    null_file = false;
    if (f1 == NULL || f2 == NULL) {
        if(f1 == NULL) 
            printf("Error opening \"%s\" file\n" , *first_file_name);
        if(f2 == NULL) 
            printf("Error opening \"%s\" file\n" , *second_file_name);
        free_or_close(2 , 2 , f1 , f2);
        free_or_close(1 , 1 , *second_file_name);
        *second_file_name = NULL;
        null_file = true;
    }
    return null_file;
}





