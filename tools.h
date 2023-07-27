#pragma once

#include <stdio.h>
#include <stdbool.h>
#include "canvas.h"
#include "colors.h"

bool check_if_file_exists(char *filename);

int read_ascii_into_canvas(FILE *fp, canvas_pixel_t **canvas, int canvas_height, int canvas_width);
void get_ascii_width_height_from_file(FILE *fp, int *h, int *w);
void print_canvas(canvas_pixel_t **canvas, int canvas_height, int canvas_width);

canvas_pixel_t ** read_ascii_from_filepath(char *path, int *h, int *w);

int get_fg_color(int **color_array, int array_len, int color_pair);
int get_bg_color(int **color_array, int array_len, int color_pair);
