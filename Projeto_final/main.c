#include "globals.h"
#include "asnt.h"
#include "parser.h"
#include "analyze.h"
#include "symtab.h"
#include "semantic.h"

extern int yyparse(); /*função do parser*/

int main(int argc, char *argv[]) {
    printf("Iniciando a análise...\n");

    // Realiza a análise sintática
    int syntaxError = yyparse();
    if (syntaxError != 0) {
        printf("Erro: Ocorreram erros sintáticos durante a análise.\n");
        return 1;
    }
    printf("Análise sintática concluída com sucesso!\n\n");
    
    if (root != NULL) {
        // Imprime a árvore se solicitado
        if (argc > 1 && strcmp(argv[1], "--print-tree") == 0) 
            printReducedAST(root, 0);
        else if (argc > 1 && strcmp(argv[1], "--print-full-tree") == 0) 
            printASTVertical(root);

        // Construção da tabela de símbolos
        printf("\nConstruindo tabela de símbolos...\n"); 
        buildSymtab(root);
        
        // Análise semântica
        printf("\nIniciando análise semântica...\n");
        semanticAnalysis(root);
        
        if (hasSemanticError) {
            printf("Erro: Ocorreram erros semânticos durante a análise.\n");
            freeAST(root);
            freeTypeTable();  // Libera a tabela de tipos
            return 2;
        }
        printf("Análise semântica concluída com sucesso!\n");

        pop_scope();
        freeAST(root);
        freeTypeTable();  // Libera a tabela de tipos
    } else {
        printf("Aviso: Nenhuma árvore foi construída.\n");
        return 3;
    }

    return 0;
}