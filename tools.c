
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <wchar.h>
#include "mPrint.h"



void read_ascii_by_byte(FILE *fp)
{
    if (fp == NULL)
    {
        printf("Error: fp is NULL\n");
        exit(EXIT_FAILURE);
    }

    // reset the file pointer to the beginning of the file
    fseek(fp, 0, SEEK_SET);

    int max_w = -1;
    int h = 0;

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

    read_ascii_by_byte(fp);

    fclose(fp);
}
