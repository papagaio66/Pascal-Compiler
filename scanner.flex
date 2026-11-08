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

%%
[ \t]+ {  }
"#"[^\n]* {}
\n { yyline++; }

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

