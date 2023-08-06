#pragma once

#include <stdio.h>
#include <stdbool.h>
#include "canvas.h"
#include "colors.h"

#define FULL_BLOCK        L'█' //0x2588
#define UPPER_HALF_BLOCK  L'▀' //0x2580
#define BOTTOM_HALF_BLOCK L'▄' //0x2584
#define LEFT_HALF_BLOCK   L'▌' //0x258C
#define RIGHT_HALF_BLOCK  L'▐' //0x2590
#define BOTTOM_LEFT_BLOCK  L'▖' //0x2596
#define BOTTOM_RIGHT_BLOCK L'▗' //0x2597
#define UPPER_LEFT_BLOCK   L'▘' //0x2598
#define UPPER_LEFT_CORNER  L'▛' //0x259B
#define UPPER_RIGHT_CORNER L'▜' //0x259C
#define UPPER_RIGHT_BLOCK  L'▝' //0x259D
#define BOTTOM_LEFT_CORNER  L'▙' //0x2599
#define BOTTOM_RIGHT_CORNER L'▟' //0x259F
#define LIGHT_SHADE  L'░' //0x2591
#define MEDIUM_SHADE L'▒' //0x2592
#define DARK_SHADE   L'▓' //0x2593




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

