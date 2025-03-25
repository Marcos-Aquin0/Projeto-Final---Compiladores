#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "globals.h"

// Variável global para rastrear erros semânticos
extern int hasSemanticError;

// Função principal de análise semântica
void semanticAnalysis(ASTNode* node);

// Função para liberar recursos do analisador semântico
void freeSemanticResources(void);

#endif