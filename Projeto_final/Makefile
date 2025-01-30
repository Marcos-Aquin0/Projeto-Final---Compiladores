# Compilador e flags
CC = gcc
CFLAGS = -Wall -g

# Arquivos fonte
FLEX_FILE = lex.flex
BISON_FILE = parser.y
MAIN_FILE = main.c
AST_FILES = ast.c

# Arquivos gerados
LEX_C = lex.yy.c
BISON_C = parser.tab.c
BISON_H = parser.tab.h
EXEC = cminus_parser

# Regras principais
all: $(EXEC)

$(LEX_C): $(FLEX_FILE) $(BISON_H)
	flex -o $(LEX_C) $(FLEX_FILE)

$(BISON_C) $(BISON_H): $(BISON_FILE)
	bison -d -o $(BISON_C) $(BISON_FILE) -Wcounterexamples

$(EXEC): $(BISON_C) $(LEX_C) $(MAIN_FILE) $(AST_FILES)
	$(CC) $(CFLAGS) -o $(EXEC) $(BISON_C) $(LEX_C) $(MAIN_FILE) $(AST_FILES) -lfl

# Limpeza
clean:
	rm -f $(LEX_C) $(BISON_C) $(BISON_H) $(EXEC)

.PHONY: all clean
