#ifndef FUNCTION_NORMALIZER_H
#define FUNCTION_NORMALIZER_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Removes spaces from a function string.
 * 
 * @param function A string with a function containing quotes and spaces
 * @return char* A new line where spaces are removed.
 * 
 * The function removes any unnecessary
 * spaces inside the function string for correct processing.
 */
char* remove_spaces(const char* function);

/**
 * @brief Checks a function string for invalid characters.
 * 
 * @param function String with mathematical function
 * @return bool Returns true if the function is valid.
 * 
 * Checks a string for only allowed characters: variable x,
 * mathematical operators, valid functions and numbers. If found
 * invalid characters, returns false.
 */
bool is_valid_expression(char* function);

/**
 * @brief Checks if a valid mathematical function is present in an expression.
 * 
 * @param expressionConst Line with expression
 * @param index Pointer to the current position in the line
 * @return bool Returns true if a valid function is found.
 * 
 * The function checks whether the substring at the current position starts with one of the allowed
 * mathematical functions. If a function is found, the index is updated by the length of the function found.
 */
bool check_valid_functions(const char* expression, size_t* index);

bool convert_scientific_to_decimal(char* expression, size_t* index, char* buffer, size_t buffer_size);

/**
 * @brief Checks if a substring is a valid number.
 * 
 * @param expression Line with expression
 * @param index Pointer to the current position in the line
 * @return bool Returns true if the number is correct.
 * 
 * The function checks whether a substring starts with a number (integer or floating point), 
 * including the possible sign of the number. If the number is correct, the index is updated by the length of the number.
 */
bool check_number(char* expression, size_t* index);

/**
 * @brief Checks the balance of parentheses in an expression.
 * 
 * @param expression Line with expression
 * @return bool Returns true if the parentheses are balanced.
 * 
 * The function checks that the number of opening and closing parentheses is the same,
 * and that each closing parenthesis corresponds to a previously opened parenthesis.
 */
bool check_parentheses_balance(const char* expression);

#endif
