#pragma once

#include <stdio.h>
#include <stdbool.h>
#include "canvas.h"
#include "colors.h"

bool check_if_file_exists(char *filename);
bool iscomma(char c) ;

int read_ascii_into_canvas_unsafe(FILE *fp, canvas_pixel_t **canvas, int canvas_height, int canvas_width);
int get_fg_color(int **color_array, int array_len, int color_pair);
int get_bg_color(int **color_array, int array_len, int color_pair);

unsigned int count_color_palette();

void get_ascii_width_height_from_file(FILE *fp, int *h, int *w);
void print_canvas(canvas_pixel_t **canvas, int canvas_height, int canvas_width);
void get_colors_in_line(char *s);
void print_color_palette();

canvas_pixel_t ** read_ascii_from_filepath(char *path, int *h, int *w);

