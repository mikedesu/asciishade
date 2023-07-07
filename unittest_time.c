#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main() {
    // get milliseconds
    struct timespec ts0, ts1;
    clock_gettime(CLOCK_MONOTONIC, &ts0);
    //printf("time: %ld\n", ts0.tv_sec);
    //printf("time: %ld\n", ts0.tv_nsec);
    for (int i = 0; i < 100000000; i++) {
        // do nothing
    }
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    //printf("time: %ld\n", ts1.tv_sec);
    //printf("time: %ld\n", ts1.tv_nsec);
    // get the difference in milliseconds
    long diff = (ts1.tv_sec - ts0.tv_sec) * 1000 + (ts1.tv_nsec - ts0.tv_nsec) / 1000000;
    printf("diff: %ld ms\n", diff);
    return EXIT_SUCCESS;
}
