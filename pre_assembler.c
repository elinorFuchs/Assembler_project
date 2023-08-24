#include "pre_assembler.h"

bool valid_pre= true;

typedef struct mcro_table {
    char** mcro_name;
    char** line;
    int *lines_max_inside_each_mcro;
    int *lines_count_inside_each_mcro;
    int index;
    int max_allocated_mcro_lines;
} mcro_table;
 
mcro_table_p create_mcro_table()
{
    int i;
    mcro_table* m1;

    m1 = safe_malloc(sizeof(mcro_table));
    m1->mcro_name = safe_malloc(sizeof(char*) * START_MAX_MACROS_NUM);
    m1->line = safe_malloc(sizeof(char*) * START_MAX_MACROS_NUM);
    m1->lines_max_inside_each_mcro = safe_malloc(sizeof(int) * START_MAX_MACROS_NUM);
    m1->lines_count_inside_each_mcro = safe_malloc(sizeof(int) * START_MAX_MACROS_NUM);
    m1->max_allocated_mcro_lines = START_MAX_MACROS_NUM;
    m1->index = 0;

    for (i = 0; i < START_MAX_MACROS_NUM; i ++) {
        m1->mcro_name[i] = safe_malloc(sizeof(char) * MAX_MCRO_NAME);
        m1->line[i] = safe_malloc(sizeof(char) * MAX_LINE_LEN * START_INSIDE_EACH_MCRO);
        m1->lines_max_inside_each_mcro[i] = START_INSIDE_EACH_MCRO;
        m1->lines_count_inside_each_mcro[i] = 0;
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

void mcro_table_line_increase(mcro_table_p m1 , int index) {
    int new_max_size;
    m1->lines_max_inside_each_mcro[index] *= 2;
    new_max_size = sizeof(char) * MAX_LINE_LEN * m1->lines_max_inside_each_mcro[index];
    safe_realloc(NULL , &(m1->line[index]) , NULL , new_max_size , 'c');
}

void mcro_table_count_increase(mcro_table_p m1) {
    int i , old_count;
    old_count = m1->max_allocated_mcro_lines;
    m1->max_allocated_mcro_lines *= 2;
    printf("new count: %d\n" , m1->max_allocated_mcro_lines);
    safe_realloc(&m1->mcro_name , NULL , NULL , sizeof(char*) * m1->max_allocated_mcro_lines , 'd');
    safe_realloc(&m1->line , NULL , NULL , sizeof(char*) * m1->max_allocated_mcro_lines , 'd');
    safe_realloc(NULL , NULL , &m1->lines_max_inside_each_mcro , sizeof(int) * m1->max_allocated_mcro_lines , 'i');
    safe_realloc(NULL , NULL , &m1->lines_count_inside_each_mcro , sizeof(int) * m1->max_allocated_mcro_lines , 'i');

    for (i = old_count ; i < m1->max_allocated_mcro_lines; i ++) {
        m1->mcro_name[i] = safe_malloc(sizeof(char) * MAX_MCRO_NAME);
        m1->line[i] = safe_malloc(sizeof(char) * MAX_LINE_LEN * START_INSIDE_EACH_MCRO);
        m1->lines_max_inside_each_mcro[i] = START_INSIDE_EACH_MCRO;
        m1->lines_count_inside_each_mcro[i] = 0;
        m1->mcro_name[i][0] = '\0';
        m1->line[i][0] = '\0';
    }
}

void is_mcro_line(mcro_table_p m1 , char* line , char **inside_mcro) {
    int index , exist_index;
    char *name_check , *line2;

    index = 0;

    clean_white(line , &index);
    line2 = safe_malloc(sizeof(char) * ((strlen(&line[index])) + 1));
    if(line2 == NULL) {
        printf("Memory allocation Failed\n");
        valid_pre = false;
    }
    line2 = strcpy(line2 , &line[index]);
    name_check = strtok(line2 , " \t\n\r\0\f");
    exist_index = is_exist_mcro(m1 , name_check);
    if(exist_index != -1 && is_all_white(&line[index + strlen(name_check)])) {
        *inside_mcro = safe_malloc(sizeof(char) * (1 + strlen(m1->line[exist_index])));
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
        printf("new mcro name: %s\n" , m1->mcro_name[m1->index]);
    }

    else if(method == 2) {
        printf("index is: %d\n" , m1->index);
        if(m1->index + 1 == m1->max_allocated_mcro_lines){
            mcro_table_count_increase(m1);
            printf("mcros max count increase in mcro %s to %d\n" , m1->mcro_name[m1->index] , m1->max_allocated_mcro_lines);
        } 
        if(m1->lines_count_inside_each_mcro[m1->index] + 1 == m1->lines_max_inside_each_mcro[m1->index]) {
            printf("was: %d of %d\n" , m1->lines_count_inside_each_mcro[m1->index] , m1->lines_max_inside_each_mcro[m1->index]);
            mcro_table_line_increase(m1 , m1->index);
            printf("increase size inside mcro %s to %d\n" , m1->mcro_name[m1->index] , m1->lines_max_inside_each_mcro[m1->index]);
        }
        printf("__________________\n");
        strcat(m1->line[m1->index] , line);
        m1->lines_count_inside_each_mcro[m1->index]++;
        printf("count in index is: %d\n" , m1->lines_count_inside_each_mcro[m1->index]);
    }
}

void free_mcro_table(mcro_table_p m1)
{
    int index;
    for (index = 0 ; index < m1->max_allocated_mcro_lines ; index++) {
        free_or_close(1 , 2 , m1->mcro_name[index] , m1->line[index]);  
    }
    safe_free(m1->lines_max_inside_each_mcro);
    safe_free(m1->lines_count_inside_each_mcro);
    safe_free(m1->mcro_name);
    safe_free(m1->line);
}

void str_copy(char **copy , char **original , bool start_no_spaces)
{
    int index;
    *copy = safe_malloc(sizeof(char) * (1 + strlen(*original)));
    index = 0;
    if(*copy != NULL) {
        if(start_no_spaces) {
            clean_white(*original , &index);
            strcpy(*copy , &(*original)[index]);
        }
        else strcpy(*copy , *original);
    }
    else{
        (printf("Error: malloc did not succeed."));
        valid_pre = false;
    }
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
    if(!is_all_white(&(curr_line[*index_copy]))) {
        printf("ERROR: forbidden macro declaration in line %d \"%s\". Only one word allowed.\n" , \
        line_number , strtok(*line_copy , "\n\0"));
        valid_pre = false;
    }
        
    else if(is_exist_mcro(m1 , *mcro_name) != -1) {
        printf("ERROR: macro named \"%s\" declared twice" , *mcro_name);
        valid_pre = false;
    }
    else if(!valid_mcro_name(*mcro_name)) {
        printf("ERROR: macro name \"%s\" is name of instruction\n" , *mcro_name);
        valid_pre = false;
    }
    else *mcro_flag = true;
    free_or_close(1 , 1 , *line_copy);
}

void line_inside_mcro_dec(char* curr_line , char* corrected_line , int* index , bool* mcro_flag , char* mcro_name , mcro_table_p m1){
    clean_white(curr_line , index);
    if(!strncmp(&curr_line[*index] , "endmcro" , ENDMCRO_LEN) && is_all_white(&curr_line[*index + ENDMCRO_LEN])){
        *mcro_flag = false; 
        mcro_name = NULL;
        (m1->index)++;
    } /* "endmcro" line */
    else {
        corrected_line = line_template(curr_line);
        if(corrected_line[*index] != '\0') {
            insert_mcro(m1 , NULL , corrected_line , 2);
        }
        free_or_close(1 , 1 , corrected_line);
    } /*line of content inside macro declaration */
}

void regular_line_add(char* curr_line , char* corrected_line , int* index , FILE* fptr2){
    if (!is_all_white(curr_line)) {
        corrected_line = line_template(&(curr_line[*index]));
        /*was in index "index - afik"*/
        if(corrected_line[0] != '\0')
            fputs(corrected_line , fptr2);
        free_or_close(1 , 1 , corrected_line);
    }  
}


int pre_as(char* path) {
    int index , index_copy , line_number;
    char *am_file_name , *curr_line , *mcro_name , *line_copy , *corrected_line , *inside_mcro , *curr_line_index;
    FILE *fptr , *fptr2;
    mcro_table* m1;
    bool mcro_flag;

    strings_letter_change(strlen(path) - 1 , 'm' , path , &am_file_name);
    line_copy = NULL , mcro_name = NULL , corrected_line = NULL;
    line_number = 0;
    mcro_flag = false;
    fptr = fopen(path , "r");
    fptr2 = fopen(am_file_name , "w");
    m1 = create_mcro_table();
    curr_line = safe_malloc(sizeof(char) * MAX_GET_LEN);
    
    if(is_null_file(fptr , fptr2 , &path ,  &am_file_name)) {
        free_or_close(1 , 1 , curr_line);
        return false;
    }
    
    while(fgets(curr_line , MAX_GET_LEN , fptr) != NULL) {
        index = 0 , line_number++;
        clean_white(curr_line , &index);
        if(strlen(curr_line) > MAX_LINE_LEN) {
            printf("Error: Line %d too long and will be omitted.\n" , line_number);
            valid_pre = false;
            continue;
        }  /*too long line check*/
        if(is_all_white(curr_line) || curr_line[index] == ';') 
            continue; /*empty or note line skip*/
        is_mcro_line(m1 , curr_line , &inside_mcro);
        if(inside_mcro != NULL) {
            fputs(inside_mcro , fptr2);
            free_or_close(1 , 1 , inside_mcro);
            continue; 
        } /*line to replace with a macro content case. was without mcro flag check */
        index_copy = index + MCRO_LEN;
        clean_white(curr_line , &index_copy);
        if(!strncmp(&curr_line[(index)] , "mcro" , MCRO_LEN) && !isspace(curr_line[index_copy]) &&
           !(strlen(&curr_line[index]) > MCRO_LEN && !isspace(curr_line[index + MCRO_LEN]))) 
        { 
            mcro_dec_line(&index, &index_copy , &mcro_flag, &curr_line_index, curr_line, &mcro_name , m1, &line_copy, line_number);
        } /* case: optional macro declaration line */
        else if(!mcro_flag)  
            regular_line_add(curr_line ,corrected_line , &index , fptr2);
        else line_inside_mcro_dec(curr_line , corrected_line , &index , &mcro_flag , mcro_name , m1);      
    }
    free_or_close(2 , 2 , fptr , fptr2);
    free_or_close(1 , 3 , am_file_name ,  curr_line , inside_mcro);
    free_mcro_table(m1);
    safe_free(m1);
    return valid_pre;
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
            safe_free(str);
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
  
void comma_switch(char* line , char* template_line , int* index , int* template_index){
    char temp;
    temp = template_line[*template_index - 1];
    template_line[*template_index - 1] = line[*index];
    template_line[*template_index] = temp;
}

void add_space_after_comma(char* line , char* template_line , int* index , int* template_index , int* extra_spaces_counter , int* max_extra){
        template_line[*template_index] = ' ';
        (*extra_spaces_counter)++;
        if(*extra_spaces_counter == *max_extra) {
            (*max_extra) *= 2;
            safe_realloc(NULL , &template_line , NULL , sizeof(char) * strlen(line) + END_LINE_CHARS + (*max_extra) , 'c');
        }
        (*template_index)++;
        template_line[*template_index] = line[*index];
        clean_white(line , index);  
}

void one_space_separate(char* line , char* template_line , int* index , int* template_index){
        if((*template_index) > 0 && template_line[*template_index - 1] != ' ') 
            template_line[*template_index] = ' ';
        else (*template_index)--;
        (*index)++;
        clean_white(line , index);
        (*index)--;
}


char* line_template(char* line) {
    int index , template_index, extra_spaces_counter , max_extra , template_len;
    char *template_line;

    index = 0 , template_index = 0 , extra_spaces_counter = 0 , max_extra = 2;
    template_len = strlen(line) + END_LINE_CHARS + max_extra;
    template_line = safe_calloc(template_len , sizeof(char));
    
    while(line[index] != '\n' && line[index] != '\0') {
        if(line[index] == ' ' || line[index] == '\t'){
            one_space_separate(line , template_line , &index , &template_index);
        }
        else if (line[index] == ',' && template_index > 0 && template_line[template_index - 1] == ' ' && strstr(line , ".string") == NULL){
            comma_switch(line , template_line , &index , &template_index);
        }   
        else if(template_index > 0 && template_line[template_index - 1] == ',' && strstr(line , ".string") == NULL) {
            add_space_after_comma(line , template_line , &index , &template_index , &extra_spaces_counter , &max_extra);
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
    *after_change = safe_malloc(sizeof(char) * (path_len + 1)); /*&&&& was path_len + 1*/
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
        if(second_file_name != NULL) {
            free_or_close(1 , 1 , *second_file_name);
            *second_file_name = NULL;
        }
        valid_pre = false;
        null_file = true;
    }
    return null_file;
}






