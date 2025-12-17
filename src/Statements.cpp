#include "Statements.h"
#include "Environment.h"


Bytecode Stmt::BreakStmt(Token oper, Environment* env)
{
    Bytecode ret;
    PushJmp(ret, env->GetParentLoopBreak());
    return ret;
}


Bytecode Stmt::ContinueStmt(Token oper, Environment* env)
{
    Bytecode ret;
    PushJmp(ret, env->GetParentLoopContinue());
    return ret;
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
    if (expr.empty())
    {
        Push(ret, Token(TOKEN_PRINT_BLANK, oper.Line(), oper.Filename()));
    }
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


Bytecode Stmt::WhileStmt(Token oper, Bytecode condition, Bytecode body, Bytecode post, std::string postLabel, std::string mergeLabel, Environment* env)
{
    Bytecode ret;

    std::string loopLabel = env->NewLabel("loop");
    std::string bodyLabel = env->NewLabel("body");
    
    PushJmp(ret, loopLabel);
    PushLabel(ret, loopLabel);

    // if condition is true, jump to body
    Append(ret, condition);
    Push(ret, Token(TOKEN_IF, oper.Line(), oper.Filename()));
    Push(ret, Token(TOKEN_IDENTIFIER, bodyLabel, oper.Line(), oper.Filename()));
    PushLn(ret);

    // else jump to merge
    PushJmp(ret, mergeLabel);

    // insert body and jump to post
    PushLabel(ret, bodyLabel);
    Append(ret, body);
    PushJmp(ret, postLabel);

    // insert post logic and jump back to the top
    PushLabel(ret, postLabel);
    Append(ret, post);
    PushJmp(ret, loopLabel);

    // insert merge
    PushLabel(ret, mergeLabel);
    
    return ret;
}