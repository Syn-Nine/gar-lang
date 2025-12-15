#ifndef COMPILER_H
#define COMPILER_H


#include "Token.h"
#include "ErrorHandler.h"
#include "Expressions.h"
#include "Statements.h"
#include "Environment.h"


class Compiler
{
public:
    Compiler() = delete;
    Compiler(TokenList tokenList, Environment* environment, ErrorHandler* errorHandler)
    {
        m_tokenList = tokenList;
        m_current = 0;
        m_errorHandler = errorHandler;
        m_env = environment;
        m_env->RegisterErrorHandler(errorHandler);
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
        if (Match(TOKEN_VAR)) return VarDeclaration();

        return Statement();
    }

    
    //-----------------------------------------------------------------------------
    Bytecode VarDeclaration()
    {
        if (!Consume(TOKEN_IDENTIFIER, "Expected identifier.")) return Bytecode();

        Token id = Previous();
        Bytecode stmt = Stmt::VarStmt(id, m_env);

        if (Match(TOKEN_EQUAL))
        {
            Bytecode value = Addition();
            Bytecode expr = Expr::AssignExpr(id, value, m_env);
            Append(stmt, expr);
        }

        return stmt;
    }


    //-----------------------------------------------------------------------------
    Bytecode Statement()
    {
        if (MatchVar(2, TOKEN_PRINT, TOKEN_PRINTLN)) return PrintStatement();
        if (Match(TOKEN_IF)) return IfStatement();
        if (Match(TOKEN_LEFT_BRACE)) return Stmt::BlockStmt(BlockStatement(), m_env);

        return ExpressionStatement();
    }

    //-----------------------------------------------------------------------------
    Bytecode BlockStatement()
    {
        Bytecode ret;
        while (!Check(TOKEN_RIGHT_BRACE) && !IsAtEnd() && !m_errorHandler->HasErrors())
        {
            Bytecode stmt = Declaration();
            Append(ret, stmt);
        }

        Consume(TOKEN_RIGHT_BRACE, "Expected '}' after block.");
        return ret;
    }

    //-----------------------------------------------------------------------------
    Bytecode IfStatement()
    {
        Token token = Previous();
        Bytecode condition = Expression();

        if (Check(TOKEN_LEFT_BRACE))
        {
            Bytecode thenBranch = Statement();
            Bytecode elseBranch;

            if (Match(TOKEN_ELSE))
            {
                if (Check(TOKEN_IF))
                {
                    elseBranch = Statement();
                }
                else if (Check(TOKEN_LEFT_BRACE))
                {
                    elseBranch = Statement();
                }
                else
                {
                    Error(Previous(), "Expected '{' or ':' after else condition.");
                }
            }
            return Stmt::IfStmt(token, condition, thenBranch, elseBranch, m_env);
        }
        else
        {
            Error(token, "Expected '{' or ':' after if condition.");
        }
        
        return Bytecode();
    }

    
    //-----------------------------------------------------------------------------
    Bytecode PrintStatement()
    {
        Token prev = Previous();
        if (Consume(TOKEN_LEFT_PAREN, "Expected '(' after print."))
        {
            Bytecode expr;
            Token str = Peek();
            if (!Check(TOKEN_RIGHT_PAREN)) expr = Expression();

            if (Consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression."))
            {
                return Stmt::PrintStmt(expr, prev);
            }
        }
        return Bytecode();
    }

    
    //-----------------------------------------------------------------------------
    Bytecode ExpressionStatement()
    {
        return Expression();
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Expression()
    {
        return Assignment();
    }

    //-----------------------------------------------------------------------------
    Bytecode Assignment()
    {
        if (Check(TOKEN_IDENTIFIER) && CheckNext(TOKEN_EQUAL))
        {
            Advance();
            Token id = Previous();
            Advance();
            Bytecode value = Addition();
            return Expr::AssignExpr(id, value, m_env);
        }

        return Or();
    }

    //-----------------------------------------------------------------------------
    Bytecode Or()
    {
        Bytecode lhs = And();
        while (Match(TOKEN_OR))
        {
            Token oper = Previous();
            Bytecode rhs = And();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }


    //-----------------------------------------------------------------------------
    Bytecode And()
    {
        Bytecode lhs = Comparison();
        while (Match(TOKEN_AND))
        {
            Token oper = Previous();
            Bytecode rhs = Comparison();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }


    //-----------------------------------------------------------------------------
    Bytecode Comparison()
    {
        Bytecode lhs = Addition();
        while (MatchVar(6, TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL))
        {
            Token oper = Previous();
            Bytecode rhs = Addition();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
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
        Bytecode lhs = As();
        while (Match(TOKEN_PERCENT))
        {
            Token oper = Previous();
            Bytecode rhs = As();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    Bytecode As()
    {
        // todo

        return Unary();
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Unary()
    {
        if (MatchVar(2, TOKEN_BANG, TOKEN_MINUS))
        {
            Token oper = Previous();
            Bytecode rhs = Unary();
            return Expr::UnaryExpr(oper, rhs);
        }

        return Primary();
    }

    
    //-----------------------------------------------------------------------------
    Bytecode Primary()
    {
        if (Match(TOKEN_FALSE)) return Expr::LiteralExpr(Previous(), false);
        if (Match(TOKEN_TRUE)) return Expr::LiteralExpr(Previous(), true);
        if (Match(TOKEN_FLOAT)) return Expr::LiteralExpr(Previous(), Previous().DoubleValue());
        if (Match(TOKEN_INTEGER)) return Expr::LiteralExpr(Previous(), Previous().IntValue());
        if (Match(TOKEN_STRING)) return Expr::LiteralExpr(Previous(), Previous().StringValue(), m_env);
        if (Match(TOKEN_PI)) return Expr::LiteralExpr(Previous(), acos(-1));

        if (Match(TOKEN_LEFT_PAREN))
        {
            Bytecode expr = Expression();
            Consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
            return expr;
        }

        if (Match(TOKEN_IDENTIFIER))
        {
            Token prev = Previous();
            std::string name = prev.Lexeme();
            Bytecode expr = Expr::VariableExpr(prev, m_env);
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


    Environment* m_env;
    ErrorHandler* m_errorHandler;
    TokenList m_tokenList;
    TokenList m_bytecode;
    int m_current;
    
};




#endif // COMPILER_H