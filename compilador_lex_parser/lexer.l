%{
#include <stdio.h>
#include "y.tab.h"

// Variável para rastrear a linha
int linha = 0;
%}

// Definições de tokens
ELSE		else
IF			if
INT			int
RETURN		return
VOID		void
WHILE		while
DIGIT		[0-9]
LETTER		[a-zA-Z]
ID			{LETTER}({LETTER}|{DIGIT})*
NUMBER_INT	{DIGIT}+
NUMBER_FLOAT	{DIGIT}+\.{DIGIT}+
OPAREN		\(
CPAREN		\)
OKEYS		\{
CKEYS		\}
OBRACKT		\[
CBRACKT		\]
SUM			\+
SUB			\-
DIV			\/
MULT			\*
NQ			!=
BT			\>
BTE			\>=
LT			\<
LTE			\<=
EQ			==
SMC			;
COMMA		,
ATRIB		=
COMINIT		\/\*
COMEND		\*\/

%%

{ELSE}          { return ELSE; }
{IF}            { return IF; }
{INT}           { return INT; }
{RETURN}        { return RETURN; }
{VOID}          { return VOID; }
{WHILE}         { return WHILE; }

{NUMBER_INT}    { yylval = atoi(yytext); return NUMBER_INT; }
{NUMBER_FLOAT}  { yylval = atof(yytext); return NUMBER_FLOAT; }
{ID}            { yylval = strdup(yytext); return ID; }

{OPAREN}        { return OPAREN; }
{CPAREN}        { return CPAREN; }
{OKEYS}         { return OKEYS; }
{CKEYS}         { return CKEYS; }
{OBRACKT}       { return OBRACKT; }
{CBRACKT}       { return CBRACKT; }
{SUM}           { return SUM; }
{SUB}           { return SUB; }
{DIV}           { return DIV; }
{MULT}          { return MULT; }
{NQ}            { return NQ; }
{BT}            { return BT; }
{BTE}           { return BTE; }
{LT}            { return LT; }
{LTE}           { return LTE; }
{EQ}            { return EQ; }
{SMC}           { return SMC; }
{COMMA}         { return COMMA; }
{ATRIB}         { return ATRIB; }

{COMINIT}[^*]*{COMEND} { /* Ignorar comentários */ }
{COMINIT}[^*]*         { printf("Erro: comentário não fechado\n"); }

\n               { linha++; }
[ \t]+          { /* Ignorar espaços em branco */ }
.                { printf("Erro: caractere desconhecido '%s' na linha %d\n", yytext, linha); }

%%

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Erro ao abrir o arquivo");
            return 1;
        }
    }

    while (yylex())
        ;

    return 0;
}
