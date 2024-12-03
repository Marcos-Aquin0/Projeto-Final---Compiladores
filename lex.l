%{
#include <stdio.h>
int linha = 1;
%}

DIGIT	[0-9]
ELSE    else
IF      if
INT     int
RETURN  return 
VOID    void
WHILE   while 
LETTER	[a-zA-Z]
OPAREN	\(
CPAREN 	\)
OKEYS 	\{
CKEYS 	\}
OBRACKT \[
CBRACKT \]
SUM 	\+
SUB 	\-
DIV 	\/
MULT 	\*
NQ 	    !=
BT 	    \>
BTE 	\>=
LT 	    \<
LTE 	\<=
EQ 	    ==
SMC 	;
COMMA 	,
ATRIB 	= 
POINT   \.
COMINIT \/\* 
COMEND  \*\/
ERROR   [\:\?\!\@\#\$\%\&\_]
%%


{ELSE}          fprintf(stdout, "ELSE\n");
{IF}            fprintf(stdout, "IF\n");
{INT}           fprintf(stdout, "INT\n");
{RETURN}        fprintf(stdout, "RETURN\n"); 
{VOID}          fprintf(stdout, "VOID\n");
{WHILE}         fprintf(stdout, "WHILE\n"); 
{DIGIT}{DIGIT}*		                        fprintf(stdout, "INT\n");
{DIGIT}{DIGIT}*{POINT}{DIGIT}{DIGIT}*		fprintf(stdout, "FLOAT\n");
{COMINIT}([^*]|{MULT}[^/])*{COMEND}	        /* ignora comentarios */
{LETTER}{LETTER}* 	                        fprintf(stdout, "ID\n");
{OPAREN}		fprintf(stdout, "OPAREN\n");
{CPAREN}		fprintf(stdout, "CPAREN\n");
{OKEYS}			fprintf(stdout, "OKEYS\n");
{CKEYS}			fprintf(stdout, "CKEYS\n");
{OBRACKT}		fprintf(stdout, "OBRACKT\n");
{CBRACKT}		fprintf(stdout, "CBRACKT\n");
{SUM}			fprintf(stdout, "SUM\n");
{SUB}			fprintf(stdout, "SUB\n");
{MULT}			fprintf(stdout, "MULT\n");
{DIV}			fprintf(stdout, "DIV\n");
{BT}			fprintf(stdout, "BT\n");
{BTE}			fprintf(stdout, "BTE\n");
{LT}			fprintf(stdout, "LT\n");
{LTE}			fprintf(stdout, "LTE\n");
{EQ}			fprintf(stdout, "EQ\n");
{SMC}			fprintf(stdout, "SMC\n");
{COMMA}			fprintf(stdout, "COMMA\n");
{ATRIB}			fprintf(stdout, "ATRIB\n");
{NQ}			fprintf(stdout, "NQ\n");

{DIGIT}{DIGIT}*{POINT}[^0-9] {
    fprintf(stdout, "Erro: Numero malformado '%s' na linha %d\n", yytext, linha);
};
{ERROR} {
    fprintf(stdout, "Erro: Caractere invalido '%s' na linha %d\n", yytext, linha);
}
{COMINIT}([^*])* {
    fprintf(stdout, "Erro: Comentario nao encerrado iniciado na linha %d\n", linha);
}

\n {
    linha++; // Incrementa manualmente ao encontrar uma nova linha
}

<<EOF>>			return 0;

%%



int main(int argc, char *argv[]) {
    FILE *f_in;
    if (argc == 2) {
        if (f_in = fopen(argv[1], "r")) {
            yyin = f_in;
        } else {
            perror(argv[0]);
        }
    } else {
        yyin = stdin;
    }
    yylex();
    return 0;
}
