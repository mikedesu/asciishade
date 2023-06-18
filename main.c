#define _XOPEN_SOURCE_EXTENDED 1

#include <ncurses.h>
#include <ctype.h>
#include <locale.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <wchar.h>

// something broken about ncurses movement keys but i hacked this together
#define MV_DOWN 66
#define MV_UP 65
#define MV_LEFT 68
#define MV_RIGHT 67

int max_y = -1;
int max_x = -1;
int y = -1;
int x = -1;
int quit = 0;
int hud_color = 7;
int current_color_pair = 0;
int max_color_pairs = -1;
int max_colors = -1;

// this is the current filename
char *filename = NULL;

// this is used to quickly grab info about what the current
// fg and bg color is based on the current "color pair"
int color_array[128*128][2] = { 0 };

int convert_to_irc_color(int color);
void handle_save();
void define_color_pairs();
void draw_initial_ascii();
void handle_input();
void draw_hud();
void reset_cursor();
void parse_arguments(int argc, char **argv);
int get_fg_color(int color_pair);
int get_bg_color(int color_pair);


int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    // parse arguments
    parse_arguments(argc, argv);
    initscr();
    clear();
    noecho();
    start_color();
    use_default_colors();
    define_color_pairs();
    draw_initial_ascii();
    getmaxyx(stdscr, max_y, max_x);
    y = 0;
    x = 0;
    refresh();
    while (!quit) {
        draw_hud();
        handle_input();
        refresh();
    }
    endwin();
    return EXIT_SUCCESS;
}


void reset_cursor() { 
    move(y, x); 
}


void parse_arguments(int argc, char **argv) {
    int c = -1;
    while ((c = getopt(argc, argv, "f:")) != -1) {
        // set filename of ascii
        if (c == 'f') {
            filename = optarg;
        }
        else if (c == '?') {
            if (optopt == 'f') {
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            }
            else if (isprint(optopt)) {
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            }
            else {
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            }
            exit(EXIT_FAILURE);
        }
    }
}


void define_color_pairs() {
    int current_pair = 0;
    const int local_max_colors = 128; // for now...
    for (int bg_color = 0; bg_color < local_max_colors; bg_color++) {
        for (int fg_color = 0; fg_color < local_max_colors; fg_color++) {
            init_pair(current_pair, fg_color, bg_color);

            // store the color pair in the array
            color_array[current_pair][0] = fg_color;
            color_array[current_pair][1] = bg_color;

            current_pair++;
        }
    }
    max_color_pairs = current_pair;
    max_colors = local_max_colors;
}


int get_fg_color(int color_pair) {
    return color_array[color_pair][0];
}


int get_bg_color(int color_pair) {
    return color_array[color_pair][1];
}


void draw_initial_ascii() {
    char *lines[3] = { 
        "Welcome to asciishade\n", 
        "by darkmage\n", 
        "www.evildojo.com\n" 
    };
    current_color_pair = 1;
    attron(COLOR_PAIR(current_color_pair));
    for (int i = 0; i < 3; i++) {
        addstr(lines[i]);
    }
    attroff(COLOR_PAIR(current_color_pair));
}


void add_block() { 
    attron(COLOR_PAIR(current_color_pair)); 
    mvaddstr(y, x, "█"); 
    //mvaddstr(y, x, "x"); 
    attroff(COLOR_PAIR(current_color_pair)); 
}


void incr_color_pair() { 
    current_color_pair++; 
    if (current_color_pair >= max_color_pairs) {
        current_color_pair = 0;  
    }
}


void incr_color_pair_by_max() { 
    for (int i = 0; i < max_colors; i++) {
        incr_color_pair();                                   
    }
}


void decr_color_pair() { 
    current_color_pair--; 
    if (current_color_pair < 0) { 
        current_color_pair = max_color_pairs-1; 
    }
}


void decr_color_pair_by_max() { 
    for (int i = 0; i < max_colors; i++) { 
        decr_color_pair();                                   
    }
}


int convert_to_irc_color(int color) {
    switch (color) {
        case COLOR_BLACK:   return 1;
        case COLOR_RED:     return 4;
        case COLOR_GREEN:   return 3;
        case COLOR_YELLOW:  return 8;
        case COLOR_BLUE:    return 2;
        case COLOR_MAGENTA: return 6;
        case COLOR_CYAN:    return 10;
        case COLOR_WHITE:   return 0;
        default:            return color;
    }
    return 0;
}



void handle_save() {
    // 1. filename is (null)
    // 2. filename is not (null)
    if ( filename != NULL ) {
        // test writing out file
        FILE *outfile = fopen(filename, "w");
        if (outfile == NULL) {
            perror("Error opening file for writing");
            exit(-1);
        }
        
        //int canvas_width = 80;
        //int canvas_height = 4;
        
        int canvas_width = 80; // once we've optimized the output, this can probably go back to normal
        // it will be reasonable to have certain limits on the canvas width
        int canvas_height = 8;
        //int canvas_height = max_y-2;
        
        int prev_irc_fg_color = -1;
        int prev_irc_bg_color = -1;

        //wchar_t blockCharacter = L'\u2588';  // Wide character representation of block character
        for (int i = 0; i < canvas_height; i++) {
            for (int j = 0; j < canvas_width; j++) {
                cchar_t character;
                mvwin_wch(stdscr, i, j, &character);  // Read wide character from the canvas
                wchar_t wc = character.chars[0];
                attr_t attribute = character.attr;
                int color_pair_number = PAIR_NUMBER(attribute);
                int foreground_color = get_fg_color(color_pair_number);
                int background_color = get_bg_color(color_pair_number);
                int irc_foreground_color = convert_to_irc_color(foreground_color);
                int irc_background_color = convert_to_irc_color(background_color);
                bool color_changed = prev_irc_fg_color != irc_foreground_color || prev_irc_bg_color != irc_background_color;
                if (color_changed) {
                    fprintf(outfile, "\x03%d,%d%lc", irc_foreground_color, irc_background_color, wc);
                }
                else {
                    fprintf(outfile, "%lc", wc);
                }
                prev_irc_fg_color = irc_foreground_color;
                prev_irc_bg_color = irc_background_color;

                // soon we will be able to optimize the format that we write things out
                // by recognizing when fg and bg colors actually change on a line
                // but for now, we write out the fg/bg color for each character
                //fprintf(outfile, "\x03%d,%d%lc", irc_foreground_color, irc_background_color, wc);
            }
            fprintf(outfile, "\x03\n");
        }
        fclose(outfile);
    }
}


void handle_input() {
    int c = getch();
    if (c == 'q') { 
        quit = 1; 
    }
    else if (c == 's') {
        handle_save();       
        //else {
        //}
    }
    // movement keys
    else if (c == MV_DOWN) {
        if (y+1 < max_y-2) {
            y++;
        }
    }
    else if (c == MV_UP) {
        if (y-1 >= 0) {
            y--;
        }
    }
    else if (c == MV_LEFT) {
        if (x-1 >= 0) {
            x--;
        }
    }
    else if (c == MV_RIGHT) {
        if (x+1 < max_x) {
            x++;
        }
    }
    // variations on adding blocks
    else if (c == ' ') { 
        add_block(); 
        if (x+1 < max_x) {
            x++;
        }
    }
    //else if (c == 'f') {
    //    add_block(); 
    //    x++; 
    //}
    //else if (c == 'd') { 
    //    add_block(); 
    //    x--; 
    //}
    //else if (c == 'c') { 
    //    add_block(); 
    //    y++; 
    //}
    //else if (c == 'e') { 
    //    add_block(); 
    //    y--; 
    //}
    //else if (c == 'r') { add_block(); y--; x++; }
    //else if (c == 'v') { add_block(); y++; x++; }
    //else if (c == 'x') { add_block(); y++; x--; }
    //else if (c == 'w') { add_block(); y--; x--; }
    // color changing
    else if (c == 'o') { 
        decr_color_pair(); 
    }
    else if (c == 'p') { 
        incr_color_pair(); 
    }
    else if (c == 'O') { 
        decr_color_pair_by_max(); 
    }
    else if (c == 'P') { 
        incr_color_pair_by_max(); 
    }
}


void switch_between_hud_and_current_color() { 
    attroff(COLOR_PAIR(hud_color));  
    attron(COLOR_PAIR(current_color_pair)); 
}


void switch_between_current_and_hud_color() { 
    attroff(COLOR_PAIR(current_color_pair));  
    attron(COLOR_PAIR(hud_color)); 
}


void draw_hud_row_1() {
    char str[64] = {0};
    sprintf(str, "y: %03d | ", y);

    mvaddstr(max_y-2, 0, str);
    
    switch_between_hud_and_current_color();
    
    addstr("█");
    
    //addstr(0x2588);
    switch_between_current_and_hud_color();
    memset(str,0,32);
    int fg_color = current_color_pair % max_colors;
    sprintf(str, " %03d FG CurrentColorPair(%05d) FG %03d Filename: %s", fg_color, current_color_pair, 0, filename );
    
    addstr(str);
}


void draw_hud_row_2() {
    char str[16] = {0};
    sprintf(str, "x: %03d | ", x);
    mvaddstr(max_y-1, 0, str);
    switch_between_hud_and_current_color();
    addstr(" ");
    switch_between_current_and_hud_color();
    char str2[64] = {0};
    int bg_color = current_color_pair / max_colors;

    // get where our cursor is
    int cursor_y, cursor_x;
    getyx(stdscr, cursor_y, cursor_x);

    // now that we have memory of where we stopped drawing...
    chtype character = mvinch(y, x);
    attr_t attribute = character & A_COLOR;
    int color_pair_number = PAIR_NUMBER(attribute);

    //move back to where the cursor was
    move(cursor_y, cursor_x);

    //sprintf(str2, " %03d BG PairUnderCursor (%05d)", bg_color, color_pair_number);
    sprintf(str2, " %03d BG PairUnderCursor (%05d) BG %03d %dx%d", bg_color, color_pair_number, 0, max_y, max_x);
    addstr(str2);
    attroff(COLOR_PAIR(hud_color));
    reset_cursor();
}


void draw_hud() {
    attron(COLOR_PAIR(hud_color));
    for (int j = max_y - 2; j < max_y; j++) { 
        for (int i = 0; i < max_x-1; i++) {
            mvaddstr(j, i, " ");
        }
    }
    draw_hud_row_1();
    draw_hud_row_2();
}


