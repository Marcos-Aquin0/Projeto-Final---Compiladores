%{
//bibliotecas
#include <stdio.h>

//variaveis globais
int linha = 0;
int tok;

//Definicoes de erros
#define ERRO_SINTATICO 1
#define ERRO_LEXICO 2

// Definicoes de tokens
#define T_BEGIN 590
#define T_IF 595
#define T_ELSE 405
#define T_END 396
#define T_THEN 888
#define T_SMC 517
#define T_ATRIB 222
#define T_NUM 210
#define T_ID 666
#define T_PRINT 723

// Prototipos de funcoes
void S();
void L();
void E();
void eat(int t);
int getToken();
void ERRO(const char *mensagem, int tipoErro);
%}

DIGIT   [0-9]
ELSE    else
IF      if
LETTER  [a-zA-Z]
BEGIN   begin
END     end
THEN    then
SMC     ;
ATRIB   =
PRINT   print
%%

{PRINT}             {printf("PRINT"); return T_PRINT;} 
{BEGIN}             {printf("BEGIN");return T_BEGIN;}
{IF}                {printf("IF"); return T_IF;}
{ELSE}              {printf("ELSE"); return T_ELSE;}
{END}               {printf("END"); return T_END;}
{THEN}              {printf("THEN"); return T_THEN;}
{SMC}               {printf("SMC"); return T_SMC;}
{DIGIT}{DIGIT}*     {printf("NUM"); return T_NUM;}
{LETTER}{LETTER}*   {printf("ID"); return T_ID;}
{ATRIB}             {printf("ATRIB"); return T_ATRIB;}

\n                  { printf("\n"); linha++; }
[\t]                {printf("\t");}
" "                 {printf(" ");}
[\r]+               /* Ignorar espacos e tabulacoes */
.                   { printf("Erro lexico - token desconhecido na linha %d: %s\n", linha, yytext); }
<<EOF>>             return 0;
%%

void S()
{
    switch (tok)
    {
        case T_BEGIN:
            eat(T_BEGIN);
            S();
            L();
            break;
        case T_IF:
            eat(T_IF);
            E();
            eat(T_THEN); // Verifica e consome THEN
            S();
            eat(T_ELSE); // Verifica e consome ELSE
            S();
            break;
        case T_PRINT:
            eat(T_PRINT);
            eat(T_ID); // Verifica e consome ID apos PRINT
            break;
        default:
            ERRO("De acordo com a GLC, token inesperado", ERRO_LEXICO);
    }
}

void L()
{
    switch (tok)
    {
        case T_SMC:
            eat(T_SMC);
            S();
            L();
            break;
        case T_END:
            eat(T_END);
            break;
        default:
            ERRO("De acordo com a GLC, token inesperado", ERRO_LEXICO);
    }
}

void E()
{
    switch (tok)
    {
        case T_ID:
            eat(T_ID);
            eat(T_ATRIB);
            eat(T_NUM);
            break;
        default:
            ERRO("De acordo com a GLC, token inesperado", ERRO_LEXICO);
    }
}

void eat(int t)
{
    if (tok == t) //se o token é esperado
    {
        tok = getToken(); // Avança para o próximo token
    }
    else
    {
        switch (t)
        {
            case T_PRINT:
                ERRO("De acordo com a GLC, está faltando o token PRINT", ERRO_SINTATICO);
                break; 
            case T_IF:
                ERRO("De acordo com a GLC, está faltando o token IF", ERRO_SINTATICO);
                break;
            case T_SMC:
                ERRO("De acordo com a GLC, está faltando o token SMC", ERRO_SINTATICO);
                break;
            case T_ID:
                ERRO("De acordo com a GLC, está faltando o token ID", ERRO_SINTATICO);
                break;
            case T_NUM:
                ERRO("De acordo com a GLC, está faltando o token NUM", ERRO_SINTATICO);
                break;
            case T_ATRIB:
                ERRO("De acordo com a GLC, está faltando o token ATRIB", ERRO_SINTATICO);
                break;
            case T_THEN:
                ERRO("De acordo com a GLC, está faltando o token THEN", ERRO_SINTATICO);
                break;
            case T_ELSE:
                ERRO("De acordo com a GLC, está faltando o token ELSE", ERRO_SINTATICO);
                break;
            case T_BEGIN:
                ERRO("De acordo com a GLC, está faltando o token BEGIN", ERRO_SINTATICO);
                break;
            case T_END:
                ERRO("De acordo com a GLC, está faltando o token END", ERRO_SINTATICO);
                break;
            default:
                ERRO("Token inesperado", ERRO_LEXICO);
                break;
        }
    }
}


int getToken()
{
    return yylex();
}

void ERRO(const char *mensagem, int tipoErro)
{
    printf("\nERRO TIPO %d: %s na linha %d\n", tipoErro, mensagem, linha);
    exit(tipoErro); // Interrompe a execução do programa
}

int main()
{
    tok = getToken();
    S();
    printf("\nAnalise finalizada\n");
    return 0;
}
