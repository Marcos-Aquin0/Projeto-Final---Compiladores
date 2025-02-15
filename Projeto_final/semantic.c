#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "semantic.h"

// Forward declarations for all static functions
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

// Global variable definition
int hasSemanticError = 0;
static int hasMainFunction = 0;  // Flag to check if main function exists
static int hasDeclaration = 0;   // Flag to check if at least one declaration exists
static ASTNode* lastFunctionNode = NULL;  // Pointer to the last function node

// Estrutura para armazenar informações de tipo
typedef struct TypeInfo {
    char* type;
    int isArray;
    int arraySize;
} TypeInfo;

// Hash table para tipos
#define TYPE_SIZE 211
static TypeInfo* typeTable[TYPE_SIZE];

// Função de hash (mantida do código original)
static int hash(char* key) {
    int temp = 0;
    int i = 0;
    while (key[i] != '\0') {
        temp = ((temp << 5) + temp) + key[i];
        i++;
    }
    return temp % TYPE_SIZE;
}

// Verifica compatibilidade de tipos
static int checkTypeCompatibility(const char* type1, const char* type2) {
    if (!type1 || !type2) return 0;
    return (strcmp(type1, type2) == 0);
}

// Adicionar funções para gerenciar a tabela de tipos
static void insertTypeInfo(char* name, char* type, int isArray, int arraySize) {
    int index = hash(name);
    TypeInfo* info = (TypeInfo*)malloc(sizeof(TypeInfo));
    info->type = strdup(type);
    info->isArray = isArray;
    info->arraySize = arraySize;
    typeTable[index] = info;
    printf("DEBUG: Inserindo tipo '%s' para '%s' na tabela de tipos (hash: %d)\n", 
           type, name, index);
}

static TypeInfo* lookupTypeInfo(char* name) {
    int index = hash(name);
    return typeTable[index];
}

// Função principal de análise semântica
void semanticAnalysis(ASTNode* node) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_VAR_DECL:
            // Armazena informação de tipo na tabela hash
            insertTypeInfo(node->value, node->idType, 0, 0);
            checkVariableDeclaration(node);
            hasDeclaration = 1;  // Mark that a declaration exists
            break;
            
        case NODE_EXPR:
            if (node->value && strcmp(node->value, "=") == 0) {
                checkAssignment(node);
            }
            break;
            
        case NODE_ACTIVATION:
            checkFunctionCall(node);
            break;

        case NODE_FUNC_DECL:
            lastFunctionNode = node;  // Update the last function node
            if (strcmp(node->value, "main") == 0) {
                hasMainFunction = 1;
            }
            hasDeclaration = 1;  // Mark that a declaration exists
            break;
    }
    
    semanticAnalysis(node->left);
    semanticAnalysis(node->right);

    // Verifica se a função main foi declarada
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
    
    if (l != NULL && strcmp(l->scope, scope) == 0) {
        fprintf(stderr, "Erro semântico: Variável '%s' já declarada no escopo '%s' (linha %d)\n",
                node->value, scope, node->lineno);
        hasSemanticError = 1;
    }
}

static void checkAssignment(ASTNode* node) {
    if (!node->left || !node->right) return;
    
    char* leftType = getExpressionType(node->left);
    char* rightType = getExpressionType(node->right);
    
    // Verifica se a variável do lado esquerdo foi declarada
    if (leftType == NULL) {
        fprintf(stderr, "Erro semântico na linha %d: Variável '%s' sendo usada sem ter sido declarada.\n",
                node->lineno,
                node->left->value ? node->left->value : "desconhecida");
        hasSemanticError = 1;
        return;
    }

    // Verifica se a variável/expressão do lado direito foi declarada
    if (rightType == NULL) {
        fprintf(stderr, "Erro semântico na linha %d: Expressão ou variável '%s' sendo usada sem ter sido declarada.\n",
                node->lineno,
                node->right->value ? node->right->value : "desconhecida");
        hasSemanticError = 1;
        return;
    }

    // Se ambos os tipos existem, verifica compatibilidade
    if (!checkTypeCompatibility(leftType, rightType)) {
        fprintf(stderr, "Erro semântico na linha %d: Incompatibilidade de tipos na atribuição. "
                       "Variável '%s' é do tipo '%s' mas está recebendo valor do tipo '%s'.\n",
                node->lineno,
                node->left->value ? node->left->value : "desconhecida",
                leftType,
                rightType);
        hasSemanticError = 1;
    }
}

static void checkFunctionCall(ASTNode* node) {
    if (!node->left || !node->left->value) return;
    
    BucketList l = st_lookup(node->left->value);
    if (!l) {
        fprintf(stderr, "Erro semântico: Função '%s' não declarada (linha %d)\n",
                node->left->value, node->lineno);
        hasSemanticError = 1;
        return;
    }
    
    if (strcmp(l->idType, "func") != 0) {
        fprintf(stderr, "Erro semântico: '%s' não é uma função (linha %d)\n",
                node->left->value, node->lineno);
        hasSemanticError = 1;
    }
}

static char* getExpressionType(ASTNode* node) {
    if (!node) return NULL;
    
    switch (node->type) {
        case NODE_VAR:
            {
                TypeInfo* info = lookupTypeInfo(node->value);
                if (info) return info->type;
                
                // Fallback para a tabela de símbolos se não encontrar na tabela de tipos
                BucketList l = st_lookup(node->value);
                return l ? l->dataType : NULL;
            }
        
        case NODE_FACTOR:
            if (node->value) return "int";
            break;
            
        case NODE_ACTIVATION:
            if (node->left && node->left->value) {
                TypeInfo* info = lookupTypeInfo(node->left->value);
                if (info) return info->type;
                
                BucketList l = st_lookup(node->left->value);
                return l ? l->dataType : NULL;
            }
            break;

        case NODE_SUM_EXPR:
        case NODE_TERM:
        case NODE_MULT:
            // Operadores matemáticos retornam tipo int
            return "int";
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
    
    printf("DEBUG: Verificando acesso ao array %s (isArray: %d, arraySize: %d)\n", node->value, l->isArray, l->arraySize);
    
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
            if (index < 0 || index >= l->arraySize) {
                fprintf(stderr, "Erro semântico: Índice %d fora dos limites do array '%s[%d]' (linha %d)\n",
                        index, node->value, l->arraySize, node->lineno);
                hasSemanticError = 1;
            }
        }
    }
}

// Adicionar ao globals.h
extern int hasSemanticError;
