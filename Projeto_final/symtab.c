#include "globals.h"
#include "symtab.h"
#include "semantic.h"

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
        DEBUG_SYMTAB("Erro: Parâmetros inválidos para st_insert. %d\n", lineno);
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
            DEBUG_SYMTAB("Erro: Falha ao alocar memória para BucketList.\n");
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
        l->paramCount = 0;
        l->params = NULL;
        l->next = hashTable[h];
        hashTable[h] = l;

        DEBUG_SYMTAB("st_insert: Nova entrada criada para '%s' no escopo '%s'", name, scope);
    } else { // Símbolo já existe, atualiza informações
        DEBUG_SYMTAB("st_insert: Atualizando entrada existente para '%s' no escopo '%s'", name, scope);
        l->isArray = isArray;
        l->arraySize = arraySize;

        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList)malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
}

// Função para adicionar informação de um parâmetro a uma função
void add_param_info(char *func_name, char *param_type, int is_array) {
    // First check if this is a function call in the current scope
    char *current = current_scope();
    BucketList func = st_lookup_in_scope(func_name, current);
    
    // If not found in current scope or if we're in global scope, look in global
    if (func == NULL || strcmp(current, "global") == 0) {
        func = st_lookup_in_scope(func_name, "global");
    }
    
    if (func == NULL) {
        DEBUG_SYMTAB("Erro: Tentativa de adicionar parâmetro à função '%s' que não existe.", func_name);
        return;
    }

    ParamInfo param = (ParamInfo)malloc(sizeof(struct ParamInfoRec));
    if (param == NULL) {
        DEBUG_SYMTAB("Erro: Falha ao alocar memória para ParamInfo.\n");
        exit(EXIT_FAILURE);
    }

    param->paramType = strdup(param_type);
    param->isArray = is_array;
    param->next = NULL;

    // Adiciona o parâmetro ao final da lista
    if (func->params == NULL) {
        func->params = param;
    } else {
        ParamInfo current = func->params;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = param;
    }

    func->paramCount++;
    DEBUG_SYMTAB("Adicionado parâmetro %d à função '%s' no escopo '%s': tipo '%s', is_array=%d",
                func->paramCount, func_name, func->scope, param_type, is_array);
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

BucketList st_lookup_all_scopes(char *name, char *scope) {
    if (name == NULL || scope == NULL) {
        return NULL;
    }

    // Primeiro, procura no escopo atual
    BucketList l = st_lookup_in_scope(name, scope);
    
    // Se não encontrar no escopo atual e não estiver no escopo global,
    // procura no escopo global
    if (l == NULL && strcmp(scope, "global") != 0) {
        l = st_lookup_in_scope(name, "global");
    }
    
    return l;
}

void printSymTab(FILE *listing) {
    int i;
    FILE* outfile = fopen("Output/symtab.txt", "w");
    
    fprintf(outfile, "Variable Name  Scope       ID Type  Data Type  Location  Line Numbers\n");
    fprintf(outfile, "-------------  ----------  -------  ---------  --------  ------------\n");
    for (i = 0; i < SIZE; ++i) {
        if (hashTable[i] != NULL) {
            BucketList l = hashTable[i];
            while (l != NULL) {
                LineList t = l->lines;
                fprintf(outfile, "%-14s %-12s %-8s %-10s %-9d ", l->name, l->scope, l->idType, l->dataType, l->memloc);
                
                // // Mostrar informações de parâmetros para funções
                // if (strcmp(l->idType, "func") == 0) {
                //     fprintf(outfile, "%d params: ", l->paramCount);
                //     ParamInfo param = l->params;
                //     while (param != NULL) {
                //         fprintf(outfile, "%s%s ", param->paramType, param->isArray ? "[]" : "");
                //         param = param->next;
                //         if (param != NULL) fprintf(outfile, ", ");
                //     }
                //     fprintf(outfile, "   ");
                // } else {
                //     fprintf(outfile, "%-17s ", l->isArray ? "[array]" : "");
                // }
                
                // Para funções no escopo global, a primeira linha é a declaração
                if (strcmp(l->idType, "func") == 0 && strcmp(l->scope, "global") == 0) {
                    fprintf(outfile, "%4d(D) ", t->lineno);
                    t = t->next;
                    // As linhas restantes são chamadas
                    while (t != NULL) {
                        fprintf(outfile, "%4d ", t->lineno);
                        t = t->next;
                    }
                } 
                // Para funções em outros escopos, todas são chamadas
                else if (strcmp(l->idType, "func") == 0) {
                    while (t != NULL) {
                        fprintf(outfile, "%4d(C) ", t->lineno);
                        t = t->next;
                    }
                }
                else {
                    // Para variáveis, mostrar todas as linhas normalmente
                    while (t != NULL) {
                        fprintf(outfile, "%4d ", t->lineno);
                        t = t->next;
                    }
                }
                
                fprintf(outfile, "\n");
                l = l->next;
            }
        }
    }
    printf("Tabela de símbolos salvo em 'Projeto_final/Output/symtab.txt'\n");
    fclose(outfile);
}

static ScopeNode *scope_stack = NULL; // scope_stack: Declara uma pilha de escopos para gerenciar os escopos aninhados. Inicialmente, a pilha está vazia (NULL).

// Função para empilhar um novo escopo
void push_scope(char *scope_name) {
    ScopeNode *node = (ScopeNode*)malloc(sizeof(ScopeNode));

    if (node == NULL) {
        DEBUG_SYMTAB("Erro: Falha ao alocar memória para ScopeNode.\n");
        exit(EXIT_FAILURE);
    }

    node->scope_id = scope_name ? strdup(scope_name) : NULL; 
    node->next = scope_stack;
    scope_stack = node;
}

// Função para desempilhar o escopo atual
void pop_scope() {
    if (scope_stack == NULL) {
        DEBUG_SYMTAB("Erro: Tentativa de desempilhar escopo de pilha vazia.\n");
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
        DEBUG_SYMTAB("Erro: scope_id é NULL no escopo atual.\n");
        return "global";
    }

    return scope_stack->scope_id;
}

//processa os parâmetros da chamada de função
void processArguments_Func(ASTNode* argNode, char* funcName, int* argCount) {
    if (argNode == NULL || funcName == NULL || argCount == NULL) {
        DEBUG_SYMTAB("  > processArguments_Func: NULL arguments");
        return;
    }
    
    if (argNode->type == NODE_ARGS && argNode->left == NULL && argNode->right == NULL) {
        DEBUG_SYMTAB("  > processArguments_Func: Empty NODE_ARGS for function '%s'", funcName);
        
        return;
    }
    
    DEBUG_SYMTAB("  > processArguments_Func: nó tipo %s", getNodeTypeName(argNode->type));
    
    char* scope = current_scope();
    BucketList funcEntry = st_lookup_in_scope(funcName, scope);
    if (!funcEntry) {
        DEBUG_SYMTAB("  > Function '%s' not found in current scope '%s'", funcName, scope);
        return;
    }
    
    if (argNode->left != NULL) {
        
        if (argNode->left->type == NODE_ACTIVATION) {
            char* calledFuncName = argNode->left->left ? argNode->left->left->value : "unknown";
            DEBUG_SYMTAB("    Arg %d é uma chamada de função: %s", *argCount + 1, calledFuncName);
            
            BucketList calledFunc = st_lookup_in_scope(calledFuncName, "global");
            if (calledFunc) {
                
                add_param_info(funcName, calledFunc->dataType, 0); 
                (*argCount)++;
            }
        } 
        
        else if (argNode->left->type == NODE_ARRAY_ACCESS) {
            DEBUG_SYMTAB("    Arg %d é um acesso de vetor", *argCount + 1);
            
            
            add_param_info(funcName, "int", 0); 
            (*argCount)++;
        }
        
        else if (argNode->left->type == NODE_EXPR || 
                 argNode->left->type == NODE_SUM_EXPR || 
                 argNode->left->type == NODE_TERM ||
                 argNode->left->type == NODE_RELATIONAL) {
            DEBUG_SYMTAB("    Arg %d é uma expressão complexa tipo: %s", 
                    *argCount + 1, getNodeTypeName(argNode->left->type));
            
            
            add_param_info(funcName, "int", 0);
            (*argCount)++;
        }
        
        else if (argNode->left->type == NODE_VAR) {
            char* varName = argNode->left->value;
            DEBUG_SYMTAB("    Arg %d é uma variável: %s", *argCount + 1, varName);
            
            int isArray = 0;
            if (varName) {
                BucketList varEntry = st_lookup_all_scopes(varName, scope);
                if (varEntry) {
                    isArray = varEntry->isArray;
                }
            }
            
            add_param_info(funcName, "int", isArray);
            (*argCount)++;
        }
        
        else {
            processArguments_Func(argNode->left, funcName, argCount);
        }
    }
    
    if (argNode->type != NODE_ARG_LIST && argNode->type != NODE_ARGS) {
        if (argNode->type == NODE_ACTIVATION) {
            char* calledFuncName = argNode->left ? argNode->left->value : "unknown";
            
            BucketList calledFunc = st_lookup_in_scope(calledFuncName, "global");
            if (calledFunc) {
                
                add_param_info(funcName, calledFunc->dataType, 0); 
                (*argCount)++;
            }
        }
        // acesso ao array
        else if (argNode->type == NODE_ARRAY_ACCESS) {
            DEBUG_SYMTAB("    Arg %d é um acesso de vetor", *argCount + 1);
            
            
            add_param_info(funcName, "int", 0); 
            (*argCount)++;
        }
        // expressao
        else if (argNode->type == NODE_EXPR || 
                argNode->type == NODE_SUM_EXPR || 
                argNode->type == NODE_TERM ||
                argNode->type == NODE_RELATIONAL) {
            
            add_param_info(funcName, "int", 0);
            (*argCount)++;
        }
        // variavel ou fator
        else if (argNode->type == NODE_VAR || argNode->type == NODE_FACTOR) {
            char* varName = argNode->value;
            
            int isArray = 0;
            if (varName) {
                BucketList varEntry = st_lookup_all_scopes(varName, scope);
                if (varEntry) {
                    isArray = varEntry->isArray;
                }
            }
            
            add_param_info(funcName, "int", isArray);
            (*argCount)++;
        }
    }
    
    if (argNode->right != NULL) {
        
        if (argNode->right->type == NODE_ACTIVATION) {
            char* calledFuncName = argNode->right->left ? argNode->right->left->value : "unknown";
            
            BucketList calledFunc = st_lookup_in_scope(calledFuncName, "global");
            if (calledFunc) {
                
                add_param_info(funcName, calledFunc->dataType, 0); 
                (*argCount)++;
            }
        }
        
        else if (argNode->right->type == NODE_ARRAY_ACCESS) {
            DEBUG_SYMTAB("    Arg %d é um acesso de vetor", *argCount + 1);
            
           
            add_param_info(funcName, "int", 0);
            (*argCount)++;
        }
        
        else if (argNode->right->type == NODE_EXPR || 
                 argNode->right->type == NODE_SUM_EXPR || 
                 argNode->right->type == NODE_TERM ||
                 argNode->right->type == NODE_RELATIONAL) {
            
            
            add_param_info(funcName, "int", 0);
            (*argCount)++;
        }
        
        else if (argNode->right->type == NODE_VAR || argNode->right->type == NODE_FACTOR) {
            char* varName = argNode->right->value;
            
            
            int isArray = 0;
            if (varName) {
                BucketList varEntry = st_lookup_all_scopes(varName, scope);
                if (varEntry) {
                    isArray = varEntry->isArray;
                }
            }
            
            add_param_info(funcName, "int", isArray);
            (*argCount)++;
        }
        
        else {
            processArguments_Func(argNode->right, funcName, argCount);
        }
    }
}

//um contador estático para manter o controle da localização na memória
static int location = 0;

//A função traverse é uma rotina genérica de travessia recursiva da árvore 
// sintática. Ela aplica preProc em pré-ordem e postProc em pós-ordem à árvore apontada por t.
static void traverse(ASTNode *t,
                    void (*preProc)(ASTNode *),
                    void (*postProc)(ASTNode *)) {
    if (t != NULL) {
        // Verifica se o nó introduz um novo escopo
        if (t->type == NODE_FUNC_DECL) {
            // Para funções, usamos o nome da função como escopo
            if (t->left && t->left->right && t->left->right->value) {
                push_scope(t->left->right->value);
                DEBUG_SYMTAB("Entrando no escopo %s", current_scope());
            }
        } else if (t->type == NODE_COMPOUND_DECL) {
            // Para blocos compostos, mantemos o escopo atual
            push_scope(current_scope());
            DEBUG_SYMTAB("Entrando no escopo %s", current_scope());
        }
        
        preProc(t);
        
        // Caso especial para NODE_ACTIVATION para garantir que ele seja processado antes de seus filhos
        if (t->type == NODE_ACTIVATION) {
            if (t->left) {
                char *funcName = t->left->value;
                char *scope = current_scope();
                
                if (funcName) {
                    DEBUG_SYMTAB("Processando chamada de função '%s' no escopo '%s'", funcName, scope);
                    
                    // Procuramos a função na tabela de símbolos global
                    BucketList func = st_lookup_in_scope(funcName, "global");
                    
                    if (func) {
                        // Registra a chamada de função no escopo atual se não for global
                        if (strcmp(scope, "global") != 0) {
                            // Verifica se a função já foi registrada no escopo atual
                            BucketList funcInScope = st_lookup_in_scope(funcName, scope);
                            
                            if (funcInScope) {
                                // Adiciona a linha de chamada
                                LineList lines = funcInScope->lines;
                                while (lines->next != NULL) lines = lines->next;
                                lines->next = (LineList)malloc(sizeof(struct LineListRec));
                                lines->next->lineno = t->lineno;
                                lines->next->next = NULL;
                            } else {
                                // Registra nova entrada para a função no escopo atual
                                DEBUG_SYMTAB("Registrando função '%s' no escopo '%s'", funcName, scope);
                                st_insert(funcName, t->lineno, location++, scope, "func", func->dataType, 0, 0);
                            }
                        } else {
                            // Adiciona linha à entrada global
                            LineList lines = func->lines;
                            while (lines->next != NULL) lines = lines->next;
                            lines->next = (LineList)malloc(sizeof(struct LineListRec));
                            lines->next->lineno = t->lineno;
                            lines->next->next = NULL;
                        }
                    } else {
                        // Se a função não existe, registra-a
                        DEBUG_SYMTAB("Função '%s' não encontrada", funcName);
                        // st_insert(funcName, t->lineno, location++, "global", "func", "int", 0, 0);
                        
                        // if (strcmp(scope, "global") != 0) {
                        //     st_insert(funcName, t->lineno, location++, scope, "func", "int", 0, 0);
                        // }
                        return;
                    }
                    
                    if (t->right != NULL) {
                        if (t->right->type == NODE_ARGS) {
                            int argCount = 0;
                            processArguments_Func(t->right, funcName, &argCount);
                        }
                    } 
                    // caso como input();
                    else {
                        DEBUG_SYMTAB("Chamada de função '%s' sem argumentos", funcName);
                        BucketList funcEntry = st_lookup_in_scope(funcName, scope);
                        if (funcEntry) {
                            if (funcEntry->paramCount != 0 && funcEntry->params != NULL) {
                                ParamInfo current = funcEntry->params;
                                while (current != NULL) {
                                    ParamInfo temp = current;
                                    current = current->next;
                                    free(temp->paramType);
                                    free(temp);
                                }
                                funcEntry->params = NULL;
                                funcEntry->paramCount = 0;
                            }
                        }
                    }
                }
            }
        }
        
        traverse(t->left, preProc, postProc);
        traverse(t->right, preProc, postProc);
        
        if (t->type == NODE_FUNC_DECL || t->type == NODE_COMPOUND_DECL) {
            DEBUG_SYMTAB("Saindo do escopo %s", current_scope());
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
    
    DEBUG_SYMTAB("insertNode: Processando nó tipo %d para '%s' no escopo '%s'", t->type, t->value, scope);

    switch (t->type) {
        case NODE_VAR_DECL:
            if (existing == NULL) {
                DEBUG_SYMTAB("insertNode: Inserindo nova variável '%s'", t->value);
                st_insert(t->value, t->lineno, location++, strdup(scope), "var", t->idType, t->isArray, t->arraySize);
            }
            break;

        case NODE_PARAM:
            if (existing == NULL) {
                DEBUG_SYMTAB("insertNode: Inserindo parâmetro '%s'", t->value);
                st_insert(t->value, t->lineno, location++, strdup(scope), "param", t->idType, t->isArray, t->arraySize);
                
                // Se estamos no escopo de uma função, adiciona informação do parâmetro à função
                if (scope != NULL && strcmp(scope, "global") != 0) {
                    add_param_info(scope, t->idType, t->isArray);
                }
            }
            break;

        case NODE_VAR:
            // Busca em todos os escopos relevantes (atual e global)
            existing = st_lookup_all_scopes(t->value, scope);
            // if(existing != NULL && strcmp(existing->idType, "func") == 0) {
            //     printError("Erro semântico: '%s' é uma função, não uma variável", t->value);
            //     semanticErrorCount++;
            //     return;
            // }
            if (existing != NULL) {
                // Se o símbolo existe em algum escopo acessível, adiciona a linha de uso
                LineList lines = existing->lines;
                while (lines->next != NULL) lines = lines->next;
                lines->next = (LineList)malloc(sizeof(struct LineListRec));
                lines->next->lineno = t->lineno;
                lines->next->next = NULL;
                DEBUG_SYMTAB("insertNode: Atualizando uso de '%s' na linha %d no escopo %s", 
                            t->value, t->lineno, existing->scope);
            } else {
                // Se não existe em nenhum escopo acessível, cria uma nova entrada
                st_insert(t->value, t->lineno, location++, strdup(scope), "var", t->idType, t->isArray, t->arraySize);
                DEBUG_SYMTAB("insertNode: Criando nova variável '%s' no escopo atual %s", t->value, scope);
            }
            break;

        case NODE_FUNC_DECL:
            if (st_lookup(t->value) == NULL) {
                DEBUG_SYMTAB("insertNode: Declarando função '%s' com tipo '%s'", t->value, t->idType);
                st_insert(t->value, t->lineno, location++, "global", "func", t->idType, 0, 0);
            }
            break;

        case NODE_FUNC:
            // Adicionar tratamento para nós NODE_FUNC para garantir que sejam registrados corretamente
            DEBUG_SYMTAB("insertNode: Processando nó FUNC '%s'", t->value);
            if (st_lookup(t->value) == NULL) {
                // Se não existe ainda, tenta registrar como função (tipo será atualizado depois se necessário)
                st_insert(t->value, t->lineno, location++, "global", "func", "int", 0, 0);
            }
            break;

        // Removido o case NODE_ACTIVATION aqui, pois agora é tratado diretamente na função traverse

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
    
    //SO
    st_insert("msgLcd", 0, location++, "global", "func", "int", 0, 0);
    st_insert("saltoUser", 0, location++, "global", "func", "int", 0, 0);
    st_insert("halt", 0, location++, "global", "func", "int", 0, 0);
    st_insert("nop", 0, location++, "global", "func", "int", 0, 0);
    traverse(syntaxTree, insertNode, nullProc); //percorre a árvore sintática
    pop_scope();

    printSymTab(stdout); 
}