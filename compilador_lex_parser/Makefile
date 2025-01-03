all:
	flex lexer.l
	bison -d -v parser.y
	gcc lex.yy.c y.tab.c -lfl -ly -o lexer_and_parser
	./lexer_and_parser < sort.txt