// Tokens
%token INT REAL
%token PLUS MINUS TIMES DIV DIVI MOD
%token TRUE FALSE EQ NEQ LT LE GT GE AND OR XOR NOT
%token ASSIGN IF THEN ELSE WHILE DO P_BEGIN P_END SEMICOLON
%token <strValue> ID

%token INTEGER_TYPE REAL_TYPE

// Operator associativity & precedence
%left SEMICOLON
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left OR XOR
%left AND
%right NOT
%nonassoc EQ NEQ LT LE GT GE 
%left PLUS MINUS
%left TIMES DIV DIVI MOD

%token LPAREN RPAREN

// Root-level grammar symbol
%start program;

// Types/values in association to grammar symbols.
%union {
  int intValue;
  double realValue;
  Expr* exprValue; 
  BoolExpr* boolValue;
  Cmd* cmdValue;
  char* strValue;
}

%type <intValue> INT
%type <realValue> REAL
%type <exprValue> expr
%type <boolValue> bexpr
%type <cmdValue> cmd

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
extern Cmd* root;
}

%%
program: cmd { root = $1; }

expr: 
  INT { 
    $$ = ast_integer($1); 
  }
  | 
  REAL { 
    $$ = ast_real($1); 
  }
  |
 LPAREN expr RPAREN {
        $$ = $2;
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
  expr DIVI expr {
    $$ = ast_operation(DIVI, $1, $3);
  }
  |
  expr MOD expr {
    $$ = ast_operation(MOD, $1, $3);
  }
  | 
  ID {
    $$ = ast_variable($1);
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
  
cmd:
    INTEGER_TYPE ID {
        $$ = ast_declare(INTEGER_TYPE, $2);
    }
    |
    REAL_TYPE ID {
        $$ = ast_declare(REAL_TYPE, $2);
    }
    |
  ID ASSIGN expr {
    $$ = ast_assign($1, $3);
  }
  |
  IF bexpr THEN cmd %prec LOWER_THAN_ELSE { //then has no precedence, so that gives it 
    $$ = ast_ifthen($2, $4);                    //LOWER... precedence
  }
  |
  IF bexpr THEN cmd ELSE cmd {
    $$ = ast_ifelse($2, $4, $6);
  }
  |
  WHILE bexpr DO cmd %prec LOWER_THAN_ELSE{
    $$ = ast_while($2,$4);
  }
  |
  P_BEGIN cmds P_END {
    $$ = $2;
  };

cmds:
  cmd {
	$$=$1;
	}
|
  cmd SEMICOLON cmd {
    $$ = ast_commands($1, $3);
  }
|
	cmd SEMICOLON {
	  $$=$1;
	}
;

%%

void yyerror(const char* err) {
  printf("Line %d: %s - '%s'\n", yyline, err, yytext  );
}

