/*Created by elifu on 02/08/2023.*/
#include "line_parser.h"


char* inst_Arr[16] = {"mov","cmp","add","sub","not","clr","lea","inc",
                      "dec","jmp","bne","red","prn","jsr","rts","stop"};

FILE* safe_fopen (const char *filename, const char *mode, char* suffix){
    char* new_file_name = safe_malloc(sizeof (char) * (strlen(filename) + strlen(suffix) + 1));
    FILE* fptr;
    strcpy(new_file_name,filename);
    strcat(new_file_name, suffix);
    fptr = fopen(new_file_name,mode);
    if (fptr == NULL)
        perror("error");
    safe_free(new_file_name);
    return fptr;
}
char* path_string (const char *filename,char* suffix){

    char* new_file_name = safe_malloc(sizeof (char) * (strlen(filename) + strlen(suffix) + 1));
    strcpy(new_file_name,filename);
    strcat(new_file_name, suffix);
    return new_file_name;


}
char* copy_word(const char* line, int* index) {

    skip_spaces(index, line);
    skip_commas(index, line);
    int length = 0;
    int length_index = *index;

    while (!(isspace(line[length_index]) ||  line[length_index] == ',' || line[length_index] == '\0' || line[length_index] == EOF )) {
        length++;
        length_index++;
    }
    char* word = (char*)safe_malloc((length + 1) * sizeof(char));
    if (word == NULL) {
        return NULL;
    }
    strncpy(word, &line[*index], length);
    word[length] = '\0';
    *index = length_index;/*index will point to after the word*/

    return word;
}

int args_counter (char* line){
    int count = 0;
    int i;
    bool in_word = false;

    for (i = 0; line[i] != '\0'; i++) {
        if (!(isspace(line[i]) || line[i] == ',' )) {
            if (!in_word) {
                count++;
                in_word = true;
            }
        }
        else {/*there is a space or comma character*/
            in_word = false;
        }
    }
    return count;
}

void skip_spaces (int* index,const char* line){
    while (isspace(line[*index]))
        (*index)++;
}

void skip_commas(int* index, const char* line) {
    while (line[*index] == ',') {
        (*index)++;
    }
}

void bool_safe_fopen(FILE** fptr , char* path , bool* valid){
    (*valid) = true;
    *fptr = fopen(path , "w");
    if(*fptr == NULL) {
        printf("error openning %s.\n" , path);
        (*valid) = false;
    }
}

void safe_realloc(char *** c_3ptr , char** c_2ptr , int** i_ptr , size_t size , char method) {
    void* temp_ptr;
    switch(method){
        case 'd':
            temp_ptr = realloc(*c_3ptr , size);
            break;
        case 'c':
            temp_ptr = realloc(*c_2ptr , size);
            break;
        case 'i':
            temp_ptr = realloc(*i_ptr , size);
            break;
        default: 
            printf("Wrong method called.\n");
            exit(1);
    }
    if (temp_ptr == NULL) {
        printf("Memory reallocation failed.\n");
        exit(1);
    }
    if(method == 'd')
        *c_3ptr = temp_ptr;
    else if(method == 'c')
        *c_2ptr = temp_ptr;
    else *i_ptr = temp_ptr;
}

int string_to_sign_int(char* data_line, int* index) {
    int sign = 1;
    if (data_line[*index] == '-') {
        sign = -1;
        (*index)++;
    } else if (data_line[*index] == '+') {
        (*index)++;
    }
    int num = 0;
    while (isdigit(data_line[*index])) {
        num = num * 10 + (data_line[*index] - '0');
        (*index)++;
    }
    return num * sign;
}


void* safe_calloc(size_t num_elements, size_t element_size) {
    void* ptr = calloc(num_elements , element_size);
    if (ptr == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    return ptr;
}

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    return ptr;
}
void resize_int_arr(int** arr, int* size) {
    *size *= 2;
    int* temp_arr = (int*)realloc(*arr, (*size) * sizeof(int));
    if (temp_arr == NULL) {
        printf("Memory reallocation failed.\n");
        free(*arr);
        exit(1);
    }
    *arr = temp_arr;
}

void safe_free_double_p(void **ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}

void safe_free(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
        ptr = NULL;
    }
}




