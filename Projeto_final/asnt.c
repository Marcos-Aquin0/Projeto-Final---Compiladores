#include "asnt.h"

// Função para criar nós da árvore
ASTNode* createNode(NodeType type, ASTNode* left, ASTNode* right, char* value, int lineno, char *idType) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode)); //alocação de memória para o nó
    if (!node) { //se não funcionar, imprime um erro
        fprintf(stderr, "Erro ao alocar memória para ASTNode.\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->left = left;
    node->right = right;
    node->value = value ? strdup(value) : NULL; //Se value não for NULL, duplica a string usando strdup e inicializa o campo value
    node->lineno = lineno;
    node->idType = idType;
    node->scope = NULL;  // Inicializa o novo campo
    return node;
}

// Função para liberar memória da árvore
void freeAST(ASTNode* root) {
    if (root == NULL) return;
    freeAST(root->left);
    freeAST(root->right);
    if (root->value) free(root->value);
    free(root);
}

// Função para converter NodeType para string
static const char* nodeTypeToString(NodeType type) {
    switch(type) {
        case NODE_PROGRAM: return "Program";
        case NODE_DECL: return "Decl";
        case NODE_DECL_LIST: return "DeclList";
        case NODE_VAR_DECL: return "VarDecl";
        case NODE_SPEC_TYPE: return "SpecType";
        case NODE_FUNC_DECL: return "FuncDecl";
        case NODE_PARAMS: return "Params";
        case NODE_PARAM_LIST: return "ParamList";
        case NODE_PARAM: return "Param";
        case NODE_COMPOUND_DECL: return "CompoundDecl";
        case NODE_LOCAL_DECL: return "LocalDecl";
        case NODE_STATE_LIST: return "StateList";
        case NODE_STATEMENT: return "Statement";
        case NODE_EXPR_DECL: return "ExprDecl";
        case NODE_SELECT_DECL: return "SelectDecl";
        case NODE_ITER_DECL: return "IterDecl";
        case NODE_RETURN_DECL: return "ReturnDecl";
        case NODE_EXPR: return "Expr";
        case NODE_VAR: return "Var";
        case NODE_SIMP_EXPR: return "SimpExpr";
        case NODE_RELATIONAL: return "Relational";
        case NODE_SUM_EXPR: return "SumExpr";
        case NODE_TERM: return "Term";
        case NODE_MULT: return "Mult";
        case NODE_FACTOR: return "Factor";
        case NODE_ACTIVATION: return "Activation";
        case NODE_ARGS: return "Args";
        case NODE_ARG_LIST: return "ArgList";
        case NODE_FUNC: return "Func";
        case NODE_ARRAY_ACCESS: return "ArrayAccess";
        default: return "UnknownNode";
    }
}

// Função auxiliar para obter o valor do nó
static void getNodeValue(ASTNode* node, char* buffer) {
//ASTNode* node: Ponteiro para o nó da AST cujo valor queremos obter.
//char* buffer: Ponteiro para um buffer de caracteres onde a representação legível do valor do nó será armazenada.
    if (node->value) {
        //sprintf: Usa a função sprintf para formatar a string e armazená-la no buffer.
        sprintf(buffer, "%s (%s)", nodeTypeToString(node->type), node->value);
        //Se node->value não for NULL, formata a string como "<tipo do nó> (<valor do nó>)" e armazena no buffer.
    } else {
        sprintf(buffer, "%s", nodeTypeToString(node->type));
        //Se node->value for NULL, formata a string apenas com o tipo do nó e armazena no buffer.
    }
}

// Função auxiliar recursiva para imprimir a árvore verticalmente com ASCII simples
static void _printASTVerticalSimpleHelper(ASTNode* root, char* prefix, int isLast) {
    if (root == NULL) return;

    // Imprimir o prefixo atual
    printf("%s", prefix);
    // Escolher o conector baseado se é o último filho ou não
    printf(isLast ? "\\-- " : "|-- "); // "\\--" : Representa que o nó atual é o último filho.

    // Imprimir o valor do nó
    char value[100];
    getNodeValue(root, value); //Obtém a representação legível do valor do nó e armazena no buffer value.
    printf("%s\n", value);

    // Preparar o prefixo para os próximos níveis
    char newPrefix[1000];
    strcpy(newPrefix, prefix);
    if (isLast) {
        strcat(newPrefix, "    ");
    } else {
        strcat(newPrefix, "|   ");
    }

    // Contar o número de filhos para determinar quais são os últimos
    int numChildren = 0;
    if (root->left) numChildren++;
    if (root->right) numChildren++;

    // Imprimir os filhos recursivamente
    if (root->left || root->right) {
        if (root->left) {
            // Se houver apenas um filho, ele é o último
            int isLastChild = (numChildren == 1) ? 1 : 0;
            _printASTVerticalSimpleHelper(root->left, newPrefix, isLastChild);
        }
        if (root->right) {
            // O segundo filho (se existir) é sempre o último
            _printASTVerticalSimpleHelper(root->right, newPrefix, 1);
        }
    }
}

// Função pública para imprimir a árvore verticalmente
void printASTVertical(ASTNode* root) {
    printf("arvore de analise sintatica:\n\n");
    
    _printASTVerticalSimpleHelper(root, "", 1);
    //"": Prefixo inicial vazio.
    //1: Indica que o nó raiz é o último nó (não tem irmãos).
}

// Função para verificar se um nó é significativo para a árvore reduzida
int isSignificantNode(ASTNode* node) {
    if (node == NULL) return 0;
    
    // Lista de tipos de nós que são sempre significativos
    switch(node->type) {
        case NODE_PROGRAM:
        case NODE_FUNC_DECL:
        case NODE_VAR_DECL:
        case NODE_SPEC_TYPE:
        case NODE_PARAM:
        case NODE_VAR:
        case NODE_RELATIONAL:
        case NODE_SUM_EXPR:
        case NODE_TERM:
        case NODE_MULT:
        case NODE_FACTOR:
        case NODE_FUNC:
        case NODE_RETURN_DECL:
        case NODE_SELECT_DECL:
        case NODE_ITER_DECL:
        case NODE_ACTIVATION:
            return 1;
        default:
            // Para outros tipos, determina com base no valor
            return (node->value != NULL && strlen(node->value) > 0);
    }
}

// Função auxiliar recursiva para imprimir a árvore reduzida verticalmente
// Função para procurar o próximo nó significativo em uma subárvore
static ASTNode* findNextSignificantNode(ASTNode* node) {
    if (node == NULL) return NULL;
    
    // Verificar se o nó atual é significativo
    if (isSignificantNode(node)) return node;
    
    // Verificar na subárvore esquerda
    ASTNode* left = findNextSignificantNode(node->left);
    if (left) return left;
    
    // Verificar na subárvore direita
    return findNextSignificantNode(node->right);
}


// Função auxiliar recursiva para imprimir a árvore reduzida verticalmente
static void _printReducedASTVerticalHelper(ASTNode* root, char* prefix, int isLast) {
    if (root == NULL) return;
    
    // Verifica se o nó é significativo
    int significant = isSignificantNode(root);
    
    // Se o nó não é significativo, mas tem filhos significativos, devemos pular diretamente para eles
    if (!significant) {
        ASTNode* leftSignificant = findNextSignificantNode(root->left);
        ASTNode* rightSignificant = findNextSignificantNode(root->right);
        
        if (leftSignificant) {
            _printReducedASTVerticalHelper(root->left, prefix, rightSignificant ? 0 : isLast);
        }
        
        if (rightSignificant) {
            _printReducedASTVerticalHelper(root->right, prefix, isLast);
        }
        
        return;
    }
    
    // Se o nó for significativo, imprime-o
    printf("%s", prefix);
    printf(isLast ? "\\-- " : "|-- ");
    
    char value[100];
    getNodeValue(root, value);
    printf("%s\n", value);
    
    // Prepara o prefixo para os próximos níveis
    char newPrefix[1000];
    strcpy(newPrefix, prefix);
    
    if (isLast) {
        strcat(newPrefix, "    ");
    } else {
        strcat(newPrefix, "|   ");
    }
    
    // Contar os filhos significativos
    int numSignificantChildren = 0;
    if (root->left && findNextSignificantNode(root->left)) numSignificantChildren++;
    if (root->right && findNextSignificantNode(root->right)) numSignificantChildren++;
    
    // Imprimir os filhos recursivamente
    if (root->left) {
        int isLastChild = (numSignificantChildren == 1) ? 1 : 0;
        _printReducedASTVerticalHelper(root->left, newPrefix, isLastChild);
    }
    
    if (root->right) {
        _printReducedASTVerticalHelper(root->right, newPrefix, 1);
    }
}

// Função pública para imprimir a árvore reduzida verticalmente
void printReducedAST(ASTNode* root, int depth) {
    printf("arvore de analise sintatica reduzida:\n\n");
    _printReducedASTVerticalHelper(root, "", 1);
}