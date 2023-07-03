#include "tools.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mPrint.h"
#include "colors.h"


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
            printf("%c", c);
        }
        printf("\n");
    }
}


void read_ascii_into_canvas(FILE *fp, canvas_pixel_t **canvas, int canvas_height, int canvas_width) {
    if (fp == NULL) {
        printf("Error: fp is NULL\n");
        exit(EXIT_FAILURE);
    }
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
    // reset the file pointer to the beginning of the file
    fseek(fp, 0, SEEK_SET);
    int max_w = -1;
    int h = 0;
    int canvas_y = 0;
    int canvas_x = 0;
    int current_fg_color = DEFAULT_FG_COLOR;
    int current_bg_color = DEFAULT_BG_COLOR;
    char buffer[1024] = {0};
    while (fgets(buffer, 1024, fp) != NULL) {
        h++;
        int w = 0;
        for (int i = 0; i < strlen(buffer); i++) {
            char c = buffer[i];
            if (c == 0x03) {
                // we will have to read color codes to skip past them when computing width
                // we can expect after a ctrl char that the color code is of format:
                // 00,00
                // where the first 00 is the foreground color and the second 00 is the background color
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
                        printf("Error: invalid foreground color code: %d\n", fg_irc_color);
                        exit(EXIT_FAILURE);
                    }
                    if (bg_irc_color < 0 || bg_irc_color > 15) {
                        printf("Error: invalid background color code: %d\n", bg_irc_color);
                        exit(EXIT_FAILURE);
                    }
                    // convert the irc color code to ncurses color code
                    current_fg_color = convert_to_ncurses_color(fg_irc_color);
                    current_bg_color = convert_to_ncurses_color(bg_irc_color);
                    if (current_fg_color < 0 || current_fg_color > 15) {
                        printf("Error: invalid foreground color code: %d\n", current_fg_color);
                        exit(EXIT_FAILURE);
                    }
                    if (current_bg_color < 0 || current_bg_color > 15) {
                        printf("Error: invalid background color code: %d\n", current_bg_color);
                        exit(EXIT_FAILURE);
                    }
                    continue;
                }
                continue;
            }
            else if (c == 0xFFFFFFE2 && i <= strlen(buffer)-3) {
                char c2 = buffer[i+1];
                char c3 = buffer[i+2];
                if (c2 == 0xFFFFFF96 && c3 == 0xFFFFFF88) {
                    putchar(c);
                    putchar(c2);
                    putchar(c3);
                    i += 2;
                    // write the block to the canvas
                    canvas[canvas_y][canvas_x].foreground_color = current_fg_color;
                    canvas[canvas_y][canvas_x].background_color = current_bg_color;
                    canvas[canvas_y][canvas_x].character = L'█';
                    canvas_x++;
                    w++;
                }
            }
            else if (c == '\n') {
                putchar(c);
                canvas_y++;
                canvas_x = 0;
            }
            else {
                putchar(c);
                // write the block to the canvas
                canvas[canvas_y][canvas_x].foreground_color = current_fg_color;
                canvas[canvas_y][canvas_x].background_color = current_bg_color;
                canvas[canvas_y][canvas_x].character = c;
                canvas_x++;
                w++;
            }
        }
        if (w > max_w) {
            max_w = w;
        }
    }
    //printf("w: %d\n", max_w);
    //printf("h: %d\n", h);
}


void get_ascii_width_height_from_file(FILE *fp, int *h, int *w) {
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
    int max_width = 0;
    int max_height = 0;
    char buffer[1024] = {0};
    while (fgets(buffer, 1024, fp) != NULL) {
        int width = strlen(buffer);
        if (width > max_width) {
            max_width = width;
        }
        max_height++;
    }
    // this does not account for control-characters, wide-characters, or color-codes
    // however, 
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
    //printf("w: %d\n", w);
    //printf("h: %d\n", h);
    canvas_pixel_t **canvas = init_canvas(h, w);
    read_ascii_into_canvas(fp, canvas, h, w);
    *height = h;
    *width = w;
    fclose(fp);
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

