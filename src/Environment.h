#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "ErrorHandler.h"
#include "Token.h"

#include <set>
#include <string>

class Environment {

public:
    
    static Environment* Push()
    {
        Environment* e = new Environment();
        //e->m_parent_function = nullptr;
        if (!m_stack.empty())
        {
            e->m_parent = m_stack.back();
            //e->m_parent_function = e->m_parent->m_parent_function;
        }
        m_stack.push_back(e);
        return e;
    }

    static Environment* Pop()
    {
        if (!m_stack.empty())
        {
            //m_stack.back()->EmitCleanup();
            Environment* parent = m_stack.back()->m_parent;
            delete m_stack.back();
            m_stack.pop_back();
            return parent;
        }
        
        Error(Token(TOKEN_INVALID, 0, ""), "Attempting to pop empty environment.");
        return nullptr;
    }

    void DefineVariable(std::string lexeme)
    {
        if (0 == m_alloca.count(lexeme))
        {
            int idx = m_var_names.size();
            m_var_names.push_back(lexeme);
            m_alloca.insert(std::make_pair(lexeme, idx));
        }
    }

    void DefineStaticString(std::string lexeme)
    {
        m_static_strings.insert(lexeme);
    }

    std::set<std::string>& GetStaticStrings()
    {
        return m_static_strings;
    }

    size_t GetVariable(Token token)
    {
        if (m_stack.empty())
        {
            Error(token, "Environment stack is empty.");
            return 0;
        }

        return m_stack.back()->GetVariable_Recursive(token);
    }

    size_t NewEnum()
    {
        return m_enum_counter++;
    }

    std::string NewLabel(std::string prefix)
    {
        return prefix + "_" + std::to_string(m_block_counter++);
    }

    std::string GetParentLoopBreak()
    {
        if (!m_parent) return nullptr;
        if (!m_parent->m_loopBreak.empty()) return m_parent->m_loopBreak;
        return m_parent->GetParentLoopBreak();
    }

    std::string GetParentLoopContinue()
    {
        if (!m_parent) return nullptr;
        if (!m_parent->m_loopContinue.empty()) return m_parent->m_loopContinue;
        return m_parent->GetParentLoopContinue();
    }


    void PushLoopBreakContinue(std::string merge, std::string post)
    {
        m_loopBreak = merge;
        m_loopContinue = post;
    }

    void PopLoopBreakContinue()
    {
        PushLoopBreakContinue("", "");
    }

    static void RegisterErrorHandler(ErrorHandler* eh)
    {
        m_errorHandler = eh;
    }

    static void Error(Token token, const std::string& err)
    {
        m_errorHandler->Error(token.Filename(), token.Line(), "at '" + token.Lexeme() + "'", err);
    }


private:

    Environment()
    {
        m_parent = nullptr;
    }

    size_t GetVariable_Recursive(Token token)
    {
        std::string lex = token.Lexeme();
        if (0 != m_alloca.count(lex)) return m_alloca.at(lex);
        if (m_parent) return m_parent->GetVariable_Recursive(token);

        Error(token, "Variable not found in environment.");
        return 0;
    }

    static ErrorHandler* m_errorHandler;
    static std::vector<Environment*> m_stack;
    static size_t m_block_counter;
    static std::set<std::string> m_static_strings;
    static size_t m_enum_counter;

    Environment* m_parent;
    static std::vector<std::string> m_var_names;
    std::map<std::string, int> m_alloca;

    std::string m_loopBreak;
    std::string m_loopContinue;

};

#endif // ENVIRONMENT_H