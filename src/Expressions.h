#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "Token.h"
#include <string>

class Environment;

class Expr
{
public:
    static IRCode AsExpr(IRCode lhs, Token oper);
    static IRCode AssignExpr(Token id, IRCode rhs, Environment* env);
    static IRCode BinaryExpr(IRCode lhs, Token oper, IRCode rhs);
    static IRCode CallExpr(IRCode params, Token callee);
    static IRCode LiteralExpr(Token prev);
    static IRCode LiteralExpr(Token prev, std::string value, Environment* env);
    static IRCode LogicalExpr(IRCode lhs, Token oper, IRCode rhs);
    static IRCode UnaryExpr(Token oper, IRCode rhs);
    static IRCode VariableExpr(Token id, Environment* env);

private:

};


#endif // EXPRESSIONS_H