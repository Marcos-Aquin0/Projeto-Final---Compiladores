#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#define SIZE 211
#define SHIFT 4

/* the hash table */
static BucketList hashTable[SIZE];

static int hash(char *key) {
    unsigned long hash = 5381;  // Valor inicial para o algoritmo DJB2
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash % SIZE;
}

void st_insert(char *name, int lineno, int loc, char *scope, char *idType, char *dataType, int isArray, int arraySize) {
    if (name == NULL || scope == NULL || idType == NULL || dataType == NULL) {
        fprintf(stderr, "Erro: Parâmetros inválidos para st_insert.\n");
        return;
    }

    int h = hash(name);
    BucketList l = hashTable[h];
    BucketList prev = NULL;

    // Procura pelo símbolo no escopo atual
    while (l != NULL && (strcmp(name, l->name) != 0 || strcmp(scope, l->scope) != 0)) {
        prev = l;
        l = l->next;
    }

    if (l == NULL) {
        // Símbolo não encontrado, insere novo
        l = (BucketList)malloc(sizeof(struct BucketListRec));
        if (l == NULL) {
            fprintf(stderr, "Erro: Falha ao alocar memória para BucketList.\n");
            exit(EXIT_FAILURE);
        }

        l->name = strdup(name);
        l->scope = strdup(scope);
        l->idType = strdup(idType);
        l->dataType = strdup(dataType);
        l->lines = (LineList)malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->lines->next = NULL;
        l->memloc = loc;
        l->isArray = isArray;
        l->arraySize = arraySize;
        l->next = hashTable[h];
        hashTable[h] = l;

        printf("DEBUG: st_insert: Nova entrada criada para '%s' no escopo '%s'\n", name, scope);
    } else {
        // Símbolo já existe, atualiza informações
        printf("DEBUG: st_insert: Atualizando entrada existente para '%s' no escopo '%s'\n", name, scope);
        l->isArray = isArray;
        l->arraySize = arraySize;

        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList)malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
}

BucketList st_lookup(char *name) {
    if (name == NULL) {
        return NULL;
    }

    int h = hash(name);
    BucketList l = hashTable[h];
    while (l != NULL && strcmp(name, l->name) != 0) {
        l = l->next;
    }
    return l;
}

BucketList st_lookup_in_scope(char *name, char *scope) {
    if (name == NULL || scope == NULL) {
        return NULL;
    }

    int h = hash(name);
    BucketList l = hashTable[h];
    while (l != NULL && (strcmp(name, l->name) != 0 || strcmp(scope, l->scope) != 0)) {
        l = l->next;
    }
    return l;
}

void printSymTab(FILE *listing) {
    int i;
    fprintf(listing, "Variable Name  Scope  ID Type  Data Type  Location   Is Array  Array Size  Line Numbers\n");
    fprintf(listing, "-------------  -----  -------  ---------  --------   --------  ----------  ------------\n");
    for (i = 0; i < SIZE; ++i) {
        if (hashTable[i] != NULL) {
            BucketList l = hashTable[i];
            while (l != NULL) {
                LineList t = l->lines;
                fprintf(listing, "%-14s %-8s %-8s %-10s %-8d  %-8d  %-10d  ", l->name, l->scope, l->idType, l->dataType, l->memloc, l->isArray, l->arraySize);
                while (t != NULL) {
                    fprintf(listing, "%4d ", t->lineno);
                    t = t->next;
                }
                fprintf(listing, "\n");
                l = l->next;
            }
        }
    }
}

static ScopeNode *scope_stack = NULL;

// Função para empilhar um novo escopo
void push_scope(char *scope_name) {
    ScopeNode *node = (ScopeNode*)malloc(sizeof(ScopeNode));

    if (node == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para ScopeNode.\n");
        exit(EXIT_FAILURE);
    }

    node->scope_id = scope_name ? strdup(scope_name) : NULL; 
    node->next = scope_stack;
    scope_stack = node;
}

// Função para desempilhar o escopo atual
void pop_scope() {
    if (scope_stack == NULL) {
        fprintf(stderr, "Erro: Tentativa de desempilhar escopo de pilha vazia.\n");
        return;
    }

    ScopeNode *temp = scope_stack;
    scope_stack = scope_stack->next;

    if (temp->scope_id) {
        free(temp->scope_id); // Libera a memória alocada para o scope_id
    }
    free(temp); // Libera o nó
}

// Função para obter o escopo atual
char* current_scope() {
    if (scope_stack == NULL) {
        return "global";
    }

    if (scope_stack->scope_id == NULL) {
        fprintf(stderr, "Erro: scope_id é NULL no escopo atual.\n");
        return "global";
    }

    return scope_stack->scope_id;
}