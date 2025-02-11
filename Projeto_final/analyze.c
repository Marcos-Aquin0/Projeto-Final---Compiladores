#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse(ASTNode *t,
                    void (*preProc)(ASTNode *),
                    void (*postProc)(ASTNode *)) {
    if (t != NULL) {
        // Verifica se o nodo introduz um novo escopo
        if (t->type == NODE_FUNC_DECL || t->type == NODE_COMPOUND_DECL) {
            push_scope(t->value);  // Entra no novo escopo
            printf("DEBUG: Entrando no escopo %s\n", current_scope());
        }
        
        preProc(t);
        traverse(t->left, preProc, postProc);
        traverse(t->right, preProc, postProc);
        
        // Verifica se deve sair do escopo atual
        if (t->type == NODE_FUNC_DECL || t->type == NODE_COMPOUND_DECL) {
            printf("DEBUG: Saindo do escopo %s\n", current_scope());
            pop_scope();  // Sai do escopo atual
        }
        
        postProc(t);
    }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(ASTNode *t) {
    if (t == NULL) return;
    else return;
}

static void insertNode(ASTNode *t) {
    if (t == NULL || t->value == NULL)
        return;

    char *scope = current_scope();
    
    if (scope == NULL) {
        scope = "global";
    }

    switch (t->type) {
        case NODE_VAR_DECL:
            if (st_lookup(t->value) == NULL) {
                st_insert(t->value, t->lineno, location++, strdup(scope), "var", t->idType);
                printf("DEBUG: Inserindo variável %s no escopo %s\n", t->value, scope);
            } else {
                BucketList existing = st_lookup(t->value);
                st_insert(t->value, t->lineno, existing->memloc, strdup(scope), "var", t->idType);
                printf("DEBUG: Variável %s já existe no escopo %s, atualizando linha %d\n", t->value, scope, t->lineno);
            }
            break;

        case NODE_FUNC_DECL:
            if (st_lookup(t->value) == NULL) {
                st_insert(t->value, t->lineno, location++, "global", "func", t->idType);
                printf("DEBUG: Inserindo função %s no escopo global\n", t->value);
            } else {
                BucketList existing = st_lookup(t->value);
                st_insert(t->value, t->lineno, existing->memloc, "global", "func", t->idType);
                printf("DEBUG: Função %s já existe no escopo global, atualizando linha %d\n", t->value, t->lineno);
            }
            break;

        case NODE_FUNC:
            // Chamada de função - deve estar declarada no escopo global
            if (st_lookup(t->value) == NULL) {
                st_insert(t->value, t->lineno, location++, "global", "func", t->idType);
                printf("DEBUG: Inserindo chamada de função %s no escopo global\n", t->value);
            } else {
                BucketList existing = st_lookup(t->value);
                st_insert(t->value, t->lineno, existing->memloc, "global", "func", t->idType);
                printf("DEBUG: Chamada de função %s já existe no escopo global, atualizando linha %d\n", t->value, t->lineno);
            }
            break;

        case NODE_VAR:
            if (st_lookup(t->value) == NULL) {
                st_insert(t->value, t->lineno, location++, strdup(scope), "var", t->idType);
                printf("DEBUG: Inserindo variável %s no escopo %s\n", t->value, scope);
            } else {
                BucketList existing = st_lookup(t->value);
                st_insert(t->value, t->lineno, existing->memloc, strdup(existing->scope), "var", t->idType);
                printf("DEBUG: Variável %s já existe no escopo %s, atualizando linha %d\n", t->value, scope, t->lineno);
            }
            break;

        case NODE_PARAM:
            if (st_lookup(t->value) == NULL) {
                st_insert(t->value, t->lineno, location++, strdup(scope), "param", t->idType);
                printf("DEBUG: Inserindo parâmetro %s no escopo %s\n", t->value, scope);
            } else {
                BucketList existing = st_lookup(t->value);
                st_insert(t->value, t->lineno, existing->memloc, strdup(scope), "param", t->idType);
                printf("DEBUG: Parâmetro %s já existe no escopo %s, atualizando linha %d\n", t->value, scope, t->lineno);
            }
            break;

        default:
            break;
    }
}

/* Function buildSymtab constructs the symbol 
 * table by traversing the syntax tree
 */
void buildSymtab(ASTNode *syntaxTree) {
    push_scope("global");
    traverse(syntaxTree, insertNode, nullProc);
    pop_scope();

    printf("\nSymbol table:\n");
    printSymTab(stdout); 
}