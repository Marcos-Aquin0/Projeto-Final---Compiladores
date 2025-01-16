%{
//GLC para gerar parser para calculadora simples

#include <iostream>
using namespace std;

extern "C"
{
  int yylex(void);
  void abrirArq();
}

void yyerror(char *);
%}

%start entrada
%token NUM ERRO FIMLIN

%%

entrada:	/*  vazia  */
	|	entrada result
	;
result	:	FIMLIN
	|	exp FIMLIN		{ cout << "Resposta: " << $1 << endl; }
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
  cout << "\nParser em execução...\n";
  abrirArq();
  return yyparse();
}

void yyerror(char * msg)
{
  extern char* yytext;
  cout << msg << ": " << yytext << yylval << yychar << endl;
}

