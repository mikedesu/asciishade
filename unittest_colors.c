


#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>


int main() {

    initscr();
    clear();

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    // as it turns out, tmux doesn't allow for color changing
    // so we need to check for that
    if (can_change_color() == FALSE) {
        endwin();
        printf("Your terminal can not change color\n");
        exit(1);
    }

    start_color();
    use_default_colors();

//#define COLOR_BROWN 8

    //int colors_definitions[1][4] = {
    //    
    //    {COLOR_BROWN, 500, 300, 0}
    //};
    
    init_color(COLORS-1, 647, 164, 164);

    init_pair(1, COLORS-1, COLORS-1);

    attron(COLOR_PAIR(1));

    printw("A Big Brown Fox");

    attroff(COLOR_PAIR(1));

    refresh();

    getch();

    endwin();




    return EXIT_SUCCESS;
}
