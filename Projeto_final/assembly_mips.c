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

//Inicialização dos registradores
void initRegisterMappings() {
    int i;
    for (i = 0; i < 6; i++) { //os regs a seguir possuem 6 regs disponíveis
        paramRegs[i].isUsed = 0; //marca como não utilizados
        argumentRegs[i].isUsed = 0;
    }
    for (i = 0; i < 1; i++) { 
        returnRegs[i].isUsed = 0;
    }
    for (i = 0; i < 27; i++) {
        tempLocalRegs[i].isUsed = 0;
    }
    for (i = 0; i < 13; i++) {
        tempGlobalRegs[i].isUsed = 0;
    }
}

// Reinicializa os mapeamentos de registradores
void reinitRegisterMappings() {
    int i;
    for (i = 0; i < 6; i++) { 
        paramRegs[i].isUsed = 0;
        argumentRegs[i].isUsed = 0;
    }
    for (i = 0; i < 27; i++) {
        tempLocalRegs[i].isUsed = 0;
    }
    returnRegs[0].isUsed = 0;
}

// pega o próximo registrador livre
int getNextFreeReg(RegisterMapping* regs, int count) {
    // Primeiro procura por registradores não utilizados
    for (int i = 0; i < count; i++) {
        if (regs[i].isUsed == 0) {
            regs[i].isUsed = 1;
            return i;
        }
    }
    
    DEBUG_ASSEMBLY("DEBUG - getNextFreeReg: AVISO - Todos os registradores preservados, reutilizando reg 0\n");
    return 0; //se por algum motivo estourou, retorna o primeiro registrador
}

// Função para obter o índice do registrador a partir do nome da variável
int getRegisterIndex(char* name) {
    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Procurando registrador para '%s'\n", name ? name : "NULL");
    
    if (name == NULL) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Nome NULL, retornando reg 0\n");
        return 0;
    }

    if(strcmp(name, "processos")==0){
        return 61;
    }
    if(strcmp(name, "processoAtual")==0){
        return 59;
    }
    if(strcmp(name, "processosCarregados")==0){
        return 60;
    }
    if(strcmp(name, "salto")==0){
        return 44;
    }
    if(strcmp(name, "memdados")==0){
        return 62;
    }
    if(strcmp(name, "opcao")==0){
        return 41;
    }
    
    // Verifica se é um valor constante
    if (isdigit(name[0]) || (name[0] == '-' && isdigit(name[1]))) {
        for (int i = 0; i < 27; i++) {
            if (tempLocalRegs[i].isUsed && strcmp(tempLocalRegs[i].varName, name) == 0) {
                DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Constante '%s' já mapeado para a%d (r%d)\n", 
                       name, i, 4 + i);
                return 4 + i; 
            }
        }
        // Usa um registrador temporário para constantes
        int tempIdx = getNextFreeReg(tempLocalRegs, 27);
        sprintf(tempLocalRegs[tempIdx].varName, "%s", name);
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Constante '%s' mapeada para reg temporário t%d (r%d)\n", 
               name, tempIdx, 4 + tempIdx);
        return 4 + tempIdx;
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
                           name, i, 52 + i);
                    return 52 + i; // p0-p5
                }
            }
            
            // Novo parâmetro, mapeia para o próximo registrador livre
            int paramIdx = getNextFreeReg(paramRegs, 6);
            sprintf(paramRegs[paramIdx].varName, "%s", name);
            DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Novo parâmetro '%s' mapeado para a%d (r%d)\n", 
                   name, paramIdx, 52 + paramIdx);
            return 52 + paramIdx; // p0-p5
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
    

 
    if (name[0] == 't' && isdigit(name[1])) {
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

    // Valores de retorno
    if (strncmp(name, "ret", 3) == 0 || strstr(name, "return") != NULL) {
        return 45;
    }
    
    // Fallback: usa registrador do kernel
    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Fallback para '%s', usando registrador kernel r59\n", name);
    return 59; //kernel r58-r60
}

// Atualiza a função atual e reinicia os mapeamentos de registradores
void updateCurrentFunction(const char* funcName) {
    if (funcName != NULL) {
        strncpy(currentFunction, funcName, sizeof(currentFunction) - 1);
        currentFunction[sizeof(currentFunction) - 1] = '\0';
        
        // Reinicia mapeamentos ao mudar de função
        reinitRegisterMappings();
    }
}

// Função auxiliar para verificar a próxima quádrupla
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
    fprintf(output, "%d - subi $r1 $r1 1      # aloca espaço na pilha\n", (*lineIndex)++);
    fprintf(output, "%d - sw $r%d 0($r1)       # empilha registrador\n", (*lineIndex)++, reg);
    DEBUG_ASSEMBLY("DEBUG - pushRegister: Empilhando r%d, stack offset agora é %d\n", reg, *stackOffset);
}

// Restaura um registrador da pilha
void popRegister(FILE* output, int reg, int* stackOffset, int* lineIndex) {
    fprintf(output, "%d - lw $r%d 0($r1)       # desempilha registrador\n", (*lineIndex)++, reg);
    (*stackOffset) += 4;
    fprintf(output, "%d - addi $r1 $r1 1       #desaloca espaço na pilha\n", (*lineIndex)++);
    DEBUG_ASSEMBLY("DEBUG - popRegister: Desempilhando para r%d, stack offset agora é %d\n", reg, *stackOffset);
}

//função auxiliar para imprimir o uso de registradores
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
        else if (i == 3)
            strcpy(regUsage[i].regName, "in");
        else if (i >= 4 && i <= 30)
            sprintf(regUsage[i].regName, "tl%d", i - 4);
        else if (i == 31)
            strcpy(regUsage[i].regName, "ra");
        else if (i >= 32 && i <= 42)
            sprintf(regUsage[i].regName, "tg%d", i - 32);
        else if (i >= 43 && i <= 44)
            sprintf(regUsage[i].regName, "k%d", i - 43);
        else if (i == 45)
            sprintf(regUsage[i].regName, "v%d", i - 45);
        else if (i >= 46 && i <= 51)
            sprintf(regUsage[i].regName, "a%d", i - 46);
        else if (i >= 52 && i <= 57)
            sprintf(regUsage[i].regName, "p%d", i - 52);
        else if (i >= 58 && i <= 61)
            sprintf(regUsage[i].regName, "k%d", i - 58+2);
        else if (i == 62)
            strcpy(regUsage[i].regName, "off");
        else if (i == 63)
            strcpy(regUsage[i].regName, "zero");
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
                            else if (i == 3) strcpy(regUsage[i].purpose, "input");
                            else if (i >= 4 && i <= 30) strcpy(regUsage[i].purpose, "temporary local");
                            else if (i == 31) strcpy(regUsage[i].purpose, "return address");
                            else if (i >= 32 && i <= 42) strcpy(regUsage[i].purpose, "temporary global");
                            else if (i >= 43 && i <= 44) strcpy(regUsage[i].purpose, "so aux");
                            else if (i == 45) strcpy(regUsage[i].purpose, "return value");
                            else if (i >= 46 && i <= 51) strcpy(regUsage[i].purpose, "argument");
                            else if (i >= 52 && i <= 57) strcpy(regUsage[i].purpose, "param");
                            else if (i == 58) strcpy(regUsage[i].purpose, "so_ra");
                            else if (i == 59) strcpy(regUsage[i].purpose, "so_ap");
                            else if (i >= 60) strcpy(regUsage[i].purpose, "so_tp");
                            else if (i >= 61) strcpy(regUsage[i].purpose, "so_sp");
                            else if (i == 62) strcpy(regUsage[i].purpose, "stack pointer offset");
                            else if (i == 63) strcpy(regUsage[i].purpose, "zero constant");
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
    return (paramIndex + 1); // +1 por causa do fp atual
}

// Carrega um parâmetro da pilha para um registrador
void loadParameter(FILE* output, int paramIndex, int destReg, int* lineIndex) {
    int offset = (paramIndex + 2); // +2 para o endereço de retorno e o frame pointer salvo
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

// Aloca espaço para parâmetros de funções
void allocateArgumentSpace(FILE* output, int argumentCount, int* lineIndex, int* stackOffset) {
    if (argumentCount > 0) {
        int totalSize = argumentCount;  // Cada parâmetro ocupa 4 bytes
        
        // fprintf(output, "%d - addi $r1 $r1 -%d  # aloca espaço para %d argumentos\n", 
        //         (*lineIndex)++, totalSize, argumentCount);
        
        *stackOffset -= totalSize;
        DEBUG_ASSEMBLY("DEBUG - allocateArgumentSpace: Alocados %d bytes para %d argumentos, stack offset agora é %d\n", 
               totalSize, argumentCount, *stackOffset);
    }
}

// Função para contar quantos argumentos existem até encontrar uma chamada de função, para empilhar o último primeiro
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

void reiniciarRg(int r1){
    if(r1 > 3 && r1 < 31){
        // Se for um registrador temporário local
        tempLocalRegs[r1-4].isUsed = 0;
    } else if(r1 > 31 && r1 < 45){
        // Se for um registrador temporário global
        tempGlobalRegs[r1-32].isUsed = 0;
    } else if(r1 > 45 && r1 < 52){
        // Se for um registrador de argumento
        argumentRegs[r1-46].isUsed = 0;
    } else if(r1 > 52 && r1 < 58){
        // Se for um registrador de parâmetro
        paramRegs[r1-52].isUsed = 0;
    } else if(r1 == 45){
        // Se for o registrador de retorno
        returnRegs[0].isUsed = 0;
    }
}

// Função principal para gerar o código assembly a partir do arquivo de entrada
void generateAssembly(FILE* inputFile) {
    FILE* output = fopen("Output/assembly.asm", "w");
    // Inicializa os mapeamentos de registradores
    initRegisterMappings();
    
    // Variável para controlar o deslocamento da pilha
    int stackOffset = 0;
    
    // Ignora o cabeçalho da tabela (4 linhas)
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
    //variáveis para controle de quádruplas
    int lineIndex = 0;
    long filePos;
    int argumentCount = 0;  
    int varLocalCount = 0;
    int varGlobalCount = 0;
    int paramCount = 0;
    int parameters[6];
    int localVars[27];
    int globalVars[13];
    int rvet; 
    int rindice;
    int rbase;
    char* pularTemp = "";
    int addehone = 0;
    int r1comp;
    int r2comp;
    int pularJump = 0;
    int proximoReturn = 0;
    int labelCount = 0;

    // fprintf(output, "%d - nop 1\n", lineIndex++); //nop limpa os sinais e pula para a instrução 1
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
            //reseta contadores
            argumentCount = 0;  
            varLocalCount = 0;  
            paramCount = 0;  
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
                fprintf(output, "%d - addil $r43 $r44 %s\n", lineIndex++, nextQuad.result);
                    
                switch (opType) {
                    case OP_EQ:
                        if (nextOpType == OP_JUMPFALSE) {
                            // Debug information to verify register mapping
                            DEBUG_ASSEMBLY("DEBUG - EQ operation: comparing %s (%d) == %s (%d)\n", 
                                quad.arg1, r1, quad.arg2, r2);
                            
                            // Se EQ é falso (valores diferentes), pula para o label
                            fprintf(output, "%d - bnq $r%d $r%d %s # jump se %s != %s\n", 
                                    lineIndex++, r1, r2, nextQuad.result, quad.arg1, quad.arg2);
                        } else { // JUMPTRUE
                            // Se EQ é verdadeiro (valores iguais), pula para o label
                            fprintf(output, "%d - beq $r%d $r%d %s # jump se %s == %s\n", 
                                    lineIndex++, r1, r2, nextQuad.result, quad.arg1, quad.arg2);
                        }
                        r1comp = r1;
                        r2comp = r2;
                        break;
                        
                    case OP_NEQ:
                        if (nextOpType == OP_JUMPFALSE) {
                            // Se NEQ é falso (valores iguais), pula para o label
                            fprintf(output, "%d - beq $r%d $r%d %s # jump se ==\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        } else { // JUMPTRUE
                            // Se NEQ é verdadeiro (valores diferentes), pula para o label
                            fprintf(output, "%d - bnq $r%d $r%d %s # jump se !=\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        }
                        r1comp = r1;
                        r2comp = r2;
                        break;
                        
                    case OP_LT:
                        if (nextOpType == OP_JUMPFALSE) {
                            // Se LT é falso (>=), pula para o label
                            fprintf(output, "%d - bge $r%d $r%d %s # jump >=\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        } else { // JUMPTRUE
                            // Se LT é verdadeiro (<), pula para o label
                            fprintf(output, "%d - blt $r%d $r%d %s #jump se <\n", 
                                    lineIndex++, r1, r2, nextQuad.result);
                        }
                        r1comp = r1;
                        r2comp = r2;
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
                        r1comp = r1;
                        r2comp = r2;
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
                        r1comp = r1;
                        r2comp = r2;
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
                        r1comp = r1;
                        r2comp = r2;
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
        if (strcmp(quad.op, "ASSIGN") == 0){
            if(isdigit(quad.arg1[0])) {
                fprintf(output, "%d - li $r%d %s\n", lineIndex++, r3, quad.arg1);
                reiniciarRg(r1);
                checkNextQuadruple(inputFile, &filePos, &nextQuad);
                if(strcmp(nextQuad.op,"RETURN")== 0){
                    proximoReturn = 1;
                }
                
                continue;
            }
        }

        switch (opType) {
            case OP_ASSIGN:
                // Verifica se é uma movimentação redundante (mesmo registrador fonte e destino)
                if (r3 == 44 || r3 == 59 || r3 == 60 || r3 == 58 || r3 == 62 || r3 == 41 || r3 == 42 || r3 == 43) {
                    // Força o uso de 'move' para atribuir valor diretamente a esses registradores
                    fprintf(output, "%d - move $r%d $r%d # movendo %s para registrador especial %s\n", lineIndex++, r3, r1, quad.arg1, quad.result);
                } 
                else if (r1 == 44 || r1 == 59 || r1 == 60 || r1 == 58 || r1 == 62 || r1 == 41 || r1 == 42 || r1 == 43) {
                    // Força o uso de 'move' para atribuir valor diretamente a esses registradores
                    fprintf(output, "%d - move $r%d $r%d # movendo %s para registrador especial %s\n", lineIndex++, r3, r1, quad.arg1, quad.result);
                } 
                else if (r1 != r3) {
                    if (((r1 > 3 && r1 < 31) || (r1 > 31 && r1 < 41)||(r1 >= 58 && r1 <= 62)) && (quad.result[0] == 't' && isdigit(quad.result[1]))){
                        fprintf(output, "%d - lw $r%d 0($r%d) # movendo %s para %s\n", lineIndex++, r3, r1, quad.arg1, quad.result); 
                        checkNextQuadruple(inputFile, &filePos, &nextQuad);
                        if(strcmp(nextQuad.op,"RETURN")== 0){
                            proximoReturn = 1;
                        }
                    }
                    else if (((r3 > 3 && r3 < 31) || (r3 > 31 && r3 < 41) ||(r3 >= 59 && r3 <= 62)) && (quad.arg1[0] == 't' && isdigit(quad.arg1[1]))) {
                        fprintf(output, "%d - sw $r%d 0($r%d) # movendo %s para %s\n", lineIndex++, r1, r3, quad.arg1, quad.result);
                    }
                    else {
                        fprintf(output, "%d - move $r%d $r%d # movendo %s para %s\n", lineIndex++, r3, r1, quad.arg1, quad.result);
                        checkNextQuadruple(inputFile, &filePos, &nextQuad);
                        if(strcmp(nextQuad.op,"RETURN")== 0){
                            proximoReturn = 1;
                        }
                    }
                    if(quad.arg1[0] == 't' && isdigit(quad.arg1[1])){
                        reiniciarRg(r1);
                    }
                    
                }
                break;

            case OP_ADD:
                // printf("addehone = %d\n", addehone);
                // printf("pularTempoi = %s\n", pularTemp);
                // printf("quad.arg2 = %s\n", quad.arg2);
                fprintf(output, "%d - add $r%d $r%d $r%d # salva em %s (r%d) \n", lineIndex++, r3, r1, r2, quad.result, r3);
                reiniciarRg(r1);
                reiniciarRg(r2);
                checkNextQuadruple(inputFile, &filePos, &nextQuad);
                if(strcmp(nextQuad.op,"RETURN")== 0){
                    proximoReturn = 1;
                }            
                break;

            case OP_SUB:
                // Se o segundo operando é a constante "1", usar diretamente $r61 (que já contém o valor 1)
                fprintf(output, "%d - sub $r%d $r%d $r%d # salva em %s (r%d) \n", lineIndex++, r3, r1, r2, quad.result, r3);
                reiniciarRg(r1);
                reiniciarRg(r2);
                
                break;

            case OP_MULT:
                fprintf(output, "%d - mul $r%d $r%d $r%d # salva em %s (r%d) \n", lineIndex++, r3, r1, r2, quad.result, r3);
                reiniciarRg(r1);
                reiniciarRg(r2);
                break;

            case OP_DIV:
                fprintf(output, "%d - div $r%d $r%d $r%d # salva em %s (r%d) \n", lineIndex++, r3, r1, r2, quad.result, r3);
                reiniciarRg(r1);
                reiniciarRg(r2);
                break;

            case OP_LABEL:
                labelCount  = labelCount + 1;
                checkNextQuadruple(inputFile, &filePos, &nextQuad);
                   
                    fprintf(output, "%d - %s: #Nova Label %s\n", lineIndex++, quad.result, quad.result);
                    // reiniciarRg(r1comp);
                    if(r2comp != 63){
                        reiniciarRg(r2comp);
                    }
                
                    break;

            case OP_JUMP:
                if(pularJump == 0){
                    fprintf(output, "%d - addil $r43 $r44 %s\n", lineIndex++, quad.result);
                    fprintf(output, "%d - j %s\n", lineIndex++, quad.result);
                }
                else {
                    pularJump = 0;
                }
                break;

            case OP_JUMPFALSE:
                // Jump se o valor é falso (igual a zero)
                fprintf(output, "%d - beq $r%d $r63 %s # jump se é falso\n", 
                        lineIndex++, r1, quad.result);
                r1comp = r1;
                r2comp = 63;
                break;

            case OP_JUMPTRUE:
                // Jump se o valor é verdadeiro (diferente de zero)
                fprintf(output, "%d - bnq $r%d $r63 %s # jump se é verdadeiro\n", 
                        lineIndex++, r1, quad.result);
                r1comp = r1;
                r2comp = 63;
                break;

            case OP_FUNCTION:
                labelCount  = labelCount + 1;
                reinitRegisterMappings(); // Reinicia os mapeamentos de registradores
                if(ehPrimeiraFuncao){
                    fprintf(output, "%d - addil $r43 $r44 main\n", lineIndex++);
                    fprintf(output, "%d - j main\n", lineIndex++); //começa na main
                    ehPrimeiraFuncao = 0; // marca que já processou a primeira função
                }
                fprintf(output, "%d - %s: # nova função %s\n", lineIndex++, quad.arg1, quad.arg1);
                // fprintf(output, "%d - out $r1 # define o início da função\n", lineIndex++);
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
                if (r1 < 45 && proximoReturn==0) { // Evita move redundante se o valor já estiver em v0
                    fprintf(output, "%d - lw $r45 0($r%d) # move valor de retorno para v0\n", lineIndex++, r1);
                }
                else{
                    fprintf(output, "%d - move $r45 $r%d # move valor de retorno para v0\n", lineIndex++, r1);   
                    proximoReturn=0;
                }
                
                // Restaura o frame usando nossa nova função
                restoreFrame(output, &lineIndex, &stackOffset);
                fprintf(output, "%d - jr $r31         # retorna\n", lineIndex++);
                checkNextQuadruple(inputFile, &filePos, &nextQuad);
                if(strcmp(nextQuad.op,"JUMP")== 0){
                     pularJump = 1;
                }
                break;

            case OP_END:
                if (strcmp(currentFunction, "main") != 0) {
                    BucketList funcSymbol = st_lookup_in_scope(currentFunction, "global");
                    int isVoidFunction = (funcSymbol && strcmp(funcSymbol->dataType, "void") == 0);
                    if (isVoidFunction) {
                        restoreFrame(output, &lineIndex, &stackOffset);
                        fprintf(output, "%d - move $r0 $r0\n", lineIndex++);
                        fprintf(output, "%d - jr $r31         # retorna (void function end)\n", lineIndex++);
                    }
                }
            break;

            case OP_ARGUMENT: 
                {
                    checkNextQuadruple(inputFile, &filePos, &nextQuad);
                    int argumentNum = atoi(quad.arg2);
                    int destReg = 46 + argumentNum; // a0, a1, etc.
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
                        
                        if(strcmp(nextQuad.arg1,"output") !=0 && strcmp(nextQuad.arg1,"msgLcd")!=0 && strcmp(nextQuad.arg1,"saltoUser")!=0){
                            // Aloca espaço para todos os argumentos de uma vez
                            fprintf(output, "%d - subi $r1 $r1 %d  # aloca espaço para %d argumentos\n", 
                                    lineIndex++, (totalArgs+1), totalArgs+1);
                            stackOffset -= ((totalArgs+1));
                            
                            DEBUG_ASSEMBLY("DEBUG - OP_ARGUMENT: Alocados %d bytes para %d argumentos\n", 
                                (totalArgs+1), totalArgs);
                        }
                    }
                    
                    //se o argumento é uma constante, carrega o valor em um registrador primeiro
                    if (isdigit(quad.arg1[0])) {
                        fprintf(output, "%d - li $r%d %s # argument %d (%s)\n", lineIndex++, destReg, quad.arg1, argumentNum, quad.arg1);
                    }
                    // Se o argumento é uma variável local, precisamos carregar seu valor da memória
                    else if (symbol != NULL && strcmp(symbol->idType, "var") == 0 && strcmp(nextQuad.arg1,"saltoUser") !=0) {  
                        // Primeiro carrega o valor da memória 
                        // Carrega o endereço da variável (que já está em r1) para o registrador de argumento
                        fprintf(output, "%d - lw $r%d 0($r%d)    # carrega valor da variável local '%s' para argument %d\n", 
                                lineIndex++, destReg, r1, quad.arg1, argumentNum);
                    } 
                    //precisa ver a variavel global varglobalcount $gp 62
                    
                    else{
                        BucketList symbol = st_lookup_in_scope(quad.arg1, "global"); // Procura a variável na tabela de símbolos
  
                        if (destReg != r1) {
                            if((r1 > 31 && r1 < 40) && (symbol != NULL && symbol->isArray != 1)){
                                fprintf(output, "%d - lw $r%d 0($r%d) # argument %d - (%s)\n", 
                                        lineIndex++, destReg, r1, argumentNum, quad.arg1);
                            }
                            else{
                                if(strcmp(nextQuad.arg1,"saltoUser") !=0){
                                    fprintf(output, "%d - move $r%d $r%d # argument %d (%s)\n", 
                                            lineIndex++, destReg, r1, argumentNum, quad.arg1);
                                }
                            }
                        } else {
                            fprintf(output, "%d - # argument (%d) %s já está em $r%d\n", 
                                    lineIndex++, argumentNum, quad.arg1, destReg);
                        }
                    }
                    if(strcmp(nextQuad.arg1,"output") !=0 && strcmp(nextQuad.arg1,"halt") !=0 && strcmp(nextQuad.arg1,"saltoUser") !=0 && strcmp(nextQuad.arg1,"msgLcd") !=0){
                        fprintf(output, "%d - sw $r%d %d($r1)  # salva argument %d na pilha\n", 
                                lineIndex++, destReg, argumentNum, argumentNum);
                    }
                }
                break;

            case OP_PARAM:
                // Carrega o parâmetro da pilha para o registrador correspondente
                
                parameters[paramCount] = r1; // Armazena o nome do parâmetro
                // printf("parametro %d, index %d\n", parameters[paramCount], paramCount);
                // printf("quad.arg1 %s\n", quad.arg1);
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
                    fprintf(output, "%d - move $r0 $r46\n", lineIndex++); // out r0 (registrador reservado para output)
                    fprintf(output, "%d - out $r0\n", lineIndex++); // out r0 (registrador reservado para output)
                    // fprintf(output, "%d - addi $r1 $r1 1 # desaloca espaço na pilha\n", lineIndex++);
                } else if (strcmp(quad.arg1, "msgLcd") == 0){
                    fprintf(output, "%d - msgLcd $r%d\n", 
                            lineIndex++, 46); // talvez mudar para pegar do regs
                } else if (strcmp(quad.arg1, "halt") == 0){
                    fprintf(output, "%d - halt # termina a execução\n", lineIndex++);
                } else if (strcmp(quad.arg1, "nop") == 0){
                    fprintf(output, "%d - nop 0\n", lineIndex++);
                } else if (strcmp(quad.arg1, "saltoUser") == 0){
                    fprintf(output, "%d - move $r42 $r1 # salva a posição de memoria\n", lineIndex++); //r referente ao destino do salto  
                    fprintf(output, "%d - move $r1 $r62 # carrega o destino do salto\n", lineIndex++); //r referente ao salto
                    fprintf(output, "%d - addil $r43 $r44 0\n", lineIndex++); //r referente ao salto
                    fprintf(output, "%d - li $r58 %d \n", lineIndex++, (lineIndex)-labelCount+2); //r referente ao salto
                    fprintf(output, "%d - saltoUser $r%d # usar o dado1 para o salto_rom\n", 
                            lineIndex++, 44); //r referente ao salto
                    fprintf(output, "%d - move $r1 $r42 # salva a posição de memoria\n", lineIndex++); //r referente ao destino do salto  
                    
                } 
                else {
                    // Aloca espaço para os argumentos na pilha antes da chamada
                    int argCount = atoi(quad.arg2);
                    if (argCount > 0) {
                        allocateArgumentSpace(output, argCount, &lineIndex, &stackOffset);
                    }
                    

                    // Salva registradores que podem ser modificados pela chamada de função
                    // saveCallerSavedRegs(output, &lineIndex, &stackOffset);
                    
                    // Chamada normal de função
                    fprintf(output, "%d - addil $r43 $r44 %s\n", lineIndex++, quad.arg1);
                    fprintf(output, "%d - jal %s\n", lineIndex++, quad.arg1);
                    // Libera espaço dos argumentos após chamada
                    
                    if (argCount > 0) {
                        int totalSize = argCount;
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
                            // printf("parametro %d, registrador\n", parameters[i]);
                            // int rx = getRegisterIndex(parameters[i]);
                            loadParameter(output, i, parameters[i], &lineIndex);
                        }
                    }

                    if(varLocalCount > 0){
                     //recarregar variaveis locais 
                    //  fprintf(output, "%d - move $r62 $r2 # sp = fp\n", lineIndex++);
                     // Carrega os parâmetros da pilha para os registradores correspondentes
                        for (int i = 0; i < varLocalCount; i++) {
                            // fprintf(output, "%d - subi $r62 $r62 1 # desce na pilha\n", lineIndex++);
                            // fprintf(output, "%d - move $r%d $r62 # recarrega variavel local\n", lineIndex++, localVars[i]);
                            fprintf(output, "%d - lw $r%d -%d($r2)  # recarrega variavel local\n", lineIndex++, localVars[i], i+1);
                        
                        }
                    }
                    
                    // Copia o valor de retorno (v0) para o resultado, verifica se não é redundante
                    if (strcmp(quad.result, "-") != 0 && r3 != 45) { // r45 é v0, evita move para o mesmo registrador
                        if (strncmp(quad.result, "tv", 2) == 0 && isdigit(quad.result[2])) {
                            // Função void, não gera instrução de move, apenas adiciona comentário
                            // fprintf(output, "%d - # Chamada void para função %s, sem valor de retorno\n", lineIndex++, quad.arg1);
                        } else {
                            fprintf(output, "%d - move $r%d $r45 # copia retorno (v0) para %s\n", 
                                        lineIndex++, r3, quad.result);
                        } 
                    }
                    checkNextQuadruple(inputFile, &filePos, &nextQuad);
                    if(strcmp(nextQuad.op,"RETURN")== 0){
                        proximoReturn = 1;
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
                // fprintf(output, "%d - addi $r%d $r%d 1 # índice + 1\n", lineIndex++, rindice, rindice);
                // fprintf(output, "%d - mul $r%d $r%d $r62      # índice * 4 (tamanho do inteiro)\n", lineIndex++, rindice, rindice);
                fprintf(output, "%d - sub $r%d $r%d $r%d    # endereço base - deslocamento\n", lineIndex++, rbase, rvet, rindice);
                fprintf(output, "%d - lw $r%d 0($r%d)      # carrega %s[%s] em %s\n", lineIndex++, r3, rbase, quad.arg1, quad.arg2, quad.result);
                reiniciarRg(rbase);

                checkNextQuadruple(inputFile, &filePos, &nextQuad);
                if(strcmp(nextQuad.op,"RETURN")== 0){
                    proximoReturn = 1;
                }
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
                // fprintf(output, "%d - addi $r%d $r%d 1 # índice + 1\n", lineIndex++, rindice, rindice);
                // fprintf(output, "%d - mul $r%d $r%d $r62      # índice * 4 (tamanho do inteiro)\n", lineIndex++, rindice, rindice);
                fprintf(output, "%d - sub $r%d $r%d $r%d    # endereço base - deslocamento\n", lineIndex++, rbase, rvet, rindice);
                // fprintf(output, "%d - out $r%d\n", lineIndex++, rbase); // Exibe o endereço base do array
                fprintf(output, "%d - sw $r%d 0($r%d)      # armazena %s em %s[%s]\n", lineIndex++, r1, rbase, quad.arg1, quad.result, quad.arg2);
                reiniciarRg(rbase);
                reiniciarRg(r1);
                break;

            case OP_ALLOC:
                // strncpy(lastAllocLabel, currentLabel.labelName, sizeof(lastAllocLabel) - 1);
    
                if (isdigit(quad.arg1[0]) && strcmp(quad.arg2, "array") == 0) {
                    // Implementação otimizada para alocação de array usando manipulação de pilha
                    int size = atoi(quad.arg1);
                    
                    // fprintf(output, "%d - subi $r1 $r1 1  # aloca espaço para referência do array '%s'\n", lineIndex++, quad.result);
                    // // Salva o endereço base do array no registrador de resultado
                    fprintf(output, "%d - move $r%d $r1   # endereço base do array '%s'\n", lineIndex++, r3, quad.result);
                    // fprintf(output, "%d - out $r32\n", lineIndex++); // Exibe o endereço base do array
                    if (strcmp(currentFunction, "global") == 0) {
                        globalVars[varGlobalCount] = r3; // Armazena o registrador da variável global
                        varGlobalCount++;
                    }
                    else {
                        localVars[varLocalCount] = r3; // Armazena o registrador da variável local
                        varLocalCount++;
                    }
                    int rindex = getRegisterIndex(quad.arg1);
                    // fprintf(output, "%d - li $r%d %d # espaço na pilha para os valores de vet\n", lineIndex++, rindex, size);
                    stackOffset -= (size); //size já é o tamanho em bytes
                    
                    char loopLabel[32], endLoopLabel[32];
                    fprintf(output, "%d - subi $r1 $r1 %d  # próximo elemento\n", lineIndex++, size/4);
                    // fprintf(output, "%d - out $r1\n", lineIndex++);
                } else {
                    if (strcmp(quad.result, "processosCarregados")!=0 && strcmp(quad.result, "processoAtual")!=0 && strcmp(quad.result, "salto")!=0  && strcmp(quad.result, "memdados")!=0  && strcmp(quad.result, "opcao")!=0){

                        // É uma variável simples, apenas reserva espaço na pilha
                        fprintf(output, "%d - subi $r1 $r1 1 # aloca espaço para variável '%s'\n", lineIndex++, quad.result);
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
                }
                break;

            default:
                fprintf(output, "%d - ; instrução %s ainda não implementada\n", lineIndex++, quad.op);
                break;
        }
    }

    // Finaliza com syscall, 1
    fprintf(output, "%d - syscall $r58\n", lineIndex);
    fclose(output);
    analyzeRegisterUsage("Output/assembly.asm");
}