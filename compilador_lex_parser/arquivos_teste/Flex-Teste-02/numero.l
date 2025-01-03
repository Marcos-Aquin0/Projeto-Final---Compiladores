%{
/* Um programa Flex para contar entrada de números */

#include <stdlib.h>
#include <stdio.h>

#define NUM   300
#define ID    301
#define OUTRO 302
#define FIM   303

%}

digito [0-9]
letra [a-zA-Z]
numero {digito}+
identificador {letra}({letra}|{digito})*

%%

{numero}  	 {return NUM;}
{identificador}  {return ID;}
<<EOF>>          {return FIM;}
\n		 {  }
%%

int main()
{
  int token, cont = 0;
  FILE *f_in;

  f_in = fopen("teste","r");
  yyin = f_in;
  
  while ((token = yylex()) != FIM)
  {
    if (token == NUM) cont++;
  }
  printf("\nQtd. de números na entrada: %d \n", cont);
  return(0);
}
