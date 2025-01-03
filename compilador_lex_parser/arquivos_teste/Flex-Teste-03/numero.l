%{
#include <stdlib.h>
#include <stdio.h>

#define DEC 300
#define OCT 301
#define HEX 302
#define BIN 303
#define FIM 304
%}
DIGIT [0-9]

%%
[1-9]{DIGIT}*		return DEC;
0[0-7]*			return OCT;
0x[0-9A-Fa-f]+		return HEX;
0b[01]+			return BIN;
<<EOF>>                 return FIM;
\n			{  }
%%

int main(int argc, char *argv[])
{
  int cdec=0, coct=0, chex=0, cbin=0, token;
  FILE *f_in;

  if (argc == 2)
  {
    if(f_in = fopen(argv[1],"r"))  yyin = f_in;
    else  perror(argv[0]);
  }
  else  yyin = stdin;
  
  while ((token=yylex())!=FIM)
  {
    switch(token)
    {
      case DEC : cdec++; break;
      case OCT : coct++; break;
      case HEX : chex++; break;
      case BIN : cbin++;
    } 
  }
  printf("DEC:%d  OCT:%d  HEX:%d  BIN:%d\n", cdec, coct, chex, cbin);
  return(0);
}

