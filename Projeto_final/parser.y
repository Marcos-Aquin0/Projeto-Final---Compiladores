%{
#include "globals.h"
#include "asnt.h"
#include "symtab.h"

void yyerror(const char *s);
int yylex(); //lexer

ASTNode* root = NULL; // raiz da árvore sintática
%}

%code requires {
    #include "asnt.h"  /* Necessário para o Bison reconhecer ASTNode na união */
}

%union {
    int ival;           // para valor inteiros
    char *sval;         // para strings (ID)
    struct ASTNode *node;      // para nós da árvore sintática
}

%token <ival> NUM // token para números
%token <sval> ID // token para identificadores
%token ELSE IF INT RETURN VOID WHILE
%token OPAREN CPAREN OKEYS CKEYS OBRACKT CBRACKT
%token SUM SUB DIV MULT NQ BT BTE LT LTE EQ SMC COMMA ATRIB

%type <node> program decl_list decl var_decl spec_type func_decl
%type <node> params param_list param compound_decl local_decl
%type <node> state_list statement expr_decl select_decl iter_decl
%type <node> return_decl expr var simp_expr relational sum_expr
%type <node> sum term mult factor activation args arg_list

// Precedência e associatividade
%left SUM SUB
%left MULT DIV //multiplicação e divisão têm a mesma precedência, mas antes de soma e subtração
%nonassoc IFX //if sem else
%left ELSE 

%%

program:
    decl_list                   { 
        $$ = createNode(NODE_PROGRAM, $1, NULL, NULL, yylineno, NULL);
        root = $$;
    }
    ;
    //O símbolo $$ é usado para armazenar o nó da AST que representa a regra de produção atual.
    //O símbolo $1 é usado para acessar o nó da AST que representa a primeira regra de produção da regra atual.
    //createNode: Função que cria um novo nó na AST. Neste caso, está criando um nó do tipo NODE_PROGRAM com decl_list como seu filho.
    //root = $$;: Define a raiz da AST como o nó criado para program
    //yylineno é uma variável global que armazena o número da linha atual do arquivo de entrada.

decl_list:
    decl_list decl             { $$ = createNode(NODE_DECL_LIST, $1, $2, NULL, yylineno, NULL); }
    | decl                     { $$ = createNode(NODE_DECL_LIST, $1, NULL, NULL, yylineno, NULL); }
    ;

decl:
    func_decl                   { $$ = $1; }
    | var_decl               { $$ = $1; }
    ;

var_decl:
    spec_type ID SMC          { 
        //printf("DEBUG: Declarando variável simples %s do tipo %s\n", $2, $1->value);
        $$ = createNode(NODE_VAR_DECL, $1, NULL, $2, yylineno, $1->value); 
        $$->scope = current_scope(); 
        $$->isArray = 0; 
        $$->arraySize = 0; 
        $$->value = $2;  // Importante: armazenar o nome da variável
    }
    | spec_type ID OBRACKT NUM CBRACKT SMC {
        //printf("DEBUG: Declarando vetor %s do tipo %s com tamanho %d\n", $2, $1->value, $4);
        $$ = createNode(NODE_VAR_DECL, $1, NULL, $2, yylineno, $1->value);
        $$->scope = current_scope();
        $$->isArray = 1;  // Indica que é um vetor
        $$->arraySize = $4;  // Armazena o tamanho do vetor
        $$->value = $2;  // Importante: armazenar o nome da variável
    }
    | spec_type error SMC     { yyerror("Erro na declaração de variável"); $$ = NULL; }
    ;

spec_type:
    INT                       { $$ = createNode(NODE_SPEC_TYPE, NULL, NULL, "int", yylineno, NULL); }
    | VOID                    { $$ = createNode(NODE_SPEC_TYPE, NULL, NULL, "void", yylineno, NULL); }
    ;

func_decl:
    spec_type ID OPAREN params CPAREN compound_decl {
        ASTNode* idNode = createNode(NODE_FUNC, NULL, NULL, $2, yylineno, $1->value);
        $$ = createNode(NODE_FUNC_DECL, createNode(NODE_DECL, $1, idNode, NULL, yylineno, NULL), 
                       createNode(NODE_DECL, $4, $6, NULL, yylineno, NULL), $2, yylineno, $1->value);
    }
    | spec_type ID OPAREN CPAREN compound_decl {
        ASTNode* idNode = createNode(NODE_FUNC, NULL, NULL, $2, yylineno, $1->value);
        $$ = createNode(NODE_FUNC_DECL, createNode(NODE_DECL, $1, idNode, NULL, yylineno, NULL), 
                       createNode(NODE_DECL, NULL, $5, NULL, yylineno, NULL), $2, yylineno, $1->value);
    }
    ;

params:
    param_list                { $$ = createNode(NODE_PARAMS, $1, NULL, NULL, yylineno, NULL); }
    | VOID                    { $$ = createNode(NODE_PARAMS, NULL, NULL, "void", yylineno, NULL); }
    ;

param_list:
    param_list COMMA param    { $$ = createNode(NODE_PARAM_LIST, $1, $3, NULL, yylineno, NULL); }
    | param                   { $$ = createNode(NODE_PARAM_LIST, $1, NULL, NULL, yylineno, NULL); }
    ;

param:
    spec_type ID             { 
        //printf("DEBUG: Declarando parâmetro simples %s\n", $2);
        $$ = createNode(NODE_PARAM, $1, NULL, $2, yylineno, $1->value); 
        $$->scope = current_scope();
        $$->isArray = 0;
        $$->arraySize = 0;
        $$->value = $2;
    }
    | spec_type ID OBRACKT CBRACKT {
        //printf("DEBUG: Declarando parâmetro array %s[]\n", $2);
        $$ = createNode(NODE_PARAM, $1, NULL, $2, yylineno, $1->value);
        $$->scope = current_scope();
        $$->isArray = 1;  // Marca como array
        $$->arraySize = -1;  // -1 indica array sem tamanho específico (parâmetro de função)
        $$->value = $2;  // Nome do parâmetro
        $$->idType = $1->value;  // Tipo do parâmetro (int, void, etc)
    }
    ;

compound_decl:
    OKEYS local_decl state_list CKEYS {
        $$ = createNode(NODE_COMPOUND_DECL, $2, $3, NULL, yylineno, NULL);
        $$->scope = current_scope();  // Define o escopo do bloco composto
    }
    ;

local_decl:
    local_decl var_decl      { $$ = createNode(NODE_LOCAL_DECL, $1, $2, NULL, yylineno, NULL); }
    |                        { $$ = NULL; }
    ;

state_list:
    state_list statement     { $$ = createNode(NODE_STATE_LIST, $1, $2, NULL, yylineno, NULL); }
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
    expr SMC               { 
        // Verifica se a expressão é uma atribuição ou uma chamada de função
        if ($1->type == NODE_EXPR || ($1->type == NODE_ACTIVATION)) {
            $$ = createNode(NODE_EXPR_DECL, $1, NULL, NULL, yylineno, NULL);
        } else {
            yyerror("Expressão inválida: uso de vetor sem atribuição ou chamada de função");
            $$ = NULL;
        }
    }
    | SMC                  { $$ = NULL; }
    ;

select_decl:
    IF OPAREN expr CPAREN statement %prec IFX {
        $$ = createNode(NODE_SELECT_DECL, $3, $5, NULL, yylineno, NULL);
    }
    | IF OPAREN expr CPAREN statement ELSE statement {
        $$ = createNode(NODE_SELECT_DECL, $3, 
            createNode(NODE_STATEMENT, $5, $7, NULL, yylineno, NULL), NULL, yylineno, NULL);
    }
    ;

iter_decl:
    WHILE OPAREN expr CPAREN statement {
        $$ = createNode(NODE_ITER_DECL, $3, $5, NULL, yylineno, NULL);
    }
    ;

return_decl:
    RETURN SMC             { $$ = createNode(NODE_RETURN_DECL, NULL, NULL, NULL, yylineno, NULL); }
    | RETURN expr SMC      { $$ = createNode(NODE_RETURN_DECL, $2, NULL, NULL, yylineno, NULL); }
    ;

expr:
    var ATRIB expr        { $$ = createNode(NODE_EXPR, $1, $3, "=", yylineno, NULL); }
    | simp_expr           { $$ = $1; }
    ;

var:
    ID                    { 
        $$ = createNode(NODE_VAR, NULL, NULL, $1, yylineno, NULL); 
        $$->isArray = 0;
    }
    | ID OBRACKT expr CBRACKT {
        ASTNode* idNode = createNode(NODE_VAR, NULL, NULL, $1, yylineno, NULL);
        idNode->isArray = 1;  // Mark base node as array
        $$ = createNode(NODE_ARRAY_ACCESS, idNode, $3, $1, yylineno, NULL);
        $$->isArray = 1;  // Mark array access node
    }
    ;

simp_expr:
    sum_expr relational sum_expr {
        $$ = createNode(NODE_SIMP_EXPR, $1, createNode(NODE_RELATIONAL, $2, $3, NULL, yylineno, NULL), NULL, yylineno, NULL);
    }
    | sum_expr            { $$ = $1; }
    ;

relational:
    NQ                    { $$ = createNode(NODE_RELATIONAL, NULL, NULL, "!=", yylineno, NULL); }
    | BT                  { $$ = createNode(NODE_RELATIONAL, NULL, NULL, ">", yylineno, NULL); }
    | BTE                 { $$ = createNode(NODE_RELATIONAL, NULL, NULL, ">=", yylineno, NULL); }
    | LT                  { $$ = createNode(NODE_RELATIONAL, NULL, NULL, "<", yylineno, NULL); }
    | LTE                 { $$ = createNode(NODE_RELATIONAL, NULL, NULL, "<=", yylineno, NULL); }
    | EQ                  { $$ = createNode(NODE_RELATIONAL, NULL, NULL, "==", yylineno, NULL); }
    ;

sum_expr:
    sum_expr sum term     { $$ = createNode(NODE_SUM_EXPR, $1, $3, $2->value, yylineno, NULL); }
    | term               { $$ = $1; }
    ;

sum:
    SUM                  { $$ = createNode(NODE_SUM_EXPR, NULL, NULL, "+", yylineno, NULL); }
    | SUB               { $$ = createNode(NODE_SUM_EXPR, NULL, NULL, "-", yylineno, NULL); }
    ;

term:
    term mult factor     { $$ = createNode(NODE_TERM, $1, $3, $2->value, yylineno, NULL); }
    | factor            { $$ = $1; }
    ;

mult:
    MULT                { $$ = createNode(NODE_MULT, NULL, NULL, "*", yylineno, NULL); }
    | DIV              { $$ = createNode(NODE_MULT, NULL, NULL, "/", yylineno, NULL); }
    ;

factor:
    OPAREN expr CPAREN  { $$ = $2; }
    | var              { $$ = $1; }
    | activation       { $$ = $1; }
    | NUM             {
        char numStr[32];
        sprintf(numStr, "%d", $1);
        $$ = createNode(NODE_FACTOR, NULL, NULL, numStr, yylineno, NULL);
    }
    ;

activation:
    ID OPAREN args CPAREN {
        ASTNode* idNode = createNode(NODE_VAR, NULL, NULL, $1, yylineno, NULL);
        $$ = createNode(NODE_ACTIVATION, idNode, $3, NULL, yylineno, NULL);
    }
    ;

args:
    arg_list           { $$ = createNode(NODE_ARGS, $1, NULL, NULL, yylineno, NULL); }
    |                  { $$ = NULL; }
    ;

arg_list:
    arg_list COMMA expr { $$ = createNode(NODE_ARG_LIST, $1, $3, NULL, yylineno, NULL); }
    | expr             { $$ = createNode(NODE_ARG_LIST, $1, NULL, NULL, yylineno, NULL); }
    ;

%%

void yyerror(const char *s) {
    if(strcmp(s, "syntax error") != 0){
        fprintf(stderr, "Erro sintático na linha %d: %s\n", yylineno, s);
        exit(2);
    }
}