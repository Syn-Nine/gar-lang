#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "ErrorHandler.h"
#include "Token.h"

#include <set>
#include <string>

class Environment {

public:
    Environment()
    {
        m_parent = nullptr;
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
        std::string lex = token.Lexeme();
        if (0 == m_alloca.count(lex))
        {
            Error(token, "Variable not found in environment.");
            return 0;
        }
        return m_alloca.at(lex);
    }

    std::string NewLabel(std::string prefix)
    {
        return prefix + "_" + std::to_string(m_block_counter++);
    }

    static void RegisterErrorHandler(ErrorHandler* eh)
    {
        m_errorHandler = eh;
    }

    void Error(Token token, const std::string& err)
    {
        m_errorHandler->Error(token.Filename(), token.Line(), "at '" + token.Lexeme() + "'", err);
    }


private:

    static ErrorHandler* m_errorHandler;
    static std::vector<Environment*> m_stack;
    static size_t m_block_counter;
    static std::set<std::string> m_static_strings;

    Environment* m_parent;
    std::vector<std::string> m_var_names;
    std::map<std::string, int> m_alloca;
};

#endif // ENVIRONMENT_H