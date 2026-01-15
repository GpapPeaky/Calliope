#include <stdio.h>

#define EXIT_SUCCESS 1

// 4.6 55.4

/**
 * Main entry
 */
int main(int argc, char** argv) {

    // Entry
    for (int i = 0 ; i < argc ; i++) {
        printf("Hello, %s!\n", argv[i]);
     }
    
    return EXIT_SUCCESS;
}