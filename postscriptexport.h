#ifndef POSTSCRIPTEXPORT_H
#define POSTSCRIPTEXPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Constants for graph settings
#define POSTSCRIPT_WIDTH 600
#define POSTSCRIPT_HEIGHT 600
#define GRAPH_SCALE 500.0
#define AXIS_DIVISIONS 10
#define FONT_SCALE_FACTOR 0.5
#define LINE_WIDTH_DIVISOR 300
#define FONT_SIZE_CONST 8



// Function to find minimum and maximum in array
void find_min_max(const double* arr, int num_points, double* min, double* max);

// Export function to create a PostScript file
void export_to_postscript(const char* filename, const double *x_values, const double *y_values, int num_points,
                          double x_min, double x_max, double y_min, double y_max,
                          const char* function_label, const char* interval_label);

// Functions for writing parts of a PostScript file
void write_postscript_header(FILE* file, float line_width, float font_size);
void write_postscript_trailer(FILE* file);

// Functions for drawing different parts of the graph
void draw_grid_and_axes(FILE *file, double x_min, double x_max, double y_min, double y_max, double scale_factor, double font_size);
void draw_labels(FILE* file, double x_min, double x_max, double y_min, double y_max, double font_size, double scale_factor);
void draw_function_text(FILE *file, const char* function_label, const char* interval_label, double x_min, double x_max,
                        double y_max, double xy_scale, double font_size);

// Helper functions for calculating text size
double calculate_text_width_from_number(double number, double font_size);
double calculate_text_width_from_string(const char* str, double font_size);

#endif // POSTSCRIPTEXPORT_H
