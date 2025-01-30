#include "globals.h"
#include "ast.h"
#include "parser.h"

extern int yyparse();  // Função do Bison

int main() {
    printf("Iniciando a análise...\n");

    // Realiza a análise sintática
    if (yyparse() == 0) {
        printf("Análise sintática concluída com sucesso!\n\n");
        
        // Verifica se a árvore foi construída
        if (root != NULL) {
            printf("Imprimindo a Árvore Sintática Abstrata:\n");
            printAST(root, 0);
            
            printASTVertical(root);
            // Libera a memória da árvore
            freeAST(root);
        } else {
            printf("Aviso: Nenhuma árvore foi construída.\n");
        }
    } else {
        printf("Erro: Ocorreram erros durante a análise.\n");
    }

    return 0;
}