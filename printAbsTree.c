#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.h"
 
#define PFX 1024   // max length of the indentation prefix
 
// Prints one tree line and builds the prefix that its children must use.
// last: 0 = has later siblings, 1 = last child, -1 = root (no connector).
static void emit(const char* prefix, int last, const char* label, char* childPrefix) {
  if (last < 0) {
    printf("%s\n", label);
    childPrefix[0] = '\0';
    return;
  }
  printf("%s%s%s\n", prefix, last ? "`-- " : "|-- ", label);
  snprintf(childPrefix, PFX, "%s%s", prefix, last ? "    " : "|   ");
}
 
static const char* opName(int op) {
  switch (op) {
    case PLUS:  return "ADD (+)";
    case MINUS: return "SUB (-)";
    case TIMES: return "MULT (*)";
    case DIV:   return "DIV (/)";
    case DIVI:  return "DIVI (div)";
    case MOD:   return "MOD (mod)";
    case EQ:    return "EQ (=)";
    case NEQ:   return "NEQ (<>)";
    case LT:    return "LT (<)";
    case LE:    return "LE (<=)";
    case GT:    return "GT (>)";
    case GE:    return "GE (>=)";
    case AND:   return "AND";
    case OR:    return "OR";
    case XOR:   return "XOR";
    default:    return "Unknown operator!";
  }
}
 
// ---------- expressions ----------
 
static void printExprAux(Expr* e, const char* prefix, int last) {
  char child[PFX];
  char label[256];
 
  if (e == 0) {
    yyerror("Null expression!!");
    return;
  }
 
  switch (e->kind) {
    case E_INTEGER:
      snprintf(label, sizeof label, "%d", e->attr.value);
      emit(prefix, last, label, child);
      break;
 
    case E_REAL:
      snprintf(label, sizeof label, "%g", e->attr.real);
      // keep reals visibly different from integers (2 -> 2.0)
      if (!strpbrk(label, ".eEn")) strncat(label, ".0", sizeof label - strlen(label) - 1);
      emit(prefix, last, label, child);
      break;
 
    case E_VARIABLE:
      emit(prefix, last, e->attr.variable, child);
      break;
 
    case E_UNARY:
      emit(prefix, last, e->attr.unary.operator == MINUS ? "NEG (-)" : "POS (+)", child);
      printExprAux(e->attr.unary.operand, child, 1);
      break;
 
    case E_OPERATION:
      emit(prefix, last, opName(e->attr.op.operator), child);
      printExprAux(e->attr.op.left, child, 0);
      printExprAux(e->attr.op.right, child, 1);
      break;
  }
}
 
// ---------- boolean expressions ----------
 
static void printBoolExprAux(BoolExpr* b, const char* prefix, int last) {
  char child[PFX];
 
  if (b == 0) {
    yyerror("Null expression!!");
    return;
  }
 
  switch (b->kind) {
    case BE_BOOLEAN:
      emit(prefix, last, b->attr.value ? "TRUE" : "FALSE", child);
      break;
 
    case BE_COMPARE:
      emit(prefix, last, opName(b->attr.comp.operator), child);
      printExprAux(b->attr.comp.left, child, 0);
      printExprAux(b->attr.comp.right, child, 1);
      break;
 
    case BE_GATES:
      emit(prefix, last, opName(b->attr.gate.operator), child);
      printBoolExprAux(b->attr.gate.left, child, 0);
      printBoolExprAux(b->attr.gate.right, child, 1);
      break;
 
    case BE_INNER:
      emit(prefix, last, "NOT", child);
      printBoolExprAux(b->attr.inner, child, 1);
      break;
  }
}
 
// ---------- commands ----------
 
static int countFlat(Cmd* c) {
  if (c && c->kind == C_COMMANDS)
    return countFlat(c->attr.commands.left) + countFlat(c->attr.commands.right);
  return 1;
}
 
static void collectFlat(Cmd* c, Cmd** list, int* n) {
  if (c && c->kind == C_COMMANDS) {
    collectFlat(c->attr.commands.left, list, n);
    collectFlat(c->attr.commands.right, list, n);
  } else {
    list[(*n)++] = c;
  }
}
 
static void printCmdAux(Cmd* c, const char* prefix, int last);
 
static void declLabel(Cmd* c, char* label, size_t size) {
  snprintf(label, size, "%s : %s", c->attr.decl.var,
           c->attr.decl.type == INTEGER_TYPE ? "INTEGER" : "REAL");
}
 
// Prints a node named `name` whose children are the flattened statements of c.
static void printBlock(Cmd* c, const char* name, const char* prefix, int last) {
  char child[PFX];
  int total = countFlat(c);
  Cmd** list = malloc(total * sizeof(Cmd*));
  int n = 0;
  collectFlat(c, list, &n);
 
  emit(prefix, last, name, child);
  for (int i = 0; i < n; i++)
    printCmdAux(list[i], child, i == n - 1);
  free(list);
}
 
// Body of THEN / ELSE / DO: a single command, or a BEGIN block.
static void printBody(Cmd* c, const char* prefix, int last) {
  if (c && c->kind == C_COMMANDS) printBlock(c, "BEGIN", prefix, last);
  else printCmdAux(c, prefix, last);
}
 
static void printCmdAux(Cmd* c, const char* prefix, int last) {
  char child[PFX];
  char label[256];
 
  if (c == 0) {
    yyerror("Null command!!");
    return;
  }
 
  switch (c->kind) {
    case C_ASSIGN:
      emit(prefix, last, "ASSIGN", child);
      {
        char child2[PFX];
        emit(child, 0, c->attr.assign.var, child2);
      }
      printExprAux(c->attr.assign.value, child, 1);
      break;
 
    case C_DECL:
      declLabel(c, label, sizeof label);
      emit(prefix, last, label, child);
      break;
 
    case C_IFTHEN:
      emit(prefix, last, "IF", child);
      printBoolExprAux(c->attr.ifthen.cond, child, 0);
      {
        char child2[PFX];
        emit(child, 1, "THEN", child2);
        printBody(c->attr.ifthen.thenCmd, child2, 1);
      }
      break;
 
    case C_IFELSE:
      emit(prefix, last, "IF", child);
      printBoolExprAux(c->attr.ifelse.cond, child, 0);
      {
        char child2[PFX];
        emit(child, 0, "THEN", child2);
        printBody(c->attr.ifelse.thenCmd, child2, 1);
        emit(child, 1, "ELSE", child2);
        printBody(c->attr.ifelse.elseCmd, child2, 1);
      }
      break;
 
    case C_WHILE:
      emit(prefix, last, "WHILE", child);
      printBoolExprAux(c->attr.loop.cond, child, 0);
      {
        char child2[PFX];
        emit(child, 1, "DO", child2);
        printBody(c->attr.loop.thenCmd, child2, 1);
      }
      break;
 
    case C_COMMANDS:
      printBlock(c, "BEGIN", prefix, last);
      break;
 
    case C_WRITE:
      emit(prefix, last, "WRITE", child);
      printExprAux(c->attr.write.value, child, 1);
      break;
  }
}
 
// ---------- public entry points ----------
 
// Prints the whole program: declarations (leading C_DECL nodes) and body.
void printCmd(Cmd* c) {
  char child[PFX];
  char label[256];
 
  if (c == 0) {
    yyerror("Null command!!");
    return;
  }
 
  int total = countFlat(c);
  Cmd** list = malloc(total * sizeof(Cmd*));
  int n = 0;
  collectFlat(c, list, &n);
 
  int nDecl = 0;
  while (nDecl < n && list[nDecl]->kind == C_DECL) nDecl++;
  int nBody = n - nDecl;
 
  emit("", -1, "PROGRAM", child);
 
  if (nDecl > 0) {
    char child2[PFX];
    emit(child, nBody == 0, "DECLARATIONS", child2);
    for (int i = 0; i < nDecl; i++) {
      char child3[PFX];
      declLabel(list[i], label, sizeof label);
      emit(child2, i == nDecl - 1, label, child3);
    }
  }
 
  if (nBody > 0) {
    char child2[PFX];
    emit(child, 1, "BEGIN", child2);
    for (int i = nDecl; i < n; i++)
      printCmdAux(list[i], child2, i == n - 1);
  }
 
  free(list);
}
 
void printBoolExpr(BoolExpr* b) {
  printBoolExprAux(b, "", -1);
}
 
void printExpr(Expr* e) {
  printExprAux(e, "", -1);
}
