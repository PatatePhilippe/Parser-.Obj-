//
// Created by pat on 14/10/2026.
//

#include "fileReader.h"

#include <assert.h>


void read3dFile() {
    // Directly use "text.txt" as the filename since command-line args
    // are not directly supported by %%cuda for argv
    unsigned int log2size, size;
    unsigned int *vec;
    FILE *f = fopen("gf.obj", "r"); // Hardcoding filename
    if (f == NULL) {
        printf("Error: Could not open text.txt\n");
        exit(-1);
    }
    unsigned int bytes = 100 * sizeof(unsigned int);
    vec = (unsigned int *) malloc(bytes);
    assert(vec);
    char buffer[100];
    fclose(f);
    return;
}