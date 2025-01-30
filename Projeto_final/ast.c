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
