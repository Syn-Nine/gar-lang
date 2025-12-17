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
        Push(m_bytecode, Token(TOKEN_PUSH_SCRATCH_PTR, 0, ""));

        while (!IsAtEnd() && !m_errorHandler->HasErrors())
        {
            bc = Declaration();
            if (bc.empty()) break;
            Append(m_bytecode, bc);
        }

        Push(m_bytecode, Token(TOKEN_POP_SCRATCH_PTR, 0, ""));
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
        Bytecode ret;
        TokenList ids;

        while (true)
        {
            if (!Consume(TOKEN_IDENTIFIER, "Expected identifier.")) return Bytecode();

            Token id = Previous();
            ids.push_back(id);
            Append(ret, Stmt::VarStmt(id, m_env));

            if (!Check(TOKEN_COMMA)) break;
            Advance();
        }

        if (!Consume(TOKEN_EQUAL, "Expected assignment after variable declaration.")) return Bytecode();
        
        for (int i = 0; i < ids.size(); ++i)
        {
            Bytecode value = Addition();
            if (value.empty()) return Bytecode();
            Bytecode expr = Expr::AssignExpr(ids[i], value, m_env);
            Append(ret, expr);
            if (i < ids.size() - 1)
            {
                if (!Consume(TOKEN_COMMA, "Expected comma.")) return Bytecode();
            }
        }

        return ret;
    }


    //-----------------------------------------------------------------------------
    Bytecode Statement()
    {
        if (Match(TOKEN_LEFT_BRACE)) return BlockStatement();
        if (Match(TOKEN_BREAK)) return Stmt::BreakStmt(Previous(), m_env);
        if (Match(TOKEN_CONTINUE)) return Stmt::ContinueStmt(Previous(), m_env);
        if (Match(TOKEN_FOR)) return ForStatement();
        if (Match(TOKEN_IF)) return IfStatement();
        if (Match(TOKEN_LOOP)) return LoopStatement();
        if (MatchVar(2, TOKEN_PRINT, TOKEN_PRINTLN)) return PrintStatement();
        if (Match(TOKEN_WHILE)) return WhileStatement();

        return ExpressionStatement();
    }


    //-----------------------------------------------------------------------------
    Bytecode BlockStatement()
    {

        m_env = Environment::Push();

        Bytecode ret;
        while (!Check(TOKEN_RIGHT_BRACE) && !IsAtEnd() && !m_errorHandler->HasErrors())
        {
            Bytecode stmt = Declaration();
            Append(ret, stmt);
        }

        m_env = Environment::Pop();

        Consume(TOKEN_RIGHT_BRACE, "Expected '}' after block.");
        return ret;
    }


    //-----------------------------------------------------------------------------
    Bytecode ForStatement()
    {
        if (!Consume(TOKEN_IDENTIFIER, "Expected identifier.")) return Bytecode();
        Token id = Previous();

        if (!Consume(TOKEN_IN, "Expected 'in'.")) return Bytecode();

        Bytecode lhs = Addition(); // lhs of range

        if (!MatchVar(2, TOKEN_DOT_DOT, TOKEN_DOT_DOT_EQUAL))
        {
            Error(Previous(), "Expected range.");
            return Bytecode();
        }
        
        Token oper = Previous();
        Bytecode one = Expr::LiteralExpr(Token(TOKEN_INTEGER, "1", 1, 1, oper.Line(), oper.Filename()));

        Bytecode rhs = Addition(); // rhs of range
        if (oper.GetType() == TOKEN_DOT_DOT_EQUAL)
        {
            // if ..= then add one to rhs
            rhs = Expr::BinaryExpr(rhs, Token(TOKEN_PLUS, oper.Line(), oper.Filename()), one);                
        }
        
        if (!Check(TOKEN_LEFT_BRACE))
        {
            Error(oper, "Expected '{' after while condition.");
            return Bytecode();
        }

        std::string postLabel = m_env->NewLabel("post");
        std::string mergeLabel = m_env->NewLabel("merge");

        Bytecode ret;
        m_env = Environment::Push();
        Push(ret, Token(TOKEN_PUSH_SCRATCH_PTR, oper.Line(), oper.Filename()));

        Token loop_end = Token(TOKEN_IDENTIFIER, "__loop_end", oper.Line(), oper.Filename());

        Stmt::VarStmt(id, m_env);
        Stmt::VarStmt(loop_end, m_env);

        // init before the loop
        Append(ret, Expr::AssignExpr(id, lhs, m_env));
        Append(ret, Expr::AssignExpr(loop_end, rhs, m_env));
        
        // lhs < rhs
        Bytecode condition = Expr::BinaryExpr(Expr::VariableExpr(id, m_env), Token(TOKEN_LESS, oper.Line(), oper.Filename()), Expr::VariableExpr(loop_end, m_env));

        m_env->PushLoopBreakContinue(mergeLabel, postLabel);
        Bytecode body = Statement();
        m_env->PopLoopBreakContinue();

        // build post increment
        Bytecode inc = Expr::BinaryExpr(Expr::VariableExpr(id, m_env), Token(TOKEN_PLUS, oper.Line(), oper.Filename()), one);
        Bytecode post = Expr::AssignExpr(id, inc, m_env);

        Append(ret, Stmt::WhileStmt(oper, condition, body, post, postLabel, mergeLabel, m_env));
        
        Push(ret, Token(TOKEN_POP_SCRATCH_PTR, oper.Line(), oper.Filename()));
        m_env = Environment::Pop();

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
                    Error(Previous(), "Expected '{' after else condition.");
                }
            }
            return Stmt::IfStmt(token, condition, thenBranch, elseBranch, m_env);
        }
        else
        {
            Error(token, "Expected '{' after if condition.");
        }
        
        return Bytecode();
    }


    //-----------------------------------------------------------------------------
    Bytecode LoopStatement()
    {
        Token prev = Previous();
        Bytecode condition = Expr::LiteralExpr(Token(TOKEN_TRUE, prev.Line(), prev.Filename()));
        
        if (Check(TOKEN_LEFT_BRACE))
        {
            std::string postLabel = m_env->NewLabel("post");
            std::string mergeLabel = m_env->NewLabel("merge");

            m_env->PushLoopBreakContinue(mergeLabel, postLabel);
            Bytecode body = Statement();
            m_env->PopLoopBreakContinue();
            
            Bytecode post;

            return Stmt::WhileStmt(prev, condition, body, post, postLabel, mergeLabel, m_env);
        }
        else
        {
            Error(Previous(), "Expected '{' after loop statement.");
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
    Bytecode WhileStatement()
    {
        Token prev = Previous();
        Bytecode condition = Expression();

        if (Check(TOKEN_LEFT_BRACE))
        {
            std::string postLabel = m_env->NewLabel("post");
            std::string mergeLabel = m_env->NewLabel("merge");

            m_env->PushLoopBreakContinue(mergeLabel, postLabel);
            Bytecode body = Statement();
            m_env->PopLoopBreakContinue();

            Bytecode post;
            return Stmt::WhileStmt(prev, condition, body, post, postLabel, mergeLabel, m_env);
        }
        else
        {
            Error(Previous(), "Expected '{' after while condition.");
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
        Bytecode lhs = Unary();
        while (Match(TOKEN_AS))
        {
            if (MatchVar(3, TOKEN_VAR_INT, TOKEN_VAR_FLOAT, TOKEN_VAR_STRING))
            {
                Token oper = Previous();
                lhs = Expr::AsExpr(lhs, oper);
            }
            else
            {
                Error(Previous(), "Expected variable type after 'as'.");
                return Bytecode();
            }
        }
        return lhs;
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
        if (MatchVar(4, TOKEN_TRUE, TOKEN_FALSE, TOKEN_FLOAT, TOKEN_INTEGER)) return Expr::LiteralExpr(Previous());
        if (Match(TOKEN_STRING)) return Expr::LiteralExpr(Previous(), Previous().StringValue(), m_env);
        if (Match(TOKEN_PI)) return Expr::LiteralExpr(Token(TOKEN_FLOAT, "pi", acos(-1), acos(-1), Previous().Line(), Previous().Filename()));

        if (Match(TOKEN_LEFT_PAREN))
        {
            Bytecode expr = Expression();
            Consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
            return expr;
        }

        if (Match(TOKEN_IDENTIFIER))
        {
            Token name = Previous();
            
            if (Match(TOKEN_LEFT_PAREN))
            {
                // function call
                return FinishCall(name);
            }
            else
            {
                // load variable
                return Expr::VariableExpr(name, m_env);
            }
        }

        return Bytecode();
    }


    //-----------------------------------------------------------------------------
    Bytecode FinishCall(Token callee)
    {
        Bytecode params;        
        
        if (!Check(TOKEN_RIGHT_PAREN))
        {
            // push arguments to the param stack
            do
            {
                Append(params, Expression());
            } while (Match(TOKEN_COMMA));
        }

        if (!Consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.")) return Bytecode();
        
        return Expr::CallExpr(params, callee);
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