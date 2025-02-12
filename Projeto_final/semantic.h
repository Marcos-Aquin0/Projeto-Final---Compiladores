#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "asnt.h"

// Main semantic analysis function
void semanticAnalysis(ASTNode* syntaxTree);

// Function to free type table memory
void freeTypeTable(void);

// Global error flag
extern int hasSemanticError;

#endif /* _SEMANTIC_H_ */