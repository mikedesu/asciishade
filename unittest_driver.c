


#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include "canvas.h"

int main() 
{
    //char *filename = "small.ascii";
    int h = -1;
    int w = -1;

    //print_ncurses_color_codes();
    canvas_pixel_t **canvas = NULL;
    

    canvas = read_ascii_from_filepath("small.ascii", &h, &w);
    //read_ascii_from_filepath("test2.ascii", &h, &w);
    //read_ascii_from_filepath("test.ascii", &h, &w);

    print_canvas(canvas, h, w);

    free_canvas(canvas, h);
    

    return EXIT_SUCCESS;
}
