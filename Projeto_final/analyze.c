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
        // Antes de aplicar preProc, verificar se o nodo introduz um novo escopo
        if (t->type == NODE_FUNC || t->type == NODE_COMPOUND_DECL) {
            printf("%s \n", t->value);
            push_scope(t->value); // Entrar em um novo escopo
        }

        preProc(t);

        traverse(t->left, preProc, postProc);  // Percorre o filho esquerdo
        traverse(t->right, preProc, postProc); // Percorre o filho direito

        // Após percorrer os filhos, verificar se o escopo deve ser desempilhado
        if (t->type == NODE_FUNC_DECL || t->type == NODE_COMPOUND_DECL) {
            pop_scope(); // Sair do escopo atual
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

    char *scope = current_scope(); // Gets current scope
    
    // Ensure we have a valid scope
    if (scope == NULL) {
        scope = "global";  // Default to global scope if none is set
    }

    switch (t->type) {
        case NODE_VAR_DECL:
            if (st_lookup(t->value) == NULL) {
                // New variable declaration
                st_insert(t->value, t->lineno, location++, strdup(scope), "var", t->idType);
            } else {
                // Additional line number for existing variable
                st_insert(t->value, t->lineno, -1, strdup(scope), "var", t->idType);
            }
            break;

        case NODE_FUNC_DECL:
            if (st_lookup(t->value) == NULL) {
                // New function declaration
                st_insert(t->value, t->lineno, location++, "global", "func", t->idType);
            } else {
                // Additional line number for existing function
                st_insert(t->value, t->lineno, -1, "global", "func", t->idType);
            }
            break;

        case NODE_VAR:
            if (st_lookup(t->value) == NULL) {
                // New variable usage
                st_insert(t->value, t->lineno, location++, strdup(scope), "var", t->idType);
            } else {
                // Additional line number for existing variable
                st_insert(t->value, t->lineno, -1, strdup(scope), "var", t->idType);
            }
            break;

        case NODE_FUNC:
            if (st_lookup(t->value) == NULL) {
                // New function usage
                st_insert(t->value, t->lineno, location++, "global", "func", t->idType);
            } else {
                // Additional line number for existing function
                st_insert(t->value, t->lineno, -1, "global", "func", t->idType);
            }
            break;

        case NODE_PARAM:
            if (st_lookup(t->value) == NULL) {
                // New parameter
                st_insert(t->value, t->lineno, location++, strdup(scope), "param", t->idType);
            } else {
                // Additional line number for existing parameter
                st_insert(t->value, t->lineno, -1, strdup(scope), "param", t->idType);
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