#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição dos tipos de nós
typedef enum {
    NODE_PROGRAM, NODE_DECL, NODE_DECL_LIST, NODE_VAR_DECL, NODE_SPEC_TYPE,
    NODE_FUNC_DECL, NODE_PARAMS, NODE_PARAM_LIST, NODE_PARAM, NODE_COMPOUND_DECL, NODE_LOCAL_DECL, NODE_STATE_LIST,
    NODE_STATEMENT, NODE_EXPR_DECL, NODE_SELECT_DECL, NODE_ITER_DECL, NODE_RETURN_DECL, NODE_EXPR, NODE_VAR, NODE_SIMP_EXPR,
    NODE_RELATIONAL, NODE_SUM_EXPR, NODE_TERM, NODE_MULT, NODE_FACTOR, NODE_ACTIVATION, NODE_ARGS, NODE_ARG_LIST, NODE_FUNC
} NodeType;

// Estrutura base para os nós da AST
typedef struct ASTNode {
    NodeType type;       // Tipo do nó
    struct ASTNode* left;
    struct ASTNode* right;
    char* value;         // Valor para identificadores e constantes
    int lineno;
    char *idType;
    char *scope;  // Novo campo para armazenar o escopo
} ASTNode;

// Funções para criar nós
ASTNode* createNode(NodeType type, ASTNode* left, ASTNode* right, char* value, int lineno, char *idType);
void printAST(ASTNode* root, int depth);

void freeAST(ASTNode* root);

void printASTVertical(ASTNode* root);

void printReducedAST(ASTNode* root, int depth);

#endif
