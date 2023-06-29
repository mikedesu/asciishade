#pragma once

#include <stdio.h>
#include "canvas.h"

#define COLOR_BRIGHT_BLACK   8
#define COLOR_BRIGHT_BLUE    9
#define COLOR_BRIGHT_GREEN   10
#define COLOR_BRIGHT_CYAN    11
#define COLOR_BRIGHT_RED     12
#define COLOR_BRIGHT_MAGENTA 13
#define COLOR_BRIGHT_YELLOW  14
#define COLOR_BRIGHT_WHITE   15

void read_ascii_into_canvas(FILE *fp, canvas_pixel_t **canvas, int canvas_height, int canvas_width);
void get_ascii_width_height_from_file(FILE *fp, int *h, int *w);
void print_ncurses_color_codes();
void print_canvas(canvas_pixel_t **canvas, int canvas_height, int canvas_width);

int convert_to_irc_color(int color);
int convert_to_ncurses_color(int irc_color);

canvas_pixel_t ** read_ascii_from_filepath(char *path, int *h, int *w);

