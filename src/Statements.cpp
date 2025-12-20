#include "Statements.h"
#include "Environment.h"
#include "Compiler.h"


//-----------------------------------------------------------------------------
IRCode Stmt::BreakStmt(Token oper, Environment* env)
{
    IRCode ret;
    Compiler::PushJmp(ret, env->GetParentLoopBreak());
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Stmt::ContinueStmt(Token oper, Environment* env)
{
    IRCode ret;
    // unwind frame pointer to cached location at start of the loop
    //Token loop_fptr = Token(TOKEN_IDENTIFIER, "__loop_fptr", oper.Line(), oper.Filename());
    //Compiler::Append(ret, Expr::VariableExpr(loop_fptr, env));
    //Compiler::Push(ret, Token(TOKEN_STORE_FPTR, oper.Line(), oper.Filename()));
    Compiler::PushJmp(ret, env->GetParentLoopContinue());
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Stmt::FunctionStmt(Token name, int arity, IRCode body, Environment* env)
{
    std::string lex = name.Lexeme();
    
    std::string entryLabel = "__def_" + lex + "_entry";
    std::string tailLabel = "__def_" + lex + "_tail";

    // prototype
    IRCode proto;
    Compiler::PushLabel(proto, entryLabel);
    
    // tail
    Compiler::PushJmp(body, tailLabel);
    Compiler::PushLabel(body, tailLabel);
    Compiler::Push(body, Token(TOKEN_RET, name.Line(), name.Filename()));

    env->DefineFunction("%" + lex, arity, entryLabel, tailLabel, proto, body);

    IRCode ret;
    Compiler::PushNoop(ret);
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Stmt::IfStmt(Token oper, IRCode condition, IRCode thenBranch, IRCode elseBranch, Environment* env)
{
    IRCode ret;
    Compiler::Append(ret, condition);

    std::string thenLabel = env->NewLabel("then");
    std::string tailLabel = env->NewLabel("tail");

    Compiler::Push(ret, Token(TOKEN_IF, oper.Line(), oper.Filename()));
    Compiler::Push(ret, Token(TOKEN_IDENTIFIER, thenLabel, oper.Line(), oper.Filename()));
    Compiler::PushLn(ret);

    Compiler::Append(ret, elseBranch);
    Compiler::PushJmp(ret, tailLabel);

    Compiler::PushLabel(ret, thenLabel);
    Compiler::Append(ret, thenBranch);
    Compiler::PushJmp(ret, tailLabel);

    Compiler::PushLabel(ret, tailLabel);

    return ret;
}


//-----------------------------------------------------------------------------
IRCode Stmt::PrintStmt(IRCode expr, Token oper)
{
    IRCode ret;
    Compiler::Append(ret, expr);
    Compiler::Push(ret, oper);
    if (expr.empty())
    {
        Compiler::Push(ret, Token(TOKEN_PRINT_BLANK, oper.Line(), oper.Filename()));
    }
    Compiler::PushLn(ret);
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Stmt::ReturnStmt(Token oper)
{
    IRCode ret;
    Compiler::Push(ret, Token(TOKEN_RET, oper.Line(), oper.Filename()));
    return ret;
}


//-----------------------------------------------------------------------------
IRCode Stmt::VarStmt(Token oper, Environment* env, bool is_const /* = false */)
{
    std::string lex = oper.Lexeme();
    
    IRCode ret;
    Compiler::PushNoop(ret);

    if (env->IsGlobal())
    {
        env->DefineGlobalVariable(lex, is_const);
    }
    else
    {
        env->DefineLocalVariable(lex, is_const);
    }

    return ret;
}


//-----------------------------------------------------------------------------
IRCode Stmt::WhileStmt(Token oper, IRCode condition, IRCode body, IRCode post, std::string postLabel, std::string mergeLabel, Environment* env)
{
    IRCode ret;

    std::string loopLabel = env->NewLabel("loop");
    std::string bodyLabel = env->NewLabel("body");
    
    Compiler::PushJmp(ret, loopLabel);
    Compiler::PushLabel(ret, loopLabel);

    // if condition is true, jump to body
    Compiler::Append(ret, condition);
    Compiler::Push(ret, Token(TOKEN_IF, oper.Line(), oper.Filename()));
    Compiler::Push(ret, Token(TOKEN_IDENTIFIER, bodyLabel, oper.Line(), oper.Filename()));
    Compiler::PushLn(ret);

    // else jump to merge
    Compiler::PushJmp(ret, mergeLabel);

    // insert body and jump to post
    Compiler::PushLabel(ret, bodyLabel);
    Compiler::Append(ret, body);
    Compiler::PushJmp(ret, postLabel);

    // insert post logic and jump back to the top
    Compiler::PushLabel(ret, postLabel);
    Compiler::Append(ret, post);
    Compiler::PushJmp(ret, loopLabel);

    // insert merge
    Compiler::PushLabel(ret, mergeLabel);
    
    return ret;
}