#pragma once

void read_ascii_by_byte(FILE *fp);
void get_ascii_width_height_from_file(FILE *fp, int *w, int *h);
int read_ascii_from_filepath(char *path);
