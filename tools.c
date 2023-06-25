
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <wchar.h>

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


    char c;
    int characters_per_line = 0;
    int max_characters_per_line = -1;
    int lines = 0;

    while ((c = fgetc(fp)) != EOF)
    {
        printf("%c", c);
        characters_per_line++;
        // a color code follows in form: 00,00
        // single-digit numbers may or may not have a leading 0
        // such as 01 or 1
        //    int fgcolor = -1;
        //    int bgcolor = -1;
        //    fscanf(fp, "%d,%d", &fgcolor, &bgcolor);
        //    printf("%d,%d", fgcolor, bgcolor);
        if (c == '\x03')
        {
            characters_per_line--;
        }   
        else if (c == '\n')
        {
            lines++;
            if (characters_per_line > max_characters_per_line)
            {
                max_characters_per_line = characters_per_line;
            }
            characters_per_line = 0;
        }
    }

    fclose(fp);

    int w = max_characters_per_line;
    int h = lines;

    printf("-----\n");
    printf("%dx%d\n", h, w);
}
