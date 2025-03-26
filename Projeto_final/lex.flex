%{
#include "parser.tab.h"
int lexErrorCount = 0;
%}
/*definição dos tokens*/
DIGIT	[0-9]
ELSE    else
IF      if
INT     int
RETURN  return 
VOID    void
WHILE   while 
LETTER	[a-zA-Z]
OPAREN	\(
CPAREN 	\)
OKEYS 	\{
CKEYS 	\}
OBRACKT \[
CBRACKT \]
SUM 	\+
SUB 	\-
DIV 	\/
MULT 	\*
NQ 	    !=
BT 	    \>
BTE 	\>=
LT 	    \<
LTE 	\<=
EQ 	    ==
SMC 	;
COMMA 	,
ATRIB 	= 
POINT   \.
COMINIT \/\* 
COMEND  \*\/
WHITESPACE [ ]
TAB [\t]
ENTER [\n]

%% //gramática regular
{ELSE}                                  { return ELSE; }
{IF}                                    { return IF; }
{INT}                                   { return INT; }
{RETURN}                                { return RETURN; } 
{VOID}                                  { return VOID; }
{WHILE}                                 { return WHILE; } 
{DIGIT}{DIGIT}*		                    { yylval.ival = atoi(yytext); return NUM; } 

{COMINIT}([^*]|{MULT}[^/])*{COMEND}     {
                                            for (int i = 0; yytext[i] != '\0'; i++) {
                                                if (yytext[i] == '\n') {
                                                    yylineno++;
                                                }
                                            }
                                         } //reconhece a quantidade de linhas dentro do comentário
                        
{LETTER}{LETTER}* 	                    { yylval.sval = strdup(yytext); return ID; }
{OPAREN}		                        { return OPAREN; }
{CPAREN}		                        { return CPAREN; }
{OKEYS}			                        { return OKEYS; }
{CKEYS}			                        { return CKEYS; }
{OBRACKT}		                        { return OBRACKT; }
{CBRACKT}		                        { return CBRACKT; }
{SUM}			                        { return SUM; }
{SUB}			                        { return SUB; }
{MULT}			                        { return MULT; }
{DIV}			                        { return DIV; }
{BT}			                        { return BT; }
{BTE}			                        { return BTE; }
{LT}			                        { return LT; }
{LTE}			                        { return LTE; }
{EQ}			                        { return EQ; }
{SMC}			                        { return SMC; }
{COMMA}			                        { return COMMA; }
{ATRIB}			                        { return ATRIB; }
{NQ}			                        { return NQ; }
{WHITESPACE}                            { }
{TAB}                                   { }
{ENTER}                                 { yylineno++; }

{DIGIT}{DIGIT}*{POINT}[^0-9] {
    printError("Erro léxico: Numero float malformado '%s' na linha %d\n", yytext, yylineno);
    // exit(1); 
    lexErrorCount++;
};

. {
    printError("Erro léxico: Caractere invalido '%s' na linha %d\n", yytext, yylineno);
    // exit(1);
    lexErrorCount++;
}

{COMINIT}([^*])* {
    printError("Erro léxico: Comentario nao encerrado iniciado na linha %d\n", yylineno);
    // exit(1);
    lexErrorCount++;
}

{LETTER}+{DIGIT}+{LETTER}* {
    printError("Erro léxico: Variavel '%s' no formato inválido na linha %d\n", yytext, yylineno);
    // exit(1);
    lexErrorCount++;
} 

%%

int yywrap() {
    return 1;
}
//A implementação padrão de yywrap simplesmente retorna 1, indicando que o final da entrada foi alcançado e que não há mais dados a serem processados

//atoi é utilizado para converter uma string que representa um número em um valor do tipo inteiro para ser lido pelo parser
//yytext contém o texto que corresponde ao token atual
//yylval é uma união definida no arquivo parser.y que pode armazenar diferentes tipos de valores. ival é o membro da união usado para armazenar valores inteiros
//A função strdup duplica a string yytext, alocando memória suficiente para armazenar a cópia da string.
//yylineno é uma variável global que mantém o número da linha atual do arquivo de entrada