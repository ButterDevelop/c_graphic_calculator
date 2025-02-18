#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "function_normalizer.h"
#include "defs.h"
#include "parser_utils.h"

const char VALID_OPERATORS[] = VALID_DEFINED_OPERATORS;

// Valid functions are ordered by length (descending) to prevent functions from the conflict
// For example: sin(x) and sinh(x)
const char* VALID_FUNCTIONS[] = {
    FUNC_ASIN, FUNC_ACOS, FUNC_ATAN, FUNC_SINH, FUNC_COSH, FUNC_TANH,
    FUNC_ABS, FUNC_EXP, FUNC_LOG, FUNC_SIN, FUNC_COS, FUNC_TAN, FUNC_LN
};
const size_t NUM_VALID_FUNCTIONS = sizeof(VALID_FUNCTIONS) / sizeof(VALID_FUNCTIONS[0]);


char* remove_spaces(const char* function) {
    if (function == NULL) {
        return NULL;
    }

    size_t length = strlen(function);
    
    char* cleaned_str = (char*)malloc((length + 1) * sizeof(char));
    if (cleaned_str == NULL) {
        return NULL;
    }

    char* dest = cleaned_str;
    for (size_t i = 0; i < length; i++) {
        if (!isspace((unsigned char)function[i])) {
            *dest++ = function[i];
        }
    }
    *dest = END_STRING_CHAR;

    return cleaned_str;
}

bool is_valid_expression(char* function) {
    if (function == NULL) {
        return false;
    }

    size_t len = strlen(function);
    size_t i = 0;

    // First, we check the balance of the brackets
    if (!check_parentheses_balance(function)) {
        return false;
    }

    while (i < len) {
        len = strlen(function);

        // Checking functions
        if (check_valid_functions(function, &i)) {
            continue;
        }

        // Checking numbers
        if (isdigit(function[i]) || function[i] == DECIMAL_POINT || function[i] == DECIMAL_E_CHAR) {
            if (!check_number(function, &i)) {
                return false;
            }
            continue;
        }

        // Checking a variable
        if (function[i] == VALID_VARIABLE) {
            i++;
            continue;
        }

        // Checking operators
        if (strchr(VALID_OPERATORS, function[i]) != NULL) {
            i++;
            continue;
        }

        // Checking brackets
        if (function[i] == OPERATOR_LEFT_PAREN || function[i] == OPERATOR_RIGHT_PAREN) {
            i++;
            continue;
        }

        return false;
    }

    return true;
}

bool check_parentheses_balance(const char* expression) {
    if (expression == NULL) {
        return false;
    }

    int balance = 0;
    size_t i = 0;

    while (expression[i] != END_STRING_CHAR) {
        if (expression[i] == OPERATOR_LEFT_PAREN) {
            balance++;
        } else if (expression[i] == OPERATOR_RIGHT_PAREN) {
            balance--;
            if (balance < 0) {
                // A closing parenthesis without a corresponding opening parenthesis
                return false;
            }
        }
        i++;
    }

    // If the balance is zero, the brackets are balanced.
    return balance == 0;
}

bool check_valid_functions(const char* expression, size_t *index) {
    if (expression == NULL || index == NULL) {
        return false;
    }

    for (size_t j = 0; j < NUM_VALID_FUNCTIONS; j++) {
        size_t func_len = strlen(VALID_FUNCTIONS[j]);
        // Valid functions are ordered by length (descending) to prevent functions from the conflict
        // For example: sin(x) and sinh(x)
        if (strncmp(&expression[*index], VALID_FUNCTIONS[j], func_len) == 0) {
            *index += func_len;
            return true;
        }
    }
    return false;
}

// Helper Function to Convert Scientific Notation to Standard Decimal
bool convert_scientific_to_decimal(char* expression, size_t* index, char* buffer, size_t buffer_size) {
    size_t start = *index;
    size_t len = 0;

    // Temporary buffer to hold the number substring
    char number_substr[BUFFER_SIZE] = {0};

    // Copy the number substring
    // The condition needs to handle e/E and exponent parts
    while (isdigit(expression[*index]) || expression[*index] == DECIMAL_POINT ||
           expression[*index] == DECIMAL_E_CHAR ||
           (*index > NUMBER_ZERO && expression[*index - 1] == DECIMAL_E_CHAR &&
               (expression[*index] == OPERATOR_PLUS || expression[*index] == OPERATOR_MINUS)
           )
          ) {
        if (len >= BUFFER_SIZE - 1) {
            // Number too long
            return false;
        }
        number_substr[len++] = expression[*index];
        (*index)++;
    }
    number_substr[len] = END_STRING_CHAR;

    // Parse the number using strtod for better error handling
    double value;
    char* endptr;
    value = strtod(number_substr, &endptr);
    if (endptr == number_substr) {
        // Conversion failed
        return false;
    }

    // Convert the double value back to string in standard decimal format with sufficient precision
    // %.10f will give 10 decimal places; adjust as needed
    int written = snprintf(buffer, buffer_size, "%.10f", value);
    if (written < 0 || (size_t)written >= buffer_size) {
        return false;
    }

    // Remove trailing zeros
    remove_trailing_zeros(buffer);

    // Replace the original number substring with the standard decimal string
    size_t number_len = len;
    size_t decimal_len = strlen(buffer);

    if (decimal_len > number_len) {
        // Shift the remaining part of the expression to the right
        size_t shift = decimal_len - number_len;
        size_t expr_len = strlen(expression);
        if (expr_len + shift >= BUFFER_SIZE) {
            // Not enough space to shift
            return false;
        }
        memmove(&expression[start + decimal_len], &expression[start + number_len],
                expr_len - start - number_len + 1); // +1 to include null terminator
    } else if (decimal_len < number_len) {
        // Shift the remaining part of the expression to the left
        memmove(&expression[start + decimal_len], &expression[start + number_len],
                strlen(&expression[start + number_len]) + 1); // +1 to include null terminator
    }

    // Copy the standard decimal string into the expression
    memcpy(&expression[start], buffer, decimal_len);

    // Update the index to point after the new number
    *index = start + decimal_len;

    return true;
}

bool check_number(char* expression, size_t* index) {
    if (expression == NULL || index == NULL) {
        return false;
    }

    size_t original_index = *index;

    bool has_digits = false;
    bool has_decimal_point = false;

    // Buffer to hold the converted number
    char converted_number[BUFFER_SIZE] = {0};

    // Check for optional minus sign
    if (expression[*index] == MINUS_SIGN) {
        (*index)++;
    }

    // Check for digits and decimal point
    while (isdigit(expression[*index]) || expression[*index] == DECIMAL_POINT) {
        if (expression[*index] == DECIMAL_POINT) {
            if (has_decimal_point) {
                // Two decimal points are invalid
                *index = original_index;
                return false;
            }
            has_decimal_point = true;
        } else {
            has_digits = true;
        }
        (*index)++;
    }

    // Check for scientific notation
    if (expression[*index] == DECIMAL_E_CHAR) {
        (*index)++;
        // Check for optional '+' or '-' after 'E'
        if (expression[*index] == OPERATOR_PLUS || expression[*index] == OPERATOR_MINUS) {
            (*index)++;
        }
        // There must be at least one digit after 'E'
        if (!isdigit(expression[*index])) {
            // Invalid exponent
            *index = original_index;
            return false;
        }
        while (isdigit(expression[*index])) {
            (*index)++;
        }
    }

    // Now, determine if the number is in scientific notation
    bool is_scientific = false;
    for (size_t i = original_index; i < *index; i++) {
        if (expression[i] == DECIMAL_E_CHAR) {
            is_scientific = true;
            break;
        }
    }

    if (is_scientific) {
        // Convert scientific notation to standard decimal
        *index = original_index;
        if (!convert_scientific_to_decimal(expression, index, converted_number, BUFFER_SIZE)) {
            // Conversion failed
            *index = original_index;
            return false;
        }
        printf("[DEBUG]: Converted scientific notation to standard decimal: %s\n", converted_number);
    } else {
        // No conversion needed; ensure that there are digits
        if (!has_digits) {
            *index = original_index;
            return false;
        }
    }

    return true;
}


