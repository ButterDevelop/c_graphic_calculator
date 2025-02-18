#include <stdio.h>
#include <stdlib.h>
#include "parse_input.h"
#include "defs.h"
#include "shuntingyard.h"
#include "postscriptexport.h"
#include "parser_utils.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 4 || argv == NULL) {
        return ERROR_ARG_COUNT;
    }

    // Check argument count
    if (!check_arg_count(argc)) {
        printf("Usage: %s <function> <output_file> [<limits>]\n", argv[0]);
        return ERROR_ARG_COUNT;
    }

    // Allocate params
    input_params_t* params = allocate_params();
    if (!params) {
        return ERROR_MEMORY_ALLOCATION;
    }

    // Extract function
    if (!extract_function_param(params, argv[1])) {
        free_input_params(params);
        return ERROR_INVALID_FUNCTION;
    }

    // Extract output file
    if (!extract_output_file_param(params, argv[2])) {
        free_input_params(params);
        return ERROR_OUTPUT_FILE;
    }

    // Parse limits if provided
    if (argc == 4) {
        if (!parse_limits_param(params, argv[3])) {
            free_input_params(params);
            return ERROR_INVALID_LIMITS;
        }
    } else {
        set_default_limits(params);
    }

    // Normalize function
    if (!normalize_function_param(params)) {
        free_input_params(params);
        return ERROR_INVALID_FUNCTION;
    }

    // Validate expression
    if (!validate_expression_param(params)) {
        free_input_params(params);
        return ERROR_INVALID_FUNCTION;
    }

    // Check limits
    if (!check_limits_valid(params)) {
        free_input_params(params);
        return ERROR_INVALID_LIMITS;
    }

    // Proceed with the rest of the program
    printf("[DEBUG]: All input parameters are valid.\n");

    printf("---------------------------------\n");
    printf("Parsed input:\n");
    printf("Function: %s\n",         params->function_str);
    printf("Output file: %s\n",      params->output_file_str);
    printf("X limits: [%lf, %lf]\n", params->x_min, params->x_max);
    printf("Y limits: [%lf, %lf]\n", params->y_min, params->y_max);
    printf("---------------------------------\n");

    TokenQueue token_queue;
    init_token_queue(&token_queue);
    bool success = parse_expression(params->function_str, &token_queue);
    if (!success) {
        printf("Unsuccessful expression parsing!\n");
        clear_token_queue(&token_queue);
        free_input_params(params);
        return ERROR_INVALID_FUNCTION;
    }

    // Calculate the number of points based on x limits and X_STEP_VALUE
    int num_points = (int)round((params->x_max - params->x_min) / X_STEP_VALUE);
    int real_num_points = 0;

    // Dynamically allocate memory for x and y arrays
    double* x = (double*)malloc(num_points * sizeof(double));
    double* y = (double*)malloc(num_points * sizeof(double));

    if (x == NULL || y == NULL) {
        perror("Failed to allocate memory");
        if (x != NULL) {
            free(x);
        }
        if (y != NULL) {
            free(y);
        }
        free_input_params(params);
        clear_token_queue(&token_queue);
        return ERROR_MEMORY_ALLOCATION;
    }

    // Fill x and y arrays
    double current_x = params->x_min;
    for (int i = 0; i < num_points; i++) {
        double result = evaluate_expression(&token_queue, current_x);

        if (result >= params->y_min && result <= params->y_max) {
            x[real_num_points] = current_x;
            y[real_num_points] = result;
            ++real_num_points;
        }
        current_x += X_STEP_VALUE;
    }

    char interval_label[100];  // Buffer for the interval string

    // Generate the interval label using params min and max values
    snprintf(interval_label, sizeof(interval_label), INTERVAL_STRING_FORMAT,
             params->x_min, params->x_max, params->y_min, params->y_max);

    // Export data to PostScript file
    export_to_postscript(params->output_file_str, x, y, real_num_points,
                         params->x_min, params->x_max, params->y_min, params->y_max,
                         params->function_str, interval_label);

    // Free allocated memory
    free(x);
    free(y);

    // Cleanup
    clear_token_queue(&token_queue);
    free_input_params(params);

    return SUCCESS;
}
