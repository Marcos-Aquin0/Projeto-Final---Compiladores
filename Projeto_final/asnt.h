#ifndef AST_H
#define AST_H

#include "globals.h"

// Definição dos tipos de nós
typedef enum {
    NODE_PROGRAM, NODE_DECL, NODE_DECL_LIST, NODE_VAR_DECL, NODE_SPEC_TYPE,
    NODE_FUNC_DECL, NODE_PARAMS, NODE_PARAM_LIST, NODE_PARAM, NODE_COMPOUND_DECL, NODE_LOCAL_DECL, NODE_STATE_LIST,
    NODE_STATEMENT, NODE_EXPR_DECL, NODE_SELECT_DECL, NODE_ITER_DECL, NODE_RETURN_DECL, NODE_EXPR, NODE_VAR, NODE_SIMP_EXPR,
    NODE_RELATIONAL, NODE_SUM_EXPR, NODE_TERM, NODE_MULT, NODE_FACTOR, NODE_ACTIVATION, NODE_ARGS, NODE_ARG_LIST, NODE_FUNC,
    NODE_ARRAY_ACCESS  // Add this new type
} NodeType;

// Estrutura base para os nós da AST
typedef struct ASTNode {
    NodeType type;       // Tipo do nó
    struct ASTNode* left; //árvore de estrutura binária
    struct ASTNode* right;
    char* value;         // String que armazena o valor do nó. Pode ser usado para armazenar identificadores, literais ou outros valores.
    int lineno;          // Localização do nó no código fonte
    char *idType;        // String que indica o tipo do identificador. Pode ser "func", "var" ou "param"
    char *scope;         // String que armazena o escopo onde o identificador foi declarado.
    int isArray;         // Indica se é um vetor
    int arraySize;       // Tamanho do vetor
} ASTNode;

// Funções para criar nós
ASTNode* createNode(NodeType type, ASTNode* left, ASTNode* right, char* value, int lineno, char *idType);
// Parâmetros: Tipo do nó, ponteiros para os filhos esquerdo e direito, valor do nó, número da linha e tipo do identificador.
// Retorna um ponteiro para o nó criado.


//imprime a árvore de análise sintática
// void printAST(ASTNode* root, int depth);
//Parâmetros: Ponteiro para a raiz da AST e profundidade atual.


//liberar a memória utilizada para a árvore
void freeAST(ASTNode* root);
//imprimir a árvore completa
void printASTVertical(ASTNode* root);

//imprimir a árvore reduzida (ainda em desenvolvimento)
void printReducedAST(ASTNode* root, int depth);

#endif
