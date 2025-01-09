#include <stdio.h>

extern int yyparse();  // Função do Bison

int main() {
    printf("Iniciando a analise...\n");

    if (yyparse() == 0) {  // Chama o analisador sintático
        printf("Analise concluída com sucesso!\n");
    } else {
        printf("Ocorreram erros durante a analise.\n");
    }

    return 0;
}
