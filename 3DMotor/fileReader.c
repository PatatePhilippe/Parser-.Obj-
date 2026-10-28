//
// Created by pat on 14/10/2026.
//

#include "fileReader.h"
#include <assert.h>
#include <string.h>



unsigned int cptLine(){
    FILE *f = fopen("../ressources/gf.obj", "r"); // Hardcoding filename
    if (f == NULL) {
        printf("Error: Could not open text.txt\n");
        exit(-1);
    }
    char myString[100];
    unsigned int cpt=0;
    while(fgets(myString, 100, f)) {
        cpt+=strlen(myString);
    }
    return cpt;
}

char* read3dFile() {

    unsigned int size=cptLine();
    char *vec=malloc(size*sizeof(char*)) ;
    printf("taille %d \n",size);
    FILE *f = fopen("../ressources/gf.obj", "r"); // Hardcoding filename
    if (f == NULL) {
        printf("Error: Could not open text.txt\n");
        exit(-1);
    }
    char myString[100];
    unsigned int cpt=0;
    while(fgets(myString, 100, f)) {
        strcpy(vec+cpt, myString);
        cpt+=strlen(myString);
    }
    fclose(f);
    return vec;
}