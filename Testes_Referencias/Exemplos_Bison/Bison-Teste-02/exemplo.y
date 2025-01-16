%token NUM

%%

entrada:	exp
	;
exp	:	exp '+' termo 
	|	exp '-' termo
	|	termo 
	;
termo	:	termo '*' fator
	|	fator	
	;
fator	:	'('exp')'
	|	NUM 
	;

