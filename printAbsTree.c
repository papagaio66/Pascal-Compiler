#include <stdio.h>
#include "ast.h"
#include "parser.h"

static void printIndent(int depth) {
  for (int i=0; i<depth; i++) printf("   ");
}

static const char* opName(int op) {
  switch (op) {
    case PLUS:  return "ADD";
    case MINUS: return "SUB";
    case TIMES: return "MULT";
    case DIV:   return "DIV";
    case DIVI: return "DIVI";
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
    printIndent(depth);
    printf("INTEGER\n");
    printIndent(depth+1);
    printf("|\n");
    printIndent(depth+1);
    printf("%d\n",expr->attr.value);
  }
  else if (expr->kind==E_REAL){
	  printIndent(depth);
	  printf("REAL\n");
	  printIndent(depth+1);
	  printf("|\n");
	  printIndent(depth+1);
	  printf("%g\n",expr->attr.real);
  }
  else if (expr->kind == E_UNARY) {
	    printIndent(depth);

	    if (expr->attr.unary.operator == MINUS)
		printf("UNARY MINUS\n");
	    else
		printf("UNARY PLUS\n");

	    printIndent(depth + 1);
	    printf("|\n");

	    printExprAux(expr->attr.unary.operand, depth + 1);
	}
  else if (expr->kind == E_OPERATION) {
    printIndent(depth);
    printf("%s\n", opName(expr->attr.op.operator));
    printIndent(depth + 1);
    printf("|\n");
    printExprAux(expr->attr.op.left, depth + 1);
    printIndent(depth + 1);
    printf("|\n");
    printExprAux(expr->attr.op.right, depth + 1);
  }
  else if (expr->kind == E_VARIABLE) {
    printIndent(depth);
    printf("VARIABLE\n");
    printIndent(depth+1);
    printf("|\n");
    printIndent(depth+1);
    printf("%s\n", expr->attr.variable);
  }
  return;
}


static void printBoolExprAux(BoolExpr* bexpr, int depth) {
  if (bexpr == 0) {
    yyerror("Null expression!!");
  }
  else if (bexpr->kind == BE_BOOLEAN) {
    printIndent(depth);
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

static void printCmdAux(Cmd* cmd, int depth) {
  if (cmd == 0) {
    yyerror("Null expression!!");
  }
  else if (cmd->kind==C_ASSIGN) {
    printIndent(depth);
    printf("ASSIGN\n");
    printIndent(depth + 1);
    printf("|\n");
    printIndent(depth + 1);
    printf("%s\n", cmd->attr.assign.var);
    printIndent(depth + 1);
    printf("|\n");
    printExprAux(cmd->attr.assign.value, depth + 1);
  }
  else if (cmd->kind == C_DECL) {
    printIndent(depth);
    printf("DECLARATION\n");
    printIndent(depth + 1);
    printf("|\n");
    printIndent(depth + 1);
    printf("%s\n", cmd->attr.decl.type == INTEGER_TYPE ? "INTEGER" : "REAL");
    printIndent(depth + 1);
    printf("|\n");
    printIndent(depth + 1);
    printf("%s\n", cmd->attr.decl.var);
  }
  else if (cmd->kind == C_IFTHEN) {
    printIndent(depth);
    printf("IFTHEN\n");
    printIndent(depth + 1);
    printf("|\n");
    printBoolExprAux(cmd->attr.ifthen.cond, depth + 1);
    printIndent(depth + 1);
    printf("|\n");
    printCmdAux(cmd->attr.ifthen.thenCmd, depth + 1);
  }
  else if (cmd->kind == C_IFELSE) {
    printIndent(depth);
    printf("IFTHENELSE\n");
    printIndent(depth + 1);
    printf("|\n");
    printBoolExprAux(cmd->attr.ifelse.cond, depth + 1);
    printIndent(depth + 1);
    printf("|\n");
    printCmdAux(cmd->attr.ifelse.thenCmd, depth + 1);
    printIndent(depth + 1);
    printf("|\n");
    printCmdAux(cmd->attr.ifelse.elseCmd, depth + 1);
  }
  else if (cmd->kind == C_WHILE) {
    printIndent(depth);
    printf("WHILE\n");
    printIndent(depth + 1);
    printf("|\n");
    printBoolExprAux(cmd->attr.loop.cond, depth + 1);
    printIndent(depth + 1);
    printf("|\n");
    printCmdAux(cmd->attr.loop.thenCmd, depth + 1);
  }
  else if (cmd->kind == C_COMMANDS) {
    printIndent(depth);
    printf("COMPOUND\n");
    printIndent(depth + 1);
    printf("|\n");
    printCmdAux(cmd->attr.commands.left, depth + 1);
    printIndent(depth + 1);
    printf("|\n");
    printCmdAux(cmd->attr.commands.right, depth + 1);
  }
  else if (cmd->kind == C_WRITE) {
    printIndent(depth);
    printf("WRITE\n");
    printIndent(depth + 1);
    printf("|\n");
    printExprAux(cmd->attr.write.value, depth + 1);
}
else if (cmd->kind == C_READ) {
    printIndent(depth);
    printf("READ\n");

    printIndent(depth + 1);
    printf("|\n");

    printIndent(depth + 1);
    printf("%s\n", cmd->attr.read.var);
}
  return;
}

void printCmd(Cmd* c) {
  printCmdAux(c, 0);
}
void printBoolExpr(BoolExpr* b) {
  printBoolExprAux(b, 0);
}
void printExpr(Expr* e) {
  printExprAux(e, 0);
}
