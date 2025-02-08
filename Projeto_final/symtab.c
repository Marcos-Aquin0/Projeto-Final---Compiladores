#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#define SIZE 211
#define SHIFT 4

/* the hash table */
static BucketList hashTable[SIZE];

static int hash(char *key) {
    int temp = 0;
    int i = 0;
    while (key[i] != '\0') {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

void st_insert(char *name, int lineno, int loc, char *scope, char *idType, char *dataType) {
    if (name == NULL) {
        //fprintf(stderr, "Erro: tentativa de inserir variável com nome nulo.\n");
        return; // Ou lide com o erro conforme a lógica do programa
    }
    
    int h = hash(name);
    BucketList l = hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL) { // Variável não está na tabela
        l = (BucketList)malloc(sizeof(struct BucketListRec));
        l->name = name;
        l->scope = scope;
        l->idType = idType;
        l->dataType = dataType;
        l->lines = (LineList)malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->lines->next = NULL;
        l->memloc = loc;
        l->next = hashTable[h];
        hashTable[h] = l;
    } else { // Variável já está na tabela, apenas adiciona o número da linha
        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList)malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
}

BucketList st_lookup(char *name) {
    int h = hash(name);
    BucketList l = hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    return l; // Retorna NULL se não encontrar
}

void printSymTab(FILE *listing) {
    int i;
    fprintf(listing, "Variable Name  Scope  ID Type  Data Type  Location   Line Numbers\n");
    fprintf(listing, "-------------  -----  -------  ---------  --------   ------------\n");
    for (i = 0; i < SIZE; ++i) {
        if (hashTable[i] != NULL) {
            BucketList l = hashTable[i];
            while (l != NULL) {
                LineList t = l->lines;
                fprintf(listing, "%-14s %-8s %-8s %-10s %-8d  ", l->name, l->scope, l->idType, l->dataType, l->memloc);
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
    if (scope_stack == NULL) 
        return "global";

    return scope_stack->scope_id;
}