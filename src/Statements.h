#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "Token.h"
#include <string>

class Environment;

class Stmt
{
public:
    static Bytecode BlockStmt(Bytecode body, Environment* env);
    static Bytecode IfStmt(Token oper, Bytecode condition, Bytecode thenBranch, Bytecode elseBranch, Environment* env);
    static Bytecode PrintStmt(Bytecode expr, Token oper);
    static Bytecode VarStmt(Token oper, Environment* env);

private:

};


#endif // STATEMENTS_H