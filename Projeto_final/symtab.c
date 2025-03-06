#include "globals.h"
#include "symtab.h"

#define SIZE 211 //tamanho da tabela hash
#define SHIFT 4 //deslocamento para a função hash

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
        fprintf(stderr, "Erro: Parâmetros inválidos para st_insert. %d\n", lineno);
        return;
    } //verifica se os parâmetros são válidos

    int h = hash(name);
    BucketList l = hashTable[h];
    BucketList prev = NULL;

    // Procura pelo símbolo no escopo atual
    while (l != NULL && (strcmp(name, l->name) != 0 || strcmp(scope, l->scope) != 0)) {
        prev = l;
        l = l->next;
    } //procura pelo símbolo na lista de buckets correspondente ao valor do hash.

    if (l == NULL) {
        // Símbolo não encontrado, insere novo
        l = (BucketList)malloc(sizeof(struct BucketListRec)); //aloca memória
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

        // printf("DEBUG: st_insert: Nova entrada criada para '%s' no escopo '%s'\n", name, scope);
    } else { // Símbolo já existe, atualiza informações
        // printf("DEBUG: st_insert: Atualizando entrada existente para '%s' no escopo '%s'\n", name, scope);
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
    } //Percorre a lista de buckets no índice h da tabela hash, comparando o nome do identificador (name) 
    //com o nome armazenado no bucket (l->name) usando strcmp.
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
    fprintf(listing, "Variable Name  Scope  ID Type  Data Type  Location  Line Numbers\n");
    fprintf(listing, "-------------  -----  -------  ---------  --------  ------------\n");
    for (i = 0; i < SIZE; ++i) {
        if (hashTable[i] != NULL) {
            BucketList l = hashTable[i];
            while (l != NULL) {
                LineList t = l->lines;
                fprintf(listing, "%-14s %-8s %-8s %-10s %-8d ", l->name, l->scope, l->idType, l->dataType, l->memloc);
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
// scope_stack: Declara uma pilha de escopos para gerenciar os escopos aninhados. Inicialmente, a pilha está vazia (NULL).

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

//um contador estático para manter o controle da localização na memória
static int location = 0;

//A função traverse é uma rotina genérica de travessia recursiva da árvore 
// sintática. Ela aplica preProc em pré-ordem e postProc em pós-ordem à árvore apontada por t.
static void traverse(ASTNode *t,
                    void (*preProc)(ASTNode *),
                    void (*postProc)(ASTNode *)) {
    if (t != NULL) {
        // Verifica se o nodo introduz um novo escopo
        if (t->type == NODE_FUNC_DECL) {
            // Para funções, usamos o nome da função como escopo
            if (t->left && t->left->right && t->left->right->value) {
                push_scope(t->left->right->value);
                // printf("DEBUG: Entrando no escopo %s\n", current_scope());
            }
        } else if (t->type == NODE_COMPOUND_DECL) {
            // Para blocos compostos, mantemos o escopo atual
            push_scope(current_scope());
           // printf("DEBUG: Entrando no escopo %s\n", current_scope());
        }
        
        preProc(t);
        traverse(t->left, preProc, postProc);
        traverse(t->right, preProc, postProc);
        
        if (t->type == NODE_FUNC_DECL || t->type == NODE_COMPOUND_DECL) {
            //printf("DEBUG: Saindo do escopo %s\n", current_scope());
            pop_scope();
        }
        
        postProc(t);
    }
}

 //A função nullProc é uma função que não faz nada, 
// usada para gerar travessias apenas em pré-ordem ou pós-ordem a partir de traverse.
static void nullProc(ASTNode *t) {
    if (t == NULL) return;
    else return;
}

//A função insertNode insere informações sobre um nó na tabela de símbolos.
//A função é chamada por traverse para cada nó da árvore sintática.
static void insertNode(ASTNode *t) {
    if (t == NULL || t->value == NULL)
        return;

    char *scope = current_scope();
    if (scope == NULL) {
        scope = "global";
    }

    // Verifica se o símbolo já existe no escopo atual
    BucketList existing = st_lookup_in_scope(t->value, scope);
    
    //printf("DEBUG: insertNode: Processando nó tipo %d para '%s' no escopo '%s'\n", t->type, t->value, scope);

    switch (t->type) {
        case NODE_VAR_DECL:
            if (existing == NULL) {
                //printf("DEBUG: insertNode: Inserindo nova variável '%s'\n", t->value);
                st_insert(t->value, t->lineno, location++, strdup(scope), "var", t->idType, t->isArray, t->arraySize);
            }
            break;

        case NODE_PARAM:
            if (existing == NULL) {
                //printf("DEBUG: insertNode: Inserindo parâmetro '%s'\n", t->value);
                st_insert(t->value, t->lineno, location++, strdup(scope), "param", t->idType, t->isArray, t->arraySize);
            }
            break;

        case NODE_VAR:
            if (existing != NULL) {
                // Se o símbolo já existe, apenas adiciona a linha de uso
                LineList lines = existing->lines;
                while (lines->next != NULL) lines = lines->next;
                lines->next = (LineList)malloc(sizeof(struct LineListRec));
                lines->next->lineno = t->lineno;
                lines->next->next = NULL;
                //printf("DEBUG: insertNode: Atualizando uso de '%s' na linha %d\n", t->value, t->lineno);
            } else if (st_lookup(t->value) == NULL) {
                // Se não existe em nenhum escopo, cria uma nova entrada
                st_insert(t->value, t->lineno, location++, strdup(scope), "var", t->idType, t->isArray, t->arraySize);
            }
            break;

        case NODE_FUNC_DECL:
            if (st_lookup(t->value) == NULL) {
                st_insert(t->value, t->lineno, location++, "global", "func", t->idType, 0, 0);
            }
            break;

        default:
            break;
    }
}

//Construção da tabela de simbolos
void buildSymtab(ASTNode *syntaxTree) {
    push_scope("global");
 
    // Adiciona funções predefinidas ao ambiente global
    st_insert("input", 0, location++, "global", "func", "int", 0, 0);
    st_insert("output", 0, location++, "global", "func", "void", 0, 0);

    traverse(syntaxTree, insertNode, nullProc); //percorre a árvore sintática
    pop_scope();

    printf("\nSymbol table:\n");
    printSymTab(stdout); 
}