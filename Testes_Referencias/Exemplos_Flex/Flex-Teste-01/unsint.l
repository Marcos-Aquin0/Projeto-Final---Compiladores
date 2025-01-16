DIGITO [0-9]
LETRA [a-zA-Z]

%%
0x[0-9A-Fa-f]+          printf("%s HEX\n",yytext); ECHO;
[1-9]{DIGITO}*		printf("%s DEC\n",yytext);
0[0-7]*			printf("%s OCT\n",yytext);
0b[01]+			printf("%s BIN\n",yytext);
{LETRA}*		printf("%s LET\n",yytext);
#			return 0;
<<EOF>>                 printf("FIM DE ARQUIVO.../n"); return 0;
%%

int main(int argc, char *argv[])
{
  FILE *f_in, *f_out;
  f_out = fopen("teste_s","w");  
  yyout = f_out;

  if (argc == 2)
  {
    if(f_in = fopen(argv[1],"r"))  yyin = f_in;
    else  perror(argv[0]);
  }
  else  yyin = stdin;

  yylex();
  
  return(0);
  
}

