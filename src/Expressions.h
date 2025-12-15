#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "Token.h"
#include <string>

class Environment;

class Expr
{
public:
    static Bytecode LiteralExpr(Token prev, int value);
    static Bytecode LiteralExpr(Token prev, double value);

private:

};


#endif // EXPRESSIONS_H