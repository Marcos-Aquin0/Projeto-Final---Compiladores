%{
#include <stdio.h>
#include <stdlib.h>

// Extern para integrar com o léxico
extern int yylex();
extern int yyparse();
extern void yyerror(const char *s);

// Variável global para o valor de tokens
extern int yylval;
%}

// Declaração de tokens
%token ELSE IF INT RETURN VOID WHILE
%token NUMBER_INT NUMBER_FLOAT ID
%token OPAREN CPAREN OKEYS CKEYS
%token OBRACKT CBRACKT SUM SUB DIV MULT
%token NQ BT BTE LT LTE EQ SMC COMMA ATRIB

%%

// Regras gramaticais
programa:
    declaracoes
    ;

declaracoes:
    declaracao
    | declaracoes declaracao
    ;

declaracao:
    tipo ID SMC
    | tipo ID OPAREN parametros CPAREN corpo
    ;

tipo:
    INT
    | VOID
    ;

parametros:
    /* vazio */
    | tipo ID
    | parametros COMMA tipo ID
    ;

corpo:
    OKEYS comandos CKEYS
    ;

comandos:
    comando
    | comandos comando
    ;

comando:
    expressao SMC
    | declaracao
    | comando_composto
    ;

comando_composto:
    OKEYS comandos CKEYS
    ;

expressao:
    ID ATRIB expressao
    | expressao SUM expressao
    | expressao SUB expressao
    | termo
    ;

termo:
    ID
    | NUMBER_INT
    | NUMBER_FLOAT
    ;

%%

// Função de erro
void yyerror(const char *s) {
    fprintf(stderr, "Erro: %s\\n", s);
}

int main() {
    return yyparse();
}
