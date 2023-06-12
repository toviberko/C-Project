#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"

FILE *obj_file;
int IC;
int is_ext_empty =1;
extern codeLine *data_code;
extern symbol *symbol_head;
extern codeLine *code_head;
/**
 * The second pass through the file.
 * Encodes the rest of the file.
 * @param fname The name of the file.
 * @param symbol_head The head of the symbol table.
 * @param code_head The head of the first code line.
 * @param total_DC The total words of data is needed.
 * @param total_IC The total words of instructions is needed.
 */
void secondPass(char *fname, symbol *symbol_head,codeLine *code_head, int total_DC, int total_IC){
   
    int input_length = 0, i=0, arg1, arg2, prm1, prm2,m,is_ent_empty;
    char *input = get_input(fname,&input_length), *line, *word,*obj_name,*ext_name,*ent_name;
    FILE *ent_file,*ext_file;
    symbol *ptr;
    codeLine *cptr,*data_cptr;
    row = 0;
    IC = 100;
    is_ext_empty =1;

    /*Create the object file*/
    for(m=0; *(fname+m) != '.';m++);
    *(fname+m) = '\0';
    obj_name = strcat(fname,".ob");
    obj_file = fopen(obj_name,"w");

    /*Create the externs file*/
    for(m=0; *(fname+m) != '.';m++);
    *(fname+m) = '\0';
    ext_name = strcat(fname,".ext");
    ext_file = fopen(ext_name,"w");

    for(line = get_line(input,&i,input_length); line != NULL;  line = get_line(input,&i,input_length)) {
	  int j=0,k;   
	  int num_of_args, has_params = 0;     
	  if(strcmp(line,"\n") == 0) {
            while(isspace(*(input+i)))
                i++;
            continue;
        }
        row++;
        i++;
        word = get_word(line,&j);

	  /*Comment line*/
        if(*word == ';')
            continue;

        is_error = 1; /*To avoid multiple errors for the same reason*/
        if(is_label(word)) {
            word = get_word(line, &j);
        }

        if(strcmp(word,".data") == 0 || strcmp(word,".string") == 0 || strcmp(word,".extern") == 0 )
            continue;


	  /*Entry label*/
        if(strcmp(word,".entry") == 0){
		/*Change the flag to "entry"*/
            char *label_name = get_word(line,&j);
            symbol *ptr;
            for(ptr = symbol_head; ptr !=NULL; ptr=ptr->next){
                if(strcmp(ptr->name,label_name) == 0)
                    ptr->flag = "entry";
            }
            continue;
        }

        IC++;
        k = j;
        num_of_words(line, &k, word, &prm1,&prm2,&arg1,&arg2);
        
        if(arg1 == -1){
            if(arg2 == -1)
                num_of_args = 0;
            else
                num_of_args = 1;
        }
        else
            num_of_args = 2;


	  /*Encode each argument according to its address method*/
        if(num_of_args == 2) {
            switch (arg1) {
                case 0:
                    write_immediate(line, &j,code_head);
                    break;
                case 1:
                    write_label(line, &j, symbol_head,code_head, ext_file);
                    break;
                case 3:
                    if(arg2 == 3) {
                        write_register(line, &j, 0, 1,code_head);
                        arg2 = -1;
                    }
                    else
                        write_register(line, &j, 0, 0,code_head);

                    break;
            }
            skip_spaces(line, &j);
            check_commas(line, &j);
            skip_spaces(line, &j);
        }

        if(num_of_args > 0) {
            switch (arg2) {
                case 0:
                    write_immediate(line, &j,code_head);
                    break;
                case 1:
                    write_label(line, &j, symbol_head,code_head,ext_file);
                    break;
                case 2:
                    write_label(line,&j,symbol_head,code_head,ext_file);
                    has_params = 1;
                    break;
                case 3:
                    write_register(line, &j, 1,0,code_head);
                    break;
            }
        }

	  /*Encode each parameter if exists*/
        if(has_params) {
            switch (prm1) {
                case 0:
                    write_immediate(line, &j,code_head);
                    break;
                case 1:
                    write_label(line, &j, symbol_head,code_head,ext_file);
                    break;
                case 3:
                    if(prm2 == 3) {
                        write_register(line, &j, 0, 1,code_head);
                        prm2 = -1;
                    }else
                        write_register(line,&j,0,0,code_head);
                    break;
                case -1:
                    break;
            }

            check_commas(line, &j);
            switch (prm2) {
                case 0:
                    write_immediate(line, &j,code_head);
                    break;
                case 1:
                    write_label(line, &j, symbol_head,code_head,ext_file);
                    break;
                case 3:
                    write_register(line, &j, 1,0,code_head);
                    break;
                case -1:
                    break;
            }
        }
    }


    /*Add the data lines of code to the end of the other lines of code*/
    for(cptr = code_head; cptr->next != NULL; cptr = cptr->next);
    for(data_cptr = data_code; data_cptr != NULL; data_cptr = data_cptr->next)
        data_cptr->row += cptr->row;
    cptr->next = data_code->next;

    /*Write the code in the object file*/
    if(!empty_obj) {
	  codeLine *cptr;
	  /*Number of instruction words and data words*/
        fprintf(obj_file, "%d %d", total_IC - START_IC, total_DC);
        for (cptr = code_head; cptr != NULL;) {
		int row, *pcode;
		char *unique_code;
            fprintf(obj_file, "\n");

		/*Write the number of row*/
            row = cptr->row;
            if (row < MIN_FOUR_DIGITS)
                fprintf(obj_file, "%d", 0);
            fprintf(obj_file, "%d\t", cptr->row);
          
		/*Write the row*/
            pcode = cptr->code;
            unique_code = bin_to_unique(pcode);
            fprintf(obj_file, "%s", unique_code);
            cptr = cptr->next;
        }
    }

    /*Creat the entries file*/
    for(m=0; *(fname+m) != '.';m++);
    *(fname+m) = '\0';
    ent_name = strcat(fname,".ent");
    ent_file = fopen(ent_name,"w");

    is_ent_empty = 1;
    for(ptr = symbol_head; ptr !=NULL; ptr=ptr->next){
        if(strcmp(ptr->flag,"entry") == 0){
            is_ent_empty = 0;
            fprintf(ent_file,"%s\t %d\n",ptr->name,ptr->value);
        }
    }

    /*Remove unneccesery files*/
    if(is_ent_empty) {
        fclose(ent_file);
        remove(ent_name);
    }

    for(m=0; *(fname+m) != '.';m++);
    *(fname+m) = '\0';
    ext_name = strcat(fname,".ext");
    if(is_ext_empty) {
        fclose(ext_file);
        remove(ext_name);
    }

    for(m=0; *(fname+m) != '.';m++);
    *(fname+m) = '\0';
    obj_name = strcat(fname,".ob");
    if(empty_obj) {
        fclose(obj_file);
        remove(obj_name);
    }

}

/**
 * Converts the binary code to the unique code of . and /
 * Changes every '1' to '.' and every '0' to '/'.
 * @param code The code to be converted.
 * @return A string with the unique code.
 */
char *bin_to_unique(int *code){
    char *res = malloc(14);
    int i;
    for(i=0;i<14;i++){
        if(*(code+i) == 0)
            *(res+i) = '.';
        else
            *(res+i) = '/';
    }
    return res;
}

/**
 * Encodes an immediate argument/parameter.
 * @param line The line of the code.
 * @param j The place of the immediate argument in the line.
 * @param code_head The head of the lines of code table.
 */
void write_immediate(char *line, int *j,codeLine *code_head){
    int *code = (int*)malloc(14*sizeof(int));
    int is_negative = 0,m,num,k;
    char *char_num;

    /*Absolute in ARE field*/
    *(code+12) = 0;
    *(code+13) = 0;

    m = *j;
    char_num = get_word(line,&m);
    if(!is_immediate(char_num))
        return;

    skip_spaces(line,j);
    (*j)++;
    num = get_number(line,j);
    if(num < 0) {
        num *= -1;
        is_negative = 1;
    }

    /*convert to 12 digit binary number*/
    to_binary(code,num);

    /*Use the two's complement method*/
    if(is_negative){
        for(k=0; k<12; k++){
            switch_bit(code+k);
        }

        for(k=11;k>=0;k--){
            if(*(code+k) == 0) {
                switch_bit(code + k);
                break;
            }
        }

        k++;
        for(;k<12;k++)
            switch_bit(code +k);
    }

    print_code(code_head, code);
    IC++;
}

/**
 * Encodes a symbol argument/parameter.
 * @param line The line of the code.
 * @param j The place of the symbol argument in the line.
 * @param symbol_head The head of the symbol table.
 * @param code_head The head of the lines of code table.
 * @param ext_file The file of the extern symbols.
 */
void write_label(char *line, int *j, symbol *symbol_head, codeLine *code_head, FILE *ext_file){
    int *code = (int*)malloc(14*sizeof(int));
    int m,i,address;
    char *label_name;
    symbol *ptr;

    /*Label is relocateable*/
    *(code+12) = 1;
    *(code+13) = 0;

    skip_spaces(line,j);
    m = *j;
    label_name = get_word(line,&m);
    if(strcmp(label_name,"") == 0)
        return;

    i=0;
    while(*(label_name+i) != '(' && *(label_name+i) != ')' && *(label_name+i) != '\0')
        i++;

    *j = *j+i+1;
    if(*(line+*j-1) == ',')
        (*j)--;

    *(label_name+i)='\0';

    address = -1;

    /*Check if the lable is extern*/
    for(ptr = symbol_head; ptr !=NULL; ptr=ptr->next){
        if(strcmp(ptr->name,label_name) == 0){
            address = ptr->value;
            if(strcmp(ptr->flag,"external")==0){
                address = 0;
                is_ext_empty = 0;
                fprintf(ext_file,"%s\t%d\n",ptr->name,IC);
                *(code+12) = 0;
                *(code+13) = 1;
            }
        }
    }

    is_error = 0;
    if(address == -1)
        error_msg(UNKNOWN_LABEL,row);
    is_error = 1;

    to_binary(code,address);

    print_code(code_head, code);
    IC++;
}

/**
 * Encodes a register argument/parameter.
 * Two registers from the same source line are written in the same word.
 * @param line The line of the code.
 * @param j The place of the register argument in the line.
 * @param is_dest Equal to 1 if the register is the destination, and 0 if it's the source.
 * @param two_registers Equal to 1 if there are two registers, and 0 if there is 1.
 * @param code_head The head of the lines of code table.
 */
void write_register(char *line, int *j, int is_dest, int two_registers, codeLine *code_head){
    int *code = (int*)malloc(14*sizeof(int));
    int num;
    /*Register is absoulte*/
    *(code+12) = 0;
    *(code+13) = 0;

    skip_spaces(line,j);
    (*j)++;
    num = get_number(line,j);

    if(!is_dest){
        int k=5;
        while (num > 0) {
            *(code+k) = num % 2;
            num /= 2;
            k--;
        }

        while(k>=0){
            *(code+k) = 0;
            k--;
        }

        k=6;
        while(k<12){
            *(code+k) = 0;
            k++;
        }

	  /*Two registers are written in the same word*/
        if(two_registers){
            skip_spaces(line,j);
            check_commas(line,j);
            skip_spaces(line,j);
            (*j)++;
            num = get_number(line,j);
        }
    }

    if(is_dest || two_registers){
        int k=11;
        while (num > 0) {
            *(code+k) = num % 2;
            num /= 2;
            k--;
        }

        while(k>=6){
            *(code+k) = 0;
            k--;
        }

        if(!two_registers) {
            k = 0;
            while (k < 6) {
                *(code + k) = 0;
                k++;
            }
        }
    }

    print_code(code_head, code);
    IC++;
}

/**
 * Switches the bit from 1 to 0 and vice versa.
 * @param bit The bit to be changed.
 */
void switch_bit(int *bit){
    if(*bit==0)
        *bit = 1;
    else if(*bit == 1)
        *bit = 0;
}

/**
 * Adds to the lines of code table new code line.
 * @param code_head The head of the lines of code table.
 * @param code The code to be added.
 */
void print_code(codeLine *code_head, int *code){
    int *empty_code_line = calloc(14,sizeof(int));
    codeLine *cptr;
    for(cptr = code_head; cptr->row != IC; cptr = cptr->next){
        if(cptr->next == NULL)
            cptr->next = new_code((cptr->row)+1,empty_code_line);
    }
    cptr->code = code;
}
