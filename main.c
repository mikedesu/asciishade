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



// something broken about ncurses movement keys but i hacked this together
#define MV_DOWN 66
#define MV_UP 65
#define MV_LEFT 68
#define MV_RIGHT 67
#define MAX_FG_COLORS 16
#define MAX_BG_COLORS 16
#define MAX_COLOR_PAIRS (MAX_FG_COLORS * MAX_BG_COLORS)




// this is the base of the canvas
typedef struct 
{
    wchar_t character;
    int foreground_color;
    int background_color;
} canvas_pixel_t;




bool is_text_mode = false;


canvas_pixel_t **canvas = NULL;
int last_char_pressed   = -1;
int canvas_width        = -1;
int canvas_height       = -1;
int max_y               = -1;
int max_x               = -1;
int y                   = 0;
int x                   = 0;
int quit                = 0;
int hud_color           = 7;
int current_color_pair  = 0;
int max_color_pairs     = -1;
int max_colors          = -1;
// this is the current filename
char filename[1024] = {0};
// this is used to quickly grab info about what the current
// fg and bg color is based on the current "color pair"
// 128x128 = 16384
// 16x16 = 256
int color_array[MAX_COLOR_PAIRS][2]                = { 0 };
int color_pair_array[MAX_FG_COLORS][MAX_BG_COLORS] = { 0 };


int convert_to_irc_color(int color);
int get_fg_color(int color_pair);
int get_bg_color(int color_pair);

void add_character(wchar_t c);
void add_character_and_move_right(wchar_t c);
void add_block_and_move_right();
void add_block_and_move_left();
void add_block_and_move_up();
void add_block_and_move_down();
void define_color_pairs();
void draw_hud();
void draw_hud_background();
void draw_initial_ascii();
void draw_canvas();
void fail_with_msg(const char *msg);
void free_canvas();
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
void init_canvas(int width, int height);
void parse_arguments(int argc, char **argv);
void print_help(char **argv);
void reset_cursor();
void write_char_to_canvas(int y, int x, wchar_t c, int fg_color, int bg_color);
void cleanup();



int main(int argc, char *argv[]) 
{
    parse_arguments(argc, argv);
    init_program();
    draw_initial_ascii();
    refresh();
    while (!quit) 
    {
        clear();
        draw_canvas();
        draw_hud();
        handle_input();
        refresh();
    }
    endwin();
    free_canvas();
    return EXIT_SUCCESS;
}



void reset_cursor() 
{ 
    move(y, x); 
}



void print_help(char **argv) 
{
    printf("Usage: %s [OPTION]...\n", argv[0]);
    printf("  -f, --filename=FILENAME    specify a filename to save to\n");
    printf("  -h, --help                 display this help and exit\n");
}



void parse_arguments(int argc, char **argv) 
{
    // parsing arguments using getopt_long
    int c = -1;
    int option_index = 0;
    static struct option longoptions[] = 
    {
        {"filename", 1, NULL, 'f'},
        {"help",     0, NULL, 'h'}
    };

    while (1) 
    {
        c = getopt_long(argc, argv, "f:h", longoptions, &option_index);
        if (c == -1)
        {
            break;
        }

        switch (c) 
        {
            case 'f':
                strncpy(filename, optarg, 1024);
                break;
            case 'h':
                print_help(argv);
                exit(EXIT_SUCCESS);
                break;
            case '?':
                if (optopt == 'f') 
                {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if (isprint(optopt)) 
                {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                }
                else 
                {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                exit(EXIT_FAILURE);
                break;
            default:
                abort();
        }
    }
}



void define_color_pairs() 
{
    int current_pair = 0;
    const int local_max_colors = MAX_FG_COLORS; // for now...

    for (int bg_color = 0; bg_color < local_max_colors; bg_color++) 
    {
        for (int fg_color = 0; fg_color < local_max_colors; fg_color++) 
        {
            init_pair(current_pair, fg_color, bg_color);
            // store the color pair in the array
            color_array[current_pair][0] = fg_color;
            color_array[current_pair][1] = bg_color;
            // store the color pair in the array
            color_pair_array[fg_color][bg_color] = current_pair;
            current_pair++;
        }
    }

    max_color_pairs = current_pair;
    max_colors = local_max_colors;
}



int get_fg_color(int color_pair) 
{
    return color_array[color_pair][0];
}



int get_bg_color(int color_pair) 
{
    return color_array[color_pair][1];
}


void cleanup() 
{
    endwin();
    free_canvas();
}


void fail_with_msg(const char *msg) 
{
    cleanup();
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}



void write_char_to_canvas(int y, int x, wchar_t c, int fg_color, int bg_color) 
{
    // check to make sure y,x is within the canvas
    if (y < 0 || y >= canvas_height || x < 0 || x >= canvas_width) 
    {
        fail_with_msg("write_char_to_canvas: y,x is out of bounds");
    }
    // make sure the other parameters arent absurd
    if (fg_color < 0 || fg_color >= max_colors || bg_color < 0 || bg_color >= max_colors) 
    {
        fail_with_msg("write_char_to_canvas: fg_color or bg_color is out of bounds");
    }
    canvas[y][x].character = c;
    canvas[y][x].foreground_color = fg_color;
    canvas[y][x].background_color = bg_color;
} 



void draw_initial_ascii() 
{
    char *lines[3] = 
    { 
        "Welcome to asciishade", 
        "by darkmage", 
        "www.evildojo.com" 
    };
    current_color_pair = 1;
    int fg_color = get_fg_color(current_color_pair);
    int bg_color = get_bg_color(current_color_pair);
    for (int i=0; i < 3; i++) {
        for (int j=0; j < strlen(lines[i]); j++) {
            // convert the lines[i][j] to a wchar_t and store in the canvas
            write_char_to_canvas(i, j, lines[i][j], fg_color, bg_color);
        }
    }
}



void draw_canvas() 
{
    wchar_t c      = L' ';
    int fg_color   = 0;
    int bg_color   = 0;
    int color_pair = 0;
    int i          = -1;
    int j          = -1;
    for (i = 0; i < canvas_height; i++) 
    {
        for (j = 0; j < canvas_width; j++) 
        {
            // set the color pair
            // first, get the color pair
            // it should be equal to fg_color + (bg_color * 16)
            fg_color   = canvas[i][j].foreground_color;
            bg_color   = canvas[i][j].background_color;
            color_pair = color_pair_array[fg_color][bg_color];
            // draw the character
            attron(COLOR_PAIR(color_pair));
            c = canvas[i][j].character;
            // this fixes the block-rendering bug for now...
            // there has to be a better way to do this
            if ( c == L'█' ) 
            {
                mvaddstr(i, j, "█");
            }
            else 
            {
                mvaddch(i, j, c);
            }
            attroff(COLOR_PAIR(color_pair));
        }
    }
}



void add_character(wchar_t c) 
{
    // add the character to the canvas
    canvas[y][x].character = c;
    // store the color component
    canvas[y][x].foreground_color = get_fg_color(current_color_pair);
    canvas[y][x].background_color = get_bg_color(current_color_pair);
}



void add_character_and_move_right(wchar_t c) 
{
    add_character(c);

    if (x < canvas_width) 
    {
        x++;
    }
}



void add_block() 
{ 
    add_character(L'█');
}



void incr_color_pair() 
{ 
    current_color_pair++; 
    if (current_color_pair >= max_color_pairs) 
    {
        current_color_pair = 0;  
    }
}




void incr_color_pair_by_max() 
{ 
    for (int i = 0; i < max_colors; i++) 
    {
        incr_color_pair();                                   
    }
}



void decr_color_pair() 
{ 
    current_color_pair--; 
    if (current_color_pair < 0) 
    { 
        current_color_pair = max_color_pairs-1; 
    }
}



void decr_color_pair_by_max() 
{ 
    for (int i = 0; i < max_colors; i++) 
    { 
        decr_color_pair();                                   
    }
}



int convert_to_irc_color(int color) 
{
    switch (color) 
    {
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



void handle_save_inner_loop(FILE *outfile) 
{
    if (outfile == NULL) 
    {
        fail_with_msg("Error opening file for writing");
    }

    for (int i = 0; i < canvas_height; i++) 
    {
        int prev_irc_fg_color = -1;
        int prev_irc_bg_color = -1;
        for (int j = 0; j < canvas_width; j++) 
        {
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
                fprintf(outfile, "\x03%d,%d%lc", irc_foreground_color, irc_background_color, wc);
            }
            else 
            {
                fprintf(outfile, "%lc", wc);
            }
            prev_irc_fg_color = irc_foreground_color;
            prev_irc_bg_color = irc_background_color;
        }
        fprintf(outfile, "\x03\n");
    }   
}



void handle_save() 
{
    // 1. filename is empty
    // 2. filename is not empty
    if (strcmp(filename, "") != 0) 
    { 
        // test writing out file
        FILE *outfile = fopen(filename, "w");
        if (outfile == NULL) 
        {
            perror("Error opening file for writing");
            exit(-1);
        }
        handle_save_inner_loop(outfile);
        fclose(outfile);
    }
}


void handle_move_down() 
{
    if (y+1 < canvas_height) 
    {
        y++;
    }
}


void handle_move_up() 
{
    if (y-1 >= 0) 
    {
        y--;
    }
}



void handle_move_left() 
{
    if (x-1 >= 0) 
    {
        x--;
    }
}



void handle_move_right() 
{
    if (x+1 < canvas_width) 
    {
        x++;
    }
}


void add_block_and_move_right() 
{
    add_block();
    handle_move_right();
}


void add_block_and_move_left() 
{
    add_block();
    handle_move_left();
}


void add_block_and_move_up() 
{
    add_block();
    handle_move_up();
}


void add_block_and_move_down() 
{
    add_block();
    handle_move_down();
}




void handle_normal_mode_input(int c) 
{
    if (c == '`')
    {
        is_text_mode = true;
    }
    else if (c=='q') 
    {
        quit = 1;
    }
    else if (c=='S') 
    {
        handle_save();
    }
    else if (c==KEY_DOWN)
    {
        handle_move_down();
    }
    else if (c==KEY_UP)
    {
        handle_move_up();
    }
    else if (c==KEY_LEFT)
    {
        handle_move_left();
    }
    else if (c==KEY_RIGHT)
    {
        handle_move_right();
    }
    else if (c==' ')
    {
        add_block_and_move_right();
    }
    else if (c=='a')
    {
        add_block_and_move_left();
    }
    else if (c=='w')
    {
        add_block_and_move_up();
    }
    else if (c=='s')
    {
        add_block_and_move_down();
    }
    else if (c=='d')
    {
        add_block_and_move_right();
    }
    
    else if (c=='o')
    {
        decr_color_pair();
    }
    else if (c=='p')
    {
        incr_color_pair();
    }
    else if (c=='O')
    {
        decr_color_pair_by_max();
    }
    else if (c=='P') 
    {
        incr_color_pair_by_max();
    }
}



void handle_text_mode_input(int c)
{
    if (c == '`') 
    {
        is_text_mode = false;
    }
    else if (c==KEY_LEFT) 
    {
        handle_move_left();
    }
    else if (c==KEY_RIGHT) 
    {
        handle_move_right();
    }
    else if (c==KEY_UP) 
    {
        handle_move_up();
    }
    else if (c==KEY_DOWN) 
    {
        handle_move_down();
    }
    else 
    {
        add_character_and_move_right(c);
    }
}



void handle_input() 
{
    int c = getch();
    
    last_char_pressed = c;

    if (is_text_mode)
    {
        handle_text_mode_input(c);
    }
    else 
    {
        handle_normal_mode_input(c);
    }
}



void switch_between_hud_and_current_color() 
{ 
    attroff(COLOR_PAIR(hud_color));  
    attron(COLOR_PAIR(current_color_pair)); 
}



void switch_between_current_and_hud_color() 
{ 
    attroff(COLOR_PAIR(current_color_pair));  
    attron(COLOR_PAIR(hud_color)); 
}



void draw_hud_row_1() 
{
    attron(COLOR_PAIR(hud_color));

    int hud_y     = max_y-2;
    int hud_x     = 0;
    int hud_max_x = max_x;
    int fg_color  = get_fg_color(current_color_pair);
    int fg_color_cursor = canvas[y][x].foreground_color;

    // perhaps the string could be longer than the hud_max_x,
    // since we are going to stop printing there anyway

    // estimating length of the hud status row
    int len_of_str = (24 + strlen(filename) + 1)*2;
    char *str      = calloc(1, len_of_str);

    // before we do this sprintf, we need to make sure the terminal is wide enough
    // to display the entire string.  if it's not, we need to truncate the string
    // and display a warning message

    sprintf(str, "y:%03d|#%02d(%02x)F%02d %s", y, fg_color, current_color_pair, fg_color_cursor, 
        is_text_mode ? "TEXT" : "NORMAL"
    );

    // get the length of str
    int str_len = strlen(str);
    if (str_len > hud_max_x) 
    {
        // truncate the string
        str[hud_max_x-1] = '\0';
    }

    move(hud_y,hud_x);
    
    for (char c = str[0]; c != '\0'; c = str[hud_x]) 
    {
        if (hud_x >= hud_max_x) 
        {
            break;
        }

        if (c=='#') 
        {
            switch_between_hud_and_current_color();
            addstr("█");
            switch_between_current_and_hud_color();
        }
        else 
        {
            addch(c);
        }
        hud_x++;
    }
    free(str);
    attroff(COLOR_PAIR(hud_color));
}



void draw_hud_row_2() 
{
    attron(COLOR_PAIR(hud_color));
    int hud_y            = max_y-1;
    int hud_x            = 0;
    int hud_max_x        = max_x;
    const int len_of_str = 40;
    
    move(hud_y,hud_x);
    
    char *str            = calloc(1, len_of_str);
    if (str == NULL) 
    {
        mPrint("Error allocating memory for str\n");
        exit(EXIT_FAILURE);
    }
    
    int bg_color = get_bg_color(current_color_pair);
    int fg_color_cursor = canvas[y][x].foreground_color;
    int bg_color_cursor = canvas[y][x].background_color;
    int color_pair_num = color_pair_array[fg_color_cursor][bg_color_cursor];

    sprintf(str, "x:%03d|#%02d(%02x)B%02d %dx%d %d", x, bg_color, 
        color_pair_num, bg_color_cursor, canvas_height, canvas_width, 
        last_char_pressed);

    for (char c = str[0]; c != '\0'; c = str[hud_x]) 
    {
        if (hud_x >= hud_max_x) 
        {
            break;
        }

        else if (c=='#') 
        {
            switch_between_hud_and_current_color();
            addstr(" ");
            switch_between_current_and_hud_color();
        }
        else 
        {
            addch(c);
        }
        hud_x++;
    }

    free(str);
    attroff(COLOR_PAIR(hud_color));
}



void draw_hud_background() 
{
    attron(COLOR_PAIR(hud_color));
    for (int j = max_y - 2; j < max_y; j++) 
    { 
        for (int i = 0; i < max_x; i++) 
        {
            mvaddstr(j, i, " ");
        }
    }
    attroff(COLOR_PAIR(hud_color));
}



void draw_hud() 
{
    draw_hud_background();
    draw_hud_row_1();
    draw_hud_row_2();
    reset_cursor();
}



void init_program() 
{
    setlocale(LC_ALL, "");
    initscr();
    clear();
    noecho();
    start_color();
    use_default_colors();
    keypad(stdscr, true);


    define_color_pairs();

    getmaxyx(stdscr, max_y, max_x);
    // if the terminal is too small, exit
    if (max_x < 4) 
    {
        fprintf(stderr, "Error: terminal too small\n");
        exit(EXIT_FAILURE);
    }
    
    // make the cursor visible
    curs_set(1);
    // initialize the canvas
    init_canvas(max_x, max_y-2);
}



void init_canvas(int width, int height) 
{
    // do some basic checks on the input parameters
    if (width < 1 || height < 1) 
    {
        fprintf(stderr, "Error: width and height must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    canvas_width  = width;
    canvas_height = height;
    canvas = calloc(canvas_height, sizeof(canvas_pixel_t *));
    for (int i = 0; i < canvas_height; i++) 
    {
        canvas[i] = calloc(canvas_width, sizeof(canvas_pixel_t));

        for (int j = 0; j < canvas_width; j++) 
        {
            canvas[i][j].foreground_color = 0;
            canvas[i][j].background_color = 0;
            canvas[i][j].character = ' ';
        }
    }

}



void free_canvas() 
{
    printf("Freeing the canvas memory...\n");
    for (int i = 0; i < canvas_height; i++) 
    {
        free(canvas[i]);
    }
    free(canvas);
}


