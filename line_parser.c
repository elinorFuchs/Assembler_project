//
// Created by elifu on 02/08/2023.
//

#include "line_parser.h"
#include "help_functions.h"

char delim[] =", \t\n\v\f";
op_args_method op_arg_arr [16] = {
        {lea,label,none,none,label,reg,none},
        {sub, immediate, label,reg,label,reg,none},
        {add,immediate, label,reg,label,reg,none},
        {cmp,immediate, label,reg,label,reg,none},
        {mov,immediate, label,reg,label,reg,none},
        {not,none,none,none,label,reg,none},
        {clr,none,none,none,label,reg,none},
        {inc,none,none,none,label,reg,none},
        {dec,none,none,none,label,reg,none},
        {jmp,none,none,none,label,reg,none},
        {bne,none,none,none,label,reg,none},
        {red,none,none,none,label,reg,none},
        {prn,none,none,none,immediate,label,reg},
        {jsr,none,none,none,label,reg,none},
        {stop,none,none,none,none,none,none},
        {rts,none,none,none,none,none,none}
};


line_data* create_line_data(char *line) {

    line_data* ld = malloc(sizeof(line_data));
    int index =0;
    char* word;
    char temp_line[MAX_LINE_SIZE];
    data_int_arr* d_arr;

    strcpy(temp_line, line);
    word = copy_word (temp_line,&index);

    if(word == NULL){/*it's an empty line*/
        ld->is_empty_line = true;
        return ld;
    }
    if(word[0] == ';'){/*it's a comment line - skip this line*/
        ld->is_comment = true;
        return ld;
    }
    /*check if it's a label*/
    if (is_label_def(word)) {
        strcpy(ld->label_name, word) ;
        word = copy_word (temp_line, &index);/*we want to continue check the next word after the label */
    }

    if(is_direction(word)) {

        direction_type d_t = which_data_type(word);
        if(d_t == d_string){

            string_parser(temp_line, ld, &index);
        }
        else if (d_t == d_data){
            data_parser(temp_line, d_arr, ld, &index);
        }
        else if (d_t == d_entry || d_t == d_extern){
            int count;
            char* arg;
            if(ld->label_name != NULL){/*there is label definition before .entry or .extern*/
                strcpy(ld->label_name ,NULL);
                printf("Warning: the is unnecessary label definition before .entry or .extern");
            }
            skip_spaces(&index,temp_line);/*index is pointing after the .entry or .extern*/
            count = args_counter(temp_line);
            if (count != 1) {
                printf(".entry or .extern line with more then one argument");
                return ld;
            }
            arg = copy_word(temp_line,&index);
            if(d_t == d_entry)
                strcpy(ld->l_type->dir->dc->entry,arg);
            else /*.extern*/
                strcpy(ld->l_type->dir->dc->extern_,arg);
        }
    }
    else if (which_instruction(word) != invalid){/*it's an instruction line*/
        ld->is_instruction = true;
        opcode code = which_instruction(word);
        inst_args_parser(temp_line, code, &index);/*check commas, count arguments, check if the address method is valid .do all the ld struct updates*/
    }
    else {/*not valid first word - get error*/printf("Error: not valid first word");}

    return ld;
}

bool is_label_def (char* word){

    int length = strlen(word),i = 0;
    if (length > MAX_LABEL_SIZE){
        /*printf("first word is too long for label or any other line type (data or instruction");*/
        return false;
    }
    if (!(isalpha(word[0]))){/*the first character of a label must be a-z or A-Z*/
        return false;
    }
    while (i < length - 1){/*the rest of the label must be numbers or letters, and the last character is ':'*/
        if(!(isalpha(word[i])) || !(isdigit(word[i])))
            return false;
        i++;
    }
    if(word[length-1] != ":")
        return false;
    word[length-1] = '\0';/*replace the ':' with null terminator to save the label name*/
    return  true;
}

bool is_valid_string(char* string_line) {
    /*skip spaces in beginning, search for quote, search for end quote check if there is something after*/
    size_t s_len = strlen(string_line);
    int i =0;
    skip_spaces(&i,string_line);
    if (s_len >= 2 && string_line[i] == '"') {/*there is opening quote, find the ending] == '"'*/
        i++;/*point after double quote*/
        while(string_line [i] != '"' && string_line[i] != '\0') {
            i++;
        }
        if(string_line[i] == '"'){/*found the ending quote*/
            (i)++;
            while (string_line[i] != '\0') {/*check if there is more invalid char after the string*/
                skip_spaces(&i, string_line);
                if (string_line[i] != '\0')
                    return false;
            }
            return true;
        }/*the lind ended and there is no ending quote*/
        return false;
    } else /*after .string there isn't opening quote*/
        return false;
}

bool string_parser(char* temp_line, line_data* ld, int* index){

    char* string_line = malloc(sizeof(char));
    char* args;

    strcpy(string_line,&temp_line[*index]);/*index is pointing to after .string*/

    if(is_valid_string(string_line)){/*include error message if not, skip spaces in beginning, search for quote, search for end quote check if there is something after*/
        args = copy_s_args(string_line);/*skip spaces in the beginning, find quote, copy till the ending quote:*/
        strcpy(ld->l_type->dir->dc->string,args);/*put the .string argument in line_data struct*/
    }
    else {/*is_string returned false and already know what is the error*/
        return false;
    }

    return true;
}

char *copy_s_args(char *line) {
    int i;
    int start = -1;
    int end = -1;

    for (i = 0; line[i] != '\0'; i++) {
        if (line[i] == '"' && start == -1) {
            start = i + 1; /* start point to the character after the opening quote*/
        }
        else if (line[i] == '"' && start != -1) {
            end = i;
            break;
        }
    }
    int length = end - start;
    char* s_content = (char*)malloc((length + 1) * sizeof(char));
    if (s_content == NULL) {
        return NULL;
    }
    strncpy(s_content,&line[start],length-1);
    s_content[length] = '\0';
    return s_content;
}

bool inst_args_parser(char *temp_line, opcode code,int *index)/*check commas, count arguments, check if the address method is valid, ld updates*/
{
    char* inst_line = malloc(sizeof(char));
    strcpy(inst_line,&temp_line[*index]);/*index is pointing to after the code name*/

    if(!(is_commas_valid(inst_line))){/*there is invalid comma*/
        printf("there is invalid comma");
        return false;
    }
    int args_count = args_counter (inst_line);
    if(!(a_count_as_expected(code, args_count))){/*the number of arguments is invalid*/
        printf("error args num is not valid.");
        return false;
    }

    op_args_method* op_add_method = 0;
    op_add_method = set_op_args(inst_line, op_add_method);

    if(!(is_args_as_expected(op_add_method,code))){/*the args address method is invalid*/
        printf("error args type is not valid.");
        return false;
    }
    return  true;
}

bool is_args_as_expected(op_args_method* op_args_to_validate,opcode code)
{
    int i;
    int j;
    int k;
    for (i = 0; i < 16; i++) {
        if (op_arg_arr[i].code == op_args_to_validate->code) {
            for (j = 0; j < VALID_ADDRSS_MTHDS; j++) {
                if ((op_arg_arr[i].src[j]) == op_args_to_validate->src) {
                    for (k = 0; k < VALID_ADDRSS_MTHDS; k++) {
                        if ((op_arg_arr[i].dest[k]) == op_args_to_validate->dest)
                            return true;
                    }
                }
            }
        }
    }
}

op_args_method* set_op_args(char *data_args, op_args_method* op_add_method) {

    char* first_arg = strtok(data_args, delim);
    char* second_arg = strtok(NULL, delim);

    if(!(is_inst_arg_valid(first_arg))){
        printf("instruction argument isn't valid");
        return op_add_method;
    }
    if (second_arg != NULL){
        if(!(is_inst_arg_valid(second_arg))){
            printf("instruction argument isn't valid");
            return op_add_method;
        }
        set_src_add(first_arg, op_add_method->src);
        set_dest_add(second_arg, op_add_method->dest);
    } else/*there is only one argument*/
        set_dest_add(first_arg, op_add_method->dest);
    return op_add_method;
}
bool is_inst_arg_valid(char* argument){

    if(is_register(argument) || is_label(argument) || is_immediate(argument))
        return true;
    else
        return false;

}

void set_src_add (char* arg, op_args_method* op_add_method) {
    if (is_immediate(arg)) {
        op_add_method->src[0] = immediate;
    } else if (is_label(arg)) {
        /*need to check if the label exist in lable table or extern table-so add this label string somewhere, then
         * in the first pass func will be created a label validation table, and second pass /or the end of first pass will check
         * if this label exist in the label table. also there will be a check that entry labels i difind in this table*/
        op_add_method->src[0] = label;
    } else if (is_register(arg)) {
        op_add_method->src[0] = reg;
    }
}
void set_dest_add (char* args, op_args_method* op_add_method) {
    if (is_immediate(args)) {
        op_add_method->dest[0] = immediate;
    } else if (is_label(args)) {
        op_add_method->dest[0] = label;
    } else if (is_register(args)) {
        op_add_method->dest[0] = reg;
    }
}

bool is_register(char *arg) {
    int i;
    char *registers = {"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"};
    for (i = 0; i < REGISTERS_NUM; i++) {
        if (strcmp(arg, &registers[i]) == 0)
            return true;
    }
    return false;
}

bool is_label(char *arg) {
    int length = strlen(arg),i = 0;
    if (length > MAX_LABEL_SIZE){
        /*printf("word is too long for label");*/
        return false;
    }
    if (!(isalpha(arg[0]))){/*the first character of a label must be a-z or A-Z*/
        return false;
    }
    while (i < length){/*the rest of the label must be numbers or letters*/
        if(!(isalpha(arg[i])) || !(isdigit(arg[i])))
            return false;
        i++;
    }
    return true;
}

bool is_immediate(char *arg) {
    if (arg == NULL || arg[0] == '\0') {
        return false;
    }
    int i = 0;
    if (arg[i] == '+' || arg[i] == '-') {
        i++;
    }
    while (arg[i] != '\0') {
        if (!isdigit(arg[i])) {
            return false;
        }
        i++;
    }
}

bool a_count_as_expected(opcode op, int args_c) {

    if (!(is_A_group(op) && args_c == 2 || (is_B_group(op) && args_c == 1) || is_C_group(op) && args_c == 0)) {
        return false;
    }
    else
        return true;
}

bool data_parser(char* temp_line, data_int_arr* d_arr, line_data* ld, int* index){

    char* data_line = malloc(sizeof(char));
    strcpy(data_line,&temp_line[*index]);/*index is pointing to after .data*/

    if(is_valid_data(data_line)){/*include error message if not, skip spaces in beginning,check commas*/
        d_arr = copy_d_args(data_line);/*skip spaces in the beginning, creat int array*/
        ld->l_type->dir->dc->data = d_arr;/*put the .data argument as an array in line_data struct-*/
    }
    else {/*is_data returned false and already know what is the error*/
        return false;
    }
    return true;
}

bool is_more_args(char* line, int* index) {
    return false;
}

bool is_A_group(opcode code){
    if(code == lea || code == sub  || code == add || code == cmp || code == mov)
        return  true;
    return false;
}
bool is_B_group(opcode code){
    if(code == not || code == clr  || code == inc || code == dec || code == jmp
       || code == bne ||code == red || code == prn || code == jsr)
        return  true;
    return false;
}
bool is_C_group(opcode code){
    if(code == rts || code == stop)
        return  true;
    return false;
}

bool is_valid_data(char* data_line){
    int i;
    if(is_commas_valid(data_line)){
        for(i = 0; i < strlen(data_line); i++){
            skip_spaces(&i, data_line);
            if(!(isdigit(data_line[i]) || data_line[i] == ',')){
                return false;
            };
        }
        return true;/*the arguments are all integers or commas*/
    }
    else
        return false;/*there is invalid commas*/
}

data_int_arr* copy_d_args(char* data_line) {
    int* d_args = NULL;
    int i, j = 0, data_size = 10;

    d_args = (int*)malloc(data_size * sizeof(*d_args));
    if (d_args == NULL) {
        printf("Memory allocation failed. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    for (i = 0; data_line[i];) {
        skip_spaces(&i, data_line);
        skip_commas(&i, data_line);

        int num = string_to_sign_int(data_line, &i);
        d_args[j] = num;
        j++;

        skip_spaces(&i, data_line);
        skip_commas(&i, data_line);
        if (j >= data_size) {
            data_size *= 2;
            int* temp_d_args = (int*)realloc(d_args, data_size * sizeof(*d_args));
            if (temp_d_args == NULL) {
                printf("Memory reallocation failed. Exiting...\n");
                free(d_args);
                exit(EXIT_FAILURE);
            }
            d_args = temp_d_args;
        }
    }
    data_int_arr* d_arr = (data_int_arr*)malloc(sizeof(data_int_arr));
    if (d_arr == NULL) {
        printf("Memory allocation failed. Exiting...\n");
        free(d_args);
        exit(EXIT_FAILURE);
    }
    d_arr->data_args = d_args;
    d_arr->size = j;
    return d_arr;
}

direction_type which_data_type(char* word){
    direction_type dt;
    int i;
    char* directions_string [DIRECTION_NUM] = {".string",".data" , "entry" , ".extern"};

    for (i = 0; i < DIRECTION_NUM; i++) {
        if (0 == strcmp(directions_string[i], word)) {
            switch (i) {
                case 0:
                    dt = d_string;
                    break;
                case 1:
                    dt = d_data;
                    break;
                case 2:
                    dt = d_entry;
                    break;
                case 3:
                    dt = d_extern;
                    break;
                default:
                    break;
            }
        }
    }
    if((void *) dt == NULL){
        printf("\nUndefined command name\n");
    }
    return dt;
}

bool is_direction (char* word){/*זיהוי אחד מאופציות ההנחיה*/
    int i;
    char* directions_string [DIRECTION_NUM] = {".string",".data" , ".entry" , ".extern"};
    if (word == NULL){/*what exactly are the cases this is null*/
        return false;
    }
    for (i = 0; i<DIRECTION_NUM; i++){
        if (strcmp(directions_string[i],word) == 0)
            return true;
        else if (strncmp(directions_string[i], word, strlen(directions_string[i])) == 0) {
            if (word[strlen(word)-1] == ',') {
                printf("Error: illegal comma at the end of the direction word");
            }
            return false;
        }
    }
    return false;
}

opcode which_instruction(char* word){
    return  true;
}

bool is_instruction(char* word) {
    return true;
}





/*check if argument method is valid
 * if (op_add_method->code == mov || op_add_method->code == add || op_add_method->code == sub){
        if(!(op_add_method->src[0] == immediate || op_add_method->src[0] == label || op_add_method->src[0] == reg)) {
            printf("error the src method is invalid");
            return false;
        }
        if(!(op_add_method->dest[0] == label || op_add_method->dest[0] == reg)){
            printf("error the dest method is invalid");
            return false;
        }
    }
    else if (op_add_method->code == not || op_add_method->code == clr || op_add_method->code == inc
    || op_add_method->code == dec || op_add_method->code == jmp || op_add_method->code == bne
    || op_add_method->code == red || op_add_method->code == jsr){
        if(!(op_add_method->src[0] == none)) {
            printf("error the src method is invalid- should be none");
            return false;
        }
        if(!(op_add_method->dest[0] == label || op_add_method->dest[0] == reg)){
            printf("error the dest method is invalid -should be label or register");
            return false;
        }
    }
    else if (op_add_method->code == cmp){
        if(!(op_add_method->src[0] == immediate || op_add_method->src[0] == label || op_add_method->src[0] == reg)) {
            printf("error the src method is invalid");
            return false;
        }
        if(!(op_add_method->dest[0] == immediate || op_add_method->dest[0] == label || op_add_method->dest[0] == reg)){
            printf("error the dest method is invalid -shold be label or register");
            return false;
        }
    }
    else if((op_add_method->code == lea)){
        if(!(op_add_method->src[0] == label)) {
            printf("error the src method is invalid- should be label");
            return false;
        }
        if(!(op_add_method->dest[0] == label || op_add_method->dest[0] == reg)){
            printf("error the dest method is invalid -shold be label or register");
            return false;
        }
    }
    else if((op_add_method->code == prn)){
        if(!(op_add_method->src[0] == none)) {
            printf("error the src method is invalid- should be none");
            return false;
        }
        if(!(op_add_method->dest[0] == immediate|| op_add_method->dest[0] == label || op_add_method->dest[0] == reg)){
            printf("error the dest method is invalid -shold be label or register");
            return false;
        }
    }

    else if((op_add_method->code == rts || op_add_method->code == stop)){
        if(!(op_add_method->src[0] == none)) {
            printf("error the src method is invalid- should be none");
            return false;
        }
        if(!(op_add_method->dest[0] == none)){
            printf("error the dest method is invalid -should be none");
            return false;
        }
    }*/



