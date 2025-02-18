#ifndef INPUT_PARSER_UTILS_H
#define INPUT_PARSER_UTILS_H

#include <stdbool.h>
#include "parse_input.h"

void remove_trailing_zeros(char* num_str);

/**
 * @brief Extracts a mathematical function from its arguments.
 * 
 * @param function_str String with mathematical function
 * @return char* Copy of the string with the math function, or NULL on error
 * 
 * The function returns a copy of the function string, ready for further processing.
 */
char* extract_function(const char* function_str);

bool is_valid_filename(const char* filename);

/**
 * @brief Extracts the output file name from the arguments.
 * 
 * @param output_file_str String with the name of the output file
 * @return char* A copy of the output PostScript file name, or NULL on error.
 * 
 * Returns a copy of the string with the name of the file where the graph will be saved.
 */
char* extract_output_file(const char* output_file_str);

/**
 * @brief Parses and validates display boundaries.
 * 
 * @param limits_str A string with boundaries in the format "x_min,x_max,y_min,y_max"
 * @param x_min Pointer to a variable for the lower bound of x
 * @param x_max Pointer to variable for upper bound of x
 * @param y_min Pointer to a variable for the lower bound of y
 * @param y_max Pointer to a variable for the upper bound of y
 * @return true if successful, false if error occurred.
 * 
 * The function extracts and checks the boundaries from a string, if present.
 */
bool parse_limits(const char* limits_str, double *x_min, double *x_max, double *y_min, double *y_max);

void set_default_limits(input_params_t* params);

// Check if a string is a supported function
bool is_supported_function(const char* func);

// Checking if a symbol is a binary operator
bool is_binary_operator(char ch);

#endif  // INPUT_PARSER_UTILS_H
