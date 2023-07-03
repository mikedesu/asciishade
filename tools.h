#pragma once

#include <stdio.h>
#include <stdbool.h>
#include "canvas.h"
#include "colors.h"

bool check_if_file_exists(char *filename);

void read_ascii_into_canvas(FILE *fp, canvas_pixel_t **canvas, int canvas_height, int canvas_width);
void get_ascii_width_height_from_file(FILE *fp, int *h, int *w);
void print_canvas(canvas_pixel_t **canvas, int canvas_height, int canvas_width);

canvas_pixel_t ** read_ascii_from_filepath(char *path, int *h, int *w);

