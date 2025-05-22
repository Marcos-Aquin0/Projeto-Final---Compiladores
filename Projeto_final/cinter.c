#include "cinter.h"

static IRCode irCode;

// Função para gerar um novo nome de variável temporária para chamadas void, apenas para manipulação
static int voidTempCount = 0;
char* newVoidTemp(void) {
    char* temp = (char*)malloc(12);
    if (temp == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para variável temporária void.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(temp, "tv%d", voidTempCount++);
    return temp;
}

// Helper para nomes de operações 
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
        case OP_ARGUMENT: return "ARGUMENT"; // Alterado de PARAM para ARGUMENT
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

// Gera um novo nome de variável temporária t_
char* newTemp(void) {
    char* temp = (char*)malloc(12);
    if (temp == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para variável temporária.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(temp, "t%d", irCode.temp_count++);
    return temp;
}

// Gera um novo rótulo para desvios (label L_)
char* newLabel(void) {
    char* label = (char*)malloc(12);
    if (label == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para rótulo.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(label, "L%d", irCode.label_count++);
    return label;
}

// Variável global para rastrear a linha atual do código fonte
static int currentSourceLine = 0;

// Adiciona uma nova quadrupla à lista de código intermediário
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

// Imprime o código intermediário gerado em quádruplas
void printIRCode(FILE* listing) {
    // Cria arquivo de saída para as quádruplas
    FILE* outfile = fopen("Output/quadruples.txt", "w");
    if (outfile == NULL) {
        fprintf(stderr, "Erro: Não foi possível criar o arquivo de saída para quádruplas.\n");
    } else {
        fprintf(outfile, "Código Intermediário (Quadruplas):\n");
        fprintf(outfile, "------------------------------------------------------------\n");
        fprintf(outfile, "Quad  Fonte  Operação    Arg1        Arg2        Resultado\n");
        fprintf(outfile, "------------------------------------------------------------\n");
    }
    
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
            case OP_ARGUMENT: strcpy(op_str, "ARGUMENT"); break; // Alterado de PARAM para ARGUMENT
            case OP_ARRAY_LOAD: strcpy(op_str, "ARRAY_LOAD"); break;
            case OP_ARRAY_STORE: strcpy(op_str, "ARRAY_STORE"); break;
            case OP_ALLOC: strcpy(op_str, "ALLOC"); break;
            case OP_FUNCTION: strcpy(op_str, "FUNCTION"); break;
            case OP_END: strcpy(op_str, "END"); break;
            default: strcpy(op_str, "UNKNOWN"); break;
        }
        
        // Escreve também no arquivo de saída
        if (outfile != NULL) {
            fprintf(outfile, "%-6d %-6d %-12s %-12s %-12s %-12s\n",
                    current->line,
                    current->sourceLine,
                    op_str,
                    current->arg1 ? current->arg1 : "-",
                    current->arg2 ? current->arg2 : "-",
                    current->result ? current->result : "-");
        }
        
        current = current->next;
    }
    
    if (outfile != NULL) {
        fprintf(outfile, "------------------------------------------------------------\n");
        fclose(outfile);
        printf("Código intermediário (quádruplas) salvo em 'Projeto_final/Output/quadruples.txt'\n");
    }
}

// Função para imprimir as quadruplas como código intermediário de 3 endereços
void printThreeAddressCode(FILE* listing) {
    // Cria arquivo de saída para o código de 3 endereços
    FILE* outfile = fopen("Output/three_address_code.txt", "w");
    if (outfile == NULL) {
        fprintf(stderr, "Erro: Não foi possível criar o arquivo de saída para código de 3 endereços.\n");
    } else {
        fprintf(outfile, "Código Intermediário de 3 Endereços:\n");
        fprintf(outfile, "-------------------------------------\n");
    }
    
    Quadruple* current = irCode.head;
    while (current != NULL) {
        char line[256];  // Buffer para armazenar a linha formatada
        
        switch (current->op) {
            case OP_ASSIGN:
                sprintf(line, "%s = %s", current->result, current->arg1);
                break;
            case OP_ADD:
                sprintf(line, "%s = %s + %s", current->result, current->arg1, current->arg2);
                break;
            case OP_SUB:
                sprintf(line, "%s = %s - %s", current->result, current->arg1, current->arg2);
                break;
            case OP_MULT:
                sprintf(line, "%s = %s * %s", current->result, current->arg1, current->arg2);
                break;
            case OP_DIV:
                sprintf(line, "%s = %s / %s", current->result, current->arg1, current->arg2);
                break;
            case OP_LT:
                sprintf(line, "%s = %s < %s", current->result, current->arg1, current->arg2);
                break;
            case OP_LTE:
                sprintf(line, "%s = %s <= %s", current->result, current->arg1, current->arg2);
                break;
            case OP_GT:
                sprintf(line, "%s = %s > %s", current->result, current->arg1, current->arg2);
                break;
            case OP_GTE:
                sprintf(line, "%s = %s >= %s", current->result, current->arg1, current->arg2);
                break;
            case OP_EQ:
                sprintf(line, "%s = %s == %s", current->result, current->arg1, current->arg2);
                break;
            case OP_NEQ:
                sprintf(line, "%s = %s != %s", current->result, current->arg1, current->arg2);
                break;
            case OP_LABEL:
                sprintf(line, "%s:", current->result);
                break;
            case OP_JUMP:
                sprintf(line, "goto %s", current->result);
                break;
            case OP_JUMPFALSE:
                sprintf(line, "if (!%s) goto %s", current->arg1, current->result);
                break;
            case OP_JUMPTRUE:
                sprintf(line, "if (%s) goto %s", current->arg1, current->result);
                break;
            case OP_CALL:
                if (current->result && strncmp(current->result, "tv", 2) == 0) {
                    // Chamada de função void - não mostra atribuição
                    sprintf(line, "call %s, %s", current->arg1, current->arg2);
                } else {
                    // Chamada de função normal com atribuição
                    sprintf(line, "%s = call %s, %s", current->result, current->arg1, current->arg2);
                }
                break;
            case OP_ARGUMENT: // Alterado de OP_PARAM para OP_ARGUMENT
                sprintf(line, "argument %s", current->arg1);
                break;
            case OP_PARAM:
                sprintf(line, "param %s", current->arg1);
                break;
            case OP_RETURN:
                if (current->arg1) {
                    sprintf(line, "return %s", current->arg1);
                } else {
                    sprintf(line, "return");
                }
                break;
            case OP_ARRAY_LOAD:
                sprintf(line, "%s = %s[%s]", current->result, current->arg1, current->arg2);
                break;
            case OP_ARRAY_STORE:
                sprintf(line, "%s[%s] = %s", current->result, current->arg2, current->arg1);
                break;
            case OP_ALLOC:
                sprintf(line, "%s = alloc %s", current->result, current->arg1);
                break;
            case OP_FUNCTION:
                sprintf(line, "function %s", current->arg1);
                break;
            case OP_END:
                sprintf(line, "end %s", current->arg1);
                break;
            default:
                sprintf(line, "unknown operation");
                break;
        }
        
        // Escreve também no arquivo de saída
        if (outfile != NULL) {
            fprintf(outfile, "%s\n", line);
        }
        
        current = current->next;
    }

    if (outfile != NULL) {
        fprintf(outfile, "-------------------------------------\n");
        fclose(outfile);
        printf("Código de 3 endereços salvo em 'Projeto_final/Output/three_address_code.txt'\n");
    }
}

// Trata as expressões do código
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
            
        case NODE_ARRAY_ACCESS:
            // Tratar explicitamente NODE_ARRAY_ACCESS
            genArrayAccessCode(expr, target);
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
            
        case NODE_SIMP_EXPR:
            // Expressões simples que podem conter operadores relacionais
            DEBUG_IR("  Processando NODE_SIMP_EXPR");
            
            // Se existe um operador relacional (filho da direita é NODE_RELATIONAL)
            if (expr->right != NULL && expr->right->type == NODE_RELATIONAL) {
                leftResult = newTemp();
                rightResult = newTemp();
                
                // Gera código para o lado esquerdo (geralmente uma variável ou expressão)
                genExprCode(expr->left, leftResult);
                
                // Obtém o operador relacional e o operando direito da relação
                ASTNode* relational = expr->right;
                
                // Gera código para o operando direito da relação
                if (relational->right) {
                    genExprCode(relational->right, rightResult);
                } else {
                    DEBUG_IR("ERRO: Nó relacional sem operando direito!");
                    return;
                }
                DEBUG_IR("relational->type: %s", relational->left->value);
                
                // Determina o tipo de operação relacional
                OperationType opType = OP_EQ; // Padrão
                if (relational->left->value) {
                    if (strcmp(relational->left->value, "<") == 0) {
                        opType = OP_LT;
                    } else if (strcmp(relational->left->value, "<=") == 0) {
                        opType = OP_LTE;
                    } else if (strcmp(relational->left->value, ">") == 0) {
                        opType = OP_GT;
                    } else if (strcmp(relational->left->value, ">=") == 0) {
                        opType = OP_GTE;
                    } else if (strcmp(relational->left->value, "==") == 0) {
                        opType = OP_EQ;
                    } else if (strcmp(relational->left->value, "!=") == 0) {
                        opType = OP_NEQ;
                    }
                }
                
                DEBUG_IR("  Gerando operador relacional %s", getOpName(opType));
                
                // Gera quadrupla para a operação relacional
                genQuad(opType, leftResult, rightResult, target);
            } else {
                // Se não for uma expressão relacional, apenas passa o processamento para o filho esquerdo
                genExprCode(expr->left, target);
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
            // Expressões relacionais quando processadas diretamente
            leftResult = newTemp();
            rightResult = newTemp();
            
            if (expr->left) {
                DEBUG_IR("expr->left: %s\n", expr->left->value);
                genExprCode(expr->left, leftResult);
            } else {
                DEBUG_IR("AVISO: Nó relacional sem operando esquerdo!");
                // Em alguns casos, o operando esquerdo pode já estar sendo tratado no NODE_SIMP_EXPR
            }
            
            if (expr->right) {
                genExprCode(expr->right, rightResult);
            } else {
                DEBUG_IR("ERRO: Nó relacional sem operando direito!");
                return;
            }
            
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
    
    // Debug 
    DEBUG_IR("Processando atribuição na linha %d", assign->lineno);
    DEBUG_IR("  Lado esquerdo: tipo %s, valor %s", 
             getNodeTypeName(assign->left->type), 
             assign->left->value ? assign->left->value : "(nulo)");
    
    // Verifica se o lado esquerdo da atribuição é um acesso a array
    if (assign->left->type == NODE_ARRAY_ACCESS || 
        (assign->left->type == NODE_VAR && assign->left->right != NULL)) {
        // É um acesso a array (a[i] = ...)
        genArrayAssignCode(assign);
    } else {
        // É uma variável simples (ou outro tipo de expressão)
        char* exprResult = newTemp();
        genExprCode(assign->right, exprResult);
        
        // Verifica se o lado direito da atribuição é um acesso a array
        if (assign->right->type == NODE_ARRAY_ACCESS ||
            (assign->right->type == NODE_VAR && assign->right->right != NULL)) {
            // Debug para acesso a array no lado direito (k = a[i])
            DEBUG_IR("  Lado direito é acesso a array");
        }
        
        // Completa a atribuição
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

// Função para processar parâmetros de uma declaração de função
void processParamDeclarations(ASTNode* params) {
    if (params == NULL) return;
    
    DEBUG_IR("Processando parâmetros de função, nó tipo: %s", getNodeTypeName(params->type));
    
    // Processa o nó de parâmetros
    if (params->type == NODE_PARAMS) {
        // Processa a lista de parâmetros
        processParamDeclarations(params->left);
    }
    // Processa a lista de parâmetros
    else if (params->type == NODE_PARAM_LIST) {
        if(params->left && params->left->type == NODE_PARAM_LIST) processParamDeclarations(params->left);
        if (params->left && params->left->type == NODE_PARAM) {
            ASTNode* param = params->left;
            if (param->value) {
                DEBUG_IR("  Declarando parâmetro: %s", param->value);
                genQuad(OP_PARAM, param->value, NULL, NULL);
            }
        }
        
        // Depois processa o resto da lista (ordem importante)
        if (params->right) {
            processParamDeclarations(params->right);
        }
    }
    // Processa um parâmetro individual
    else if (params->type == NODE_PARAM) {
        if (params->value) {
            DEBUG_IR("  Declarando parâmetro: %s", params->value);
            genQuad(OP_PARAM, params->value, NULL, NULL);
        }
    }
}

// Gera código para declaração de função
void genFunctionCode(ASTNode* funcDecl) {
    if (funcDecl == NULL || funcDecl->value == NULL) return;
    currentSourceLine = funcDecl->lineno;  // Atualiza com a linha da declaração
    DEBUG_IR("Gerando código para função %s na linha %d", funcDecl->value, currentSourceLine);
    
    // Marca o início da função
    genQuad(OP_FUNCTION, funcDecl->value, NULL, NULL);
    
    // Processa os parâmetros da função (se houver)
    if (funcDecl->right->left != NULL) {
        DEBUG_IR("  Processando parâmetros da função %s", funcDecl->value);
        processParamDeclarations(funcDecl->right->left);
    }
    
    // Processa o corpo da função
    generateIRCode(funcDecl->right);
    
    // Marca o fim da função
    genQuad(OP_END, funcDecl->value, NULL, NULL);
}

//processa os argumentos da chamada de função
void processArguments(ASTNode* argNode, int* argCount) {
    if (argNode == NULL) return;
    
    DEBUG_IR("  > processArguments: nó tipo %s", getNodeTypeName(argNode->type));
    
    // In-order traversal: filhos esquerdos, atual, filho da direita
    if (argNode->left != NULL) {
        // Caso especial: argumento é uma chamada de função
        if (argNode->left->type == NODE_ACTIVATION) {
            DEBUG_IR("    Arg %d é uma chamada de função: %s", 
                    *argCount + 1,
                    argNode->left->left ? argNode->left->left->value : "unknown");
            
            // Gera o código e armazena em uma variável temp
            char* funcResult = newTemp();
            genCallCode(argNode->left, funcResult);
            
            // Passa o resultado como parâmetro
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, funcResult, argNum, NULL);
            
            (*argCount)++;
        } 
        // Caso adicional: argumento é um acesso a vetor
        else if (argNode->left->type == NODE_ARRAY_ACCESS) {
            DEBUG_IR("    Arg %d é um acesso a vetor: %s", 
                    *argCount + 1,
                    argNode->left->left ? argNode->left->left->value : "unknown");
            
            // Gera o código para o acesso ao vetor e armazena em uma variável temp
            char* arrayResult = newTemp();
            genArrayAccessCode(argNode->left, arrayResult);
            
            // Passa o resultado como parâmetro
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, arrayResult, argNum, NULL);
            
            (*argCount)++;
        }
        // Lida com expressões complexas
        else if (argNode->left->type == NODE_EXPR || 
                 argNode->left->type == NODE_SUM_EXPR || 
                 argNode->left->type == NODE_TERM ||
                 argNode->left->type == NODE_RELATIONAL) {
            DEBUG_IR("    Arg %d é uma expressão complexa tipo: %s", 
                    *argCount + 1, 
                    getNodeTypeName(argNode->left->type));
            
            char* exprResult = newTemp();
            genExprCode(argNode->left, exprResult);
            
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, exprResult, argNum, NULL);
            
            (*argCount)++;
        }
        // processo recursivo normal
        else {
            processArguments(argNode->left, argCount);
        }
    } else {
        DEBUG_IR("  > processArguments: left é NULL");
    }
    
    // se o nó não é do tipo lista
    if (argNode->type != NODE_ARG_LIST && argNode->type != NODE_ARGS) {
        // nó atual é uma chamada de função
        if (argNode->type == NODE_ACTIVATION) {
            DEBUG_IR("    Arg %d é uma chamada de função: %s", 
                    *argCount + 1,
                    argNode->left ? argNode->left->value : "unknown");
            
            char* funcResult = newTemp();
            genCallCode(argNode, funcResult);
            
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, funcResult, argNum, NULL);
            
            (*argCount)++;
        }
        // Caso adicional: nó atual é um acesso a vetor
        else if (argNode->type == NODE_ARRAY_ACCESS) {
            DEBUG_IR("    Arg %d é um acesso a vetor: %s", 
                    *argCount + 1,
                    argNode->left ? argNode->left->value : "unknown");
            
            char* arrayResult = newTemp();
            genArrayAccessCode(argNode, arrayResult);
            
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, arrayResult, argNum, NULL);
            
            (*argCount)++;
        }
        else if (argNode->type == NODE_EXPR || 
                argNode->type == NODE_SUM_EXPR || 
                argNode->type == NODE_TERM ||
                argNode->type == NODE_RELATIONAL) {
            DEBUG_IR("    Arg %d é uma expressão complexa tipo: %s", 
                    *argCount + 1, getNodeTypeName(argNode->type));
            
            char* exprResult = newTemp();
            genExprCode(argNode, exprResult);
            
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, exprResult, argNum, NULL);
            
            (*argCount)++;
        }
        // processa valores simples
        else {
            DEBUG_IR("    Arg %d: tipo %d (%s), valor %s", 
                    *argCount + 1, 
                    argNode->type, 
                    getNodeTypeName(argNode->type),
                    argNode->value ? argNode->value : "expressão");
            
            char* argResult = newTemp();
            genExprCode(argNode, argResult);
            
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, argResult, argNum, NULL);
            
            (*argCount)++;
        }
    }
    
    // subarvore da direita
    if (argNode->right != NULL) {
        // filho da direita é uma chamada de função
        if (argNode->right->type == NODE_ACTIVATION) {
            DEBUG_IR("    Arg %d é uma chamada de função (à direita): %s", 
                    *argCount + 1,
                    argNode->right->left ? argNode->right->left->value : "unknown");
            
            char* funcResult = newTemp();
            genCallCode(argNode->right, funcResult);
            
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, funcResult, argNum, NULL);
            
            (*argCount)++;
        }
        // Caso adicional: filho da direita é um acesso a vetor
        else if (argNode->right->type == NODE_ARRAY_ACCESS) {
            DEBUG_IR("    Arg %d é um acesso a vetor (à direita): %s", 
                    *argCount + 1,
                    argNode->right->left ? argNode->right->left->value : "unknown");
            
            char* arrayResult = newTemp();
            genArrayAccessCode(argNode->right, arrayResult);
            
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, arrayResult, argNum, NULL);
            
            (*argCount)++;
        }
        // expressões complexas no filho da direita
        else if (argNode->right->type == NODE_EXPR || 
                 argNode->right->type == NODE_SUM_EXPR || 
                 argNode->right->type == NODE_TERM ||
                 argNode->right->type == NODE_RELATIONAL) {
            DEBUG_IR("    Arg %d é uma expressão complexa (à direita) tipo: %s", 
                    *argCount + 1, getNodeTypeName(argNode->right->type));
            
            char* exprResult = newTemp();
            genExprCode(argNode->right, exprResult);
            
            char argNum[12];
            sprintf(argNum, "%d", *argCount);
            genQuad(OP_ARGUMENT, exprResult, argNum, NULL);
            
            (*argCount)++;
        }
        
        else {
            processArguments(argNode->right, argCount);
        }
    } else {
        DEBUG_IR("  > processArguments: right é NULL");
    }
}

void genCallCode(ASTNode* call, char* target) {
    if (call == NULL || call->left == NULL || call->left->value == NULL) return;
    
    DEBUG_IR("Gerando código para chamada de função '%s' (tipo nó: %s) na linha %d", 
             call->left->value, getNodeTypeName(call->type), call->lineno);
    DEBUG_IR("  Target da chamada: %s", target ? target : "void (usando tv)");
    
    // Processa os argumentos da chamada
    int argCount = 0;
    
    // Verifica se há argumentos na chamada de função
    if (call->right != NULL) {
        DEBUG_IR("  Processando argumentos (tipo nó: %s)", getNodeTypeName(call->right->type));
        
        ASTNode* args = call->right;
        
        // argumento é chamada de função
        if (args->type == NODE_ACTIVATION) {
            DEBUG_IR("  Argumento é uma chamada de função: %s", 
                args->left ? args->left->value : "unknown");
            
            char* funcResult = newTemp();
            genCallCode(args, funcResult);
            
            genQuad(OP_PARAM, funcResult, "0", NULL);
            argCount = 1;
        }
        
        else if (args->type == NODE_EXPR || 
                args->type == NODE_SUM_EXPR || 
                args->type == NODE_TERM ||
                args->type == NODE_RELATIONAL) {
            DEBUG_IR("  Argumento é uma expressão complexa tipo: %s", 
                    getNodeTypeName(args->type));
            
            char* exprResult = newTemp();
            genExprCode(args, exprResult);
            
            genQuad(OP_PARAM, exprResult, "0", NULL);
            argCount = 1;
        }
        // casos padrões
        else if (args->type == NODE_ARGS) {
            // Verifica se o nó ARGS tem filhos
            if (args->left != NULL) {
                // Usar travessia in-order para processar todos os argumentos na ordem correta
                processArguments(args, &argCount);
            } else {
                DEBUG_IR("  Nó ARGS não tem argumentos (filho left é NULL)");
            }
        } else {
            // Caso em que args é diretamente o único argumento simples
            DEBUG_IR("    Arg único: tipo %d (%s)", args->type, getNodeTypeName(args->type));
            
            char* argResult = newTemp();
            genExprCode(args, argResult);
            
            genQuad(OP_PARAM, argResult, "0", NULL);
            argCount = 1;
        }
    } else {
        DEBUG_IR("  Sem argumentos (call->right é NULL)");
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
    if (arrayAccess == NULL) return;
    
    DEBUG_IR("Gerando código para acesso a array tipo %s", getNodeTypeName(arrayAccess->type));
    
    // Determinar o tipo correto de nó de acesso a array
    ASTNode* arrayName = NULL;
    ASTNode* indexExpr = NULL;
    
    if (arrayAccess->type == NODE_ARRAY_ACCESS) {
        // Formato específico NODE_ARRAY_ACCESS
        arrayName = arrayAccess->left;
        indexExpr = arrayAccess->right;
    } else if (arrayAccess->type == NODE_VAR && arrayAccess->right != NULL) {
        // Formato NODE_VAR com índice em 'right'
        arrayName = arrayAccess;
        indexExpr = arrayAccess->right;
    } else {
        DEBUG_IR("ERRO: formato de acesso a array desconhecido!");
        return;
    }
    
    if (arrayName == NULL || arrayName->value == NULL) {
        DEBUG_IR("ERRO: nome do array ausente ou inválido!");
        return;
    }
    
    DEBUG_IR("  Nome do array: %s", arrayName->value);
    
    // Calcula o índice
    char* indexResult = newTemp();
    genExprCode(indexExpr, indexResult);
    
    // Carrega o valor do array
    genQuad(OP_ARRAY_LOAD, arrayName->value, indexResult, target);
}

// Gera código para atribuição a elementos de array
void genArrayAssignCode(ASTNode* arrayAssign) {
    if (arrayAssign == NULL || arrayAssign->left == NULL || arrayAssign->right == NULL) return;
    
    DEBUG_IR("Gerando código para atribuição a array");
    
    // Determinar o tipo correto de nó de acesso a array
    ASTNode* arrayAccess = arrayAssign->left;
    ASTNode* arrayName = NULL;
    ASTNode* indexExpr = NULL;
    
    if (arrayAccess->type == NODE_ARRAY_ACCESS) {
        // Formato específico NODE_ARRAY_ACCESS
        arrayName = arrayAccess->left;
        indexExpr = arrayAccess->right;
    } else if (arrayAccess->type == NODE_VAR && arrayAccess->right != NULL) {
        // Formato NODE_VAR com índice em 'right'
        arrayName = arrayAccess;
        indexExpr = arrayAccess->right;
    } else {
        DEBUG_IR("ERRO: formato de acesso a array desconhecido!");
        return;
    }
    
    if (arrayName == NULL || arrayName->value == NULL) {
        DEBUG_IR("ERRO: nome do array ausente ou inválido!");
        return;
    }
    
    DEBUG_IR("  Nome do array: %s", arrayName->value);
    
    // Calcula o índice
    char* indexResult = newTemp();
    genExprCode(indexExpr, indexResult);
    
    // Calcula o valor a ser armazenado
    char* valueResult = newTemp();
    genExprCode(arrayAssign->right, valueResult);
    
    // Armazena o valor no array
    genQuad(OP_ARRAY_STORE, valueResult, indexResult, arrayName->value);
}

// Gera código para declaração de variáveis (globais ou locais)
void genVarDeclCode(ASTNode* varDecl) {
    if (varDecl == NULL) return;
    currentSourceLine = varDecl->lineno;  // Atualiza com a linha da declaração
    
    // Extrai o nome da variável
    char* varName = varDecl->value;
    if (varName == NULL) {
        DEBUG_IR("ERRO: Declaração de variável sem nome!");
        return;
    }
    //se a variavel nao for do escopo global, retorna
    // char* varInfo = current_scope();
    // if (strcmp(varInfo,"global") != 0) {
    //     return;
    // }
    
    DEBUG_IR("Processando declaração de variável: %s (linha %d)", varName, currentSourceLine);
        
    // Verifica se é um array ou uma variável simples
    if (varDecl->arraySize > 0) {
        // É um array
        int size = varDecl->arraySize;
        if (size <= 0) {
            DEBUG_IR("ERRO: Tamanho de array inválido: %s", varDecl->right->value);
            return;
        }
        
        char sizeStr[12];
        sprintf(sizeStr, "%d", 4*size);
        
        DEBUG_IR("  Alocando array %s[%s]", varName, sizeStr);
        genQuad(OP_ALLOC, sizeStr, "array", varName);
    } else {
        // É uma variável simples (inteiro ou outro tipo)
        DEBUG_IR("  Alocando variável simples: %s", varName);
        genQuad(OP_ALLOC, "4", "var", varName);
    }
}

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
        
        case NODE_VAR_DECL:
            // Processa declaração de variáveis (globais ou locais)
            genVarDeclCode(node);
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

//otimização simples das quádruplas
void optimizeIRCode(void) {
    if (!irCode.head) return;

    // Mapa para armazenar o mapeamento de temporários dentro de cada função
    typedef struct {
        char* original;
        char* renamed;
    } TempMapping;
    
    TempMapping* tempMappings = NULL;
    int mappingCapacity = 0;
    int mappingCount = 0;

    Quadruple* current = irCode.head;
    while (current) {
        // Quando encontra o início de uma função, reinicia o mapeamento
        if (current->op == OP_FUNCTION) {
            // Limpa o mapeamento anterior
            for (int i = 0; i < mappingCount; i++) {
                free(tempMappings[i].original);
                free(tempMappings[i].renamed);
            }
            free(tempMappings);
            tempMappings = NULL;
            mappingCapacity = 0;
            mappingCount = 0;
            
            // Agora processa todas as quadruplas dentro da função
            Quadruple* funcPtr = current->next;
            int nextTempIndex = 0;
            
            while (funcPtr && funcPtr->op != OP_FUNCTION) {
                // Processa arg1 se for temporário
                if (funcPtr->arg1 && funcPtr->arg1[0] == 't' && funcPtr->arg1[1] != 'v' && isdigit(funcPtr->arg1[1])) {
                    // Verifica se já mapeamos esse temporário
                    int found = 0;
                    for (int i = 0; i < mappingCount; i++) {
                        if (strcmp(funcPtr->arg1, tempMappings[i].original) == 0) {
                            char* oldValue = funcPtr->arg1;
                            funcPtr->arg1 = strdup(tempMappings[i].renamed);
                            free(oldValue);
                            found = 1;
                            break;
                        }
                    }
                    
                    // Se não encontrou, cria um novo mapeamento
                    if (!found) {
                        // Verifica se precisamos realocar
                        if (mappingCount >= mappingCapacity) {
                            mappingCapacity = mappingCapacity == 0 ? 8 : mappingCapacity * 2;
                            tempMappings = realloc(tempMappings, mappingCapacity * sizeof(TempMapping));
                        }
                        
                        char newTemp[12];
                        sprintf(newTemp, "t%d", nextTempIndex++);
                        
                        tempMappings[mappingCount].original = strdup(funcPtr->arg1);
                        tempMappings[mappingCount].renamed = strdup(newTemp);
                        
                        char* oldValue = funcPtr->arg1;
                        funcPtr->arg1 = strdup(newTemp);
                        free(oldValue);
                        
                        mappingCount++;
                    }
                }
                
                // Repete para arg2
                if (funcPtr->arg2 && funcPtr->arg2[0] == 't' && funcPtr->arg2[1] != 'v' && isdigit(funcPtr->arg2[1])) {
                    int found = 0;
                    for (int i = 0; i < mappingCount; i++) {
                        if (strcmp(funcPtr->arg2, tempMappings[i].original) == 0) {
                            char* oldValue = funcPtr->arg2;
                            funcPtr->arg2 = strdup(tempMappings[i].renamed);
                            free(oldValue);
                            found = 1;
                            break;
                        }
                    }
                    
                    if (!found) {
                        if (mappingCount >= mappingCapacity) {
                            mappingCapacity = mappingCapacity == 0 ? 8 : mappingCapacity * 2;
                            tempMappings = realloc(tempMappings, mappingCapacity * sizeof(TempMapping));
                        }
                        
                        char newTemp[12];
                        sprintf(newTemp, "t%d", nextTempIndex++);
                        
                        tempMappings[mappingCount].original = strdup(funcPtr->arg2);
                        tempMappings[mappingCount].renamed = strdup(newTemp);
                        
                        char* oldValue = funcPtr->arg2;
                        funcPtr->arg2 = strdup(newTemp);
                        free(oldValue);
                        
                        mappingCount++;
                    }
                }
                
                // Repete para result
                if (funcPtr->result && funcPtr->result[0] == 't' && funcPtr->result[1] != 'v' && isdigit(funcPtr->result[1])) {
                    int found = 0;
                    for (int i = 0; i < mappingCount; i++) {
                        if (strcmp(funcPtr->result, tempMappings[i].original) == 0) {
                            char* oldValue = funcPtr->result;
                            funcPtr->result = strdup(tempMappings[i].renamed);
                            free(oldValue);
                            found = 1;
                            break;
                        }
                    }
                    
                    if (!found) {
                        if (mappingCount >= mappingCapacity) {
                            mappingCapacity = mappingCapacity == 0 ? 8 : mappingCapacity * 2;
                            tempMappings = realloc(tempMappings, mappingCapacity * sizeof(TempMapping));
                        }
                        
                        char newTemp[12];
                        sprintf(newTemp, "t%d", nextTempIndex++);
                        
                        tempMappings[mappingCount].original = strdup(funcPtr->result);
                        tempMappings[mappingCount].renamed = strdup(newTemp);
                        
                        char* oldValue = funcPtr->result;
                        funcPtr->result = strdup(newTemp);
                        free(oldValue);
                        
                        mappingCount++;
                    }
                }
                
                // Se chegou no final da função, sai do loop
                if (funcPtr->op == OP_END) break;
                funcPtr = funcPtr->next;
            }
        }
        
        // Otimizações existentes
        if (current->next) {
            Quadruple* next = current->next;
            
            // Caso 1: CALL seguido de ASSIGN (nova otimização)
            if (current->op == OP_CALL && next->op == OP_ASSIGN && 
                current->result && next->arg1 && next->result && 
                strcmp(current->result, next->arg1) == 0) {
                
                // Move o resultado diretamente para a variável de destino final
                free(current->result);
                current->result = strdup(next->result);
                
                // Desconecta a quadrupla de atribuição (redundante)
                current->next = next->next;
                
                // Libera a quadrupla redundante
                free(next->arg1);
                if (next->arg2) free(next->arg2);
                free(next->result);
                free(next);
                
                continue;
            }
            
            // Caso 2: ASSIGN seguido de PARAM com o mesmo resultado/argumento
            if (current->op == OP_ASSIGN && next->op == OP_ARGUMENT && 
                current->result && next->arg1 && 
                strcmp(current->result, next->arg1) == 0) {
                
                // Substitui o argumento do PARAM pelo argumento do ASSIGN
                free(next->arg1);
                next->arg1 = strdup(current->arg1);
                
                // Desconecta a quadrupla atual
                if (current == irCode.head) {
                    irCode.head = next;
                } else {
                    Quadruple* prev = irCode.head;
                    while (prev->next != current) prev = prev->next;
                    prev->next = next;
                }
                
                // Libera a quadrupla redundante
                free(current->arg1);
                if (current->arg2) free(current->arg2);
                free(current->result);
                free(current);
                
                current = next;
                continue;
            }
            
            // Caso 3: ASSIGN seguido de RETURN com o mesmo resultado/argumento
            if (current->op == OP_ASSIGN && next->op == OP_RETURN && 
                current->result && next->arg1 && 
                strcmp(current->result, next->arg1) == 0) {
                
                // Substitui o argumento do RETURN pelo argumento do ASSIGN
                free(next->arg1);
                next->arg1 = strdup(current->arg1);
                
                // Desconecta a quadrupla atual
                if (current == irCode.head) {
                    irCode.head = next;
                } else {
                    Quadruple* prev = irCode.head;
                    while (prev->next != current) prev = prev->next;
                    prev->next = next;
                }
                
                // Libera a quadrupla redundante
                free(current->arg1);
                if (current->arg2) free(current->arg2);
                free(current->result);
                free(current);
                
                current = next;
                continue;
            }
        }
        
        current = current->next;
    }
    
    // Libera memória do último mapeamento, se houver
    for (int i = 0; i < mappingCount; i++) {
        free(tempMappings[i].original);
        free(tempMappings[i].renamed);
    }
    free(tempMappings);
}

// Função de entrada para gerar o código intermediário
void ircode_generate(ASTNode* syntaxTree) {
    initIRCode();
    generateIRCode(syntaxTree);
    optimizeIRCode();  //  otimização do código intermediário
    printIRCode(stdout);
    printThreeAddressCode(stdout);  // Adiciona impressão do código de 3 endereços
}