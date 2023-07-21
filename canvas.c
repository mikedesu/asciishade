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
    if (!canvas) {
	    fprintf(stderr, "Error: in calloc\n");
	    exit(EXIT_FAILURE);
    }
    for (int i = 0; i < height; i++) {
        canvas[i] = calloc(width, sizeof(canvas_pixel_t));
        if (!canvas[i]) {
            fprintf(stderr, "Error: in calloc\n");
            exit(EXIT_FAILURE);
        }
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


void fill_canvas_row_unsafe(canvas_pixel_t *row, int width, int fgcolor, int bgcolor) {
    if (row == NULL) {
        fprintf(stderr, "Error: row is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (width < 1) {
        fprintf(stderr, "Error: row width must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < width; i++) {
        row[i].foreground_color = fgcolor;
        row[i].background_color = bgcolor;
        row[i].character = ' ';
    }
}



void fill_canvas(canvas_pixel_t **canvas, int canvas_height, int canvas_width, int fgcolor, int bgcolor) {
    if (canvas == NULL) {
        fprintf(stderr, "Error: canvas is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (canvas_height < 1 || canvas_width < 1) {
        fprintf(stderr, "Error: canvas height and width must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < canvas_height; i++) {
        fill_canvas_row_unsafe(canvas[i], canvas_width, fgcolor, bgcolor);
    }
}


void free_canvas(canvas_pixel_t **canvas, int canvas_height) {
    printf("Freeing the canvas memory...\n");
    for (int i = 0; i < canvas_height; i++) {
        free(canvas[i]);
    }
    free(canvas);
}


void copy_canvas_row(canvas_pixel_t *dest, canvas_pixel_t *src, int dest_w, int src_w) {
    int i;
    if (dest == NULL || src == NULL) {
        fprintf(stderr, "copy_canvas_row: Error: dest or src is NULL\n");
        exit(EXIT_FAILURE);
    }
    // dest_w must be greater than 1
    // src_w must be greater than 1
    if (dest_w < 1 || src_w < 1) {
        fprintf(stderr, "copy_canvas_row: Error: dest_w and src_w must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    if (dest_w < src_w) {
        src_w = dest_w;
    }
    for (i = 0; i < src_w; i++) {
        dest[i] = src[i];
    }
    // fill the rest with spaces
    for (; i < dest_w; i++) {
        dest[i].character = L' ';
        dest[i].foreground_color = 0;
        dest[i].background_color = 0;
    }
}


void copy_canvas(canvas_pixel_t **dest, canvas_pixel_t **src, int dest_h, int dest_w, int src_h, int src_w) {
    int i;
    if (dest_h < src_h) {
        src_h = dest_h;
    }
    for (i = 0; i < src_h; i++) {
        copy_canvas_row(dest[i], src[i], dest_w, src_w);
    }
    // fill the rest with spaces
    for (; i < dest_h; i++) {
        clear_canvas_row_unsafe(dest[i], dest_w);
    }
}


