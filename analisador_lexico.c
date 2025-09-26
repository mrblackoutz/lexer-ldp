#include "analisador_lexico.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Gustavo Nascimento Siqueira - 10419057
// Felipe Ujvari Gasparino de Sousa - 10418415

// Variáveis globais para controle do analisador
static FILE *arquivo_fonte = NULL;
static int linha_atual = 1;
static int coluna_atual = 0;
static char char_atual = '\0';
static bool fim_arquivo = false;

// Estrutura para mapear palavras reservadas
typedef struct {
    const char *palavra;
    TipoAtomo tipo;
} PalavraReservada;

// Tabela de palavras reservadas
static PalavraReservada palavras_reservadas[] = {
    {"and", ATOM_AND},
    {"begin", ATOM_BEGIN},
    {"char", ATOM_CHAR},
    {"else", ATOM_ELSE},
    {"end", ATOM_END},
    {"float", ATOM_FLOAT},
    {"for", ATOM_FOR},
    {"if", ATOM_IF},
    {"int", ATOM_INT},
    {"not", ATOM_NOT},
    {"or", ATOM_OR},
    {"prg", ATOM_PRG},
    {"read", ATOM_READ},
    {"repeat", ATOM_REPEAT},
    {"return", ATOM_RETURN},
    {"subrot", ATOM_SUBROT},
    {"then", ATOM_THEN},
    {"until", ATOM_UNTIL},
    {"var", ATOM_VAR},
    {"void", ATOM_VOID},
    {"while", ATOM_WHILE},
    {"write", ATOM_WRITE},
    {NULL, 0}
};

// Função auxiliar para ler próximo caractere
static void proximo_char(void) {
    if (fim_arquivo) {
        return;
    }
    
    int c = fgetc(arquivo_fonte);
    if (c == EOF) {
        fim_arquivo = true;
        char_atual = '\0';
    } else {
        char_atual = (char)c;
        coluna_atual++;
        
        if (char_atual == '\n') {
            linha_atual++;
            coluna_atual = 0;
        }
    }
}

// Função auxiliar para retroceder um caractere
static void retroceder_char(void) {
    if (!fim_arquivo && char_atual != '\0') {
        ungetc(char_atual, arquivo_fonte);
        coluna_atual--;
        if (char_atual == '\n') {
            linha_atual--;
        }
    }
}

// Função para pular espaços em branco
static void pular_espacos(void) {
    while (!fim_arquivo && isspace(char_atual)) {
        proximo_char();
    }
}

// Função para pular comentários
static bool pular_comentario(void) {
    if (char_atual == '{') {
        proximo_char();
        while (!fim_arquivo && char_atual != '}') {
            proximo_char();
        }
        if (fim_arquivo) {
            return false; // Erro: comentário não fechado
        }
        proximo_char(); // Pula o '}'
        return true;
    }
    return false;
}

// Função para verificar se é palavra reservada
static TipoAtomo verificar_palavra_reservada(const char *lexema) {
    for (int i = 0; palavras_reservadas[i].palavra != NULL; i++) {
        if (strcmp(lexema, palavras_reservadas[i].palavra) == 0) {
            return palavras_reservadas[i].tipo;
        }
    }
    return ATOM_IDENTIFICADOR;
}

// Função para ler identificador ou palavra reservada
static TInfoAtomo ler_identificador(void) {
    TInfoAtomo info;
    char buffer[256];
    int pos = 0;
    
    info.linha = linha_atual;
    
    // Primeiro caractere deve ser letra
    if (!isalpha(char_atual) && char_atual != '_') {
        info.tipo = ATOM_ERRO;
        strcpy(info.lexema.string, "Identificador inválido");
        return info;
    }
    
    // Lê o identificador
    while (!fim_arquivo && (isalnum(char_atual) || char_atual == '_')) {
        if (pos < 255) {
            buffer[pos++] = char_atual;
        }
        proximo_char();
    }
    buffer[pos] = '\0';
    
    // Verifica se é palavra reservada
    info.tipo = verificar_palavra_reservada(buffer);
    strcpy(info.lexema.string, buffer);
    
    return info;
}

// Função para ler número (inteiro ou float)
static TInfoAtomo ler_numero(void) {
    TInfoAtomo info;
    char buffer[256];
    int pos = 0;
    bool tem_ponto = false;
    
    info.linha = linha_atual;
    
    // Lê a parte inteira
    while (!fim_arquivo && isdigit(char_atual)) {
        if (pos < 255) {
            buffer[pos++] = char_atual;
        }
        proximo_char();
    }
    
    // Verifica se tem ponto decimal
    if (char_atual == '.') {
        tem_ponto = true;
        if (pos < 255) {
            buffer[pos++] = char_atual;
        }
        proximo_char();
        
        // Lê a parte decimal
        if (!isdigit(char_atual)) {
            info.tipo = ATOM_ERRO;
            strcpy(info.lexema.string, "Número float inválido");
            return info;
        }
        
        while (!fim_arquivo && isdigit(char_atual)) {
            if (pos < 255) {
                buffer[pos++] = char_atual;
            }
            proximo_char();
        }
    }
    
    buffer[pos] = '\0';
    
    if (tem_ponto) {
        info.tipo = ATOM_NUMERO_FLOAT;
        info.lexema.valor_float = atof(buffer);
    } else {
        info.tipo = ATOM_NUMERO_INT;
        info.lexema.valor_int = atoi(buffer);
    }
    
    return info;
}

// Função para ler string
static TInfoAtomo ler_string(void) {
    TInfoAtomo info;
    char buffer[256];
    int pos = 0;
    
    info.linha = linha_atual;
    
    proximo_char(); // Pula a aspa inicial
    
    while (!fim_arquivo && char_atual != '"') {
        if (char_atual == '\n') {
            info.tipo = ATOM_ERRO;
            strcpy(info.lexema.string, "String não terminada");
            return info;
        }
        
        if (pos < 255) {
            buffer[pos++] = char_atual;
        }
        proximo_char();
    }
    
    if (fim_arquivo) {
        info.tipo = ATOM_ERRO;
        strcpy(info.lexema.string, "String não terminada");
        return info;
    }
    
    buffer[pos] = '\0';
    info.tipo = ATOM_STRING;
    strcpy(info.lexema.string, buffer);
    
    proximo_char(); // Pula a aspa final
    
    return info;
}

// Função para ler caractere
static TInfoAtomo ler_caractere(void) {
    TInfoAtomo info;
    
    info.linha = linha_atual;
    
    proximo_char(); // Pula a aspa simples inicial
    
    if (fim_arquivo || char_atual == '\'') {
        info.tipo = ATOM_ERRO;
        strcpy(info.lexema.string, "Caractere vazio");
        return info;
    }
    
    char c = char_atual;
    proximo_char();
    
    if (char_atual != '\'') {
        info.tipo = ATOM_ERRO;
        strcpy(info.lexema.string, "Caractere inválido");
        return info;
    }
    
    info.tipo = ATOM_CHAR_CONST;
    info.lexema.string[0] = c;
    info.lexema.string[1] = '\0';
    
    proximo_char(); // Pula a aspa simples final
    
    return info;
}

// Função principal do analisador léxico
TInfoAtomo obter_atomo(void) {
    TInfoAtomo info;
    
    // Pula espaços e comentários
    while (!fim_arquivo) {
        pular_espacos();
        if (char_atual == '{') {
            if (!pular_comentario()) {
                info.tipo = ATOM_ERRO;
                info.linha = linha_atual;
                strcpy(info.lexema.string, "Comentário não fechado");
                return info;
            }
        } else {
            break;
        }
    }
    
    // Verifica fim de arquivo
    if (fim_arquivo) {
        info.tipo = ATOM_EOF;
        info.linha = linha_atual;
        strcpy(info.lexema.string, "EOF");
        return info;
    }
    
    info.linha = linha_atual;
    
    // Identifica o tipo de token
    switch (char_atual) {
        case ';':
            info.tipo = ATOM_PONTO_VIRGULA;
            strcpy(info.lexema.string, ";");
            proximo_char();
            break;
            
        case '.':
            info.tipo = ATOM_PONTO;
            strcpy(info.lexema.string, ".");
            proximo_char();
            break;
            
        case ',':
            info.tipo = ATOM_VIRGULA;
            strcpy(info.lexema.string, ",");
            proximo_char();
            break;
            
        case '(':
            info.tipo = ATOM_ABRE_PAR;
            strcpy(info.lexema.string, "(");
            proximo_char();
            break;
            
        case ')':
            info.tipo = ATOM_FECHA_PAR;
            strcpy(info.lexema.string, ")");
            proximo_char();
            break;
            
        case '[':
            info.tipo = ATOM_ABRE_COL;
            strcpy(info.lexema.string, "[");
            proximo_char();
            break;
            
        case ']':
            info.tipo = ATOM_FECHA_COL;
            strcpy(info.lexema.string, "]");
            proximo_char();
            break;
            
        case '+':
            info.tipo = ATOM_MAIS;
            strcpy(info.lexema.string, "+");
            proximo_char();
            break;
            
        case '-':
            info.tipo = ATOM_MENOS;
            strcpy(info.lexema.string, "-");
            proximo_char();
            break;
            
        case '*':
            info.tipo = ATOM_MULT;
            strcpy(info.lexema.string, "*");
            proximo_char();
            break;
            
        case '/':
            info.tipo = ATOM_DIV;
            strcpy(info.lexema.string, "/");
            proximo_char();
            break;
            
        case '<':
            proximo_char();
            if (char_atual == '-') {
                info.tipo = ATOM_ATRIBUICAO;
                strcpy(info.lexema.string, "<-");
                proximo_char();
            } else if (char_atual == '=') {
                info.tipo = ATOM_MENOR_IGUAL;
                strcpy(info.lexema.string, "<=");
                proximo_char();
            } else {
                info.tipo = ATOM_MENOR;
                strcpy(info.lexema.string, "<");
            }
            break;
            
        case '>':
            proximo_char();
            if (char_atual == '=') {
                info.tipo = ATOM_MAIOR_IGUAL;
                strcpy(info.lexema.string, ">=");
                proximo_char();
            } else {
                info.tipo = ATOM_MAIOR;
                strcpy(info.lexema.string, ">");
            }
            break;
            
        case '=':
            proximo_char();
            if (char_atual == '=') {
                info.tipo = ATOM_IGUAL;
                strcpy(info.lexema.string, "==");
                proximo_char();
            } else {
                info.tipo = ATOM_ERRO;
                strcpy(info.lexema.string, "Operador = inválido");
            }
            break;
            
        case '!':
            proximo_char();
            if (char_atual == '=') {
                info.tipo = ATOM_DIFERENTE;
                strcpy(info.lexema.string, "!=");
                proximo_char();
            } else {
                info.tipo = ATOM_ERRO;
                strcpy(info.lexema.string, "Operador ! inválido");
            }
            break;
            
        case '"':
            info = ler_string();
            break;
            
        case '\'':
            info = ler_caractere();
            break;
            
        default:
            if (isalpha(char_atual) || char_atual == '_') {
                info = ler_identificador();
            } else if (isdigit(char_atual)) {
                info = ler_numero();
            } else {
                info.tipo = ATOM_ERRO;
                sprintf(info.lexema.string, "Caractere inválido: %c", char_atual);
                proximo_char();
            }
            break;
    }
    
    return info;
}

// Função para inicializar o analisador
void inicializar_analisador(FILE *arquivo) {
    arquivo_fonte = arquivo;
    linha_atual = 1;
    coluna_atual = 0;
    fim_arquivo = false;
    proximo_char(); // Lê o primeiro caractere
}

// Função para finalizar o analisador
void finalizar_analisador(void) {
    if (arquivo_fonte != NULL) {
        fclose(arquivo_fonte);
        arquivo_fonte = NULL;
    }
}

// Função auxiliar para obter o nome do átomo
const char* nome_atomo(TipoAtomo tipo) {
    static const char* nomes[] = {
        "AND", "BEGIN", "CHAR", "ELSE", "END", "FLOAT", "FOR", "IF", "INT",
        "NOT", "OR", "PRG", "READ", "REPEAT", "RETURN", "SUBROT", "THEN",
        "UNTIL", "VAR", "VOID", "WHILE", "WRITE", "IDENTIFICADOR",
        "NUMERO_INT", "NUMERO_FLOAT", "CHAR_CONST", "STRING",
        "ATRIBUICAO", "MAIS", "MENOS", "MULT", "DIV", "IGUAL", "DIFERENTE",
        "MAIOR", "MENOR", "MAIOR_IGUAL", "MENOR_IGUAL",
        "PONTO_VIRGULA", "PONTO", "VIRGULA", "ABRE_PAR", "FECHA_PAR",
        "ABRE_COL", "FECHA_COL", "EOF", "ERRO"
    };
    
    if (tipo >= 0 && tipo < sizeof(nomes)/sizeof(nomes[0])) {
        return nomes[tipo];
    }
    return "DESCONHECIDO";
}