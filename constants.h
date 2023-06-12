
#define START_IC 100
#define MIN_FOUR_DIGITS 1000
#define MAX_LINE_LENGTH 81

enum {NOT_ENOUGH_ARGS,FILE_NOT_FOUND, USED_LABEL,MISSING_COMMA,MULTIPLE_COMMAS, EXTERN_LABEL,UNKNOWN_COMMAND,NOT_INT,ILLEGAL_DEST,ILLEGAL_SOURCE,
        UNKNOWN_REG,ILLEGAL_LABEL,EXCESSIVE,MISSING_AFTER_LABEL,MISSING_PARAM,
        MISSING_ARG,COMMA_ISSUE,TOO_LONG_LINE,CHAR_NOT_INT,NO_STRING,NO_DATA,NO_EXTERN,NO_ENTRY, UNKNOWN_LABEL};
enum {MOV, CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP};

typedef struct macro{
    char *name;
    char *content;
    struct macro *next;
}macro;

typedef struct symbol{
    char *name;
    int value;
    char *flag;
    struct symbol *next;
}symbol;

typedef struct codeLine{
    int row;
    int *code;
    struct codeLine *next;
}codeLine;


