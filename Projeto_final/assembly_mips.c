#include "globals.h"
#include "assembly_mips.h"
#include "cinter.h"
#include "symtab.h"

// Mapeamento interno de variáveis para registradores
typedef struct {
    char varName[50];
    int regIndex;
    int isUsed;
} RegisterMapping;

static RegisterMapping paramRegs[12]; // a0-a11
static RegisterMapping localRegs[16]; // s0-s15
static RegisterMapping tempRegs[12];  // t0-t11
static RegisterMapping returnRegs[12]; // v0-v11

static char currentFunction[50] = ""; // Função atual sendo processada

// Implementação da função getOpTypeFromString
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
    if (strcmp(op, "PARAM") == 0) return OP_PARAM;
    if (strcmp(op, "CALL") == 0) return OP_CALL;
}

void initRegisterMappings() {
    int i;
    for (i = 0; i < 12; i++) {
        paramRegs[i].isUsed = 0;
        tempRegs[i].isUsed = 0;
        returnRegs[i].isUsed = 0;
    }
    for (i = 0; i < 16; i++) {
        localRegs[i].isUsed = 0;
    }
}

int getNextFreeReg(RegisterMapping* regs, int count) {
    for (int i = 0; i < count; i++) {
        if (!regs[i].isUsed) {
            regs[i].isUsed = 1;
            return i;
        }
    }
    // Se não encontrar registrador livre, reusa o primeiro
    return 0;
}

int getRegisterIndex(const char* name) {
    if (name == NULL) return 0;

    // Já é uma notação de registrador?
    if (strncmp(name, "t", 1) == 0 && isdigit(name[1])) {
        return 3 + atoi(&name[1]);  // t0-t11 → r3-r14
    } else if (strncmp(name, "s", 1) == 0 && isdigit(name[1])) {
        return 15 + atoi(&name[1]); // s0-s15 → r15-r30
    } else if (strncmp(name, "a", 1) == 0 && isdigit(name[1])) {
        return 32 + atoi(&name[1]); // a0-a11 → r32-r43
    } else if (strncmp(name, "v", 1) == 0 && isdigit(name[1])) {
        return 44 + atoi(&name[1]); // v0-v11 → r44-r55
    } else if (strncmp(name, "tv", 2) == 0) {
        return 56 + atoi(&name[2]);  // tv0-tv2 variáveis temporárias void
    } else if (strcmp(name, "sp") == 0) {
        return 1;
    } else if (strcmp(name, "fp") == 0) {
        return 2;
    } else if (strcmp(name, "out") == 0) {
        return 0;
    } else if (strcmp(name, "ra") == 0) {
        return 31;
    } else if (strcmp(name, "zero") == 0) {
        return 63;
    }
    
    // Verifica se é um valor constante
    if (isdigit(name[0]) || (name[0] == '-' && isdigit(name[1]))) {
        // Usa um registrador temporário para constantes
        int tempIdx = getNextFreeReg(tempRegs, 12);
        sprintf(tempRegs[tempIdx].varName, "%s", name);
        return 3 + tempIdx; // t0-t11
    }
    
    // Verifica na tabela de símbolos se é um parâmetro ou variável local
    BucketList symbol = NULL;
    
    // Primeiro procura no escopo atual (função atual)
    if (strlen(currentFunction) > 0) {
        symbol = st_lookup_in_scope(name, currentFunction);
    }
    
    // Se não encontrar, procura no escopo global
    if (symbol == NULL) {
        symbol = st_lookup_in_scope(name, "global");
    }
    
    if (symbol != NULL) {
        // É um parâmetro?
        if (strcmp(symbol->idType, "param") == 0) {
            // Procurar se já mapeamos esse parâmetro
            for (int i = 0; i < 12; i++) {
                if (paramRegs[i].isUsed && strcmp(paramRegs[i].varName, name) == 0) {
                    return 32 + i; // a0-a11
                }
            }
            
            // Novo parâmetro, mapeia para o próximo registrador livre
            int paramIdx = getNextFreeReg(paramRegs, 12);
            sprintf(paramRegs[paramIdx].varName, "%s", name);
            return 32 + paramIdx; // a0-a11
        }
        // É uma variável local?
        else if (strcmp(symbol->idType, "var") == 0) {
            // Procurar se já mapeamos essa variável
            for (int i = 0; i < 16; i++) {
                if (localRegs[i].isUsed && strcmp(localRegs[i].varName, name) == 0) {
                    return 15 + i; // s0-s15
                }
            }
            
            // Nova variável local, mapeia para o próximo registrador livre
            int localIdx = getNextFreeReg(localRegs, 16);
            sprintf(localRegs[localIdx].varName, "%s", name);
            return 15 + localIdx; // s0-s15
        }
    }
    
    // Temporários (geralmente variáveis com nome tX)
    if (name[0] == 't' && isdigit(name[1])) {
        int tempNum = atoi(&name[1]);
        return 3 + (tempNum % 12); // Mapeia para t0-t11 de forma cíclica
    }
    
    // Valores de retorno
    if (strncmp(name, "ret", 3) == 0 || strstr(name, "return") != NULL) {
        int retIdx = getNextFreeReg(returnRegs, 12);
        sprintf(returnRegs[retIdx].varName, "%s", name);
        return 44 + retIdx; // v0-v11
    }
    
    // Fallback: usa registrador do kernel
    return 59;
}

void updateCurrentFunction(const char* funcName) {
    if (funcName != NULL) {
        strncpy(currentFunction, funcName, sizeof(currentFunction) - 1);
        currentFunction[sizeof(currentFunction) - 1] = '\0';
        
        // Reinicia mapeamentos ao mudar de função
        initRegisterMappings();
    }
}

void generateAssembly(FILE* inputFile) {
    FILE* output = fopen("Output/assembly.asm", "w");
    // Inicializa os mapeamentos de registradores
    initRegisterMappings();
    
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

    QuadrupleInfo quad;
    int lineIndex = 0;

    // Começo com jump para main
    fprintf(output, "%d - j main\n", lineIndex++);

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

        // Processa a quádrupla lida
        OperationType opType = getOpTypeFromString(quad.op);
        int r1 = strcmp(quad.arg1, "-") != 0 ? getRegisterIndex(quad.arg1) : 0;
        int r2 = strcmp(quad.arg2, "-") != 0 ? getRegisterIndex(quad.arg2) : 0;
        int r3 = strcmp(quad.result, "-") != 0 ? getRegisterIndex(quad.result) : 0;

        // Se estamos carregando uma constante
        if (strcmp(quad.op, "ASSIGN") == 0 && isdigit(quad.arg1[0])) {
            fprintf(output, "%d - li $r%d %s\n", lineIndex++, r3, quad.arg1);
            continue;
        }

        switch (opType) {
            case OP_ASSIGN:
                fprintf(output, "%d - move $r%d $r%d\n", lineIndex++, r3, r1);
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
                fprintf(output, "%d - beq $r%d $r0 %s\n", lineIndex++, r1, quad.result);
                break;

            case OP_JUMPTRUE:
                fprintf(output, "%d - bgt $r%d $r0 %s\n", lineIndex++, r1, quad.result);
                break;

            case OP_FUNCTION:
                fprintf(output, "%d - %s: # nova função\n", lineIndex++, quad.arg1);
                // Salva registradores na pilha se necessário
                fprintf(output, "%d - addi $r1 $r1 -4 # ajusta stack pointer\n", lineIndex++);
                fprintf(output, "%d - sw $r31 0($r1)  # salva return address\n", lineIndex++);
                break;

            case OP_RETURN:
                // Carrega valor de retorno em v0
                fprintf(output, "%d - move $r44 $r%d # move valor de retorno para v0\n", lineIndex++, r1);
                // Restaura registradores da pilha
                fprintf(output, "%d - lw $r31 0($r1)  # restaura return address\n", lineIndex++);
                fprintf(output, "%d - addi $r1 $r1 4  # ajusta stack pointer\n", lineIndex++);
                fprintf(output, "%d - jr $r31         # retorna\n", lineIndex++);
                break;

            case OP_END:
                fprintf(output, "%d - # fim da função %s\n", lineIndex++, quad.arg1);
                break;

            case OP_EQ:
                fprintf(output, "%d - beq $r%d $r%d %s\n", lineIndex++, r1, r2, quad.result);
                break;

            case OP_NEQ:
                fprintf(output, "%d - bne $r%d $r%d %s\n", lineIndex++, r1, r2, quad.result);
                break;

            case OP_LT:
                fprintf(output, "%d - blt $r%d $r%d %s\n", lineIndex++, r1, r2, quad.result);
                break;

            case OP_GT:
                fprintf(output, "%d - bgt $r%d $r%d %s\n", lineIndex++, r1, r2, quad.result);
                break;

            case OP_PARAM:
                {
                    int paramNum = atoi(quad.arg2);
                    fprintf(output, "%d - move $r%d $r%d # param %d\n", lineIndex++, 32 + paramNum, r1, paramNum);
                }
                break;

            case OP_CALL:
                if (strcmp(quad.arg1, "input") == 0) {
                    // input() → in $rX
                    fprintf(output, "%d - in $r%d\n", lineIndex++, r3);
                } else if (strcmp(quad.arg1, "output") == 0) {
                    // Para output, usamos o registro do parâmetro passado (a0)
                    fprintf(output, "%d - out $r32\n", lineIndex++); // a0 = r32
                } else {
                    // Chamada normal de função
                    fprintf(output, "%d - jal %s\n", lineIndex++, quad.arg1);
                    // Copia o valor de retorno (v0) para o resultado
                    if (strcmp(quad.result, "-") != 0) {
                        fprintf(output, "%d - move $r%d $r44 # copia retorno (v0) para %s\n", 
                                lineIndex++, r3, quad.result);
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
}