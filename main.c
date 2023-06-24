#define _XOPEN_SOURCE_EXTENDED 1

#include <ncurses.h>
#include <ctype.h>
#include <locale.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

// this is the base of the canvas
typedef struct 
{
    wchar_t character;
    int foreground_color;
    int background_color;
} canvas_pixel_t;



//canvas_pixel_t canvas[1024][1024] = { 0 };
canvas_pixel_t **canvas = NULL;
int canvas_width  = -1;
int canvas_height = -1;



// something broken about ncurses movement keys but i hacked this together
#define MV_DOWN 66
#define MV_UP 65
#define MV_LEFT 68
#define MV_RIGHT 67

int max_y              = -1;
int max_x              = -1;
int y                  = 0;
int x                  = 0;
int quit               = 0;
int hud_color          = 7;
int current_color_pair = 0;
int max_color_pairs    = -1;
int max_colors         = -1;

// this is the current filename
char filename[1024] = {0};

// this is used to quickly grab info about what the current
// fg and bg color is based on the current "color pair"
// 128x128 = 16384
// 16x16 = 256
//int color_array[16384][2] = { 0 };
int color_array[256][2] = { 0 };
int color_pair_array[256][256] = { 0 };


int convert_to_irc_color(int color);
void handle_save_inner_loop(FILE *outfile);
void handle_save();
void define_color_pairs();
void draw_initial_ascii();
void handle_input();
void draw_hud();
void draw_hud_background();
void draw_canvas();
void reset_cursor();
void parse_arguments(int argc, char **argv);
int get_fg_color(int color_pair);
int get_bg_color(int color_pair);
void init_program();
void init_canvas(int width, int height);
void free_canvas();
void write_char_to_canvas(int y, int x, wchar_t c, int fg_color, int bg_color);
void fail_with_msg(const char *msg);






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
    
    // free the canvas
    free_canvas();

    return EXIT_SUCCESS;
}


void reset_cursor() 
{ 
    move(y, x); 
}


void parse_arguments(int argc, char **argv) 
{
    int c = -1;
    while ((c = getopt(argc, argv, "f:")) != -1) 
    {
        // set filename of ascii
        if (c == 'f') 
        {
            //filename = optarg;
            strncpy(filename, optarg, 1024);
        }
        else if (c == '?') 
        {
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
        }
    }
}


void define_color_pairs() 
{
    int current_pair = 0;
    const int local_max_colors = 16; // for now...
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



void fail_with_msg(const char *msg) 
{
    endwin();
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
            // convert the char at lines[i][j] to a wchar_t
            // store in the canvas

            write_char_to_canvas(i, j, lines[i][j], fg_color, bg_color);

            //canvas[i][j].character = lines[i][j];
            //canvas[i][j].foreground_color = fg_color;
            //canvas[i][j].background_color = bg_color;
        }
    }

    //attron(COLOR_PAIR(current_color_pair));
    //for (int i = 0; i < 3; i++) 
    //{
    //    mvaddstr(i, 0, lines[i]);
    //}
    //attroff(COLOR_PAIR(current_color_pair));
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
            //int color_pair = canvas[i][j].foreground_color + (canvas[i][j].background_color * 16);

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

            // turn off the color pair
            attroff(COLOR_PAIR(color_pair));
        }
    }
}




void add_block() 
{ 
    //attron(COLOR_PAIR(current_color_pair)); 
    // add the block to the canvas
    canvas[y][x].character = L'█';
    // store the color component
    canvas[y][x].foreground_color = get_fg_color(current_color_pair);
    canvas[y][x].background_color = get_bg_color(current_color_pair);
    // this will soon go away once we have a function to render the canvas
    // that way we dont have to write directly to stdscr
    //mvaddstr(y, x, "█"); 
    //attroff(COLOR_PAIR(current_color_pair)); 
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
        perror("Error opening file for writing");
        exit(-1);
    }
    //int canvas_width = max_x; 
    //int canvas_height = max_y-2;
    int prev_irc_fg_color = -1;
    int prev_irc_bg_color = -1;
    for (int i = 0; i < canvas_height; i++) 
    {
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


void handle_input() 
{
    int c = getch();
    if (c == 'q') 
    { 
        quit = 1; 
    }
    else if (c == 's') 
    {
        handle_save();       
    }
    // movement keys
    else if (c == MV_DOWN) 
    {
        if (y+1 < max_y-2) 
        {
            y++;
        }
    }
    else if (c == MV_UP) 
    {
        if (y-1 >= 0) 
        {
            y--;
        }
    }
    else if (c == MV_LEFT) 
    {
        if (x-1 >= 0) 
        {
            x--;
        }
    }
    else if (c == MV_RIGHT) 
    {
        if (x+1 < max_x) 
        {
            x++;
        }
    }
    // variations on adding blocks
    else if (c == ' ') 
    { 
        add_block(); 
        if (x+1 < max_x) 
        {
            x++;
        }
    }
    // color changing
    else if (c == 'o') 
    { 
        decr_color_pair(); 
    }
    else if (c == 'p') 
    { 
        incr_color_pair(); 
    }
    else if (c == 'O') 
    { 
        decr_color_pair_by_max(); 
    }
    else if (c == 'P') 
    { 
        incr_color_pair_by_max(); 
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
    int hud_y = max_y-2;
    int hud_x = 0;
    int hud_max_x = max_x;
    int fg_color = current_color_pair % max_colors;
    char *str = calloc(1, hud_max_x);
    sprintf(str, "y: %03d | # %03d FG CurrentColorPair(%05d) FG %03d Filename: %s", y, fg_color, current_color_pair, 0, filename );
    move(hud_y,hud_x);
    for (char c = str[0]; c != '\0'; c = str[++hud_x]) 
    {
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
    }
    free(str);
}


void draw_hud_row_2() 
{
    int cursor_y = -1;
    int cursor_x = -1;
    char str[16] = {0};
    sprintf(str, "x: %03d | ", x);
    mvaddstr(max_y-1, 0, str);
    switch_between_hud_and_current_color();
    addstr(" ");
    switch_between_current_and_hud_color();
    char str2[64] = {0};
    int bg_color = current_color_pair / max_colors;
    // get where our cursor is
    getyx(stdscr, cursor_y, cursor_x);
    // now that we have memory of where we stopped drawing...
    chtype character = mvinch(y, x);
    attr_t attribute = character & A_COLOR;
    int color_pair_number = PAIR_NUMBER(attribute);
    //move back to where the cursor was
    move(cursor_y, cursor_x);
    sprintf(str2, " %03d BG PairUnderCursor (%05d) BG %03d %dx%d", bg_color, color_pair_number, 0, max_y, max_x);
    addstr(str2);
    attroff(COLOR_PAIR(hud_color));
    reset_cursor();
}


void draw_hud_background() 
{
    for (int j = max_y - 2; j < max_y; j++) 
    { 
        for (int i = 0; i < max_x-1; i++) 
        {
            mvaddstr(j, i, " ");
        }
    }
}


void draw_hud() 
{
    attron(COLOR_PAIR(hud_color));
    draw_hud_background();
    draw_hud_row_1();
    draw_hud_row_2();
}


void init_program() 
{
    setlocale(LC_ALL, "");
    initscr();
    clear();
    noecho();
    start_color();
    use_default_colors();
    define_color_pairs();
    getmaxyx(stdscr, max_y, max_x);
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
    //canvas = malloc(sizeof(canvas_pixel_t *) * canvas_height);
    canvas = calloc(canvas_height, sizeof(canvas_pixel_t *));
    for (int i = 0; i < canvas_height; i++) 
    {
        //canvas[i] = malloc(sizeof(canvas_pixel_t) * canvas_width);
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


