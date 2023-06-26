


#include <stdio.h>
#include <stdlib.h>
#include "tools.h"

int main() 
{
    //char *filename = "small.ascii";
    read_ascii_from_filepath("small.ascii");
    read_ascii_from_filepath("test.ascii");

    return EXIT_SUCCESS;
}
