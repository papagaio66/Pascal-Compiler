%{
// HEADERS
#include <stdlib.h>
#include "parser.h"
#include <string.h>

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
[ \t\r]+ {  }

"{"[^}]*"}" {
  for (char* p = yytext; *p; p++)
    if (*p == '\n') yyline++;
}

"(*"([^*]|\*+[^*)])*\*+")" {
  for (char* p = yytext; *p; p++)
    if (*p == '\n') yyline++;
}
\n { yyline++; }



program { return PROGRAM; }
var     { return VAR; }

":" { return COLON; }
"." { return DOT; }


{real} {
	yylval.realValue=atof(yytext);
	return REAL;
}


{digit}+ { 
   yylval.intValue = atoi(yytext);
   return INT; 
}

"+" { return PLUS; }
"-" { return MINUS; }
"*" { return TIMES; }
"/" { return DIV; }
[mM][oO][dD] { return MOD; }
[dD][iI][vV] { return DIVI; }

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

"(" { return LPAREN; }
")" { return RPAREN; }

if { return IF; }
then { return THEN; }
else { return ELSE; }
while { return WHILE;}
do { return DO; }
begin { return P_BEGIN; }
end { return P_END; }

integer { return INTEGER_TYPE; }
real    { return REAL_TYPE; }


":=" { return ASSIGN; }
";" { return SEMICOLON; }



write { return WRITE; }



{digit}+({alpha}({alpha}|{digit})*) {
	yyerror("unexpected character");
}

{alpha}({alpha}|{digit})* { 
   yylval.strValue = strdup(yytext); 
   return ID; 
   }

.  { yyerror("unexpected character"); }
%%
