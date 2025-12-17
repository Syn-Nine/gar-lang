#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "Token.h"
#include <string>

class Environment;

class Expr
{
public:
    static Bytecode AsExpr(Bytecode lhs, Token oper);
    static Bytecode AssignExpr(Token id, Bytecode rhs, Environment* env);
    static Bytecode BinaryExpr(Bytecode lhs, Token oper, Bytecode rhs);
    static Bytecode CallExpr(Bytecode params, Token callee);
    static Bytecode LiteralExpr(Token prev);
    static Bytecode LiteralExpr(Token prev, std::string value, Environment* env);
    static Bytecode LogicalExpr(Bytecode lhs, Token oper, Bytecode rhs);
    static Bytecode UnaryExpr(Token oper, Bytecode rhs);
    static Bytecode VariableExpr(Token id, Environment* env);

private:

};


#endif // EXPRESSIONS_H