#include <stdio.h>
#include <stdlib.h>
#include "analisador_lexico.h"

// Gustavo Nascimento Siqueira - 10419057
// Felipe Ujvari Gasparino de Sousa - 10418415

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_fonte.lpd>\n", argv[0]);
        return 1;
    }
    
    FILE *arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo: %s\n", argv[1]);
        return 1;
    }
    
    printf("=== Análise Léxica do arquivo: %s ===\n\n", argv[1]);
    printf("%-15s %-20s %-30s %s\n", "Linha", "Token", "Tipo", "Valor");
    printf("%-15s %-20s %-30s %s\n", "-----", "-----", "----", "-----");
    
    inicializar_analisador(arquivo);
    
    TInfoAtomo atomo;
    do {
        atomo = obter_atomo();
        
        printf("%-15d %-20s %-30s ", atomo.linha, nome_atomo(atomo.tipo),
               atomo.tipo == ATOM_ERRO ? "ERRO" : "OK");
        
        switch (atomo.tipo) {
            case ATOM_NUMERO_INT:
                printf("%d", atomo.lexema.valor_int);
                break;
            case ATOM_NUMERO_FLOAT:
                printf("%f", atomo.lexema.valor_float);
                break;
            case ATOM_STRING:
            case ATOM_CHAR_CONST:
            case ATOM_IDENTIFICADOR:
            case ATOM_ERRO:
                printf("%s", atomo.lexema.string);
                break;
            default:
                printf("%s", atomo.lexema.string);
                break;
        }
        printf("\n");
        
        if (atomo.tipo == ATOM_ERRO) {
            fprintf(stderr, "\nErro léxico na linha %d: %s\n", 
                    atomo.linha, atomo.lexema.string);
        }
        
    } while (atomo.tipo != ATOM_EOF && atomo.tipo != ATOM_ERRO);
    
    finalizar_analisador();
    
    printf("\n=== Análise concluída ===\n");
    
    return 0;
}