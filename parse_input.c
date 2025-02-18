#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function_normalizer.h"
#include "parser_utils.h"
#include "parse_input.h"

// Function to check argument count
bool check_arg_count(int argc) {
    if (argc < 3 || argc > 4) {
        printf("[DEBUG]: Incorrect number of arguments: %d\n", argc);
        return false;
    }
    return true;
}

// Function to allocate and initialize input_params_t
input_params_t* allocate_params() {
    input_params_t* params = malloc(sizeof(input_params_t));
    if (!params) {
        printf("[DEBUG]: Memory allocation error for params\n");
        return NULL;
    }
    memset(params, 0, sizeof(input_params_t));  // Initialize all fields to zero
    return params;
}

// Function to extract the mathematical function string
bool extract_function_param(input_params_t* params, const char* arg) {
    if (params == NULL || arg == NULL) {
        return false;
    }

    params->function_str = extract_function(arg);
    if (!params->function_str) {
        printf("[DEBUG]: Failed to extract function string from argv[1]\n");
        return false;
    }
    printf("[DEBUG]: Extracted function: %s\n", params->function_str);
    return true;
}

// Function to extract the output file name
bool extract_output_file_param(input_params_t* params, const char* arg) {
    if (params == NULL || arg == NULL) {
        return false;
    }

    params->output_file_str = extract_output_file(arg);
    if (!params->output_file_str) {
        printf("[DEBUG]: Failed to extract output file name from argv[2]\n");
        return false;
    }
    printf("[DEBUG]: Extracted output file name: %s\n", params->output_file_str);
    return true;
}

// Function to parse limits
bool parse_limits_param(input_params_t* params, const char* arg) {
    if (params == NULL || arg == NULL) {
        return false;
    }

    printf("[DEBUG]: Parsing limits from argv[3]: %s\n", arg);
    if (!parse_limits(arg, &params->x_min, &params->x_max, &params->y_min, &params->y_max)) {
        printf("[DEBUG]: Failed to parse limits\n");
        return false;
    }
    printf("[DEBUG]: Successfully parsed limits: x_min=%f, x_max=%f, y_min=%f, y_max=%f\n",
           params->x_min, params->x_max, params->y_min, params->y_max);
    params->has_limits = true;
    return true;
}

// Function to normalize the function string
bool normalize_function_param(input_params_t* params) {
    if (params == NULL) {
        return false;
    }

    char* normalized_function = remove_spaces(params->function_str);
    if (!normalized_function) {
        printf("[DEBUG]: Failed to normalize function string\n");
        return false;
    }
    free(params->function_str);
    params->function_str = normalized_function;
    printf("[DEBUG]: Normalized function: %s\n", params->function_str);
    return true;
}

// Function to validate the mathematical expression
bool validate_expression_param(input_params_t* params) {
    if (params == NULL) {
        return false;
    }

    if (!is_valid_expression(params->function_str)) {
        printf("[DEBUG]: Function contains invalid characters or is incorrect\n");
        return false;
    }
    printf("[DEBUG]: Function is valid\n");
    return true;
}

// Function to check if limits are valid
bool check_limits_valid(input_params_t* params) {
    if (params == NULL) {
        return false;
    }

    if (params->x_min >= params->x_max || params->y_min >= params->y_max) {
        printf("[DEBUG]: Limits are invalid\n");
        return false;
    }
    printf("[DEBUG]: Limits are valid\n");
    return true;
}

// Let it stay for now, maybe it will be needed
void free_input_params(input_params_t *params) {
    if (params) {
        if (params->function_str) {
            free(params->function_str);
        }
        if (params->output_file_str) {
            free(params->output_file_str);
        }
        free(params);
    }
}
