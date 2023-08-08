#include <stdlib.h>
#include <stdio.h>
#include "canvas.h"




int set_canvas_pixel(canvas_pixel_t *pixel, wchar_t character, int foreground_color, int background_color) {
    if (pixel == NULL) {
        return 0;
    }
    pixel->character = character;
    pixel->foreground_color = foreground_color;
    pixel->background_color = background_color;
    return 1;
}




canvas_pixel_t **init_canvas(int height, int width) {
    // do some basic checks on the input parameters
    if (width < 1 || height < 1) {
        fprintf(stderr, "Error: width and height must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
    canvas_pixel_t **canvas = calloc(height, sizeof(canvas_pixel_t *));
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
        clear_canvas_row(canvas[i], canvas_width);
    }
}




void clear_canvas_row(canvas_pixel_t *row, int width) {
    if (row == NULL) {
        fprintf(stderr, "Error in clear_canvas_row: row is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (width < 1) {
        fprintf(stderr, "Error in clear_canvas_row: row width must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < width; i++) {
        int r = set_canvas_pixel(&row[i], L' ', 0, 0);
        if (!r) {
            fprintf(stderr, "Error: in set_canvas_pixel\n");
            exit(EXIT_FAILURE);
        }
    }
}




void fill_canvas_row(canvas_pixel_t *row, int width, int fgcolor, int bgcolor) {
    if (row == NULL) {
        fprintf(stderr, "Error in fill_canvas_row: row is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (width < 1) {
        fprintf(stderr, "Error in fill_canvas_row: row width must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < width; i++) {
        set_canvas_pixel(&row[i], L' ', fgcolor, bgcolor);
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
        fill_canvas_row(canvas[i], canvas_width, fgcolor, bgcolor);
    }
}




void free_canvas(canvas_pixel_t **canvas, int canvas_height) {
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
        clear_canvas_row(dest[i], dest_w);
    }
}


void canvas_flip_horizontal(canvas_pixel_t **canvas, int canvas_height, int canvas_width) {
    if (canvas == NULL) {
        fprintf(stderr, "Error in canvas_flip_horizontal: canvas is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (canvas_height < 1 || canvas_width < 1) {
        fprintf(stderr, "Error in canvas_flip_horizontal: canvas height and width must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
    canvas_pixel_t *temp = calloc(canvas_width, sizeof(canvas_pixel_t));
    if (!temp) {
        fprintf(stderr, "Error: in calloc\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < canvas_height; i++) {
        for (int j = 0; j < canvas_width / 2; j++) {
            temp[j] = canvas[i][j];
            canvas[i][j] = canvas[i][canvas_width - j - 1];
            canvas[i][canvas_width - j - 1] = temp[j];
        }
    }
    free(temp);
}


void canvas_flip_vertical(canvas_pixel_t **canvas, int canvas_height, int canvas_width) {
    if (canvas == NULL) {
        fprintf(stderr, "Error in canvas_flip_vertical: canvas is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (canvas_height < 1 || canvas_width < 1) {
        fprintf(stderr, "Error in canvas_flip_vertical: canvas height and width must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    canvas_pixel_t *temp = calloc(canvas_width, sizeof(canvas_pixel_t));
    if (!temp) {
        fprintf(stderr, "Error: in calloc\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < canvas_height / 2; i++) {
        for (int j = 0; j < canvas_width; j++) {
            temp[j] = canvas[i][j];
            canvas[i][j] = canvas[canvas_height - i - 1][j];
            canvas[canvas_height - i - 1][j] = temp[j];
        }
    }
    free(temp);
}



int canvas_pixel_equals(canvas_pixel_t *pixel1, canvas_pixel_t *pixel2) {
    int retval = 0;
    if (pixel1 == NULL || pixel2 == NULL) {
        retval = 0;
    }
    else {
        retval = pixel1->character == pixel2->character &&
            pixel1->foreground_color == pixel2->foreground_color &&
            pixel1->background_color == pixel2->background_color;
    }
    return retval;
}


