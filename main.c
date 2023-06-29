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


#define MAX_FG_COLORS 8
#define MAX_BG_COLORS 8
#define MAX_COLOR_PAIRS (MAX_FG_COLORS * MAX_BG_COLORS)

#define DEFAULT_COLOR_PAIR 1


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


int get_fg_color(int color_pair);
int get_bg_color(int color_pair);

void add_block();
void delete_block();
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
    free_canvas(canvas, canvas_height);
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
    mPrint("Parsing arguments...\n");
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
    free_canvas(canvas, canvas_height);
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
        cleanup();
        fprintf(stderr, "y: %d\nx: %d\n", y, x);
        mPrint("write_char_to_canvas: y,x is out of bounds");
        exit(EXIT_FAILURE);
    }

    // make sure the other parameters arent absurd
    if (fg_color < 0 || fg_color >= max_colors) 
    {
        cleanup();
        fprintf(stderr, "fg_color: %d\n", fg_color);
        mPrint("write_char_to_canvas: fg_color is out of bounds");
        exit(EXIT_FAILURE);
    }

    if (bg_color < 0 || bg_color >= max_colors) 
    {
        cleanup();
        fprintf(stderr, "bg_color: %d\n", bg_color);
        mPrint("write_char_to_canvas: bg_color is out of bounds");
        exit(EXIT_FAILURE);
    }

    canvas[y][x].character = c;
    canvas[y][x].foreground_color = fg_color;
    canvas[y][x].background_color = bg_color;
} 



void draw_initial_ascii() 
{


    /*
    char *lines[3] = 
    { 
        "Welcome to asciishade", 
        "by darkmage", 
        "www.evildojo.com" 
    };

    // this would set the current color pair in order to draw the ascii
    current_color_pair = DEFAULT_COLOR_PAIR;
    
    int fg_color = get_fg_color(current_color_pair);
    int bg_color = get_bg_color(current_color_pair);
    */

    // read in test2.ascii

    //FILE *fp = fopen("test2.ascii", "r");
    canvas = read_ascii_from_filepath("test2.ascii", &canvas_height, &canvas_width);

    //for (int i=0; i < canvas_height; i++) {
    //    for (int j=0; j < canvas_width; j++) {
            // convert the lines[i][j] to a wchar_t and store in the canvas
            //write_char_to_canvas(i, j, lines[i][j], fg_color, bg_color);
            
    //        canvas_pixel_t pixel = my_canvas[i][j];
    //        write_char_to_canvas(i, j, pixel.character, pixel.foreground_color, pixel.background_color);
            //write_char_to_canvas(i, j, , fg_color, bg_color);
    //    }
    //}

    //free_canvas(my_canvas, canvas_height);
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
    //canvas[y][x].character = c;
    // store the color component
    //canvas[y][x].foreground_color = get_fg_color(current_color_pair);
    //canvas[y][x].background_color = get_bg_color(current_color_pair);
    write_char_to_canvas(y, x, c, get_fg_color(current_color_pair), get_bg_color(current_color_pair));
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



void delete_block() 
{
    // delete the character from the canvas
    canvas[y][x].character = L' ';
    // store the color component
    // this should be set to the default color pair
    // what was the initial ascii drawn in?
    canvas[y][x].foreground_color = 0;
    //canvas[y][x].foreground_color = get_fg_color(DEFAULT_COLOR_PAIR);
    canvas[y][x].background_color = 0;
    //canvas[y][x].background_color = get_bg_color(DEFAULT_COLOR_PAIR);
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


// this is not an accurate function
// it is close, but we are:
//
// 1. not defining all 16 colors
// 2. not scaling up to all of the colors that ncurses OR irc can handle

#define COLOR_BRIGHT_BLACK   8
#define COLOR_BRIGHT_BLUE    9
#define COLOR_BRIGHT_GREEN   10
#define COLOR_BRIGHT_CYAN    11
#define COLOR_BRIGHT_RED     12
#define COLOR_BRIGHT_MAGENTA 13
#define COLOR_BRIGHT_YELLOW  14
#define COLOR_BRIGHT_WHITE   15

// https://modern.ircdocs.horse/formatting.html




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
                fprintf(outfile, "\x03%02d,%02d%lc", irc_foreground_color, irc_background_color, wc);
                //fwprintf(outfile, L"\x03%02d,%02d%c", irc_foreground_color, irc_background_color, wc);
            }
            else 
            {
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
    else if (c==KEY_DC) 
    {
        // delete a block from the current location on the canvas
        delete_block();
    }
    else if (c==KEY_BACKSPACE) 
    {
        delete_block();
        handle_move_left();
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
    else if (c==KEY_DC) 
    {
        // delete a block from the current location on the canvas
        delete_block();
    }
    else if (c==KEY_BACKSPACE) 
    {
        delete_block();
        handle_move_left();
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
    mPrint("Initializing program\n");
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
    // for now, we are going to make the canvas the same size as the terminal
    // when we go to read in ascii files,
    canvas_height = max_y - 2;
    canvas_width  = max_x;
    //canvas_height = 20;
    //canvas_width  = 80;
    //canvas = init_canvas(canvas_height, canvas_width);
}


