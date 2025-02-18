#ifndef INPUT_PARAMS_H
#define INPUT_PARAMS_H

#include <stdbool.h>

// Error codes
#define SUCCESS                 0 // Successful program completion
#define ERROR_ARG_COUNT         1 // Incorrect number of arguments
#define ERROR_INVALID_FUNCTION  2 // Invalid function
#define ERROR_OUTPUT_FILE       3 // Error creating or writing to file
#define ERROR_INVALID_LIMITS    4 // Error parsing limits
#define ERROR_MEMORY_ALLOCATION 5 // Memory allocation failed

// Structure to store program input parameters
typedef struct {
    char*  function_str;       // Mathematical function as a string
    char*  output_file_str;    // Output PostScript file name
    bool   has_limits;         // Flag indicating if limits are provided
    double x_min;              // Lower bound for x
    double x_max;              // Upper bound for x
    double y_min;              // Lower bound for y
    double y_max;              // Upper bound for y
} input_params_t;

// Function prototypes
bool check_arg_count(int argc);
input_params_t* allocate_params();
bool extract_function_param(input_params_t* params, const char* arg);
bool extract_output_file_param(input_params_t* params, const char* arg);
bool parse_limits_param(input_params_t* params, const char* arg);
bool normalize_function_param(input_params_t* params);
bool validate_expression_param(input_params_t* params);
bool check_limits_valid(input_params_t* params);

/**
 * @brief Frees the memory allocated for the input_params_t structure.
 *
 * @param params Pointer to input_params_t structure
 *
 * The function frees memory allocated for strings and dynamic data.
 */
void free_input_params(input_params_t* params);

#endif // INPUT_PARAMS_H
