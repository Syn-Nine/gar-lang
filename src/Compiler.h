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
        IRCode bc;
        PushBlockStart(m_IRCode, Token(TOKEN_BLOCK_START, 0, ""));

        while (!IsAtEnd() && !m_errorHandler->HasErrors())
        {
            bc = Declaration();
            if (bc.empty()) break;
            Append(m_IRCode, bc);
        }

        PushBlockEnd(m_IRCode, Token(TOKEN_BLOCK_END, 0, ""));
        Push(m_IRCode, Token(TOKEN_END_OF_FILE, 0, ""));
        return m_IRCode;
    }

    
    //-----------------------------------------------------------------------------
    IRCode Declaration()
    {
        if (Match(TOKEN_VAR)) return VarDeclaration();

        return Statement();
    }


    //-----------------------------------------------------------------------------
    IRCode VarDeclaration()
    {
        IRCode ret;
        TokenList ids;

        while (true)
        {
            if (!Consume(TOKEN_IDENTIFIER, "Expected identifier.")) return IRCode();

            Token id = Previous();
            ids.push_back(id);
            Append(ret, Stmt::VarStmt(id, m_env));

            if (!Check(TOKEN_COMMA)) break;
            Advance();
        }

        if (!Consume(TOKEN_EQUAL, "Expected assignment after variable declaration.")) return IRCode();
        
        for (int i = 0; i < ids.size(); ++i)
        {
            IRCode value = Addition();
            if (value.empty()) return IRCode();
            IRCode expr = Expr::AssignExpr(ids[i], value, m_env);
            Append(ret, expr);
            if (i < ids.size() - 1)
            {
                if (!Consume(TOKEN_COMMA, "Expected comma.")) return IRCode();
            }
        }

        return ret;
    }


    //-----------------------------------------------------------------------------
    IRCode Statement()
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
    IRCode BlockStatement()
    {
        Token oper = Previous();
        IRCode ret;
        
        m_env = Environment::Push();
        PushBlockStart(ret, oper);

        while (!Check(TOKEN_RIGHT_BRACE) && !IsAtEnd() && !m_errorHandler->HasErrors())
        {
            IRCode stmt = Declaration();
            Append(ret, stmt);
        }

        PushBlockEnd(ret, oper);
        m_env = Environment::Pop();

        Consume(TOKEN_RIGHT_BRACE, "Expected '}' after block.");
        return ret;
    }


    //-----------------------------------------------------------------------------
    IRCode ForStatement()
    {
        if (!Consume(TOKEN_IDENTIFIER, "Expected identifier.")) return IRCode();
        Token id = Previous();

        if (!Consume(TOKEN_IN, "Expected 'in'.")) return IRCode();

        IRCode lhs = Addition(); // lhs of range

        if (!MatchVar(2, TOKEN_DOT_DOT, TOKEN_DOT_DOT_EQUAL))
        {
            Error(Previous(), "Expected range.");
            return IRCode();
        }
        
        Token oper = Previous();
        IRCode one = Expr::LiteralExpr(Token(TOKEN_INTEGER, "1", 1, 1, oper.Line(), oper.Filename()));

        IRCode rhs = Addition(); // rhs of range
        if (oper.GetType() == TOKEN_DOT_DOT_EQUAL)
        {
            // if ..= then add one to rhs
            rhs = Expr::BinaryExpr(rhs, Token(TOKEN_PLUS, oper.Line(), oper.Filename()), one);                
        }
        
        if (!Check(TOKEN_LEFT_BRACE))
        {
            Error(oper, "Expected '{' after while condition.");
            return IRCode();
        }

        std::string postLabel = m_env->NewLabel("post");
        std::string mergeLabel = m_env->NewLabel("merge");

        IRCode ret;
        m_env = Environment::Push();
        PushBlockStart(ret, oper);

        Token loop_end = Token(TOKEN_IDENTIFIER, "__loop_end", oper.Line(), oper.Filename());

        Stmt::VarStmt(id, m_env);
        Stmt::VarStmt(loop_end, m_env);

        // init before the loop
        Append(ret, Expr::AssignExpr(id, lhs, m_env));
        Append(ret, Expr::AssignExpr(loop_end, rhs, m_env));
        
        // lhs < rhs
        IRCode condition = Expr::BinaryExpr(Expr::VariableExpr(id, m_env), Token(TOKEN_LESS, oper.Line(), oper.Filename()), Expr::VariableExpr(loop_end, m_env));

        m_env->PushLoopBreakContinue(mergeLabel, postLabel);
        IRCode body = Statement();
        m_env->PopLoopBreakContinue();

        // build post increment
        IRCode inc = Expr::BinaryExpr(Expr::VariableExpr(id, m_env), Token(TOKEN_PLUS, oper.Line(), oper.Filename()), one);
        IRCode post = Expr::AssignExpr(id, inc, m_env);

        Append(ret, Stmt::WhileStmt(oper, condition, body, post, postLabel, mergeLabel, m_env));
        
        PushBlockEnd(ret, oper);
        m_env = Environment::Pop();

        return ret;
    }


    //-----------------------------------------------------------------------------
    IRCode IfStatement()
    {
        Token token = Previous();
        IRCode condition = Expression();

        if (Check(TOKEN_LEFT_BRACE))
        {
            IRCode thenBranch = Statement();
            IRCode elseBranch;

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
        
        return IRCode();
    }


    //-----------------------------------------------------------------------------
    IRCode LoopStatement()
    {
        Token prev = Previous();
        IRCode condition = Expr::LiteralExpr(Token(TOKEN_TRUE, prev.Line(), prev.Filename()));
        
        if (Check(TOKEN_LEFT_BRACE))
        {
            std::string postLabel = m_env->NewLabel("post");
            std::string mergeLabel = m_env->NewLabel("merge");

            m_env->PushLoopBreakContinue(mergeLabel, postLabel);
            IRCode body = Statement();
            m_env->PopLoopBreakContinue();
            
            IRCode post;

            return Stmt::WhileStmt(prev, condition, body, post, postLabel, mergeLabel, m_env);
        }
        else
        {
            Error(Previous(), "Expected '{' after loop statement.");
        }

        return IRCode();
    }

    
    //-----------------------------------------------------------------------------
    IRCode PrintStatement()
    {
        Token prev = Previous();
        if (Consume(TOKEN_LEFT_PAREN, "Expected '(' after print."))
        {
            IRCode expr;
            Token str = Peek();
            if (!Check(TOKEN_RIGHT_PAREN)) expr = Expression();

            if (Consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression."))
            {
                return Stmt::PrintStmt(expr, prev);
            }
        }
        return IRCode();
    }

    
    //-----------------------------------------------------------------------------
    IRCode WhileStatement()
    {
        Token prev = Previous();
        IRCode condition = Expression();

        if (Check(TOKEN_LEFT_BRACE))
        {
            std::string postLabel = m_env->NewLabel("post");
            std::string mergeLabel = m_env->NewLabel("merge");

            m_env->PushLoopBreakContinue(mergeLabel, postLabel);
            IRCode body = Statement();
            m_env->PopLoopBreakContinue();

            IRCode post;
            return Stmt::WhileStmt(prev, condition, body, post, postLabel, mergeLabel, m_env);
        }
        else
        {
            Error(Previous(), "Expected '{' after while condition.");
        }

        return IRCode();
    }

    
    //-----------------------------------------------------------------------------
    IRCode ExpressionStatement()
    {
        return Expression();
    }

    
    //-----------------------------------------------------------------------------
    IRCode Expression()
    {
        return Assignment();
    }

    //-----------------------------------------------------------------------------
    IRCode Assignment()
    {
        if (Check(TOKEN_IDENTIFIER) && CheckNext(TOKEN_EQUAL))
        {
            Advance();
            Token id = Previous();
            Advance();
            IRCode value = Addition();
            return Expr::AssignExpr(id, value, m_env);
        }

        return Or();
    }

    //-----------------------------------------------------------------------------
    IRCode Or()
    {
        IRCode lhs = And();
        while (Match(TOKEN_OR))
        {
            Token oper = Previous();
            IRCode rhs = And();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }


    //-----------------------------------------------------------------------------
    IRCode And()
    {
        IRCode lhs = Comparison();
        while (Match(TOKEN_AND))
        {
            Token oper = Previous();
            IRCode rhs = Comparison();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }


    //-----------------------------------------------------------------------------
    IRCode Comparison()
    {
        IRCode lhs = Addition();
        while (MatchVar(6, TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL))
        {
            Token oper = Previous();
            IRCode rhs = Addition();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }


    //-----------------------------------------------------------------------------
    IRCode Addition()
    {
        IRCode lhs = Multiplication();
        while (MatchVar(2, TOKEN_MINUS, TOKEN_PLUS))
        {
            Token oper = Previous();
            IRCode rhs = Multiplication();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    IRCode Multiplication()
    {
        IRCode lhs = Power();
        while (MatchVar(2, TOKEN_SLASH, TOKEN_STAR))
        {
            Token oper = Previous();
            IRCode rhs = Power();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    IRCode Power()
    {
        IRCode lhs = Modulus();
        while (Match(TOKEN_HAT))
        {
            Token oper = Previous();
            IRCode rhs = Modulus();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    IRCode Modulus()
    {
        IRCode lhs = As();
        while (Match(TOKEN_PERCENT))
        {
            Token oper = Previous();
            IRCode rhs = As();
            lhs = Expr::BinaryExpr(lhs, oper, rhs);
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    IRCode As()
    {
        IRCode lhs = Unary();
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
                return IRCode();
            }
        }
        return lhs;
    }

    
    //-----------------------------------------------------------------------------
    IRCode Unary()
    {
        if (MatchVar(2, TOKEN_BANG, TOKEN_MINUS))
        {
            Token oper = Previous();
            IRCode rhs = Unary();
            return Expr::UnaryExpr(oper, rhs);
        }

        return Primary();
    }

    
    //-----------------------------------------------------------------------------
    IRCode Primary()
    {
        if (MatchVar(4, TOKEN_TRUE, TOKEN_FALSE, TOKEN_FLOAT, TOKEN_INTEGER)) return Expr::LiteralExpr(Previous());
        if (Match(TOKEN_STRING)) return Expr::LiteralExpr(Previous(), Previous().StringValue(), m_env);
        if (Match(TOKEN_PI)) return Expr::LiteralExpr(Token(TOKEN_FLOAT, "pi", acos(-1), acos(-1), Previous().Line(), Previous().Filename()));

        if (Match(TOKEN_LEFT_PAREN))
        {
            IRCode expr = Expression();
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

        return IRCode();
    }


    //-----------------------------------------------------------------------------
    IRCode FinishCall(Token callee)
    {
        IRCode params;        
        
        if (!Check(TOKEN_RIGHT_PAREN))
        {
            // push arguments to the param stack
            do
            {
                Append(params, Expression());
            } while (Match(TOKEN_COMMA));
        }

        if (!Consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.")) return IRCode();
        
        return Expr::CallExpr(params, callee);
    }

    static void Append(IRCode& lhs, IRCode rhs);
    static void Push(IRCode& lhs, Token rhs);
    static void PushLn(IRCode& lhs);
    static void PushJmp(IRCode& lhs, std::string label);
    static void PushLabel(IRCode& lhs, std::string label);
    static void PushNoop(IRCode& lhs);
    static void PushBlockStart(IRCode& lhs, Token oper);
    static void PushBlockEnd(IRCode& lhs, Token oper);

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
    TokenList m_IRCode;
    int m_current;
    
};




#endif // COMPILER_H