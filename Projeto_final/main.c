#include "globals.h"
#include "ast.h"
#include "parser.h"
#include "analyze.h"
#include "symtab.h"

extern int yyparse();  // Função do Bison

int main(int argc, char *argv[]) {
    printf("Iniciando a análise...\n");

    // Realiza a análise sintática
    if (yyparse() == 0) {
        printf("Análise sintática concluída com sucesso!\n\n");
        
        if (root != NULL) {
            if (strcmp(argv[1], "--print-tree") == 0) 
                printReducedAST(root, 0);
            else if (strcmp(argv[1], "--print-full-tree") == 0) 
                printASTVertical(root);


            buildSymtab(root);
            freeAST(root);
        } else {
            printf("Aviso: Nenhuma árvore foi construída.\n");
        }
    } else {
        printf("Erro: Ocorreram erros durante a análise.\n");
    }

    return 0;
}