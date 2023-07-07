#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <assert.h>
#include <stdlib.h>

int main() {

    char *str = "█";

    wchar_t *wstr = L"█";

    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = str[i];   
        //printf("%x\n", (unsigned char)str[i]);
        printf("%x\n", c);
        //if (c == 0xe2) {
        //    printf("nice\n");
        //}

    }

    for (int i = 0; wstr[i] != '\0'; i++) {
        printf("%x\n", wstr[i]);
    }

    printf("-----\n");
    printf("%s\n", str);
    printf("-----\n");

    wprintf(wstr);
    printf("-----\n");



    return EXIT_SUCCESS;
}

