#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "asnt.h"

// Função principal de análise semantica
void semanticAnalysis(ASTNode* syntaxTree);

// libera a memória da tabela de tipos
void freeTypeTable(void);

// flag global de erro
extern int hasSemanticError;

#endif /* _SEMANTIC_H_ */