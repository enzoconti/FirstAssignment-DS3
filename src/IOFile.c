#include "../include/IOFile.h"

// this is a simple function to input a filepath into a dynamically allocated string and reallocate it not to waste memory
char* inputFilepath(){
    char* outFilepath;
    outFilepath = malloc(100 * sizeof(char));
    scanf("%s", outFilepath);
    outFilepath = realloc(outFilepath, (strlen(outFilepath) + 1 ) * sizeof(char) );

    return outFilepath;
}

void printOpenError(){
    printf("Falha no processamento do arquivo\n");
}