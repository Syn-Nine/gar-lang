#include "Expressions.h"
#include "Environment.h"


//-----------------------------------------------------------------------------
Bytecode Expr::AsExpr(Bytecode lhs, Token oper)
{
    Bytecode ret = lhs;
    TokenTypeEnum type = oper.GetType();
    if (TOKEN_VAR_INT == type)
    {
        Push(ret, Token(TOKEN_CAST_INT, oper.Line(), oper.Filename()));
    }
    else if (TOKEN_VAR_FLOAT == type)
    {
        Push(ret, Token(TOKEN_CAST_FLOAT, oper.Line(), oper.Filename()));
    }
    else if (TOKEN_VAR_STRING == type)
    {
        Push(ret, Token(TOKEN_CAST_STRING, oper.Line(), oper.Filename()));
    }
    PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
Bytecode Expr::AssignExpr(Token id, Bytecode rhs, Environment* env)
{
    Bytecode ret = rhs;
    Push(ret, Token(TOKEN_STORE_VAR, id.Line(), id.Filename()));
    size_t addr = env->GetVariable(id);
    Push(ret, Token(TOKEN_INTEGER, "%" + id.Lexeme(), addr, addr, id.Line(), id.Filename()));
    PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
Bytecode Expr::BinaryExpr(Bytecode lhs, Token oper, Bytecode rhs)
{
    Bytecode ret = lhs;
    Append(ret, rhs);
    Push(ret, oper);
    PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
Bytecode Expr::CallExpr(Bytecode params, Token callee)
{
    Bytecode ret = params;
    Push(ret, Token(TOKEN_CALL, callee.Line(), callee.Filename()));
    Push(ret, Token(TOKEN_IDENTIFIER, "%" + callee.Lexeme(), callee.Line(), callee.Filename()));
    PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
Bytecode Expr::LiteralExpr(Token prev)
{
    Bytecode ret;
    TokenTypeEnum type = prev.GetType();
    if (TOKEN_TRUE == type || TOKEN_FALSE == type)
    {
        Push(ret, Token(TOKEN_LOAD_BOOL, prev.Line(), prev.Filename()));
    }
    else if (TOKEN_INTEGER == type)
    {
        Push(ret, Token(TOKEN_LOAD_INT, prev.Line(), prev.Filename()));
    }
    else if (TOKEN_FLOAT == type)
    {
        Push(ret, Token(TOKEN_LOAD_FLOAT, prev.Line(), prev.Filename()));
    }

    ret.push_back(prev);
    PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
Bytecode Expr::LiteralExpr(Token prev, std::string value, Environment* env)
{
    Bytecode ret;
    Push(ret, Token(TOKEN_LOAD_STRING, prev.Line(), prev.Filename()));
    env->DefineStaticString(value);
    Push(ret, prev);
    PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
Bytecode Expr::UnaryExpr(Token oper, Bytecode rhs)
{
    Bytecode ret = rhs;
    TokenTypeEnum type = oper.GetType();
    if (TOKEN_BANG == type)
    {
        Push(ret, Token(TOKEN_INV, oper.Line(), oper.Filename()));
    }
    else if (TOKEN_MINUS == type)
    {
        Push(ret, Token(TOKEN_NEG, oper.Line(), oper.Filename()));
    }
    PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
Bytecode Expr::VariableExpr(Token id, Environment* env)
{
    Bytecode ret;
    Push(ret, Token(TOKEN_LOAD_VAR, id.Line(), id.Filename()));
    size_t addr = env->GetVariable(id);
    Push(ret, Token(TOKEN_INTEGER, "%" + id.Lexeme(), addr, addr, id.Line(), id.Filename()));
    PushLn(ret);
    return ret;
}
