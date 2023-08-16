/*Created by elifu on 02/08/2023.*/

#include "line_parser.h"




/*char* rgstrs[8] = {"@r0","@r1","@r2", "@r3", "@r4", "@r5", "@r6", "@r7"};*/

op_args_mthd op_args_arr [16] = {
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
/*add comment if empty line or comment line am file already deleted it.*/

line_data* create_line_data(char *line) {

    line_data* ld = safe_malloc(sizeof(line_data));
    *ld = (line_data){0};
    int index =0;
    char* word;
    char temp_line[MAX_LINE_SIZE];

    strcpy(temp_line, line);
    word = copy_word (temp_line,&index);
    ld->ei = SUCCESS;

    /*check if it's a label*/
    if (is_label_def(word, ld)) {
        ld->is_label_def = true;
        strcpy(ld->label_name, word) ;
        word = copy_word (temp_line, &index);/*we continue check the next word after the label definition*/
    }
    if(is_direction(word,ld)) {
        ld->is_direction = true;
        ld->dir = (direction *) safe_malloc(sizeof(direction));
        ld->dir->d_content = (direction_content *) safe_malloc(sizeof(direction_content));
        direction_type d_t = which_data_type(word);

        if (d_t == d_string) {
            ld->dir->d_type = d_string;
            string_parser(temp_line, ld, &index);
            if (0 == ld->dir->d_content->string->str_len)
                ld->ei = NO_CONTENT_DIRECTION;
        } else if (d_t == d_data) {
            ld->dir->d_type = d_data;
            data_parser(temp_line, ld, &index);
            if (ld->dir->d_content->d_arr->data_arr_size == 0) {
                ld->ei = NO_CONTENT_DIRECTION;
                ld->dir->dir_line_keeper = 0;
            } else
                ld->dir->dir_line_keeper = ld->dir->d_content->d_arr->data_arr_size;/*keep lines as the size of the data array*/
        }
        else if (d_t == d_entry || d_t == d_extern) {
                char *args;
                args = (char *) safe_malloc(MAX_LINE_SIZE * sizeof(char));
                if (ld->is_label_def) {/*there is label definition before .entry or .extern*/
                    strcpy(ld->label_name, "");/*delete the label definition before .extern, it's meaningless */
                    ld->ei = UNNECESSARY_LABEL;
                }
                skip_spaces(&index, temp_line);/*index is pointing after the .entry or .extern*/
                strcpy(args, &temp_line[index]);
                if (d_t == d_entry) {
                    ld->dir->d_type = d_entry;
                    set_entry_labels(ld, args);/*put the args in string arr */
                    if (ld->dir->d_content->en_arr->entry == NULL)
                        ld->ei = NO_CONTENT_DIRECTION;
                } else {
                    ld->dir->d_type = d_extern;
                    set_extern_labels(ld, args);/*put the args in string arr */
                    if (ld->dir->d_content->ex_arr->extern_ == NULL)
                        ld->ei = NO_CONTENT_DIRECTION;
                }
            } else {/*Invalid data type*/
                ld->ei = UNDEFINED_DATA_NAME;
            }

        } else if (which_instruction(word) != invalid) {/*it's an instruction line*/
            ld->is_instruction = true;
            opcode code = which_instruction(word);
            ld->inst = safe_malloc(sizeof(instruction));
            ld->inst->op_args_type = safe_malloc(sizeof(op_args_mthd));
            ld->inst->op_args_type->code = code;

            inst_args_parser(temp_line, code, &index,ld);
            /*line keeper update*/
            int keeper_count = 1;/*at least 1 for the code*/
            address_type s_type = ld->inst->op_args_type->src[0];
            address_type d_type = ld->inst->op_args_type->dest[0];

            if (s_type == none && d_type == none){
                ld->inst->inst_line_keeper = keeper_count;
                return ld;
            }
            else if((s_type == reg && d_type == reg) || (s_type == none) || d_type == none){/*only one more line needed*/
                keeper_count++;
                ld->inst->inst_line_keeper = keeper_count;
                return ld;
            } else{
                keeper_count += 2;/*two more lines needed*/
                ld->inst->inst_line_keeper = keeper_count;
                return ld;
            }
        }

        else { /*not valid first word - get error*/
        ld->ei = INVALID_FIRST_WORD;
        }
    return ld;
}


bool is_label_def(char *word, line_data *ld) {

    int length = strlen(word),i = 0;
    if (length > MAX_LABEL_SIZE){
        printf("Error: first word is too long for label definition / direction or instruction.\n");
        return false;
    }
    if (!(isalpha(word[0]))){/*the first character of a label must be a-z or A-Z.
    *that's covers cases of saved word as registers or directions which begin with '.'*/
        return false;
    }
    while (i < length - 1){/*the rest of the label must be numbers or letters, and the last character is ':'*/
        if(!(isalpha(word[i])) && !(isdigit(word[i])))
            return false;
        i++;
    }
    if(strcmp(&word[length-1],":") != 0 )
        return false;
    if(is_instruction(word)) {/*label can't be saved word as opcode*/
        ld->ei = OPCODE_LABEL_NAME;
        return false;
    }
    word[length-1] = '\0';/*replace ':' with null terminator*/
    return  true;
}

bool is_valid_string(char *string_line, line_data *ld) {
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
                if (string_line[i] != '\0') {
                    ld->ei = CHAR_AFTER_QUOTE;
                    return false;
                }
            }
            return true;
        }/*the lind ended and there is no ending quote*/
        return false;
    } else /*after .string there isn't opening quote*/
        return false;
}

bool string_parser(char* temp_line, line_data* ld, int* index){

    char* string_line;
    char* args;

    string_line = safe_malloc(strlen(temp_line) + 1);
    ld->dir->d_content->string = (str_d*)safe_malloc(sizeof(str_d));
    ld->dir->d_content->string->string = (char*)safe_malloc(strlen(temp_line) + 1);

    strcpy(string_line,&temp_line[*index]);/*index is pointing to after .string*/
    ld->dir->d_content->string->str_len = 0;

    if(is_valid_string(string_line, ld)){/*include error message if not, search for quotes*/
        args = copy_s_args(string_line);/*skip spaces in the beginning, find quote, copy till the ending quote:*/

        strcpy(ld->dir->d_content->string->string, args);/*put the .string argument in line_data struct*/
        ld->dir->d_content->string->str_len = strlen(args);
        ld->dir->dir_line_keeper =strlen(args)+1;
   }
    else /*not a valid string*/
        return false;
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
    char* s_content = (char*)safe_malloc((length + 1) * sizeof(char));
    if (s_content == NULL) {
        return NULL;
    }
    strncpy(s_content, &line[start], length);
    s_content[length] = '\0';
    return s_content;
}

bool inst_args_parser(char *temp_line, opcode code, int *index, line_data *ld)/*check commas, count arguments, check if the address method is valid, ld updates*/
{
    char* inst_line = safe_malloc(sizeof(char) * MAX_LINE_SIZE);

    strcpy(inst_line,&temp_line[*index]);/*index is pointing to after the code name*/

    if(!(is_commas_valid(inst_line,ld))){/*there is invalid comma*/
        return false;
    }
    int args_count = args_counter (inst_line);
    if(!(a_count_as_expected(code, args_count))){/*the number of arguments is invalid*/
        ld->ei = INVALID_ARGS_AMOUNT;
        return false;
    }
    if(!(set_op_args(inst_line,ld))){
        return false;
    }
    if(!(is_args_as_expected(ld->inst->op_args_type))){/*-inc debug returned false expected true*/
        ld->ei = INVALID_ARGS_ADD_METHOD;
        return false;
    }
    return  true;
}

bool is_args_as_expected(op_args_mthd* op_args_to_validate){
    int i;
    int j;
    int k;
    for (i = 0; i < 16; i++) {
        if (op_args_arr[i].code == op_args_to_validate->code) {
            for (j = 0; j < VALID_ADDRSS_MTHDS; j++) {
                if(op_args_arr[i].src[j] == op_args_to_validate->src[0]){
                    for (k = 0; k < VALID_ADDRSS_MTHDS; k++) {
                        if ((op_args_arr[i].dest[k]) == op_args_to_validate->dest[0])
                            return true;
                    }
                }
            }
            return false;
        }
    }
    return false;
}

bool set_op_args(char* data_args, line_data* ld) {
    
    char delim[] =", \t\n\v\f\r";
    char *first_arg = strtok(data_args,delim );
    char *second_arg = strtok(NULL, delim);

    if (first_arg == NULL) {/*happen only when there is no args at all*/
        ld->inst->op_args_type->src[0] = none;
        ld->inst->op_args_type->dest[0] = none;
        return true;
    }
    else if (!(is_inst_arg_valid(first_arg, ld))) {
       /* ld->ei = INVALID_INST_ARGS;*/
        return false;
    }
    if (second_arg != NULL) {
        if (!(is_inst_arg_valid(second_arg, ld))) {
            /*ld->ei = INVALID_INST_ARGS;*/
            return false;
        }
        set_src_add(first_arg, ld);
        set_dest_add(second_arg, ld);
    }
    else/*there is only one argument, so it's the dest definition*/{
        ld->inst->op_args_type->src[0] = none;
        set_dest_add(first_arg, ld);
        return true;
    }
    return true;
}
    bool is_inst_arg_valid(char *argument, line_data *ld) {

        if (is_register(argument, ld) || is_label(argument, ld) || is_immediate(argument, ld))
            return true;
        else
            return false;
    }

    void set_src_add(char *arg, line_data *ld) {

        if (is_immediate(arg, ld)) {
            ld->inst->op_args_type->src[0] = immediate;
            ld->inst->src_name = arg;
        } else if (is_label(arg, ld)) {
            ld->inst->op_args_type->src[0] = label;
        } else if (is_register(arg, ld)) {
            ld->inst->op_args_type->src[0] = reg;
        }
        ld->inst->src_name = arg;
    }
    void set_dest_add(char *args, line_data *ld) {

        if (is_immediate(args, ld)) {
            ld->inst->op_args_type->dest[0] = immediate;
        } else if (is_label(args, ld)) {
            ld->inst->op_args_type->dest[0] = label;
        } else if (is_register(args, ld)) {
            ld->inst->op_args_type->dest[0] = reg;
        }
        ld->inst->dest_name = args;
    }

    bool is_register(char *arg, line_data* ld) {
        int i;
        char *registers[REGISTERS_NUM] = {"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"};
        for (i = 0; i < REGISTERS_NUM; i++) {
            if (strcmp(arg, registers[i]) == 0)
                return true;
        }
        return false;
    }

    bool is_label(char *arg, line_data* ld) {/*add check if it's a saved word*/
        int length = strlen(arg), i = 0;

        if (is_instruction(arg)) {/*opcode is saved word and can't be a label*/
            return false;
            ld->ei = OPCODE_LABEL_NAME;
        }
        if (length > MAX_LABEL_SIZE) {
            /*ld->ei = LONG_LABEL;*/
            return false;
        }
        if (!(isalpha(arg[0]))) {
            /*ld->ei = LABEL_FIRST_CHAR;*/
            /*the first character of a label must be a-z or A-Z*/
            return false;
        }
        while (i < length) {/*the rest of the label must be numbers or letters*/
            if (!(isalpha(arg[i])) && !(isdigit(arg[i])))
             /*   ld->ei = INVALID_LABEL_NAME;*/
                return false;
            i++;
        }
        return true;
    }

    bool is_immediate(char *arg, line_data* ld) {
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
        int j = 0;
        int num = string_to_sign_int(arg, &j);
        if(!(num <= 511 && num >= -512)){
            ld->ei = IMMEDIATE_RANGE;
            return false;
        }

        return true;
    }

    bool a_count_as_expected(opcode op, int args_c) {

        if (!((is_A_group(op) && args_c == 2) || (is_B_group(op) && args_c == 1) || (is_C_group(op) && args_c == 0))) {
            return false;
        } else
            return true;
    }

    bool data_parser(char *temp_line, line_data *ld, int *index) {

        char *data_line = safe_malloc(sizeof(char) * MAX_LINE_SIZE);

        strcpy(data_line, &temp_line[*index]);/*index is pointing to after .data*/
        if (!is_valid_data(data_line,ld)) {/*include error message if not, skip spaces in beginning,check commas*/
            return false;
        }
        copy_d_args(data_line, ld);/*skip spaces in the beginning, creat int array*/
        return true;
    }

    bool is_A_group(opcode code) {
        if (code == lea || code == sub || code == add || code == cmp || code == mov)
            return true;
        return false;
    }
    bool is_B_group(opcode code) {
        if (code == not || code == clr || code == inc || code == dec || code == jmp
            || code == bne || code == red || code == prn || code == jsr)
            return true;
        return false;
    }
    bool is_C_group(opcode code) {
        if (code == rts || code == stop)
            return true;
        return false;
    }

    bool is_valid_data(char *data_line, line_data* ld) {
        int i;

        if (!(is_commas_valid(data_line,ld)))
            return false;
        for (i = 0; i < strlen(data_line); i++) {
            skip_spaces(&i, data_line);
            skip_commas(&i,data_line);
            skip_spaces(&i, data_line);

            if (data_line[i] == '-' || data_line[i] == '+') {
                i++;
            }
            if (!(isdigit(data_line[i]))) {
                    if (data_line[i] == '\0' || data_line[i] == '\n')
                        return true;
                    return false;
            }
        }
        i = 0;
        int data_num =string_to_sign_int(data_line, &i);
        if (!(data_num >= -2048 && data_num <= 2047)){
            ld->ei = DATA_RANGE;
            return false;
        }
            return true;/*the arguments are all  sign integers or commas*/
    }

    void copy_d_args(char* data_line, line_data* ld) {
        int *d_args;
        int i, j = 0, data_size = 10;

        d_args = (int*)safe_malloc(data_size * sizeof(*d_args));

        for (i = 0; data_line[i];) {
            skip_spaces(&i, data_line);
            skip_commas(&i, data_line);

            int num = string_to_sign_int(data_line, &i);
            d_args[j] = num;
            j++;

            skip_spaces(&i, data_line);
            skip_commas(&i, data_line);

            if (j >= data_size) /*reallocate memory*/
                resize_int_arr(&d_args, &data_size);/*maybe need to update data size?*/
        }
        ld->dir->d_content->d_arr = (data_arr*)safe_malloc(j * sizeof(data_arr));
        ld->dir->d_content->d_arr->data_arr = (int*)safe_malloc(j * sizeof(int));
        ld->dir->d_content->d_arr->data_arr = 0;

        ld->dir->d_content->d_arr->data_arr = d_args;
        if(j == 1 && ld->dir->d_content->d_arr->data_arr[0] == 0 ) {/*if there is not data after .data*/
            ld->dir->d_content->d_arr->data_arr_size = 0;
        }
        else
            ld->dir->d_content->d_arr->data_arr_size = j;
        /*print for debugging
        int k;
        for ( k = 0; k < j; ++k) {
            printf("%d\n", ld->dir->d_content->d_arr->data_arr[k]);
        }*/
        return;
    }

    direction_type which_data_type(char *word) {
        direction_type dt = invalid_data;
        int i;
        char* directions_string[DIRECTION_NUM] = {".string", ".data", ".entry", ".extern"};

        for (i = 0; i < DIRECTION_NUM; i++) {
            if (strcmp(directions_string[i], word) == 0) {
                dt = i;
                break;
            }     
        }
        return dt;
    }

    bool is_direction(char *word, line_data* ld) {
            int i;
            char *directions_string[DIRECTION_NUM] = {".string", ".data", ".entry", ".extern"};

            for (i = 0; i < DIRECTION_NUM; i++) {
                if (strcmp(directions_string[i], word) == 0)
                    return true;
                else if (strncmp(directions_string[i], word, strlen(directions_string[i])) == 0) {
                    if (word[strlen(word)] == ',') {
                        ld->ei = COMMA_AT_END_OF_D_WORD;
                         return false;}
                }
            }
            return false;
    }

    opcode which_instruction(char *word) {
            opcode code_name = invalid;
            int i;

            for (i = 0; i < INST_SIZE; i++) {
                if (0 == strcmp(inst_Arr[i], word)) {
                    switch (i) {
                        case 0:
                            code_name = mov;
                            break;
                        case 1:
                            code_name = cmp;
                            break;
                        case 2:
                            code_name = add;
                            break;
                        case 3:
                            code_name = sub;
                            break;
                        case 4:
                            code_name = not;
                            break;
                        case 5:
                            code_name = clr;
                            break;
                        case 6:
                            code_name = lea;
                            break;
                        case 7:
                            code_name = inc;
                            break;
                        case 8:
                            code_name = dec;
                            break;
                        case 9:
                            code_name = jmp;
                            break;
                        case 10:
                            code_name = bne;
                            break;
                        case 11:
                            code_name = red;
                            break;
                        case 12:
                            code_name = prn;
                            break;
                        case 13:
                            code_name = jsr;
                            break;
                        case 14:
                            code_name = rts;
                            break;
                        case 15:
                            code_name = stop;
                            break;
                        default:
                            break;
                    }
                    if (code_name != invalid)
                        break;
                }
            }
            return code_name;
    }
    bool is_instruction(char *word) {
            int i;
            for (i = 0; i < INST_SIZE; i++) {
                if (strcmp(word, inst_Arr[i]) == 0)
                    return true;
            }
            return false;
    }
void set_entry_labels(line_data* ld, char* args){

        int count, i, k;
        char* label_name;
        char** entry_label_arr;
        
        count = args_counter(args);
        entry_label_arr = (char**)safe_malloc(count*(sizeof (char*)));

        for (i = 0, k = 0; i < count; i++) {
            skip_spaces(&k,args);
            skip_commas(&k, args);
            label_name = copy_word(args, &k);

            if(label_name == NULL)
                break;
            if(is_label(label_name, ld)) {
                entry_label_arr[i] = (char*)safe_malloc(strlen((label_name))+1);
                strcpy(entry_label_arr[i], label_name);
            } /*else error*/
        }
        ld->dir->d_content->en_arr = (entry_arr*) safe_malloc(sizeof(entry_arr));
        ld->dir->d_content->en_arr->entry = (char**)safe_malloc(count * sizeof(char*));

        for (i = 0; i < count; i++) {
            ld->dir->d_content->en_arr->entry[i] = (char*)safe_malloc(strlen(entry_label_arr[i])+1);
            strcpy(ld->dir->d_content->en_arr->entry[i], entry_label_arr[i]);
        }
        ld->dir->d_content->en_arr->en_size = count;
        /*DEBUG PRINT*/
        for (i = 0; i < count; i++) {
            printf("%s \n", ld->dir->d_content->en_arr->entry[i]);
        }
        free(entry_label_arr);
        return;
}
 void set_extern_labels(line_data * ld, char* args){
     int count, i, k;
     char* extrn_name;
     char** e_arr;

     count = args_counter(args);
     e_arr = (char**)safe_malloc(count*(sizeof (char*)));

     for (i = 0, k = 0; i < count; i++) {
         skip_spaces(&k,args);
         skip_commas(&k, args);
         extrn_name = copy_word(args, &k);

         if(extrn_name == NULL)
             break;
         if(is_label(extrn_name, ld)) {
             e_arr[i] = (char*)safe_malloc(strlen((extrn_name))+1);
             strcpy(e_arr[i], extrn_name);
         } else
             return;
     }
     ld->dir->d_content->ex_arr = (extern_arr *) safe_malloc(sizeof(extern_arr ));
     ld->dir->d_content->ex_arr->extern_ = (char**)safe_malloc(count * sizeof(char*));
     for (i = 0; i < count; i++) {
         ld->dir->d_content->ex_arr->extern_[i] = (char*)safe_malloc(strlen(e_arr[i])+1);
         strcpy(ld->dir->d_content->ex_arr->extern_[i], e_arr[i]);
     }
     /*DEBUG PRINT*/
     for (i = 0; i < count; i++) {
         printf("%s \n", ld->dir->d_content->ex_arr->extern_[i]);
     }
     ld->dir->d_content->ex_arr->ex_size = count;

     free(e_arr);
     return;
 }
bool is_commas_valid(char* args, line_data * ld) {

    size_t length = strlen(args);
    int i = 0;
    bool isValid = false;
    bool hasArgument = false;
    bool previousIsComma = false; /* Track if the previous character was a comma*/
    skip_spaces(&i, args);
    if (args[i] == '\0') {/*it's an empty line*/
        isValid = true;
        return isValid;
    }
    /* Check for comma at the beginning*/
    if (args[i] == ',') {
        ld->ei = ILLEGAL_COMMA;
        isValid = false;
        return isValid;
    }
    for (; i < length; i++) {
        if (args[i] == ',') {
            /* Check if there are consecutive commas or a comma at the end*/
            if (previousIsComma) {
                isValid = false;
                ld->ei = MULT_COMMAS;
                break;
            } else if (!hasArgument) {
                ld->ei = ILLEGAL_COMMA;
                isValid = false;
                break;
            }
            previousIsComma = 1;
        } else if (!isspace(args[i])) {
            if (hasArgument == true && isspace(args[i - 1]) && previousIsComma == false) {
                ld->ei = MISSING_COMMA;
                isValid = false;
                break;
            } else {
                hasArgument = true;
                isValid = true; /* Mark as valid when encountering the first non-space character*/
                previousIsComma = false;
            }
        }
    }
    return isValid;
}
