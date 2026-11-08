#include <stdio.h>
#include "ast.h"
#include "parser.h"

Expr* root = NULL;

int eval(Expr* expr) {
  int result = 0;
  if (expr == 0) {
    yyerror("Null expression!!");
  }
  else if (expr->kind == E_INTEGER) {
    result = expr->attr.value; 
  } 
  else if (expr->kind == E_OPERATION) {
    int vLeft = eval(expr->attr.op.left);
    int vRight = eval(expr->attr.op.right);
    switch (expr->attr.op.operator) {
      case PLUS: //ja veio com
        result = vLeft + vRight; 
        break;
      case MINUS: //Até mod é novo
        result = vLeft - vRight;
        break;
      case TIMES:
        result = vLeft * vRight;
        break;
      case DIV:
        result = vLeft / vRight;
        break;
      case MOD:
        result = vLeft % vRight;
        break;
      default: yyerror("Unknown operator!");
    }
  }
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
    int vLeft = eval(bexpr->attr.comp.left); //numeros entao expr
    int vRight = eval(bexpr->attr.comp.right);
    switch (bexpr->attr.comp.operator) {
      case EQ:
        result = (vLeft == vRight); 
        break;
      case NEQ:
        result = (vLeft != vRight);
        break;
      case LT:
        result = (vLeft < vRight);
        break;
      case LE:
        result = (vLeft <= vRight);
        break;
      case GT:
        result = (vLeft > vRight);
        break;
      case GE:
        result = (vLeft >= vRight);
        break;
      default: yyerror("Unknown operator!");
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
      printExpr(root);
      printf("Result = %d\n", eval(root));
  }
  return 0;
}
