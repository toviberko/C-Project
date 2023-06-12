#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"

extern int row;
extern int is_error;
extern int empty_obj;

/********* MAIN FUNCTIONS ***********/
void error_msg(int n, int row);
FILE *macro_file(FILE *f, char *fname);
symbol *firstPass(char *fname);

/********** FIRST PASS FUNCTIONS ************/
int is_label(char *word);
int number_of_values(char *line, int *j);
int number_of_chars(char *line, int *j);
int used_label(char *word);
symbol *new_label(char *label, int value,char *flag);
int num_of_words(char *line, int *j, char *command, int *prm1,int *prm2,int *arg1,int *arg2);
int enum_command(char *command);
int is_register(char *word);
int num_of_params(char *word,int *j,int *prm1, int *prm2);
int *op_code(char *command,int *prm1,int *prm2,int *arg1,int *arg2);
int is_immediate(char *word);
void address_method(int *method, int *code, int place);
void check_address_method(int arg1,int arg2,int command_num);
int illegal_label(char *word);
int end_of_line(char *line, int *j);

/************ SECOND PASS FUNCTIONS ****************/
void secondPass(char *fname, symbol *symbol_head, codeLine *code_head,int DC,int IC);
char *bin_to_unique(int *code);
void write_immediate(char *line, int *j,codeLine *code_head);
void write_label(char *line, int *j, symbol *symbol_head, codeLine *code_head, FILE *ext_file);
void write_register(char *line, int *j, int is_dest, int two_registers, codeLine *code_head);
void switch_bit(int *bit);
void print_code(codeLine *code_head, int *code);

/************ GENERAL FUNCTIONS ****************/
int get_number(char *line, int *j);
int is_number(char *line,int j);
void skip_spaces(char *line, int *j);
void check_commas(char *line, int *j);
void skip_commas(char *line, int *j);
void to_binary(int *code, int num);
codeLine *new_code(int row, int *code);
char *get_word(char *input, int *i);
char *get_line(char *input, int *i,int input_length);
char *get_input(char *fname, int *input_length);
