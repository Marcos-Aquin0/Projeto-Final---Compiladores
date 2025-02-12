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

// Global variable definition
int hasSemanticError = 0;

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
            break;
            
        case NODE_EXPR:
            if (node->value && strcmp(node->value, "=") == 0) {
                checkAssignment(node);
            }
            break;
            
        case NODE_ACTIVATION:
            checkFunctionCall(node);
            break;
    }
    
    semanticAnalysis(node->left);
    semanticAnalysis(node->right);
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
    
    if (!checkTypeCompatibility(leftType, rightType)) {
        fprintf(stderr, "Erro semântico na linha %d: Incompatibilidade de tipos na atribuição. "
                       "Variável '%s' é do tipo '%s' mas está recebendo valor do tipo '%s'.\n",
                node->lineno,
                node->left->value ? node->left->value : "desconhecida",
                leftType ? leftType : "desconhecido",
                rightType ? rightType : "desconhecido");
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
    }
    
    return NULL;
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

// Adicionar ao globals.h
extern int hasSemanticError;
