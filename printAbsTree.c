#include <stdio.h>
#include "ast.h"
#include "parser.h"

static void printIndent(int depth) {
  for (int i=0; i<depth; i++) printf(" ");
}

static const char* opName(int op) { //Claude
  switch (op) {
    case PLUS:  return "ADD";
    case MINUS: return "SUB";
    case TIMES: return "MULT";
    case DIV:   return "DIV";
    case MOD:   return "MOD";
    case EQ: return "EQ";
    case NEQ: return "NEQ";
    case LT: return "LT";
    case LE: return "LE";
    case GT: return "GT";
    case GE: return "GE";
    case AND: return "AND";
    case OR: return "OR";
    case XOR: return "XOR";
    default: return "Unknown operator!";
  }
}

static void printExprAux(Expr* expr, int depth) {
  if (expr == 0) {
    yyerror("Null expression!!");
  }
  else if (expr->kind == E_INTEGER) {
    printIndent(depth+1);
    printf("NUM\n");
    printIndent(depth+1);
    printf("|\n");
    printIndent(depth+1);
    printf("%d\n",expr->attr.value);
  }
  else if (expr->kind==E_REAK){
	  printIndent(depth+1);
	  printf("REAL\n");
	  printIndent(depth+1);
	  printf("|\n");
	  printIndent(depth+1);
	  printf("%g\n",expr->attr.value);
  }
  else if (expr->kind == E_OPERATION) { //claude
    printIndent(depth);
    printf("%s\n", opName(expr->attr.op.operator));
    printIndent(depth + 1);
    printf("|\n");
    printExprAux(expr->attr.op.left, depth + 1);
    printIndent(depth + 1);
    printf("|\n");
    printExprAux(expr->attr.op.right, depth + 1);
  }
  return;
}

static void printBoolExprAux(BoolExpr* bexpr, int depth) {
  if (bexpr == 0) {
    yyerror("Null expression!!");
  }
  else if (bexpr->kind == BE_BOOLEAN) {
    printIndent(depth+1);
    printf("BOOL\n");
    printIndent(depth+1);
    printf("|\n");
    printIndent(depth+1);
    printf("%s\n",bexpr->attr.value ? "TRUE" : "FALSE");
  }
  else if (bexpr->kind == BE_COMPARE) {
    printIndent(depth);
    printf("%s\n", opName(bexpr->attr.comp.operator));
    printIndent(depth + 1);
    printf("|\n");
    printExprAux(bexpr->attr.comp.left, depth + 1);
    printIndent(depth + 1);
    printf("|\n");
    printExprAux(bexpr->attr.comp.right, depth + 1);
  }
  else if (bexpr->kind == BE_GATES) {
    printIndent(depth);
    printf("%s\n", opName(bexpr->attr.gate.operator));
    printIndent(depth + 1);
    printf("|\n");
    printBoolExprAux(bexpr->attr.gate.left, depth + 1);
    printIndent(depth + 1);
    printf("|\n");
    printBoolExprAux(bexpr->attr.gate.right, depth + 1);
  }
  else if (bexpr->kind == BE_INNER) {
    printIndent(depth);
    printf("NOT\n");
    printIndent(depth + 1);
    printf("|\n");
    printBoolExprAux(bexpr->attr.inner, depth + 1);
  }
  return;
}

void printBoolExpr(BoolExpr* b) {
  printBoolExprAux(b, 0);
}
void printExpr(Expr* e) {
  printExprAux(e, 0);
}
