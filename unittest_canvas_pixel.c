#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"

int main() {
    printf("Unit Test: canvas_pixel\n");

    canvas_pixel_t pixel, pixel2;
    pixel.character = L' ';
    pixel.foreground_color = 0;
    pixel.background_color = 0;

    pixel2.character = L' ';
    pixel2.foreground_color = 0;
    pixel2.background_color = 0;

    int canvas_pixel_equals_retval = canvas_pixel_equals(&pixel, &pixel2);
    if (canvas_pixel_equals_retval != 1) {
        printf("canvas_pixel_equals failed\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
