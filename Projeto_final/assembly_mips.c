#include "assembly_mips.h"

// Ver o Operador (OP) a partir da tabela de quadruplas
OperationType getOpTypeFromString(const char* op) {
    if (strcmp(op, "ASSIGN") == 0) return OP_ASSIGN;
    if (strcmp(op, "ADD") == 0) return OP_ADD;
    if (strcmp(op, "SUB") == 0) return OP_SUB;
    if (strcmp(op, "MULT") == 0) return OP_MULT;
    if (strcmp(op, "DIV") == 0) return OP_DIV;
    if (strcmp(op, "LABEL") == 0) return OP_LABEL;
    if (strcmp(op, "JUMP") == 0) return OP_JUMP;
    if (strcmp(op, "JUMPFALSE") == 0) return OP_JUMPFALSE;
    if (strcmp(op, "JUMPTRUE") == 0) return OP_JUMPTRUE;
    if (strcmp(op, "FUNCTION") == 0) return OP_FUNCTION;
    if (strcmp(op, "RETURN") == 0) return OP_RETURN;
    if (strcmp(op, "END") == 0) return OP_END;
    if (strcmp(op, "EQ") == 0) return OP_EQ;
    if (strcmp(op, "NEQ") == 0) return OP_NEQ;
    if (strcmp(op, "LT") == 0) return OP_LT;
    if (strcmp(op, "GT") == 0) return OP_GT;
    if (strcmp(op, "LTE") == 0) return OP_LTE;  
    if (strcmp(op, "GTE") == 0) return OP_GTE;  
    if (strcmp(op, "PARAM") == 0) return OP_PARAM;
    if (strcmp(op, "ARGUMENT") == 0) return OP_ARGUMENT;
    if (strcmp(op, "CALL") == 0) return OP_CALL;
    if (strcmp(op, "ARRAY_LOAD") == 0) return OP_ARRAY_LOAD;
    if (strcmp(op, "ARRAY_STORE") == 0) return OP_ARRAY_STORE;
    if (strcmp(op, "ALLOC") == 0) return OP_ALLOC;
    return -1;
}

// Inicializa os mapeamentos de registradores
void initRegisterMappings() {
    int i;
    for (i = 0; i < 6; i++) { //os regs a seguir possuem 6 regs disponíveis
        paramRegs[i].isUsed = 0; //marca como não utilizados
        paramRegs[i].preserved = 0; // inicialmente, não precisam ser preservados
        argumentRegs[i].isUsed = 0;
        argumentRegs[i].preserved = 0;
    }
    for (i = 0; i < 2; i++) { //os regs a seguir possuem 2 regs disponíveis
        returnRegs[i].isUsed = 0; //marca como não utilizados
        returnRegs[i].preserved = 0; // inicialmente, não precisam ser preservados
    }
    for (i = 0; i < 27; i++) {
        tempLocalRegs[i].isUsed = 0;
    }
    for (i = 0; i < 13; i++) {
        tempGlobalRegs[i].isUsed = 0;
    }
}

// pega o próximo registrador livre
int getNextFreeReg(RegisterMapping* regs, int count) {
    // Primeiro procura por registradores não utilizados
    for (int i = 0; i < count; i++) {
        if (!regs[i].isUsed) {
            regs[i].isUsed = 1;
            return i;
        }
    }
    
    // Se não encontrar registrador livre, procura por um não preservado
    for (int i = 0; i < count; i++) {
        if (!regs[i].preserved) {
            // Podemos reutilizar este registrador não preservado
            DEBUG_ASSEMBLY("DEBUG - getNextFreeReg: Reutilizando registrador não-preservado %d\n", i);
            return i;
        }
    }
    
    // Se todos os registradores estão em uso e preservados, usamos o primeiro como fallback
    // spilling
    DEBUG_ASSEMBLY("DEBUG - getNextFreeReg: AVISO - Todos os registradores preservados, reutilizando reg 0\n");
    return 0;
}

// Função para obter o índice do registrador a partir do nome da variável
int getRegisterIndex(char* name) {
    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Procurando registrador para '%s'\n", name ? name : "NULL");
    
    if (name == NULL) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Nome NULL, retornando reg 0\n");
        return 0;
    }
    
    // Verifica se é um valor constante
    if (isdigit(name[0]) || (name[0] == '-' && isdigit(name[1]))) {
        // Usa um registrador temporário para constantes
        int tempIdx = getNextFreeReg(tempLocalRegs, 27);
        sprintf(tempLocalRegs[tempIdx].varName, "%s", name);
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Constante '%s' mapeada para reg temporário t%d (r%d)\n", 
               name, tempIdx, 4 + tempIdx);
        return 4 + tempIdx; // t0-t26
    }
    
    // Verifica na tabela de símbolos se é um parâmetro ou variável local
    BucketList symbol = NULL;
    
    // Primeiro procura no escopo atual (função atual)
    if (strlen(currentFunction) > 0) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Procurando '%s' no escopo '%s'\n", name, currentFunction);
        symbol = st_lookup_in_scope(name, currentFunction);
    }
    
    // Se não encontrar, procura no escopo global
    if (symbol == NULL) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Procurando '%s' no escopo global\n", name);
        symbol = st_lookup_in_scope(name, "global");
    }
    
    if (symbol != NULL) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Símbolo '%s' encontrado, tipo: '%s'\n", name, symbol->idType);
        
        // É um parâmetro?
        if (strcmp(symbol->idType, "param") == 0) {
            // Procurar se já mapeamos esse parâmetro
            for (int i = 0; i < 6; i++) {
                if (paramRegs[i].isUsed && strcmp(paramRegs[i].varName, name) == 0) {
                    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Parâmetro '%s' já mapeado para a%d (r%d)\n", 
                           name, i, 53 + i);
                    return 53 + i; // p0-p5
                }
            }
            
            // Novo parâmetro, mapeia para o próximo registrador livre
            int paramIdx = getNextFreeReg(paramRegs, 6);
            sprintf(paramRegs[paramIdx].varName, "%s", name);
            DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Novo parâmetro '%s' mapeado para a%d (r%d)\n", 
                   name, paramIdx, 53 + paramIdx);
            return 53 + paramIdx; // p0-p5
        }
        // É uma variável local?
        else if (strcmp(symbol->idType, "var") == 0 && strcmp(symbol->scope, currentFunction) == 0) {
            // Procurar se já mapeamos essa variável
            for (int i = 0; i < 27; i++) {
                if (tempLocalRegs[i].isUsed && strcmp(tempLocalRegs[i].varName, name) == 0) {
                    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Variável local '%s' já mapeada para t%d (r%d)\n", 
                           name, i, 4 + i);
                    return 4 + i; // t0-t26
                }
            }
            
            // Nova variável local, mapeia para o próximo registrador livre
            int tempIdx = getNextFreeReg(tempLocalRegs, 27);
            sprintf(tempLocalRegs[tempIdx].varName, "%s", name);
            DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Nova variável local '%s' mapeada para t%d (r%d)\n", 
                   name, tempIdx, 4 + tempIdx);
            return 4 + tempIdx; // t0-t26
        }
        else if (strcmp(symbol->idType, "var") == 0 && strcmp(symbol->scope, "global") == 0){
            // se a variavel for global
            for (int i = 0; i < 13; i++) {
                if (tempGlobalRegs[i].isUsed && strcmp(tempGlobalRegs[i].varName, name) == 0) {
                    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Variável global '%s' já mapeada para t%d (r%d)\n", 
                           name, i, 32 + i);
                    return 32 + i; // tg0-tg12
                }
            }
            // Nova variável global, mapeia para o próximo registrador livre
            int tempIdx = getNextFreeReg(tempGlobalRegs, 13);
            sprintf(tempGlobalRegs[tempIdx].varName, "%s", name);
            DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Nova variável local '%s' mapeada para t%d (r%d)\n", 
                   name, tempIdx, 32 + tempIdx);
            return 32 + tempIdx; // tg0-tg12
        }
    } else {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Símbolo '%s' não encontrado na tabela de símbolos\n", name);
    }
    
    // Temporários genéricos (geralmente variáveis com nome tX)
    if (name[0] == 't' && isdigit(name[1])) {
        int tempNum = atoi(&name[1]);
        int reg = 4 + (tempNum % 27); // Use todos os 27 registradores temporários
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Temporário '%s' mapeado ciclicamente para t%d (r%d)\n", 
               name, tempNum % 27, reg);
        return reg; // Mapeia para t0-t26 de forma cíclica
    }

    // Valores de retorno
    if (strncmp(name, "ret", 3) == 0 || strstr(name, "return") != NULL) {
        int retIdx = getNextFreeReg(returnRegs, 2);
        sprintf(returnRegs[retIdx].varName, "%s", name);
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Valor de retorno '%s' mapeado para v%d (r%d)\n", 
               name, retIdx, 45 + retIdx);
        return 45 + retIdx; // v0-v1
    }
    
    // Fallback: usa registrador do kernel
    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Fallback para '%s', usando registrador kernel r59\n", name);
    return 59; //kernel r59-r62
}

// typedef struct {
//     char labelName[64];
//     int varCount;
//     int varRegisters[32]; // Armazena registradores das variáveis alocadas na label
// } LabelScope;

// LabelScope currentLabel = {"", 0};
// char lastAllocLabel[64] = "";

// // Adicione esta função para limpar as variáveis alocadas em uma label
// void deallocateLabelVars(FILE* output, int* lineIndex, int* stackOffset) {
//     if (currentLabel.varCount > 0) {
//         fprintf(output, "%d - # Desalocando %d variáveis da label %s\n", 
//                 (*lineIndex)++, currentLabel.varCount, currentLabel.labelName);
        
//         // Libera espaço na pilha para todas as variáveis alocadas
//         int totalSpace = currentLabel.varCount * 4;
//         fprintf(output, "%d - addi $r1 $r1 %d # libera espaço das variáveis locais da label\n", 
//                 (*lineIndex)++, totalSpace);
//         *stackOffset += totalSpace;
        
//         // Reset do contador de variáveis da label
//         currentLabel.varCount = 0;
//     }
// }

// Atualiza a função atual e reinicia os mapeamentos de registradores
void updateCurrentFunction(const char* funcName) {
    if (funcName != NULL) {
        strncpy(currentFunction, funcName, sizeof(currentFunction) - 1);
        currentFunction[sizeof(currentFunction) - 1] = '\0';
        
        // Reinicia mapeamentos ao mudar de função
        initRegisterMappings();
    }
}

// Função auxiliar para verificar se a próxima quádrupla é um JUMPTRUE ou JUMPFALSE
void checkNextQuadruple(FILE* inputFile, long* filePos, QuadrupleInfo* nextQuad) {
    char buffer[256];
    
    // Salva a posição atual no arquivo
    *filePos = ftell(inputFile);
    
    // Inicializa a próxima quadrupla
    strcpy(nextQuad->op, "");
    strcpy(nextQuad->arg1, "-");
    strcpy(nextQuad->arg2, "-");
    strcpy(nextQuad->result, "-");
    
    // Tenta ler a próxima linha válida (não vazia, não separador)
    while (fgets(buffer, sizeof(buffer), inputFile) != NULL) {
        // Ignora linhas de separação e cabeçalho
        if (strstr(buffer, "---") != NULL || 
            strstr(buffer, "Quad") != NULL ||
            strlen(buffer) <= 1) {
            continue;
        }
        
        // Analisa a próxima linha para extrair os campos
        sscanf(buffer, "%d %d %s %s %s %s",
               &nextQuad->line, &nextQuad->sourceLine, 
               nextQuad->op, nextQuad->arg1, nextQuad->arg2, nextQuad->result);
        break;
    }
    
    // Volta para a posição original no arquivo
    fseek(inputFile, *filePos, SEEK_SET);
}

// Empurra um registrador na pilha
void pushRegister(FILE* output, int reg, int* stackOffset, int* lineIndex) {
    (*stackOffset) -= 4;
    fprintf(output, "%d - addi $r1 $r1 -4      # aloca espaço na pilha\n", (*lineIndex)++);
    fprintf(output, "%d - sw $r%d 0($r1)       # empilha registrador\n", (*lineIndex)++, reg);
    DEBUG_ASSEMBLY("DEBUG - pushRegister: Empilhando r%d, stack offset agora é %d\n", reg, *stackOffset);
}

// Restaura um registrador da pilha
void popRegister(FILE* output, int reg, int* stackOffset, int* lineIndex) {
    fprintf(output, "%d - lw $r%d 0($r1)       # desempilha registrador\n", (*lineIndex)++, reg);
    (*stackOffset) += 4;
    fprintf(output, "%d - addi $r1 $r1 4       #desaloca espaço na pilha\n", (*lineIndex)++);
    DEBUG_ASSEMBLY("DEBUG - popRegister: Desempilhando para r%d, stack offset agora é %d\n", reg, *stackOffset);
}

void analyzeRegisterUsage(const char* assemblyFilePath) {
    FILE* assemblyFile = fopen(assemblyFilePath, "r");
    if (!assemblyFile) {
        fprintf(stderr, "Erro: Não foi possível abrir o arquivo assembly para análise: %s\n", assemblyFilePath);
        return;
    }
    
    // Estrutura para armazenar informações sobre o uso de registradores
    typedef struct {
        int isUsed;
        int firstUsedAt;
        int lastUsedAt;
        int useCount;
        char purpose[64];
        char regName[10]; // Nome do registrador (t0, a0, v0, etc.)
    } RegUsageInfo;
    
    // Array para armazenar análise de todos os registradores
    RegUsageInfo regUsage[64] = {0};
    
    // Inicializar os nomes dos registradores
    for (int i = 0; i < 64; i++) {
        if (i == 0)
            strcpy(regUsage[i].regName, "out");
        else if (i == 1)
            strcpy(regUsage[i].regName, "sp");
        else if (i == 2)
            strcpy(regUsage[i].regName, "fp");
        else if (i == 31)
            strcpy(regUsage[i].regName, "ra");
        else if (i == 63)
            strcpy(regUsage[i].regName, "zero");
        else if (i == 3)
            strcpy(regUsage[i].regName, "in");
        else if (i >= 4 && i <= 30)
            sprintf(regUsage[i].regName, "tl%d", i - 4);
        else if (i >= 32 && i <= 44)
            sprintf(regUsage[i].regName, "tg%d", i - 32);
        else if (i >= 45 && i <= 46)
            sprintf(regUsage[i].regName, "v%d", i - 45);
        else if (i >= 47 && i <= 52)
            sprintf(regUsage[i].regName, "a%d", i - 47);
        else if (i >= 53 && i <= 58)
            sprintf(regUsage[i].regName, "p%d", i - 53);
        else if (i >= 59 && i <= 62)
            sprintf(regUsage[i].regName, "k%d", i - 59);
    }
    
    char buffer[256];
    int lineNum = 0;
    char currentFunction[64] = "";
    
    while (fgets(buffer, sizeof(buffer), assemblyFile) != NULL) {
        lineNum++;
        
        // Verificar se estamos mudando de função
        char functionName[64];
        if (sscanf(buffer, "%*d - %[^:]:", functionName) == 1 && 
            strstr(buffer, "# nova função") != NULL) {
            strcpy(currentFunction, functionName);
            printf("\nAnalisando função: %s\n", currentFunction);
        }
        
        // Extrair registradores usados na instrução
        for (int i = 0; i < 64; i++) {
            char regPattern[10];
            sprintf(regPattern, "$r%d", i);
            
            // Verifica se o registro é realmente usado na instrução, não apenas mencionado em comentário
            char* regPos = strstr(buffer, regPattern);
            if (regPos != NULL) {
                // Verifica se o registro aparece antes de qualquer "#" (comentário)
                char* commentPos = strstr(buffer, "#");
                
                // Se não há comentário ou o registro aparece antes do comentário
                if (commentPos == NULL || regPos < commentPos) {
                    // Verifica se é um verdadeiro uso (não parte de outro nome de registro)
                    // Ex: $r3 vs $r30 - queremos detectar $r3, mas não como parte de $r30
                    char nextChar = *(regPos + strlen(regPattern));
                    if (nextChar == ' ' || nextChar == ',' || nextChar == '\n' || 
                        nextChar == ')' || nextChar == '\0' || nextChar == '\r') {
                        
                        // Atualiza informações de uso do registrador
                        if (!regUsage[i].isUsed) {
                            regUsage[i].isUsed = 1;
                            regUsage[i].firstUsedAt = lineNum;
                            
                            // Determinar finalidade baseado no registrador
                            if (i == 0) strcpy(regUsage[i].purpose, "output");
                            else if (i == 1) strcpy(regUsage[i].purpose, "stack pointer");
                            else if (i == 2) strcpy(regUsage[i].purpose, "frame pointer");
                            else if (i == 31) strcpy(regUsage[i].purpose, "return address");
                            else if (i == 63) strcpy(regUsage[i].purpose, "zero constant");
                            else if (i == 3) strcpy(regUsage[i].purpose, "input");
                            else if (i >= 4 && i <= 30) strcpy(regUsage[i].purpose, "temporary local");
                            else if (i >= 32 && i <= 44) strcpy(regUsage[i].purpose, "temporary global");
                            else if (i >= 45 && i <= 46) strcpy(regUsage[i].purpose, "return value");
                            else if (i >= 47 && i <= 52) strcpy(regUsage[i].purpose, "argument");
                            else if (i >= 53 && i <= 58) strcpy(regUsage[i].purpose, "param");
                            else if (i >= 59 && i <= 62) strcpy(regUsage[i].purpose, "kernel register");
                        }
                        
                        regUsage[i].lastUsedAt = lineNum;
                        regUsage[i].useCount++;
                    }
                }
            }
        }
    }
    
    // Imprime relatório de uso de registradores
    printf("\n=== RELATÓRIO DE USO DE REGISTRADORES ===\n");
    printf("Reg  | Nome | Usado | Primeira | Última | Contagem | Finalidade\n");
    printf("-----+------+-------+----------+--------+----------+------------\n");
    
    for (int i = 0; i < 64; i++) {
        if (regUsage[i].isUsed) {
            printf("$r%-2d | %-4s | Sim   | %-8d | %-6d | %-8d | %s\n", 
                  i, regUsage[i].regName, regUsage[i].firstUsedAt-1, regUsage[i].lastUsedAt-1, 
                  regUsage[i].useCount, regUsage[i].purpose);
        }
    }
    
    fclose(assemblyFile);
    printf("\nAnálise de registradores concluída.\n");
}

// Calcula o deslocamento de um parâmetro na pilha
int getParameterOffset(int paramIndex) {
    return 4 * (paramIndex + 1); // +1 por causa do fp atual
}

// Carrega um parâmetro da pilha para um registrador
void loadParameter(FILE* output, int paramIndex, int destReg, int* lineIndex) {
    int offset = 4 * (paramIndex + 2); // +2 para o endereço de retorno e o frame pointer salvo
    fprintf(output, "%d - lw $r%d %d($r2)      # carrega param %d\n", (*lineIndex)++, destReg, offset, paramIndex);
    DEBUG_ASSEMBLY("DEBUG - loadParameter: Carregando parâmetro %d do offset %d para r%d\n", 
           paramIndex, offset, destReg);
}

// Salva o frame atual na entrada de uma função
void setupFrame(FILE* output, int* lineIndex, int* stackOffset) {
    // Salva o endereço de retorno
    pushRegister(output, 31, stackOffset, lineIndex);  // RA
    
    // Salva o frame pointer atual
    pushRegister(output, 2, stackOffset, lineIndex);   // FP
    
    DEBUG_ASSEMBLY("DEBUG - setupFrame: Frame configurado, SP=%d, FP=SP\n", *stackOffset);
}

// Restaura o frame na saída de uma função
void restoreFrame(FILE* output, int* lineIndex, int* stackOffset) {
    // Restaura o stack pointer para o frame pointer atual
    fprintf(output, "%d - move $r1 $r2         # restaura stack pointer\n", (*lineIndex)++);
    
    // Restaura o frame pointer antigo
    popRegister(output, 2, stackOffset, lineIndex);    // FP
    
    // Restaura o endereço de retorno
    popRegister(output, 31, stackOffset, lineIndex);   // RA
    
    DEBUG_ASSEMBLY("DEBUG - restoreFrame: Frame restaurado, SP=%d\n", *stackOffset);
}



// Nova função: Aloca espaço para parâmetros de funções
void allocateArgumentSpace(FILE* output, int argumentCount, int* lineIndex, int* stackOffset) {
    if (argumentCount > 0) {
        int totalSize = argumentCount * 4;  // Cada parâmetro ocupa 4 bytes
        
        // fprintf(output, "%d - addi $r1 $r1 -%d  # aloca espaço para %d argumentos\n", 
        //         (*lineIndex)++, totalSize, argumentCount);
        
        *stackOffset -= totalSize;
        DEBUG_ASSEMBLY("DEBUG - allocateArgumentSpace: Alocados %d bytes para %d argumentos, stack offset agora é %d\n", 
               totalSize, argumentCount, *stackOffset);
    }
}

// Função para contar quantos argumentos existem até encontrar uma chamada de função
int countArgumentsUntilCall(FILE* inputFile, long currentPos) {
    char buffer[256];
    int count = 0;
    long savedPos = ftell(inputFile); // Salva a posição atual no arquivo
    
    // Posiciona o ponteiro do arquivo na posição fornecida
    fseek(inputFile, currentPos, SEEK_SET);
    
    // Lê as quádruplas até encontrar um CALL ou EOF
    while (fgets(buffer, sizeof(buffer), inputFile) != NULL) {
        // Ignora linhas de separação e cabeçalho
        if (strstr(buffer, "---") != NULL || 
            strstr(buffer, "Quad") != NULL ||
            strlen(buffer) <= 1) {
            continue;
        }
        
        // Extrai o operador da quádrupla
        char op[32] = "";
        int line, sourceLine;
        if (sscanf(buffer, "%d %d %s", &line, &sourceLine, op) < 3) {
            continue;
        }
        
        if (strcmp(op, "ARGUMENT") == 0) {
            count++;
        } else if (strcmp(op, "CALL") == 0) {
            // Encontrou a chamada de função, termina a contagem
            break;
        }
    }
    
    // Retorna à posição original no arquivo
    fseek(inputFile, savedPos, SEEK_SET);
    
    return count;
}

// Função principal para gerar o código assembly a partir do arquivo de entrada
void generateAssembly(FILE* inputFile) {
    FILE* output = fopen("Output/assembly.asm", "w");
    // Inicializa os mapeamentos de registradores
    initRegisterMappings();
    
    // Variável para controlar o deslocamento da pilha
    int stackOffset = 0;
    
    // Ignora o cabeçalho (4 linhas)
    char buffer[256];
    for (int i = 0; i < 4; i++) {
        if (fgets(buffer, sizeof(buffer), inputFile) == NULL) {
            fprintf(stderr, "Erro: Formato de arquivo inesperado.\n");
            fclose(inputFile);
            fclose(output);
            return;
        }
    }

    QuadrupleInfo quad, nextQuad;
    int lineIndex = 0;
    long filePos;
    int argumentCount = 0;  // Contador para parâmetros de função
    int varLocalCount = 0;
    int varGlobalCount = 0;
    int paramCount = 0;
    int parameters[6];
    int localVars[27];
    int globalVars[13];
    int rvet; 
    int rindice;
    int rbase;

    fprintf(output, "%d - nop 1\n", lineIndex++); //nop limpa os sinais e pula para a instrução 1
    int ehPrimeiraFuncao = 1;

    // Lê as quádruplas do arquivo e gera o código assembly
    while (fgets(buffer, sizeof(buffer), inputFile) != NULL) {
        // Ignora linhas de separação e cabeçalho
        if (strstr(buffer, "---") != NULL || 
            strstr(buffer, "Quad") != NULL ||
            strlen(buffer) <= 1) {
            continue;
        }

        // Inicializa com valores padrão
        quad.line = 0;
        quad.sourceLine = 0;
        strcpy(quad.op, "");
        strcpy(quad.arg1, "-");
        strcpy(quad.arg2, "-");
        strcpy(quad.result, "-");

        // Analisa a linha para extrair os campos da quádrupla
        sscanf(buffer, "%d %d %s %s %s %s",
               &quad.line, &quad.sourceLine, quad.op, quad.arg1, quad.arg2, quad.result);

        // Atualiza a função atual quando encontra uma definição de função
        if (strcmp(quad.op, "FUNCTION") == 0) {
            updateCurrentFunction(quad.arg1);
            argumentCount = 0;  // Reset parameter count for new function
            varLocalCount = 0;  // Reset local variable count for new function
            paramCount = 0;  // Reset parameter count for new function
        }

        // Processa a quádrupla lida, para saber o operador e os index
        OperationType opType = getOpTypeFromString(quad.op);
        int r1 = strcmp(quad.arg1, "-") != 0 ? getRegisterIndex(quad.arg1) : 0;
        int r2 = strcmp(quad.arg2, "-") != 0 ? getRegisterIndex(quad.arg2) : 0;
        int r3 = strcmp(quad.result, "-") != 0 ? getRegisterIndex(quad.result) : 0;

        // Para operações de comparação, olha a próxima quádrupla para otimizar
        if (opType == OP_EQ || opType == OP_NEQ || opType == OP_LT || opType == OP_GT || 
            opType == OP_LTE || opType == OP_GTE) {
            
            // Verifica a próxima quádrupla para ver se é um salto condicional
            checkNextQuadruple(inputFile, &filePos, &nextQuad);
            OperationType nextOpType = getOpTypeFromString(nextQuad.op);
            
            // Se a próxima op for um salto condicional, otimiza para um único jump
            if (nextOpType == OP_JUMPFALSE || nextOpType == OP_JUMPTRUE) {
                switch (opType) {
                    case OP_EQ:
                        if (nextOpType == OP_JUMPFALSE) {
                            // Debug information to verify register mapping
                            DEBUG_ASSEMBLY("DEBUG - EQ operation: comparing %s (%d) == %s (%d)\n", 
                                quad.arg1, r1, quad.arg2, r2);
                            
                            // Se EQ é falso (valores diferentes), pula para o label
                            fprintf(output, "%d - bne $r%d $r%d %s # jump se %s != %s\n", 
                                    lineIndex++, r1, r2, nextQuad.result, quad.arg1, quad.arg2);
                        } else { // JUMPTRUE
                            // Se EQ é verdadeiro (valores iguais), pula para o label
                            fprintf(output, "%d - beq $r%d $r%d %s # jump se %s == %s\n", 
                                    lineIndex++, r1, r2, nextQuad.result, quad.arg1, quad.arg2);
                        }
                        break;
                        
                    case OP_NEQ:
                        if (nextOpType == OP_JUMPFALSE) {
                            // Se NEQ é falso (valores iguais), pula para o label
                            fprintf(output, "%d - beq $r%d $r%d %s # jump se ==\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        } else { // JUMPTRUE
                            // Se NEQ é verdadeiro (valores diferentes), pula para o label
                            fprintf(output, "%d - bne $r%d $r%d %s # jump se !=\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        }
                        break;
                        
                    case OP_LT:
                        if (nextOpType == OP_JUMPFALSE) {
                            // Se LT é falso (>=), pula para o label
                            fprintf(output, "%d - bge $r%d $r%d %s # jump >=\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        } else { // JUMPTRUE
                            // Se LT é verdadeiro (<), pula para o label
                            fprintf(output, "%d - blt $r%d $r%d %s # jump se <\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        }
                        break;
                        
                    case OP_GT:
                        if (nextOpType == OP_JUMPFALSE) {
                            // Se GT é falso (<=), pula para o label
                            fprintf(output, "%d - ble $r%d $r%d %s # jump se <=\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        } else { // JUMPTRUE
                            // Se GT é verdadeiro (>), pula para o label
                            fprintf(output, "%d - bgt $r%d $r%d %s # jump se >\n",
                                    lineIndex++, r1, r2, nextQuad.result);
                        }
                        break;
                        
                    case OP_LTE:  // <=
                        if (nextOpType == OP_JUMPFALSE) {
                            // Se BLTE é falso (>), pula para o label
                            fprintf(output, "%d - bgt $r%d $r%d %s # jump se >\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        } else { // JUMPTRUE
                            // Se BLTE é verdadeiro (<=), pula para o label
                            fprintf(output, "%d - ble $r%d $r%d %s # jump se <=\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        }
                        break;
                        
                    case OP_GTE:  // >=
                        if (nextOpType == OP_JUMPFALSE) {
                            // Se BGTE é falso (<), pula para o label
                            fprintf(output, "%d - blt $r%d $r%d %s # jump se <\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        } else { // JUMPTRUE
                            // Se BGTE é verdadeiro (>=), pula para o label
                            fprintf(output, "%d - bge $r%d $r%d %s # jump se >=\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        }
                        break;
                        default:
                            fprintf(output, "%d - ; operação relacional não suportada\n", lineIndex++);
                            break;
                }
                
                // Avança para consumir a próxima quadrupla (JUMPFALSE/JUMPTRUE)
                // já que ela foi processada em conjunto com a operação de comparação
                fgets(buffer, sizeof(buffer), inputFile);
                continue;
            } 
            continue;
        }

        // Se estamos carregando uma constante
        if (strcmp(quad.op, "ASSIGN") == 0 && isdigit(quad.arg1[0])) {
            fprintf(output, "%d - li $r%d %s\n", lineIndex++, r3, quad.arg1);
            continue;
        }

        switch (opType) {
            case OP_ASSIGN:
                // Verifica se é uma movimentação redundante (mesmo registrador fonte e destino)
                if (r1 != r3) {
                    fprintf(output, "%d - move $r%d $r%d # copiando %s para %s\n", lineIndex++, r3, r1, quad.arg1, quad.result);
                }
                break;

            case OP_ADD:
                fprintf(output, "%d - add $r%d $r%d $r%d\n", lineIndex++, r3, r1, r2);
                break;

            case OP_SUB:
                fprintf(output, "%d - sub $r%d $r%d $r%d\n", lineIndex++, r3, r1, r2);
                break;

            case OP_MULT:
                fprintf(output, "%d - mul $r%d $r%d $r%d\n", lineIndex++, r3, r1, r2);
                break;

            case OP_DIV:
                fprintf(output, "%d - div $r%d $r%d $r%d\n", lineIndex++, r3, r1, r2);
                break;

            case OP_LABEL:
                fprintf(output, "%d - %s:\n", lineIndex++, quad.result);
                // Rastrear início de uma nova label
                // strncpy(currentLabel.labelName, quad.result, sizeof(currentLabel.labelName) - 1);
                // currentLabel.varCount = 0; // Reset do contador de variáveis para nova label
                break;

            case OP_JUMP:
                // Desaloca variáveis antes do jump se não for um loop
                // if (strstr(quad.result, currentLabel.labelName) == NULL) {
                //     // Não é um salto para a própria label (não é um loop)
                //     deallocateLabelVars(output, &lineIndex, &stackOffset);
                // }
                fprintf(output, "%d - j %s\n", lineIndex++, quad.result);
                break;

            case OP_JUMPFALSE:
                // Jump se o valor é falso (igual a zero)
                fprintf(output, "%d - beq $r%d $r63 %s # jump se é falso\n", 
                        lineIndex++, r1, quad.result);
                break;

            case OP_JUMPTRUE:
                // Jump se o valor é verdadeiro (diferente de zero)
                fprintf(output, "%d - bne $r%d $r63 %s # jump se é verdadeiro\n", 
                        lineIndex++, r1, quad.result);
                break;

            case OP_FUNCTION:
                if(ehPrimeiraFuncao){
                    fprintf(output, "%d - j main\n", lineIndex++); //começa na main
                    ehPrimeiraFuncao = 0; // marca que já processou a primeira função
                }
                fprintf(output, "%d - %s: # nova função\n", lineIndex++, quad.arg1);
                
                // Configura o frame da função usando nossa nova função
                setupFrame(output, &lineIndex, &stackOffset);
                
                checkNextQuadruple(inputFile, &filePos, &nextQuad);
                fprintf(output, "%d - move $r2 $r1         # fp = sp\n", lineIndex++);
                
                // Aloca espaço para parâmetros no início da função
                // Será atualizado quando encontrarmos todos os parâmetros
                argumentCount = 0;
                varLocalCount = 0;
                paramCount = 0;
                break;

            case OP_RETURN:
                // Carrega valor de retorno em v0 (r45)
                if (r1 != 45) { // Evita move redundante se o valor já estiver em v0
                    fprintf(output, "%d - move $r45 $r%d # move valor de retorno para v0\n", lineIndex++, r1);
                }
                
                // Restaura o frame usando nossa nova função
                restoreFrame(output, &lineIndex, &stackOffset);
                
                fprintf(output, "%d - jr $r31         # retorna\n", lineIndex++);
                break;

            case OP_END:
                fprintf(output, "%d - # fim da função %s\n", lineIndex++, quad.arg1);
                break;

            case OP_ARGUMENT: // Alterado de OP_PARAM para OP_ARGUMENT
                {
                    
                    checkNextQuadruple(inputFile, &filePos, &nextQuad);
                    int argumentNum = atoi(quad.arg2);
                    int destReg = 47 + argumentNum; // a0, a1, etc.
                    // Verifica se o argumento é uma variável local (armazenada na memória)
                    BucketList symbol = NULL;
                    if (strlen(currentFunction) > 0) {
                        symbol = st_lookup_in_scope(quad.arg1, currentFunction);
                    }
                    
                    // Atualiza a contagem de parâmetros se necessário
                    if (argumentNum >= argumentCount) {
                        if(strcmp(nextQuad.arg1,"output") !=0){
                            argumentCount = argumentNum + 1; // +1 porque os parâmetros começam do zero
                        }
                    }
                    
                    // Verifica se é o primeiro argumento (arg0) e conta o total até o CALL
                    if (argumentNum == 0) {
                        long currentPos = ftell(inputFile);
                        int totalArgs = countArgumentsUntilCall(inputFile, currentPos);
                        
                        if(strcmp(nextQuad.arg1,"output") !=0){
                            // Aloca espaço para todos os argumentos de uma vez
                            fprintf(output, "%d - addi $r1 $r1 -%d  # aloca espaço para %d argumentos\n", 
                                    lineIndex++, (totalArgs+1) * 4, totalArgs+1);
                            stackOffset -= ((totalArgs+1) * 4);
                            
                            DEBUG_ASSEMBLY("DEBUG - OP_ARGUMENT: Alocados %d bytes para %d argumentos\n", 
                                (totalArgs+1) * 4, totalArgs);
                        }
                    }
                    
                    //se o argumento é uma constante, carrega o valor em um registrador primeiro
                    if (isdigit(quad.arg1[0])) {
                        fprintf(output, "%d - li $r%d %s # argument %d: %s\n", lineIndex++, destReg, quad.arg1, argumentNum, quad.arg1);
                    }
                    // Se o argumento é uma variável local, precisamos carregar seu valor da memória
                    else if (symbol != NULL && strcmp(symbol->idType, "var") == 0) {
                        // Primeiro carrega o valor da memória 
                        // Carrega o endereço da variável (que já está em r1) para o registrador de argumento
                        fprintf(output, "%d - lw $r%d 0($r%d)    # carrega valor da variável local '%s' para argument %d\n", 
                                lineIndex++, destReg, r1, quad.arg1, argumentNum);
                    } 
                    //precisa ver a variavel global varglobalcount $gp 62
                    
                    else{
                        if (destReg != r1) {
                            fprintf(output, "%d - move $r%d $r%d # argument %d: %s\n", 
                                    lineIndex++, destReg, r1, argumentNum, quad.arg1);
                        } else {
                            fprintf(output, "%d - # argument %d: %s já está em $r%d\n", 
                                    lineIndex++, argumentNum, quad.arg1, destReg);
                        }
                    }
                    if(strcmp(nextQuad.arg1,"output") !=0){
                        fprintf(output, "%d - sw $r%d %d($r1)  # salva argument %d na pilha\n", 
                                lineIndex++, destReg, argumentNum * 4, argumentNum);
                    }
                }
                break;

            case OP_PARAM:
                // Carrega o parâmetro da pilha para o registrador correspondente
                
                parameters[paramCount] = r1; // Armazena o nome do parâmetro
                printf("parametro %d, index %d\n", parameters[paramCount], paramCount);
                printf("quad.arg1 %s\n", quad.arg1);
                loadParameter(output, paramCount++, r1, &lineIndex);
                checkNextQuadruple(inputFile, &filePos, &nextQuad);
                break;

            case OP_CALL:
                if (strcmp(quad.arg1, "input") == 0) {
                    // input() → in $rX
                    fprintf(output, "%d - in $r3\n", lineIndex++);
                    if(quad.result[0] == 't'){
                        tempLocalRegs[r3-4].isUsed = 1;
                        fprintf(output, "%d - move $r%d $r3 # move valor de input para %s\n", 
                                lineIndex++, r3, quad.result);
                    } else {
                        fprintf(output, "%d - sw $r3 0($r%d) # armazena em %s\n", 
                             lineIndex++, r3, quad.result);
                    }
                } else if (strcmp(quad.arg1, "output") == 0) {
                    // Para output, usamos o registro do parâmetro passado (a0)
                    fprintf(output, "%d - move $r0 $r47\n", lineIndex++); // out r0 (registrador reservado para output)
                    fprintf(output, "%d - out $r0\n", lineIndex++); // out r0 (registrador reservado para output)
                    // fprintf(output, "%d - addi $r1 $r1 4 # desaloca espaço na pilha\n", lineIndex++);
                } else {
                    // Aloca espaço para os argumentos na pilha antes da chamada
                    int argCount = atoi(quad.arg2);
                    if (argCount > 0) {
                        allocateArgumentSpace(output, argCount, &lineIndex, &stackOffset);
                    }
                    

                    // Salva registradores que podem ser modificados pela chamada de função
                    // saveCallerSavedRegs(output, &lineIndex, &stackOffset);
                    
                    // Chamada normal de função
                    fprintf(output, "%d - jal %s\n", lineIndex++, quad.arg1);
                    // Libera espaço dos argumentos após chamada
                    
                    if (argCount > 0) {
                        int totalSize = argCount * 4;
                        fprintf(output, "%d - addi $r1 $r1 %d  # libera espaço de %d argumentos\n", 
                                lineIndex++, totalSize, argCount);
                        stackOffset += totalSize;
                    }

                    if (varGlobalCount > 0){
                        //basta acessar o registrador global desejado
                    }
                    
                    if (paramCount > 0) {
                        // Carrega os parâmetros da pilha para os registradores correspondentes
                        for (int i = 0; i < paramCount; i++) {
                            printf("parametro %d, registrador\n", parameters[i]);
                            // int rx = getRegisterIndex(parameters[i]);
                            loadParameter(output, i, parameters[i], &lineIndex);
                        }
                    }

                    if(varLocalCount > 0){
                     //recarregar variaveis locais 
                     fprintf(output, "%d - move $r1 $r2 # sp = fp\n", lineIndex++);
                     // Carrega os parâmetros da pilha para os registradores correspondentes
                        for (int i = 0; i < varLocalCount; i++) {
                            fprintf(output, "%d - addi $r1 $r1 -4 # desce na pilha\n", lineIndex++);
                            fprintf(output, "%d - lw $r%d 0($r1) # recarrega variavel local\n", lineIndex++, localVars[i]);
                        }
                    }
                    
                    // Copia o valor de retorno (v0) para o resultado, verifica se não é redundante
                    if (strcmp(quad.result, "-") != 0 && r3 != 45) { // r45 é v0, evita move para o mesmo registrador
                        if (strncmp(quad.result, "tv", 2) == 0 && isdigit(quad.result[2])) {
                            // Função void, não gera instrução de move, apenas adiciona comentário
                            fprintf(output, "%d - # Chamada void para função %s, sem valor de retorno\n", 
                                    lineIndex++, quad.arg1);
                        } else {
                            fprintf(output, "%d - move $r%d $r45 # copia retorno (v0) para %s\n", 
                                        lineIndex++, r3, quad.result);
                        } 
                    }
                }
                break;

            case OP_ARRAY_LOAD:
                // Instrução para carregar um elemento de um array: result = arg1[arg2]
                // Formato da quádrupla: ARRAY_LOAD arg1 arg2 result
                // arg1 = nome do array (endereço base)
                // arg2 = índice
                // result = variável de destino
                
                // Usamos um registrador temporário para calcular o endereço
                rvet = getRegisterIndex(quad.arg1); //precisa ser referente ao local do vetor e nao o conteudo
                rindice = getRegisterIndex(quad.arg2);
                rbase = getNextFreeReg(tempLocalRegs, 27) + 4;
                tempLocalRegs[rbase-4].isUsed = 1;
                fprintf(output, "%d - addi $r%d $r%d 1 # índice + 1\n", lineIndex++, rindice, rindice);
                fprintf(output, "%d - mul $r%d $r%d -4      # índice * 4 (tamanho do inteiro)\n", lineIndex++, rindice, rindice);
                fprintf(output, "%d - addi $r%d $r%d r%d    # endereço base + deslocamento\n", lineIndex++, rbase, rvet, rindice);
                fprintf(output, "%d - lw $r%d 0($r%d)      # carrega %s[%s] em %s\n", lineIndex++, r3, rbase, quad.arg1, quad.arg2, quad.result);
                break;

            case OP_ARRAY_STORE:
                // Instrução para armazenar em um elemento de um array: result[arg2] = arg1
                // Formato da quádrupla: ARRAY_STORE arg1 arg2 result
                // result = nome do array (endereço base)
                // arg2 = índice
                // arg1 = valor a ser armazenado
                
                // Primeiro calculamos o endereço do elemento: base + (índice * 4)
                // Usamos um registrador temporário para calcular o endereço
                rvet = getRegisterIndex(quad.result); //precisa ser referente ao local do vetor e nao o conteudo
                rindice = getRegisterIndex(quad.arg2);
                rbase = getNextFreeReg(tempLocalRegs, 27) + 4;
                tempLocalRegs[rbase-4].isUsed = 1;
                fprintf(output, "%d - addi $r%d $r%d 1 # índice + 1\n", lineIndex++, rindice, rindice);
                fprintf(output, "%d - mul $r%d $r%d -4      # índice * 4 (tamanho do inteiro)\n", lineIndex++, rindice, rindice);
                fprintf(output, "%d - addi $r%d $r%d r%d    # endereço base + deslocamento\n", lineIndex++, rbase, rvet, rindice);
                fprintf(output, "%d - sw $r%d 0($r%d)      # armazena %s em %s[%s]\n", lineIndex++, r1, rbase, quad.arg1, quad.result, quad.arg2);
                break;

            case OP_ALLOC:
                // strncpy(lastAllocLabel, currentLabel.labelName, sizeof(lastAllocLabel) - 1);
    
                if (isdigit(quad.arg1[0]) && strcmp(quad.arg2, "array") == 0) {
                    // Implementação otimizada para alocação de array usando manipulação de pilha
                    int size = atoi(quad.arg1);
                    
                    fprintf(output, "%d - addi $r1 $r1 -4  # aloca espaço para referência do array '%s'\n", lineIndex++, quad.result);
                    // Salva o endereço base do array no registrador de resultado
                    fprintf(output, "%d - move $r%d $r1   # endereço base do array '%s'\n", lineIndex++, r3, quad.result);
                    if (strcmp(currentFunction, "global") == 0) {
                        globalVars[varGlobalCount] = r3; // Armazena o registrador da variável global
                        varGlobalCount++;
                    }
                    else {
                        localVars[varLocalCount] = r3; // Armazena o registrador da variável local
                        varLocalCount++;
                    }
                    int rindex = getRegisterIndex(quad.arg1);
                    fprintf(output, "%d - li $r%d -%d # espaço na pilha para os valores de vet\n", lineIndex++, rindex, size);
                    stackOffset -= (size); //size já é o tamanho em bytes
                    
                    char loopLabel[32], endLoopLabel[32];
                    sprintf(loopLabel, "init_array_%d", quad.line);
                    sprintf(endLoopLabel, "end_init_array_%d", quad.line);
                    
                    fprintf(output, "%d - %s:\n", lineIndex++, loopLabel);
                    fprintf(output, "%d - beq $r%d $r63 %s # se contador == 0, termina\n", lineIndex++, rindex, endLoopLabel);
                    fprintf(output, "%d - addi $r1 $r1 -4  # próximo elemento\n", lineIndex++);
                    fprintf(output, "%d - sw $r63 0($r1)  # inicializa com 0\n", lineIndex++);
                    fprintf(output, "%d - addi $r%d $r%d 4 # decrementa contador\n", lineIndex++, rindex, rindex);
                    fprintf(output, "%d - j %s\n", lineIndex++, loopLabel);
                    fprintf(output, "%d - %s:\n", lineIndex++, endLoopLabel);
                } else {
                    // É uma variável simples, apenas reserva espaço na pilha
                    fprintf(output, "%d - addi $r1 $r1 -4 # aloca espaço para variável '%s'\n", lineIndex++, quad.result);
                    fprintf(output, "%d - sw $r63 0($r1)  # inicializa com 0\n", lineIndex++);
                    stackOffset -= 4;
                    
                    // Guarda o endereço da variável no registrador de resultado
                    fprintf(output, "%d - move $r%d $r1   # endereço da variável '%s'\n", lineIndex++, r3, quad.result);
                    
                    //se escopo == global
                    if (strcmp(currentFunction, "global") == 0) {
                        globalVars[varGlobalCount] = r3; // Armazena o registrador da variável global
                        varGlobalCount++;
                    } else {
                        // printf("currentLabel.labelName %s\n", currentLabel.labelName);
                        localVars[varLocalCount] = r3; // Armazena o registrador da variável local
                        varLocalCount++;
                    }
                }
                break;

            default:
                fprintf(output, "%d - ; instrução %s ainda não implementada\n", lineIndex++, quad.op);
                break;
        }
    }

    // Finaliza com HALT
    fprintf(output, "%d - halt\n", lineIndex);
    fclose(output);
    analyzeRegisterUsage("Output/assembly.asm");
}