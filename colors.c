#include "colors.h"
#include <ncurses.h>
#include <stdio.h>

#define CONVERT(c) (c*1000/255)

int convert_to_irc_color(int color) {
    switch (color) {
        case COLOR_BLACK:       return IRC_COLOR_BLACK;
        case COLOR_RED:         return IRC_COLOR_RED;
        case COLOR_GREEN:       return IRC_COLOR_GREEN;
        
        case COLOR_YELLOW:      return IRC_COLOR_YELLOW;
        
        case COLOR_BLUE:        return IRC_COLOR_BLUE;
        case COLOR_MAGENTA:     return IRC_COLOR_MAGENTA;
        case COLOR_CYAN:        return IRC_COLOR_CYAN;
        case COLOR_WHITE:       return IRC_COLOR_WHITE;   
        case COLOR_BROWN:       return IRC_COLOR_BROWN;   
        
        case COLOR_ORANGE:      return IRC_COLOR_ORANGE;  
        
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
        case IRC_COLOR_WHITE:       return COLOR_WHITE;
        case IRC_COLOR_BLACK:       return COLOR_BLACK;
        case IRC_COLOR_BLUE:        return COLOR_BLUE;
        case IRC_COLOR_GREEN:       return COLOR_GREEN;
        case IRC_COLOR_RED:         return COLOR_RED;
        case IRC_COLOR_BROWN:       return COLOR_BROWN;   
        case IRC_COLOR_MAGENTA:     return COLOR_MAGENTA;
        
        case IRC_COLOR_ORANGE:      return COLOR_ORANGE; 
        case IRC_COLOR_YELLOW:      return COLOR_YELLOW;
        
        case IRC_COLOR_LIGHT_GREEN: return COLOR_LIGHT_GREEN;
        case IRC_COLOR_LIGHT_CYAN:  return COLOR_LIGHT_CYAN;
        case IRC_COLOR_LIGHT_BLUE:  return COLOR_LIGHT_BLUE;
        case IRC_COLOR_PINK:        return COLOR_PINK;
        case IRC_COLOR_GREY:        return COLOR_GREY;
        case IRC_COLOR_LIGHT_GREY:  return COLOR_LIGHT_GREY;
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


// https://stackoverflow.com/questions/18551558/how-to-use-terminal-color-palette-with-curses
void define_colors() {
    // default colors already defined via use_default_colors()
    // colors below define the remaining 8 colors used by irc
    // extended colors 16-99
    // size of array is 99-16=84
    int colors[83][3] = {
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

        {0xff, 0x00, 0x00}, // 52
        {0xff, 0x8c, 0x00}, // 53
        {0xff, 0xff, 0x00}, // 54
        {0xb2, 0xff, 0x00}, // 55
        {0x00, 0xff, 0x00}, // 56
        {0x00, 0xff, 0xa0}, // 57
        {0x00, 0xff, 0xff}, // 58
        {0x00, 0x8c, 0xff}, // 59
        {0x00, 0x00, 0xff}, // 60
        {0xa5, 0x00, 0xff}, // 61
        {0xff, 0x00, 0xff}, // 62
        {0xff, 0x00, 0x98}, // 63

        {0xff, 0x59, 0x59}, // 64
        {0xff, 0xb4, 0x59}, // 65
        {0xff, 0xff, 0x71}, // 66
        {0xcf, 0xff, 0x60}, // 67
        {0x6f, 0xff, 0x6f}, // 68
        {0x65, 0xff, 0xc9}, // 69
        {0x6d, 0xff, 0xff}, // 70
        {0x59, 0xb4, 0xff}, // 71
        {0x59, 0x59, 0xff}, // 72
        {0xc4, 0x59, 0xff}, // 73
        {0xff, 0x66, 0xff}, // 74
        {0xff, 0x59, 0xbc}, // 75

        {0xff, 0x9c, 0x9c}, // 76
        {0xff, 0xd3, 0x9c}, // 77
        {0xff, 0xff, 0x9c}, // 78
        {0xe2, 0xff, 0x9c}, // 79
        {0x9c, 0xff, 0x9c}, // 80
        {0x9c, 0xff, 0xdb}, // 81
        {0x9c, 0xff, 0xff}, // 82
        {0x9c, 0xd3, 0xff}, // 83
        {0x9c, 0x9c, 0xff}, // 84
        {0xdc, 0x9c, 0xff}, // 85
        {0xff, 0x9c, 0xff}, // 86
        {0xff, 0x94, 0xd3}, // 87

        {0x00, 0x00, 0x00}, // 88
        {0x13, 0x13, 0x13}, // 89
        {0x28, 0x28, 0x28}, // 90
        {0x36, 0x36, 0x36}, // 91
        {0x4d, 0x4d, 0x4d}, // 92
        {0x65, 0x65, 0x65}, // 93
        {0x81, 0x81, 0x81}, // 94

        {0x9f, 0x9f, 0x9f}, // 95
        {0xbc, 0xbc, 0xbc}, // 96
        {0xe2, 0xe2, 0xe2}, // 97
        {0xff, 0xff, 0xff}, // 98
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
}

