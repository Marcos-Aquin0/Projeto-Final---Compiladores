#ifndef ASSEMBLY_MIPS_H
#define ASSEMBLY_MIPS_H

#include "globals.h"
#include "cinter.h"  
#include "symtab.h"

#define MAX_REGS 64 //numero maximo de registradores
#define REG_OUT  0   // r0 - registrador de saída (OUT)
#define REG_RA   31  // r31 - return address (JAL)
#define REG_ZERO 63  // r63 - sempre contém 0

// Estrutura para armazenar uma quádrupla durante a leitura do arquivo
typedef struct {
    int line;
    int sourceLine;
    char op[20];
    char arg1[50];
    char arg2[50];
    char result[50];
} QuadrupleInfo;

// Mapeamento interno de variáveis para registradores
typedef struct {
    char varName[50];
    int regIndex;
    int isUsed;
} RegisterMapping;

static RegisterMapping paramRegs[8]; // a0-a7
static RegisterMapping localRegs[16]; // s0-s15
static RegisterMapping tempRegs[20];  // t0-t19
static RegisterMapping returnRegs[8]; // v0-v7

static char currentFunction[50] = ""; // Função atual sendo processada

OperationType getOpTypeFromString(const char* op);
void initRegisterMappings(void);
int getNextFreeReg(RegisterMapping* regs, int count);
int getRegisterIndexFromName(const char* name);
void updateCurrentFunction(const char* funcName);
void checkNextQuadruple(FILE* inputFile, long* filePos, QuadrupleInfo* nextQuad);
void generateAssembly(FILE* inputFile);

#endif