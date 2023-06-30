#pragma once

#define DEFAULT_FG_COLOR 7
#define DEFAULT_BG_COLOR 0

#define COLOR_BROWN       8
#define COLOR_ORANGE      9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN  11
#define COLOR_LIGHT_BLUE  12
#define COLOR_PINK        13
#define COLOR_GREY        14
#define COLOR_LIGHT_GREY  15

#define IRC_COLOR_WHITE 0
#define IRC_COLOR_BLACK 1
#define IRC_COLOR_BLUE 2
#define IRC_COLOR_GREEN 3
#define IRC_COLOR_RED 4
#define IRC_COLOR_BROWN 5
#define IRC_COLOR_MAGENTA 6
#define IRC_COLOR_ORANGE 7
#define IRC_COLOR_YELLOW 8
#define IRC_COLOR_LIGHT_GREEN 9
#define IRC_COLOR_CYAN 10
#define IRC_COLOR_LIGHT_CYAN 11
#define IRC_COLOR_LIGHT_BLUE 12
#define IRC_COLOR_PINK 13
#define IRC_COLOR_GREY 14
#define IRC_COLOR_LIGHT_GREY 15

int convert_to_irc_color(int color);
int convert_to_ncurses_color(int irc_color);
void print_ncurses_color_codes();
