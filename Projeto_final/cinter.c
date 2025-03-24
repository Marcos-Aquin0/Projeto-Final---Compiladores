#include "globals.h"
#include "cinter.h"
#include "symtab.h"
#include <stdarg.h>

static IRCode irCode;
static int voidTempCount = 0;
// Função para gerar um novo nome de variável temporária para chamadas void
char* newVoidTemp(void) {
    char* temp = (char*)malloc(12);
    if (temp == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para variável temporária void.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(temp, "tv%d", voidTempCount++);
    return temp;
}

// Helper para nomes de operações - movido para o início
const char* getOpName(OperationType op) {
    switch(op) {
        case OP_ASSIGN: return "ASSIGN";
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MULT: return "MULT";
        case OP_DIV: return "DIV";
        case OP_LT: return "LT";
        case OP_LTE: return "LTE";
        case OP_GT: return "GT";
        case OP_GTE: return "GTE";
        case OP_EQ: return "EQ";
        case OP_NEQ: return "NEQ";
        case OP_LABEL: return "LABEL";
        case OP_JUMP: return "JUMP";
        case OP_JUMPFALSE: return "JUMPFALSE";
        case OP_JUMPTRUE: return "JUMPTRUE";
        case OP_CALL: return "CALL";
        case OP_PARAM: return "PARAM";
        case OP_RETURN: return "RETURN";
        case OP_ARRAY_LOAD: return "ARRAY_LOAD";
        case OP_ARRAY_STORE: return "ARRAY_STORE";
        case OP_ALLOC: return "ALLOC";
        case OP_FUNCTION: return "FUNCTION";
        case OP_END: return "END";
        default: return "UNKNOWN";
    }
}

// Função para obter o nome do tipo de nó baseado em seu valor numérico
const char* getNodeTypeName(NodeType type) {
    switch(type) {
        case NODE_PROGRAM: return "NODE_PROGRAM";
        case NODE_DECL: return "NODE_DECL";
        case NODE_DECL_LIST: return "NODE_DECL_LIST";
        case NODE_VAR_DECL: return "NODE_VAR_DECL";
        case NODE_SPEC_TYPE: return "NODE_SPEC_TYPE";
        case NODE_FUNC_DECL: return "NODE_FUNC_DECL";
        case NODE_PARAMS: return "NODE_PARAMS";
        case NODE_PARAM_LIST: return "NODE_PARAM_LIST";
        case NODE_PARAM: return "NODE_PARAM";
        case NODE_COMPOUND_DECL: return "NODE_COMPOUND_DECL";
        case NODE_LOCAL_DECL: return "NODE_LOCAL_DECL";
        case NODE_STATE_LIST: return "NODE_STATE_LIST";
        case NODE_STATEMENT: return "NODE_STATEMENT";
        case NODE_EXPR_DECL: return "NODE_EXPR_DECL";
        case NODE_SELECT_DECL: return "NODE_SELECT_DECL";
        case NODE_ITER_DECL: return "NODE_ITER_DECL";
        case NODE_RETURN_DECL: return "NODE_RETURN_DECL";
        case NODE_EXPR: return "NODE_EXPR";
        case NODE_VAR: return "NODE_VAR";
        case NODE_SIMP_EXPR: return "NODE_SIMP_EXPR";
        case NODE_RELATIONAL: return "NODE_RELATIONAL";
        case NODE_SUM_EXPR: return "NODE_SUM_EXPR";
        case NODE_TERM: return "NODE_TERM";
        case NODE_MULT: return "NODE_MULT";
        case NODE_FACTOR: return "NODE_FACTOR";
        case NODE_ACTIVATION: return "NODE_ACTIVATION";
        case NODE_ARGS: return "NODE_ARGS";
        case NODE_ARG_LIST: return "NODE_ARG_LIST";
        case NODE_FUNC: return "NODE_FUNC";
        case NODE_ARRAY_ACCESS: return "NODE_ARRAY_ACCESS";
        default: return "UNKNOWN_NODE_TYPE";
    }
}

// Inicializa a estrutura de código intermediário
void initIRCode(void) {
    irCode.head = NULL;
    irCode.tail = NULL;
    irCode.temp_count = 0;
    irCode.label_count = 0;
}

// Libera a memória alocada para o código intermediário
void freeIRCode(void) {
    Quadruple* current = irCode.head;
    while (current != NULL) {
        Quadruple* temp = current;
        current = current->next;
        if (temp->arg1) free(temp->arg1);
        if (temp->arg2) free(temp->arg2);
        if (temp->result) free(temp->result);
        free(temp);
    }
    irCode.head = NULL;
    irCode.tail = NULL;
}

// Gera um novo nome de variável temporária
char* newTemp(void) {
    char* temp = (char*)malloc(12);
    if (temp == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para variável temporária.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(temp, "t%d", irCode.temp_count++);
    return temp;
}

// Gera um novo rótulo para desvios
char* newLabel(void) {
    char* label = (char*)malloc(12);
    if (label == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para rótulo.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(label, "L%d", irCode.label_count++);
    return label;
}

// Adiciona uma nova quadrupla à lista de código intermediário
// Adicionar variável global para rastrear a linha atual do código fonte
static int currentSourceLine = 0;

// Modificar genQuad para usar a linha atual
void genQuad(OperationType op, char* arg1, char* arg2, char* result) {
    static int currentLine = 1;
    Quadruple* quad = (Quadruple*)malloc(sizeof(Quadruple));
    if (quad == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para quadrupla.\n");
        exit(EXIT_FAILURE);
    }
    
    quad->op = op;
    quad->arg1 = arg1 ? strdup(arg1) : NULL;
    quad->arg2 = arg2 ? strdup(arg2) : NULL;
    quad->result = result ? strdup(result) : NULL;
    quad->next = NULL;
    quad->line = currentLine++;
    quad->sourceLine = currentSourceLine;  // Usa a linha atual do código fonte
    
    DEBUG_IR("Gerando quadrupla: %s %s %s %s (quad: %d, fonte: %d)", 
             getOpName(op), arg1 ? arg1 : "-", 
             arg2 ? arg2 : "-", result ? result : "-", 
             quad->line, quad->sourceLine);
    
    if (irCode.head == NULL) {
        irCode.head = quad;
        irCode.tail = quad;
    } else {
        irCode.tail->next = quad;
        irCode.tail = quad;
    }
}

// Imprime o código intermediário gerado
void printIRCode(FILE* listing) {
    fprintf(listing, "Código Intermediário (Quadruplas):\n");
    fprintf(listing, "------------------------------------------------------------\n");
    fprintf(listing, "Quad  Fonte  Operação    Arg1        Arg2        Resultado\n");
    fprintf(listing, "------------------------------------------------------------\n");
    
    Quadruple* current = irCode.head;
    while (current != NULL) {
        char op_str[20];
        
        switch (current->op) {
            case OP_ASSIGN: strcpy(op_str, "ASSIGN"); break;
            case OP_ADD: strcpy(op_str, "ADD"); break;
            case OP_SUB: strcpy(op_str, "SUB"); break;
            case OP_MULT: strcpy(op_str, "MULT"); break;
            case OP_DIV: strcpy(op_str, "DIV"); break;
            case OP_LT: strcpy(op_str, "LT"); break;
            case OP_LTE: strcpy(op_str, "LTE"); break;
            case OP_GT: strcpy(op_str, "GT"); break;
            case OP_GTE: strcpy(op_str, "GTE"); break;
            case OP_EQ: strcpy(op_str, "EQ"); break;
            case OP_NEQ: strcpy(op_str, "NEQ"); break;
            case OP_LABEL: strcpy(op_str, "LABEL"); break;
            case OP_JUMP: strcpy(op_str, "JUMP"); break;
            case OP_JUMPFALSE: strcpy(op_str, "JUMPFALSE"); break;
            case OP_JUMPTRUE: strcpy(op_str, "JUMPTRUE"); break;
            case OP_CALL: strcpy(op_str, "CALL"); break;
            case OP_PARAM: strcpy(op_str, "PARAM"); break;
            case OP_RETURN: strcpy(op_str, "RETURN"); break;
            case OP_ARRAY_LOAD: strcpy(op_str, "ARRAY_LOAD"); break;
            case OP_ARRAY_STORE: strcpy(op_str, "ARRAY_STORE"); break;
            case OP_ALLOC: strcpy(op_str, "ALLOC"); break;
            case OP_FUNCTION: strcpy(op_str, "FUNCTION"); break;
            case OP_END: strcpy(op_str, "END"); break;
            default: strcpy(op_str, "UNKNOWN"); break;
        }
        
        fprintf(listing, "%-6d %-6d %-12s %-12s %-12s %-12s\n",
                current->line,
                current->sourceLine,
                op_str,
                current->arg1 ? current->arg1 : "-",
                current->arg2 ? current->arg2 : "-",
                current->result ? current->result : "-");
        
        current = current->next;
    }
    fprintf(listing, "------------------------------------------------------------\n");
}

// Função para imprimir as quadruplas como código intermediário de 3 endereços
void printThreeAddressCode(FILE* listing) {
    fprintf(listing, "Código Intermediário de 3 Endereços:\n");
    fprintf(listing, "-------------------------------------\n");
    
    Quadruple* current = irCode.head;
    while (current != NULL) {
        switch (current->op) {
            case OP_ASSIGN:
                fprintf(listing, "%s = %s\n", current->result, current->arg1);
                break;
            case OP_ADD:
                fprintf(listing, "%s = %s + %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_SUB:
                fprintf(listing, "%s = %s - %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_MULT:
                fprintf(listing, "%s = %s * %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_DIV:
                fprintf(listing, "%s = %s / %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_LT:
                fprintf(listing, "%s = %s < %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_LTE:
                fprintf(listing, "%s = %s <= %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_GT:
                fprintf(listing, "%s = %s > %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_GTE:
                fprintf(listing, "%s = %s >= %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_EQ:
                fprintf(listing, "%s = %s == %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_NEQ:
                fprintf(listing, "%s = %s != %s\n", current->result, current->arg1, current->arg2);
                break;
            case OP_LABEL:
                fprintf(listing, "%s:\n", current->result);
                break;
            case OP_JUMP:
                fprintf(listing, "goto %s\n", current->result);
                break;
            case OP_JUMPFALSE:
                fprintf(listing, "if (!%s) goto %s\n", current->arg1, current->result);
                break;
            case OP_JUMPTRUE:
                fprintf(listing, "if (%s) goto %s\n", current->arg1, current->result);
                break;
                case OP_CALL:
                if (current->result && strncmp(current->result, "tv", 2) == 0) {
                    // Chamada de função void - não mostra atribuição
                    fprintf(listing, "call %s, %s\n", current->arg1, current->arg2);
                } else {
                    // Chamada de função normal com atribuição
                    fprintf(listing, "%s = call %s, %s\n", current->result, current->arg1, current->arg2);
                }
                break;
            case OP_PARAM:
                fprintf(listing, "param %s\n", current->arg1);
                break;
            case OP_RETURN:
                fprintf(listing, "return %s\n", current->arg1 ? current->arg1 : "");
                break;
            case OP_ARRAY_LOAD:
                fprintf(listing, "%s = %s[%s]\n", current->result, current->arg1, current->arg2);
                break;
            case OP_ARRAY_STORE:
                fprintf(listing, "%s[%s] = %s\n", current->result, current->arg2, current->arg1);
                break;
            case OP_ALLOC:
                fprintf(listing, "%s = alloc %s\n", current->result, current->arg1);
                break;
            case OP_FUNCTION:
                fprintf(listing, "function %s\n", current->arg1);
                break;
            case OP_END:
                fprintf(listing, "end %s\n", current->arg1);
                break;
            default:
                fprintf(listing, "unknown operation\n");
                break;
        }
        current = current->next;
    }
    fprintf(listing, "-------------------------------------\n");
}

// Modificar genExprCode para imprimir o tipo de nó com seu nome
void genExprCode(ASTNode* expr, char* target) {
    if (expr == NULL) return;
    currentSourceLine = expr->lineno;  // Atualiza com a linha da expressão
    
    // Debug para mostrar informações da expressão
    DEBUG_IR("Gerando código para expressão tipo %d: %s na linha %d", 
             expr->type, getNodeTypeName(expr->type), expr->lineno);
    if (expr->value) {
        DEBUG_IR("  Expressão valor: '%s'", expr->value);
    }
    if (expr->idType) {
        DEBUG_IR("  Expressão tipo: '%s'", expr->idType);
    }
    DEBUG_IR("  Target: %s", target ? target : "NULL");
    
    char* leftResult = NULL;
    char* rightResult = NULL;
    
    switch (expr->type) {
        case NODE_FACTOR:
            // Para constantes numéricas
            if (expr->value != NULL) {
                genQuad(OP_ASSIGN, expr->value, NULL, target);
            }
            break;
            
        case NODE_VAR:
            // Para variáveis
            if (expr->right != NULL) {
                // É um acesso a array
                genArrayAccessCode(expr, target);
            } else {
                genQuad(OP_ASSIGN, expr->value, NULL, target);
            }
            break;
            
        case NODE_ACTIVATION:
            // Chamada de função
            genCallCode(expr, target);
            break;
            
        case NODE_SUM_EXPR:
            // Expressões de soma e subtração
            if (strcmp(expr->value, "+") == 0) {
                leftResult = newTemp();
                rightResult = newTemp();
                
                genExprCode(expr->left, leftResult);
                genExprCode(expr->right, rightResult);
                
                genQuad(OP_ADD, leftResult, rightResult, target);
            } else if (strcmp(expr->value, "-") == 0) {
                leftResult = newTemp();
                rightResult = newTemp();
                
                genExprCode(expr->left, leftResult);
                genExprCode(expr->right, rightResult);
                
                genQuad(OP_SUB, leftResult, rightResult, target);
            }
            break;
            
        case NODE_TERM:
            // Expressões de multiplicação e divisão
            if (strcmp(expr->value, "*") == 0) {
                leftResult = newTemp();
                rightResult = newTemp();
                
                genExprCode(expr->left, leftResult);
                genExprCode(expr->right, rightResult);
                
                genQuad(OP_MULT, leftResult, rightResult, target);
            } else if (strcmp(expr->value, "/") == 0) {
                leftResult = newTemp();
                rightResult = newTemp();
                
                genExprCode(expr->left, leftResult);
                genExprCode(expr->right, rightResult);
                
                genQuad(OP_DIV, leftResult, rightResult, target);
            }
            break;
            
        case NODE_RELATIONAL:
            // Expressões relacionais
            leftResult = newTemp();
            rightResult = newTemp();
            
            genExprCode(expr->left, leftResult);
            genExprCode(expr->right, rightResult);
            
            if (strcmp(expr->value, "==") == 0) {
                genQuad(OP_EQ, leftResult, rightResult, target);
            } else if (strcmp(expr->value, "<") == 0) {
                genQuad(OP_LT, leftResult, rightResult, target);
            } else if (strcmp(expr->value, "<=") == 0) {
                genQuad(OP_LTE, leftResult, rightResult, target);
            } else if (strcmp(expr->value, ">") == 0) {
                genQuad(OP_GT, leftResult, rightResult, target);
            } else if (strcmp(expr->value, ">=") == 0) {
                genQuad(OP_GTE, leftResult, rightResult, target);
            } else if (strcmp(expr->value, "!=") == 0) {
                genQuad(OP_NEQ, leftResult, rightResult, target);
            }
            break;
            
        default:
            // Outros tipos de expressão
            if (expr->left != NULL) {
                genExprCode(expr->left, target);
            } else if (expr->right != NULL) {
                genExprCode(expr->right, target);
            }
            break;
    }
}

// Gera código para atribuições
void genAssignCode(ASTNode* assign) {
    if (assign == NULL || assign->value == NULL || strcmp(assign->value, "=") != 0) return;
    
    if (assign->left->type == NODE_ARRAY_ACCESS) {
        // Atribuição para elemento de array
        genArrayAssignCode(assign);
    } else {
        // Atribuição para variável simples
        char* exprResult = newTemp();
        genExprCode(assign->right, exprResult);
        genQuad(OP_ASSIGN, exprResult, NULL, assign->left->value);
    }
}

// Gera código para condicionais (if-then-else)
void genIfCode(ASTNode* ifStmt) {
    if (ifStmt == NULL) return;
    
    // Gera código para a condição
    char* condResult = newTemp();
    genExprCode(ifStmt->left, condResult);
    
    // Cria rótulos para os blocos then e else
    char* falseLabel = newLabel();
    char* endLabel = newLabel();
    
    // Gera salto condicional
    genQuad(OP_JUMPFALSE, condResult, NULL, falseLabel);
    
    // Gera código para o bloco then
    generateIRCode(ifStmt->right->left);
    
    // Salta para o fim do if após o bloco then
    genQuad(OP_JUMP, NULL, NULL, endLabel);
    
    // Marca o início do bloco else
    genQuad(OP_LABEL, NULL, NULL, falseLabel);
    
    // Gera código para o bloco else, se existir
    if (ifStmt->right->right != NULL) {
        generateIRCode(ifStmt->right->right);
    }
    
    // Marca o fim do if
    genQuad(OP_LABEL, NULL, NULL, endLabel);
}

// Gera código para loops (while)
void genWhileCode(ASTNode* whileStmt) {
    if (whileStmt == NULL) return;
    currentSourceLine = whileStmt->lineno;  // Atualiza com a linha do while
    
    // Cria rótulos para o início da verificação da condição e fim do loop
    char* condLabel = newLabel();  // Rótulo para verificação da condição
    char* bodyLabel = newLabel();  // Rótulo para o corpo do loop
    char* endLabel = newLabel();   // Rótulo para o fim do loop
    
    // 1. Marca o ponto de início da verificação da condição
    genQuad(OP_LABEL, NULL, NULL, condLabel);
    
    // 2. Gera código para a condição
    char* condResult = newTemp();
    genExprCode(whileStmt->left, condResult);
    
    // 3. Gera salto condicional:
    //    - Se verdadeiro, executar o corpo do loop (bodyLabel)
    //    - Se falso, pular para o fim do loop (endLabel)
    genQuad(OP_JUMPTRUE, condResult, NULL, bodyLabel);
    genQuad(OP_JUMP, NULL, NULL, endLabel);
    
    // 4. Marca o início do corpo do loop
    genQuad(OP_LABEL, NULL, NULL, bodyLabel);
    
    // 5. Gera código para o corpo do loop
    generateIRCode(whileStmt->right);
    
    // 6. Salta de volta para a verificação da condição
    genQuad(OP_JUMP, NULL, NULL, condLabel);
    
    // 7. Marca o fim do loop
    genQuad(OP_LABEL, NULL, NULL, endLabel);
    
    DEBUG_IR("Gerado código para while: cond=%s, corpo=%s, fim=%s", 
             condLabel, bodyLabel, endLabel);
}

// Gera código para retorno de função
void genReturnCode(ASTNode* returnStmt) {
    if (returnStmt == NULL) return;
    currentSourceLine = returnStmt->lineno;  // Atualiza com a linha do return
    if (returnStmt->left != NULL) {
        // Retorno com valor
        char* returnResult = newTemp();
        genExprCode(returnStmt->left, returnResult);
        genQuad(OP_RETURN, returnResult, NULL, NULL);
    } else {
        // Retorno sem valor
        genQuad(OP_RETURN, NULL, NULL, NULL);
    }
}

// Gera código para declaração de função
void genFunctionCode(ASTNode* funcDecl) {
    if (funcDecl == NULL || funcDecl->value == NULL) return;
    currentSourceLine = funcDecl->lineno;  // Atualiza com a linha da declaração
    DEBUG_IR("Gerando código para função %s na linha %d", funcDecl->value, currentSourceLine);
    
    // Marca o início da função
    genQuad(OP_FUNCTION, funcDecl->value, NULL, NULL);
    
    // Processa o corpo da função
    generateIRCode(funcDecl->right);
    
    // Marca o fim da função
    genQuad(OP_END, funcDecl->value, NULL, NULL);
}

// Modificar genCallCode para usar variáveis temporárias void quando necessário
void genCallCode(ASTNode* call, char* target) {
    if (call == NULL || call->left == NULL || call->left->value == NULL) return;
    
    DEBUG_IR("Gerando código para chamada de função '%s' (tipo nó: %s) na linha %d", 
             call->left->value, getNodeTypeName(call->type), call->lineno);
    DEBUG_IR("  Target da chamada: %s", target ? target : "void (usando tv)");
    
    // Processa os argumentos da chamada
    ASTNode* args = call->right;
    int argCount = 0;
    
    // Verifica se a função é um caso especial como 'gcd' ou 'input'
    int isSpecialFunction = 
        (strcmp(call->left->value, "gcd") == 0) ||
        (strcmp(call->left->value, "input") == 0) ||
        (strcmp(call->left->value, "output") == 0);
    
    // Nova lógica para processar os argumentos corretamente
    if (args != NULL) {
        // O nó args pode ser NODE_ARGS ou diretamente o primeiro argumento
        DEBUG_IR("  Processando argumentos (tipo nó: %s)", getNodeTypeName(args->type));
        
        if (args->type == NODE_ARGS) {
            // Para nós do tipo NODE_ARGS, o primeiro argumento está em args->left
            ASTNode* argList = args->left;
            
            while (argList != NULL) {
                // Verifica se este é um nó de argumento válido
                if (argList->type == NODE_ARG_LIST && argList->left != NULL) {
                    // Debug para mostrar informações do argumento
                    DEBUG_IR("    Arg %d: tipo %d (%s), valor %s", 
                            argCount + 1, 
                            argList->left->type, 
                            getNodeTypeName(argList->left->type),
                            argList->left->value ? argList->left->value : "expressão");
                    
                    // Gera código para o argumento
                    char* argResult = newTemp();
                    genExprCode(argList->left, argResult);
                    
                    // Passa o argumento para a função
                    char argNum[12];
                    sprintf(argNum, "%d", argCount);
                    genQuad(OP_PARAM, argResult, argNum, NULL);
                    
                    argCount++;
                    // Próximo argumento está em argList->right
                    argList = argList->right;
                } else {
                    // Se não for um nó de lista de argumentos, verifica se é um argumento direto
                    if (argList->type != NODE_ARG_LIST) {
                        DEBUG_IR("    Arg direto %d: tipo %d (%s)", 
                                argCount + 1, argList->type, getNodeTypeName(argList->type));
                        
                        // Gera código para o argumento
                        char* argResult = newTemp();
                        genExprCode(argList, argResult);
                        
                        // Passa o argumento para a função
                        char argNum[12];
                        sprintf(argNum, "%d", argCount);
                        genQuad(OP_PARAM, argResult, argNum, NULL);
                        
                        argCount++;
                    }
                    break; // Sai do loop, já processou todos os argumentos
                }
            }
        } else {
            // Caso em que args é diretamente o único argumento
            DEBUG_IR("    Arg único: tipo %d (%s)", args->type, getNodeTypeName(args->type));
            
            char* argResult = newTemp();
            genExprCode(args, argResult);
            
            char argNum[12];
            sprintf(argNum, "%d", argCount);
            genQuad(OP_PARAM, argResult, argNum, NULL);
            
            argCount++;
        }
    } else {
        DEBUG_IR("  Sem argumentos");
    }
    
    // Determina o target para a chamada da função
    char* callTarget = target;
    
    // Se target for NULL, é uma chamada de função void
    // Neste caso, usamos uma variável temporária especial "tv" para representar a chamada
    if (callTarget == NULL) {
        callTarget = newVoidTemp();
        DEBUG_IR("  Usando %s para chamada void", callTarget);
    }
    
    // Gera a chamada de função
    char argCountStr[12];
    sprintf(argCountStr, "%d", argCount);
    
    DEBUG_IR("  Gerando CALL com %d argumentos", argCount);
    genQuad(OP_CALL, call->left->value, argCountStr, callTarget);
}

// Gera código para acesso a elementos de array
void genArrayAccessCode(ASTNode* arrayAccess, char* target) {
    if (arrayAccess == NULL || arrayAccess->value == NULL) return;
    
    // Calcula o índice
    char* indexResult = newTemp();
    genExprCode(arrayAccess->right, indexResult);
    
    // Carrega o valor do array
    genQuad(OP_ARRAY_LOAD, arrayAccess->value, indexResult, target);
}

// Gera código para atribuição a elementos de array
void genArrayAssignCode(ASTNode* arrayAssign) {
    if (arrayAssign == NULL || arrayAssign->left == NULL || arrayAssign->right == NULL) return;
    
    // Obtém o array e o índice
    ASTNode* arrayAccess = arrayAssign->left;
    char* arrayName = arrayAccess->left->value;
    
    // Calcula o índice
    char* indexResult = newTemp();
    genExprCode(arrayAccess->right, indexResult);
    
    // Calcula o valor a ser armazenado
    char* valueResult = newTemp();
    genExprCode(arrayAssign->right, valueResult);
    
    // Armazena o valor no array
    genQuad(OP_ARRAY_STORE, valueResult, indexResult, arrayName);
}

// Também podemos modificar a função generateIRCode para imprimir tipos dos nós
void generateIRCode(ASTNode* node) {
    if (node == NULL) return;
    
    // Debug para mostrar tipo do nó sendo processado
    if (node->type != NODE_PROGRAM && node->type != NODE_DECL_LIST && 
        node->type != NODE_COMPOUND_DECL && node->type != NODE_STATE_LIST) {
        DEBUG_IR("Processando nó tipo %d: %s", node->type, getNodeTypeName(node->type));
    }
    
    switch (node->type) {
        case NODE_PROGRAM:
        case NODE_DECL_LIST:
        case NODE_COMPOUND_DECL:
        case NODE_LOCAL_DECL:
        case NODE_STATE_LIST:
        case NODE_STATEMENT:
            // Nós de controle - apenas continua a recursão
            generateIRCode(node->left);
            generateIRCode(node->right);
            break;
            
        case NODE_EXPR:
            if (node->value != NULL && strcmp(node->value, "=") == 0) {
                genAssignCode(node);
            } else if (node->type == NODE_ACTIVATION || 
                      (node->left != NULL && node->left->type == NODE_ACTIVATION)) {
                // Chamada de função sem atribuição (possivelmente void)
                genCallCode(node, NULL);
            } else {
                char* tempResult = newTemp();
                genExprCode(node, tempResult);
            }
            break;
            
        case NODE_ACTIVATION:
            // Chamada de função independente (possivelmente void)
            genCallCode(node, NULL);
            break;
            
        case NODE_SELECT_DECL:
            genIfCode(node);
            break;
            
        case NODE_ITER_DECL:
            genWhileCode(node);
            break;
            
        case NODE_RETURN_DECL:
            genReturnCode(node);
            break;
            
        case NODE_FUNC_DECL:
            genFunctionCode(node);
            break;
            
        default:
            // Para outros tipos de nó, apenas continua a recursão
            if (node->left) generateIRCode(node->left);
            if (node->right) generateIRCode(node->right);
            break;
    }
}


// Função de entrada para gerar o código intermediário
void ircode_generate(ASTNode* syntaxTree) {
    initIRCode();
    generateIRCode(syntaxTree);
    printIRCode(stdout);
    printThreeAddressCode(stdout);  // Adiciona impressão do código de 3 endereços
}