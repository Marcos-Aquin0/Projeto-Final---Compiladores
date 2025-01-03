%{
  int num_lines = 0;
  int num_chars = 0;
%}

%%
\n	++num_lines; ++num_chars;
. 	++num_chars;
fim	return 0;
%%

void main()
{
 yylex();
 printf("# of lines = %d, # of chars = %d\n", num_lines, num_chars);
}
