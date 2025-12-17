#include "Expressions.h"
#include "Environment.h"
#include "Compiler.h"

//-----------------------------------------------------------------------------
IRCode Expr::AsExpr(IRCode lhs, Token oper)
{
    IRCode ret = lhs;
    TokenTypeEnum type = oper.GetType();
    if (TOKEN_VAR_INT == type)
    {
        Compiler::Push(ret, Token(TOKEN_CAST_INT, oper.Line(), oper.Filename()));
    }
    else if (TOKEN_VAR_FLOAT == type)
    {
        Compiler::Push(ret, Token(TOKEN_CAST_FLOAT, oper.Line(), oper.Filename()));
    }
    else if (TOKEN_VAR_STRING == type)
    {
        Compiler::Push(ret, Token(TOKEN_CAST_STRING, oper.Line(), oper.Filename()));
    }
    Compiler::PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Expr::AssignExpr(Token id, IRCode rhs, Environment* env)
{
    IRCode ret = rhs;
    Compiler::Push(ret, Token(TOKEN_STORE_VAR, id.Line(), id.Filename()));
    size_t addr = env->GetVariable(id);
    Compiler::Push(ret, Token(TOKEN_INTEGER, "%" + id.Lexeme(), addr, addr, id.Line(), id.Filename()));
    Compiler::PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Expr::BinaryExpr(IRCode lhs, Token oper, IRCode rhs)
{
    IRCode ret = lhs;
    Compiler::Append(ret, rhs);
    Compiler::Push(ret, oper);
    Compiler::PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Expr::CallExpr(IRCode params, Token callee)
{
    IRCode ret = params;
    Compiler::Push(ret, Token(TOKEN_CALL, callee.Line(), callee.Filename()));
    Compiler::Push(ret, Token(TOKEN_IDENTIFIER, "%" + callee.Lexeme(), callee.Line(), callee.Filename()));
    Compiler::PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Expr::LiteralExpr(Token prev)
{
    IRCode ret;
    TokenTypeEnum type = prev.GetType();
    if (TOKEN_TRUE == type || TOKEN_FALSE == type)
    {
        Compiler::Push(ret, Token(TOKEN_LOAD_BOOL, prev.Line(), prev.Filename()));
    }
    else if (TOKEN_INTEGER == type)
    {
        Compiler::Push(ret, Token(TOKEN_LOAD_INT, prev.Line(), prev.Filename()));
    }
    else if (TOKEN_FLOAT == type)
    {
        Compiler::Push(ret, Token(TOKEN_LOAD_FLOAT, prev.Line(), prev.Filename()));
    }

    Compiler::Push(ret, prev);
    Compiler::PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Expr::LiteralExpr(Token prev, std::string value, Environment* env)
{
    IRCode ret;
    Compiler::Push(ret, Token(TOKEN_LOAD_STRING, prev.Line(), prev.Filename()));
    env->DefineStaticString(value);
    Compiler::Push(ret, prev);
    Compiler::PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Expr::UnaryExpr(Token oper, IRCode rhs)
{
    IRCode ret = rhs;
    TokenTypeEnum type = oper.GetType();
    if (TOKEN_BANG == type)
    {
        Compiler::Push(ret, Token(TOKEN_INV, oper.Line(), oper.Filename()));
    }
    else if (TOKEN_MINUS == type)
    {
        Compiler::Push(ret, Token(TOKEN_NEG, oper.Line(), oper.Filename()));
    }
    Compiler::PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Expr::VariableExpr(Token id, Environment* env)
{
    IRCode ret;
    Compiler::Push(ret, Token(TOKEN_LOAD_VAR, id.Line(), id.Filename()));
    size_t addr = env->GetVariable(id);
    Compiler::Push(ret, Token(TOKEN_INTEGER, "%" + id.Lexeme(), addr, addr, id.Line(), id.Filename()));
    Compiler::PushLn(ret);
    return ret;
}
