#ifndef _CINTER_H_
#define _CINTER_H_

#include "globals.h"
#include "asnt.h"
#include "symtab.h"

// Tipos de operações para o código intermediário
typedef enum {
    OP_ASSIGN,      // Atribuição (=)
    OP_ADD,         // Adição (+)
    OP_SUB,         // Subtração (-)
    OP_MULT,        // Multiplicação (*)
    OP_DIV,         // Divisão (/)
    OP_LT,          // Menor que (<)
    OP_LTE,         // Menor ou igual a (<=)
    OP_GT,          // Maior que (>)
    OP_GTE,         // Maior ou igual a (>=)
    OP_EQ,          // Igual a (==)
    OP_NEQ,         // Diferente de (!=)
    OP_LABEL,       // Rótulo
    OP_JUMP,        // Desvio incondicional
    OP_JUMPFALSE,   // Desvio condicional se falso
    OP_JUMPTRUE,    // Desvio condicional se verdadeiro
    OP_CALL,        // Chamada de função
    OP_PARAM,       // Parâmetro para função
    OP_RETURN,      // Retorno de função
    OP_ARRAY_LOAD,  // Carrega valor de um array
    OP_ARRAY_STORE, // Armazena valor em um array
    OP_ALLOC,       // Aloca memória para variáveis locais
    OP_FUNCTION,    // Definição de função
    OP_END          // Fim de função
} OperationType;

// Estrutura para representar uma quadrupla
typedef struct Quadruple {
    OperationType op;
    char* arg1;
    char* arg2;
    char* result;
    int line;           // Linha da quadrupla
    int sourceLine;     // Nova: linha do código fonte
    struct Quadruple* next;
} Quadruple;

// Estrutura para a tabela de código intermediário
typedef struct {
    Quadruple* head;
    Quadruple* tail;
    int temp_count;
    int label_count;
} IRCode;

// Funções para gerenciamento do código intermediário
void initIRCode(void);
void freeIRCode(void);
char* newTemp(void);
char* newLabel(void);
void genQuad(OperationType op, char* arg1, char* arg2, char* result);
void printIRCode(FILE* listing);

// Funções para geração de código específico
void genExprCode(ASTNode* expr, char* target);
void genAssignCode(ASTNode* assign);
void genIfCode(ASTNode* ifStmt);
void genWhileCode(ASTNode* whileStmt);
void genReturnCode(ASTNode* returnStmt);
void genFunctionCode(ASTNode* funcDecl);
void genCallCode(ASTNode* call, char* target);
void genArrayAccessCode(ASTNode* arrayAccess, char* target);
void genArrayAssignCode(ASTNode* arrayAssign);

// Função principal para geração de código
void ircode_generate(ASTNode* syntaxTree);
void generateIRCode(ASTNode* syntaxTree);
const char* getOpName(OperationType op);
const char* getNodeTypeName(NodeType type);

#endif