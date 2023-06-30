


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

    start_color();
    


#define COLOR_BROWN 8

    //int colors_definitions[1][4] = {
    //    
    //    {COLOR_BROWN, 500, 300, 0}
    //};
    
    init_color(COLOR_BROWN, 500, 300, 0);

    init_pair(1, COLOR_BLACK, COLOR_BROWN);

    attron(COLOR_PAIR(1));

    printw("A Big Brown Fox");

    attroff(COLOR_PAIR(1));

    refresh();

    getch();

    endwin();




    return EXIT_SUCCESS;
}
