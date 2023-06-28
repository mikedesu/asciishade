#pragma once

#include <stdio.h>
#include "canvas.h"

void read_ascii_into_canvas(FILE *fp, canvas_pixel_t **canvas, int canvas_height, int canvas_width);
void get_ascii_width_height_from_file(FILE *fp, int *w, int *h);
void read_ascii_from_filepath(char *path);
int convert_to_irc_color(int color);
int convert_to_ncurses_color(int irc_color);
