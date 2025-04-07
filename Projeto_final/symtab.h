#ifndef SYMTAB_H
#define SYMTAB_H

#include "asnt.h"
//definição da função de construção da tabela de símbolos
void buildSymtab(ASTNode *syntaxTree);

//A estrutura LineListRec é usada para armazenar uma lista de números de 
//linha onde um identificador aparece no código fonte.
typedef struct LineListRec {
    int lineno;
    struct LineListRec *next; //next: Ponteiro para o próximo item na lista, permitindo a criação de uma lista encadeada de números de linha.
} *LineList;

typedef struct ParamInfoRec {
    char *paramType;
    int isArray;
    struct ParamInfoRec *next;
} *ParamInfo;

//A estrutura ScopeNode é usada para representar um escopo na tabela de símbolos.
typedef struct ScopeNode {
    char *scope_id;
    struct ScopeNode *next;
} ScopeNode;

//A estrutura BucketListRec é usada para armazenar informações sobre um identificador 
// na tabela de símbolos. Cada identificador tem um nome, escopo, tipo, localização na memória, etc
typedef struct BucketListRec {
    char *name;          // Nome do ID
    char *scope;         // Escopo do ID
    char *idType;        // Tipo do ID (variável ou função)
    char *dataType;      // Tipo do dado (int ou void)
    LineList lines;      // Lista de números de linha
    int memloc;          // Localização na memória
    struct BucketListRec *next;
    int isArray;         // Indica se é um vetor
    int arraySize;       // Tamanho do vetor
    int paramCount;      // Número de parâmetros (para funções)
    ParamInfo params;    // Lista de informações sobre parâmetros (para funções)
} *BucketList;

void push_scope(char *scope_name); //Empilha um novo escopo na pilha de escopos.
void pop_scope(); //Desempilha o escopo atual da pilha de escopos.
char* current_scope(); //Retorna o nome do escopo atual.

BucketList st_lookup(char *name); //Procura um identificador na tabela de símbolos.
//name: Nome do identificador a ser procurado.
//Retorno: Ponteiro para o bucket do identificador, ou NULL se não encontrado.

BucketList st_lookup_in_scope(char *name, char *scope); //Procura um identificador em um escopo específico na tabela de símbolos.
//name: Nome do identificador a ser procurado.
//scope: Nome do escopo onde o identificador deve ser procurado.
//Retorno: Ponteiro para o bucket do identificador, ou NULL se não encontrado.

/* Busca um símbolo em todos os escopos acessíveis (atual e global) */
BucketList st_lookup_all_scopes(char *name, char *scope);

void st_insert(char *name, int lineno, int loc, char *scope, char *idType, char *dataType, int isArray, int arraySize);
//Insere um novo identificador na tabela de símbolos.

void printSymTab(FILE *listing); //imprime a tabela de símbolos.

void process_func_args(ASTNode *argsNode, char *funcName, int lineno);

#endif