// AST constructor functions

#include <string.h>
#include <stdlib.h> // for malloc
#include "ast.h" // AST header

Expr* ast_integer(int v) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_INTEGER;
  node->attr.value = v;
  return node;
}

Expr* ast_real(double v) {
    Expr* node = (Expr*) malloc(sizeof(Expr));
    node->kind = E_REAL;
    node->attr.real = v;
    return node;
}

Expr* ast_operation
(int operator, Expr* left, Expr* right) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_OPERATION;
  node->attr.op.operator = operator;
  node->attr.op.left = left;
  node->attr.op.right = right;
  return node;
}

Expr* ast_variable(char* name) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_VARIABLE;
  node->attr.variable = strdup(name);
  return node;
}

BoolExpr* ast_boolean(int v) {
  BoolExpr* node = (BoolExpr*) malloc(sizeof(BoolExpr));
  node->kind = BE_BOOLEAN;
  node->attr.value = v;
  return node;
}

BoolExpr* ast_compare
(int operator, Expr* left, Expr* right) {
  BoolExpr* node = (BoolExpr*) malloc(sizeof(BoolExpr));
  node->kind = BE_COMPARE;
  node->attr.comp.operator = operator;
  node->attr.comp.left = left;
  node->attr.comp.right = right;
  return node;
}

BoolExpr* ast_gates
(int operator, BoolExpr* left, BoolExpr* right) {
  BoolExpr* node = (BoolExpr*) malloc(sizeof(BoolExpr));
  node->kind = BE_GATES;
  node->attr.gate.operator = operator;
  node->attr.gate.left = left;
  node->attr.gate.right = right;
  return node;
}

BoolExpr* ast_inner
(BoolExpr* inner) {
  BoolExpr* node = (BoolExpr*) malloc(sizeof(BoolExpr));
  node->kind = BE_INNER;
  node->attr.inner = inner;
  return node;
}

Cmd* ast_assign
(char* var, Expr* value) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_ASSIGN;
  node->attr.assign.var = strdup(var); //alocates new memory so next scan doesnt change
  node->attr.assign.value = value;
  return node;
}
Cmd* ast_declare(int type, char* var) {
    Cmd* node = malloc(sizeof(Cmd));
    node->kind = C_DECL;
    node->attr.decl.type = type;
    node->attr.decl.var = strdup(var);
    return node;
}
Cmd* ast_ifthen
(BoolExpr* cond, Cmd* thenCmd) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_IFTHEN;
  node->attr.ifthen.cond = cond;
  node->attr.ifthen.thenCmd = thenCmd;
  return node;
}
Cmd* ast_ifelse
(BoolExpr* cond, Cmd* thenCmd, Cmd* elseCmd) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_IFELSE;
  node->attr.ifelse.cond = cond;
  node->attr.ifelse.thenCmd = thenCmd;
  node->attr.ifelse.elseCmd = elseCmd;
  return node;
}
Cmd* ast_while
(BoolExpr* cond, Cmd* thenCmd) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_WHILE;
  node->attr.loop.cond = cond;
  node->attr.loop.thenCmd = thenCmd;
  return node;
}
Cmd* ast_commands
(Cmd* left, Cmd* right) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_COMMANDS;
  node->attr.commands.left = left;
  node->attr.commands.right = right;
  return node;
}
