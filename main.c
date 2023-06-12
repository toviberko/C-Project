#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

static char *file_name;
int is_error = 0;
int empty_obj = 0;

symbol *symbol_head;
codeLine *code_head;

int main(int argc, char *argv[]) {
    FILE *original_f, *macro_f;
	int arg_i = 1;

    if(argc<2){
        printf("ERROR! Not enough arguments in command line\n");
        return 0;
    }

    /*Get the name of the file and the word from the command line.*/
    
    while(arg_i < argc) {

        char *fname;
	  int name_length = 0;
        int i;
	char *newname;
        for (i = 0; *(argv[arg_i] + i) != '\0'; i++)
		name_length++;

	fname = (char *)malloc(name_length+1);
	fname = strcpy(fname,argv[arg_i]);

        newname = strcat(fname, ".as");
        file_name = fname;
        is_error = 0;
        symbol_head = NULL;
        code_head = NULL;

        /*Open the file if possible*/
        original_f = fopen(fname, "r");
        if (original_f == NULL) {
            printf("ERROR! File %s not found\n",fname);
            return 0;
        }
        for (i = 0; *(fname + i) != '.'; i++);
        *(fname + i) = '\0';
        newname = strcat(fname, ".am");
        file_name = newname;

	/*Creating an .am file without the macros*/
        macro_f = macro_file(original_f, fname);
        fclose(macro_f);

        for (i = 0; *(fname + i) != '.'; i++);
        *(fname + i) = '\0';
        newname = strcat(fname, ".am");

	/*Starting the first pass*/
        symbol_head = firstPass(newname);

        arg_i++;
    }

    return 0;
}

/**
 * Prints error message according to the type of the error.
 * @param n The type of the error (according to enum in constants.h).
 * @param row The row of the error.
 */
void error_msg(int n, int row){
	int input_length = 0, i;
    char *fname = file_name;
char *newname, *input;
int j;
    char *line;
    if(is_error)
        return;
    is_error = 1;
    empty_obj = 1;
    for(i=0; *(fname+i) != '.';i++);
    *(fname+i) = '\0';
    newname = strcat(fname,".am");

    input = get_input(newname,&input_length);

    i=0;
    
    for(j=0; j<row; j++) {
        line = get_line(input, &i, input_length);
        i++;
    }

for(i=0; *(fname+i) != '.';i++);
    *(fname+i) = '\0';
    printf("File: %s - ",file_name);
    switch(n) {
        case NOT_ENOUGH_ARGS:
            printf("ERROR! Not enough arguments in command line\n");
            break;
        case FILE_NOT_FOUND:
            printf("ERROR! File %s not found\n",file_name);
            break;
        case USED_LABEL:
            printf("ERROR! line %d: %s  -> The label is already used\n\n",row,line);
            break;
        case MULTIPLE_COMMAS:
            printf("ERROR! line %d: %s  -> Multiple commas\n\n",row, line);
            break;
        case MISSING_COMMA:
            printf("ERROR! line %d: %s  -> Missing comma\n\n",row,line);
            break;
        case EXTERN_LABEL:
            printf("ERROR! line %d: %s  -> Cannot assign label to an extern or entry sentence.\n\n",row ,line);
            break;
        case UNKNOWN_COMMAND:
            printf("ERROR! line %d: %s  -> Unknown command name.\n\n",row,line);
            break;
        case NOT_INT:
            printf("ERROR! line %d: %s ->  After the \"#\" there is no integer.\n\n",row,line);
            break;
        case ILLEGAL_DEST:
            printf("ERROR! line %d: %s -> Illegal destination address method.\n\n",row,line);
            break;
        case ILLEGAL_SOURCE:
            printf("ERROR! line %d: %s -> Illegal source address method.\n\n",row,line);
            break;
        case UNKNOWN_REG:
            printf("ERROR! line %d: %s -> Unknown register.\n\n",row,line);
            break;
        case ILLEGAL_LABEL:
            printf("ERROR! line %d: %s -> The name of the label is illegal.\n\n",row,line);
            break;
        case EXCESSIVE:
            printf("ERROR! line %d: %s -> Excessive input after end of command.\n\n",row,line);
            break;
        case MISSING_AFTER_LABEL:
            printf("ERROR! line %d: %s -> Missing command after the label.\n\n",row,line);
            break;
        case MISSING_PARAM:
            printf("ERROR! line %d: %s -> Missing parameter.\n\n",row,line);
            break;
        case MISSING_ARG:
            printf("ERROR! line %d: %s -> Missing argument.\n\n",row,line);
            break;
        case COMMA_ISSUE:
            printf("ERROR! line %d: %s -> There are commas in illegal places.\n\n",row,line);
            break;
        case TOO_LONG_LINE:
            printf("ERROR! line %d: -> There are more than 80 characters in the line, which is the maximum.\n\n",row);
            break;
        case CHAR_NOT_INT:
            printf("ERROR! line %d: %s -> Data can read only integers.\n\n",row,line);
            break;
        case NO_STRING:
            printf("ERROR! line %d: %s -> There is a problem with the string.\n\n",row,line);
            break;
        case NO_DATA:
            printf("ERROR! line %d: %s -> There is no data.\n\n",row,line);
            break;
        case NO_EXTERN:
            printf("ERROR! line %d: %s -> There is no label after .extern.\n\n",row,line);
            break;
        case NO_ENTRY:
            printf("ERROR! line %d: %s -> There is no label after .entry.\n\n",row,line);
            break;
        case UNKNOWN_LABEL:
            printf("ERROR! line %d: %s -> The label is not declared.\n\n",row,line);
            break;
    }
}


