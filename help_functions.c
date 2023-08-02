//
// Created by elifu on 02/08/2023.
//

#include "help_functions.h"

char* copy_word(const char* line, int* index) {

    skip_spaces(index, line);
    int length = 0;
    int length_index = *index;

    while (!(isspace(line[length_index]) || line[length_index] == '\0' || line[length_index] == EOF )) {
        length++;
        length_index++;
    }
    char* word = (char*)malloc((length + 1) * sizeof(char));
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
        if (!(isspace(line[i]))) {
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

bool is_commas_valid(char* args){

    int length = strlen(args);
    int i = 0;
    bool isValid = false;
    bool hasArgument = false;
    bool previousIsComma = false; /* Track if the previous character was a comma*/

    /* Check for comma at the beginning*/
    if (args[i] == ',') {
        printf("\nIllegal comma\n");
        isValid = false;
        return isValid;
    }
    for (; i < length; i++) {
        if (args[i] == ',') {
            /* Check if there are consecutive commas or a comma at the end*/
            if(previousIsComma){
                isValid = false;
                printf("\nMultiple consecutive commas\n");
                break;
            }
            else if((i + 1) == (length-1)){
                printf("\nExtraneous text after end of command\n");
                isValid = false;
                break;
            }
            else if (!hasArgument)  {
                printf("\nIllegal comma\n");
                isValid = false;
                break;
            }

            previousIsComma = 1;
        }
        else if (!isspace(args[i])) {
            if(hasArgument == true && isspace(args[i-1]) && previousIsComma == false){
                printf("\nMissing comma\n");
                isValid = false;
                break;
            }
            else{
                hasArgument = true;
                isValid = true; /* Mark as valid when encountering the first non-space character*/
                previousIsComma = false;
            }
        }
    }
    return isValid;
}
