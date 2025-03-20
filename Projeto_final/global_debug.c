#include "globals.h"

void debugPrint(const char* format, ...) {
    #ifdef DEBUG
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\033[33m"); // Amarelo para debug
    vfprintf(stderr, format, args);
    fprintf(stderr, "\033[0m\n");
    va_end(args);
    #endif
}