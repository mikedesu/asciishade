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
#include <time.h>
#include <assert.h>

#include "mPrint.h"
#include "canvas.h"
#include "tools.h"
#include "hud.h"
#include "colors.h"
#include "mPrint.h"

#define DEFAULT_COLOR_PAIR 1

//#define SPACE        L' ' 0x0020



extern int g_color_palette[100][100];

// we can technically handle 99 colors and have defined them
// BUT we can only have 256 color pairs (16x16) active at a time
int MAX_FG_COLORS = 16;
int MAX_BG_COLORS = 16;
int MAX_COLOR_PAIRS = 256;

struct timespec ts0;
struct timespec ts1;

bool can_change_color_mode          = true;
bool is_line_draw_mode              = false;
bool is_rect_draw_mode              = false;
bool is_text_mode                   = false;
bool is_cam_mode                    = false;
bool was_loaded_from_file           = false;
bool ncurses_initialized            = false;
bool color_array_initialized        = false;
bool color_pair_array_initialized   = false;

double last_cmd_ms        = -1;

long last_cmd_ns        = -1;
long last_cmd_us        = -1;

int last_char_pressed   = -1;
int canvas_width        = -1;
int canvas_height       = -1;
int terminal_height     = -1;
int terminal_width      = -1;
int y                   = 0;
int x                   = 0;
int cy                  = 0;
int cx                  = 0;
int quit                = 0;
int hud_color           = 7;
int current_color_pair  = 0;
int max_color_pairs     = -1;
int max_colors          = -1;
int line_draw_y0        = 0;
int line_draw_x0        = 0;
int line_draw_y1        = 0;
int line_draw_x1        = 0;

char filename[1024]     = {0};

canvas_pixel_t **canvas = NULL;


//wchar_t gblock = L' ';
wchar_t gblock = FULL_BLOCK;

int **color_array       = NULL;
int **color_pair_array  = NULL;

int get_current_fg_color();
int get_current_bg_color();
int get_new_width_from_user();
int get_new_height_from_user();

void add_block();
void add_character(wchar_t c);
void add_character_and_move_right(wchar_t c);

void cleanup();

void decr_cam_y();
void decr_cam_x();
void decr_color_pair();
void decr_color_pair_by_max();

void define_color_pairs();
void define_color_pairs_for_ascii();
void delete_block();
void draw_canvas();
void draw_hud();
void draw_initial_ascii();
void draw_line(int y1, int x1, int y2, int x2, int fg, int bg);
void draw_rect(int y1, int x1, int y2, int x2, int fg, int bg);

void exit_with_error(char *error_msg);

void fail_with_msg(const char *msg);
void free_color_arrays();
void free_color_pair_array();

void get_filename_from_user();
void get_int_str_from_user(char *prompt);

void handle_cam_mode_arrow_keys(int c);
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

void incr_cam_y();
void incr_cam_x();
void incr_color_pair();
void incr_color_pair_by_max();
void init_color_arrays();
void init_program();
void invert_current_color_pair();

void paintbucket(int y, int x, wchar_t old_char, wchar_t new_char, int old_fg, int old_bg, int new_fg, int new_bg);
void parse_arguments(int argc, char **argv);
void print_help(char **argv);

void render_temp_line(int y1, int x1, int y2, int x2);
//void render_temp_rect(int y1, int x1, int y2, int x2, int fg, int bg);
void render_temp_rect(int y1, int x1, int y2, int x2);
void reset_cursor();
void rotate_gblock_forward();
void rotate_gblock_backward();

void show_error(char *error_msg);
void show_help();

void write_char_to_canvas(int y, int x, wchar_t c, int fg_color, int bg_color);




int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);
    init_program();
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
    printf("  -f, --filename=FILENAME    specify a filename to save to\n"
    "  -w, --width=WIDTH          specify the width of the canvas\n"
    "  -h, --height=HEIGHT        specify the height of the canvas\n\n");
}




void parse_arguments(int argc, char **argv) {
    int c = -1;
    int option_index = 0;
    static struct option longoptions[] = {
        {"filename", 1, NULL, 'f'},
        {"width",    1, NULL, 'w'},
        {"height",    1, NULL, 'h'},
        {0, 0, 0, 0}
    };
    while (1) {
        c = getopt_long(argc, argv, "f:w:h:", longoptions, &option_index);
        if (c == -1) { break; }
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
    //mPrint("Initializing color arrays\n");
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
    //mPrint("end init_color_arrays\n");
}




void invert_current_color_pair() {
    int fg = -1;
    int bg = -1;
    int index = -1;
    int inverse_index = -1;

    if (current_color_pair > MAX_COLOR_PAIRS) {
        exit_with_error("invert_current_color_pair: current_color_pair > MAX_COLOR_PAIRS");
    }

    fg = color_array[current_color_pair][0];
    bg = color_array[current_color_pair][1];

    index = color_pair_array[fg][bg];
    
    if (index != current_color_pair) {
        char msg[1024] = {0};
        snprintf(msg, 1024, "invert_current_color_pair: index != current_color_pair: %d != %d", index, current_color_pair);
        exit_with_error(msg);
    }
    
    inverse_index = color_pair_array[bg][fg];
    current_color_pair = inverse_index;
}




// depending on if the ascii was loaded from file or not
// we may wish to define custom color pairs
void define_color_pairs() {
    mPrint("defining color pairs\n");
    int current_pair = 0;
    int bg_color_start = 0;
    int fg_color_start = 0;
    for (int bg_color = bg_color_start; bg_color < MAX_BG_COLORS; bg_color++) {
        for (int fg_color = fg_color_start; fg_color < MAX_FG_COLORS; fg_color++) {
            init_pair(current_pair, fg_color, bg_color);
            color_array[current_pair][0] = fg_color;
            color_array[current_pair][1] = bg_color;
            color_pair_array[fg_color][bg_color] = current_pair;
            current_pair++;
        }
    }
    max_color_pairs = current_pair;
    max_colors = MAX_FG_COLORS;
    assert(max_color_pairs == MAX_COLOR_PAIRS);
}




// this is a custom pair definer based on the ascii
void define_color_pairs_for_ascii() {
    mPrint("defining color pairs for ascii\n");
    int current_pair = 0;
    for (int i=0; i < 100; i++) {
        for (int j=0; i < 100; i++) {

            if (g_color_palette[i][j] == 1) {
                char buffer[1024] = {0};
                snprintf(buffer, 1024, "i: %d, j: %d current_pair: %d\n", i, j, current_pair);
                mPrint(buffer);
                mPrint("init_pair\n");

                init_pair(current_pair, i, j);

                mPrint("color_array assignment\n");
                color_array[current_pair][0] = i;
                color_array[current_pair][1] = j;
                
                mPrint("color_pair_array assignment\n");
                color_pair_array[i][j] = current_pair;

                mPrint("incrementing current_pair\n");
                current_pair++;
            }
        }
    }
    max_color_pairs = current_pair;
    MAX_COLOR_PAIRS = current_pair;
    max_colors = MAX_FG_COLORS;
    //max_colors = MAX_FG_COLORS;
    //assert(max_color_pairs == MAX_COLOR_PAIRS);
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
    if (y < 0 || y >= canvas_height || x < 0 || x >= canvas_width) {
        return;
    } 
    else if (fg_color < 0 || fg_color >= max_colors) {
        clear();
        refresh();
        cleanup();
        fprintf(stderr, "fg_color: %d\n", fg_color);
        mPrint("write_char_to_canvas: fg_color is out of bounds");
        exit(EXIT_FAILURE);
    }
    else if (bg_color < 0 || bg_color >= max_colors) {
        clear();
        refresh();
        cleanup();
        fprintf(stderr, "bg_color: %d\n", bg_color);
        fprintf(stderr, "max_colors: %d\n", max_colors);
        mPrint("write_char_to_canvas: bg_color is out of bounds");
        exit(EXIT_FAILURE);
    }
    else {
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




void draw_canvas() {
    wchar_t c      = L' ';
    int fg_color   = 0;
    int bg_color   = 0;
    int color_pair = 0;
    int i          = -1;
    int j          = -1;
    int local_y          = -1;
    int local_x          = -1;

    // this loop renders the real canvas
    for (i = 0; i + cy < canvas_height; i++) {
        local_y = i + cy;
        for (j = 0; j + cx < canvas_width; j++) {
            // first, get the color pair
            local_x = j + cx;
            if (local_x < 0 || local_x >= canvas_width || local_y < 0 || local_y >= canvas_height) {
                continue;
            }
            fg_color   = canvas[local_y][local_x].foreground_color;
            bg_color   = canvas[local_y][local_x].background_color;
            color_pair = color_pair_array[fg_color][bg_color];
            attron(COLOR_PAIR(color_pair));
            c = canvas[local_y][local_x].character;
            // this fixes the block-rendering bug for now...
            // there has to be a better way to do this
            if ( c == UPPER_HALF_BLOCK ) {
                mvaddstr(i, j, "▀");
            } 
            else if ( c == BOTTOM_HALF_BLOCK ) {
                mvaddstr(i, j, "▄");
            } 
            else if ( c == FULL_BLOCK ) {
                mvaddstr(i, j, "█");
            } 
            else if (c==LEFT_HALF_BLOCK) {
                mvaddstr(i, j, "▌");
            }
            else if (c==RIGHT_HALF_BLOCK) {
                mvaddstr(i, j, "▐");
            }
            else if (c == DARK_SHADE ) {
                mvaddstr(i, j, "░");
            } 
            else if (c == MEDIUM_SHADE ) {
                mvaddstr(i, j, "▒");
            } 
            else if (c == LIGHT_SHADE ) {
                mvaddstr(i, j, "▓");
            }
            else if (c == UPPER_LEFT_CORNER ) {
                mvaddstr(i, j, "▛");
            }
            else if (c == UPPER_RIGHT_CORNER ) {
                mvaddstr(i, j, "▜");
            }
            else if (c == BOTTOM_LEFT_CORNER ) {
                mvaddstr(i, j, "▙");
            }
            else if (c == BOTTOM_RIGHT_CORNER ) {
                mvaddstr(i, j, "▟");
            }
            else if (c == UPPER_LEFT_BLOCK ) {
                mvaddstr(i, j, "▘");
            }
            else if (c == UPPER_RIGHT_BLOCK ) {
                mvaddstr(i, j, "▝");
            }
            else if (c == BOTTOM_RIGHT_BLOCK ) {
                mvaddstr(i, j, "▗");
            }
            else if (c == BOTTOM_LEFT_BLOCK ) {
                mvaddstr(i, j, "▖");
            }
            else {
                mvaddch(i, j, c);
            }
            attroff(COLOR_PAIR(color_pair));
        }
    }

    // if we are currently in line draw mode...
    // we need to draw a temporary representation of the line
    // after we draw the real canvas
    if (is_line_draw_mode) {
        render_temp_line(line_draw_y0, line_draw_x0, y, x);
    }
    else if (is_rect_draw_mode) {
        render_temp_rect(line_draw_y0, line_draw_x0, y, x);
    }

    refresh();
}




void render_temp_rect(int y1, int x1, int y2, int x2) {
    render_temp_line(y1, x1, y1, x2);
    render_temp_line(y1, x2, y2, x2);
    render_temp_line(y2, x2, y2, x1);
    render_temp_line(y2, x1, y1, x1);
}




void render_temp_line(int y1, int x1, int y2, int x2) {
    attron(COLOR_PAIR(current_color_pair));
    //int x1 = line_draw_x0;
    //int y1 = line_draw_y0;
    int dx = abs(x2-x1);
    int dy = abs(y2-y1);
    int err = dx-dy;
    int e2 = -1;
    int sx = -1;
    int sy = -1;
    if (x1 < x2) {
        sx = 1;
    }
    if (y1 < y2) {
        sy = 1;
    }
    while (true) {
        mvaddstr(y1, x1, convert_wchar_block_to_str(gblock));
        if (x1==x2 && y1==y2) {
            break;
        }
        e2 = 2*err;
        if (e2 > -dy) {
            err = err - dy;
            x1 = x1 + sx;
        }
        if (e2 < dx) {
            err = err + dx;
            y1 = y1 + sy;
        }
    }
    attroff(COLOR_PAIR(current_color_pair));
}




void add_character(wchar_t c) {
    int fg = get_fg_color(color_array, MAX_COLOR_PAIRS, current_color_pair);
    int bg = get_bg_color(color_array, MAX_COLOR_PAIRS, current_color_pair);
    int my = cy + y;
    int mx = cx + x;
    write_char_to_canvas(my, mx, c, fg, bg);
}




void add_character_and_move_right(wchar_t c) {
    add_character(c);
    handle_move_right();
}




void add_block() { 
    //add_character(L' ');
    add_character(gblock);
}




void delete_block() {
    int my = cy + y;
    int mx = cx + x;
    write_char_to_canvas(my, mx, L' ', 0, 0);
}




void incr_color_pair() { 
    current_color_pair++; 
    if (current_color_pair >= max_color_pairs) {
        current_color_pair = 0;
    }
}




void incr_color_pair_by_max(){
    for(int i=0;i<max_colors;i++){
        incr_color_pair();
    }
}




void decr_color_pair() { 
    current_color_pair--; 
    if (current_color_pair < 0) {
        current_color_pair = max_color_pairs - 1;
    }
}




void decr_color_pair_by_max(){
    for(int i=0;i<max_colors;i++){
        decr_color_pair();
    }
}




void handle_save_inner_loop(FILE *outfile) {
    if (outfile == NULL) {
        fail_with_msg("Error opening file for writing");
    }
    for (int i = 0; i < canvas_height; i++) {
        int prev_irc_fg = -1;
        int prev_irc_bg = -1;
        for (int j = 0; j < canvas_width; j++) {
            // now, instead of grabbing characters from stdscr
            // and having to do all this shit on the fly
            // we can just render from the canvas
            wchar_t wc = canvas[i][j].character;
            int fg = canvas[i][j].foreground_color;
            int bg = canvas[i][j].background_color;
            int irc_fg = convert_to_irc_color(fg);
            int irc_bg = convert_to_irc_color(bg);
            bool color_changed = prev_irc_fg != irc_fg || prev_irc_bg != irc_bg;
            //old code here for historic reasons
            //cchar_t character;
            //mvwin_wch(stdscr, i, j, &character);  // Read wide character from the canvas
            //wchar_t wc = character.chars[0];
            //attr_t attribute = character.attr;
            //int color_pair_number = PAIR_NUMBER(attribute);
            if (color_changed) {
                fprintf(outfile, "\x03%02d,%02d%lc", irc_fg, irc_bg, wc);
            }
            else {
                fprintf(outfile, "%lc", wc);
            }
            prev_irc_fg = irc_fg;
            prev_irc_bg = irc_bg;
        }
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
    if (cx-1 >= 0){
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
    switch(c) {
            case KEY_DOWN:
                    handle_move_down();
                    break;
            case KEY_UP:
                    handle_move_up();
                    break;
            case KEY_LEFT:
                    handle_move_left();
                    break;
            case KEY_RIGHT:
                    handle_move_right();
                    break;
                    default:
                    break;
        }
}







void handle_cam_mode_arrow_keys(int c) {
    switch(c) {
            case KEY_DOWN:
                    incr_cam_y();
                    break;
            case KEY_UP:
                    decr_cam_y();
                    break;
            case KEY_LEFT:
                    decr_cam_x();
                    break;
            case KEY_RIGHT:
                    incr_cam_x();
                    break;
            default:
                    break;  
        }


}




void handle_color_pair_change(int c) {
    if (c=='o') {
        decr_color_pair();
    } 
    else if (c=='p') {
        incr_color_pair();
    } 
    else if (c=='O') {
        decr_color_pair_by_max();
    } 
    else if (c=='P') {
        incr_color_pair_by_max();
    }
}




void paintbucket(int y, int x, wchar_t old_char, wchar_t new_char, int old_fg, int old_bg, int new_fg, int new_bg) {
    if (y < 0) {
        return;
    }
    if (y >= canvas_height) {
        return;
    }
    if (x < 0) {
        return;
    }
    if (x >= canvas_width) {
        return;
    }

    if (canvas[y][x].foreground_color != old_fg) {
        return;
    }
    if (canvas[y][x].background_color != old_bg) {
        return;
    }
    if (canvas[y][x].character != old_char) {
        return;
    }


    //if (canvas[y][x].character == new_char) {
    //    return;
    //}
    
    if (canvas[y][x].foreground_color == new_fg) {
        return;
    }
    /*
    if (canvas[y][x].background_color == new_bg) {
        return;
    }
    */

    canvas[y][x].character = new_char;
    canvas[y][x].foreground_color = new_fg;
    canvas[y][x].background_color = new_bg;

    paintbucket(y-1, x, old_char, new_char, old_fg, old_bg, new_fg, new_bg);
    /*
    */
    paintbucket(y+1, x, old_char, new_char, old_fg, old_bg, new_fg, new_bg);
    paintbucket(y, x-1, old_char, new_char, old_fg, old_bg,new_fg, new_bg);
    paintbucket(y, x+1, old_char, new_char, old_fg, old_bg,new_fg, new_bg);
    /*
    */
}




void draw_line(int y1, int x1, int y2, int x2, int fg, int bg) {
    int dx = abs(x2-x1);
    int dy = abs(y2-y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx-dy;
    int e2;
    // TODO: check if x1,y1 is in bounds
    if (x1 < 0 || x1 >= canvas_width || y1 < 0 || y1 >= canvas_height) {
        return;
    }

    if (x2 < 0 || x2 >= canvas_width || y2 < 0 || y2 >= canvas_height) {
        return;
    }

    if (x1==x2 && y1==y2) {
        write_char_to_canvas(y1, x1, gblock, fg, bg);
        return;
    }

    while (true) {
        write_char_to_canvas(y1, x1, gblock, fg, bg);
        if (x1==x2 && y1==y2) {
            break;
        }
        e2 = 2*err;
        if (e2 > -dy) {
            err = err - dy;
            x1 = x1 + sx;
        }
        if (e2 < dx) {
            err = err + dx;
            y1 = y1 + sy;
        }
    }
}



void draw_rect(int y1, int x1, int y2, int x2, int fg, int bg) {
    draw_line(y1, x1, y1, x2, fg, bg);
    draw_line(y1, x2, y2, x2, fg, bg);
    draw_line(y2, x2, y2, x1, fg, bg);
    draw_line(y2, x1, y1, x1, fg, bg);
}






// the 13 is hard-coded AF we need to extend this
// eventually the pixels will no longer be wchar_t but rather a 5-byte array
#define GBLOCK_PALETTE_SIZE 13
static int gblock_palette_index = 0;
static wchar_t gblock_palette[GBLOCK_PALETTE_SIZE] = { 
    FULL_BLOCK,
    UPPER_HALF_BLOCK,
    BOTTOM_HALF_BLOCK,
    LEFT_HALF_BLOCK,
    RIGHT_HALF_BLOCK,
    UPPER_LEFT_BLOCK,
    UPPER_LEFT_CORNER,
    UPPER_RIGHT_BLOCK,
    UPPER_RIGHT_CORNER,
    BOTTOM_LEFT_BLOCK,
    BOTTOM_LEFT_CORNER,
    BOTTOM_RIGHT_BLOCK,
    BOTTOM_RIGHT_CORNER
};




void rotate_gblock_forward() {
    gblock_palette_index = (gblock_palette_index + 1) % GBLOCK_PALETTE_SIZE;
    gblock = gblock_palette[gblock_palette_index];
}




void rotate_gblock_backward() {
    gblock_palette_index = (gblock_palette_index - 1) % GBLOCK_PALETTE_SIZE;
    if (gblock_palette_index < 0) {
        gblock_palette_index = GBLOCK_PALETTE_SIZE - 1;
    }
    gblock = gblock_palette[gblock_palette_index];
}




void handle_gblock_rotation(int c) {
    if (c=='[') {
        rotate_gblock_backward();
    }
    else if (c==']') {
        rotate_gblock_forward();
    }
}



void handle_normal_mode_input(int c) {
    // escape key switches back and forth between normal & text modes
    if (c == 27) {
        if (!is_line_draw_mode && !is_rect_draw_mode) {
            is_text_mode = true;
        }
        else if (is_line_draw_mode) {
            is_line_draw_mode = false;
            line_draw_x0 = -1;
            line_draw_y0 = -1;
            line_draw_x1 = -1;
            line_draw_y1 = -1;
            curs_set(1);
        }
        else if (is_rect_draw_mode) {
            is_rect_draw_mode = false;
            line_draw_x0 = -1;
            line_draw_y0 = -1;
            line_draw_x1 = -1;
            line_draw_y1 = -1;
            curs_set(1);
        }
    } 
    else if (c=='q') {
        quit = 1;
    } 
    else if (c==KEY_MOUSE) {
        MEVENT event;
        if (getmouse(&event) == OK) {
            if (event.bstate & BUTTON1_CLICKED) {
                if (event.x < canvas_width) {
                    x = event.x;
                } 
                else if (event.x >= canvas_width) {
                    x = canvas_width - 1;
                }
                else {
                    x = 0;
                }
                if (event.y < canvas_height) {
                    y = event.y;
                }
                else if (event.y >= canvas_height) {
                    y = canvas_height - 1;
                }
                else {
                    y = 0;
                }
            }
        }
    }
    // to draw a line, we will need a function line(y0,x0,y1,x1)
    // you will press l, then navigate to where you want the line drawn
    else if (c=='l') {
        // y0 and x0 must be set and it must set that we are in line draw mode 
        if (!is_line_draw_mode) {
            is_line_draw_mode = true;
            line_draw_y0 = y;
            line_draw_x0 = x;
            curs_set(0);
        }
        else {
            is_line_draw_mode = false;
            line_draw_y1 = y;
            line_draw_x1 = x;
            int fg = get_current_fg_color();
            int bg = get_current_bg_color();
            draw_line(line_draw_y0, line_draw_x0, line_draw_y1, line_draw_x1, fg, bg);
            line_draw_y0 = -1;
            line_draw_x0 = -1;
            curs_set(1);
        }
        // user must either
        // 1. move to the y1 x1 that they want to drawn the line to and then press space or l or something
        // 2. press escape or something to exit line draw mode
    }
    
    // to draw a square, we will need a function square(y0,x0,y1,x1)
    // you will press s, then navigate to where you want the square drawn
    else if (c=='s') {
        // it is going to be a lot like 4 separate line draws
        if (!is_rect_draw_mode) {
            is_rect_draw_mode = true;
            line_draw_y0 = y;
            line_draw_x0 = x;
            curs_set(0);
        }
        else {
            is_rect_draw_mode = false;
            line_draw_y1 = y;
            line_draw_x1 = x;
            int fg = get_current_fg_color();
            int bg = get_current_bg_color();
            draw_rect(line_draw_y0, line_draw_x0, line_draw_y1, line_draw_x1, fg, bg);
            line_draw_y0 = -1;
            line_draw_x0 = -1;
            curs_set(1);
        }
    }

    else if (c=='C') {
        clear_canvas(canvas, canvas_height, canvas_width);
    } 
    else if (c=='c') {
        is_cam_mode = ! is_cam_mode;
        is_text_mode = false;
    } 
    // fill canvas
    else if (c=='F') {
        int fg = get_current_fg_color();
        int bg = get_current_bg_color();
        fill_canvas(canvas, canvas_height, canvas_width, fg, bg);
    } 
    else if (c==KEY_DC) {
        delete_block();
    } 
    else if (c==KEY_BACKSPACE) {
        delete_block();
        handle_move_left();
    } 
    else if (c=='S')  {
        handle_save();
    } 

    else if (c==KEY_DOWN || c==KEY_UP || c==KEY_RIGHT || c==KEY_LEFT) {
        
        if (is_cam_mode) {
            handle_cam_mode_arrow_keys(c);
        }
        else {
            handle_normal_mode_arrow_keys(c);
        }
    
    } 
    // disabling temporarily
    else if (c=='G') {
        // paintbucket tool
        // smart fill
        int fg = get_current_fg_color();
        int bg = get_current_bg_color();
        
        int old_fg = canvas[y][x].foreground_color;
        int old_bg = canvas[y][x].background_color;
        
        wchar_t old_char = canvas[y][x].character;
        wchar_t new_char = gblock;
        
        paintbucket(y, x, old_char, new_char, old_fg, old_bg, fg, bg);
    }
    /*
    */
    else if (c==' ') {
        add_block();
        handle_move_right();
    } 
    else if (c=='o' || c=='O' || c=='p' || c=='P') {
        handle_color_pair_change(c);
    }
    else if (c=='[' || c==']') {
        // lets just test switching between a full block and a half block
        handle_gblock_rotation(c);
    }
    //else if (c=='E') { // experimental
    //    show_error("This is an error message");
    //    get_int_str_from_user("Enter a number: ");
    //}
    else if (c=='?') {
        show_help();
    }
    // resize the canvas' width
    else if (c=='W') {
        int w = -1;
        // TODO: this is a temporary max width
        // eventually we will decide on something resonable
        const int max_width = 150;
        canvas_pixel_t **new_canvas = NULL;
        canvas_pixel_t **old_canvas = NULL;
        while (w == -1 || w > max_width) {
            w = get_new_width_from_user();
        }
        new_canvas = init_canvas(canvas_height, w);
        if (new_canvas == NULL) {
            show_error("Error creating new canvas");
            return;
        }
        y = 0;
        x = 0;
        move(y,x);
        copy_canvas(new_canvas, canvas, canvas_height, w, canvas_height, canvas_width);
        canvas_width = w;
        old_canvas = canvas;
        canvas = new_canvas;
        free_canvas(old_canvas, canvas_height);
    }
    // resize the canvas' height
    else if (c=='H') {
        int h = -1;
        // whats the max height?
        // eventually we will decide on something resonable
        const int tmp_max_height = 1000;
        canvas_pixel_t **new_canvas = NULL;
        int old_canvas_height = -1;
        canvas_pixel_t **old_canvas = NULL;
        
        while (h==-1 || h > tmp_max_height) {
            h = get_new_height_from_user();
        }
        new_canvas = init_canvas(h, canvas_width);
        if (new_canvas == NULL) {
            show_error("Error creating new canvas");
            return;
        }
        y = 0;
        x = 0;
        move(y,x);
        copy_canvas(new_canvas, canvas, h, canvas_width, canvas_height, canvas_width);
        old_canvas_height = canvas_height;
        canvas_height = h;
        old_canvas = canvas;
        canvas = new_canvas;
        free_canvas(old_canvas, old_canvas_height);
    }
    else if (c==0x09) { // tab
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
    }
    else if (c==KEY_DC) {
        // delete a block from the current location on the canvas
        delete_block();
    }
    else if (c==KEY_BACKSPACE) {
        delete_block();
        handle_move_left();
    }
    else if (c==KEY_UP || c==KEY_DOWN || c==KEY_LEFT || c==KEY_RIGHT) {
        handle_normal_mode_arrow_keys(c);
    }
    else if (c==KEY_RESIZE) {
        getmaxyx(stdscr, terminal_height, terminal_width);
    } 
    else {
        add_character_and_move_right(c);
    }
}




void handle_input() {
    int c = getch(); // start the clock
    clock_gettime(CLOCK_MONOTONIC, &ts0);
    last_char_pressed = c;
    if (is_text_mode) {
        handle_text_mode_input(c);
    } 
    else {
        handle_normal_mode_input(c);
    }
    clock_gettime(CLOCK_MONOTONIC, &ts1); // stop clock
    // do nanoseconds
    //last_cmd_ns = (ts1.tv_sec - ts0.tv_sec) * 1000000000 + (ts1.tv_nsec - ts0.tv_nsec);
    // do microseconds
    //last_cmd_us = last_cmd_ns / 1000;
    // do milliseconds
    //last_cmd_ms = last_cmd_ns / 1000000;

    // do milliseconds
    last_cmd_ms = (ts1.tv_sec - ts0.tv_sec) * 1000.0 + (ts1.tv_nsec - ts0.tv_nsec) / 1000000.0;
}




void draw_hud() {
    draw_hud_background(hud_color, terminal_height, terminal_width);
    int fg = get_fg_color(color_array, MAX_COLOR_PAIRS, current_color_pair);
    //int bg = get_bg_color(color_array, MAX_COLOR_PAIRS, current_color_pair);
    draw_hud_row_1(canvas, 
        fg,
        y, 
        x,
        cy,
        hud_color, 
        terminal_height, 
        terminal_width, 
        canvas_height,
        canvas_width,
        current_color_pair,
        
            is_text_mode ? 1 : 
            is_line_draw_mode ? 2 : 
            is_rect_draw_mode ? 3 : 
            0,
        
        gblock
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
    draw_hud_row_3(terminal_height, terminal_width, hud_color, last_cmd_ms);
    reset_cursor();
}




void init_ncurses() {
    const size_t smallest_width = 1;
    initscr();
    clear();
    noecho();
    keypad(stdscr, true);
    if (!has_colors()) {
        exit_with_error("Your terminal does not support color");
    }
    start_color();
    use_default_colors();
    if (!can_change_color()) {
        can_change_color_mode = false;
        MAX_FG_COLORS = 8;
        MAX_BG_COLORS = 8;
        MAX_COLOR_PAIRS = 64;
    }
    else {
        define_colors();
    }
    getmaxyx(stdscr, terminal_height, terminal_width);
    if (terminal_width < smallest_width) {
        fprintf(stderr, "Error: terminal too small\n");
        exit(EXIT_FAILURE);
    }
    curs_set(1);
    mousemask(ALL_MOUSE_EVENTS, NULL);
}




void init_program() {
    mPrint("Initializing program\n");
    setlocale(LC_ALL, "");
    setenv("ESCDELAY", "200", 1); // 200 ms delay for escape sequences
    init_ncurses();
    init_color_arrays();
    define_color_pairs();
    handle_canvas_load();
    // to come soon...
    //if (was_loaded_from_file) {
    //    mPrint("Loaded from file\n");
    //    init_color_arrays();
    //    define_color_pairs_for_ascii();
    //}
    //else {
    //    mPrint("Not loaded from file\n");
    //    init_color_arrays();
    //    define_color_pairs();
    //}
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
        if (canvas == NULL) {
            exit_with_error("Error: could not read file");
        }
        was_loaded_from_file = true;
    } else {
        canvas = init_canvas(canvas_height, canvas_width);
    }
    //mPrint("done");
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



void show_help() {
    char *helpfilename = "helpfile.txt";
    char help_msg[1024] = {0};
    FILE *helpfile = fopen(helpfilename, "r");
    if (helpfile == NULL) {
        exit_with_error("Error: could not open help file");
    }
    // read whole file into help_msg
    fread(help_msg, 1, 1024, helpfile);
    fclose(helpfile);
    clear();
    mvaddstr(0,0,help_msg);
    refresh();
    getch();
    clear();
    refresh();
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
    char *prompt = "Enter new height: ";
    char user_input[4] = {0}; // up to a 3-digit number
    int user_input_int = -1;
    echo();
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
    if (!ncurses_initialized) {
        mPrint("Freeing color arrays");
    }
    if (color_array_initialized) {
        for (int i = 0; i < MAX_COLOR_PAIRS; i++) {
            free(color_array[i]);
        }
        free(color_array);
        color_array_initialized = false;
    }
}




void free_color_pair_array() {
    if (!ncurses_initialized) {
        mPrint("Freeing color pair array");
    }
    if (color_pair_array_initialized) {
        for (int i = 0; i < MAX_FG_COLORS; i++) {
            free(color_pair_array[i]);
        }
        free(color_pair_array);
        color_pair_array_initialized = false;
    }   
}


