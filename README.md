# lexer-ldp

Analisador léxico em C para a linguagem LPD utilizada nas disciplinas de compiladores. O módulo lê um arquivo fonte e converte o fluxo de caracteres em tokens tipados.

## Compilação e execução

```bash
gcc teste_analisador.c analisador_lexico.c -o teste_analisador
./teste_analisador exemplo.lpd
```

## Documentação

Consulte `DOC_atualizado.tex` para uma descrição completa da arquitetura, dos tokens reconhecidos e do fluxo da função `obter_atomo`.