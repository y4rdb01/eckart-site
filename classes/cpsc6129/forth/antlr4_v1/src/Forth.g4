grammar Forth;

expression	: valueOrOp*
		;

valueOrOp	: NUMBER
		| operator
		;

operator	: PLUS
		| MINUS
		| TIMES
		| DIVIDE
		| DOT
		;

PLUS		: '+'		;
MINUS		: '-'		;
TIMES		: '*'		;
DIVIDE		: '/'		;
DOT		: '.'		;
NUMBER		: ( [0] | ([1-9][0-9]*) );	// Unsigned integers
WHITE_SPACE	: [ \t\r\n]+	-> skip;	// Skip whitespace
