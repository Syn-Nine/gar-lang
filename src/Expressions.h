#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "Token.h"
#include <string>

class Environment;

class Expr
{
public:
    static IRCode AsExpr(IRCode lhs, Token oper);
    static IRCode AssignExpr(Token id, IRCode rhs, Environment* env, bool allow_const = false);
    static IRCode BinaryExpr(IRCode lhs, Token oper, IRCode rhs);
    static IRCode CallExpr(IRCode params, Token callee);
    static IRCode ListExpr(IRCode params, Token count);
    static IRCode LiteralExpr(Token prev, Environment* env);
    static IRCode LiteralExpr(Token prev, std::string value, Environment* env);
    static IRCode UnaryExpr(Token oper, IRCode rhs);
    static IRCode VariableExpr(Token id, Environment* env);

private:

};


#endif // EXPRESSIONS_H