#ifndef BINARIO_PROC_H
#define BINARIO_PROC_H

#include "globals.h"
#include "assembly_mips.h"

#define MAX_LINE_LENGTH 1024

#define MAX_LABELS 1000


void getLabelNumber(char* line, char* label, int* index);
int read_assembly_file(FILE* input_file);

#endif