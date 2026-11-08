// Tokens
%token INT REAL
%token PLUS MINUS TIMES DIV MOD
%token  TRUE FALSE EQ NEQ LT LE GT GE AND OR XOR NOT 

// Operator associativity & precedence
%nonassoc EQ NEQ LT LE GT GE 
%left PLUS MINUS OR XOR
%left TIMES DIV MOD AND
%right NOT

// Root-level grammar symbol
%start program;

// Types/values in association to grammar symbols.
%union {
  int intValue;
  double realValue;
  Expr* exprValue; 
  BoolExpr* boolValue;
}

%type <intValue> INT
%type <realValue> REAL
%type <exprValue> expr
%type <boolValue> bexpr

// Use "%code requires" to make declarations go
// into both parser.c and parser.h
%code requires {
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex();
extern int yyline;
extern char* yytext;
extern FILE* yyin;
extern void yyerror(const char* msg);
extern Expr* root;
}

%%
program: expr { root = $1; }

expr: 
  INT { 
    $$ = ast_integer($1); 
  }
  | 
  REAL { 
    $$ = ast_real($1); 
  }
  |
  expr PLUS expr { 
    $$ = ast_operation(PLUS, $1, $3); 
  }
  |
  expr MINUS expr {
    $$ = ast_operation(MINUS, $1, $3);
  }
  |
  expr TIMES expr {
    $$ = ast_operation(TIMES, $1, $3);
  }
  |
  expr DIV expr {
    $$ = ast_operation(DIV, $1, $3);
  }
  |
  expr MOD expr {
    $$ = ast_operation(MOD, $1, $3);
  }

bexpr:
  TRUE {
    $$ = ast_boolean(1); 
  }
  |
  FALSE {
    $$ = ast_boolean(0);
  }
  |
  expr EQ expr {
    $$ = ast_compare(EQ, $1, $3);
  }
  |
  expr NEQ expr {
    $$ = ast_compare(NEQ, $1, $3);
  }
  |
  expr LT expr {
    $$ = ast_compare(LT, $1, $3);
  }
  | 
  expr LE expr {
    $$ = ast_compare(LE, $1, $3);
  } 
  |
  expr GT expr {
    $$ = ast_compare(GT, $1, $3);
  } 
  |
  expr GE expr {
    $$ = ast_compare(GE, $1, $3);
  }
  |
  bexpr AND bexpr {
    $$ = ast_gates(AND, $1, $3);
  }
  |
  bexpr OR bexpr {
    $$ = ast_gates(OR, $1, $3);
  }
  |
  bexpr XOR bexpr {
    $$ = ast_gates(XOR, $1, $3);
  }
  |
  NOT bexpr {
    $$ = ast_inner($2);
  }
  ;

%%

void yyerror(const char* err) {
  printf("Line %d: %s - '%s'\n", yyline, err, yytext  );
}

