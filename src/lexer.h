#ifndef LEXER_H
#define LEXER_H

// Version information
#define LEXER_VERSION_MAJOR 1
#define LEXER_VERSION_MINOR 0
#define LEXER_VERSION_PATCH 0

// Token types for the lexer
typedef enum {
    TOKEN_EOF = 0,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_KEYWORD,
    TOKEN_UNKNOWN
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char *value;
    int line;
    int column;
} Token;

// Function declarations
Token* lexer_next_token(const char *input, int *position);
void lexer_free_token(Token *token);
const char* lexer_token_type_string(TokenType type);

#endif // LEXER_H