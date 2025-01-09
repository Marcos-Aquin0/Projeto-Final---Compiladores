%{
#include "parser.tab.h"
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

{ELSE}                                  { return ELSE; }
{IF}                                    { return IF; }
{INT}                                   { return INT; }
{RETURN}                                { return RETURN; } 
{VOID}                                  { return VOID; }
{WHILE}                                 { return WHILE; } 
{DIGIT}{DIGIT}*		                    { yylval.ival = atoi(yytext); return NUM; }
{COMINIT}([^*]|{MULT}[^/])*{COMEND}     {yylineno++;} /* ignora comentarios, apenas linha++ */
{LETTER}{LETTER}* 	                    { yylval.sval = strdup(yytext); return ID; }
{OPAREN}		                        { return OPAREN; }
{CPAREN}		                        { return CPAREN; }
{OKEYS}			                        { return OKEYS; }
{CKEYS}			                        { return CKEYS; }
{OBRACKT}		                        { return OBRACKT; }
{CBRACKT}		                        { return CBRACKT; }
{SUM}			                        { return SUM; }
{SUB}			                        { return SUB; }
{MULT}			                        { return MULT; }
{DIV}			                        { return DIV; }
{BT}			                        { return BT; }
{BTE}			                        { return BTE; }
{LT}			                        { return LT; }
{LTE}			                        { return LTE; }
{EQ}			                        { return EQ; }
{SMC}			                        { return SMC; }
{COMMA}			                        { return COMMA; }
{ATRIB}			                        { return ATRIB; }
{NQ}			                        { return NQ; }
{WHITESPACE}                            { }
{TAB}                                   { }

{ENTER} {
    yylineno++; // Incrementa manualmente ao encontrar uma nova linha
}
{DIGIT}{DIGIT}*{POINT}[^0-9] {
    fprintf(stdout,"Erro léxico: Numero malformado '%s' na linha %d\n", yytext, yylineno);
    exit(1); //exit 1 é o erro
};
{ERROR} {
    fprintf(stdout,"Erro léxico: Caractere invalido '%s' na linha %d\n", yytext, yylineno);
    exit(1);
}
{COMINIT}([^*])* {
    fprintf(stdout,"Erro léxico: Comentario nao encerrado iniciado na linha %d\n", yylineno);
    exit(1);
}

{LETTER}+{DIGIT}+{LETTER}* {
    fprintf(stdout,"Erro léxico: Variavel '%s' no formato inválido na linha %d\n", yytext, yylineno);
    exit(1);
} 

%%

int yywrap() {
    return 1;
}