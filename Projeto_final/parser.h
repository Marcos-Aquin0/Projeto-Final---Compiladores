#ifndef PARSER_H
#define PARSER_H

#include "asnt.h"

// Declaração da raiz da árvore sintática
extern ASTNode* root;

// Declaração das funções do parser
extern int yyparse(void);
extern int yylex(void);
extern int yyerror(const char *s);

// Declaração da variável de linha atual
extern int yylineno;

#endif // PARSER_H