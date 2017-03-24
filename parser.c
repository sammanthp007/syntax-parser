/* Done by Samman Bikram Thapa */
/* front.c - a lexical analyzer system for simple
   arithmetic expressions */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Global declarations */
/* Variables */
int charClass;
char lexeme [100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();

/* for error tracking */
int line_num=0;
int col_num=1;
size_t len = 0;
size_t read_so_far = 0;
char* curr_line = NULL;
char curr_read_so_far[1000];
int currCharIndex;
char last_read_char;
int to_break=0;
char last_read_lexem[1000];

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();

void term();
void factor();
void expr();
void error();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define NEW_LINE 33

/******************************************************/
/* main driver */
int main(int argc, char* argv[]) {
    /* Get the file name */
    if (argc != 2) {
        printf("Filename not provided.\n ./parser <filename> \n");
        exit(0);
    }

    char* filename = argv[1];
    /* Open the input data file and process its contents */
    if ((in_fp = fopen(filename, "r")) == NULL)
        printf("ERROR - cannot open %s\n", filename);
    else {
        while ((read_so_far = getline(&curr_line, &len, in_fp)) != -1) {
            /* for handling empty lines */
            if (read_so_far - 1 == 0) {
                printf("\nEmpty Line\n");
                continue;
            }

            strcpy(curr_read_so_far, "");
            strcpy(last_read_lexem, "");
            col_num = 1;
            printf("\n\n");
            line_num++;
            currCharIndex = 0;
            to_break = 0;
            getChar();
            if (curr_line != NULL) {
                do {
                    lex();
                    expr();
                    if (to_break == 1) {
                        break;
                    }
                } while (nextToken != EOF);
            }
        }
    }
}
/*****************************************************/
/* lookup - a function to lookup operators and ­
   parentheses
   and return the token */
int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '\n':
            addChar();
            nextToken = NEW_LINE;
            break;

        default:
            addChar();
            nextToken = EOF;
            break;
    }
    strcat(curr_read_so_far, lexeme);
    strcat(curr_read_so_far, " ");
    return
        nextToken;
}
/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else
        printf("Error - lexeme is too long \n");
}
/*****************************************************/
/* getChar - a function to get the next character of
   input and determine its character class */
void getChar() {
    if (curr_line[currCharIndex] != '\n' && curr_line[currCharIndex] != '\0') {
        col_num++;
        nextChar = curr_line[currCharIndex++];
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}
/*****************************************************/
/* getNonBlank - a function to call getChar until it
   returns a ­
   non-​­
   whitespace character */
void getNonBlank() {
    while (isspace(nextChar))
        getChar();
}


/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic
   expressions */
int lex() {
    lexLen = 0;
    getNonBlank();
    last_read_char = nextChar;
    strcpy(last_read_lexem, lexeme);
    switch (charClass) {
        /* Parse identifiers */
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }
            strcat(curr_read_so_far, lexeme);
            strcat(curr_read_so_far, " ");
            nextToken = IDENT;
            break;
            /* Parse integer literals */
        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
            strcat(curr_read_so_far, lexeme);
            strcat(curr_read_so_far, " ");
            nextToken = INT_LIT;
            break;
            /* Parentheses and operators */
        case UNKNOWN:
            lookup(nextChar);
            getChar();
            break;
            /* EOF */
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    } /* End of switch */
    printf("Next token is: %d, Next lexeme is %s\n",
            nextToken, lexeme);

    return nextToken;
} /* End of function lex */

/* expr
   Parses strings in the language generated by the rule:
   <expr> -> <term> {(+ | -) <term>}
   */
void expr() {
    printf("Enter <expr>\n");
    /* Parse the first term */
    term();
    if (to_break == 1) {
        return;
    }
    /* As long as the next token is + or -, get
       the next token and parse the next term */
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        term();
        if (to_break == 1) {
            return;
        }
    }
    printf("Exit <expr>\n");
} /* End of function expr */

/* term
   Parses strings in the language generated by the rule:
   <term> -> <factor> {(* | /) <factor>)
   */
void term() {
    printf("Enter <term>\n");
    /* Parse the first factor */
    factor();
    if (to_break == 1) {
        return;
    }
    /* As long as the next token is * or /, get the
       next token and parse the next factor */
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        lex();
        factor();
        if (to_break == 1) {
            return;
        }
    }
    printf("Exit <term>\n");
} /* End of function term */


/* factor
   Parses strings in the language generated by the rule:
   <factor> -> id | int_constant | ( <expr )
   */
void factor() {
    printf("Enter <factor>\n");
    /* Determine which RHS */
    if (nextToken == IDENT || nextToken == INT_LIT)
        /* Get the next token */
        lex();
    /* If the RHS is ( <expr>), call lex to pass over the
       left parenthesis, call expr, and check for the right
       parenthesis */
    else {
        if (nextToken == LEFT_PAREN) {
            lex();
            expr();
            if (nextToken == RIGHT_PAREN) {
                lex();
            }
            else {
                error();
                return;
            }
        } /* End of if (nextToken == ... */
        /* It was not an id, an integer literal, or a left
           parenthesis */
        else {
            error();
            return;
        }
    } /* End of else */
    printf("Exit <factor>\n");;
    /* End of function factor */
}

void error() {
    printf("In line %d:%d: %s", line_num, col_num, curr_line);
    printf("Syntax Error: %s\nError occurs at %s\n", curr_read_so_far, last_read_lexem);
    // exit(0);
    to_break = 1;
}
