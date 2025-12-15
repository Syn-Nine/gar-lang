#ifndef COMPILER_H
#define COMPILER_H


#include "Token.h"
#include "ErrorHandler.h"
#include "Expressions.h"


class Compiler
{
public:
    Compiler() = delete;
    Compiler(TokenList tokenList, ErrorHandler* errorHandler)
    {
        m_tokenList = tokenList;
        m_current = 0;
        m_errorHandler = errorHandler;
    }

    
    //-----------------------------------------------------------------------------
    TokenList Compile()
    {
        Bytecode bc;
        while (!IsAtEnd() && !m_errorHandler->HasErrors())
        {
            bc = Declaration();
            if (bc.empty()) break;
            Append(m_bytecode, bc);
        }
        Push(m_bytecode, Token(TOKEN_END_OF_FILE, 0, ""));
        return m_bytecode;
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Declaration()
    {
        return Statement();
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Statement()
    {
        return ExpressionStatement();
    }

    
    //-----------------------------------------------------------------------------
    Bytecode ExpressionStatement()
    {
        return Expression();
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Expression()
    {
        return Addition();
    }


    //-----------------------------------------------------------------------------
    Bytecode Addition()
    {
        Bytecode lhs = Multiplication();
        while (MatchVar(2, TOKEN_MINUS, TOKEN_PLUS))
        {
            Token oper = Previous();
            Bytecode rhs = Multiplication();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Multiplication()
    {
        Bytecode lhs = Power();
        while (MatchVar(2, TOKEN_SLASH, TOKEN_STAR))
        {
            Token oper = Previous();
            Bytecode rhs = Power();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Power()
    {
        Bytecode lhs = Modulus();
        while (Match(TOKEN_HAT))
        {
            Token oper = Previous();
            Bytecode rhs = Modulus();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Modulus()
    {
        Bytecode lhs = Primary();
        while (Match(TOKEN_PERCENT))
        {
            Token oper = Previous();
            Bytecode rhs = Primary();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Primary()
    {
        if (Match(TOKEN_FLOAT)) return Expr::LiteralExpr(Previous(), Previous().DoubleValue());
        if (Match(TOKEN_INTEGER)) return Expr::LiteralExpr(Previous(), Previous().IntValue());
        if (Match(TOKEN_PI)) return Expr::LiteralExpr(Previous(), acos(-1));

        if (Match(TOKEN_LEFT_PAREN))
        {
            Bytecode expr = Expression();
            Consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
            return expr;
        }

        return Bytecode();
    }

private:

    Token Advance();
    bool Check(TokenTypeEnum tokenType);
    bool CheckNext(TokenTypeEnum tokenType);
    bool CheckNextNext(TokenTypeEnum tokenType);
    bool Consume(TokenTypeEnum tokenType, std::string err);
    void Include();
    bool IsAtEnd();
    bool Match(TokenTypeEnum tokenType);
    bool MatchVar(int count, ...);
    Token Peek();
    Token Previous();
    void Error(Token token, const std::string& err);


    ErrorHandler* m_errorHandler;
    TokenList m_tokenList;
    TokenList m_bytecode;
    int m_current;
    
};




#endif // COMPILER_H