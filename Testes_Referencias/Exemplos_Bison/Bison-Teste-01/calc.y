%{
//GLC para gerar parser para calculadora

#include <iostream>
using namespace std;

extern "C"
{
//  int yyparse(void);
  int yylex(void);
  void abrirArq();
}

void yyerror(char *);
%}

%start entrada
%token VALOR FIMLIN ERRO
%left SOMA SUB
%left MULT
%token ABRPAR FECPAR

%%

entrada:	/*  vazia  */
	|	entrada result
	;
result:	FIMLIN
	|	expr FIMLIN	{ cout << "Resposta: " << $1 << endl; }
	|	error FIMLIN	{ yyerrok; }
	;
expr	:	expr SOMA expr { $$ = $1 + $3; }
	|	expr MULT expr { $$ = $1 * $3; }
      | 	expr SUB expr  { $$ = $1 - $3; }
	|	ABRPAR expr FECPAR	{ $$ = $2; }
	|	VALOR
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
  cout << msg << ": " << yytext << endl;
}

