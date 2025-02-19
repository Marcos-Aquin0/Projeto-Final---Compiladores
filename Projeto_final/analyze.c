#include "globals.h"
#include "symtab.h"
#include "analyze.h"

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