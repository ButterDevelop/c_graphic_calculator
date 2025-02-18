#include "postscriptexport.h"
#include "defs.h"

// Function to find minimum and maximum in array
void find_min_max(const double *arr, int num_points, double *min, double *max) {
    if (arr == NULL || num_points <= 0 || min == NULL || max == NULL) {
        fprintf(stderr, "Error: invalid arguments in find_min_max\n");
        return;
    }
    *min = *max = arr[0];
    for (int i = 1; i < num_points; i++) {
        if (arr[i] < *min) *min = arr[i];
        if (arr[i] > *max) *max = arr[i];
    }
}

// Main export function to create a PostScript file
void export_to_postscript(const char* filename, const double *x_values, const double *y_values, int num_points,
                          double x_min, double x_max, double y_min, double y_max,
                          const char* function_label, const char* interval_label) {
    if (filename == NULL || x_values == NULL || y_values == NULL || num_points < 0 ||
        function_label == NULL || interval_label == NULL) {
        fprintf(stderr, "Error: Invalid arguments in export_to_postscript\n");
        return;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Calculating the scale to maintain the graph's proportions
    double xy_scale = (x_max - x_min) / (y_max - y_min);

    // Calculate font size and line thickness based on axis range
    double axis_range = fmax(x_max - x_min, (y_max - y_min) * xy_scale);
    double line_width = axis_range / LINE_WIDTH_DIVISOR;  // Adjusting line thickness
    double font_size  = FONT_SIZE_CONST * line_width;     // Setting the font size

    write_postscript_header(file, line_width, font_size); // Header entry

    double scale_ps_x = GRAPH_SCALE / (x_max - x_min);
    double scale_ps_y = GRAPH_SCALE / ((y_max - y_min) * xy_scale);

    fprintf(file, "75 300 translate\n");
    fprintf(file, "%.2f %.2f scale\n", scale_ps_x, scale_ps_y);
    fprintf(file, "%.2f %.2f translate\n", -x_min, -y_min * xy_scale);

    // Drawing axes, grid, labels and function text
    draw_grid_and_axes(file, x_min, x_max, y_min, y_max, xy_scale, font_size);
    draw_labels(file, x_min, x_max, y_min, y_max, font_size, xy_scale);
    draw_function_text(file, function_label, interval_label, x_min, x_max, y_max, xy_scale, font_size);

    // Drawing a function graph
    fprintf(file, "0 0 1 setrgbcolor\n"); 
    fprintf(file, "newpath\n");
    fprintf(file, "%.2f %.2f moveto\n", x_values[0], y_values[0] * xy_scale);
    for (int i = 1; i < num_points; i++) {
        if (fabs(x_values[i] - x_values[i - 1] - X_STEP_VALUE) < ABOUT_ZERO_CONST) {
            fprintf(file, "%.2f %.2f lineto\n", x_values[i], y_values[i] * xy_scale);
        } else {
            fprintf(file, "%.2f %.2f moveto\n", x_values[i], y_values[i] * xy_scale);
        }
    }
    fprintf(file, "stroke\n");

    write_postscript_trailer(file); // File End Recording
    fclose(file);
    printf("PostScript файл '%s' успешно создан.\n", filename);
}

// Function for writing the header of a PostScript file
void write_postscript_header(FILE *file, float line_width, float font_size) {
    if (file == NULL) {
        fprintf(stderr, "Error: Invalid file pointer in write_postscript_header\n");
        return;
    }

    fprintf(file, "%%!PS-Adobe-3.0\n");
    fprintf(file, "%%%%Author: Hleb Hnatsiuk\n");
    fprintf(file, "%%%%BoundingBox: 0 0 %d %d\n", POSTSCRIPT_WIDTH, POSTSCRIPT_HEIGHT);
    fprintf(file, "/Courier findfont %.2f scalefont setfont\n", font_size);
    fprintf(file, "/l {lineto} bind def\n");
    fprintf(file, "/m {moveto} bind def\n");
    fprintf(file, "/a {stroke} bind def\n");
    fprintf(file, "%.2f setlinewidth\n", line_width);
}

// Function to write the end of a PostScript file
void write_postscript_trailer(FILE *file) {
    if (file == NULL) {
        fprintf(stderr, "Error: Invalid file pointer in write_postscript_trailer\n");
        return;
    }

    fprintf(file, "showpage\n");
    fprintf(file, "%%%%Author: Hleb Hnatsiuk\n");
    fprintf(file, "%%%%Trailer\n");
}

// Function for drawing axes and grid
void draw_grid_and_axes(FILE *file, double x_min, double x_max, double y_min, double y_max, double xy_scale, double font_size) {
    if (file == NULL) {
        fprintf(stderr, "Error: Invalid file pointer in draw_axes\n");
        return;
    }

    // Drawing a grid
    fprintf(file, "0.8 setgray\n"); // Light grey color for the mesh

    double x_grid_interval = (x_max - x_min) / AXIS_DIVISIONS;
    double y_grid_interval = (y_max - y_min) / AXIS_DIVISIONS;

    // grid
    for (double i = x_min; i <= x_max; i += x_grid_interval) {
        fprintf(file, "newpath %.2f %.2f moveto %.2f %.2f lineto stroke\n",
                i, y_min * xy_scale, i, y_max * xy_scale);
    }
    for (double j = y_min; j <= y_max; j += y_grid_interval) {
        fprintf(file, "newpath %.2f %.2f moveto %.2f %.2f lineto stroke\n",
                x_min, j * xy_scale, x_max, j * xy_scale);
    }

    // Drawing axes
    fprintf(file, "0 setgray\n");
    fprintf(file, "newpath\n");
    fprintf(file, "0 %.2f moveto 0 %.2f lineto stroke\n",
            y_min * xy_scale, y_max * xy_scale); // Y-axis
    fprintf(file, "%.2f 0 moveto %.2f 0 lineto stroke\n",
            x_min, x_max); // X-axis

    // Drawing arrows and axis labels
    fprintf(file, "/Courier findfont %.2f scalefont setfont\n", font_size * 1.2);
    fprintf(file, "newpath\n");
    // X-axis label
    fprintf(file, "%.2f %.2f moveto (-> x) show\n",
            x_min + ((x_max - x_min) / 2) - (2 * font_size),
            y_min * xy_scale - (2 * font_size));
    // Y axis label
    fprintf(file, "90 rotate\n");
    fprintf(file, "%.2f %.2f moveto (-> f(x)) show\n",
            -((y_min + ((y_max - y_min) / 2)) * xy_scale + (2 * font_size)),
            -(x_min - 3 * font_size));
    fprintf(file, "-90 rotate\n");
}

// Function for drawing labels on axes
void draw_labels(FILE *file, double x_min, double x_max, double y_min, double y_max, double font_size, double xy_scale) {
    if (file == NULL) {
        fprintf(stderr, "Error: Invalid file pointer in draw_labels\n");
        return;
    }

    // Adjusting the label spacing to avoid overlapping
    double x_label_interval = (x_max - x_min) / AXIS_DIVISIONS;
    double y_label_interval = (y_max - y_min) / AXIS_DIVISIONS;

    fprintf(file, "/Courier findfont %.2f scalefont setfont\n", font_size);
    fprintf(file, "0.3 0.6 0.3 setrgbcolor\n");

    // Drawing Labels on the X-Axis
    for (double i = x_min; i <= x_max; i += x_label_interval) {
        double rounded_i = round(i * 100.0) / 100.0;

        // Calculating the approximate width of text
        double text_width = calculate_text_width_from_number(rounded_i, font_size);

        // Positioning the label
        fprintf(file, "%.2f %.2f moveto (%g) show\n",
                i - (text_width / 2), (y_min * xy_scale) - font_size, rounded_i);
    }

    // Drawing Labels on the Y-Axis
    for (double j = y_min; j <= y_max; j += y_label_interval) {
        double rounded_j = round(j * 100.0) / 100.0;

        // Calculating the approximate width of text
        double text_width = calculate_text_width_from_number(rounded_j, font_size);

        // Positioning the label
        fprintf(file, "%.2f %.2f moveto (%g) show\n",
                x_min - text_width - font_size, (j * xy_scale) - (font_size / 3), rounded_j);
    }
}

// Function to draw function text and interval
void draw_function_text(FILE *file, const char* function_label, const char* interval_label, double x_min, double x_max,
                        double y_max, double xy_scale, double font_size) {
    if (file == NULL || function_label == NULL || interval_label == NULL) {
        fprintf(stderr, "Error: invalid arguments in draw_function_text\n");
        return;
    }

    // Drawing a function description below the graph
    fprintf(file, "/Courier findfont %.2f scalefont setfont\n", font_size);
    fprintf(file, "0 setgray\n");  // Черный цвет для текста

    // Positions for labels
    double function_text_x = x_min + ((x_max - x_min) / 2);
    double function_text_y = y_max * xy_scale + (1.5 * font_size);

    // Drawing a function label
    fprintf(file, "%.2f %.2f moveto (%s) show\n",
            function_text_x - (calculate_text_width_from_string(function_label, font_size) / 2),
            function_text_y,
            function_label);

    // Draw interval label below function label
    fprintf(file, "%.2f %.2f moveto (%s) show\n",
            function_text_x - (calculate_text_width_from_string(interval_label, font_size) / 2),
            function_text_y - font_size,
            interval_label);
}

// Helper function for calculating text width from a number
double calculate_text_width_from_number(double number, double font_size) {
    // Convert number to string to get length
    char label[20]; // MAGIC?
    snprintf(label, sizeof(label), "%g", number); // Number formatting

    return calculate_text_width_from_string(label, font_size);
}

// Helper function for calculating text width from a string
double calculate_text_width_from_string(const char* str, double font_size) {
    if (str == NULL) {
        fprintf(stderr, "Error: invalid string in calculate_text_width_from_string\n");
        return 0.0;
    }

    int num_chars = strlen(str);
    // Estimate text width based on character count and font size
    return num_chars * font_size * FONT_SCALE_FACTOR;
}
