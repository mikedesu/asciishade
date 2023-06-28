#pragma once

#include <stdio.h>

void read_ascii_by_byte(FILE *fp);
void get_ascii_width_height_from_file(FILE *fp, int *w, int *h);
void read_ascii_from_filepath(char *path);
int convert_to_irc_color(int color);
int convert_to_ncurses_color(int irc_color);
