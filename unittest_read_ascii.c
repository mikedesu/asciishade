#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include "canvas.h"

int main(int argc, char **argv) {
    int h = -1;
    int w = -1;
    canvas_pixel_t **canvas = NULL;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ascii_filepath>\n", argv[0]);
        return EXIT_FAILURE;
    }
    printf("Reading ascii file: %s\n", argv[1]);
    canvas = read_ascii_from_filepath(argv[1], &h, &w);
    //printf("Printing canvas:\n");
    //print_canvas(canvas, h, w);
    //printf("Size of canvas: %d x %d\n", h, w);
    //printf("Clearing canvas:\n");
    //clear_canvas(canvas, h, w);
    //printf("Printing canvas:\n");
    //print_canvas(canvas, h, w);
    printf("Freeing canvas:\n");
    free_canvas(canvas, h);
    //print_color_palette();
    //printf("Total color pairs for this ASCII: %u\n", count_color_palette());
    //print_ncurses_color_codes();
    return EXIT_SUCCESS;
}

