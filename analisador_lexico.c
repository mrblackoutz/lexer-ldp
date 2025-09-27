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
        // Já chegamos ao fim, portanto não há novos caracteres a serem lidos
        return;
    }
    
    int c = fgetc(arquivo_fonte);
    if (c == EOF) {
        // Marca o fim do arquivo e zera o caractere atual para evitar leituras inválidas
        fim_arquivo = true;
        char_atual = '\0';
    } else {
        // Converte o inteiro lido para char e atualiza a posição horizontal
        char_atual = (char)c;
        coluna_atual++;
        
        if (char_atual == '\n') {
            // Sempre que encontrar uma quebra de linha, avança o contador de linhas
            linha_atual++;
            coluna_atual = 0;
        }
    }
}

// Função auxiliar para retroceder um caractere
static void retroceder_char(void) {
    if (!fim_arquivo && char_atual != '\0') {
        // Só retrocede se não estiver no fim do arquivo e se o caractere não for nulo
        
        ungetc(char_atual, arquivo_fonte);
        // Coloca o caractere atual de volta no fluxo de entrada
        
        coluna_atual--;
        // Atualiza a posição da coluna (decrementando)
        
        if (char_atual == '\n') {
            linha_atual--;
            // Se o caractere for uma quebra de linha, também decrementa o contador de linhas
        }
    }
}

// Função para pular espaços em branco
static void pular_espacos(void) {
    while (!fim_arquivo && isspace(char_atual)) {
        // Avança enquanto o caractere atual for branco (espaço, tab, quebra de linha)
        proximo_char();
    }
}

// Função para pular comentários
static TInfoAtomo pular_comentario(void) {
    TInfoAtomo info;
    int linha_inicio = linha_atual;
    
    if (char_atual == '{') {
        // Consome o '{' inicial antes de entrar no loop de leitura do comentário
        proximo_char();
        while (!fim_arquivo && char_atual != '}') {
            // Continua lendo até encontrar '}' ou atingir o fim do arquivo
            proximo_char();
        }
        if (fim_arquivo) {
            // Erro: comentário não fechado
            info.tipo = ATOM_ERRO;
            info.linha = linha_inicio;
            strcpy(info.lexema.string, "Comentário não fechado");
            return info;
        }
        // Garante que o '}' não seja processado posteriormente como símbolo avulso
        proximo_char();
    }
    
    // Retorna um token especial indicando que processou comentário com sucesso
    info.tipo = -1; // Valor especial para indicar sucesso
    return info;
}

// Função para verificar se é palavra reservada
static TipoAtomo verificar_palavra_reservada(const char *lexema) {
    for (int i = 0; palavras_reservadas[i].palavra != NULL; i++) {
        if (strcmp(lexema, palavras_reservadas[i].palavra) == 0) {
            // Encontrou uma correspondência na tabela de palavras reservadas
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
            // Armazena o caractere atual no buffer do lexema
            buffer[pos++] = char_atual;
        }
        // Avança para o próximo caractere para continuar o identificador
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
    bool erro = false;
    
    info.linha = linha_atual;
    
    // Lê a parte inteira
    while (!fim_arquivo && isdigit(char_atual)) {
        if (pos < 255) {
            // Captura cada dígito lido durante a fase inteira do número
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
        // Avança para tentar ler a parte fracionária
        proximo_char();
        
        // Lê a parte decimal
        if (!isdigit(char_atual)) {
            // Erro: número float mal formado, mas continua processando
            erro = true;
            strcpy(info.lexema.string, "Número float mal formado - falta parte decimal");
            
            // Tenta recuperar: volta o ponto e trata como inteiro
            retroceder_char();
            pos--; // Remove o ponto do buffer
            tem_ponto = false;
        } else {
            while (!fim_arquivo && isdigit(char_atual)) {
                if (pos < 255) {
                    // Continua acumulando os dígitos após o ponto
                    buffer[pos++] = char_atual;
                }
                proximo_char();
            }
        }
    }
    
    buffer[pos] = '\0';
    
    if (erro) {
        info.tipo = ATOM_ERRO;
        // O erro já foi definido acima
    } else if (tem_ponto) {
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
    int linha_inicio = linha_atual;
    
    info.linha = linha_inicio;
    
    // Move para o primeiro caractere após a aspa de abertura
    proximo_char();
    
    while (!fim_arquivo && char_atual != '"' && char_atual != '\n') {
        if (pos < 255) {
            // Armazena o caractere literal da string
            buffer[pos++] = char_atual;
        }
        // Continua a leitura até encontrar a aspa final ou erro
        proximo_char();
    }
    
    buffer[pos] = '\0';
    
    if (fim_arquivo || char_atual == '\n') {
        // Erro: string não terminada
        info.tipo = ATOM_ERRO;
        sprintf(info.lexema.string, "String não terminada (iniciada na linha %d)", linha_inicio);
        
        // Recuperação: continua na próxima linha se não chegou ao fim do arquivo
        if (char_atual == '\n') {
            proximo_char();
        }
    } else {
        // String válida
        info.tipo = ATOM_STRING;
        strcpy(info.lexema.string, buffer);
        // Consome a aspa final para prosseguir com a análise lexical
        proximo_char();
    }
    
    return info;
}

// Função para ler caractere
static TInfoAtomo ler_caractere(void) {
    TInfoAtomo info;
    char buffer[10];
    int pos = 0;
    
    info.linha = linha_atual;
    
    // Avança para o conteúdo após a aspa simples de abertura
    proximo_char();
    
    if (fim_arquivo || char_atual == '\'') {
        info.tipo = ATOM_ERRO;
        strcpy(info.lexema.string, "Caractere vazio");
        if (char_atual == '\'') {
            // Se a aspa de fechamento estiver presente, consome para evitar laço infinito
            proximo_char();
        }
        return info;
    }
    
    // Lê caracteres até encontrar aspa ou fim
    while (!fim_arquivo && char_atual != '\'' && char_atual != '\n' && pos < 9) {
        buffer[pos++] = char_atual;
        proximo_char();
    }
    buffer[pos] = '\0';
    
    if (char_atual != '\'') {
        // Erro: caractere mal formado
        info.tipo = ATOM_ERRO;
        if (pos > 1) {
            strcpy(info.lexema.string, "Constante caractere com mais de um caractere");
        } else {
            strcpy(info.lexema.string, "Caractere não terminado");
        }
        
        // Recuperação: pula até encontrar aspa ou fim de linha
        while (!fim_arquivo && char_atual != '\'' && char_atual != '\n' && 
               char_atual != ';' && char_atual != ' ') {
            proximo_char();
        }
        if (char_atual == '\'') {
            proximo_char();
        }
    } else if (pos != 1) {
        // Erro: mais de um caractere
        info.tipo = ATOM_ERRO;
        strcpy(info.lexema.string, "Constante caractere deve ter exatamente um caractere");
        // Consome a aspa de fechamento mesmo em caso de erro para continuar análise
        proximo_char();
    } else {
        // Caractere válido
        info.tipo = ATOM_CHAR_CONST;
        info.lexema.string[0] = buffer[0];
        info.lexema.string[1] = '\0';
        // Consome a aspa final para posicionar no próximo símbolo
        proximo_char();
    }
    
    return info;
}

// Função principal do analisador léxico
TInfoAtomo obter_atomo(void) {
    TInfoAtomo info;
    
    // Pula espaços e comentários
    while (!fim_arquivo) {
        pular_espacos();
        if (char_atual == '{') {
            TInfoAtomo comentario_result = pular_comentario();
            if (comentario_result.tipo == ATOM_ERRO) {
                // Retorna o erro do comentário não fechado
                return comentario_result;
            }
        } else {
            // Sai do laço assim que encontrar um caractere relevante
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
                // Erro: operador = sozinho não existe em LPD
                info.tipo = ATOM_ERRO;
                strcpy(info.lexema.string, "Operador '=' inválido (use '==' para comparação ou '<-' para atribuição)");
                // Não consome o próximo caractere para permitir sua análise
            }
            break;
            
        case '!':
            proximo_char();
            if (char_atual == '=') {
                info.tipo = ATOM_DIFERENTE;
                strcpy(info.lexema.string, "!=");
                proximo_char();
            } else {
                // Erro: operador ! sozinho não existe em LPD
                info.tipo = ATOM_ERRO;
                strcpy(info.lexema.string, "Operador '!' inválido (use '!=' para diferença ou 'not' para negação)");
                // Não consome o próximo caractere para permitir sua análise
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
                // Trata sequências iniciadas por letra ou '_' como identificadores/palavras reservadas
                info = ler_identificador();
            } else if (isdigit(char_atual)) {
                // Sequências numéricas são encaminhadas para o leitor de números
                info = ler_numero();
            } else {
                // Caractere inválido - registra erro e continua
                info.tipo = ATOM_ERRO;
                sprintf(info.lexema.string, "Caractere inválido: '%c' (ASCII: %d)", 
                        char_atual, (unsigned char)char_atual);
                // Consome o símbolo inesperado para tentar recuperar a análise
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
    // Faz a primeira leitura para preencher char_atual antes da análise começar
    proximo_char();
}

// Função para finalizar o analisador
void finalizar_analisador(void) {
    // Não fecha o arquivo aqui, pois pode ter sido passado de outro módulo
    arquivo_fonte = NULL;
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