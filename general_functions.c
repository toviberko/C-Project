#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"

/**
 * Gets the number starting in place j in the line.
 * @param line The line to be checked in.
 * @param j The place of the character to be checked.
 * @return The number if exists, and NULL if doesn't exists.
 */
int get_number(char *line, int *j){
    char *num = malloc(sizeof(line)),cur;
    int p = 0;
    skip_spaces(line,j);
    cur = *(line+*j);
    while(cur != ',' && !isspace(cur) && cur != '\0' && cur != ')' && cur != '('){
        if(cur != '-' && cur != '+' && (cur >'9' || cur <'0'))
            error_msg(CHAR_NOT_INT,row);
        *(num+p) = cur;
        (*j)++;
        p++;
        cur = *(line+*j);
    }
    *(num+p) = '\0';

    return atoi(num);
}

/**
 * Checks if the character is a number.
 * @param line The line to be checked in.
 * @param j The place of the first character to be checked.
 * @return 1 if the characters are a number and 0 if not.
 */
int is_number(char *line,int j){
    int p = 0;
    int k = j;
    char cur;
    skip_spaces(line,&k);
    skip_commas(line,&k);
    skip_spaces(line,&k);
    cur = *(line+k);
    while(cur != ',' && !isspace(cur) && cur != '\0'){
        if((cur >'9' || cur <'0') && cur != '-' && cur != '+')
            return 0;
        k++;
        p++;
        cur = *(line+k);
    }

    if(p == 0)
        return 0;

    return 1;
}

/**
 * Skips the spaces in the line starting from place j.
 * @param line The line to be skipped at.
 * @param j The current place.
 */
void skip_spaces(char *line, int *j){
    while(isspace(*(line+(*j))) && *(line+(*j))!='\n')
        (*j)++;
}

/**
 * Checks if the commas are legal.
 * @param line The line to be checked at.
 * @param j The place in the line where there should be a comma.
 */
void check_commas(char *line, int *j){
    char cur = *(line+*j);
    if(cur =='\0')
        return;
    if(cur != ',' && cur !='\n')
        error_msg(MISSING_COMMA,row);
    if(cur == ',')
        (*j)++;
    skip_spaces(line,j);
    cur = *(line+*j);
    skip_spaces(line,j);
    if(cur == ',')
        error_msg(MULTIPLE_COMMAS,row);
}

/**
 * Skips the commas in the line starting from place j.
 * @param line The line to be skipped at.
 * @param j The current place.
 */
void skip_commas(char *line, int *j){
    while(*(line+(*j))==',')
        (*j)++;
}

/**
 * Converts a decimal number to a 12 digits binary number.
 * @param code The binary number to be created.
 * @param num The decimal number.
 */
void to_binary(int *code, int num){
    int k=11;
    while (num > 0) {
        *(code+k) = num % 2;
        num /= 2;
        k--;
    }

    while(k>=0){
        *(code+k) = 0;
        k--;
    }
}

/**
 * Creates new code line and returns it.
 * @param row The row of the line.
 * @param code The code.
 * @return The new code line.
 */
codeLine *new_code(int row, int *code){

    codeLine *new_codeLine = (codeLine *) malloc(sizeof(codeLine));
    new_codeLine->row = row;
    new_codeLine->code = code;
    new_codeLine->next = NULL;

    return new_codeLine;
}

/**
 * Returns the first word in the line starting from place i.
 * @param line The line to be checked.
 * @param i The place of the first character of the word.
 * @return The word
 */
char *get_word(char *line, int *i) {
    char *word;
    int word_length=1,start,j=0;

    while(*(line+ (*i)) !='(' &&(isspace(*(line+ (*i)))|| *(line+ (*i)) < 0 || *(line+ (*i))>127))
        (*i)++;

    start = *i;

    while(*(line+ (*i)) != '(' && *(line+ (*i)) != ':' &&(!isspace(*(line+ (*i))) && *(line+ (*i)) != ',' && *(line+ (*i)) !='\0' )){
        if(*(line+(*i)) == EOF)
            return NULL;
        word_length++;
        (*i)++;
    }

    if(*(line+ (*i)) == ':') {
        (*i)++;
        word_length++;
    }

    word = malloc(1+word_length);

    while(start<*i){
        *(word+j) = *(line+start);
        j++;
        start++;
    }

    *(word+j) = '\0';
    return word;
}

/**
 * Returns the first line in the input starting from place i.
 * @param input The input from the file.
 * @param i The place of the first character in the line.
 * @param input_length The length of the input.
 * @return The line.
 */
char *get_line(char *input, int *i, int input_length){
    char *line;
    int size = 1,j=*i;

    if(*i>= input_length)
        return NULL;

    while(*(input + j) != '\n'&& *(input + j) != EOF && *(input + j) != '\0'){
        size++;
        j++;
    }

    if(size > MAX_LINE_LENGTH) {
        error_msg(TOO_LONG_LINE, row);
        return NULL;
    }

    line = malloc(MAX_LINE_LENGTH);
    for(j=0;j<size;j++){
        *(line + j) = *(input + *i);
        (*i)++;
    }
    (*i)--;

    *(line+j) = '\0';
    return line;
}

/**
 * Returns the input from a file, and changes the value of input_length.
 * @param fname The name of the file.
 * @param input_length The length of the input.
 * @return The input.
 */
char *get_input(char *fname, int *input_length){
    char c,*input;
    int i;
    FILE *f = fopen(fname, "r");
    *input_length=0;
    for(c = getc(f); c != EOF;c= getc(f)){
        (*input_length)++;
    }

    fclose(f);
    f = fopen(fname,"r");

    i=0;
    input = malloc(1+*input_length);
    for(c = getc(f); c != EOF;c= getc(f)){
        if(c>0 && c<127) {
            *(input + i) = c;
            i++;
        }
    }

    *(input+i) = '\0';
    return input;

}
