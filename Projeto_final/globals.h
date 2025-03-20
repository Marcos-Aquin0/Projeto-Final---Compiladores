#ifndef _GLOBALS_H_
#define _GLOBALS_H_

/*declarações compartilhadas entre lex e parser*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Variáveis Globais */
extern int yylineno; // Número da linha para mensagens de erro

// Declare the function (but don't define it here)
void debugPrint(const char* format, ...);

#define DEBUG_IR(fmt, ...) debugPrint("IR Debug [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define DEBUG_SEM(fmt, ...) debugPrint("Semantic Debug [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define DEBUG_AST(fmt, ...) debugPrint("AST Debug [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define DEBUG_SYMTAB(fmt, ...) debugPrint("Symtab Debug [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)


#endif /* _GLOBALS_H_ */