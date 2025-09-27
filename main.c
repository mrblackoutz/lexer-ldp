#include <stdio.h>
#include <stdlib.h>
#include "analisador_lexico.h"

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
    printf("%-6s %-20s %-15s %s\n", "Linha", "Token", "Status", "Valor/Descrição");
    printf("%-6s %-20s %-15s %s\n", "------", "--------------------", "---------------", "----------------------------------------");
    
    inicializar_analisador(arquivo);
    
    TInfoAtomo atomo;
    int total_tokens = 0;
    int total_erros = 0;
    
    do {
        atomo = obter_atomo();
        
        if (atomo.tipo != ATOM_EOF) {
            total_tokens++;
        }
        
        // Formata a saída
        printf("%-6d %-20s ", atomo.linha, nome_atomo(atomo.tipo));
        
        if (atomo.tipo == ATOM_ERRO) {
            printf("%-15s %s", "*** ERRO ***", atomo.lexema.string);
            total_erros++;
        } else {
            printf("%-15s ", "OK");
            
            switch (atomo.tipo) {
                case ATOM_NUMERO_INT:
                    printf("%d", atomo.lexema.valor_int);
                    break;
                case ATOM_NUMERO_FLOAT:
                    printf("%.6f", atomo.lexema.valor_float);
                    break;
                case ATOM_STRING:
                    printf("\"%s\"", atomo.lexema.string);
                    break;
                case ATOM_CHAR_CONST:
                    printf("'%s'", atomo.lexema.string);
                    break;
                case ATOM_IDENTIFICADOR:
                case ATOM_ATRIBUICAO:
                case ATOM_MAIS:
                case ATOM_MENOS:
                case ATOM_MULT:
                case ATOM_DIV:
                case ATOM_IGUAL:
                case ATOM_DIFERENTE:
                case ATOM_MAIOR:
                case ATOM_MENOR:
                case ATOM_MAIOR_IGUAL:
                case ATOM_MENOR_IGUAL:
                case ATOM_PONTO_VIRGULA:
                case ATOM_PONTO:
                case ATOM_VIRGULA:
                case ATOM_ABRE_PAR:
                case ATOM_FECHA_PAR:
                case ATOM_ABRE_COL:
                case ATOM_FECHA_COL:
                    printf("%s", atomo.lexema.string);
                    break;
                case ATOM_EOF:
                    printf("Fim do arquivo");
                    break;
                default:
                    // Para palavras reservadas
                    break;
            }
        }
        printf("\n");
        
    } while (atomo.tipo != ATOM_EOF);
    
    printf("\n");
    printf("==================================================================\n");
    printf("                        RESUMO DA ANÁLISE                         \n");
    printf("==================================================================\n");
    printf("Total de tokens processados: %d\n", total_tokens);
    printf("Total de erros encontrados: %d\n", total_erros);
    
    if (total_erros > 0) {
        printf("\n*** ATENÇÃO: Foram encontrados %d erro(s) léxico(s) no código ***\n", total_erros);
        printf("Por favor, verifique os erros listados acima e corrija o código fonte.\n");
    } else {
        printf("\n✓ Análise léxica concluída com sucesso! Nenhum erro encontrado.\n");
    }
    printf("==================================================================\n");
    
    fclose(arquivo);
    finalizar_analisador();
    
    return (total_erros > 0) ? 1 : 0;
}