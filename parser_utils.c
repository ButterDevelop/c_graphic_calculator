#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser_utils.h"
#include "defs.h"

// Helper Function to Remove Trailing Zeros from Fractional Part
void remove_trailing_zeros(char* num_str) {
    char* decimal_ptr = strchr(num_str, DECIMAL_POINT);
    if (decimal_ptr == NULL) {
        return; // No decimal point, nothing to do
    }

    char* end_ptr = num_str + strlen(num_str) - 1;

    // Remove trailing zeros
    while (end_ptr > decimal_ptr && *end_ptr == '0') {
        *end_ptr = END_STRING_CHAR;
        end_ptr--;
    }

    // If the last character is a decimal point, remove it
    if (end_ptr == decimal_ptr) {
        *end_ptr = END_STRING_CHAR;
    }
}

char* extract_function(const char* function_str) {
    if (function_str == NULL) {
        return NULL;
    }

    char* function_copy = strdup(function_str);
    if (function_copy == NULL) {
        return NULL;
    }
    return function_copy;
}

// Implementation of filename validation
bool is_valid_filename(const char* filename) {
    if (filename == NULL || strlen(filename) == 0) {
        return false;
    }

    const char* invalid_chars = "<>:\"/\\|?*";
    for (size_t i = 0; i < strlen(filename); i++) {
        if (strchr(invalid_chars, filename[i]) != NULL) {
            return false;
        }
    }

    return true;
}

// Refactored extract_output_file function
char* extract_output_file(const char* output_file_str) {
    // Check if the output file string is NULL or empty
    if (output_file_str == NULL || strlen(output_file_str) == 0) {
        printf("[DEBUG]: Output file string is NULL or empty.\n");
        return NULL;
    }

    // Validate the filename
    if (!is_valid_filename(output_file_str)) {
        printf("[DEBUG]: Output file name contains invalid characters: %s\n", output_file_str);
        return NULL;
    }

    // Attempt to open the file for writing to verify creation and write permissions
    FILE* file = fopen(output_file_str, "w");
    if (file == NULL) {
        perror("[DEBUG]: Failed to create or open the output file");  // Use perror for more detailed error
        return NULL;
    }

    // Successfully opened the file, now close it
    fclose(file);

    // Duplicate the file string to store in params
    char* output_file_copy = strdup(output_file_str);
    if (output_file_copy == NULL) {
        printf("[DEBUG]: Memory allocation error for output_file_copy.\n");
        return NULL;
    }

    return output_file_copy;
}

bool parse_limits(const char* limits_str, double* x_min, double* x_max, double* y_min, double* y_max) {
    if (limits_str == NULL || x_min == NULL || x_max == NULL || y_min == NULL || y_max == NULL) {
        return false;
    }

    char* limits_copy = strdup(limits_str);
    if (limits_copy == NULL) {
        return false;
    }

    char* token;
    char* rest = limits_copy;

    token = strtok(rest, DELIMITER);
    if (token == NULL || sscanf(token, "%lf", x_min) != 1) {
        free(limits_copy);
        return false;
    }

    token = strtok(NULL, DELIMITER);
    if (token == NULL || sscanf(token, "%lf", x_max) != 1) {
        free(limits_copy);
        return false;
    }

    token = strtok(NULL, DELIMITER);
    if (token == NULL || sscanf(token, "%lf", y_min) != 1) {
        free(limits_copy);
        return false;
    }

    token = strtok(NULL, DELIMITER);
    if (token == NULL || sscanf(token, "%lf", y_max) != 1) {
        free(limits_copy);
        return false;
    }

    if (strtok(NULL, DELIMITER) != NULL) {
        free(limits_copy);
        return false;
    }

    free(limits_copy);
    return true;
}

// Function to set default limits
void set_default_limits(input_params_t* params) {
    if (params) {
        printf("[DEBUG]: Setting default limits\n");
        params->x_min = DEFAULT_MIN;
        params->x_max = DEFAULT_MAX;
        params->y_min = DEFAULT_MIN;
        params->y_max = DEFAULT_MAX;
        params->has_limits = false;
    }
}

// Check if a string is a supported function
bool is_supported_function(const char* func) {
    if (func == NULL) {
        return false;
    }

    return strcmp(func, FUNC_ABS)  == 0 || strcmp(func, FUNC_EXP)  == 0 || strcmp(func, FUNC_LN)   == 0 ||
           strcmp(func, FUNC_LOG)  == 0 || strcmp(func, FUNC_SIN)  == 0 || strcmp(func, FUNC_COS)  == 0 ||
           strcmp(func, FUNC_TAN)  == 0 || strcmp(func, FUNC_ASIN) == 0 || strcmp(func, FUNC_ACOS) == 0 ||
           strcmp(func, FUNC_ATAN) == 0 || strcmp(func, FUNC_SINH) == 0 || strcmp(func, FUNC_COSH) == 0 ||
           strcmp(func, FUNC_TANH) == 0;
}


// Checking if a symbol is a binary operator
bool is_binary_operator(char op) {
    return op == OPERATOR_PLUS || op == OPERATOR_MINUS || op == OPERATOR_MULTIPLY || op == OPERATOR_DIVIDE || op == OPERATOR_POWER;
}