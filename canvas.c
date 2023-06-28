#include <stdlib.h>
#include <stdio.h>
#include "canvas.h"



canvas_pixel_t **init_canvas(int height, int width) 
{
    canvas_pixel_t **canvas = NULL;

    // do some basic checks on the input parameters
    if (width < 1 || height < 1) 
    {
        fprintf(stderr, "Error: width and height must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    //canvas_width  = width;
    //canvas_height = height;
    canvas = calloc(height, sizeof(canvas_pixel_t *));
    for (int i = 0; i < height; i++) 
    {
        canvas[i] = calloc(width, sizeof(canvas_pixel_t));
        for (int j = 0; j < width; j++) 
        {
            canvas[i][j].foreground_color = 0;
            canvas[i][j].background_color = 0;
            canvas[i][j].character = ' ';
        }
    }

    return canvas;
}



void free_canvas(canvas_pixel_t **canvas, int canvas_height)
{
    printf("Freeing the canvas memory...\n");
    for (int i = 0; i < canvas_height; i++) 
    {
        free(canvas[i]);
    }
    free(canvas);
}


