#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "parser.h"

Cmd* root = NULL;


Value make_integer(int n){
	Value v;
	v.isReal=0;
	v.value.integer=n;
	return v;
}

Value make_real(double n){
	Value v;
	v.isReal=1;
	v.value.real=n;
	return v;
}

#define MAX_VARS 100 //value
static char* varNames[MAX_VARS];
static Value varValues[MAX_VARS];      // was: int
static int   nVars = 0;

static void storeVar(const char* name, Value value) {   // was: int value
  for (int i = 0; i < nVars; i++) {
    if (strcmp(varNames[i], name) == 0) {
      varValues[i] = value;
      return;
    }
  }
  if (nVars >= MAX_VARS) {
    yyerror("Too many variables!");
    return;
}
  varNames[nVars] = strdup(name);
  varValues[nVars] = value;
  nVars++;
}

static Value getVar(const char* name) {
    for (int i = 0; i < nVars; i++) {
        if (strcmp(varNames[i], name) == 0) {
            return varValues[i];
        }
    }

    yyerror("Variable not found!");
    return make_integer(0);
}

Value eval(Expr* expr) {
  Value result = make_integer(0);
  if (expr == 0) {
    yyerror("Null expression!!");
  }
  else if (expr->kind == E_INTEGER) {
    result = make_integer(expr->attr.value); 
  } 
  else if (expr->kind == E_REAL) {
    result = make_real(expr->attr.real); 
  } 
  else if (expr->kind==E_VARIABLE){
	  result=getVar(expr->attr.variable);
  }
  else if (expr->kind == E_UNARY) {
	    Value operand = eval(expr->attr.unary.operand);

	    switch (expr->attr.unary.operator) {

		case PLUS:
		    result = operand;
		    break;

		case MINUS:
		    if (operand.isReal) {
			result = make_real(-operand.value.real);
		    }
		    else {
			result = make_integer(-operand.value.integer);
		    }
		    break;

		default:
		    yyerror("Unknown unary operator!");
	    }
	}
  else if (expr->kind == E_OPERATION) {
    Value vLeft = eval(expr->attr.op.left);
    Value vRight = eval(expr->attr.op.right);

    int op=expr->attr.op.operator;


    if(op==MOD){
	    if(vLeft.isReal||vRight.isReal){
		    yyerror("MOD requires integer operands!");
		    return make_integer(0);
	    }
	    //nao tamos a chekar divs e mods por 0, acho q o fpc n faz isso (99% certeza)
	    return make_integer(vLeft.value.integer%vRight.value.integer);
    }
    if(op==DIV){
	    double a=vLeft.isReal ? vLeft.value.real : vLeft.value.integer;
	    double b=vRight.isReal ? vRight.value.real : vRight.value.integer;
	    //nao chekar div por 0 denovo
	    return make_real(a/b);
    }
    if(op==DIVI){
	    if(vLeft.isReal||vRight.isReal){
		    yyerror("DIV requires integer operands!");
		    return make_integer(0);
	    }
	    //nao tamos a chekar divs e mods por 0, acho q o fpc n faz isso (99% certeza)
	    return make_integer(vLeft.value.integer/vRight.value.integer);
    }
    if(vLeft.isReal||vRight.isReal){
	    double a=vLeft.isReal ? vLeft.value.real : vLeft.value.integer;
	    double b=vRight.isReal ? vRight.value.real : vRight.value.integer;
	    switch(op){
	      case PLUS: //ja veio com
          result = make_real(a + b); 
          break;
	      case MINUS: //Até mod é novo
          result = make_real(a - b);
          break;
	      case TIMES:
          result = make_real(a * b);
          break;
	      default: yyerror("Unknown operator!");
	    }
      return result;
    }
    int a=vLeft.value.integer;
    int b=vRight.value.integer;

    switch(op){
      case PLUS: //ja veio com
	result = make_integer(a + b); 
	break;
      case MINUS: //Até mod é novo
	result = make_integer(a - b);
	break;
      case TIMES:
	result = make_integer(a * b);
	break;
      default: yyerror("Unknown operator!");
    }
  }
  //yyerror("Unknown expression type!");
  return result;
}


int beval(BoolExpr* bexpr) {
  int result = 0;
  if (bexpr == 0){
    yyerror("Null expression!!");
  }
  else if (bexpr->kind == BE_BOOLEAN) {
    result = bexpr->attr.value;
  }
  else if (bexpr->kind == BE_COMPARE) {
    Value vLeft = eval(bexpr->attr.comp.left); //numeros estao expr
    Value vRight = eval(bexpr->attr.comp.right);

    int op =bexpr->attr.comp.operator;

    if(!vLeft.isReal&&!vRight.isReal){
	    int a=vLeft.value.integer;
	    int b=vRight.value.integer;
      switch(op){
        case EQ:
          result = (a == b); 
          break;
        case NEQ:
          result = (a != b);
          break;
        case LT:
          result = (a < b);
          break;
        case LE:
          result = (a <= b);
          break;
        case GT:
          result = (a > b);
          break;
        case GE:
          result = (a >= b);
          break;
        default: yyerror("Unknown operator!");
      }
    }
    else {
	    double a=vLeft.isReal ? vLeft.value.real : vLeft.value.integer;
	    double b=vRight.isReal ? vRight.value.real : vRight.value.integer;

		  switch(op){
	      case EQ:
		      result = (a == b); 
		      break;
	      case NEQ:
		      result = (a != b);
          break;
	      case LT:
          result = (a < b);
          break;
	      case LE:
          result = (a <= b);
          break;
	      case GT:
          result = (a > b);
          break;
	      case GE:
          result = (a >= b);
          break;
	      default: yyerror("Unknown operator!");
	    }
    }
  }
  else if (bexpr->kind == BE_GATES) {
    int vLeft = beval(bexpr->attr.gate.left);
    int vRight = beval(bexpr->attr.gate.right);
    switch (bexpr->attr.gate.operator) {
      case AND:
        result = (vLeft && vRight); 
        break;
      case OR:
        result = (vLeft || vRight); 
        break;
      case XOR:
        result = (vLeft ^ vRight); 
        break;
      default: yyerror("Unknown operator!");
    }
  }
  else if (bexpr->kind == BE_INNER) {
    result = !beval(bexpr->attr.inner);
  }
  return result;
}

int ceval(Cmd* cmd) {
  if (cmd->kind == C_ASSIGN) {
    storeVar(cmd->attr.assign.var, eval(cmd->attr.assign.value));
  }
  else if (cmd->kind == C_IFTHEN) {
    if (beval(cmd->attr.ifthen.cond)) ceval(cmd->attr.ifthen.thenCmd);
  }
  else if (cmd->kind == C_IFELSE) {
    if (beval(cmd->attr.ifelse.cond)) ceval(cmd->attr.ifelse.thenCmd);
    else ceval(cmd->attr.ifelse.elseCmd);
  }
  else if (cmd->kind == C_WHILE) {
    while (beval(cmd->attr.loop.cond)) ceval(cmd->attr.loop.thenCmd);
  }
  else if (cmd->kind == C_COMMANDS) {
    ceval(cmd->attr.commands.left);
    ceval(cmd->attr.commands.right);
  }
  else if (cmd->kind == C_DECL) {
    if (cmd->attr.decl.type == INTEGER_TYPE) {
        storeVar(cmd->attr.decl.var, make_integer(0));
    }
    else if (cmd->attr.decl.type == REAL_TYPE) {
        storeVar(cmd->attr.decl.var, make_real(0.0));
    }
   }
  return 0;
}

int main(int argc, char** argv) {
  --argc; ++argv;
  if (argc != 0) {
    yyin = fopen(*argv, "r");
    if (!yyin) {
      printf("'%s': could not open file\n", *argv);
      return 1;
    }
  } //  yyin = stdin
  if (yyparse() == 0) {
    printCmd(root);
    ceval(root);
  }
  return 0;
}
