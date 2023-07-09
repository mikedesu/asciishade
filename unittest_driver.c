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
    printf("Printing canvas:\n");
    print_canvas(canvas, h, w);
    printf("Freeing canvas:\n");
    free_canvas(canvas, h);

    
    //canvas = read_ascii_from_filepath("test2.ascii", &h, &w);
    //print_canvas(canvas, h, w);
    //free_canvas(canvas, h);
    
    //canvas = read_ascii_from_filepath("test.ascii", &h, &w);
    //print_canvas(canvas, h, w);
    //free_canvas(canvas, h);


    print_ncurses_color_codes();

    return EXIT_SUCCESS;
}
