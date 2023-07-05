#pragma once

#include "canvas.h"

void draw_hud_background(int color, int h, int w);

void draw_hud_row_1(canvas_pixel_t **canvas, int **color_array, int color_array_len, char *filename, int y, int x, int hud_color, int h, int w, int current_color_pair, bool is_text_mode);

void draw_hud_row_2(canvas_pixel_t **canvas, int **color_array, int color_array_len, int **color_pair_array, int color_pair_array_len, int terminal_height, int terminal_width, int hud_color, int current_color_pair, int y, int x, int last_char_pressed);

void switch_between_hud_and_current_color(int hud_color, int current_color_pair);

void switch_between_current_and_hud_color(int hud_color, int current_color_pair);
