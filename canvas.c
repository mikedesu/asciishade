#include <stdlib.h>
#include <stdio.h>
#include "canvas.h"

canvas_pixel_t **init_canvas(int height, int width) {
    canvas_pixel_t **canvas = NULL;
    // do some basic checks on the input parameters
    if (width < 1 || height < 1) {
        fprintf(stderr, "Error: width and height must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
    canvas = calloc(height, sizeof(canvas_pixel_t *));
    for (int i = 0; i < height; i++) {
        canvas[i] = calloc(width, sizeof(canvas_pixel_t));
    }
    clear_canvas(canvas, height, width);
    return canvas;
}


void clear_canvas(canvas_pixel_t **canvas, int canvas_height, int canvas_width) {
    if (canvas == NULL) {
        fprintf(stderr, "Error: canvas is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (canvas_height < 1 || canvas_width < 1) {
        fprintf(stderr, "Error: canvas height and width must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < canvas_height; i++) {
        clear_canvas_row_unsafe(canvas[i], canvas_width);
    }
}


void clear_canvas_row_unsafe(canvas_pixel_t *row, int width) {
    if (row == NULL) {
        fprintf(stderr, "Error: row is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (width < 1) {
        fprintf(stderr, "Error: row width must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < width; i++) {
        row[i].foreground_color = 0;
        row[i].background_color = 0;
        row[i].character = ' ';
    }
}


void free_canvas(canvas_pixel_t **canvas, int canvas_height) {
    printf("Freeing the canvas memory...\n");
    for (int i = 0; i < canvas_height; i++) {
        free(canvas[i]);
    }
    free(canvas);
}

