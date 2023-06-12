#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"

char *commands_names[16] = {"mov", "cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
codeLine *data_code;
extern symbol *symbol_head;
extern codeLine *code_head;

/*Legal source and destination methods for each operation, by the order in the enum in constants.h*/
static int legal_source_methods[16][4] = {{0,1,3,-1},{0,1,3,-1},{0,1,3,-1},{0,1,3,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}};
static int legal_dest_methods[16][4] = {{1,3,-1,-1},{-1,0,1,3},{-1,-1,1,3},{-1,-1,1,3},{-1,-1,1,3},{-1,-1,1,3},{-1,-1,1,3},{-1,-1,1,3},{-1,-1,1,3},{-1,1,2,3},{-1,1,2,3},{-1,-1,1,3},{-1,0,1,3},{-1,1,2,3},{-1,-1,-1,-1},{-1,-1,-1,-1}};

/**
 * The first pass through the file.
 * Encodes the instructions and data.
 * Creates symbols table.
 * @param fname The name of the file.
 * @return The head of the symbol tabel.
 */
symbol *firstPass(char *fname){
    int DC =0, IC=100;
    int input_length=0,i=0,first_symbol=1, command=0;
    char *input = get_input(fname,&input_length), *line;
    symbol *ptr;
    int *empty_code_line = calloc(14,sizeof(int));
    code_head = new_code(100,empty_code_line);
    data_code = new_code(0,empty_code_line);
    symbol_head = NULL;
    row = 0;

    for(line = get_line(input,&i,input_length); line != NULL;  line = get_line(input,&i,input_length)) {
        
	  char *label, *word;
        int j=0, is_symbol = 0, L=0,is_operation=0;
	  int *code;
        int prm1=0,prm2=0,arg1=0,arg2=0;

	/*if the line is empty, go to the next line*/
	  if(strcmp(line,"\n") == 0) {
            while(isspace(*(input+i)))
                i++;
            continue;
        }

        row++;
        i++;
        command=1; /*is this line contains a command or data*/
        is_error=0; /*is there already an error in this line*/
        
        skip_spaces(line,&j);
        label = get_word(line,&j);

	/*If this line is a comment, continue to the next line*/
        if(*label == ';' || *label == '\0' || *label == '\n')
            continue;

	/*If there is a label at the beginning of the line, skip it*/
        if(is_label(label)){
            is_symbol=1;
            if(end_of_line(line,&j))
                error_msg(MISSING_AFTER_LABEL,row);
            word = get_word(line,&j);
        }else
            word = label;


	/*Data or string instruction*/
        if(strcmp(word, ".data") == 0 || strcmp(word,".string") == 0){
		/*If there was a label at the beginning of the line, add the label to the table*/
            if(is_symbol) {
			symbol *ptr, *new_symbol;
                if(used_label(label))
                    error_msg(USED_LABEL,row);

                
                for(ptr = symbol_head; ptr != NULL && ptr->next != NULL; ptr = ptr->next);
                new_symbol = new_label(label,DC,"data");

                if(first_symbol)
                    symbol_head = new_symbol;
                else{
                    ptr->next = new_symbol;
                }
                first_symbol = 0;
            }

		/*Calculate the amount of words needed for this line*/
            if(strcmp(word, ".data") == 0 ){
                DC += number_of_values(line,&j);
            }else{
                DC += number_of_chars(line,&j);
            }

        }

	/*Extern or an entry instruction*/
        else if(strcmp(word, ".extern") == 0 || strcmp(word,".entry") == 0){
            /*Extern and entry instructions can't be stored in a label*/
		if (is_symbol)
                error_msg(EXTERN_LABEL,row);

		/*Extern instruction*/
            if(strcmp(word, ".extern") == 0) {
		    char *arg;
		    symbol *ptr,*new_symbol;
                if(end_of_line(line,&j))
                    error_msg(NO_EXTERN,row);

		    /*Check if the label is already used*/
                arg = get_word(line, &j);
                if(used_label(arg))
                    error_msg(USED_LABEL,row);
                if(!end_of_line(line,&j))
                    error_msg(EXCESSIVE,row);
                
		    /*Add the lable to the table with an "extenal" flag*/
                for (ptr = symbol_head; ptr != NULL && ptr->next != NULL; ptr = ptr->next);
                new_symbol = new_label(arg, 0, "external");

                if (first_symbol)
                    symbol_head = new_symbol;
                else {
                    ptr->next = new_symbol;
                }
                first_symbol = 0;

            }else{
		/*In case there is an entry instruction but there is not label*/
                if(end_of_line(line,&j))
                    error_msg(NO_ENTRY,row);
            }
        }else{
		/*Other instucrions (not data,string,extern or entry)*/
            is_operation=1;
		/*If there is a symbol, add it to the table*/
            if(is_symbol) {
                symbol *ptr,*new_symbol;
                if(used_label(label))
                    error_msg(USED_LABEL,row);

                for (ptr = symbol_head; ptr != NULL && ptr->next != NULL; ptr = ptr->next);
                new_symbol = new_label(label, IC, "code");

                if (first_symbol)
                    symbol_head = new_symbol;
                else {
                    ptr->next = new_symbol;
                }
                first_symbol = 0;
            }
        }

        /*Operation line*/
        if(command && is_operation) {
		codeLine *cptr;
		
		/*Calculate the amount of words needed for this line*/
            if (enum_command(word) != -1){
                L += num_of_words(line, &j, word, &prm1,&prm2,&arg1,&arg2);
                code = op_code(word, &prm1,&prm2,&arg1,&arg2);
            }
            command = 0;

            empty_code_line = calloc(14,sizeof(int));
            
		/*Encode the first word in memory of this line*/
            for(cptr = code_head; cptr->row != IC; cptr = cptr->next){
                if(cptr->next == NULL)
                    cptr->next = new_code((cptr->row)+1,empty_code_line);
            }
            cptr->code = code;
        }
        IC += L;

    }

    /*Add IC to the data symbols, because they come after the instructions*/
    for(ptr = symbol_head; ptr != NULL; ptr = ptr->next){
        if(strcmp(ptr->flag,"data") == 0)
            ptr->value += IC;
    }

    secondPass(fname, symbol_head,code_head,DC,IC);
    return symbol_head;
}

/**
 * Returns 1 if the word is a label, and 0 if it's not a label.
 * Removes from the word the ":" at the end of the label.
 * @param word The word to be checked.
 * @return 1 if the word is a label, and 0 if it's not a label.
 */
int is_label(char *word){
    int i;
    for(i=0;i<strlen(word)-1;i++);
    if(*(word+i) == ':') {
        *(word+i) = '\0';

        if(!illegal_label(word))
            return 0;
        return 1;
    }
    else
        return 0;
}

/**
 * Encodes the .data line and returns how many numbers there are in the line.
 * @param line Pointer to the line to be checked.
 * @param j The place of the first value.
 * @return The number of values in the .data line.
 */
int number_of_values(char *line, int *j){
    char chr = *(line+*j);
    int counter =0;
    int is_negative =0;
    int *bin_num;
    int k;

    if(end_of_line(line,j))
        error_msg(NO_DATA,row);
   
    skip_spaces(line,j);

    /*Encode each number in the data instruction*/
    while(chr != '\0' && chr!= '\n'){
	int num = get_number(line,j);
	int k = 13;
	codeLine *cptr;
	
	bin_num = (int*)malloc(14* sizeof(int));	
	is_negative=0;
	if(num <0){
		is_negative=1;
		num *=-1;
	}

	
	/*Convert the number to 14 long binary number*/ 
      while (num > 0) {
		*(bin_num+k) = num % 2;
        	num /= 2;
        	k--;
      }
      while(k>=0){
        	*(bin_num+k) = 0;
        	k--;
      }

	/*Use two's complement method*/ 
      if(is_negative){
          for(k=0; k<14; k++){
              switch_bit(bin_num+k);
          }

          for(k=13;k>=0;k--){
              if(*(bin_num+k) == 0) {
                  switch_bit(bin_num + k);
                  break;
              }
          }

          k++;
          for(;k<14;k++)
              switch_bit(bin_num +k);
      }

	/*Add to the lines of data code tabel*/ 
      for(cptr = data_code; cptr->next!=NULL; cptr = cptr->next);
      cptr->next= new_code((cptr->row)+1,bin_num);

      counter++;


	if(!is_number(line,*j))
		break;

	skip_spaces(line,j);
	check_commas(line,j);
	chr = *(line+*j);
    }

    if(!end_of_line(line,j))
        error_msg(EXCESSIVE,row);

    /*Check for commas issues in the line*/
    k =*j;
    for(; isspace(*(line+k));k--);
    if(*(line+k) == ',')
        error_msg(COMMA_ISSUE,row);

    return counter;
}

/**
 * Encodes the .string line and returns how many characters there are in the line.
 * @param line Pointer to the line to be checked.
 * @param j The place of the first character.
 * @return The number of characters in the .string line.
 */
int number_of_chars(char *line, int *j) {
	char chr;
    	int counter = 0;
	codeLine *cptr;
	int *empty_code_line = calloc(14, sizeof(int));

	skip_spaces(line, j);
	chr = *(line + *j);
	if (chr != '"') {
	      error_msg(NO_STRING, row);
	      return 0;
	}

	(*j)++;
	chr = *(line + *j);
	while (chr != '"' && chr!= '\n' && chr != '\0') {
	      int chr_ascii = chr;
		int *chr_bin = (int *) malloc(14 * sizeof(int));
       	int k = 13;
		codeLine *cptr;

		/*Convert the ascii code of the character to a 14 long binary number*/
        	while (chr_ascii > 0) {
		      *(chr_bin + k) = chr_ascii % 2;
		      chr_ascii /= 2;
		      k--;
        	}
		while (k >= 0) {
			*(chr_bin + k) = 0;
		      k--;
		}

        
		/*Add to the lines of data code tabel*/
		for (cptr = data_code; cptr->next != NULL; cptr = cptr->next);
		cptr->next = new_code((cptr->row) + 1, chr_bin);

      	counter++;
		(*j)++;
		chr = *(line + *j);
	}

	if(*(line + *j) != '"')
	      error_msg(NO_STRING,row);
    
	/*Add the '\0' character to the lines of code table*/
	for (cptr = data_code; cptr->next != NULL; cptr = cptr->next);
	cptr->next = new_code((cptr->row) + 1, empty_code_line);
	counter++;

	(*j)++;
	if(!end_of_line(line,j))
      	error_msg(EXCESSIVE,row);
    	
	return counter;
}

/**
 * Checks if the label is already used.
 * @param word The label to be checked.
 * @return 1 if the label is used, and 0 if it is not used.
 */
int used_label(char *word){
    symbol *ptr;
    for (ptr = symbol_head; ptr != NULL; ptr = ptr->next) {
        if (strcmp(word, ptr->name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Checks if the label is legal - not a reserved word or not an english word.
 * @param word The label to be checked.
 * @return 1 if the label is legal and 0 if not.
 */
int illegal_label(char *word){
    int legal = 1;

    int i;
    if(!strcmp(word,"data")
    ||!strcmp(word,"string")
    ||!strcmp(word,"entry")
    ||!strcmp(word,"extern")
    ||!strcmp(word,"r0")
    ||!strcmp(word,"r1")
    ||!strcmp(word,"r2")
    ||!strcmp(word,"r3")
    ||!strcmp(word,"r4")
    ||!strcmp(word,"r5")
    ||!strcmp(word,"r6")
    ||!strcmp(word,"r7"))
        legal =0;

    for(i=0;i<16;i++){
        if(!strcmp(word,commands_names[i]))
            legal = 0;
    }

    /*The first character must be a letter*/
    if(*word < 'A' || (*word > 'Z' && *word < 'a') || *word>'z')
        legal =0;

    /*The rest of the characters must be a letter or a number*/ 
    for(i=0;*(word+i)!='\0';i++){
        char ch =*(word+i);
        if(ch < '0' || (ch< 'A' && ch>'9') || (ch > 'Z' && ch < 'a') || ch>'z')
            legal =0;
    }

    if(legal == 0) {
        error_msg(ILLEGAL_LABEL, row);
        return 0;
    }
    return 1;
}

/**
 * Creates new symbol and returns it.
 * @param label The name of the label.
 * @param value The value of the label.
 * @param flag The type of the label.
 * @return Pointer to the new symbol.
 */
symbol *new_label(char *label, int value, char *flag){

    symbol *new_label = (symbol*) malloc(sizeof(symbol));
    new_label->name = label;
    new_label->next = NULL;
    new_label->value = value;
    new_label->flag = flag;

    return new_label;
}

/**
 * Counts the number of words in the memory is needed for this line.
 * Changes the address method number for each argument and parameter.
 * @param line The line to be checked.
 * @param j The place of the first character of the command.
 * @param command The name of the command.
 * @param prm1 The first parameter's address method.
 * @param prm2 The second parameter's address method.
 * @param arg1 The first argument's address method.
 * @param arg2 The second argument's address method.
 * @return The number of words in the memory is needed for this line.
 */
int num_of_words(char *line, int *j, char *command, int *prm1,int *prm2,int *arg1,int *arg2){
    int command_num = enum_command(command);
    char *word;
    int count = 1;
    int num_of_p;

        switch (command_num) {
		/*Two aeguments*/
            case MOV:
            case CMP:
            case ADD:
            case SUB:
            case LEA:
                word = get_word(line, j);
                if(word != NULL) {
                   *prm1 = 0;
                   *prm2 = 0;
                    if (is_register(word)) {
                        *arg1 = 3;
                        count++;
                        skip_spaces(line, j);
                        check_commas(line,j);
                        skip_spaces(line, j);
                        word = get_word(line, j);
                        if(!strcmp(word,""))
                            error_msg(MISSING_ARG,row);
                        if (is_register(word)) {
                            *arg2 = 3;
                        }else if (is_immediate(word)) {
                            count++;
                            *arg2 = 0;
                        }else{
                            *arg2=1;
                            count++;
                        }
                    } else if (is_immediate(word)) {
                        *arg1 = 0;
                        skip_spaces(line, j);
                        check_commas(line,j);
                        skip_spaces(line, j);
                        word = get_word(line, j);
                        if(!strcmp(word,""))
                            error_msg(MISSING_ARG,row);
                        if (is_register(word)) {
                            *arg2 = 3;
                        } else if (is_immediate(word)) {
                            *arg2 = 0;
                        }else
                            *arg2 = 1;
                        count += 2;
                    }else {
                        *arg1 = 1;
                        skip_spaces(line, j);
                        check_commas(line,j);
                        skip_spaces(line, j);
                        word = get_word(line, j);
                        if(!strcmp(word,""))
                            error_msg(MISSING_ARG,row);
                        if (is_register(word)) {
                            *arg2 = 3;
                        }else if (is_immediate(word)) {
                            *arg2 = 0;
                        }else
                            *arg2 = 1;
                        count += 2;
                    }
                }
                break;

		/*One argument without parameters*/
            case NOT:
            case CLR:
            case INC:
            case DEC:
            case RED:
            case PRN:
                word = get_word(line, j);
                skip_spaces(line,j);
                if(*(line+*j) == ',')
                    error_msg(COMMA_ISSUE,row);
                if(!strcmp(word,""))
                    error_msg(MISSING_ARG,row);
                *arg1 = -1;
                if(is_register(word))
                    *arg2 = 3;
                else if(is_immediate(word))
                    *arg2 = 0;
                else
                    *arg2 = 1;
                count++;
                break;

		/*One argument with or without parameters*/
            case JMP:
            case BNE:
            case JSR:
                word = get_word(line, j);
                if(!strcmp(word,""))
                    error_msg(MISSING_ARG,row);
                *arg1 = -1;
                num_of_p = num_of_params(line,j,prm1,prm2);
                if(num_of_p == 0) {
                    if (is_register(word))
                        *arg2 = 3;
                    else if (is_immediate(word))
                        *arg2 = 0;
                    else
                        *arg2 = 1;
                }else
                    *arg2 = 2;
                count += 1+num_of_p;
                break;


		/*No arguments*/
            case RTS:
            case STOP:
                *arg1 = -1;
                *arg2 = -1;
                break;
        }

        check_address_method(*arg1,*arg2,command_num);

        if(!end_of_line(line,j))
            error_msg(EXCESSIVE,row);

        return count;
}

/**
 * Returns the enum of the command (according to the enum in constants.h)
 * @param command The name of the command.
 * @return The enum of the command (according to the enum in constants.h)
 */
int enum_command(char *command){
    if(strcmp(command, "mov") == 0)
        return MOV;
    else if(strcmp(command, "cmp") == 0)
        return CMP;
    else if(strcmp(command, "add") == 0)
        return ADD;
    else if(strcmp(command, "sub") == 0)
        return SUB;
    else if(strcmp(command, "not") == 0)
        return NOT;
    else if(strcmp(command, "clr") == 0)
        return CLR;
    else if(strcmp(command, "lea") == 0)
        return LEA;
    else if(strcmp(command, "inc") == 0)
        return INC;
    else if(strcmp(command, "dec") == 0)
        return DEC;
    else if(strcmp(command, "jmp") == 0)
        return JMP;
    else if(strcmp(command, "bne") == 0)
        return BNE;
    else if(strcmp(command, "red") == 0)
        return RED;
    else if(strcmp(command, "prn") == 0)
        return PRN;
    else if(strcmp(command, "jsr") == 0)
        return JSR;
    else if(strcmp(command, "rts") == 0)
        return RTS;
    else if(strcmp(command, "stop") == 0)
        return STOP;
    else
        error_msg(UNKNOWN_COMMAND,row);
    return -1;

}

/**
 * Checks if a word is a register.
 * @param word The word to be checked.
 * @return 1 if the word is a register and 0 if not.
 */
int is_register(char *word){
    if(strcmp(word,"r0")== 0
    ||strcmp(word,"r1")== 0
    ||strcmp(word,"r2")== 0
    ||strcmp(word,"r3")== 0
    ||strcmp(word,"r4")== 0
    ||strcmp(word,"r5")== 0
    ||strcmp(word,"r6")== 0
    ||strcmp(word,"r7")== 0)
        return 1;

    if(*word == 'r')
        error_msg(UNKNOWN_REG,row);

    return 0;
}

/**
 * Checks if a word is an immediate address method,
 * And if the characters after the "#" are numebrs.
 * @param word The word to be checked.
 * @return 1 if the word is immediate nd 0 if not.
 */
int is_immediate(char *word){
    int i = 0;
    if(*word == '#'){
        i++;
        if(get_number(word,&i))
            return 1;
        else
            error_msg(NOT_INT,row);
    }
    return 0;
}

/**
 * Checks if there are enough parameters for the command.
 * Counts the number of words in the memory is needed for the parameters.
 * Changes the address method number for each parameter.
 * @param word The command to be checked.
 * @param j The place in the word of the first character.
 * @param prm1 Address method of the first parameter.
 * @param prm2 Address method of the second parameter.
 * @return The number of words in the memory is needed for the parameters.
 */
int num_of_params(char *word,int *j, int *prm1, int *prm2){

    char *param1, *param2;
    if(*(word+*j) =='(') {
        int k,s;
        (*j)++;
        param1 = get_word(word, j);

        if(!strcmp("",param1))
            error_msg(MISSING_PARAM,row);

        for(k=0;*(param1+k+1)!='\0';k++);
        if(*(param1+k) ==')')
            error_msg(MISSING_PARAM,row);

        s = *j;
        check_commas(word,j);
        param2 = get_word(word, j);
        if(*param2 == ')')
            error_msg(MISSING_PARAM,row);
	
	  /*Get just the parameter with out the ')'*/
        *(param2+*j-s-2) = '\0';

        if(is_register(param1))
            *prm1 = 3;
        else if(is_immediate(param1))
            *prm1 = 0;
        else
            *prm1 = 1;

        if(is_register(param2))
            *prm2 = 3;
        else if(is_immediate(param2))
            *prm2 = 0;
        else
            *prm2 = 1;

        (*j)++;

        if(is_register(param1) && is_register(param2))
            return 1;
        return 2;
    }else
        return 0;
}

/**
 * Encodes the first word in the memory of the command line.
 * @param command The name of the command.
 * @param prm1 The address method of the first parameter.
 * @param prm2 The address method of the second parameter.
 * @param arg1 The address method of the first argument.
 * @param arg2 The address method of the first argument.
 * @return The code of the first word in the memory for the command line.
 */
int *op_code(char *command, int *prm1,int *prm2,int *arg1,int *arg2){
    int *code = calloc(14,sizeof(int));
    int op_num = enum_command(command);
    int k=7;

    /*The ARE field in the first word of the line is 00*/
    *(code+12) = 0;
    *(code+13) = 0;

    /*Convert the operation number to a 4 long binary number*/
    while (op_num > 0) {
        *(code+k) = op_num % 2;
        op_num /= 2;
        k--;
    }

    while(k>4){
        *(code+k) = 0;
        k--;
    }

    /*Add to the first word the address methods*/
    address_method(prm1, code, 0);
    address_method(prm2,code,2);
    address_method(arg1,code,8);
    address_method(arg2,code,10);

    return code;
}

/**
 * Encodes the address methods in the first word in the memory of the command line.
 * @param method The address method.
 * @param code The code of the first word in the memory.
 * @param place The place to put this address method encoding.
 */
void address_method( int *method, int *code, int place){
    switch(*method){
        case 0:
        case -1:
            *(code+place) = 0;
            *(code+place+1) = 0;
            break;
        case 1:
            *(code+place) = 0;
            *(code+place+1) = 1;
            break;
        case 2:
            *(code+place) = 1;
            *(code+place+1) = 0;
            break;
        case 3:
            *(code+place) = 1;
            *(code+place+1) = 1;
            break;
    }
}

/**
 * Checks if the address methods of the arguments are legal for the command (according to the table in constants.h)
 * @param arg1 The address method of the first argument.
 * @param arg2 The address method of the second argument.
 * @param command_num The command enum (according to constants.h)
 */
void check_address_method(int arg1,int arg2,int command_num){
    int i;
    int legal1 = 0;
    int legal2 = 0;
    for(i=0;i<4;i++) {
        if (legal_source_methods[command_num][i] == arg1)
            legal1 = 1;

        if (legal_dest_methods[command_num][i] == arg2)
            legal2 = 1;
    }

    if(!legal2)
        error_msg(ILLEGAL_DEST,row);
    else if(!legal1)
        error_msg(ILLEGAL_SOURCE,row);
}

/**
 * Checks if the place j in the line is the end of the line.
 * @param line The line to be checked.
 * @param j The current place in the line.
 * @return 1 if j is the last charcter in the line, and 0 if not.
 */
int end_of_line(char *line, int *j){
    while(*(line+*j)!='\n' &&*(line+*j)!='\0'&& *(line+*j)!=EOF){
        if(!isspace(*(line+*j)))
            return 0;
        (*j)++;
    }
    return 1;
}
