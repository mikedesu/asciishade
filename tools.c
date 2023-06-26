
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <wchar.h>
#include "mPrint.h"

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
    if (fp == NULL)
    {
        printf("Error: cannot open file %s\n", path);
        exit(EXIT_FAILURE);
    }

    int w = -1;
    int h = -1;
    get_ascii_width_height_from_file(fp, &w, &h);

    printf("w: %d\n", w);
    printf("h: %d\n", h);

    fclose(fp);

    //int w = max_characters_per_line;
    //int h = lines;
    //printf("\n");
    //printf("%02X \n", c);
    //printf("%02X \n", WEOF);
    //printf("sizeof(char):    %lu \n", sizeof(char));
    //printf("sizeof(int):     %lu \n", sizeof(int));
    //printf("sizeof(wchar_t): %lu \n", sizeof(int));
    //printf("\n-----\n");
    //printf("%dx%d\n", h, w);
}
