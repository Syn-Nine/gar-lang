#include "Statements.h"
#include "Environment.h"


Bytecode Stmt::BlockStmt(Bytecode body, Environment* env)
{
    return body;
}

Bytecode Stmt::IfStmt(Token oper, Bytecode condition, Bytecode thenBranch, Bytecode elseBranch, Environment* env)
{
    Bytecode ret;
    Append(ret, condition);

    std::string thenLabel = env->NewLabel("then");
    std::string tailLabel = env->NewLabel("tail");

    Push(ret, Token(TOKEN_IF, oper.Line(), oper.Filename()));
    Push(ret, Token(TOKEN_IDENTIFIER, thenLabel, oper.Line(), oper.Filename()));
    PushLn(ret);

    Append(ret, elseBranch);
    PushJmp(ret, tailLabel);

    PushLabel(ret, thenLabel);
    Append(ret, thenBranch);
    PushJmp(ret, tailLabel);

    PushLabel(ret, tailLabel);

    return ret;
}


Bytecode Stmt::PrintStmt(Bytecode expr, Token oper)
{
    Bytecode ret;
    Append(ret, expr);
    Push(ret, oper);
    PushLn(ret);
    return ret;
}

Bytecode Stmt::VarStmt(Token oper, Environment* env)
{
    Bytecode ret;
    PushNoop(ret);
    
    std::string lex = oper.Lexeme();
    env->DefineVariable(lex);

    return ret;
}