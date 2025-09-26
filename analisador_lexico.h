#ifndef ANALISADOR_LEXICO_H
#define ANALISADOR_LEXICO_H

// Gustavo Nascimento Siqueira - 10419057
// Felipe Ujvari Gasparino de Sousa - 10418415

#include <stdio.h>

// Enumeração dos tipos de átomos/tokens
typedef enum {
    // Palavras reservadas
    ATOM_AND,
    ATOM_BEGIN,
    ATOM_CHAR,
    ATOM_ELSE,
    ATOM_END,
    ATOM_FLOAT,
    ATOM_FOR,
    ATOM_IF,
    ATOM_INT,
    ATOM_NOT,
    ATOM_OR,
    ATOM_PRG,
    ATOM_READ,
    ATOM_REPEAT,
    ATOM_RETURN,
    ATOM_SUBROT,
    ATOM_THEN,
    ATOM_UNTIL,
    ATOM_VAR,
    ATOM_VOID,
    ATOM_WHILE,
    ATOM_WRITE,
    
    // Identificador
    ATOM_IDENTIFICADOR,
    
    // Literais
    ATOM_NUMERO_INT,
    ATOM_NUMERO_FLOAT,
    ATOM_CHAR_CONST,
    ATOM_STRING,
    
    // Operadores
    ATOM_ATRIBUICAO,        // <-
    ATOM_MAIS,              // +
    ATOM_MENOS,             // -
    ATOM_MULT,              // *
    ATOM_DIV,               // /
    ATOM_IGUAL,             // ==
    ATOM_DIFERENTE,         // !=
    ATOM_MAIOR,             // >
    ATOM_MENOR,             // <
    ATOM_MAIOR_IGUAL,       // >=
    ATOM_MENOR_IGUAL,       // <=
    
    // Delimitadores e pontuação
    ATOM_PONTO_VIRGULA,     // ;
    ATOM_PONTO,             // .
    ATOM_VIRGULA,           // ,
    ATOM_ABRE_PAR,          // (
    ATOM_FECHA_PAR,         // )
    ATOM_ABRE_COL,          // [
    ATOM_FECHA_COL,         // ]
    
    // Especiais
    ATOM_EOF,               // Fim de arquivo
    ATOM_ERRO               // Erro léxico
} TipoAtomo;

// Union para armazenar diferentes tipos de valores
typedef union {
    char string[256];       // Para identificadores, strings e caracteres
    int valor_int;          // Para números inteiros
    float valor_float;      // Para números reais
} ValorLexema;

// Estrutura para informações do átomo
typedef struct {
    TipoAtomo tipo;         // Tipo do átomo
    ValorLexema lexema;     // Valor do lexema
    int linha;              // Número da linha onde foi encontrado
} TInfoAtomo;

// Funções públicas
void inicializar_analisador(FILE *arquivo);
TInfoAtomo obter_atomo(void);
void finalizar_analisador(void);
const char* nome_atomo(TipoAtomo tipo);

#endif