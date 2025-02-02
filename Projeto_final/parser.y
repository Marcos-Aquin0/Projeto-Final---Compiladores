%{
#include "globals.h"
#include "ast.h"

void yyerror(const char *s);
int yylex();

ASTNode* root = NULL; // Root of the AST
%}

%code requires {
    #include "ast.h"  /* Necessário para o Bison reconhecer ASTNode na união */
}

%union {
    int ival;           // For integer values
    char *sval;         // For strings (identifiers)
    struct ASTNode *node;      // For AST nodes
}

%token <ival> NUM 
%token <sval> ID
%token ELSE IF INT RETURN VOID WHILE
%token OPAREN CPAREN OKEYS CKEYS OBRACKT CBRACKT
%token SUM SUB DIV MULT NQ BT BTE LT LTE EQ SMC COMMA ATRIB

%type <node> program decl_list decl var_decl spec_type func_decl
%type <node> params param_list param compound_decl local_decl
%type <node> state_list statement expr_decl select_decl iter_decl
%type <node> return_decl expr var simp_expr relational sum_expr
%type <node> sum term mult factor activation args arg_list

%left SUM SUB
%left MULT DIV
%nonassoc IFX
%left ELSE

%%

program:
    decl_list                   { 
        $$ = createNode(NODE_PROGRAM, $1, NULL, NULL);
        root = $$;
    }
    ;

decl_list:
    decl_list decl             { $$ = createNode(NODE_DECL_LIST, $1, $2, NULL); }
    | decl                     { $$ = createNode(NODE_DECL_LIST, $1, NULL, NULL); }
    ;

decl:
    var_decl                   { $$ = $1; }
    | func_decl               { $$ = $1; }
    ;

var_decl:
    spec_type ID SMC          { $$ = createNode(NODE_VAR_DECL, $1, NULL, $2); }
    | spec_type ID OBRACKT NUM CBRACKT SMC {
        ASTNode* idNode = createNode(NODE_VAR, NULL, NULL, $2);
        char numStr[32];
        sprintf(numStr, "%d", $4);
        ASTNode* numNode = createNode(NODE_FACTOR, NULL, NULL, numStr);
        $$ = createNode(NODE_VAR_DECL, $1, createNode(NODE_VAR, idNode, numNode, NULL), NULL);
    }
    | spec_type error SMC     { yyerror("Erro na declaração de variável"); $$ = NULL; }
    ;

spec_type:
    INT                       { $$ = createNode(NODE_SPEC_TYPE, NULL, NULL, "int"); }
    | VOID                    { $$ = createNode(NODE_SPEC_TYPE, NULL, NULL, "void"); }
    ;

func_decl:
    spec_type ID OPAREN params CPAREN compound_decl {
        ASTNode* idNode = createNode(NODE_VAR, NULL, NULL, $2);
        $$ = createNode(NODE_FUNC_DECL, createNode(NODE_DECL, $1, idNode, NULL), 
                       createNode(NODE_DECL, $4, $6, NULL), NULL);
    }
    ;

params:
    param_list                { $$ = createNode(NODE_PARAMS, $1, NULL, NULL); }
    | VOID                    { $$ = createNode(NODE_PARAMS, NULL, NULL, "void"); }
    ;

param_list:
    param_list COMMA param    { $$ = createNode(NODE_PARAM_LIST, $1, $3, NULL); }
    | param                   { $$ = createNode(NODE_PARAM_LIST, $1, NULL, NULL); }
    ;

param:
    spec_type ID             { $$ = createNode(NODE_PARAM, $1, NULL, $2); }
    | spec_type ID OBRACKT CBRACKT {
        ASTNode* idNode = createNode(NODE_VAR, NULL, NULL, $2);
        $$ = createNode(NODE_PARAM, $1, idNode, NULL);
    }
    ;

compound_decl:
    OKEYS local_decl state_list CKEYS {
        $$ = createNode(NODE_COMPOUND_DECL, $2, $3, NULL);
    }
    ;

local_decl:
    local_decl var_decl      { $$ = createNode(NODE_LOCAL_DECL, $1, $2, NULL); }
    |                        { $$ = NULL; }
    ;

state_list:
    state_list statement     { $$ = createNode(NODE_STATE_LIST, $1, $2, NULL); }
    |                        { $$ = NULL; }
    ;

statement:
    expr_decl               { $$ = $1; }
    | compound_decl         { $$ = $1; }
    | select_decl           { $$ = $1; }
    | iter_decl             { $$ = $1; }
    | return_decl           { $$ = $1; }
    ;

expr_decl:
    expr SMC               { $$ = createNode(NODE_EXPR_DECL, $1, NULL, NULL); }
    | SMC                  { $$ = NULL; }
    ;

select_decl:
    IF OPAREN expr CPAREN statement %prec IFX {
        $$ = createNode(NODE_SELECT_DECL, $3, $5, NULL);
    }
    | IF OPAREN expr CPAREN statement ELSE statement {
        $$ = createNode(NODE_SELECT_DECL, $3, 
            createNode(NODE_STATEMENT, $5, $7, NULL), NULL);
    }
    ;

iter_decl:
    WHILE OPAREN expr CPAREN statement {
        $$ = createNode(NODE_ITER_DECL, $3, $5, NULL);
    }
    ;

return_decl:
    RETURN SMC             { $$ = createNode(NODE_RETURN_DECL, NULL, NULL, NULL); }
    | RETURN expr SMC      { $$ = createNode(NODE_RETURN_DECL, $2, NULL, NULL); }
    ;

expr:
    var ATRIB expr        { $$ = createNode(NODE_EXPR, $1, $3, "="); }
    | simp_expr           { $$ = $1; }
    ;

var:
    ID                    { $$ = createNode(NODE_VAR, NULL, NULL, $1); }
    | ID OBRACKT expr CBRACKT {
        ASTNode* idNode = createNode(NODE_VAR, NULL, NULL, $1);
        $$ = createNode(NODE_VAR, idNode, $3, NULL);
    }
    ;

simp_expr:
    sum_expr relational sum_expr {
        $$ = createNode(NODE_SIMP_EXPR, $1, createNode(NODE_RELATIONAL, $2, $3, NULL), NULL);
    }
    | sum_expr            { $$ = $1; }
    ;

relational:
    NQ                    { $$ = createNode(NODE_RELATIONAL, NULL, NULL, "!="); }
    | BT                  { $$ = createNode(NODE_RELATIONAL, NULL, NULL, ">"); }
    | BTE                 { $$ = createNode(NODE_RELATIONAL, NULL, NULL, ">="); }
    | LT                  { $$ = createNode(NODE_RELATIONAL, NULL, NULL, "<"); }
    | LTE                 { $$ = createNode(NODE_RELATIONAL, NULL, NULL, "<="); }
    | EQ                  { $$ = createNode(NODE_RELATIONAL, NULL, NULL, "=="); }
    ;

sum_expr:
    sum_expr sum term     { $$ = createNode(NODE_SUM_EXPR, $1, $3, $2->value); }
    | term               { $$ = $1; }
    ;

sum:
    SUM                  { $$ = createNode(NODE_SUM_EXPR, NULL, NULL, "+"); }
    | SUB               { $$ = createNode(NODE_SUM_EXPR, NULL, NULL, "-"); }
    ;

term:
    term mult factor     { $$ = createNode(NODE_TERM, $1, $3, $2->value); }
    | factor            { $$ = $1; }
    ;

mult:
    MULT                { $$ = createNode(NODE_MULT, NULL, NULL, "*"); }
    | DIV              { $$ = createNode(NODE_MULT, NULL, NULL, "/"); }
    ;

factor:
    OPAREN expr CPAREN  { $$ = $2; }
    | var              { $$ = $1; }
    | activation       { $$ = $1; }
    | NUM             {
        char numStr[32];
        sprintf(numStr, "%d", $1);
        $$ = createNode(NODE_FACTOR, NULL, NULL, numStr);
    }
    ;

activation:
    ID OPAREN args CPAREN {
        ASTNode* idNode = createNode(NODE_VAR, NULL, NULL, $1);
        $$ = createNode(NODE_ACTIVATION, idNode, $3, NULL);
    }
    ;

args:
    arg_list           { $$ = createNode(NODE_ARGS, $1, NULL, NULL); }
    |                  { $$ = NULL; }
    ;

arg_list:
    arg_list COMMA expr { $$ = createNode(NODE_ARG_LIST, $1, $3, NULL); }
    | expr             { $$ = createNode(NODE_ARG_LIST, $1, NULL, NULL); }
    ;

%%

void yyerror(const char *s) {
    if(strcmp(s, "syntax error") != 0){
        fprintf(stderr, "Erro sintático na linha %d: %s\n", yylineno, s);
        exit(2);
    }
}