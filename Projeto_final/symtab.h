#ifndef SYMTAB_H
#define SYMTAB_H

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec {
    int lineno;
    struct LineListRec *next;
} *LineList;

typedef struct ScopeNode {
    char *scope_id;
    struct ScopeNode *next;
} ScopeNode;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
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
} *BucketList;

void push_scope(char *scope_name);
void pop_scope();
char* current_scope();

BucketList st_lookup(char *name);
BucketList st_lookup_in_scope(char *name, char *scope);
void st_insert(char *name, int lineno, int loc, char *scope, char *idType, char *dataType, int isArray, int arraySize);
void printSymTab(FILE *listing);

#endif