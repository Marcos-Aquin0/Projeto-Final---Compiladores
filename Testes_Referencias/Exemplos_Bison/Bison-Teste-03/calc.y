%{
//GLC para gerar parser para calculadora simples
#include <stdio.h>
#define YYDEBUG 1    //Para exibir na tela os passos da análise sintática quando o parser é executado
void yyerror(char *);
extern "C"
{
  int yylex(void);
  void abrirArq();
}
%}

%start entrada
%token NUM ERRO FIMLIN

//Para mostrar o valor semântico to token quando for debugar o parser
%printer { fprintf (yyoutput, "’%d’", $$); } NUM

%%

entrada :	/* entrada vazia */
	| 	entrada result;
result	:	FIMLIN
	|	exp FIMLIN		{ printf("\nResposta: %d\n", $1); }
	|	error FIMLIN	{ yyerrok; }
	;
exp	:	exp '+' termo 	{$$ = $1 + $3;}
	|	exp '-' termo 	{$$ = $1 - $3;}
	|	termo 			{$$ = $1;}
	;
termo	:	termo '*' fator	{$$ = $1 * $3;}
	|	fator				{$$ = $1;}
	;
fator	:	'('exp')'		{$$ = $2;}
	|	NUM 				{$$ = $1;}
	;
%%

int main()
{
  extern int yydebug;
  yydebug = 1;

  printf("\nParser em execução...\n");
  abrirArq();
  return yyparse();
}

void yyerror(char * msg)
{
  extern char* yytext;
  printf("\n%s : %s %d\n", msg, yytext, yylval);
}

