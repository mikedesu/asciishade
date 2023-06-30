#include "colors.h"
#include <ncurses.h>
#include <stdio.h>

int convert_to_irc_color(int color) {
    switch (color) {
        case COLOR_BLACK:       return IRC_COLOR_BLACK;
        case COLOR_RED:         return IRC_COLOR_RED;
        case COLOR_GREEN:       return IRC_COLOR_GREEN;
        case COLOR_YELLOW:      return IRC_COLOR_YELLOW;
        case COLOR_BLUE:        return IRC_COLOR_BLUE;
        case COLOR_MAGENTA:     return IRC_COLOR_MAGENTA;
        case COLOR_CYAN:        return IRC_COLOR_CYAN;
        case COLOR_WHITE:       return IRC_COLOR_WHITE;   // white
        case COLOR_BROWN:       return IRC_COLOR_BROWN;   // brown
        case COLOR_ORANGE:      return IRC_COLOR_ORANGE;  // orange
        case COLOR_LIGHT_GREEN: return IRC_COLOR_LIGHT_GREEN;
        case COLOR_LIGHT_CYAN:  return IRC_COLOR_LIGHT_CYAN;
        case COLOR_LIGHT_BLUE:  return IRC_COLOR_LIGHT_BLUE;
        case COLOR_PINK:        return IRC_COLOR_PINK;
        case COLOR_GREY:        return IRC_COLOR_GREY;
        case COLOR_LIGHT_GREY:  return IRC_COLOR_LIGHT_GREY;
        default: return color;
    }
    return 0;
}


int convert_to_ncurses_color(int irc_color) {
    // this is the inverse-function of convert_to_irc_color
    switch (irc_color) {
        case IRC_COLOR_WHITE:   return COLOR_WHITE;
        case IRC_COLOR_BLACK:   return COLOR_BLACK;
        case IRC_COLOR_BLUE:    return COLOR_BLUE;
        case IRC_COLOR_GREEN:   return COLOR_GREEN;
        case IRC_COLOR_RED:     return COLOR_RED;
        case IRC_COLOR_BROWN:   return COLOR_BROWN;   // brown
        case IRC_COLOR_MAGENTA: return COLOR_MAGENTA;
        case IRC_COLOR_ORANGE:  return COLOR_ORANGE; // orange
        case IRC_COLOR_YELLOW:  return COLOR_YELLOW;
        case IRC_COLOR_LIGHT_GREEN:  return COLOR_LIGHT_GREEN;
        case IRC_COLOR_LIGHT_CYAN: return COLOR_LIGHT_CYAN;
        case IRC_COLOR_LIGHT_BLUE: return COLOR_LIGHT_BLUE;
        case IRC_COLOR_PINK: return COLOR_PINK;
        case IRC_COLOR_GREY: return COLOR_GREY;
        case IRC_COLOR_LIGHT_GREY: return COLOR_LIGHT_GREY;
        default: return irc_color;
    }
    return 0;
}


void print_ncurses_color_codes() {
    printf("COLOR_BLACK: %d\n", COLOR_BLACK);
    printf("COLOR_RED: %d\n", COLOR_RED);
    printf("COLOR_GREEN: %d\n", COLOR_GREEN);
    printf("COLOR_YELLOW: %d\n", COLOR_YELLOW);
    printf("COLOR_BLUE: %d\n", COLOR_BLUE);
    printf("COLOR_MAGENTA: %d\n", COLOR_MAGENTA);
    printf("COLOR_CYAN: %d\n", COLOR_CYAN);
    printf("COLOR_WHITE: %d\n", COLOR_WHITE);
    //printf("COLOR_BRIGHT_BLACK: %d\n", COLOR_BRIGHT_BLACK);
    //printf("COLOR_BRIGHT_BLUE: %d\n", COLOR_BRIGHT_BLUE);
    //printf("COLOR_BRIGHT_GREEN: %d\n", COLOR_BRIGHT_GREEN);
    //printf("COLOR_BRIGHT_CYAN: %d\n", COLOR_BRIGHT_CYAN);
    //printf("COLOR_BRIGHT_RED: %d\n", COLOR_BRIGHT_RED);
    //printf("COLOR_BRIGHT_MAGENTA: %d\n", COLOR_BRIGHT_MAGENTA);
    //printf("COLOR_BRIGHT_YELLOW: %d\n", COLOR_BRIGHT_YELLOW);
    //printf("COLOR_BRIGHT_WHITE: %d\n", COLOR_BRIGHT_WHITE);
    printf("COLOR_BROWN: %d\n", COLOR_BROWN);
    printf("COLOR_ORANGE: %d\n", COLOR_ORANGE);
    printf("COLOR_LIGHT_GREEN: %d\n", COLOR_LIGHT_GREEN);
    printf("COLOR_LIGHT_CYAN: %d\n", COLOR_LIGHT_CYAN);
    printf("COLOR_LIGHT_BLUE: %d\n", COLOR_LIGHT_BLUE);
    printf("COLOR_PINK: %d\n", COLOR_PINK);
    printf("COLOR_GREY: %d\n", COLOR_GREY);
    printf("COLOR_LIGHT_GREY: %d\n", COLOR_LIGHT_GREY);
}


