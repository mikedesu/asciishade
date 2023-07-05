#include "hud.h"
#include <ncurses.h>

// hud_color
// terminal_height
// terminal_width
//void draw_hud_background() {
void draw_hud_background(int color, int h, int w) {
    if (h < 3) {
        mPrint("Error: terminal height is too small to draw the hud\n");
        exit(EXIT_FAILURE);
    }
    else if (w < 1) {
        mPrint("Error: terminal width is too small to draw the hud\n");
        exit(EXIT_FAILURE);
    }
    int num_of_rows = 3;
    int starting_row = h - num_of_rows;
    attron(COLOR_PAIR(color));
    for (int j = starting_row; j < h; j++) { 
        for (int i = 0; i < w; i++) {
            mvaddstr(j, i, " ");
        }
    }
    attroff(COLOR_PAIR(color));
}
