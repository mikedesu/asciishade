#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <ctype.h>
#include <locale.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <getopt.h>
#include "mPrint.h"
#include "canvas.h"
#include "tools.h"
#include "hud.h"
#include <time.h>
#include <assert.h>

// we can technically handle 99 colors and have defined them
// BUT we can only have 256 color pairs (16x16) active at a time
//#define MAX_FG_COLORS 99
//#define MAX_BG_COLORS 99
#define MAX_FG_COLORS 16
#define MAX_BG_COLORS 16
#define MAX_COLOR_PAIRS (MAX_FG_COLORS*MAX_BG_COLORS)
#define DEFAULT_COLOR_PAIR 1

struct timespec ts0;
struct timespec ts1;

bool is_text_mode       = false;
bool is_cam_mode        = false;
long last_cmd_ns = -1;
int last_char_pressed   = -1;
int canvas_width        = -1;
int canvas_height       = -1;
int terminal_height               = -1;
int terminal_width               = -1;
//long last_cmd_ms = -1;
int y                   = 0;
int x                   = 0;
// "camera" offsets
int cy                   = 0;
int cx                   = 0;
int quit                = 0;
int hud_color           = 7;
int current_color_pair  = 0;
int max_color_pairs     = -1;
int max_colors          = -1;
// this is the current filename
char filename[1024]     = {0};
// this is used to quickly grab info about what the current
// fg and bg color is based on the current "color pair"
// 128x128 = 16384
// 16x16 = 256
canvas_pixel_t **canvas = NULL;
int **color_array = NULL;
int **color_pair_array = NULL;

void paintbucket(int y, int x, int old_fg, int old_bg, int new_fg, int new_bg);
//void paintbucket(int y, int x, int new_fg, int new_bg);
int get_new_width_from_user();
int get_new_height_from_user();
int get_current_fg_color();
int get_current_bg_color();
void get_filename_from_user();
void init_color_arrays();
void add_block();
void delete_block();
void add_character(wchar_t c);
void add_character_and_move_right(wchar_t c);
void define_color_pairs();
void draw_hud();
void draw_initial_ascii();
void draw_canvas();
void fail_with_msg(const char *msg);
void handle_canvas_load();
void handle_color_pair_change(int c);
void handle_normal_mode_arrow_keys(int c);
void handle_input();
void handle_text_mode_input(int c);
void handle_normal_mode_input(int c);
void handle_move_right();
void handle_move_left();
void handle_move_up();
void handle_move_down();
void handle_save_inner_loop(FILE *outfile);
void handle_save();
void init_program();
void parse_arguments(int argc, char **argv);
void print_help(char **argv);
void reset_cursor();
void write_char_to_canvas(int y, int x, wchar_t c, int fg_color, int bg_color);
void cleanup();
void show_error(char *error_msg);
void get_int_str_from_user(char *prompt);
void exit_with_error(char *error_msg);
void free_color_arrays();
void free_color_pair_array();
void incr_cam_y();
void decr_cam_y();
void incr_cam_x();
void decr_cam_x();
void invert_current_color_pair();

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);
    init_program();
    //draw_initial_ascii();
    refresh();
    while (!quit) {
        clear();
        draw_canvas();
        draw_hud();
        handle_input();
        refresh();
    }
    cleanup();
    return EXIT_SUCCESS;
}

void reset_cursor() { 
    move(y, x); 
}

void print_help(char **argv) {
    printf("Usage: %s [OPTION]...\n", argv[0]);
    printf("  -f, --filename=FILENAME    specify a filename to save to\n");
    //printf("  -h, --help                 display this help and exit\n");
    printf("  -w, --width=WIDTH          specify the width of the canvas\n");
    printf("  -h, --height=HEIGHT        specify the height of the canvas\n");
    printf("\n");
}

void parse_arguments(int argc, char **argv) {
    //mPrint("Parsing arguments...\n");
    // parsing arguments using getopt_long
    int c = -1;
    int option_index = 0;
    static struct option longoptions[] = {
        {"filename", 1, NULL, 'f'},
        //{"help",     0, NULL, '?'},
        {"width",    1, NULL, 'w'},
        {"height",    1, NULL, 'h'},
        {0, 0, 0, 0}
    };

    while (1) {
        c = getopt_long(argc, argv, "f:w:h:", longoptions, &option_index);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'f':
                strncpy(filename, optarg, 1024);
                break;
            case 'w':
                canvas_width = atoi(optarg);
                break;
            case 'h':
                canvas_height = atoi(optarg);
                break;
            case '?':
                // handle input file info
                if (optopt == 'f') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if (optopt == 'w') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if (optopt == 'h') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                }
                else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                print_help(argv);
                exit(EXIT_FAILURE);
                break;
            default:
                abort();
        }
    }
}

void init_color_arrays() {
    color_array = calloc(MAX_COLOR_PAIRS, sizeof(int *));
    if (color_array == NULL) {
        mPrint("Failed to allocate memory for color_array\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_COLOR_PAIRS; i++) {
        color_array[i] = calloc(2, sizeof(int));
        if (color_array[i] == NULL) {
            mPrint("Failed to allocate memory for color_array\n");
            exit(EXIT_FAILURE);
        }
    }
    color_pair_array = calloc(MAX_FG_COLORS, sizeof(int *));
    if (color_pair_array == NULL) {
        mPrint("Failed to allocate memory for color_pair_array\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_FG_COLORS; i++) {
        color_pair_array[i] = calloc(MAX_BG_COLORS, sizeof(int));
        if (color_pair_array[i] == NULL) {
            mPrint("Failed to allocate memory for color_pair_array\n");
            exit(EXIT_FAILURE);
        }
    }
}


void invert_current_color_pair() {
    // get the current color pair
    // get the current fg and bg colors

    int fg_color = color_array[current_color_pair][0];
    int bg_color = color_array[current_color_pair][1];

    // invert the colors
    //

    // get the color pair index
    int color_pair_index = color_pair_array[fg_color][bg_color];

    assert(color_pair_index == current_color_pair);

    int inverse_color_pair_index = color_pair_array[bg_color][fg_color];

    // swap the colors
    current_color_pair = inverse_color_pair_index;
}



// I realize just now 2023-07-18 that we can optimize the color pairs...
// 16x16 right?
// that means there will be cases of duplicate colors
// like same fg and bg colors
// so we can optimize the color pairs by only storing the unique ones
// we can get back probably 16 color pairs
void define_color_pairs() {
    int current_pair = 0;
    int bg_color_start = 0;
    int fg_color_start = 0;
    for (int bg_color = bg_color_start; bg_color < MAX_BG_COLORS; bg_color++) {
        for (int fg_color = fg_color_start; fg_color < MAX_FG_COLORS; fg_color++) {
            // this is the intent but it is throwing everything off right now
            // once we count up the max colors correctly
            //if (fg_color != bg_color) {
                init_pair(current_pair, fg_color, bg_color);
                // store the color pair in the array
                color_array[current_pair][0] = fg_color;
                color_array[current_pair][1] = bg_color;
                // store the color pair in the array
                // this is problematic if we want to handle -1,-1
                color_pair_array[fg_color][bg_color] = current_pair;
                current_pair++;
            //}
        }
    }
    max_color_pairs = current_pair;
    max_colors = MAX_FG_COLORS;
    assert(max_color_pairs == MAX_COLOR_PAIRS);
}

void cleanup() {
    endwin();
    free_canvas(canvas, canvas_height);
    free_color_arrays();
    free_color_pair_array();
}

void fail_with_msg(const char *msg) {
    cleanup();
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

void write_char_to_canvas(int y, int x, wchar_t c, int fg_color, int bg_color) {
    // check to make sure y,x is within the canvas
    if (y < 0 || y >= canvas_height || x < 0 || x >= canvas_width) {
        //clear();
        //refresh();
        //cleanup();
        //fprintf(stderr, "y: %d\nx: %d\n", y, x);
        //mPrint("write_char_to_canvas: y,x is out of bounds");
        //exit(EXIT_FAILURE);
        return;
    } else if (fg_color < 0 || fg_color >= max_colors) {
        clear();
        refresh();
        cleanup();
        fprintf(stderr, "fg_color: %d\n", fg_color);
        mPrint("write_char_to_canvas: fg_color is out of bounds");
        exit(EXIT_FAILURE);
    }else if (bg_color < 0 || bg_color >= max_colors) {
        clear();
        refresh();
        cleanup();
        fprintf(stderr, "bg_color: %d\n", bg_color);
        mPrint("write_char_to_canvas: bg_color is out of bounds");
        exit(EXIT_FAILURE);
    }else {
        canvas[y][x].character = c;
        canvas[y][x].foreground_color = fg_color;
        canvas[y][x].background_color = bg_color;
    }
} 

int get_current_fg_color() {
    return color_array[current_color_pair][0];
}

int get_current_bg_color() {
    return color_array[current_color_pair][1];
}

void draw_initial_ascii() {
#define INITIAL_ASCII_LINE_COUNT 3
    char *lines[INITIAL_ASCII_LINE_COUNT] = { 
        "Welcome to asciishade", 
        "by darkmage", 
        "www.evildojo.com" 
    };
    current_color_pair = DEFAULT_COLOR_PAIR;
    int fg_color = get_fg_color(color_array, MAX_COLOR_PAIRS, current_color_pair);
    int bg_color = get_bg_color(color_array, MAX_COLOR_PAIRS, current_color_pair);
    for (int i=0; i < 3; i++) {
        for (size_t j=0; j < strlen(lines[i]); j++) {
            write_char_to_canvas(i, j, lines[i][j], fg_color, bg_color);
        }
    }
    //canvas = read_ascii_from_filepath("test2.ascii", &canvas_height, &canvas_width);
}

void draw_canvas() {
    wchar_t c      = L' ';
    int fg_color   = 0;
    int bg_color   = 0;
    int color_pair = 0;
    int i          = -1;
    int j          = -1;
    int y          = -1;
    int x          = -1;
    for (i = 0; i + cy < canvas_height; i++) {
        y = i + cy;
        for (j = 0; j + cx < canvas_width; j++) {
            // set the color pair
            // first, get the color pair
            // it should be equal to fg_color + (bg_color * 16)
            x = j + cx;
            if (x < 0 || x >= canvas_width || y < 0 || y >= canvas_height) {
                continue;
            }
            //fg_color   = canvas[i][j].foreground_color;
            //bg_color   = canvas[i][j].background_color;
            fg_color   = canvas[y][x].foreground_color;
            bg_color   = canvas[y][x].background_color;
            color_pair = color_pair_array[fg_color][bg_color];
            // draw the character
            attron(COLOR_PAIR(color_pair));
            c = canvas[y][x].character;
            // this fixes the block-rendering bug for now...
            // there has to be a better way to do this
            if ( c == L'▀' ) {
                mvaddstr(i, j, "▀");
            } else if ( c == L'▄' ) {
                mvaddstr(i, j, "▄");
            } else if ( c == L'█' ) {
                mvaddstr(i, j, "█");
            } else if (c == L'░' ) {
                mvaddstr(i, j, "░");
            } else {
                mvaddch(i, j, c);
            }
            attroff(COLOR_PAIR(color_pair));
        }
    }
}

void add_character(wchar_t c) {
    // add the character to the canvas
    //canvas[y][x].character = c;
    // store the color component
    //canvas[y][x].foreground_color = get_fg_color(current_color_pair);
    //canvas[y][x].background_color = get_bg_color(current_color_pair);
    int fg_color = get_fg_color(color_array, MAX_COLOR_PAIRS, current_color_pair);
    int bg_color = get_bg_color(color_array, MAX_COLOR_PAIRS, current_color_pair);
    //write_char_to_canvas(y, x, c, fg_color, bg_color);
    int my = cy + y;
    int mx = cx + x;
    write_char_to_canvas(my, mx, c, fg_color, bg_color);
}

void add_character_and_move_right(wchar_t c) {
    add_character(c);
    handle_move_right();
}

void add_block() { 
    //add_character(L'█');
    add_character(L' ');
}

void delete_block() {
    // delete the character from the canvas
    int my = cy + y;
    int mx = cx + x;
    write_char_to_canvas(my, mx, L' ', 0, 0);
    //canvas[y][x].character = L' ';
    // store the color component
    // this should be set to the default color pair
    // what was the initial ascii drawn in?
    //canvas[y][x].foreground_color = 0;
    //canvas[y][x].foreground_color = get_fg_color(DEFAULT_COLOR_PAIR);
    //canvas[y][x].background_color = 0;
    //canvas[y][x].background_color = get_bg_color(DEFAULT_COLOR_PAIR);
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

// this is not an accurate function
// it is close, but we are:
//
// 1. not defining all 16 colors
// 2. not scaling up to all of the colors that ncurses OR irc can handle
// https://modern.ircdocs.horse/formatting.html
void handle_save_inner_loop(FILE *outfile) {
    if (outfile == NULL) {
        fail_with_msg("Error opening file for writing");
    }
    for (int i = 0; i < canvas_height; i++) {
        int prev_irc_fg_color = -1;
        int prev_irc_bg_color = -1;
        for (int j = 0; j < canvas_width; j++) {
            // now, instead of grabbing characters from stdscr
            // and having to do all this shit on the fly
            // we can just render from the canvas
            wchar_t wc = canvas[i][j].character;
            int foreground_color = canvas[i][j].foreground_color;
            int background_color = canvas[i][j].background_color;
            int irc_foreground_color = convert_to_irc_color(foreground_color);
            int irc_background_color = convert_to_irc_color(background_color);
            bool color_changed = prev_irc_fg_color != irc_foreground_color || prev_irc_bg_color != irc_background_color;
            //old code here for historic reasons
            //cchar_t character;
            //mvwin_wch(stdscr, i, j, &character);  // Read wide character from the canvas
            //wchar_t wc = character.chars[0];
            //attr_t attribute = character.attr;
            //int color_pair_number = PAIR_NUMBER(attribute);
            if (color_changed) 
            {
                fprintf(outfile, "\x03%02d,%02d%lc", irc_foreground_color, irc_background_color, wc);
                //fwprintf(outfile, L"\x03%02d,%02d%c", irc_foreground_color, irc_background_color, wc);
            }else {
                //fwprintf(outfile, L"%c", wc);
                fprintf(outfile, "%lc", wc);
            }
            prev_irc_fg_color = irc_foreground_color;
            prev_irc_bg_color = irc_background_color;
        }
        //fprintf(outfile, "\x03\n");
        fprintf(outfile, "\n");
    }   
}



void get_filename_from_user() {
    char buffer[32] = {0};
    while(strlen(buffer) == 0) {
        clear();
        echo();
        mvprintw(0, 0, "Enter filename: ");
        refresh();
        getnstr(buffer, 32);
    }
    strncpy(filename, buffer, 1024);
    noecho();
}




void handle_save() {
    // 1. filename is empty
    if (strcmp(filename, "") == 0) { 
        // set a default filename
        //strncpy(filename, "untitled.ascii", 1024);
        
        // get a filename from the user
        get_filename_from_user();
    }
    // 2. filename is not empty
    if (strcmp(filename, "") != 0) { 
        // test writing out file
        FILE *outfile = fopen(filename, "w");
        if (outfile == NULL) {
            perror("Error opening file for writing");
            exit(-1);
        }
        handle_save_inner_loop(outfile);
        fclose(outfile);
    }
}

void handle_move_down() {
    if (y+1 < canvas_height) {
        y++;
    }
}

void handle_move_up() {
    if (y-1 >= 0) {
        y--;
    }
}

void handle_move_left() {
    if (x-1 >= 0) {
        x--;
    }
}

void handle_move_right() {
    if (x+1 < canvas_width) {
        x++;
    }
}

void incr_cam_x() {
    cx++;
}

void decr_cam_x() {
    if (cx-1 >= 0) {
        cx--;
    }
}

void incr_cam_y() {
    cy++;
}

void decr_cam_y() {
    if (cy-1 >= 0) {
        cy--;
    }
}

void handle_normal_mode_arrow_keys(int c) {
    if (c==KEY_DOWN) {
        if (! is_cam_mode) {
            handle_move_down();
        } else {
            incr_cam_y();
        }
    } else if (c==KEY_UP) {
        if (! is_cam_mode) {
            handle_move_up();
        } else {
            decr_cam_y();
        }
    } else if (c==KEY_LEFT) {
        if (! is_cam_mode) {
            handle_move_left();
        } else {
            decr_cam_x();
        }
    } else if (c==KEY_RIGHT) {
        if (! is_cam_mode) {
            handle_move_right();
        } else {
            incr_cam_x();
        }
    }
}

void handle_color_pair_change(int c) {
    if (c=='o') {
        decr_color_pair();
    } else if (c=='p') {
        incr_color_pair();
    } else if (c=='O') {
        decr_color_pair_by_max();
    } else if (c=='P') {
        incr_color_pair_by_max();
    }
}



//void paintbucket(int y, int x, int new_fg, int new_bg) {
void paintbucket(int y, int x, int old_fg, int old_bg, int new_fg, int new_bg) {
    if (y < 0 || y >= canvas_height) {
        return;
    }
    if (x < 0 || x >= canvas_width) {
        return;
    }
    if (canvas[y][x].foreground_color != old_fg) {
        return;
    }
    if (canvas[y][x].background_color != old_bg) {
        return;
    }
    if (canvas[y][x].foreground_color == new_fg) {
        return;
    }
    if (canvas[y][x].background_color == new_bg) {
        return;
    }
    canvas[y][x].foreground_color = new_fg;
    canvas[y][x].background_color = new_bg;

    //canvas[y][x].foreground_color = fg;
    //canvas[y][x].background_color = bg;
    
    paintbucket(y-1, x, old_fg, old_bg, new_fg, new_bg);
    paintbucket(y+1, x, old_fg, old_bg,new_fg, new_bg);
    paintbucket(y, x-1, old_fg, old_bg,new_fg, new_bg);
    paintbucket(y, x+1, old_fg, old_bg,new_fg, new_bg);
}




void handle_normal_mode_input(int c) {
    // escape key switches back and forth between normal & text modes
    if (c == 27) {
        is_text_mode = true;
    } else if (c=='q') {
        quit = 1;
    } 
    else if (c=='C') {
        clear_canvas(canvas, canvas_height, canvas_width);
    } 
    else if (c=='c') {
        is_cam_mode = ! is_cam_mode;
        is_text_mode = false;
    } 
    else if (c=='f') {
        int fg = get_current_fg_color();
        int bg = get_current_bg_color();
        fill_canvas(canvas, canvas_height, canvas_width, fg, bg);
    } else if (c==KEY_DC) {
        delete_block();
    } else if (c==KEY_BACKSPACE) {
        delete_block();
        handle_move_left();
    } else if (c=='S')  {
        handle_save();
    } else if (c==KEY_DOWN || c==KEY_UP || c==KEY_RIGHT || c==KEY_LEFT) {
        handle_normal_mode_arrow_keys(c);
    } 
    
    else if (c=='g') {
        // paintbucket tool
        // smart fill
        int fg = get_current_fg_color();
        int bg = get_current_bg_color();
        int old_fg = canvas[y][x].foreground_color;
        int old_bg = canvas[y][x].background_color;
        //canvas[y][x].foreground_color = fg;
        //canvas[y][x].background_color = bg;
        //paintbucket(y, x, old_fg, old_bg, fg, bg);
        paintbucket(y, x, old_fg, old_bg, fg, bg);
    }


    else if (c==' ') {
        add_block();
        handle_move_right();
    } else if (c=='o' || c=='O' || c=='p' || c=='P') {
        handle_color_pair_change(c);
    } else if (c=='E') { // experimental
        show_error("This is an error message");

        get_int_str_from_user("Enter a number: ");
    }

    // resize the canvas' width
    else if (c=='W') {
        int w = -1;
        while (w == -1 || w > 150) {
            w = get_new_width_from_user();
        }
        canvas_pixel_t **new_canvas = init_canvas(canvas_height, w);
        if (new_canvas == NULL) {
            show_error("Error creating new canvas");
            return;
        }

        y = 0;
        x = 0;
        move(y,x);



        copy_canvas(new_canvas, canvas, canvas_height, w, canvas_height, canvas_width);
        canvas_width = w;
        canvas_pixel_t **old_canvas = canvas;
        canvas = new_canvas;
        free_canvas(old_canvas, canvas_height);
    }

    
    // resize the canvas' height
    else if (c=='H') {
        int h = -1;
        // whats the max height?
        while (h==-1) {
            h = get_new_height_from_user();
        }
        canvas_pixel_t **new_canvas = init_canvas(h, canvas_width);
        if (new_canvas == NULL) {
            show_error("Error creating new canvas");
            return;
        }

        y = 0;
        x = 0;
        move(y,x);

        copy_canvas(new_canvas, canvas, h, canvas_width, canvas_height, canvas_width);
        int old_canvas_height = canvas_height;
        canvas_height = h;
        canvas_pixel_t **old_canvas = canvas;
        canvas = new_canvas;
        free_canvas(old_canvas, old_canvas_height);
    }


    //else if (c==KEY_TAB) {
    else if (c==0x09) {
        // toggle between normal mode and cam mode
        invert_current_color_pair();
    }

    else if (c=='v') {
        canvas_flip_vertical(canvas, canvas_height, canvas_width);
    }
    else if (c=='h') {
        canvas_flip_horizontal(canvas, canvas_height, canvas_width);
    }



    else if (c==KEY_RESIZE) {
        getmaxyx(stdscr, terminal_height, terminal_width);
    }      

}

void handle_text_mode_input(int c) {
    if (c == 27) {
        is_text_mode = false;
    }else if (c==KEY_DC) {
        // delete a block from the current location on the canvas
        delete_block();
    }else if (c==KEY_BACKSPACE) {
        delete_block();
        handle_move_left();
    }


    /*
    else if (c==KEY_LEFT) {
        handle_move_left();
    }else if (c==KEY_RIGHT) {
        handle_move_right();
    }else if (c==KEY_UP) {
        handle_move_up();
    }else if (c==KEY_DOWN) {
        handle_move_down();
    } 
    */
    

    else if (c==KEY_UP || c==KEY_DOWN || c==KEY_LEFT || c==KEY_RIGHT) {
        handle_normal_mode_arrow_keys(c);
    }


    else if (c==KEY_RESIZE) {
        getmaxyx(stdscr, terminal_height, terminal_width);
        
    } else {
        add_character_and_move_right(c);
    }
}

void handle_input() {
    int c = getch();
    // start the clock
    clock_gettime(CLOCK_MONOTONIC, &ts0);
    last_char_pressed = c;
    if (is_text_mode) {
        handle_text_mode_input(c);
    } else {
        handle_normal_mode_input(c);
    }
    // stop the clock
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    //last_cmd_ms = (ts1.tv_sec - ts0.tv_sec) * 1000 + (ts1.tv_nsec - ts0.tv_nsec) / 1000000;
    // do nanoseconds
    last_cmd_ns = (ts1.tv_sec - ts0.tv_sec) * 1000000000 + (ts1.tv_nsec - ts0.tv_nsec);
    // do microseconds
    //last_cmd_ns = last_cmd_ns / 1000;
}

void draw_hud() {
    draw_hud_background(hud_color, terminal_height, terminal_width);
    draw_hud_row_1(canvas, 
        color_array, 
        MAX_COLOR_PAIRS, 
        filename, 
        y, 
        x,
        cy,
        hud_color, 
        terminal_height, 
        terminal_width, 
        canvas_height,
        canvas_width,
        current_color_pair,
        is_text_mode
    );
    reset_cursor();
    draw_hud_row_2(canvas, 
            color_array, 
            MAX_COLOR_PAIRS, 
            color_pair_array, 
            MAX_FG_COLORS ,
            terminal_height, 
            terminal_width, 
            hud_color, 
            current_color_pair, 
            y, 
            x, 
            cx,
            last_char_pressed
        );
    draw_hud_row_3(terminal_height, terminal_width, hud_color, last_cmd_ns);
    /*
    */
    reset_cursor();
}

void init_program() {
    //mPrint("Initializing program\n");
    setlocale(LC_ALL, "");
    // 25 ms delay for escape sequences
    setenv("ESCDELAY", "200", 1);
    initscr();
    //nodelay(stdscr, true);
    clear();
    noecho();
    keypad(stdscr, true);
    if (!has_colors()) {
        exit_with_error("Your terminal does not support color\nPlaintext ASCII mode is not yet implemented");
    }
    start_color();
    use_default_colors();
    if (!can_change_color()) {
        exit_with_error("Your terminal does not support changing colors\nYou are likely using tmux or something else\nA mode to handle this is not yet implemented");
    }
    define_colors();
    init_color_arrays();
    define_color_pairs();
    getmaxyx(stdscr, terminal_height, terminal_width);
    // if the terminal is too small, exit
    // what is "too small"? IDFK GDI 1 pixel would be kind of PoC tho
    // smallest terminal IS diff than smallest canvas
    // smallest canvas should be 1
    // smallest terminal should be large enough to fit the smallest HUD
    // this feels dumb now but work with me here
    const size_t smallest_width = 1;
    if (terminal_width < smallest_width) {
        fprintf(stderr, "Error: terminal too small\n");
        exit(EXIT_FAILURE);
    }
    // make the cursor visible
    curs_set(1);
    // initialize the canvas
    // for now, we are going to make the canvas the same size as the terminal
    // when we go to read in ascii files,
    handle_canvas_load();
}

void handle_canvas_load() {
    int num_of_hud_rows = 3;
    // ok, initializing the canvas size
    // we already know the term width/height which is good
    // if no size information is available...
    // like, not passed in from the commandline...
    if (canvas_height == -1) {
        canvas_height = terminal_height - num_of_hud_rows;
    }
    if (canvas_width == -1) {
        canvas_width = terminal_width;
    }
    // now, it is possible that I pass in a width or height for the canvas
    // that exceeds the size of the terminal
    // or that an ascii image was read in with width or height
    // that exceeds the size of the terminal
    // it is for this reason we will introduce a "camera" offset mechanism 
    // so that you can have various configurations for both the terminal and
    // canvas
    // we may not need to do anything here, but rather when the user's cursor
    // attempts to navigate offscreen
    // at this point, if we passed a filename
    if (strcmp(filename, "")!=0 && check_if_file_exists(filename)) {
            // we will load this file into the canvas
            canvas = read_ascii_from_filepath(filename, &canvas_height, &canvas_width);
            // eventually we will have to be able to handle moving around a 
            // canvas that might be much larger than our terminal size
    } else {
        canvas = init_canvas(canvas_height, canvas_width);
    }
}

void show_error(char *error_msg) {
    if (error_msg != NULL) {
        clear();
        mvaddstr(0,0,error_msg);
        refresh();
        getch();
        clear();
        refresh();
    }
}

int get_new_width_from_user() {
    echo();
    char *prompt = "Enter new width: ";
    char user_input[4] = {0};// up to a 3-digit number
    int user_input_int = -1;
    clear();
    mvaddstr(0,0,prompt);
    refresh();
    getnstr(user_input, 3);
    user_input_int = atoi(user_input);
    if (user_input_int == 0) {
        return -1;
    }
    noecho();
    return user_input_int;
}


int get_new_height_from_user() {
    echo();
    char *prompt = "Enter new height: ";
    char user_input[4] = {0}; // up to a 3-digit number
    int user_input_int = -1;
    clear();
    mvaddstr(0,0,prompt);
    refresh();
    getnstr(user_input, 3);
    user_input_int = atoi(user_input);
    if (user_input_int == 0) {
        return -1;
    }
    noecho();
    return user_input_int;
}


void get_int_str_from_user(char *prompt) {
    if (prompt != NULL) {
        char user_input[4] = {0}; // up to 3-digit num
        int user_input_int = -1;
        clear();
        mvaddstr(0,0,prompt);
        refresh();
        getnstr(user_input, 3);
        user_input_int = atoi(user_input);
        clear();
        printw("You entered: %d\n", user_input_int);
        refresh();
        getch();
    }
}

void exit_with_error(char *error_msg) {
    endwin();
    fprintf(stderr, "%s\n", error_msg);
    exit(EXIT_FAILURE);
}

void free_color_arrays() {
    for (int i = 0; i < MAX_COLOR_PAIRS; i++) {
        free(color_array[i]);
    }
    free(color_array);
}

void free_color_pair_array() {
    for (int i = 0; i < MAX_FG_COLORS; i++) {
        free(color_pair_array[i]);
    }
    free(color_pair_array);
}

