%{
// HEADERS
#include <stdlib.h>
#include "parser.h"

// variables maintained by the lexical analyser
int yyline = 1;
%}

%option noyywrap
%option caseless

alpha [_a-zA-Z]
digit [0-9]

digits {digit}+
expon [Ee][+-]?{digits}

real ({digits}"."{digits}({expon})?|{digits}{expon})

%%
[ \t]+ {  }
"#"[^\n]* {}
\n { yyline++; }


{real} {
	yylval.realValue=atof(yytext);
	real REAL;
}


{digit}+ { 
   yylval.intValue = atoi(yytext);
   return INT; 
}

"+" { return PLUS; }
"-" { return MINUS; }
"*" { return TIMES; }
"/" { return DIV; }
"%" { return MOD; }

true { return TRUE; }
false { return FALSE; }

"=" { return EQ; }
"<>" { return NEQ; }
"<" { return LT; }
"<=" { return LE; }
">" { return GT; }
">=" { return GE; }

and { return AND; }
or { return OR; }
not { return NOT; }
xor { return XOR; }

   /*
   "and then" { return ANDTHEN; }
   "or else" { return ORELSE; }
   */

.  { yyerror("unexpected character"); }
%%

