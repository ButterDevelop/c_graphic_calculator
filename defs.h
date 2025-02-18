#ifndef DEFS_H
#define DEFS_H

#define DEFAULT_MIN (-10.0)
#define DEFAULT_MAX  10.0


extern const char VALID_OPERATORS[];

extern const char* VALID_FUNCTIONS[];
// number of elements in the VALID_FUNCTIONS array.

extern const size_t NUM_VALID_FUNCTIONS;


#define VALID_VARIABLE 'x'

// Operators
#define OPERATOR_PLUS        '+'
#define OPERATOR_MINUS       '-'
#define OPERATOR_UNARY_MINUS '~'
#define OPERATOR_MULTIPLY    '*'
#define OPERATOR_DIVIDE      '/'
#define OPERATOR_POWER       '^'
#define OPERATOR_LEFT_PAREN  '('
#define OPERATOR_RIGHT_PAREN ')'

#define VALID_DEFINED_OPERATORS "+-*/^"

#define INTERVAL_STRING_FORMAT "x: [%.2f; %.2f], y: [%.2f; %.2f]"

#define BUFFER_SIZE 1024

// Allowed functions
#define FUNC_ABS  "abs"
#define FUNC_EXP  "exp"
#define FUNC_LN   "ln"
#define FUNC_LOG  "log"
#define FUNC_SIN  "sin"
#define FUNC_COS  "cos"
#define FUNC_TAN  "tan"
#define FUNC_ASIN "asin"
#define FUNC_ACOS "acos"
#define FUNC_ATAN "atan"
#define FUNC_SINH "sinh"
#define FUNC_COSH "cosh"
#define FUNC_TANH "tanh"

#define DECIMAL_POINT  '.'
#define MINUS_SIGN     '-'
#define DECIMAL_E_CHAR 'E'

#define DELIMITER ":"

#define NUMBER_ZERO  0
#define EMPTY_STRING ""
#define END_STRING_CHAR '\0'

#define X_STEP_VALUE 0.001
#define ABOUT_ZERO_CONST 1e-7

#endif