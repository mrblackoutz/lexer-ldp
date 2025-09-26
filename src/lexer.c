#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

// Helper function to convert token type to string
const char* lexer_token_type_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_OPERATOR: return "OPERATOR";
        case TOKEN_KEYWORD: return "KEYWORD";
        case TOKEN_UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

// Create a new token
Token* lexer_create_token(TokenType type, const char *value, int line, int column) {
    Token *token = malloc(sizeof(Token));
    if (!token) return NULL;
    
    token->type = type;
    token->line = line;
    token->column = column;
    
    if (value) {
        token->value = malloc(strlen(value) + 1);
        if (token->value) {
            strcpy(token->value, value);
        }
    } else {
        token->value = NULL;
    }
    
    return token;
}

// Free a token
void lexer_free_token(Token *token) {
    if (token) {
        if (token->value) {
            free(token->value);
        }
        free(token);
    }
}

// Basic lexer function (placeholder implementation)
Token* lexer_next_token(const char *input, int *position) {
    // This is a basic placeholder implementation
    // A real lexer would parse the input character by character
    
    if (!input || !position || input[*position] == '\0') {
        return lexer_create_token(TOKEN_EOF, NULL, 1, *position);
    }
    
    // Skip whitespace
    while (input[*position] == ' ' || input[*position] == '\t' || 
           input[*position] == '\n' || input[*position] == '\r') {
        (*position)++;
    }
    
    if (input[*position] == '\0') {
        return lexer_create_token(TOKEN_EOF, NULL, 1, *position);
    }
    
    // Simple token recognition (placeholder)
    char token_value[2] = {input[*position], '\0'};
    (*position)++;
    
    return lexer_create_token(TOKEN_UNKNOWN, token_value, 1, *position - 1);
}