#include "tools.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mPrint.h"
#include "colors.h"


// global color palette
int g_color_palette[100][100];


void print_canvas(canvas_pixel_t **canvas, int canvas_height, int canvas_width) {
    if (canvas == NULL) {
        printf("Error: canvas is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (canvas_height <= 0) {
        printf("Error: canvas_height is <= 0\n");
        exit(EXIT_FAILURE);
    }
    if (canvas_width <= 0) {
        printf("Error: canvas_width is <= 0\n");
        exit(EXIT_FAILURE);
    }
    // we are printing the ascii to stdout
    // so we will be using ANSI terminal escape sequences to handle color
    // old-school style
    for (int i=0; i<canvas_height; i++) {
        for (int j=0; j<canvas_width; j++) {
            canvas_pixel_t pixel = canvas[i][j];
            //int fg_color = pixel.foreground_color;
            //int bg_color = pixel.background_color;
            wchar_t c = pixel.character;
            printf("[%lc]", c);
        }
        printf("\n");
    }
}


int read_ascii_into_canvas_unsafe(FILE *fp, canvas_pixel_t **canvas, int canvas_height, int canvas_width) {
    mPrint("read_ascii_into_canvas_unsafe()");
    int h = 0;
    int canvas_y = 0;
    int canvas_x = 0;
    int current_fg_color = DEFAULT_FG_COLOR;
    int current_bg_color = DEFAULT_BG_COLOR;
    char buffer[1024] = {0};
    // we are going to attempt to track how many unique
    // colors are used in a given ascii
    //char palette_colors[100][100];
    if (fp == NULL) {
        return -1;
    }
    if (canvas == NULL) {
        return -2;
    }
    if (canvas_height <= 0) {
        return -3;
    }
    if (canvas_width <= 0) {
        return -4;
    }
    // reset the file pointer to the beginning of the file
    fseek(fp, 0, SEEK_SET);
    while (fgets(buffer, 1024, fp) != NULL) {
        //get_colors_in_line(buffer);
        h++;
        int w = 0;
        for (size_t i = 0; i < strlen(buffer); i++) {
            unsigned char c = buffer[i];
            //printf("line: %d\n", h);
            if (c == 0x03) {
                // we will have to read color codes to skip past them when computing width
                // we can expect after a ctrl char that the color code is of format:
                // 00,00
                // where the first 00 is the foreground color and the second 00 is the background color
                
                if (i+5 >= strlen(buffer)) {
                    mPrint("Error: color code is incomplete");
                    continue;
                }

                char fg_chars[3] = { buffer[i+1], buffer[i+2], 0 };
                char bg_chars[3] = { buffer[i+4], buffer[i+5], 0 };
                char comma = buffer[i+3]; // ,
                
                // the state machine for colors is a bit more complicated than this...
                bool first_c  = fg_chars[0] >= '0' && fg_chars[0] <= '9';
                bool second_c = fg_chars[1] >= '0' && fg_chars[1] <= '9';
                bool third_c  = comma == ',';
                bool fourth_c = bg_chars[0] >= '0' && bg_chars[0] <= '9';
                bool fifth_c  = bg_chars[1] >= '0' && bg_chars[1] <= '9';
                bool color_code_is_valid = first_c && second_c && third_c && fourth_c && fifth_c;
                if (color_code_is_valid) {
                    i += 5;
                    int fg_irc_color = -1;
                    int bg_irc_color = -1;
                    sscanf(fg_chars, "%02d", &fg_irc_color);
                    sscanf(bg_chars, "%02d", &bg_irc_color);
                    if (fg_irc_color < 0 || fg_irc_color > 15) {
                        //printf("Error: invalid foreground color code: %d\n", fg_irc_color);
                        fg_irc_color = IRC_COLOR_GREY;
                        //return -5;
                    }
                    if (bg_irc_color < 0 || bg_irc_color > 15) {
                        //printf("Error: invalid background color code: %d\n", bg_irc_color);
                        bg_irc_color = IRC_COLOR_GREY;
                        //return -6;
                    }
                    // convert the irc color code to ncurses color code
                    current_fg_color = convert_to_ncurses_color(fg_irc_color);
                    current_bg_color = convert_to_ncurses_color(bg_irc_color);
                    
                    if (current_fg_color < 0 || current_fg_color > 15) {
                        printf("Error: invalid foreground color code: %d\n", current_fg_color);
                        return -7;
                    }
                    if (current_bg_color < 0 || current_bg_color > 15) {
                        printf("Error: invalid background color code: %d\n", current_bg_color);
                        return -8;
                    }
                    continue;
                }
                continue;
            }
            else if (c == 0xE2 && i <= strlen(buffer)-3) {
                unsigned char c2 = buffer[i+1];
                unsigned char c3 = buffer[i+2];
                wchar_t block;    
            
                if (c2 == 0x96 && c3== 0x80)       { block = UPPER_HALF_BLOCK; }
                else if (c2 == 0x96 && c3 == 0x84) { block = BOTTOM_HALF_BLOCK; }
                else if (c2 == 0x96 && c3 == 0x88) { block = FULL_BLOCK; }
                else if (c2 == 0x96 && c3 == 0x8C) { block = LEFT_HALF_BLOCK; }
                else if (c2 == 0x96 && c3 == 0x90) { block = RIGHT_HALF_BLOCK; }
                else if (c2 == 0x96 && c3 == 0x91) { block = LIGHT_SHADE; }
                else if (c2 == 0x96 && c3 == 0x92) { block = MEDIUM_SHADE; }
                else if (c2 == 0x96 && c3 == 0x93) { block = DARK_SHADE; }
                else if (c2 == 0x96 && c3 == 0x96) { block = BOTTOM_LEFT_BLOCK; }
                else if (c2 == 0x96 && c3 == 0x97) { block = BOTTOM_RIGHT_BLOCK; }
                else if (c2 == 0x96 && c3 == 0x98) { block = UPPER_LEFT_BLOCK; }
                else if (c2 == 0x96 && c3 == 0x99) { block = BOTTOM_LEFT_CORNER; }
                else if (c2 == 0x96 && c3 == 0x9B) { block = UPPER_LEFT_CORNER; }
                else if (c2 == 0x96 && c3 == 0x9C) { block = UPPER_RIGHT_CORNER; }
                else if (c2 == 0x96 && c3 == 0x9D) { block = UPPER_RIGHT_BLOCK; }
                else if (c2 == 0x96 && c3 == 0x9F) { block = BOTTOM_RIGHT_CORNER; }

                i += 2;
                canvas[canvas_y][canvas_x].foreground_color = current_fg_color;
                canvas[canvas_y][canvas_x].background_color = current_bg_color;
                canvas[canvas_y][canvas_x].character = block;
                canvas_x++;
                w++;
            }
            else if (c == '\n') {
                canvas_y++;
                canvas_x = 0;
            }
            else {
                // write the block to the canvas
                canvas[canvas_y][canvas_x].foreground_color = current_fg_color;
                canvas[canvas_y][canvas_x].background_color = current_bg_color;
                canvas[canvas_y][canvas_x].character = c;
                canvas_x++;
                w++;
            }
        }
    }
    mPrint("done");

    //print_color_palette();
    return 0;
}


bool iscomma(char c) {
    return c == ',';
}


void get_colors_in_line(char *s) {
    mPrint("get_colors_in_line()");
    if (s==NULL) {
        return;
    }
    int len_s = -1;
    int fg = -1;
    int bg = -1;
    unsigned char c0 = 0;
    unsigned char c1 = 0;
    unsigned char c2 = 0;
    unsigned char c3 = 0;
    unsigned char c4 = 0;
    unsigned char c5 = 0;
    char fg_color_code_str[3] = {0, 0, 0};
    char bg_color_code_str[3] = {0, 0, 0};
    len_s = strlen(s);
    for (int i=0; i<len_s; i++) {
        c0 = s[i];
        if (c0 == 0x03) {
            // look ahead at the next character
            // if it is not 0 thru 9, fail
            if (i+1 >= len_s) {
                fprintf(stderr, "Error: invalid color code\n");
                exit(EXIT_FAILURE);
            }
            c1 = s[i+1];
            if (!isdigit(c1)) {
                fprintf(stderr, "Error: invalid color code\n");
                exit(EXIT_FAILURE);
            }
            if (i+2 >= len_s) {
                fprintf(stderr, "Error: invalid color code\n");
                exit(EXIT_FAILURE);
            }
            // look ahead at the next character
            c2 = s[i+2];
            if (isdigit(c2)) {
                c3 = s[i+3];
                if (iscomma(c3)) {
                    // expecting another color code either 1 or 2 digit
                    c4 = s[i+4];
                    if (isdigit(c4)) {
                        // check for one more digit in bg color code
                        c5 = s[i+5];
                        if (isdigit(c5)) {
                            // double-digit fg color code
                            // double-digit bg color code
                            fg_color_code_str[0] = c1;
                            fg_color_code_str[1] = c2;
                            fg_color_code_str[2] = '\0';
                            bg_color_code_str[0] = c4;
                            bg_color_code_str[1] = c5;
                            bg_color_code_str[2] = '\0';
                            fg = atoi(fg_color_code_str);
                            bg = atoi(bg_color_code_str);
                            g_color_palette[fg][bg] = 1;
                        }
                        else {
                            // double-digit fg color code
                            // single-digit bg color code
                            fg_color_code_str[0] = c1;
                            fg_color_code_str[1] = c2;
                            fg_color_code_str[2] = '\0';
                            bg_color_code_str[0] = c4;
                            bg_color_code_str[1] = '\0';
                            fg = atoi(fg_color_code_str);
                            bg = atoi(bg_color_code_str);
                            g_color_palette[fg][bg] = 1;
                        }
                    }
                    else {
                        fprintf(stderr, "Error: invalid color code\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            // single-digit fg color code
            else if (iscomma(c2)) {
                // expecting another color code either 1 or 2 digit
                c3 = s[i+3];
                if (isdigit(c3)) {
                    // check for one more digit in bg color code
                    c4 = s[i+4];
                    if (isdigit(c4)) {
                        // single-digit fg color code
                        // double-digit bg color code
                        fg_color_code_str[0] = c1;
                        fg_color_code_str[1] = '\0';
                        bg_color_code_str[0] = c3;
                        bg_color_code_str[1] = c4;
                        bg_color_code_str[2] = '\0';
                        fg = atoi(fg_color_code_str);
                        bg = atoi(bg_color_code_str);
                        g_color_palette[fg][bg] = 1;
                    }
                    else {
                        // single-digit fg color code
                        // single-digit bg color code
                        fg_color_code_str[0] = c1;
                        fg_color_code_str[1] = '\0';
                        bg_color_code_str[0] = c3;
                        bg_color_code_str[1] = '\0';
                        fg = atoi(fg_color_code_str);
                        bg = atoi(bg_color_code_str);
                        g_color_palette[fg][bg] = 1;
                    }
                }
                else {
                    fprintf(stderr, "Error: invalid color code\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}



// count the number of unique color combinations
unsigned int count_color_palette() {
    unsigned int count = 0;
    for (int i=0; i<100; i++) {
        for (int j=0; j<100; j++) {
            if (g_color_palette[i][j] == 1) {
                count++;
            }
        }
    }
    return count;
}



void print_color_palette() {
    for (int i=0; i<100; i++) {
        for (int j=0; j<100; j++) {
            if (g_color_palette[i][j] == 1) {
                printf("%d,%d\n", i, j);
            }
        }
    }
}



void get_ascii_width_height_from_file(FILE *fp, int *h, int *w) {
    int max_width = 0;
    int max_height = 0;
    char buffer[1024] = {0};
    //unsigned char buffer[1024] = {0};
    
    if (fp == NULL) {
        mPrint("Error: fp is NULL\n");
        exit(EXIT_FAILURE);
    }
    else if (w == NULL) {
        mPrint("Error: w is NULL\n");
        exit(EXIT_FAILURE);
    }
    else if (h == NULL) {
        mPrint("Error: h is NULL\n");
        exit(EXIT_FAILURE);
    }
    // reset the file pointer to the beginning of the file
    fseek(fp, 0, SEEK_SET);

    while (fgets(buffer, 1024, fp) != NULL) {
        int width_for_this_line = 0;
        char *b = buffer;
        for(size_t i=0; i<strlen(buffer); i++) {
            //unsigned char c = b[i];   
            // color-codes are preceded by 0x03 and do not count towards width total
            // this currently does not account for other possible color code combos:
            // 0x03 followed by 1 or 2 digits
            // 0x03 followed by a comma and 1 or 2 digits
            // 0x03 followed by 1 or 2 digits, followed by another comma and 1 or 2 digits
            bool next_three   = i    <= strlen(b)-3;
            bool next_four    = i    <= strlen(b)-4;
            bool next_five    = i    <= strlen(b)-5;
            bool bi_is_ctrl_c = (unsigned char)b[i] == 0x03;
            bool bi_is_e2     = (unsigned char)b[i] == 0xE2;
            // combining clauses to prevent overrun and improve visual aesthetics
            bool bi1_is_96              = bi_is_e2               && next_three      && (unsigned char)b[i+1] == 0x96;
            bool bi1_isdigit_next_five  = bi_is_ctrl_c           && next_five       && isdigit(b[i+1]);
            bool bi1_isdigit_next_three = bi_is_ctrl_c           && next_three      && isdigit(b[i+1]);
            bool color_check_0          = bi1_isdigit_next_five  && isdigit(b[i+2]) && iscomma(b[i+3]) && isdigit(b[i+4]) && isdigit(b[i+5]);
            bool color_check_1          = bi_is_ctrl_c           && next_four       && iscomma(b[i+1]) && isdigit(b[i+2]) && isdigit(b[i+3]);
            bool color_check_2          = bi1_isdigit_next_three && isdigit(b[i+2]);

            if (color_check_0) {
                i += 5;// skip the next 5 chars
            }
            else if (color_check_1) {
                i += 3;// skip the next 3 chars
            }
            else if (color_check_2) {
                i += 2;// skip the next 2 chars
            }
            // handle blocks, half-blocks, etc
            // top-half block
            else if (bi1_is_96 && (unsigned char)b[i+2] == 0x80) {
                i += 2;
                width_for_this_line++;
            }
            // bottom-half block
            else if (bi1_is_96 && (unsigned char)b[i+2] == 0x84) {
                i += 2;
                width_for_this_line++;
            }
            // full block
            else if (bi1_is_96 && (unsigned char)b[i+2] == 0x88) {
                i += 2;
                width_for_this_line++;
            }
            // light shading
            else if (bi1_is_96 && (unsigned char)b[i+2] == 0x91) {
                i += 2;
                width_for_this_line++;
            }
            // skip newlines
            else if (b[i] == '\n') {
                continue;
            }
            else {
                width_for_this_line++;       
            }
        }

        if (width_for_this_line > max_width) {
            printf("max width increased from %d to %d\n", max_width, width_for_this_line);
            max_width = width_for_this_line;
        }
        max_height++;
    }
    *w = max_width;
    *h = max_height;
}


canvas_pixel_t ** read_ascii_from_filepath(char *path, int *height, int *width) {
    if (path == NULL) {
        printf("Error: path is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (height == NULL) {
        printf("Error: height is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (width == NULL) {
        printf("Error: width is NULL\n");
        exit(EXIT_FAILURE);
    }
    FILE *fp = fopen(path, "r");
    int w = -1;
    int h = -1;
    if (fp == NULL) {
        printf("Error: cannot open file %s\n", path);
        exit(EXIT_FAILURE);
    }

    get_ascii_width_height_from_file(fp, &h, &w);
    
    printf("ascii width: %d\n", w);
    printf("ascii height: %d\n", h);
    
    canvas_pixel_t **canvas = init_canvas(h, w);
    
    int result = read_ascii_into_canvas_unsafe(fp, canvas, h, w);
    fclose(fp);

    if (result != 0) {
        mPrint("Error: read_ascii_into_canvas_unsafe returned non-zero");
        printf("Result: %d\n", result);
        exit(EXIT_FAILURE);
    }

    *height = h;
    *width = w;
    return canvas;  
}


bool check_if_file_exists(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return false;
    }
    fclose(fp);
    return true;
}


int get_fg_color(int **color_array, int array_len, int color_pair) {
    if (color_array == NULL) {
        mPrint("color_array is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (color_pair >= array_len) {
        mPrint("color_pair is out of bounds\n");
        exit(EXIT_FAILURE);
    }

    return color_array[color_pair][0];
}


int get_bg_color(int **color_array, int array_len, int color_pair) {
    if (color_array == NULL) {
        mPrint("color_array is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (color_pair >= array_len) {
        mPrint("color_pair is out of bounds\n");
        exit(EXIT_FAILURE);
    }

    return color_array[color_pair][1];
}

