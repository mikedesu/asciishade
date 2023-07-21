#pragma once

#include <wchar.h>

// this is the base of the canvas
typedef struct {
    wchar_t character;
    int foreground_color;
    int background_color;
} canvas_pixel_t;

canvas_pixel_t **init_canvas(int height, int width);
void free_canvas(canvas_pixel_t **canvas, int canvas_height);
void clear_canvas_row_unsafe(canvas_pixel_t *row, int width);
void clear_canvas(canvas_pixel_t **canvas, int canvas_height, int canvas_width);
void fill_canvas_row_unsafe(canvas_pixel_t *row, int width, int fgcolor, int bgcolor);
void fill_canvas(canvas_pixel_t **canvas, int canvas_height, int canvas_width, int fgcolor, int bgcolor);

void copy_canvas_row(canvas_pixel_t *dest, canvas_pixel_t *src, int dest_w, int src_w);
void copy_canvas(canvas_pixel_t **dest, canvas_pixel_t **src, int dest_h, int dest_w, int src_h, int src_w);



void canvas_flip_horizontal(canvas_pixel_t **canvas, int canvas_height, int canvas_width);
void canvas_flip_vertical(canvas_pixel_t **canvas, int canvas_height, int canvas_width);
