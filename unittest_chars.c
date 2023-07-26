#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
    char bytes[3] = {
        'c',
        0x00,
        0x00,
    };

    printf("[%s]\n", bytes);
    
    bytes[0] = 0xE2;
    bytes[1] = 0x96;
    bytes[2] = 0x88;

    printf("\u2588\n");
    printf("%s\n", bytes);

    /*
    char buffer[1024] = {0};
    FILE *infile = fopen("zalgo.txt", "r");
    fread(buffer, 1, 1024, infile);
    fclose(infile);
    buffer[strlen(buffer) - 1] = 0;
    printf("%c\n", buffer[0]);
    //for (int j = 0; j < 10; j++) {
        for (int i = 1; i < strlen(buffer); i++) {
            unsigned char c = buffer[i];
            if (!c) break;
            //printf("%c", c);
            printf("0x%02x\n", c);
        }
    //}
    //printf("%s", buffer);
    printf("\n");
    */
    return 0;
}
