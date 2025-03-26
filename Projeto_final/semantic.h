#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "asnt.h"

// Contador de erros semânticos
extern int semanticErrorCount;

// Funções para análise semântica
void semanticAnalysis(ASTNode* node);
void freeSemanticResources(void);

#endif