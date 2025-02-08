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

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode(ASTNode *t) {
    if (t == NULL || t->value == NULL)
        return;

    char *scope = current_scope(); // Obtém o escopo atual

    switch (t->type) {
        case NODE_VAR_DECL:
            // Inserir variável na tabela de símbolos
            if (st_lookup(t->value) == NULL) { // Símbolo não está na tabela
                st_insert(t->value, t->lineno, location++, scope, "var", t->idType);
            } else { // Símbolo já está na tabela, apenas adiciona o número da linha
                st_insert(t->value, t->lineno, 0, scope, "var", t->idType);
            }
            break;

        case NODE_FUNC_DECL:
            // Inserir função na tabela de símbolos
            if (st_lookup(t->value) == NULL) { // Símbolo não está na tabela
                st_insert(t->value, t->lineno, location++, scope, "func", t->idType);
            } else { // Símbolo já está na tabela, apenas adiciona o número da linha
                st_insert(t->value, t->lineno, 0, scope, "func", t->idType);
            }
            break;

        case NODE_VAR:
            // Inserir uso de variável na tabela de símbolos
            if (st_lookup(t->value) == NULL) { // Símbolo não está na tabela
                st_insert(t->value, t->lineno, location++, scope, "var", t->idType);
            } else { // Símbolo já está na tabela, apenas adiciona o número da linha
                st_insert(t->value, t->lineno, 0, scope, "var", t->idType);
            }
            break;

        case NODE_FUNC:
            // Inserir uso de variável na tabela de símbolos
            if (st_lookup(t->value) == NULL) { // Símbolo não está na tabela
                st_insert(t->value, t->lineno, location++, scope, "func", t->idType);
            } else { // Símbolo já está na tabela, apenas adiciona o número da linha
                st_insert(t->value, t->lineno, 0, scope, "func", t->idType);
            }
            break;

        case NODE_PARAM:
            // Inserir uso de variável na tabela de símbolos
            if (st_lookup(t->value) == NULL) { // Símbolo não está na tabela
                st_insert(t->value, t->lineno, location++, scope, "param", t->idType);
            } else { // Símbolo já está na tabela, apenas adiciona o número da linha
                st_insert(t->value, t->lineno, 0, scope, "param", t->idType);
            }
            break;

        default:
            // Outros tipos de nós não são relevantes para a tabela de símbolos
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