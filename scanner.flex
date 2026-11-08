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

if { return IF; }
then { return THEN; }
else { return ELSE; }
while { return WHILE;}
do { return DO; }
begin { return P_BEGIN; }
end { return P_END; }

":=" { return ASSIGN; }
";" { return SEMICOLON; }

{alpha}({alpha}|{digit})* { 
   yylval.strValue = strdup(yytext); 
   return ID; 
   }

.  { yyerror("unexpected character"); }
%%
