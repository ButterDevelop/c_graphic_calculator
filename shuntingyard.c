#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include "shuntingyard.h"
#include "defs.h"
#include "parser_utils.h"




/* ____________________________________________________________________________

    TokenQueue
   ____________________________________________________________________________
*/
// Initializing the queue and stack
void init_token_queue(TokenQueue* queue) {
    if (queue == NULL) {
        return;
    }

    queue->front = queue->rear = NULL;
}

// Clearing the queue and stack
void clear_token_queue(TokenQueue* queue) {
    if (queue == NULL) {
        return;
    }

    TokenNode* current = queue->front;
    while (current != NULL) {
        TokenNode* temp = current;
        current = current->next;
        free(temp);
    }
    queue->front = queue->rear = NULL;
}

// Copying a token queue
void copy_token_queue(const TokenQueue* src, TokenQueue* dest) {
    if (src == NULL || dest == NULL) {
        return;
    }

    // Initializing a new queue
    init_token_queue(dest);

    // Pointer to move through the source queue
    TokenNode *current = src->front;
    while (current != NULL) {
        // Copy the token
        Token token_copy = current->token;

        // Add a copy of the token to a new queue
        enqueue_token(dest, token_copy);

        // Let's move on to the next node
        current = current->next;
    }
}

// Queue functions
void enqueue_token(TokenQueue *queue, Token token) {
    if (queue == NULL) {
        return;
    } 

    TokenNode* node = (TokenNode*)malloc(sizeof(TokenNode));
    node->token = token;
    node->next = NULL;
    if (queue->rear == NULL) {
        queue->front = queue->rear = node;
    } else {
        queue->rear->next = node;
        queue->rear = node;
    }
}

bool dequeue_token(TokenQueue *queue, Token *token) {
    if (queue == NULL || token == NULL) {
        return false;
    }
    //FIXME: maybe one of the check queue leave
    if (queue->front == NULL) {
        return false;
    }
    TokenNode *node = queue->front;
    *token = node->token;
    queue->front = queue->front->next;
    if (queue->front == NULL) queue->rear = NULL;
    free(node);
    return true;
}




/* ____________________________________________________________________________

    TokenStack
   ____________________________________________________________________________
*/
void init_token_stack(TokenStack* stack) {
    if (stack == NULL) {
        return;
    } 
    stack->top = NULL;
}

void clear_token_stack(TokenStack *stack) {
    if (stack == NULL) {
        return;
    }

    while (stack->top != NULL) {
        TokenStackNode** temp = &stack->top;
        stack->top = stack->top->next;
        free(*temp);
    }
}

// Stack functions
void push_token(TokenStack* stack, Token token) {
    if (stack == NULL)  {
        return;
    }

    TokenStackNode* node = (TokenStackNode*)malloc(sizeof(TokenStackNode));
    node->token = token;
    node->next  = stack->top;
    stack->top  = node;
}

bool pop_token(TokenStack* stack, Token *token) {
    if (stack == NULL || token == NULL) {
         return false;
    }

    if (stack->top == NULL) {
        return false;
    }
    TokenStackNode* node = stack->top;
    *token = node->token;
    stack->top = stack->top->next;
    free(node);
    return true;
}

// View the top item of a stack without removing it
bool peek_token(const TokenStack* stack, Token *token) {
    if (stack == NULL || token == NULL) {
        return false;
    } 

    // Check that the stack is not empty
    if (stack->top == NULL) {
        return false; // The stack is empty, nothing to view
    }

    // Copy the top element of the stack to `token`
    *token = stack->top->token;
    return true; // Successfully returned the top element
}




/* ____________________________________________________________________________

    Беспредел
   ____________________________________________________________________________
*/
// Parsing an expression into reverse polish notation with function support
bool parse_expression(const char* expr, TokenQueue* output_queue) {
    if (expr == NULL || output_queue == NULL) {
        return false;
    }

    TokenStack op_stack;
    init_token_stack(&op_stack);

    for (int i = 0; expr[i] != END_STRING_CHAR; i++) {
        char ch = expr[i];
        if (isdigit(ch) || ch == DECIMAL_POINT) {  // Number
            Token token = {TOKEN_NUMBER, atof(&expr[i]), NUMBER_ZERO, EMPTY_STRING};
            enqueue_token(output_queue, token);
            while (isdigit(expr[i]) || expr[i] == DECIMAL_POINT) i++;  // Skip the rest of the numbers
            i--;
        } else if (ch == VALID_VARIABLE) {  // Variable
            Token token = {TOKEN_VARIABLE, NUMBER_ZERO, NUMBER_ZERO, EMPTY_STRING};
            enqueue_token(output_queue, token);
        } else if (isalpha(ch)) {  // Function discovered
            char func[10] = {0};
            int j = 0;
            while (isalpha(expr[i]) && j < 9) func[j++] = expr[i++];
            func[j] = END_STRING_CHAR;
            i--;
            if (is_supported_function(func)) {
                Token token = {TOKEN_FUNCTION, NUMBER_ZERO, NUMBER_ZERO, EMPTY_STRING};
                strcpy(token.func, func);
                push_token(&op_stack, token);  // Adding a function to the stack
            } else {
                return false;  // Function not supported
            }
        } else if (ch == OPERATOR_MINUS) {  // Checking for unary or binary minus
            if (i == 0 || expr[i - 1] == TOKEN_LEFT_PAREN || is_binary_operator(expr[i - 1])) {
                // If the first character is after '(' or operator, then it is unary minus
                Token unary_minus_token = {TOKEN_OPERATOR, NUMBER_ZERO, OPERATOR_UNARY_MINUS, EMPTY_STRING};
                push_token(&op_stack, unary_minus_token);
            } else {
                // Otherwise it's a binary minus
                Token op_token = {TOKEN_OPERATOR, NUMBER_ZERO, ch, EMPTY_STRING};
                Token top_token;
                while (peek_token(&op_stack, &top_token) &&
                       top_token.type == TOKEN_OPERATOR &&
                       get_operator_precedence(top_token.op) >= get_operator_precedence(ch)) {
                    pop_token(&op_stack, &top_token);
                    enqueue_token(output_queue, top_token);
                }
                push_token(&op_stack, op_token);
            }
        } else if (is_binary_operator(ch)) {  // Operator
            Token op_token = {TOKEN_OPERATOR, NUMBER_ZERO, ch, EMPTY_STRING};
            Token top_token;
            while (peek_token(&op_stack, &top_token) &&
                   top_token.type == TOKEN_OPERATOR &&
                   get_operator_precedence(top_token.op) >= get_operator_precedence(ch)) {
                pop_token(&op_stack, &top_token);
                enqueue_token(output_queue, top_token);
            }
            push_token(&op_stack, op_token);
        } else if (ch == OPERATOR_LEFT_PAREN) {  // Left bracket
            Token token = {TOKEN_LEFT_PAREN, NUMBER_ZERO, NUMBER_ZERO, EMPTY_STRING};
            push_token(&op_stack, token);
        } else if (ch == OPERATOR_RIGHT_PAREN) {  // Right bracket
            Token top_token;
            while (pop_token(&op_stack, &top_token) && top_token.type != TOKEN_LEFT_PAREN) {
                enqueue_token(output_queue, top_token);
            }
            if (top_token.type != TOKEN_LEFT_PAREN) return false;  // Error: unbalanced parentheses

            // If there is a function on the top of the stack, move it to the output queue
            if (peek_token(&op_stack, &top_token) && top_token.type == TOKEN_FUNCTION) {
                pop_token(&op_stack, &top_token);
                enqueue_token(output_queue, top_token);
            }
        }
    }

    // Transferring the remaining operators to the queue
    Token top_token;
    while (pop_token(&op_stack, &top_token)) {
        if (top_token.type == TOKEN_LEFT_PAREN) return false;  // Error: unbalanced parentheses
        enqueue_token(output_queue, top_token);
    }

    return true;
}

// Calculate the result for a given operator
double calculate_operator(char op, double lhs, double rhs) {
    switch (op) {
        case OPERATOR_PLUS:
            return lhs + rhs;
        case OPERATOR_MINUS:
            return lhs - rhs;
        case OPERATOR_MULTIPLY:
            return lhs * rhs;
        case OPERATOR_DIVIDE:
            if (rhs == 0) {
                fprintf(stderr, "Error: Division by zero.\n");
                return NAN; // Return NaN in case of division by zero
            }
            return lhs / rhs;
        case OPERATOR_POWER:
            return pow(lhs, rhs); // Exponentiation
        case OPERATOR_UNARY_MINUS:  // Unary minus (works with only one operand)
            return -rhs;
        default:
            fprintf(stderr, "Error: Unsupported operator '%c'.\n", op);
            return NAN; // Return NaN for unsupported operators
    }
}

// FIXME may need to be changed to a more elegant solution later, without so many if else
// Calculate the result for a supported function
double calculate_function(const char* func, double arg) {
    if (func == NULL) {
        return NAN;
    }

    if (strcmp(func, FUNC_ABS) == 0) {
        return fabs(arg);
    } else if (strcmp(func, FUNC_EXP) == 0) {
        return exp(arg);
    } else if (strcmp(func, FUNC_LN) == 0) {
        if (arg <= 0) {
            return NAN;
        }
        return log(arg); // natural logarithm
    } else if (strcmp(func, FUNC_LOG) == 0) {
        if (arg <= 0) {
            return NAN;
        }
        return log10(arg); // common logarithm
    } else if (strcmp(func, FUNC_SIN) == 0) {
        return sin(arg);
    } else if (strcmp(func, FUNC_COS) == 0) {
        return cos(arg);
    } else if (strcmp(func, FUNC_TAN) == 0) {
        return tan(arg);
    } else if (strcmp(func, FUNC_ASIN) == 0) {
        if (arg < -1 || arg > 1) {
            return NAN;
        }
        return asin(arg);
    } else if (strcmp(func, FUNC_ACOS) == 0) {
        if (arg < -1 || arg > 1) {
            return NAN;
        }
        return acos(arg);
    } else if (strcmp(func, FUNC_ATAN) == 0) {
        return atan(arg);
    } else if (strcmp(func, FUNC_SINH) == 0) {
        return sinh(arg);
    } else if (strcmp(func, FUNC_COSH) == 0) {
        return cosh(arg);
    } else if (strcmp(func, FUNC_TANH) == 0) {
        return tanh(arg);
    } else {
        fprintf(stderr, "Error: Unsupported function '%s'.\n", func);
        return NAN; // Return NaN for unsupported features
    }
}

// Calculating the result of an expression based on the RPN
double evaluate_expression(const TokenQueue* queue, double x) {
    if (queue == NULL) {
        return NAN;
    }

    TokenStack value_stack;
    init_token_stack(&value_stack);

    TokenQueue local_queue;
    copy_token_queue(queue, &local_queue);

    Token token;
    while (dequeue_token(&local_queue, &token)) {
        if (token.type == TOKEN_NUMBER) {
            push_token(&value_stack, token);
        } else if (token.type == TOKEN_VARIABLE) {
            Token var_token = {TOKEN_NUMBER, x, NUMBER_ZERO, EMPTY_STRING};
            push_token(&value_stack, var_token);
        } else if (token.type == TOKEN_OPERATOR) {
            if (token.op == OPERATOR_UNARY_MINUS) {
                Token operand;
                pop_token(&value_stack, &operand);
                double result = calculate_operator(token.op, 0, operand.value); // Unary minus works with one operand.
                Token result_token = {TOKEN_NUMBER, result, NUMBER_ZERO, EMPTY_STRING};
                push_token(&value_stack, result_token);
            } else {
                Token rhs, lhs;
                pop_token(&value_stack, &rhs);
                pop_token(&value_stack, &lhs);
                double result = calculate_operator(token.op, lhs.value, rhs.value);
                Token result_token = {TOKEN_NUMBER, result, NUMBER_ZERO, EMPTY_STRING};
                push_token(&value_stack, result_token);
            }
        } else if (token.type == TOKEN_FUNCTION) {
            Token arg;
            pop_token(&value_stack, &arg);
            double result = calculate_function(token.func, arg.value);
            Token result_token = {TOKEN_NUMBER, result, NUMBER_ZERO, EMPTY_STRING};
            push_token(&value_stack, result_token);
        }
    }

    Token result_token;
    pop_token(&value_stack, &result_token);

    clear_token_stack(&value_stack);
    clear_token_queue(&local_queue);

    return result_token.value;
}

// Getting operator priority
int get_operator_precedence(char op) {
    switch (op) {
        case OPERATOR_PLUS:
        case OPERATOR_MINUS:       return 1;
        case OPERATOR_MULTIPLY:
        case OPERATOR_DIVIDE:      return 2;
        case OPERATOR_UNARY_MINUS: return 3;
        case OPERATOR_POWER:       return 4;
        default: return 0;
    }
}