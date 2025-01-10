%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex();

extern int yylineno;
%}

%union {
    int ival;       // Para números inteiros
    char *sval;     // Para strings (identificadores)
}

%token <ival> NUM 
%token <sval> ID
%token ERROR
%token LETTER ELSE IF INT RETURN VOID WHILE
%token OPAREN CPAREN OKEYS CKEYS OBRACKT CBRACKT
%token SUM SUB DIV MULT NQ BT BTE LT LTE EQ SMC COMMA ATRIB
%token POINT COMINIT COMEND WHITESPACE TAB ENTER 

%left SUM SUB
%left MULT DIV
%nonassoc IFX
%left ELSE

%%

program:
    decl_list
    ;

decl_list:
    decl_list decl
    | decl
    ;

decl:
    var_decl
    | func_decl
    ;

var_decl:
    spec_type ID SMC
    | spec_type ID OBRACKT NUM CBRACKT SMC
    | spec_type error SMC { yyerror("Erro na declaração de variável");}
    ;

spec_type:
    INT
    | VOID
    ;

func_decl:
    spec_type ID OPAREN params CPAREN compound_decl
    | error compound_decl { yyerror("Erro na declaração de função");}
    ;

params:
    param_list
    | VOID
    ;

param_list:
    param_list COMMA param
    | param
    ;

param:
    spec_type ID
    | spec_type ID OBRACKT CBRACKT
    ;

compound_decl:
    OKEYS local_decl state_list CKEYS
    | error CKEYS      { yyerror("Erro no bloco de declarações");}
    ;

local_decl:
    local_decl var_decl
    |
    ;

state_list:
    state_list statement
    |
    ;

statement:
    expr_decl
    | compound_decl
    | select_decl
    | iter_decl
    | return_decl
    ;

expr_decl:
    expr SMC
    | SMC
    | error SMC        { yyerror("Erro na expressão");}
    ;

select_decl:
    IF OPAREN expr CPAREN statement %prec IFX
    | IF OPAREN expr CPAREN statement ELSE statement
    | IF error statement { yyerror("Erro na condição do if");}
    ;

iter_decl:
    WHILE OPAREN expr CPAREN statement
    | WHILE error statement { yyerror("Erro na condição do while");}
    ;

return_decl:
    RETURN SMC
    | RETURN expr SMC
    | RETURN error SMC  { yyerror("Erro na expressão do return");}
    ;

expr:
    var ATRIB expr
    | simp_expr
    ;

var:
    ID
    | ID OBRACKT expr CBRACKT
    ;

simp_expr:
    sum_expr relational sum_expr
    | sum_expr
    ;

relational:
    NQ
    | BT
    | BTE
    | LT
    | LTE
    | EQ
    ;

sum_expr:
    sum_expr sum term
    | term
    ;

sum:
    SUM
    | SUB
    ;

term:
    term mult factor
    | factor
    ;

mult:
    MULT
    | DIV
    ;

factor:
    OPAREN expr CPAREN
    | var
    | activation
    | NUM
    ;

activation:
    ID OPAREN args CPAREN
    | ID OPAREN error CPAREN { yyerror("Erro nos argumentos da função");}
    ;

args:
    arg_list
    |
    ;

arg_list:
    arg_list COMMA expr
    | expr
    ;

%%

void yyerror(const char *s) {
    if(strcmp(s, "syntax error") != 0){
        fprintf(stderr, "Erro sintático na linha %d: %s\n", yylineno, s);
        exit(2);
    }
}