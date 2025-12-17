#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "Token.h"
#include <string>

class Environment;

class Stmt
{
public:
    static Bytecode BreakStmt(Token oper, Environment* env);
    static Bytecode ContinueStmt(Token oper, Environment* env);
    static Bytecode IfStmt(Token oper, Bytecode condition, Bytecode thenBranch, Bytecode elseBranch, Environment* env);
    static Bytecode PrintStmt(Bytecode expr, Token oper);
    static Bytecode VarStmt(Token oper, Environment* env);
    static Bytecode WhileStmt(Token oper, Bytecode condition, Bytecode body, Bytecode post, std::string postLabel, std::string mergeLabel, Environment* env);

private:

};


#endif // STATEMENTS_H