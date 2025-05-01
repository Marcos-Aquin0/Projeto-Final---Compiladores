#include "globals.h"
#include "symtab.h"
#include "semantic.h"

// Declarações das funções semânticas
static int hash(char* key);
static int checkTypeCompatibility(const char* type1, const char* type2);
static void checkVariableDeclaration(ASTNode* node);
static void checkAssignment(ASTNode* node);
static void checkFunctionCall(ASTNode* node);
static char* getExpressionType(ASTNode* node);
static void checkMainFunction(void);
static void checkAtLeastOneDeclaration(void);
static void checkLastFunctionIsMain(void);
static void checkArrayAccess(ASTNode* node);
static void validateExpressionVariables(ASTNode* expr);
// variáveis globias
int semanticErrorCount = 0;  // Contador para erros semânticos
static int hasMainFunction = 0;  // flag para verificar existenia da funcao main
static int hasDeclaration = 0;   // Flag para saber se tem pelo menos uma declaração no código
static ASTNode* lastFunctionNode = NULL;  // Ponteiro para último nó de função na AST

// Estrutura para rastrear variáveis já reportadas com erro
#define ERROR_TRACK_SIZE 211
static struct {
    char* name;
    int lineno;
    int reported;
} errorTracker[ERROR_TRACK_SIZE] = {{NULL, 0, 0}};

// Função de hash (mantida do código original)
static int hash(char* key) {
    unsigned long hash = 5381;  // Valor inicial para o algoritmo DJB2
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash % ERROR_TRACK_SIZE;
}

// Verifica compatibilidade de tipos
static int checkTypeCompatibility(const char* type1, const char* type2) {
    if (!type1 || !type2) return 0;
    return (strcmp(type1, type2) == 0);
}

// Função para verificar se um erro já foi reportado para uma variável
static int errorAlreadyReported(char* name, int lineno) {
    if (!name) return 0;
    
    int index = hash(name) % ERROR_TRACK_SIZE;
    
    if (errorTracker[index].name && 
        strcmp(errorTracker[index].name, name) == 0 && 
        errorTracker[index].reported) {
        return 1;
    }
    
    // Registrar este erro
    if (errorTracker[index].name) {
        free(errorTracker[index].name);
    }
    
    errorTracker[index].name = strdup(name);
    errorTracker[index].lineno = lineno;
    errorTracker[index].reported = 1;
    
    return 0;
}

// Função para limpar o rastreador de erros
static void clearErrorTracker(void) {
    for (int i = 0; i < ERROR_TRACK_SIZE; i++) {
        if (errorTracker[i].name) {
            free(errorTracker[i].name);
            errorTracker[i].name = NULL;
            errorTracker[i].lineno = 0;
            errorTracker[i].reported = 0;
        }
    }
}

// Verifica se a expressão em um comando return está declarada
static void checkReturnStatement(ASTNode* node) {
    if (!node || !node->left) return;
    
    ASTNode* returnExpr = node->left;
    
    // Se o retorno é uma variável simples
    if (returnExpr->type == NODE_VAR && returnExpr->value) {
        BucketList l = st_lookup(returnExpr->value);
        if (!l && !errorAlreadyReported(returnExpr->value, node->lineno)) {
            printError("Erro semântico: Variável '%s' usada em comando return não foi declarada (linha %d)",
                    returnExpr->value, node->lineno);
            semanticErrorCount++;
        }
    }
    // Se o retorno é uma expressão mais complexa
    else if (returnExpr->type == NODE_EXPR || 
             returnExpr->type == NODE_SUM_EXPR ||
             returnExpr->type == NODE_TERM) {
        validateExpressionVariables(returnExpr);
    }
    // Se o retorno é um acesso a array
    else if (returnExpr->type == NODE_ARRAY_ACCESS) {
        checkArrayAccess(returnExpr);
    }
    // Se o retorno é uma chamada de função
    else if (returnExpr->type == NODE_ACTIVATION) {
        checkFunctionCall(returnExpr);
    }
}
// Função principal de análise semântica
void semanticAnalysis(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    // Limpar o rastreador de erros quando começamos a análise da AST
    if (node->type == NODE_PROGRAM) {
        clearErrorTracker();
    }

    switch (node->type) {
        case NODE_VAR_DECL:
            if (node->value != NULL && node->idType != NULL) {
                checkVariableDeclaration(node);
            }
            break;

        case NODE_EXPR:
            if (node->value != NULL && strcmp(node->value, "=") == 0) {
                checkAssignment(node);
            }
            break;

        case NODE_ARRAY_ACCESS:
            checkArrayAccess(node);
            break;
        
        case NODE_SIMP_EXPR:
            printf("DEBUG - NODE_SIMP_EXPR encontrado na linha %d\n", node->lineno);
            
            // Verificar se o filho esquerdo é uma variável
            if (node->left && node->left->type == NODE_VAR && node->left->value) {
                printf("  Variável esquerda: %s\n", node->left->value);
                // Verificar se a variável foi declarada
                BucketList l = st_lookup(node->left->value);
                if (!l && !errorAlreadyReported(node->left->value, node->lineno)) {
                    printError("Erro semântico: Variável '%s' usada em comparação não foi declarada (linha %d)",
                            node->left->value, node->lineno);
                    semanticErrorCount++;
                }
            }
            
            // Verificar o operador relacional (filho direito)
            if (node->right && node->right->type == NODE_RELATIONAL) {
                printf("  Operador relacional encontrado\n");
                
                // Verificar o operador de comparação
                if (node->right->value) {
                    printf("  Operador: %s\n", node->right->value);
                }
                
                // Verificar o valor/variável direita (pode ser Factor ou outra Var)
                if (node->right->right) {
                    if (node->right->right->type == NODE_VAR && node->right->right->value) {
                        printf("  Variável direita: %s\n", node->right->right->value);
                        // Verificar se a variável foi declarada
                        BucketList l = st_lookup(node->right->right->value);
                        if (!l && !errorAlreadyReported(node->right->right->value, node->lineno)) {
                            printError("Erro semântico: Variável '%s' usada em comparação não foi declarada (linha %d)",
                                    node->right->right->value, node->lineno);
                            semanticErrorCount++;
                        }
                    } 
                    else if (node->right->right->type == NODE_FACTOR) {
                        printf("  Constante direita: %s\n", 
                              node->right->right->value ? node->right->right->value : "desconhecido");
                    }
                }
            }
            break;

        case NODE_ACTIVATION:
            checkFunctionCall(node);
            break;

        case NODE_FUNC_DECL:
            if (node->value != NULL) {
                push_scope(node->value);
                lastFunctionNode = node;

                if (strcmp(node->value, "main") == 0) {
                    hasMainFunction = 1;
                }

                hasDeclaration = 1;
            }
            break;
        case NODE_RETURN_DECL:
            checkReturnStatement(node);
            break;
        default:
            break;
    }

    semanticAnalysis(node->left);
    semanticAnalysis(node->right);

    if (node->type == NODE_PROGRAM) {
        checkMainFunction();
        checkAtLeastOneDeclaration();
        checkLastFunctionIsMain();
    }
}

static void checkVariableDeclaration(ASTNode* node) {
    if (!node || !node->value) return;

    char* scope = current_scope();
    BucketList l = st_lookup(node->value);
    BucketList aux = st_lookup_in_scope(node->value, "global");

    // Verifica se a variável já foi declarada no mesmo escopo
    if (l != NULL) {
        if (strcmp(l->scope, scope) == 0 && l->lines->lineno != node->lineno) {
            printError("Erro semântico: Variável '%s' já declarada no escopo '%s' (linha %d)",
                    node->value, scope, node->lineno);
            semanticErrorCount++;
        }
        else if (aux != NULL && strcmp(aux->idType, "func") == 0 && strcmp(aux->name, l->name) == 0) {
            printError("Erro semântico: Variável '%s' declarado com nome de função (linha %d)",
                    node->value, node->lineno);
            semanticErrorCount++;
        }
        else if (strcmp(node->idType,"void") == 0) {
            printError("Erro semântico: Variável '%s' declarada com tipo void (linha %d)",
                    node->value, node->lineno);
            semanticErrorCount++;
        }
    }

    // Verifica se o tamanho do array é válido (maior que 0)
    if (node->isArray && node->arraySize <= 0) {
        printError("Erro semântico: Tamanho do array '%s' deve ser positivo (linha %d)",
                node->value, node->lineno);
        semanticErrorCount++;
    }
}

static void checkAssignment(ASTNode* node) {
    if (!node->left || !node->right) return;

    // Check if left side is array access
    if (node->left->type == NODE_ARRAY_ACCESS) {
        checkArrayAccess(node->left);
    }

    char* leftType = getExpressionType(node->left);
    char* rightType = getExpressionType(node->right);

    // Get scopes
    char* leftScope = current_scope();
    char* rightScope = current_scope();

    // Conferir a declaração do lado esquerdo
    if (leftType == NULL) {
        printError("Erro semântico na linha %d: Variável '%s' sendo usada sem ter sido declarada.",
                node->lineno, node->left->value ? node->left->value : "desconhecida");
        semanticErrorCount++;
        return;
    }

    // Conferir a declaração do lado direito
    if (rightType == NULL) {
        printError("Erro semântico na linha %d: Expressão ou variável '%s' sendo usada sem ter sido declarada.",
                node->lineno, node->right->value ? node->right->value : "desconhecida");
        semanticErrorCount++;
        return;
    }

    // Checar a compatibilidade de tipos
    if (!checkTypeCompatibility(leftType, rightType)) {
        printError("Erro semântico na linha %d: Incompatibilidade de tipos na atribuição. "
                  "Variável '%s' é do tipo '%s' mas está recebendo valor do tipo '%s'.",
                node->lineno, node->left->value ? node->left->value : "desconhecida",
                leftType, rightType);
        semanticErrorCount++;
    }
}

static void checkFunctionCallParameters(ASTNode* node) {
    if (!node || !node->left || !node->left->value) return;

    char* funcName = node->left->value;
    if (strcmp(funcName, "output") == 0 || strcmp(funcName, "input") == 0) {
        return;  // Não verifica parâmetros para funções de entrada/saída
    }
    // Busca a declaração global da função
    BucketList funcDecl = st_lookup_in_scope(funcName, "global");
    if (!funcDecl) return;  // Função não encontrada, não verifica parâmetros
    
    // Busca a chamada da função no escopo atual
    char* currentScope = current_scope();
    BucketList funcCall = st_lookup_in_scope(funcName, currentScope);
    
    // Se não encontrou no escopo atual e não estamos no escopo global,
    // procura no escopo global (para chamadas diretas na global)
    if (!funcCall && strcmp(currentScope, "global") != 0) {
        funcCall = st_lookup_in_scope(funcName, "global");
    }
    
    if (!funcCall) return;  // Não conseguiu encontrar a chamada, pula verificação
    
    // Compara o número de parâmetros entre declaração e chamada
    if (funcDecl->paramCount != funcCall->paramCount) {
        printError("Erro semântico: Função '%s' chamada com %d argumentos, mas foi declarada com %d parâmetros (linha %d)",
                  funcName, funcCall->paramCount, funcDecl->paramCount, node->lineno);
        semanticErrorCount++;
        return;
    }
    
    // Obtém os parâmetros da declaração e da chamada
    ParamInfo declParam = funcDecl->params;
    ParamInfo callParam = funcCall->params;
    
    // Compara cada parâmetro
    int paramNum = 1;
    while (declParam != NULL && callParam != NULL) {
        // Verifica compatibilidade de tipos
        int typesCompatible = checkTypeCompatibility(callParam->paramType, declParam->paramType);
        
        // Verifica se ambos são arrays ou nenhum é array
        int arrayMatchError = (callParam->isArray != declParam->isArray);
        
        if (!typesCompatible || arrayMatchError) {
            if (arrayMatchError && typesCompatible) {
                printError("Erro semântico: Incompatibilidade de array no argumento %d da chamada da função '%s'. Esperado '%s%s', recebido '%s%s' (linha %d)",
                         paramNum, funcName, declParam->paramType, declParam->isArray ? "[]" : "", 
                         callParam->paramType, callParam->isArray ? "[]" : "", node->lineno);
            } else {
                printError("Erro semântico: Incompatibilidade de tipos no argumento %d da chamada da função '%s'. Esperado '%s%s', recebido '%s%s' (linha %d)",
                         paramNum, funcName, declParam->paramType, declParam->isArray ? "[]" : "", 
                         callParam->paramType, callParam->isArray ? "[]" : "", node->lineno);
            }
            semanticErrorCount++;
        }
        
        declParam = declParam->next;
        callParam = callParam->next;
        paramNum++;
    }
}

static void checkFunctionCall(ASTNode* node) {
    if (!node->left || !node->left->value) return;

    BucketList l = st_lookup(node->left->value);
    if (!l) {
        if (!errorAlreadyReported(node->left->value, node->lineno)) {
            printError("Erro semântico: Função '%s' não declarada (linha %d)",
                    node->left->value, node->lineno);
            semanticErrorCount++;
        }
        return;
    }

    if (strcmp(l->idType, "func") != 0) {
        if (!errorAlreadyReported(node->left->value, node->lineno)) {
            printError("Erro semântico: '%s' não é uma função (linha %d)",
                    node->left->value, node->lineno);
            semanticErrorCount++;
        }
        return;
    }

    // Verificar a lista de parâmetros
    checkFunctionCallParameters(node);

    // Verificar os argumentos da chamada de função
    ASTNode* args = node->right;
    while (args != NULL) {
        // Para cada argumento, verificar se está declarado
        if (args->type == NODE_VAR || args->type == NODE_EXPR) {
            // Obter o nome da variável do argumento
            char* argName = NULL;
            if (args->type == NODE_VAR) {
                argName = args->value;
            } else if (args->left && args->left->type == NODE_VAR) {
                argName = args->left->value;
            }

            if (argName != NULL) {
                // Verificar se a variável está declarada
                BucketList argVar = st_lookup(argName);
                if (!argVar && !errorAlreadyReported(argName, node->lineno)) {
                    printError("Erro semântico: Argumento '%s' usado na chamada da função '%s' não foi declarado (linha %d)",
                            argName, node->left->value, node->lineno);
                    semanticErrorCount++;
                }
            }
        }
        // Se for uma expressão mais complexa, devemos validar recursivamente
        // que todos os componentes estão declarados
        else if (args->type == NODE_ARRAY_ACCESS) {
            // Verificar acesso ao array
            checkArrayAccess(args);
        }
        else if (args->type == NODE_ACTIVATION) {
            // Verificar chamada de função aninhada
            checkFunctionCall(args);
        }
        else if (args->left != NULL) {
            // Validar variáveis em expressões mais complexas
            validateExpressionVariables(args);
        }

        // Avançar para o próximo argumento
        args = args->left;
    }
}

// Nova função auxiliar para validar variáveis em expressões
static void validateExpressionVariables(ASTNode* expr) {
    if (expr == NULL) return;

    // Verificar o nó atual se for uma variável
    if (expr->type == NODE_VAR && expr->value != NULL) {
        BucketList var = st_lookup(expr->value);
        if (!var && !errorAlreadyReported(expr->value, expr->lineno)) {
            printError("Erro semântico: Variável '%s' usada em expressão não foi declarada (linha %d)",
                    expr->value, expr->lineno);
            semanticErrorCount++;
        }
    }
    else if (expr->type == NODE_ARRAY_ACCESS) {
        checkArrayAccess(expr);
    }
    else if (expr->type == NODE_ACTIVATION) {
        checkFunctionCall(expr);
    }

    // Validar recursivamente os filhos da expressão
    validateExpressionVariables(expr->left);
    validateExpressionVariables(expr->right);
}

static char* getExpressionType(ASTNode* node) {
    if (node == NULL) return NULL;

    switch (node->type) {
        case NODE_VAR: {
            if (node->value == NULL) {
                return NULL;
            }

            // Se for um acesso a array (tem filho direito que é o índice)
            if (node->right != NULL) {
                // Buscar na tabela de símbolos
                BucketList l = st_lookup(node->value);
                if (l && strstr(l->dataType, "[]") != NULL) {
                    // Verifica se o índice é um inteiro
                    char* indexType = getExpressionType(node->right);
                    if (indexType == NULL || strcmp(indexType, "int") != 0) {
                        printError("Erro semântico: índice do array deve ser inteiro");
                        return NULL;
                    }
                    // Retorna o tipo base do array (sem os colchetes)
                    char* baseType = strdup(l->dataType);
                    char* bracketPos = strstr(baseType, "[]");
                    if (bracketPos) *bracketPos = '\0';
                    return baseType;
                }
            }

            // Caso normal (não é acesso a array)
            BucketList l = st_lookup(node->value);
            return l ? l->dataType : NULL;
        }

        case NODE_ARRAY_ACCESS: {
            if (node->left) {  // filho da esquerda contém o identificador do array
                BucketList l = st_lookup(node->left->value);
                if (!l) {
                    l = st_lookup_in_scope(node->left->value, current_scope());
                }
                if (l && l->isArray) {
                    return l->dataType;
                }
            }
            return NULL;
        }

        case NODE_FACTOR:
            if (node->value != NULL) {
                return "int";  // Números são do tipo int
            }
            break;

        case NODE_ACTIVATION:
            if (node->left != NULL && node->left->value != NULL) {
                // Buscar o tipo de retorno da função chamada
                BucketList l = st_lookup(node->left->value);
                return l ? l->dataType : NULL;
            }
            break;

        case NODE_SUM_EXPR:
        case NODE_TERM:
        case NODE_MULT:
            // Operadores matemáticos retornam tipo int
            return "int";

        default:
            break;
    }

    return NULL;
}

static void checkMainFunction(void) {
    if (!hasMainFunction) {
        printError("Erro semântico: Função 'main' não declarada.");
        semanticErrorCount++;
    }
}

// Adicionar função para verificar se há pelo menos uma declaração
static void checkAtLeastOneDeclaration(void) {
    if (!hasDeclaration) {
        printError("Erro semântico: O código deve conter pelo menos uma declaração (função ou variável).");
        semanticErrorCount++;
    }
}

// Adicionar função para verificar se a última função é void main(void)
static void checkLastFunctionIsMain(void) {
    if (lastFunctionNode && strcmp(lastFunctionNode->value, "main") != 0) {
        printError("Erro semântico: A última declaração de função deve ser 'void main(void)'.");
        semanticErrorCount++;
    }
}

// Liberar recursos utilizados pelo analisador semântico
void freeSemanticResources(void) {
    clearErrorTracker();
}

static void checkArrayAccess(ASTNode* node) {
    if (!node->value) return;

    BucketList l = st_lookup(node->value);
    if (!l) {
        printError("Erro semântico: Variável '%s' não declarada (linha %d)",
                node->value, node->lineno);
        semanticErrorCount++;
        return;
    }

    if (!l->isArray) {
        printError("Erro semântico: Variável '%s' não é um array (linha %d)",
                node->value, node->lineno);
        semanticErrorCount++;
        return;
    }

    // Verifica se o índice é uma expressão inteira
    if (node->right) {
        char* indexType = getExpressionType(node->right);
        if (!indexType || strcmp(indexType, "int") != 0) {
            printError("Erro semântico: Índice do array deve ser do tipo int (linha %d)",
                    node->lineno);
            semanticErrorCount++;
        }

        // Se o índice é uma constante, verifica os limites
        if (node->right->type == NODE_FACTOR && node->right->value) {
            int index = atoi(node->right->value);
            if (index < 0) {
                printError("Erro semântico: Índice negativo (%d) no acesso ao array '%s' (linha %d)",
                        index, node->value, node->lineno);
                semanticErrorCount++;
            } 
            else if (l->arraySize > 0 && index >= l->arraySize) {
                printError("Erro semântico: Índice %d fora dos limites do array '%s[%d]' (linha %d)",
                        index, node->value, l->arraySize, node->lineno);
                semanticErrorCount++;
            }
        }
    }
}