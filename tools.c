
#include "tools.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <wchar.h>
#include "mPrint.h"


void read_ascii_into_canvas(FILE *fp, canvas_pixel_t **canvas, int canvas_height, int canvas_width)
{
    if (fp == NULL)
    {
        printf("Error: fp is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (canvas == NULL)
    {
        printf("Error: canvas is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (canvas_height <= 0)
    {
        printf("Error: canvas_height is <= 0\n");
        exit(EXIT_FAILURE);
    }

    if (canvas_width <= 0)
    {
        printf("Error: canvas_width is <= 0\n");
        exit(EXIT_FAILURE);
    }

    // reset the file pointer to the beginning of the file
    fseek(fp, 0, SEEK_SET);

    int max_w = -1;
    int h = 0;


#define DEFAULT_FG_COLOR 7
#define DEFAULT_BG_COLOR 0
    //int current_fg_color = DEFAULT_FG_COLOR;
    //int current_bg_color = DEFAULT_BG_COLOR;

    char buffer[1024] = {0};
    while (fgets(buffer, 1024, fp) != NULL)
    {
        h++;
        int w = 0;
        for (int i = 0; i < strlen(buffer); i++)
        {
            char c = buffer[i];

            if (c == 0x03) 
            {
                // we will have to read color codes to skip past them when computing width

                // we can expect after a ctrl char that the color code is of format:
                // 00,00
                // where the first 00 is the foreground color and the second 00 is the background color

                char fg_chars[3] = {
                    buffer[i+1],
                    buffer[i+2],
                    0
                };

                char bg_chars[3] = {
                    buffer[i+4],
                    buffer[i+5],
                    0
                };

                char comma = buffer[i+3]; // ,

                // the state machine for colors is a bit more complicated than this...
                if (fg_chars[0] >= '0' && fg_chars[0] <= '9' && 
                    fg_chars[1] >= '0' && fg_chars[1] <= '9' && 
                    comma == ',' && 
                    bg_chars[0] >= '0' && bg_chars[0] <= '9' && 
                    bg_chars[1] >= '0' && bg_chars[1] <= '9')
                {
                    i += 5;

                    int fg_irc_color = -1;
                    int bg_irc_color = -1;

                    sscanf(fg_chars, "%d", &fg_irc_color);
                    sscanf(bg_chars, "%d", &bg_irc_color);

                    if (fg_irc_color < 0 || fg_irc_color > 15)
                    {
                        printf("Error: invalid foreground color code: %d\n", fg_irc_color);
                        exit(EXIT_FAILURE);
                    }

                    if (bg_irc_color < 0 || bg_irc_color > 15)
                    {
                        printf("Error: invalid background color code: %d\n", bg_irc_color);
                        exit(EXIT_FAILURE);
                    }

                    // convert the irc color code to ncurses color code
                    //current_fg_color = convert_to_ncurses_color(fg_irc_color);
                    //current_bg_color = convert_to_ncurses_color(bg_irc_color);

                    continue;
                }

                continue;
            }


            if (c == 0xFFFFFFE2 && i <= strlen(buffer)-3)
            {
                char c2 = buffer[i+1];
                char c3 = buffer[i+2];

                if (c2 == 0xFFFFFF96 && c3 == 0xFFFFFF88)
                {
                    putchar(c);
                    putchar(c2);
                    putchar(c3);
                    i += 2;

                    w++;
                }
            }
            else 
            {
                putchar(c);
                w++;
            }
        }

        if (w > max_w)
        {
            max_w = w;
        }
    }

    printf("w: %d\n", max_w);
    printf("h: %d\n", h);
}




void get_ascii_width_height_from_file(FILE *fp, int *w, int *h) 
{
    if (fp == NULL)
    {
        mPrint("Error: fp is NULL\n");
        exit(EXIT_FAILURE);
    }
    else if (w == NULL)
    {
        mPrint("Error: w is NULL\n");
        exit(EXIT_FAILURE);
    }
    else if (h == NULL)
    {
        mPrint("Error: h is NULL\n");
        exit(EXIT_FAILURE);
    }

    // reset the file pointer to the beginning of the file
    fseek(fp, 0, SEEK_SET);

    int max_width = 0;
    int max_height = 0;
    char buffer[1024] = {0};

    while (fgets(buffer, 1024, fp) != NULL)
    {
        int width = strlen(buffer);
        if (width > max_width)
        {
            max_width = width;
        }
        max_height++;
    }

    // this does not account for control-characters, wide-characters, or color-codes
    // however, 
    *w = max_width;
    *h = max_height;
}


void read_ascii_from_filepath(char *path)
{
    if (path == NULL)
    {
        printf("Error: path is NULL\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(path, "r");
    int w = -1;
    int h = -1;

    if (fp == NULL)
    {
        printf("Error: cannot open file %s\n", path);
        exit(EXIT_FAILURE);
    }

    get_ascii_width_height_from_file(fp, &w, &h);

    printf("w: %d\n", w);
    printf("h: %d\n", h);

    canvas_pixel_t **canvas = init_canvas(w, h);

    read_ascii_into_canvas(fp, canvas, w, h);

    fclose(fp);

    free_canvas(canvas, h);
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

        // attempting to handle colors 8-15
        //case COLOR_BRIGHT_BLACK:   return 14;
        //case COLOR_BRIGHT_BLUE:    return 12;
        //case COLOR_BRIGHT_GREEN:   return 9;
        //case COLOR_BRIGHT_CYAN:    return 11;
        //case COLOR_BRIGHT_RED:     return 5;
        //case COLOR_BRIGHT_MAGENTA: return 6;
        //case COLOR_BRIGHT_YELLOW:  return 8;
        //case COLOR_BRIGHT_WHITE:   return 15;

        default:            return color;
    }
    return 0;
}




int convert_to_ncurses_color(int irc_color)
{
    // this is the inverse-function of convert_to_irc_color

    switch (irc_color) 
    {
        case 1:  return COLOR_BLACK;
        case 4:  return COLOR_RED;
        case 3:  return COLOR_GREEN;
        case 8:  return COLOR_YELLOW;
        case 2:  return COLOR_BLUE;
        case 6:  return COLOR_MAGENTA;
        case 10: return COLOR_CYAN;
        case 0:  return COLOR_WHITE;

        // attempting to handle colors 8-15
        //case 14: return COLOR_BRIGHT_BLACK;
        //case 12: return COLOR_BRIGHT_BLUE;
        //case 9:  return COLOR_BRIGHT_GREEN;
        //case 11: return COLOR_BRIGHT_CYAN;
        //case 5:  return COLOR_BRIGHT_RED;
        //case 6:  return COLOR_BRIGHT_MAGENTA;
        //case 8:  return COLOR_BRIGHT_YELLOW;
        //case 15: return COLOR_BRIGHT_WHITE;

        default: return irc_color;
    }
    return 0;
}



