%{
#include "calc.tab.h"
extern YYSTYPE yylval;
void abrirArq();

%}

spaco [ \t]+

%%
[0-9]+	{ yylval = atoi(yytext); return NUM; }
\+	{ return '+'; }
\-	{ return '-'; }
\*	{ return '*'; }
\(	{ return '('; }
\)	{ return ')'; }
\n	{ return FIMLIN; }
{spaco} {		 }
.       { return ERRO; }
%%

void abrirArq()
{
  yyin = fopen("entrada.txt", "r");
}
	 
