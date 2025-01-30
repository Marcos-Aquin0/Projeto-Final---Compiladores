#include "ast.h"

ASTNode* createNode(NodeType type, ASTNode* left, ASTNode* right, char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->left = left;
    node->right = right;
    node->value = value ? strdup(value) : NULL;
    return node;
}

void printAST(ASTNode* root, int depth) {
    if (root == NULL) return;
    for (int i = 0; i < depth; i++) printf("  ");
    
    // Nome do nó baseado no tipo
    switch (root->type) {
        case NODE_PROGRAM: printf("Program\n"); break;
        case NODE_DECL: printf("Decl (%s)\n", root->value); break;
        case NODE_DECL_LIST: printf("DeclList\n"); break;
        case NODE_VAR_DECL: printf("VarDecl (%s)\n", root->value); break;
        case NODE_SPEC_TYPE: printf("SpecType (%s)\n", root->value); break;
        case NODE_FUNC_DECL: printf("FuncDecl (%s)\n", root->value); break;
        case NODE_PARAMS: printf("Params\n"); break;
        case NODE_PARAM_LIST: printf("ParamList\n"); break;
        case NODE_PARAM: printf("Param (%s)\n", root->value); break;
        case NODE_COMPOUND_DECL: printf("CompoundDecl\n"); break;
        case NODE_LOCAL_DECL: printf("LocalDecl\n"); break;
        case NODE_STATE_LIST: printf("StateList\n"); break;
        case NODE_STATEMENT: printf("Statement\n"); break;
        case NODE_EXPR_DECL: printf("ExprDecl\n"); break;
        case NODE_SELECT_DECL: printf("SelectDecl\n"); break;
        case NODE_ITER_DECL: printf("IterDecl\n"); break;
        case NODE_RETURN_DECL: printf("ReturnDecl\n"); break;
        case NODE_EXPR: printf("Expr\n"); break;
        case NODE_VAR: printf("Var (%s)\n", root->value); break;
        case NODE_SIMP_EXPR: printf("SimpExpr\n"); break;
        case NODE_RELATIONAL: printf("Relational\n"); break;
        case NODE_SUM_EXPR: printf("SumExpr\n"); break;
        case NODE_TERM: printf("Term\n"); break;
        case NODE_MULT: printf("Mult\n"); break;
        case NODE_FACTOR: printf("Factor\n"); break;
        case NODE_ACTIVATION: printf("Activation (%s)\n", root->value); break;
        case NODE_ARGS: printf("Args\n"); break;
        case NODE_ARG_LIST: printf("ArgList\n"); break;
        default: printf("Unknown Node\n"); break;
    }

    printAST(root->left, depth + 1);
    printAST(root->right, depth + 1);
}

void freeAST(ASTNode* root) {
    if (root == NULL) return;
    
    // Libera recursivamente as subárvores
    freeAST(root->left);
    freeAST(root->right);
    
    // Libera o valor se existir
    if (root->value) free(root->value);
    
    // Libera o nó
    free(root);
}


/*=========================================*/
#define MAX_HEIGHT 1000
#define MAX_WIDTH 1000

static void getNodeValue(ASTNode* node, char* buffer) {
    switch(node->type) {
        case NODE_RELATIONAL:
            sprintf(buffer, "%s", node->value ? node->value : "rel");
            break;
        case NODE_VAR:
            sprintf(buffer, "%s", node->value ? node->value : "var");
            break;
        case NODE_FACTOR:
            sprintf(buffer, "%s", node->value ? node->value : "factor");
            break;
        case NODE_ITER_DECL:
            sprintf(buffer, "while");
            break;
        case NODE_MULT:
            sprintf(buffer, "%s", node->value ? node->value : "*");
            break;
        case NODE_SUM_EXPR:
            sprintf(buffer, "%s", node->value ? node->value : "+");
            break;
        default:
            sprintf(buffer, "%s", node->value ? node->value : "node");
    }
}

static void printVerticalBranches(int x, int y, int* childrenX, int numChildren) {
    // Imprime linha vertical a partir do nó pai
    for(int i = 1; i <= 2; i++) {
        printf("\033[%d;%dH│", y + i, x + 1);
    }
    
    // Imprime linhas horizontais para os filhos
    if(numChildren > 0) {
        printf("\033[%d;%dH├", y + 2, x + 1);
        for(int i = 1; i < childrenX[0] - x; i++) {
            printf("─");
        }
        
        // Para os filhos do meio
        for(int i = 1; i < numChildren - 1; i++) {
            printf("\033[%d;%dH┤├", y + 2, childrenX[i-1] + 1);
            for(int j = 1; j < childrenX[i+1] - childrenX[i]; j++) {
                printf("─");
            }
        }
        
        // Para o último filho
        if(numChildren > 1) {
            printf("\033[%d;%dH┤", y + 2, childrenX[numChildren-2] + 1);
            for(int i = 1; i < childrenX[numChildren-1] - childrenX[numChildren-2]; i++) {
                printf("─");
            }
        }
    }
}

static void _printASTVerticalHelper(ASTNode* root, int x, int y, int width) {
    if (root == NULL) return;

    char value[50];
    getNodeValue(root, value);
    
    // Imprime o valor do nó na posição (x,y)
    printf("\033[%d;%dH%s", y, x + 1, value);

    // Se tem filhos, calcula suas posições
    if (root->left || root->right) {
        int numChildren = 0;
        int childrenX[2];  // Máximo de 2 filhos
        int nextY = y + 3;  // 3 linhas abaixo para as conexões
        int childWidth = width / 2;
        
        if (root->left) {
            childrenX[numChildren++] = x - childWidth/2;
        }
        if (root->right) {
            childrenX[numChildren++] = x + childWidth/2;
        }

        // Imprime as conexões
        printVerticalBranches(x, y, childrenX, numChildren);

        // Recursivamente imprime os filhos
        if (root->left) {
            _printASTVerticalHelper(root->left, childrenX[0], nextY, childWidth);
        }
        if (root->right) {
            _printASTVerticalHelper(root->right, childrenX[1], nextY, childWidth);
        }
    }
}

void printASTVertical(ASTNode* root) {
    if (root == NULL) return;
    
    // Limpa a tela
    printf("\033[2J\033[H");
    printf("Árvore Vertical:\n\n");
    
    // Começa a impressão a partir do centro da tela
    _printASTVerticalHelper(root, 40, 3, 40);
    
    // Move o cursor para depois da árvore
    printf("\033[40;1H\n");
}
