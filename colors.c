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
    printf("COLORS: %d\n", COLORS);
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


#define CONVERT(c) (c*1000/255)

// https://stackoverflow.com/questions/18551558/how-to-use-terminal-color-palette-with-curses
void define_colors() {
    // default colors already defined via use_default_colors()
    // colors below define the remaining 8 colors used by irc

    //int _127 = convert_to_ncurses_color_channel(127);
    //int _192 = convert_to_ncurses_color_channel(192);
    //int _252 = convert_to_ncurses_color_channel(252);
    //int _ff = convert_to_ncurses_color_channel(255);
    //int _a5 = convert_to_ncurses_color_channel(0xA5);
    //
    //

    // extended colors 16-99
    // size of array is 99-16=84
    int colors[84][3] = {
        {0x47, 0x00, 0x00}, // 16
        {0x47, 0x21, 0x00}, // 17
        {0x47, 0x47, 0x00}, // 18
        {0x32, 0x47, 0x00}, // 19
        {0x00, 0x47, 0x00}, // 20
        {0x00, 0x47, 0x2c}, // 21
        {0x00, 0x47, 0x47}, // 22
        {0x00, 0x27, 0x47}, // 23
        {0x00, 0x00, 0x47}, // 24
        {0x2e, 0x00, 0x47}, // 25
        {0x47, 0x00, 0x47}, // 26

        {0x47, 0x00, 0x2a}, // 27
        {0x74, 0x00, 0x00}, // 28
        {0x74, 0x3a, 0x00}, // 29
        {0x74, 0x74, 0x00}, // 30
        {0x51, 0x74, 0x00}, // 31
        {0x00, 0x74, 0x00}, // 32
        {0x00, 0x74, 0x49}, // 33
        {0x00, 0x74, 0x74}, // 34
        {0x00, 0x40, 0x74}, // 35
        {0x00, 0x00, 0x74}, // 36
        {0x4b, 0x00, 0x74}, // 37
        {0x74, 0x00, 0x74}, // 38
        {0x74, 0x00, 0x45}, // 39
        
        {0xb5, 0x00, 0x00}, // 40
        {0xb5, 0x63, 0x00}, // 41
        {0xb5, 0xb5, 0x00}, // 42
        {0x7d, 0xb5, 0x00}, // 43
        {0x00, 0xb5, 0x00}, // 44
        {0x00, 0xb5, 0x71}, // 45
        {0x00, 0xb5, 0xb5}, // 46
        {0x00, 0x63, 0xb5}, // 47
        {0x00, 0x00, 0xb5}, // 48
        {0x75, 0x00, 0xb5}, // 49
        {0xb5, 0x00, 0xb5}, // 50
        {0xb5, 0x00, 0x6b}, // 51

        {0x00, 0x00, 0x00}, // 52
        {0x00, 0x00, 0x00}, // 53
        {0x00, 0x00, 0x00}, // 54
        {0x00, 0x00, 0x00}, // 55
        {0x00, 0x00, 0x00}, // 56
        {0x00, 0x00, 0x00}, // 57
        {0x00, 0x00, 0x00}, // 58
        {0x00, 0x00, 0x00}, // 59
        {0x00, 0x00, 0x00}, // 60
        {0x00, 0x00, 0x00}, // 61
        {0x00, 0x00, 0x00}, // 62
        {0x00, 0x00, 0x00}, // 63

        {0x00, 0x00, 0x00}, // 64
        {0x00, 0x00, 0x00}, // 65
        {0x00, 0x00, 0x00}, // 66
        {0x00, 0x00, 0x00}, // 67
        {0x00, 0x00, 0x00}, // 68
        {0x00, 0x00, 0x00}, // 69
        {0x00, 0x00, 0x00}, // 70
        {0x00, 0x00, 0x00}, // 71
        {0x00, 0x00, 0x00}, // 72
        {0x00, 0x00, 0x00}, // 73
        {0x00, 0x00, 0x00}, // 74
        {0x00, 0x00, 0x00}, // 75

        {0x00, 0x00, 0x00}, // 76
        {0x00, 0x00, 0x00}, // 77
        {0x00, 0x00, 0x00}, // 78
        {0x00, 0x00, 0x00}, // 79
        {0x00, 0x00, 0x00}, // 80
        {0x00, 0x00, 0x00}, // 81
        {0x00, 0x00, 0x00}, // 82
        {0x00, 0x00, 0x00}, // 83
        {0x00, 0x00, 0x00}, // 84
        {0x00, 0x00, 0x00}, // 85
        {0x00, 0x00, 0x00}, // 86
        {0x00, 0x00, 0x00}, // 87

        {0x00, 0x00, 0x00}, // 88
        {0x00, 0x00, 0x00}, // 89
        {0x00, 0x00, 0x00}, // 90
        {0x00, 0x00, 0x00}, // 91
        {0x00, 0x00, 0x00}, // 92
        {0x00, 0x00, 0x00}, // 93
        {0x00, 0x00, 0x00}, // 94

        {0x00, 0x00, 0x00}, // 95
        {0x00, 0x00, 0x00}, // 96
        {0x00, 0x00, 0x00}, // 97
        {0x00, 0x00, 0x00}, // 98
        {0x00, 0x00, 0x00}, // 99
    };
    size_t colors_size = sizeof(colors) / sizeof(colors[0]);


    init_color(COLOR_BROWN,       647,  164,  164 );
    init_color(COLOR_ORANGE,      1000, 647,  0   );
    init_color(COLOR_LIGHT_GREEN, 564,  933,  564 );
    init_color(COLOR_LIGHT_CYAN,  878,  1000, 1000);
    init_color(COLOR_LIGHT_BLUE,  678,  847,  901 );
    init_color(COLOR_PINK,        1000, 752,  796 );
    init_color(COLOR_GREY,        501,  501,  501 );
    init_color(COLOR_LIGHT_GREY,  827,  827,  827 );
    
    for(size_t i=0; i < colors_size; i++) {
        int r = CONVERT(colors[i][0]);
        int g = CONVERT(colors[i][1]);
        int b = CONVERT(colors[i][2]);
        init_color(16+i, r, g, b);
    }

    //init_color(16, CONVERT(0x47), CONVERT(0x00), CONVERT(0x00));
    //init_color(17, CONVERT(0x47), CONVERT(0x21), CONVERT(0x00));
    //init_color(18, CONVERT(0x47), CONVERT(0x47), CONVERT(0x00));
    //init_color(19, CONVERT(0x32), CONVERT(0x47), CONVERT(0x00));
    //init_color(20, CONVERT(0x00), CONVERT(0x47), CONVERT(0x00));
    //init_color(21, CONVERT(0x00), CONVERT(0x47), CONVERT(0x2c));
    //init_color(22, CONVERT(0x00), CONVERT(0x47), CONVERT(0x47));
}


// color_value is a value between 0 and 255
int convert_to_ncurses_color_channel(int color_value) {
    return (color_value/255) * 1000;
}

