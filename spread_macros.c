#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"
int row = 0;
/**
 * Returns an .am file without the macros in the original file.
 * @param f The original file.
 * @param fname The name of the file.
 * @return An .am file without the macros in the original file.
 */
FILE *macro_file(FILE *f, char *fname){
    macro *table_head;

    int i, input_length=0,first_macro = 1,is_in_macro=0;
    FILE *macro_f;
    char *input;
    macro *ptr;
char *newname;
    char *line;

    for(i=0; *(fname+i) != '.';i++);
    *(fname+i) = '\0';
    newname = strcat(fname,".am");

    macro_f = fopen(newname,"w");
    for(i=0; *(fname+i) != '.';i++);
    *(fname+i) = '\0';
    fname = strcat(fname,".as");

    input = get_input(fname,&input_length);

    i=0;
    table_head = NULL;
    for(line = get_line(input,&i,input_length); line != NULL && i <= input_length;  line = get_line(input,&i,input_length)){
char *word;
        int j =0, swap=0;
        i++;
        row++;
       

        word = get_word(line,&j);
        if(word == NULL)
            return macro_f;

	/*If the word is a macro, swap the name of the macro with its content*/
        for(ptr = table_head; ptr != NULL; ptr = ptr->next) {
            if (strcmp(word, ptr->name) == 0 && ptr->content!=NULL) {
                fprintf(macro_f, "%s", ptr->content);
                swap = 1;
                break;
            }
        }
        if(swap)
            continue;

	/*If there is a start of a new macro, add it to the table*/
        if(strcmp(word,"mcr") == 0){
		macro *new_mcr;
            is_in_macro = 1;
            for(ptr = table_head; ptr != NULL && ptr->next != NULL; ptr = ptr->next);
            new_mcr = (macro*) malloc(sizeof(macro));
            new_mcr->name = get_word(line ,&j);
            new_mcr->next = NULL;
            new_mcr->content= calloc(1,'\0');
            if(first_macro)
                table_head = new_mcr;
            else{
                ptr->next = new_mcr;
            }
            first_macro = 0;

        }else if(is_in_macro){
		/*While in the macro, add each row to the macro content in the table*/
            if(strcmp(word,"endmcr")==0){
                is_in_macro=0;
            }else{
                for(ptr = table_head; ptr != NULL && ptr->next != NULL; ptr = ptr->next);
                ptr->content = realloc(ptr->content,sizeof(ptr->content)*sizeof (line));
                strcat(ptr->content,line);
            }
        }else if(!is_in_macro){
            fprintf(macro_f,"%s",line);
            free(line);
        }
    }

    free(table_head);
    return macro_f;
}
