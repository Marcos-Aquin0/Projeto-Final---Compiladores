#include "globals.h"

// print com cor vermelha
void printError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\033[31m"); // Vermelho para erro
    vfprintf(stderr, format, args);
    fprintf(stderr, "\033[0m\n");
    va_end(args);
}

// print com cor verde
void printSuccess(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stdout, "\033[32m"); // Verde para sucesso
    vfprintf(stdout, format, args);
    fprintf(stdout, "\033[0m\n");
    va_end(args);
}