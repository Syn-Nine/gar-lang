#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "Token.h"
#include <string>

class Environment;

class Stmt
{
public:
    static IRCode BreakStmt(Token oper, Environment* env);
    static IRCode ContinueStmt(Token oper, Environment* env);
    static IRCode IfStmt(Token oper, IRCode condition, IRCode thenBranch, IRCode elseBranch, Environment* env);
    static IRCode PrintStmt(IRCode expr, Token oper);
    static IRCode VarStmt(Token oper, Environment* env);
    static IRCode WhileStmt(Token oper, IRCode condition, IRCode body, IRCode post, std::string postLabel, std::string mergeLabel, Environment* env);

private:

};


#endif // STATEMENTS_H