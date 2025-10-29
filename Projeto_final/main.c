#include "globals.h"
#include "asnt.h"
#include "parser.h"
#include "symtab.h"
#include "semantic.h"
#include "cinter.h"
#include "assembly_mips.h"
#include "binario_proc.h"

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
        else if (argc > 1 && strcmp(argv[1], "--print-full-tree") == 0){
            FILE* outfile = fopen("Output/asnt.txt", "w");
            printASTVertical(root, outfile);
            fclose(outfile);
            printf("Árvore sintática completa impressa em 'Projeto_final/Output/asnt.txt'.\n");
        } 
            

        // Construção da tabela de símbolos
        printf("\nConstruindo tabela de símbolos...\n"); 
        buildSymtab(root);
        printf("Tabela de símbolos construída com sucesso! \n");
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
            //abrir arquivo e para usar a função e salvar na pasta output
            FILE* out_qd = fopen("Output/quadruples.txt", "r");
            if (out_qd == NULL) {
                printError("Erro ao abrir o arquivo.\n");
                return 1;
            }
            
            
            // Verifica se o argumento --dispatcher foi passado na linha de comando
            int isDispatcherFile = 0;
            for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "--dispatcher") == 0) {
                    isDispatcherFile = 1;
                    break;
                }
            }
        
            if (isDispatcherFile) {
                generateAssembly(out_qd, 0);  // Modo dispatcher (sem inicialização BCP)
                printf("Modo dispatcher ativado - código gerado sem inicialização BCP\n");
            } else {
                generateAssembly(out_qd, 1);  // Modo normal (com inicialização BCP)
                printf("Modo normal - código gerado com inicialização BCP\n");
            }
            
            fclose(out_qd);
            printSuccess("Código assembly gerado e salvo na pasta Output\n");

            FILE* out_asm = fopen("Output/assembly.asm", "r");
            if (out_asm == NULL) {
                printError("Erro ao abrir o arquivo.\n");
                return 1;
            }
            read_assembly_file(out_asm);
            fclose(out_asm);

        } else {
            printError("\nGeração de código intermediário, de código assembly e de código binário ignorada devido a erros.\n");
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