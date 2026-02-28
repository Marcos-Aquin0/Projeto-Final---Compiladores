#ifndef BINARIO_PROC_H
#define BINARIO_PROC_H

#include "globals.h"
#include "assembly_mips.h"

#define MAX_LINE_LENGTH 1024

#define MAX_LABELS 1000

// Estrutura para armazenar mapeamentos de rótulos e índices
typedef struct {
    char* label;
    int index;
} LabelMapping;

//tipos das instruções
typedef enum {
    TYPE_R,
    TYPE_I,
    TYPE_J,
    TYPE_MK, //KERNEL
    TYPE_MU, //USER
    TYPE_COMM, 
    TYPE_INVALID
} InstructionType;

//detalhes das instruções
typedef struct {
    char mnemonic[10];
    InstructionType type;
    unsigned int opcode;
    unsigned int funct;
} InstructionInfo;

void getLabelNumber(char* line, char* label, int* index);
int read_assembly_file(FILE* input_file);

#endif