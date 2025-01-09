%{
#include "parser.tab.h"

char error_msg[256];
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
ERROR .

%%

{ELSE}                                  { return ELSE; }
{IF}                                    { return IF; }
{INT}                                   { return INT; }
{RETURN}                                { return RETURN; } 
{VOID}                                  { return VOID; }
{WHILE}                                 { return WHILE; } 
{DIGIT}{DIGIT}*		                    { yylval.ival = atoi(yytext); return NUM; }
{COMINIT}([^*]|{MULT}[^/])*{COMEND}     /* ignora comentarios */
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
{ENTER}                                 { yylineno++; }

{DIGIT}{DIGIT}*{POINT}[^0-9] {
    snprintf(error_msg, sizeof(error_msg), "Número malformado '%s' na linha %d", yytext, yylineno);
    return ERROR;
}

{ERROR} {
    snprintf(error_msg, sizeof(error_msg), "Caractere inválido '%s' na linha %d", yytext, yylineno);
    return ERROR;
}

{COMINIT}([^*])* {
    snprintf(error_msg, sizeof(error_msg), "Comentário não encerrado iniciado na linha %d", yylineno);
    return ERROR;
}

{LETTER}+{DIGIT}+{LETTER}* {
    snprintf(error_msg, sizeof(error_msg), "Variável '%s' no formato inválido na linha %d", yytext, yylineno);
    return ERROR;
}

%%

int yywrap() {
    return 1;
}