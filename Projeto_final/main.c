#include "globals.h"
#include "asnt.h"
#include "parser.h"
#include "symtab.h"
#include "semantic.h"
#include "cinter.h"

extern int yyparse(); /*função do parser*/
extern int lexErrorCount; /*contador de erros léxicos*/
extern int syntaxErrorCount; /*contador de erros sintáticos*/
extern int semanticErrorCount; /*contador de erros semânticos*/

int main(int argc, char *argv[]) {
    int success = 1; // Flag para indicar se o processo foi bem-sucedido
    printf("Iniciando a análise...\n");

    // Realiza a análise sintática
    yyparse();
    if (lexErrorCount > 0) {
        printf("Análise léxica concluída com %d erro(s).\n\n", syntaxErrorCount);
        success = 0;
    } else {
        printSuccess("Análise léxica concluída com sucesso!\n\n");
    }
    if (syntaxErrorCount > 0) {
        printf("Análise sintática concluída com %d erro(s).\n\n", syntaxErrorCount);
        success = 0;
    } else {
        printSuccess("Análise sintática concluída com sucesso!\n\n");
    }
    
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
        
        if (semanticErrorCount > 0) {
            printf("Análise semântica concluída com %d erro(s).\n", semanticErrorCount);
            success = 0;
        } else {
            printSuccess("Análise semântica concluída com sucesso!\n");
        }

        // Geração de código intermediário apenas se não houver erros
        if (success) {
            printf("\nGerando código intermediário...\n");
            ircode_generate(root);
            printSuccess("Geração de código intermediário concluída!\n");
        } else {
            printError("\nGeração de código intermediário ignorada devido a erros.\n");
        }

        pop_scope();
        freeAST(root);

        if (success) {
            freeIRCode();  // Libera a memória do código intermediário
        }
    } else {
        printf("Aviso: Nenhuma árvore foi construída.\n");
        return 3;
    }
    
    // Resumo final dos erros
    if (lexErrorCount > 0 || syntaxErrorCount > 0 || semanticErrorCount > 0) {
        printf("\n-------------------------------------\n");
        printf("Resumo de erros:\n");
        printf("- Erros léxicos: %d\n", lexErrorCount);
        printf("- Erros sintáticos: %d\n", syntaxErrorCount);
        printf("- Erros semânticos: %d\n", semanticErrorCount);
        return 1;
    }

    return 0;
}