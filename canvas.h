#pragma once

#include <wchar.h>

// this is the base of the canvas
typedef struct 
{
    wchar_t character;
    int foreground_color;
    int background_color;
} canvas_pixel_t;


typedef struct 
{
    int width;
    int height;
    canvas_pixel_t **canvas;
} canvas_t;


canvas_pixel_t **init_canvas(int width, int height);
void free_canvas(canvas_pixel_t **canvas, int canvas_height);


