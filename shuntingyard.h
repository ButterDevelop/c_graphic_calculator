#ifndef SHUNTINGYARD_H
#define SHUNTINGYARD_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_FUNC_NAME_LENGTH 8

// Token Type Definitions
typedef enum {
    TOKEN_NUMBER,
    TOKEN_VARIABLE,
    TOKEN_OPERATOR,
    TOKEN_LEFT_PAREN,
    TOKEN_FUNCTION
} TokenType;

// Defining the token structure
typedef struct Token {
    TokenType type;
    double value; // for numbers
    char op;      // for operators
    char func[MAX_FUNC_NAME_LENGTH]; // for functions
} Token;

// Node for token queue
typedef struct TokenNode {
    Token token;
    struct TokenNode* next;
} TokenNode;

// Structure for a token queue
typedef struct {
    TokenNode* front;
    TokenNode* rear;
} TokenQueue;

// Token Stack
typedef struct TokenStackNode {
    Token token;
    struct TokenStackNode* next;
} TokenStackNode;

typedef struct {
    TokenStackNode* top;
} TokenStack;


/* ____________________________________________________________________________

    TokenQueue
   ____________________________________________________________________________
*/
void init_token_queue(TokenQueue* queue);
void clear_token_queue(TokenQueue* queue);
void copy_token_queue(const TokenQueue* src, TokenQueue* dest);
void enqueue_token(TokenQueue* queue, Token token);
bool dequeue_token(TokenQueue* queue, Token* token);


/* ____________________________________________________________________________

    TokenStack
   ____________________________________________________________________________
*/
void init_token_stack(TokenStack* stack);
void clear_token_stack(TokenStack* stack);
void push_token(TokenStack* stack, Token token);
bool pop_token(TokenStack* stack, Token* token);
bool peek_token(const TokenStack* stack, Token* token);



// Parsing an expression into reverse polish notation
bool parse_expression(const char* expr, TokenQueue* output_queue);

//Calculating the result of an expression based on the RPN
double evaluate_expression(const TokenQueue* queue, double x);

// Getting operator priority
int get_operator_precedence(char op);

#endif // SHUNTINGYARD_H
