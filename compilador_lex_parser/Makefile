all:
	flex lexer.l
	bison -d -v -Wcounterexamples -Wother parser.y
	gcc lex.yy.c parser.tab.c -lfl -o lexer_and_parser
	./lexer_and_parser < codigo.c-