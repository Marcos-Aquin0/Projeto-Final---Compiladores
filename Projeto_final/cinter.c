#include "globals.h"
#include "cinter.h"
#include "symtab.h"
#include <stdarg.h>

static IRCode irCode;

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
                fprintf(listing, "%s = call %s, %s\n", current->result, current->arg1, current->arg2);
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

// Gera código para expressões
void genExprCode(ASTNode* expr, char* target) {
    if (expr == NULL) return;
    currentSourceLine = expr->lineno;  // Atualiza com a linha da expressão
    DEBUG_IR("Gerando código para expressão tipo %d", expr->type);
    
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
    
    // Cria rótulos para o início e fim do loop
    char* startLabel = newLabel();
    char* endLabel = newLabel();
    
    // Marca o início do loop
    genQuad(OP_LABEL, NULL, NULL, startLabel);
    
    // Gera código para a condição
    char* condResult = newTemp();
    genExprCode(whileStmt->left, condResult);
    
    // Gera salto condicional para o fim do loop
    genQuad(OP_JUMPFALSE, condResult, NULL, endLabel);
    
    // Gera código para o corpo do loop
    generateIRCode(whileStmt->right);
    
    // Salta de volta para o início do loop
    genQuad(OP_JUMP, NULL, NULL, startLabel);
    
    // Marca o fim do loop
    genQuad(OP_LABEL, NULL, NULL, endLabel);
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

// Gera código para chamada de função
void genCallCode(ASTNode* call, char* target) {
    if (call == NULL || call->left == NULL || call->left->value == NULL) return;
    
    // Processa os argumentos da chamada
    ASTNode* args = call->right;
    int argCount = 0;
    
    if (args != NULL) {
        ASTNode* argList = args->left;
        while (argList != NULL) {
            // Gera código para o argumento
            char* argResult = newTemp();
            genExprCode(argList->left, argResult);
            
            // Passa o argumento para a função
            char argNum[12];
            sprintf(argNum, "%d", argCount);
            genQuad(OP_PARAM, argResult, argNum, NULL);
            
            argCount++;
            argList = argList->right;
        }
    }
    
    // Gera a chamada de função
    char argCountStr[12];
    sprintf(argCountStr, "%d", argCount);
    genQuad(OP_CALL, call->left->value, argCountStr, target);
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

// Função recursiva principal para geração de código
void generateIRCode(ASTNode* node) {
    if (node == NULL) return;
    
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
            } else {
                char* tempResult = newTemp();
                genExprCode(node, tempResult);
            }
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
    DEBUG_IR("Iniciando geração de código intermediário");
    initIRCode();
    generateIRCode(syntaxTree);
    printIRCode(stdout);
    printThreeAddressCode(stdout);  // Adiciona impressão do código de 3 endereços
    DEBUG_IR("Geração de código intermediário concluída");
}

// Adiciona função de debug
void debugPrint(const char* format, ...) {
    #ifdef DEBUG
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\033[33m"); // Amarelo para debug
    vfprintf(stderr, format, args);
    fprintf(stderr, "\033[0m\n");
    va_start(args, format);
    #endif
}