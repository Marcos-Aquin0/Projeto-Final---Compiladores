#include "globals.h"
#include "symtab.h"
#include "semantic.h"

// Declarações das funções semânticas
static int hash(char* key);
static int checkTypeCompatibility(const char* type1, const char* type2);
static void checkVariableDeclaration(ASTNode* node);
static void checkAssignment(ASTNode* node);
static char* getArrayElementType(char* arrayType);
static void checkFunctionCall(ASTNode* node);
static char* getExpressionType(ASTNode* node);
static void checkMainFunction(void);
static void checkAtLeastOneDeclaration(void);
static void checkLastFunctionIsMain(void);
static void checkArrayAccess(ASTNode* node);
static void validateExpressionVariables(ASTNode* expr);
// variáveis globias
int hasSemanticError = 0;
static int hasMainFunction = 0;  // flag para verificar existenia da funcao main
static int hasDeclaration = 0;   // Flag para saber se tem pelo menos uma declaração no código
static ASTNode* lastFunctionNode = NULL;  // Ponteiro para último nó de função na AST

// Estrutura para armazenar informações de tipo
typedef struct TypeInfo {
    char* type; //int ou void
    int isArray;
    int arraySize;
} TypeInfo;

// Hash table para tipos
#define TYPE_SIZE 211
static TypeInfo* typeTable[TYPE_SIZE] = {NULL};  // Inicializa todos os ponteiros como NULL

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

    return hash % TYPE_SIZE;
}

// Verifica compatibilidade de tipos
static int checkTypeCompatibility(const char* type1, const char* type2) {
    if (!type1 || !type2) return 0;
    return (strcmp(type1, type2) == 0);
}

//Insere informações de tipo na tabela de tipos.
static void insertTypeInfo(char* name, char* type, int isArray, int arraySize) {
    if (name == NULL || type == NULL) {
        fprintf(stderr, "Erro: Parâmetros inválidos para insertTypeInfo.\n");
        return;
    }

    int index = hash(name);
    if (typeTable[index] != NULL) {
        // Libera a memória do TypeInfo existente antes de sobrescrever
        free(typeTable[index]->type);
        free(typeTable[index]);
    }

    TypeInfo* info = (TypeInfo*)malloc(sizeof(TypeInfo));
    if (info == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para TypeInfo.\n");
        exit(EXIT_FAILURE);
    }

    info->type = strdup(type);
    info->isArray = isArray;
    info->arraySize = arraySize;
    typeTable[index] = info;

    //printf("DEBUG: Inserindo tipo '%s' para '%s' na tabela de tipos (hash: %d)\n", type, name, index);
}

static TypeInfo* lookupTypeInfo(char* name) {
    if (name == NULL) {
        return NULL;
    }

    int index = hash(name);
    if (index < 0 || index >= TYPE_SIZE) {
        fprintf(stderr, "Erro: Índice inválido na tabela de tipos.\n");
        return NULL;
    }

    return typeTable[index];
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
                insertTypeInfo(node->value, node->idType, node->isArray, node->arraySize);
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
            fprintf(stderr, "Erro semântico: Variável '%s' já declarada no escopo '%s' (linha %d)\n",
                    node->value, scope, node->lineno);
            hasSemanticError = 1;
        }
        else if (aux != NULL && strcmp(aux->idType, "func") == 0 && strcmp(aux->name, l->name) == 0) {
            fprintf(stderr, "Erro semântico: Variável '%s' declarado com nome de função (linha %d)\n",
                    node->value, node->lineno);
                hasSemanticError = 1;
        }
        else if (strcmp(node->idType,"void") == 0) {
            fprintf(stderr, "Erro semântico: Variável '%s' declarada com tipo void (linha %d)\n",
                    node->value, node->lineno);
            hasSemanticError = 1;
        }
    }

    // Verifica se o tamanho do array é válido (maior que 0)
    if (node->isArray && node->arraySize <= 0) {
        fprintf(stderr, "Erro semântico: Tamanho do array '%s' deve ser positivo (linha %d)\n",
                node->value, node->lineno);
        hasSemanticError = 1;
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

    // Debug vetor
    // printf("Comparando variáveis na linha %d:\n", node->lineno);
    // printf("Variável esquerda: %s, Tipo: %s, Escopo: %s\n", 
    //        node->left->value, leftType, leftScope);
    // printf("Variável direita: %s, Tipo: %s, Escopo: %s\n", 
    //        node->right->value, rightType, rightScope);

    // Conferir a declaração do lado esquerdo
    if (leftType == NULL) {
        fprintf(stderr, "Erro semântico na linha %d: Variável '%s' sendo usada sem ter sido declarada.\n",
                node->lineno, node->left->value ? node->left->value : "desconhecida");
        hasSemanticError = 1;
        return;
    }

    // Conferir a declaração do lado direito
    if (rightType == NULL) {
        fprintf(stderr, "Erro semântico na linha %d: Expressão ou variável '%s' sendo usada sem ter sido declarada.\n",
                node->lineno, node->right->value ? node->right->value : "desconhecida");
        hasSemanticError = 1;
        return;
    }

    // Checar a compatibilidade de tipos
    if (!checkTypeCompatibility(leftType, rightType)) {
        fprintf(stderr, "Erro semântico na linha %d: Incompatibilidade de tipos na atribuição. "
                       "Variável '%s' é do tipo '%s' mas está recebendo valor do tipo '%s'.\n",
                node->lineno, node->left->value ? node->left->value : "desconhecida",
                leftType, rightType);
        hasSemanticError = 1;
    }
}

// Helper para pegar o tipo do elemento do array
char* getArrayElementType(char* arrayType) {
    // Remove [] do final do tipo
    char* elementType = strdup(arrayType);
    char* bracketPos = strstr(elementType, "[]");
    if (bracketPos) {
        *bracketPos = '\0';
    }
    return elementType;
}

static void checkFunctionCall(ASTNode* node) {
    if (!node->left || !node->left->value) return;

    BucketList l = st_lookup(node->left->value);
    if (!l) {
        if (!errorAlreadyReported(node->left->value, node->lineno)) {
            fprintf(stderr, "Erro semântico: Função '%s' não declarada (linha %d)\n",
                    node->left->value, node->lineno);
            hasSemanticError = 1;
        }
        return;
    }

    if (strcmp(l->idType, "func") != 0) {
        if (!errorAlreadyReported(node->left->value, node->lineno)) {
            fprintf(stderr, "Erro semântico: '%s' não é uma função (linha %d)\n",
                    node->left->value, node->lineno);
            hasSemanticError = 1;
        }
        return;
    }

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
                    fprintf(stderr, "Erro semântico: Argumento '%s' usado na chamada da função '%s' não foi declarado (linha %d)\n",
                            argName, node->left->value, node->lineno);
                    hasSemanticError = 1;
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
            fprintf(stderr, "Erro semântico: Variável '%s' usada em expressão não foi declarada (linha %d)\n",
                    expr->value, expr->lineno);
            hasSemanticError = 1;
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
                TypeInfo* arrayInfo = lookupTypeInfo(node->value);
                if (arrayInfo != NULL) {
                    // Verifica se o tipo é um array
                    if (strstr(arrayInfo->type, "[]") != NULL) {
                        // Verifica se o índice é um inteiro
                        char* indexType = getExpressionType(node->right);
                        if (indexType == NULL || strcmp(indexType, "int") != 0) {
                            fprintf(stderr, "Erro semântico: índice do array deve ser inteiro\n");
                            return NULL;
                        }
                        // Retorna o tipo base do array (sem os colchetes)
                        char* baseType = strdup(arrayInfo->type);
                        char* bracketPos = strstr(baseType, "[]");
                        if (bracketPos) *bracketPos = '\0';
                        return baseType;
                    }
                }
                
                // Fallback para a tabela de símbolos
                BucketList l = st_lookup(node->value);
                if (l && strstr(l->dataType, "[]") != NULL) {
                    char* baseType = strdup(l->dataType);
                    char* bracketPos = strstr(baseType, "[]");
                    if (bracketPos) *bracketPos = '\0';
                    return baseType;
                }
            }

            // Caso normal (não é acesso a array)
            TypeInfo* info = lookupTypeInfo(node->value);
            if (info != NULL) {
                return info->type;
            }

            // Fallback para a tabela de símbolos
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
                    // DEBUG_SEMANTIC("Acesso ao array '%s' (type: %s)\n", 
                    //       node->left->value, l->dataType);
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
                TypeInfo* info = lookupTypeInfo(node->left->value);
                if (info != NULL) {
                    return info->type;
                }

                // Fallback para a tabela de símbolos
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
        fprintf(stderr, "Erro semântico: Função 'main' não declarada.\n");
        hasSemanticError = 1;
    }
}

// Adicionar função para verificar se há pelo menos uma declaração
static void checkAtLeastOneDeclaration(void) {
    if (!hasDeclaration) {
        fprintf(stderr, "Erro semântico: O código deve conter pelo menos uma declaração (função ou variável).\n");
        hasSemanticError = 1;
    }
}

// Adicionar função para verificar se a última função é void main(void)
static void checkLastFunctionIsMain(void) {
    if (lastFunctionNode && strcmp(lastFunctionNode->value, "main") != 0) {
        fprintf(stderr, "Erro semântico: A última declaração de função deve ser 'void main(void)'.\n");
        hasSemanticError = 1;
    }
}

// Adicionar função para liberar a memória da tabela de tipos
void freeTypeTable(void) {
    for (int i = 0; i < TYPE_SIZE; i++) {
        if (typeTable[i]) {
            free(typeTable[i]->type);
            free(typeTable[i]);
            typeTable[i] = NULL;
        }
    }
    clearErrorTracker();
}

static void checkArrayAccess(ASTNode* node) {
    if (!node->value) return;

    BucketList l = st_lookup(node->value);
    if (!l) {
        fprintf(stderr, "Erro semântico: Variável '%s' não declarada (linha %d)\n",
                node->value, node->lineno);
        hasSemanticError = 1;
        return;
    }

    if (!l->isArray) {
        fprintf(stderr, "Erro semântico: Variável '%s' não é um array (linha %d)\n",
                node->value, node->lineno);
        hasSemanticError = 1;
        return;
    }

    // Verifica se o índice é uma expressão inteira
    if (node->right) {
        char* indexType = getExpressionType(node->right);
        if (!indexType || strcmp(indexType, "int") != 0) {
            fprintf(stderr, "Erro semântico: Índice do array deve ser do tipo int (linha %d)\n",
                    node->lineno);
            hasSemanticError = 1;
        }

        // Se o índice é uma constante, verifica os limites
        if (node->right->type == NODE_FACTOR && node->right->value) {
            int index = atoi(node->right->value);
            if (index < 0) {
                fprintf(stderr, "Erro semântico: Índice negativo (%d) no acesso ao array '%s' (linha %d)\n",
                        index, node->value, node->lineno);
                hasSemanticError = 1;
            } 
            else if (l->arraySize > 0 && index >= l->arraySize) {
                fprintf(stderr, "Erro semântico: Índice %d fora dos limites do array '%s[%d]' (linha %d)\n",
                        index, node->value, l->arraySize, node->lineno);
                hasSemanticError = 1;
            }
        }
    }
}