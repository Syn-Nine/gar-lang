#include "Expressions.h"
#include "Environment.h"


Bytecode Expr::AssignExpr(Token id, Bytecode rhs, Environment* env)
{
    Bytecode ret = rhs;
    ret.push_back(Token(TOKEN_STORE_VAR, id.Line(), id.Filename()));
    size_t addr = env->GetVariable(id);
    ret.push_back(Token(TOKEN_INTEGER, std::to_string(addr), addr, addr, id.Line(), id.Filename()));
    PushLn(ret);
    return ret;
}


Bytecode Expr::BinaryExpr(Bytecode lhs, Token oper, Bytecode rhs)
{
    Bytecode ret = lhs;
    Append(ret, rhs);
    Push(ret, oper);
    PushLn(ret);
    return ret;
}


Bytecode Expr::LiteralExpr(Token prev, bool value)
{
    Bytecode ret;
    ret.push_back(Token(TOKEN_LOAD_BOOL, prev.Line(), prev.Filename()));
    ret.push_back(prev);
    PushLn(ret);
    return ret;
}

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

Bytecode Expr::LiteralExpr(Token prev, std::string value, Environment* env)
{
    Bytecode ret;
    ret.push_back(Token(TOKEN_LOAD_STRING, prev.Line(), prev.Filename()));
    env->DefineStaticString(value);
    Push(ret, prev);
    PushLn(ret);
    return ret;
}


Bytecode Expr::UnaryExpr(Token oper, Bytecode rhs)
{
    Bytecode ret = rhs;
    TokenTypeEnum type = oper.GetType();
    if (TOKEN_BANG == type)
    {
        ret.push_back(Token(TOKEN_INV, oper.Line(), oper.Filename()));
    }
    else if (TOKEN_MINUS == type)
    {
        ret.push_back(Token(TOKEN_NEG, oper.Line(), oper.Filename()));
    }
    PushLn(ret);
    return ret;
}

Bytecode Expr::VariableExpr(Token id, Environment* env)
{
    Bytecode ret;
    ret.push_back(Token(TOKEN_LOAD_VAR, id.Line(), id.Filename()));
    size_t addr = env->GetVariable(id);
    ret.push_back(Token(TOKEN_INTEGER, std::to_string(addr), addr, addr, id.Line(), id.Filename()));
    PushLn(ret);
    return ret;
}
