
// AST definitions
#ifndef __ast_h__
#define __ast_h__

// AST for expressions
struct _Expr {
  enum { 
    E_INTEGER,
    E_REAL,
    E_OPERATION,
    E_VARIABLE
  } kind;
  union { //separates between op and value
    int value; // for integer values
    float real; //  for   real  values

    struct { 
      int operator; // PLUS, MINUS, etc 
      struct _Expr* left;
      struct _Expr* right;
    } op; // for binary expressions
	
    char* variable;
  } attr;
};

// AST for boolean
struct _BoolExpr {
  enum {
    BE_BOOLEAN, //True, False
    BE_COMPARE, //EQ, NEQ, LT, GT, LE, GE
    BE_GATES, //And, Or
    BE_INNER //NOT
  } kind;
  union {
    int value; // 1=True, 0=False
    struct {
      int operator; //EQ, NEQ, LT...
      struct _Expr* left;
      struct _Expr* right;
    } comp;
    struct {
      int operator; // And, or
      struct _BoolExpr* left;
      struct _BoolExpr* right;
    } gate;
    struct _BoolExpr* inner; // Not
  } attr;
};

struct _Cmd {
  enum {
    C_ASSIGN,
    C_DECL,
    C_IFTHEN,
    C_IFELSE,
    C_WHILE,
    C_COMMANDS
  } kind;
  union {
    struct {
      char* var;
      struct _Expr* value; //can be 1+1
    } assign;
    struct {
	    int type;
	    char* var;
	} decl;
    struct {
      struct _BoolExpr* cond;
      struct _Cmd* thenCmd;
    } ifthen;
    struct {
      struct _BoolExpr* cond;
      struct _Cmd* thenCmd;
      struct _Cmd* elseCmd;
    } ifelse;
    struct {
      struct _BoolExpr* cond;
      struct _Cmd* thenCmd;
    } loop; //can be used in a for
    struct {
      struct _Cmd* left;
      struct _Cmd* right;
    } commands;
  } attr;
};

typedef struct _Cmd Cmd;
typedef struct _BoolExpr BoolExpr;
typedef struct _Expr Expr; // Convenience typedef (?)


typedef struct {
    int isReal;

    union {
        int integer;
        double real;
    } value;
} Value;


Value eval(Expr* expr);

// Constructor functions (see implementation in ast.c)
Expr* ast_integer(int v);
Expr* ast_real(double v);
Expr* ast_operation(int operator, Expr* left, Expr* right);
Expr* ast_variable(char* name);

// AQUIIIIIIIIIIIIII Constructor functions (see implementation in ast.c)
BoolExpr* ast_boolean(int v);
BoolExpr* ast_compare(int operator, Expr* left, Expr* right);
BoolExpr* ast_gates(int operator, BoolExpr* left, BoolExpr* right);
BoolExpr* ast_inner(BoolExpr* inner);

Cmd* ast_assign(char* var, Expr* value);
Cmd* ast_declare(int type,char* var);
Cmd* ast_ifthen(BoolExpr* cond, Cmd* thenCmd);
Cmd* ast_ifelse(BoolExpr* cond, Cmd* thenCmd, Cmd* elseCmd);
Cmd* ast_while(BoolExpr* cond, Cmd* thenCmd);
Cmd* ast_commands(Cmd* left, Cmd* right);

void printExpr(Expr* e);
void printBoolExpr(BoolExpr* b);
void printCmd(Cmd* c);
#endif
