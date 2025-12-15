#include "Expressions.h"
#include "Environment.h"


Bytecode Expr::LiteralExpr(Token prev, int value)
{
    Bytecode ret;
    ret.push_back(Token(TOKEN_LOAD_INT, prev.Line(), prev.Filename()));
    ret.push_back(prev);
    PushLn(ret);
    return ret;
}

Bytecode Expr::LiteralExpr(Token prev, double value)
{
    Bytecode ret;
    ret.push_back(Token(TOKEN_LOAD_FLOAT, prev.Line(), prev.Filename()));
    ret.push_back(prev);
    PushLn(ret);
    return ret;
}

