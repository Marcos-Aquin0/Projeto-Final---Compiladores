#include "globals.h"
#include "assembly_mips.h"
#include "cinter.h"

int getRegisterIndex(const char* name) {
    if (name == NULL) return 0;

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
    }else {
        return 59; // kernel
    }
}

// Função para converter uma string de operação para o enum OperationType
OperationType getOpTypeFromString(const char* opStr) {
    if (strcmp(opStr, "ASSIGN") == 0) return OP_ASSIGN;
    if (strcmp(opStr, "ADD") == 0) return OP_ADD;
    if (strcmp(opStr, "SUB") == 0) return OP_SUB;
    if (strcmp(opStr, "MULT") == 0) return OP_MULT;
    if (strcmp(opStr, "DIV") == 0) return OP_DIV;
    if (strcmp(opStr, "LT") == 0) return OP_LT;
    if (strcmp(opStr, "LTE") == 0) return OP_LTE;
    if (strcmp(opStr, "GT") == 0) return OP_GT;
    if (strcmp(opStr, "GTE") == 0) return OP_GTE;
    if (strcmp(opStr, "EQ") == 0) return OP_EQ;
    if (strcmp(opStr, "NEQ") == 0) return OP_NEQ;
    if (strcmp(opStr, "LABEL") == 0) return OP_LABEL;
    if (strcmp(opStr, "JUMP") == 0) return OP_JUMP;
    if (strcmp(opStr, "JUMPFALSE") == 0) return OP_JUMPFALSE;
    if (strcmp(opStr, "JUMPTRUE") == 0) return OP_JUMPTRUE;
    if (strcmp(opStr, "CALL") == 0) return OP_CALL;
    if (strcmp(opStr, "PARAM") == 0) return OP_PARAM;
    if (strcmp(opStr, "RETURN") == 0) return OP_RETURN;
    if (strcmp(opStr, "ARRAY_LOAD") == 0) return OP_ARRAY_LOAD;
    if (strcmp(opStr, "ARRAY_STORE") == 0) return OP_ARRAY_STORE;
    if (strcmp(opStr, "ALLOC") == 0) return OP_ALLOC;
    if (strcmp(opStr, "FUNCTION") == 0) return OP_FUNCTION;
    if (strcmp(opStr, "END") == 0) return OP_END;
    return -1;  // Operação desconhecida
}

void generateAssembly(FILE* inputFile) {
    FILE* output = fopen("Output/assembly.asm", "w");
    // Ignorar o cabeçalho (4 linhas)
    char buffer[256];
    for (int i = 0; i < 4; i++) {
        if (fgets(buffer, sizeof(buffer), inputFile) == NULL) {
            fprintf(stderr, "Erro: Formato de arquivo inesperado.\n");
            fclose(inputFile);
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

        // Processa a quádrupla lida
        OperationType opType = getOpTypeFromString(quad.op);
        int r1 = strcmp(quad.arg1, "-") != 0 ? getRegisterIndex(quad.arg1) : 0;
        int r2 = strcmp(quad.arg2, "-") != 0 ? getRegisterIndex(quad.arg2) : 0;
        int r3 = strcmp(quad.result, "-") != 0 ? getRegisterIndex(quad.result) : 0;

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
                fprintf(output, "%d - %s:\n", lineIndex++, quad.arg1);
                break;

            case OP_RETURN:
                fprintf(output, "%d - jr $r31\n", lineIndex++);
                break;

            case OP_END:
                fprintf(output, "%d - nop\n", lineIndex++);
                break;

            case OP_EQ:
                fprintf(output, "%d - beq $r%d $r%d %s\n", lineIndex++, r1, r2, quad.result);
                break;

            case OP_NEQ:
                fprintf(output, "%d - bnq $r%d $r%d %s\n", lineIndex++, r1, r2, quad.result);
                break;

            case OP_LT:
                fprintf(output, "%d - blt $r%d $r%d %s\n", lineIndex++, r1, r2, quad.result);
                break;

            case OP_GT:
                fprintf(output, "%d - bgt $r%d $r%d %s\n", lineIndex++, r1, r2, quad.result);
                break;

            case OP_CALL:
                if (strcmp(quad.arg1, "input") == 0) {
                    // input() → in $rX
                    fprintf(output, "%d - in $r%d\n", lineIndex++, r3);
                } else if (strcmp(quad.arg1, "output") == 0) {
                    fprintf(output, "%d - out $r%d\n", lineIndex++, getRegisterIndex(quad.arg2));
                } else {
                    // chamada normal de função
                    fprintf(output, "%d - jal %s\n", lineIndex++, quad.arg1);
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
