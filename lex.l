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

%%


{ELSE}          printf("ELSE");
{IF}            printf("IF");
{INT}           printf("INT");
{RETURN}        printf("RETURN"); 
{VOID}          printf("VOID");
{WHILE}         printf("WHILE"); 
{DIGIT}{DIGIT}*		                        printf("INT");
{DIGIT}{DIGIT}*{POINT}{DIGIT}{DIGIT}*		printf("FLOAT");
{DIV}{MULT}([^*]|{MULT}[^/])*{MULT}{DIV}	printf("COMMENT");
{LETTER}{LETTER}* 	printf("ID");
{OPAREN}		printf("OPAREN");
{CPAREN}		printf("CPAREN");
{OKEYS}			printf("OKEYS");
{CKEYS}			printf("CKEYS");
{OBRACKT}		printf("OBRACKT");
{CBRACKT}		printf("CBRACKT");
{SUM}			printf("SUM");
{SUB}			printf("SUB");
{MULT}			printf("MULT");
{DIV}			printf("DIV");
{BT}			printf("BT");
{BTE}			printf("BTE");
{LT}			printf("LT");
{LTE}			printf("LTE");
{EQ}			printf("EQ");
{SMC}			printf("SMC");
{COMMA}			printf("COMMA");
{ATRIB}			printf("ATRIB");
{NQ}			printf("NQ");
<<EOF>>			return 0;

%%

int main(int argc, char *argv[]) {
    FILE *f_in;
    if (argc == 2) {
        if (f_in = fopen(argv[1], "r")) {
            yyin = f_in;
        } else {
            perror(argv[0]);
        }
    } else {
        yyin = stdin;
    }
    yylex();
    return 0;
}
