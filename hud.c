#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hud.h"
#include "tools.h"
#include "mPrint.h"

// hud_color
// terminal_height
// terminal_width
//void draw_hud_background() {
void draw_hud_background(
    int color, 
    int h, 
    int w) {
    
    if (h < 3) {
        mPrint("Error: terminal height is too small to draw the hud\n");
        exit(EXIT_FAILURE);
    }
    else if (w < 1) {
        mPrint("Error: terminal width is too small to draw the hud\n");
        exit(EXIT_FAILURE);
    }
    int starting_row = h - HUD_NUM_ROWS;
    attron(COLOR_PAIR(color));
    for (int j = starting_row; j < h; j++) { 
        for (int i = 0; i < w; i++) {
            mvaddstr(j, i, " ");
        }
    }
    attroff(COLOR_PAIR(color));
}


void draw_hud_row_1(
    canvas_pixel_t **canvas, 
    int **color_array, 
    int color_array_len, 
    char *filename, 
    int y, 
    int x, 
    int hud_color, 
    int term_h, 
    int term_w, 
    int current_color_pair, 
    bool is_text_mode) {
    
    if (canvas == NULL) {
        mPrint("canvas is null");
        exit(EXIT_FAILURE);
    }
    else if (term_h < 1) {
        mPrint("term_h is less than 1");
        exit(EXIT_FAILURE);
    }
    else if (term_w < 1) {
        mPrint("term_w is less than 1");
        exit(EXIT_FAILURE);
    }
    else if (color_array == NULL) {
        mPrint("color_array is null");
        exit(EXIT_FAILURE);
    }
    else if (color_array_len <= 0) {
        mPrint("color_array_len <= 0");
        exit(EXIT_FAILURE);
    }
    attron(COLOR_PAIR(hud_color));
    int hud_y     = term_h-HUD_NUM_ROWS;
    int hud_x     = 0;
    //int hud_terminal_width = w;
    int fg_color  = get_fg_color(color_array, color_array_len, current_color_pair);
    int fg_color_cursor = canvas[y][x].foreground_color;
    // perhaps the string could be longer than the hud_terminal_width,
    // since we are going to stop printing there anyway
    // estimating length of the hud status row

    // filename could be NULL
    // so this would be at minimum (24 + 1)*2 length
    // it would make more sense to make it the terminal width
    //int len_of_str = (24 + strlen(filename) + 1)*2;
    //char *str = calloc(1, len_of_str);
    //char *str = calloc(1, terminal_width);
    char *str = calloc(1, term_w);
    if (str == NULL) {
        mPrint("Error: calloc failed\n");
        exit(EXIT_FAILURE);
    }
    // before we do this sprintf, we need to make sure the terminal is wide enough
    // to display the entire string.  if it's not, we need to truncate the string
    // and display a warning message
    //sprintf(
    snprintf(
        str, 
        term_w,
        "y:%03d|#%02d(%02x)F%02d %s", 
        y, 
        fg_color, 
        current_color_pair, 
        fg_color_cursor, 
        is_text_mode ? "TEXT" : "NORMAL"
    );
    // get the length of str
    int str_len = strlen(str);
    if (str_len > term_w) {
        // truncate the string
        str[term_w-1] = '\0';
    }
    move(hud_y,hud_x);
    for (char c = str[0]; c != '\0'; c = str[hud_x]) {
        if (hud_x > term_w) {
            break;
        }
        if (c=='#') {
            switch_between_hud_and_current_color(hud_color, current_color_pair);
            addstr("█");
            switch_between_current_and_hud_color(hud_color, current_color_pair);
        }
        else {
            addch(c);
        }
        hud_x++;
    }
    free(str);
    attroff(COLOR_PAIR(hud_color));
}


void draw_hud_row_2(
    canvas_pixel_t **canvas, 
    int **color_array, 
    int color_array_len, 
    int **color_pair_array, 
    int color_pair_array_len, 
    int terminal_height, 
    int terminal_width, 
    int hud_color, 
    int current_color_pair, 
    int y, 
    int x, 
    int last_char_pressed) {

    if (canvas == NULL) {
        mPrint("Error: canvas is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (terminal_height < 0) {
        mPrint("Error: terminal_height is negative\n");
        exit(EXIT_FAILURE);
    }
    if (terminal_width < 0) {
        mPrint("Error: terminal_width is negative\n");
        exit(EXIT_FAILURE);
    }
    if (hud_color < 0) {
        mPrint("Error: hud_color is negative\n");
        exit(EXIT_FAILURE);
    }
    if (current_color_pair < 0) {
        mPrint("Error: current_color_pair is negative\n");
        exit(EXIT_FAILURE);
    }
    if (color_array == NULL) {
        mPrint("Error: color_array is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (color_array_len <= 0) {
        mPrint("Error: color_array_len is less than or equal to 0\n");
        exit(EXIT_FAILURE);
    }
    if (color_pair_array == NULL) {
        mPrint("Error: color_pair_array is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (color_pair_array_len <= 0) {
        mPrint("Error: color_pair_array_len is less than or equal to 0\n");
        exit(EXIT_FAILURE);
    }

    attron(COLOR_PAIR(hud_color));
    int hud_y            = terminal_height-HUD_NUM_ROWS+1;
    int hud_x            = 0;
    //const int len_of_str = 40;
    move(hud_y,hud_x);
    char *str = calloc(1, terminal_width);
    if (str == NULL) {
        mPrint("Error allocating memory for str\n");
        exit(EXIT_FAILURE);
    }
    int bg_color = get_bg_color(color_array, color_array_len, current_color_pair);
    int fg_color_cursor = canvas[y][x].foreground_color;
    int bg_color_cursor = canvas[y][x].background_color;
    int color_pair_num = color_pair_array[fg_color_cursor][bg_color_cursor];
    //sprintf(str, 
    snprintf(str, terminal_width,
        "x:%03d|#%02d(%02x)B%02d %dx%d %d", 
        x, 
        bg_color, 
        color_pair_num, 
        bg_color_cursor, 
        terminal_height,
        terminal_width, 
        last_char_pressed
    );
    for (char c = str[0]; c != '\0'; c = str[hud_x]) {
        if (hud_x > terminal_width) {
            break;
        }
        else if (c=='#') {
            switch_between_hud_and_current_color(hud_color, current_color_pair);
            addstr(" ");
            switch_between_current_and_hud_color(hud_color, current_color_pair);
        }
        else {
            addch(c);
        }
        hud_x++;
    }
    free(str);
    attroff(COLOR_PAIR(hud_color));
}



void draw_hud_row_3(
    int terminal_height,
    int terminal_width,
    int hud_color,
    long last_cmd_ns
) {
    
    if (terminal_height < 0) {
        mPrint("Error: terminal_height is negative\n");
        exit(EXIT_FAILURE);
    }

    if (terminal_width < 0) {
        mPrint("Error: terminal_width is negative\n");
        exit(EXIT_FAILURE);
    }


    int starting_row = terminal_height - HUD_NUM_ROWS + 2;

    char *str = calloc(1, terminal_width);
    if (str == NULL) {
        mPrint("Error: calloc failed\n");
        exit(EXIT_FAILURE);
    }

    snprintf(str, terminal_width, "Press q to quit | %ldns", last_cmd_ns);

    attron(COLOR_PAIR(hud_color));
    mvaddstr(starting_row, 0, str);
    attroff(COLOR_PAIR(hud_color));
}



void switch_between_hud_and_current_color(int hud_color, int current_color_pair) { 
    attroff(COLOR_PAIR(hud_color));  
    attron(COLOR_PAIR(current_color_pair)); 
}

void switch_between_current_and_hud_color(int hud_color, int current_color_pair) {
    attroff(COLOR_PAIR(current_color_pair));  
    attron(COLOR_PAIR(hud_color)); 
}


