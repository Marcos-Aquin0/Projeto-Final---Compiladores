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
    char varName[64];
    int regIndex;
    int isUsed;
    int preserved;
} RegisterMapping;

static RegisterMapping paramRegs[12]; // a0-a11
static RegisterMapping tempRegs[28];  // t0-t27
static RegisterMapping returnRegs[12]; // v0-v11

static char currentFunction[50] = ""; // Função atual sendo processada

OperationType getOpTypeFromString(const char* op);
void initRegisterMappings(void);
int getNextFreeReg(RegisterMapping* regs, int count);
int getRegisterIndexFromName(const char* name);
void updateCurrentFunction(const char* funcName);
void checkNextQuadruple(FILE* inputFile, long* filePos, QuadrupleInfo* nextQuad);
void collectFunctionInfo(void);
void generateAssembly(FILE* inputFile);

// Funções para manipulação de pilha
void pushRegister(FILE* output, int reg, int* stackOffset, int* lineIndex);
void popRegister(FILE* output, int reg, int* stackOffset, int* lineIndex);
int getParameterOffset(int paramIndex);
void loadParameter(FILE* output, int paramIndex, int destReg, int* lineIndex);
void setupFrame(FILE* output, int* lineIndex, int* stackOffset);
void restoreFrame(FILE* output, int* lineIndex, int* stackOffset);
void saveCallerSavedRegs(FILE* output, int* lineIndex, int* stackOffset);
void restoreCallerSavedRegs(FILE* output, int* lineIndex, int* stackOffset);


#endif