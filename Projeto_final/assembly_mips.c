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
    if (strcmp(op, "CALL") == 0) return OP_CALL;
    if (strcmp(op, "ARRAY_LOAD") == 0) return OP_ARRAY_LOAD;
    if (strcmp(op, "ARRAY_STORE") == 0) return OP_ARRAY_STORE;
    if (strcmp(op, "ALLOC") == 0) return OP_ALLOC;
    return -1;
}

// Inicializa os mapeamentos de registradores
void initRegisterMappings() {
    int i;
    for (i = 0; i < 12; i++) { //os regs a seguir possuem 12 regs disponíveis
        paramRegs[i].isUsed = 0; //marca como não utilizados
        paramRegs[i].preserved = 0; // inicialmente, não precisam ser preservados
        
        returnRegs[i].isUsed = 0;
        returnRegs[i].preserved = 0; // inicialmente, não precisam ser preservados
        
    }
    for (i = 0; i < 28; i++) {
        tempRegs[i].isUsed = 0;
    }
}

//pega o próximo registrador livre
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
    // Isso é um erro que deve ser tratado em tempo de execução ou evitado com spilling
    DEBUG_ASSEMBLY("DEBUG - getNextFreeReg: AVISO - Todos os registradores preservados, reutilizando reg 0\n");
    return 0;
}

// Adicionar esta função para marcar registradores para preservação
void markRegistersForPreservation(int preserveStatus) {
    // Marca todos os registradores de parâmetro como preservados/não preservados
    for (int i = 0; i < 12; i++) {
        if (paramRegs[i].isUsed) {
            paramRegs[i].preserved = preserveStatus;
            DEBUG_ASSEMBLY("DEBUG - markRegistersForPreservation: Registrador a%d marcado como %s\n", 
                   i, preserveStatus ? "preservado" : "não preservado");
        }
    }
    
    // Marca todos os registradores de retorno como preservados/não preservados
    for (int i = 0; i < 12; i++) {
        if (returnRegs[i].isUsed) {
            returnRegs[i].preserved = preserveStatus;
            DEBUG_ASSEMBLY("DEBUG - markRegistersForPreservation: Registrador v%d marcado como %s\n", 
                   i, preserveStatus ? "preservado" : "não preservado");
        }
    }
}

// Função para obter o índice do registrador a partir do nome da variável
int getRegisterIndex(char* name) {
    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Procurando registrador para '%s'\n", name ? name : "NULL");
    
    if (name == NULL) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Nome NULL, retornando reg 0\n");
        return 0;
    }

    if (strncmp(name, "t", 1) == 0 && isdigit(name[1])) {
        int reg = 3 + atoi(&name[1]);
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: '%s' mapeado para registrador t%d (r%d)\n", name, atoi(&name[1]), reg);
        return reg;  // t0-t27 → r3-r30
    } else if (strncmp(name, "a", 1) == 0 && isdigit(name[1])) {
        int reg = 32 + atoi(&name[1]);
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: '%s' mapeado para registrador a%d (r%d)\n", name, atoi(&name[1]), reg);
        return reg; // a0-a11 → r32-r43
    } else if (strncmp(name, "v", 1) == 0 && isdigit(name[1])) {
        int reg = 44 + atoi(&name[1]);
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: '%s' mapeado para registrador v%d (r%d)\n", name, atoi(&name[1]), reg);
        return reg; // v0-v11 → r44-r55
    } else if (strncmp(name, "tv", 2) == 0) {
        int reg = 56 + atoi(&name[2]);
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: '%s' mapeado para registrador tv%d (r%d)\n", name, atoi(&name[2]), reg);
        return reg;  // tv0-tv2 variáveis temporárias void
    } else if (strcmp(name, "sp") == 0) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: 'sp' mapeado para r1\n");
        return 1;
    } else if (strcmp(name, "fp") == 0) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: 'fp' mapeado para r2\n");
        return 2;
    } else if (strcmp(name, "out") == 0) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: 'out' mapeado para r0\n");
        return 0;
    } else if (strcmp(name, "ra") == 0) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: 'ra' mapeado para r31\n");
        return 31;
    } else if (strcmp(name, "0") == 0) {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: constante '0' mapeada para r63\n");
        return 63;
    }
    
    // Verifica se é um valor constante
    if (isdigit(name[0]) || (name[0] == '-' && isdigit(name[1]))) {
        // Usa um registrador temporário para constantes
        int tempIdx = getNextFreeReg(tempRegs, 12);
        sprintf(tempRegs[tempIdx].varName, "%s", name);
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Constante '%s' mapeada para reg temporário t%d (r%d)\n", 
               name, tempIdx, 3 + tempIdx);
        return 3 + tempIdx; // t0-t11
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
            for (int i = 0; i < 12; i++) {
                if (paramRegs[i].isUsed && strcmp(paramRegs[i].varName, name) == 0) {
                    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Parâmetro '%s' já mapeado para a%d (r%d)\n", 
                           name, i, 32 + i);
                    return 32 + i; // a0-a11
                }
            }
            
            // Novo parâmetro, mapeia para o próximo registrador livre
            int paramIdx = getNextFreeReg(paramRegs, 12);
            sprintf(paramRegs[paramIdx].varName, "%s", name);
            DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Novo parâmetro '%s' mapeado para a%d (r%d)\n", 
                   name, paramIdx, 32 + paramIdx);
            return 32 + paramIdx; // a0-a11
        }
        // É uma variável local?
        else if (strcmp(symbol->idType, "var") == 0) {
            // Para variáveis locais, usamos registradores temporários no range de t12-t27
            // Procurar se já mapeamos essa variável
            for (int i = 12; i < 28; i++) {
                if (tempRegs[i].isUsed && strcmp(tempRegs[i].varName, name) == 0) {
                    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Variável local '%s' já mapeada para t%d (r%d)\n", 
                           name, i, 3 + i);
                    return 3 + i; // t12-t27
                }
            }
            
            // Nova variável local, mapeia para o próximo registrador livre
            int tempIdx = getNextFreeReg(tempRegs + 12, 16) + 12; // Offset de 12 para começar do t12
            sprintf(tempRegs[tempIdx].varName, "%s", name);
            DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Nova variável local '%s' mapeada para t%d (r%d)\n", 
                   name, tempIdx, 3 + tempIdx);
            return 3 + tempIdx; // t12-t27
        }
    } else {
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Símbolo '%s' não encontrado na tabela de símbolos\n", name);
    }
    
    // Temporários genéricos (geralmente variáveis com nome tX)
    if (name[0] == 't' && isdigit(name[1])) {
        int tempNum = atoi(&name[1]);
        int reg = 3 + (tempNum % 28); // Use todos os 28 registradores temporários
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Temporário '%s' mapeado ciclicamente para t%d (r%d)\n", 
               name, tempNum % 28, reg);
        return reg; // Mapeia para t0-t27 de forma cíclica
    }
    
    // Valores de retorno
    if (strncmp(name, "ret", 3) == 0 || strstr(name, "return") != NULL) {
        int retIdx = getNextFreeReg(returnRegs, 12);
        sprintf(returnRegs[retIdx].varName, "%s", name);
        DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Valor de retorno '%s' mapeado para v%d (r%d)\n", 
               name, retIdx, 44 + retIdx);
        return 44 + retIdx; // v0-v11
    }
    
    // Fallback: usa registrador do kernel
    DEBUG_ASSEMBLY("DEBUG - getRegisterIndex: Fallback para '%s', usando registrador kernel r59\n", name);
    return 59;
}

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

//iniciar analise de registradores
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
        else if (i >= 3 && i <= 30)
            sprintf(regUsage[i].regName, "t%d", i - 3);
        else if (i >= 32 && i <= 43)
            sprintf(regUsage[i].regName, "a%d", i - 32);
        else if (i >= 44 && i <= 55)
            sprintf(regUsage[i].regName, "v%d", i - 44);
        else if (i >= 56 && i <= 58)
            sprintf(regUsage[i].regName, "tv%d", i - 56);
        else
            sprintf(regUsage[i].regName, "k%d", i - 59);
    }
    
    char buffer[256];
    int lineNum = 0;
    char currentFunction[64] = "";
    
    printf("\n=== INICIANDO ANÁLISE DE REGISTRADORES ===\n");
    
    // Primeira passagem: encontrar a função main
    while (fgets(buffer, sizeof(buffer), assemblyFile) != NULL) {
        lineNum++;
        
        // Verifica se é um rótulo de função
        char* labelPos = strstr(buffer, "main:");
        if (labelPos) {
            strcpy(currentFunction, "main");
            printf("Função main encontrada na linha %d\n", lineNum-1);
            break;
        }
    }
    
    if (strcmp(currentFunction, "") == 0) {
        fprintf(stderr, "Aviso: Função main não encontrada no código assembly.\n");
        fclose(assemblyFile);
        return;
    }
    
    // Voltar ao início do arquivo e analisar registradores
    rewind(assemblyFile);
    lineNum = 0;
    
    // Segunda passagem: analisar uso de registradores
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
            
            if (strstr(buffer, regPattern) != NULL) {
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
                    else if (i >= 3 && i <= 30) strcpy(regUsage[i].purpose, "temporary");
                    else if (i >= 32 && i <= 43) strcpy(regUsage[i].purpose, "argument");
                    else if (i >= 44 && i <= 55) strcpy(regUsage[i].purpose, "return value");
                    else strcpy(regUsage[i].purpose, "other");
                }
                
                regUsage[i].lastUsedAt = lineNum;
                regUsage[i].useCount++;
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
    
    // Detecta possíveis problemas ou otimizações
    printf("\n=== POSSÍVEIS OTIMIZAÇÕES ===\n");
    
    // Registradores pouco utilizados
    for (int i = 3; i <= 30; i++) { // Apenas registradores temporários
        if (regUsage[i].isUsed && regUsage[i].useCount <= 2) {
            printf("- Registrador $r%d (%s) usado apenas %d vezes. Possível candidato para otimização.\n",
                   i, regUsage[i].regName, regUsage[i].useCount);
        }
    }
    
    // Registradores com intervalos de vida longos
    for (int i = 3; i <= 55; i++) {
        if (regUsage[i].isUsed) {
            int lifespan = regUsage[i].lastUsedAt - regUsage[i].firstUsedAt;
            if (lifespan > 20) { // Heurística: intervalo de vida longo
                printf("- Registrador $r%d (%s) tem intervalo de vida longo (%d linhas). Considerar reutilização.\n",
                       i, regUsage[i].regName, lifespan);
            }
        }
    }
    
    // Verifica excesso de uso de registradores em funções
    int totalUsedRegs = 0;
    for (int i = 0; i < 64; i++) {
        if (regUsage[i].isUsed) totalUsedRegs++;
    }
    
    if (totalUsedRegs > 20) {
        printf("- Alto uso de registradores (%d). Considere otimizar a alocação.\n", totalUsedRegs);
    }
    
    fclose(assemblyFile);
    printf("\nAnálise de registradores concluída.\n");
}

// Empurra um registrador na pilha
void pushRegister(FILE* output, int reg, int* stackOffset, int* lineIndex) {
    (*stackOffset) -= 4;
    fprintf(output, "%d - addi $r1 $r1 -4      # empilha registrador\n", (*lineIndex)++);
    fprintf(output, "%d - sw $r%d 0($r1)\n", (*lineIndex)++, reg);
    DEBUG_ASSEMBLY("DEBUG - pushRegister: Empilhando r%d, stack offset agora é %d\n", reg, *stackOffset);
}

// Restaura um registrador da pilha
void popRegister(FILE* output, int reg, int* stackOffset, int* lineIndex) {
    fprintf(output, "%d - lw $r%d 0($r1)       # desempilha registrador\n", (*lineIndex)++, reg);
    (*stackOffset) += 4;
    fprintf(output, "%d - addi $r1 $r1 4\n", (*lineIndex)++);
    DEBUG_ASSEMBLY("DEBUG - popRegister: Desempilhando para r%d, stack offset agora é %d\n", reg, *stackOffset);
}

// Calcula o deslocamento de um parâmetro na pilha
int getParameterOffset(int paramIndex) {
    return 4 * (paramIndex + 2); // +2 para o endereço de retorno e o frame pointer salvo
}

// Carrega um parâmetro da pilha para um registrador
void loadParameter(FILE* output, int paramIndex, int destReg, int* lineIndex) {
    int offset = getParameterOffset(paramIndex);
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
    
    // Estabelece o novo frame pointer
    fprintf(output, "%d - move $r2 $r1         # atualiza frame pointer\n", (*lineIndex)++);
    
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

// Função para salvar registradores usados em chamadas de função
void saveCallerSavedRegs(FILE* output, int* lineIndex, int* stackOffset) {
    // Identifica quais registradores temporários estão em uso
    for (int i = 0; i < 12; i++) {
        if (tempRegs[i].isUsed && !tempRegs[i].preserved) {
            pushRegister(output, i + 3, stackOffset, lineIndex);  // t0-t11 são r3-r14
            tempRegs[i].preserved = 1;
            DEBUG_ASSEMBLY("DEBUG - saveCallerSavedRegs: Salvando registrador temporário t%d (r%d)\n", i, i + 3);
        }
    }
}

// Função para restaurar registradores após chamadas de função
void restoreCallerSavedRegs(FILE* output, int* lineIndex, int* stackOffset) {
    // Restaura na ordem inversa (LIFO)
    for (int i = 11; i >= 0; i--) {
        if (tempRegs[i].isUsed && tempRegs[i].preserved) {
            popRegister(output, i + 3, stackOffset, lineIndex);  // t0-t11 são r3-r14
            tempRegs[i].preserved = 0;
            DEBUG_ASSEMBLY("DEBUG - restoreCallerSavedRegs: Restaurando registrador temporário t%d (r%d)\n", i, i + 3);
        }
    }
}

// void collectFunctionInfo(){

//     //cria tabela para funções
//     //olhar para a tabela de simbolos procurando funcoes BucketList->idType = "function" e insere na primeira coluna de funcao.
//     //uma coluna de qual funcoes elas chamam, olha para a tabela de simbolos procura BucketList->scope == nome_funcao, e a partir disso pega BucketList->name, essa é a função que é chamada
//     // para inserir na ordem de chamada, vai olhar BucketList->memlocation, considerando a parte do escopo que ja foi analisada. se estou na main, por exemplo, vou começar buscando a partir da posição dela, quando nome da funcao é main e o escopo é global, para funcoes com escopo main.
//     //nova coluna com quantidade de parametros e quais parametros, olha para paramlist da arvore sintatica
//     //nova coluna com variaveis locais, olha para a arvore sintatica e busca LocalDecl
//     //por fim, uma nova tabela com linhas e colunas nxn com nomes das funcoes, se a funcao da linha i chama a funcao da coluna j, deve ter os argumentos da arglist da arvore como uma lista ali, caso contrario apenas "-"    
//     //no final imprimir essas duas tabelas
// }

void collectFunctionInfo() {
    printf("\n=== ANÁLISE DE FUNÇÕES ===\n\n");
    
    // Estrutura para armazenar informações de função
    typedef struct FunctionInfo {
        char name[64];           // Nome da função
        int paramCount;          // Quantidade de parâmetros
        ParamInfo params;        // Lista de parâmetros
        char** paramNames;       // Nomes dos parâmetros
        int localVarCount;       // Quantidade de variáveis locais
        char** localVars;        // Nomes das variáveis locais
        int callCount;           // Quantidade de chamadas de função
        char** calledFunctions;  // Funções chamadas
        int* callOrder;          // Ordem das chamadas (baseada em memloc)
        struct FunctionInfo* next;
    } FunctionInfo;
    
    FunctionInfo* functionList = NULL;
    int functionCount = 0;
    FILE* tempFile = fopen("temp_symtab.txt", "w");
    
    if (!tempFile) {
        fprintf(stderr, "Erro: Não foi possível criar arquivo temporário para análise de funções.\n");
        return;
    }
    
    // Redireciona a impressão da tabela de símbolos para o arquivo temporário
    printSymTab(tempFile);
    fclose(tempFile);
    
    // Reabre o arquivo para leitura
    tempFile = fopen("temp_symtab.txt", "r");
    if (!tempFile) {
        fprintf(stderr, "Erro: Não foi possível abrir arquivo temporário para análise de funções.\n");
        return;
    }
    
    printf("1. Coletando informações de funções da tabela de símbolos...\n");
    
    // Pula as duas primeiras linhas (cabeçalhos)
    char buffer[512];
    fgets(buffer, sizeof(buffer), tempFile);
    fgets(buffer, sizeof(buffer), tempFile);
    
    // Processa linha por linha da tabela de símbolos
    while (fgets(buffer, sizeof(buffer), tempFile)) {
        char name[64], scope[64], idType[32], dataType[32];
        int location;
        
        // Extrai informações da linha
        if (sscanf(buffer, "%63s %63s %31s %31s %d", name, scope, idType, dataType, &location) >= 5) {
            // Verifica se é uma função no escopo global
            if (strcmp(idType, "func") == 0 && strcmp(scope, "global") == 0) {
                // Criar nova entrada para a função
                FunctionInfo* newFunc = (FunctionInfo*)malloc(sizeof(FunctionInfo));
                if (!newFunc) {
                    fprintf(stderr, "Erro: Falha na alocação de memória para informações de função.\n");
                    fclose(tempFile);
                    return;
                }
                
                strncpy(newFunc->name, name, 63);
                newFunc->name[63] = '\0';
                
                // Busca na tabela de símbolos para obter mais informações
                BucketList funcEntry = st_lookup_in_scope(name, "global");
                if (funcEntry) {
                    newFunc->paramCount = funcEntry->paramCount;
                    newFunc->params = funcEntry->params;
                } else {
                    newFunc->paramCount = 0;
                    newFunc->params = NULL;
                }
                
                // Inicializa array para nomes de parâmetros
                newFunc->paramNames = NULL;
                if (newFunc->paramCount > 0) {
                    newFunc->paramNames = (char**)malloc(sizeof(char*) * newFunc->paramCount);
                    if (newFunc->paramNames) {
                        for (int i = 0; i < newFunc->paramCount; i++) {
                            newFunc->paramNames[i] = NULL;
                        }
                    }
                }
                
                newFunc->localVarCount = 0;
                newFunc->localVars = NULL;
                newFunc->callCount = 0;
                newFunc->calledFunctions = NULL;
                newFunc->callOrder = NULL;
                newFunc->next = functionList;
                
                functionList = newFunc;
                functionCount++;
                
                printf("  - Função encontrada: %s (Parâmetros: %d)\n", newFunc->name, newFunc->paramCount);
            }
        }
    }
    
    // Reposiciona o arquivo para o início e pula os cabeçalhos novamente
    rewind(tempFile);
    fgets(buffer, sizeof(buffer), tempFile);
    fgets(buffer, sizeof(buffer), tempFile);
    
    if (functionCount == 0) {
        printf("Nenhuma função encontrada na tabela de símbolos.\n");
        fclose(tempFile);
        return;
    }
    
    printf("\n2. Coletando parâmetros, variáveis locais e chamadas de função...\n");
    
    // Segunda passagem para encontrar parâmetros, variáveis locais e chamadas de função
    while (fgets(buffer, sizeof(buffer), tempFile)) {
        char name[64], scope[64], idType[32], dataType[32];
        int location;
        
        if (sscanf(buffer, "%63s %63s %31s %31s %d", name, scope, idType, dataType, &location) >= 5) {
            // Procura por parâmetros
            if (strcmp(idType, "param") == 0) {
                // Encontra a função que contém este parâmetro
                FunctionInfo* func = functionList;
                while (func != NULL) {
                    if (strcmp(scope, func->name) == 0) {
                        // Adiciona à lista de nomes de parâmetros
                        for (int i = 0; i < func->paramCount; i++) {
                            if (func->paramNames[i] == NULL) {
                                func->paramNames[i] = strdup(name);
                                printf("  - Parâmetro '%s' (%s) encontrado na função '%s'\n", 
                                      name, dataType, func->name);
                                break;
                            }
                        }
                        break;
                    }
                    func = func->next;
                }
            }
            // Procura por variáveis locais
            else if (strcmp(idType, "var") == 0) {
                // Se o escopo não for global, é uma variável local
                if (strcmp(scope, "global") != 0) {
                    // Encontrar a função que contém esta variável
                    FunctionInfo* func = functionList;
                    while (func != NULL) {
                        if (strcmp(scope, func->name) == 0) {
                            func->localVarCount++;
                            func->localVars = realloc(func->localVars, func->localVarCount * sizeof(char*));
                            if (!func->localVars) {
                                fprintf(stderr, "Erro: Falha na realocação de memória para variáveis locais.\n");
                                fclose(tempFile);
                                return;
                            }
                            func->localVars[func->localVarCount - 1] = strdup(name);
                            printf("  - Variável local '%s' encontrada na função '%s'\n", name, func->name);
                            break;
                        }
                        func = func->next;
                    }
                }
            }
            // Procura por chamadas de função
            else if (strcmp(idType, "func") == 0 && strcmp(scope, "global") != 0) {
                // O escopo indica a função chamadora
                FunctionInfo* callerFunc = functionList;
                while (callerFunc != NULL) {
                    if (strcmp(scope, callerFunc->name) == 0) {
                        // O nome indica a função chamada
                        // Verifica se já não adicionamos esta função
                        int alreadyAdded = 0;
                        for (int i = 0; i < callerFunc->callCount; i++) {
                            if (strcmp(callerFunc->calledFunctions[i], name) == 0) {
                                alreadyAdded = 1;
                                break;
                            }
                        }
                        
                        if (!alreadyAdded) {
                            callerFunc->callCount++;
                            callerFunc->calledFunctions = realloc(callerFunc->calledFunctions, 
                                                                 callerFunc->callCount * sizeof(char*));
                            callerFunc->callOrder = realloc(callerFunc->callOrder, 
                                                          callerFunc->callCount * sizeof(int));
                            if (!callerFunc->calledFunctions || !callerFunc->callOrder) {
                                fprintf(stderr, "Erro: Falha na realocação de memória para chamadas de função.\n");
                                fclose(tempFile);
                                return;
                            }
                            
                            callerFunc->calledFunctions[callerFunc->callCount - 1] = strdup(name);
                            callerFunc->callOrder[callerFunc->callCount - 1] = location;
                            
                            printf("  - Chamada para função '%s' encontrada em '%s' (memloc: %d)\n", 
                                   name, callerFunc->name, location);
                        }
                        break;
                    }
                    callerFunc = callerFunc->next;
                }
            }
        }
    }
    
    fclose(tempFile);
    // Remover o arquivo temporário
    remove("temp_symtab.txt");
    
    // Ordenar as funções chamadas pela ordem de memory location
    printf("\n3. Ordenando chamadas de função por ordem de memória...\n");
    
    FunctionInfo* current = functionList;
    while (current != NULL) {
        if (current->callCount > 1) {
            // Bubble sort simples para ordenar
            for (int i = 0; i < current->callCount - 1; i++) {
                for (int j = 0; j < current->callCount - i - 1; j++) {
                    if (current->callOrder[j] > current->callOrder[j + 1]) {
                        // Troca a ordem
                        int tempLoc = current->callOrder[j];
                        current->callOrder[j] = current->callOrder[j + 1];
                        current->callOrder[j + 1] = tempLoc;
                        
                        // Troca os nomes das funções
                        char* tempFunc = current->calledFunctions[j];
                        current->calledFunctions[j] = current->calledFunctions[j + 1];
                        current->calledFunctions[j + 1] = tempFunc;
                    }
                }
            }
            printf("  - Ordenadas %d chamadas de função em '%s'\n", current->callCount, current->name);
        }
        current = current->next;
    }
    
    // Imprimir a primeira tabela: informações de funções
    printf("\n=== TABELA DE INFORMAÇÕES DE FUNÇÕES ===\n");
    printf("+-----------------+-----------------------------+---------------------+-------------------------+\n");
    printf("| Nome da Função  | Parâmetros                  | Variáveis Locais    | Funções Chamadas       |\n");
    printf("+-----------------+-----------------------------+---------------------+-------------------------+\n");
    
    current = functionList;
    while (current != NULL) {
        // Formatar string de parâmetros
        char paramStr[256] = "";
        if (current->paramCount > 0) {
            int paramAdded = 0;
            
            // Primeiro tenta usar os nomes de parâmetros
            if (current->paramNames) {
                for (int i = 0; i < current->paramCount; i++) {
                    if (current->paramNames[i]) {
                        if (paramAdded > 0) strcat(paramStr, ", ");
                        strcat(paramStr, current->paramNames[i]);
                        paramAdded++;
                    }
                }
            }
            
            // Se não conseguiu nomes, usa a informação de tipos
            if (paramAdded == 0 && current->params) {
                ParamInfo param = current->params;
                while (param != NULL) {
                    if (paramAdded > 0) strcat(paramStr, ", ");
                    strcat(paramStr, param->paramType);
                    strcat(paramStr, param->isArray ? "[]" : "");
                    paramAdded++;
                    param = param->next;
                }
            }
            
            // Se ainda não tem nada, apenas mostra a contagem
            if (paramAdded == 0) {
                sprintf(paramStr, "%d parâmetros", current->paramCount);
            }
        } else {
            strcpy(paramStr, "sem parâmetros");
        }
        
        // Formatar string de variáveis locais
        char localVarStr[256] = "";
        for (int i = 0; i < current->localVarCount; i++) {
            if (i > 0) strcat(localVarStr, ", ");
            strcat(localVarStr, current->localVars[i]);
            if (strlen(localVarStr) > 15 && i < current->localVarCount - 1) {
                strcat(localVarStr, "...");
                break;
            }
        }
        
        // Formatar string de funções chamadas
        char calledFuncStr[256] = "";
        for (int i = 0; i < current->callCount; i++) {
            if (i > 0) strcat(calledFuncStr, ", ");
            strcat(calledFuncStr, current->calledFunctions[i]);
            if (strlen(calledFuncStr) > 15 && i < current->callCount - 1) {
                strcat(calledFuncStr, "...");
                break;
            }
        }
        
        printf("| %-15s | %-27s | %-19s | %-23s |\n", 
               current->name, paramStr, 
               current->localVarCount > 0 ? localVarStr : "-", 
               current->callCount > 0 ? calledFuncStr : "-");
        
        current = current->next;
    }
    printf("+-----------------+-----------------------------+---------------------+-------------------------+\n");
    
    // Imprimir a segunda tabela: matriz de chamadas de função
    printf("\n=== MATRIZ DE CHAMADAS DE FUNÇÃO ===\n");
    printf("Linhas: funções chamadoras; Colunas: funções chamadas\n\n");
    
    // Imprimir cabeçalho da tabela
    printf("%-15s |", "");
    current = functionList;
    while (current != NULL) {
        printf(" %-15s |", current->name);
        current = current->next;
    }
    printf("\n");
    
    // Imprimir separador
    printf("----------------+");
    for (int i = 0; i < functionCount; i++) {
        printf("----------------+");
    }
    printf("\n");
    
    // Imprimir linhas da matriz
    current = functionList;
    while (current != NULL) {
        printf("%-15s |", current->name);
        
        // Para cada coluna (possível função chamada)
        FunctionInfo* column = functionList;
        while (column != NULL) {
            // Verificar se a função da linha chama a função da coluna
            int callsFunction = 0;
            for (int i = 0; i < current->callCount; i++) {
                if (strcmp(current->calledFunctions[i], column->name) == 0) {
                    callsFunction = 1;
                    break;
                }
            }
            
            if (callsFunction) {
                printf(" %-15s |", "X");
            } else {
                printf(" %-15s |", "-");
            }
            
            column = column->next;
        }
        printf("\n");
        current = current->next;
    }
    
    // Liberar memória alocada
    current = functionList;
    while (current != NULL) {
        FunctionInfo* temp = current;
        current = current->next;
        
        // Liberar arrays alocados
        for (int i = 0; i < temp->localVarCount; i++)
            free(temp->localVars[i]);
        if (temp->localVars) free(temp->localVars);
        
        for (int i = 0; i < temp->callCount; i++)
            free(temp->calledFunctions[i]);
        if (temp->calledFunctions) free(temp->calledFunctions);
        
        if (temp->callOrder) free(temp->callOrder);
        
        if (temp->paramNames) {
            for (int i = 0; i < temp->paramCount; i++)
                if (temp->paramNames[i]) free(temp->paramNames[i]);
            free(temp->paramNames);
        }
        
        free(temp);
    }
    
    printf("\nAnálise de funções concluída!\n");
}

// Função principal para gerar o código assembly a partir do arquivo de entrada
void generateAssembly(FILE* inputFile) {
    FILE* output = fopen("Output/assembly.asm", "w");
    // Inicializa os mapeamentos de registradores
    initRegisterMappings();
    
    // Variável para controlar o deslocamento da pilha
    int stackOffset = 0;
    
    // Ignorar o cabeçalho (4 linhas)
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
                break;

            case OP_JUMP:
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
                
                // Ao entrar em uma nova função, registradores não estão preservados inicialmente
                markRegistersForPreservation(0);
                break;

            case OP_RETURN:
                // Carrega valor de retorno em v0 (r44)
                if (r1 != 44) { // Evita move redundante se o valor já estiver em v0
                    fprintf(output, "%d - move $r44 $r%d # move valor de retorno para v0\n", lineIndex++, r1);
                }
                
                // Restaura o frame usando nossa nova função
                restoreFrame(output, &lineIndex, &stackOffset);
                
                fprintf(output, "%d - jr $r31         # retorna\n", lineIndex++);
                break;

            case OP_END:
                fprintf(output, "%d - # fim da função %s\n", lineIndex++, quad.arg1);
                break;

            case OP_PARAM:
                {
                    int paramNum = atoi(quad.arg2);
                    int destReg = 32 + paramNum; // a0, a1, etc.
                    
                    // Avoid redundant moves
                    if (destReg != r1) {
                        fprintf(output, "%d - move $r%d $r%d # param %d: %s\n", 
                                lineIndex++, destReg, r1, paramNum, quad.arg1);
                    } else {
                        // Parameter already in correct register, just add comment
                        fprintf(output, "%d - # param %d: %s já está em $r%d\n", 
                                lineIndex++, paramNum, quad.arg1, destReg);
                    }
                }
                break;

            case OP_CALL:
                if (strcmp(quad.arg1, "input") == 0) {
                    // input() → in $rX
                    fprintf(output, "%d - in $r%d\n", lineIndex++, r3);
                } else if (strcmp(quad.arg1, "output") == 0) {
                    // Para output, usamos o registro do parâmetro passado (a0)
                    fprintf(output, "%d - move $r0 $r32\n", lineIndex++); // out r0 (registrador reservado para output)
                    fprintf(output, "%d - out $r0\n", lineIndex++); // out r0 (registrador reservado para output)
                } else {
                    // Salva registradores que podem ser modificados pela chamada de função
                    saveCallerSavedRegs(output, &lineIndex, &stackOffset);
                    
                    // Chamada normal de função
                    fprintf(output, "%d - jal %s\n", lineIndex++, quad.arg1);
                    
                    // Restaura os registradores salvos
                    restoreCallerSavedRegs(output, &lineIndex, &stackOffset);
                    
                    // Copia o valor de retorno (v0) para o resultado, verifica se não é redundante
                    if (strcmp(quad.result, "-") != 0 && r3 != 44) { // r44 é v0, evita move para o mesmo registrador
                        // Verifica se a próxima instrução não é um RETURN com o mesmo registrador
                        checkNextQuadruple(inputFile, &filePos, &nextQuad);
                        OperationType nextOpType = getOpTypeFromString(nextQuad.op);
                        
                        // Se a próxima instrução for RETURN que devolve para v0 o valor que acabamos de obter
                        if (nextOpType == OP_RETURN && strcmp(nextQuad.arg1, quad.result) == 0) {
                            // Pula a instrução de cópia, o valor já está em v0
                        } else {
                            fprintf(output, "%d - move $r%d $r44 # copia retorno (v0) para %s\n", 
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
                
                // Primeiro calculamos o endereço do elemento: base + (índice * 4)
                // Usamos um registrador temporário para calcular o endereço
                fprintf(output, "%d - mul $r3 $r%d 4      # índice * 4 (tamanho do inteiro)\n", lineIndex++, r2);
                fprintf(output, "%d - add $r3 $r%d $r3    # endereço base + deslocamento\n", lineIndex++, r1);
                fprintf(output, "%d - lw $r%d 0($r3)      # carrega %s[%s] em %s\n", lineIndex++, r3, quad.arg1, quad.arg2, quad.result);
                break;

            case OP_ARRAY_STORE:
                // Instrução para armazenar em um elemento de um array: result[arg2] = arg1
                // Formato da quádrupla: ARRAY_STORE arg1 arg2 result
                // result = nome do array (endereço base)
                // arg2 = índice
                // arg1 = valor a ser armazenado
                
                // Primeiro calculamos o endereço do elemento: base + (índice * 4)
                // Usamos um registrador temporário para calcular o endereço
                fprintf(output, "%d - mul $r3 $r%d 4      # índice * 4 (tamanho do inteiro)\n", lineIndex++, r2);
                fprintf(output, "%d - add $r3 $r%d $r3    # endereço base + deslocamento\n", lineIndex++, r3);
                fprintf(output, "%d - sw $r%d 0($r3)      # armazena %s em %s[%s]\n", lineIndex++, r1, quad.arg1, quad.result, quad.arg2);
                break;

            case OP_ALLOC:
                if (isdigit(quad.arg1[0]) && strcmp(quad.arg2, "array") == 0) {
                    // Implementação otimizada para alocação de array usando manipulação de pilha
                    int size = atoi(quad.arg1);
                    
                    // Aloca espaço na pilha para o array
                    fprintf(output, "%d - li $r3 %d       # tamanho do array em palavras\n", lineIndex++, size);
                    fprintf(output, "%d - mul $r3 $r3 4   # tamanho do array em bytes\n", lineIndex++);
                    fprintf(output, "%d - sub $r1 $r1 $r3 # aloca espaço na pilha\n", lineIndex++);
                    stackOffset -= (size * 4);
                    
                    // Salva o endereço base do array no registrador de resultado
                    fprintf(output, "%d - move $r%d $r1   # endereço base do array '%s'\n", lineIndex++, r3, quad.result);
                    
                    // Inicializa o array com zeros
                    fprintf(output, "%d - move $r4 $r1    # ponteiro para inicialização\n", lineIndex++);
                    fprintf(output, "%d - li $r5 %d       # contador (bytes a inicializar)\n", lineIndex++, size * 4);
                    
                    char loopLabel[32], endLoopLabel[32];
                    sprintf(loopLabel, "init_array_%d", quad.line);
                    sprintf(endLoopLabel, "end_init_array_%d", quad.line);
                    
                    fprintf(output, "%d - %s:\n", lineIndex++, loopLabel);
                    fprintf(output, "%d - beq $r5 $r63 %s # se contador == 0, termina\n", lineIndex++, endLoopLabel);
                    fprintf(output, "%d - sw $r63 0($r4)  # inicializa com 0\n", lineIndex++);
                    fprintf(output, "%d - addi $r4 $r4 4  # próximo elemento\n", lineIndex++);
                    fprintf(output, "%d - addi $r5 $r5 -4 # decrementa contador\n", lineIndex++);
                    fprintf(output, "%d - j %s\n", lineIndex++, loopLabel);
                    fprintf(output, "%d - %s:\n", lineIndex++, endLoopLabel);
                } else {
                    // É uma variável simples, apenas reserva espaço na pilha
                    fprintf(output, "%d - addi $r1 $r1 -4 # aloca espaço para variável '%s'\n", lineIndex++, quad.result);
                    fprintf(output, "%d - sw $r63 0($r1)  # inicializa com 0\n", lineIndex++);
                    stackOffset -= 4;
                    
                    // Guarda o endereço da variável no registrador de resultado
                    fprintf(output, "%d - move $r%d $r1   # endereço da variável '%s'\n", lineIndex++, r3, quad.result);
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
    
    // Após gerar o código assembly, inicia a análise de registradores
    analyzeRegisterUsage("Output/assembly.asm");
    collectFunctionInfo();
    
}