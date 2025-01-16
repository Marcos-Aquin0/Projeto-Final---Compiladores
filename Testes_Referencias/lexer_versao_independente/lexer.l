%{
#include <stdio.h>
int linha = 0;
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
WHITESPACE [ ]
TAB [\t]
ENTER [\n]
ERROR [.]
%%


{ELSE}          printf("ELSE");
{IF}            printf("IF");
{INT}           printf("INT");
{RETURN}        printf("RETURN"); 
{VOID}          printf("VOID");
{WHILE}         printf("WHILE"); 
{DIGIT}{DIGIT}*		                        printf("INT");
{DIGIT}{DIGIT}*{POINT}{DIGIT}{DIGIT}*		printf("FLOAT");
{COMINIT}([^*]|{MULT}[^/])*{COMEND}	        /* ignora comentarios */
{LETTER}{LETTER}* 	                        printf("ID");
{OPAREN}		printf("OPAREN");
{CPAREN}		printf("CPAREN");
{OKEYS}			printf("OKEYS");
{CKEYS}			printf("CKEYS");
{OBRACKT}		printf("OBRACKT");
{CBRACKT}		printf("CBRACKT");
{SUM}			printf("SUM");
{SUB}			printf("SUB");
{MULT}			printf("MULT");
{DIV}			printf("DIV");
{BT}			printf("BT");
{BTE}			printf("BTE");
{LT}			printf("LT");
{LTE}			printf("LTE");
{EQ}			printf("EQ");
{SMC}			printf("SMC");
{COMMA}			printf("COMMA");
{ATRIB}			printf("ATRIB");
{NQ}			printf("NQ");
{WHITESPACE}    printf(" ");
{TAB}           printf("\t");
{DIGIT}{DIGIT}*{POINT}[^0-9] {
    fprintf(stdout,"Erro: Numero malformado '%s' na linha %d\n", yytext, linha);
    exit(1); //exit 1 é o erro
};
{ERROR} {
    fprintf(stdout,"Erro: Caractere invalido '%s' na linha %d\n", yytext, linha);
    exit(1);
}
{COMINIT}([^*])* {
    fprintf(stdout,"Erro: Comentario nao encerrado iniciado na linha %d\n", linha);
    exit(1);
}

{LETTER}+{DIGIT}+{LETTER}* {
    fprintf(stdout,"Erro: Variavel '%s' no formato inválido na linha %d\n", yytext, linha);
    exit(1);
} 

{ENTER} {
    linha++; // Incrementa manualmente ao encontrar uma nova linha
    printf("\n");
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
