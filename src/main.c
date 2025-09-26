#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main(int argc, char *argv[]) {
    printf("Lexer LDP - Basic C Project\n");
    printf("Version 1.0\n");
    
    if (argc > 1) {
        printf("Input file: %s\n", argv[1]);
        // TODO: Implement lexer functionality
    } else {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    return 0;
}