%{
#include "calc.tab.h"
extern YYSTYPE yylval;
void abrirArq();

%}

spaco [ \t]+

%%
[0-9]+	{ yylval = atoi(yytext); return VALOR; }
\+	{ return SOMA; }
\-	{ return SUB; }
\*	{ return MULT; }
\(	{ return ABRPAR; }
\)	{ return FECPAR; }
\n	{ return FIMLIN; }
{spaco} {		 }
.       { return ERRO; }
%%

void abrirArq()
{
  yyin = fopen("entrada.txt", "r");
}
	 
