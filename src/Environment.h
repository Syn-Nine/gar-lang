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
        if (!m_stack.empty())
        {
            e->m_parent = m_stack.back();
            e->m_parent_function = e->m_parent->m_parent_function;
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

    void SetParentFunction(std::string name)
    {
        m_parent_function = name;
    }

    bool IsGlobal() { return !m_parent; }

    struct func_s
    {
        int arity;
        std::string entry;
        std::string tail;
        IRCode proto;
        IRCode body;
    };

    void DefineFunction(std::string lexeme, int arity, std::string entry, std::string tail, IRCode proto, IRCode body)
    {
        m_symbols.insert(lexeme);

        func_s func;
        func.arity = arity;
        func.entry = entry;
        func.tail = tail;
        func.proto = proto;
        func.body = body;

        m_functions.insert(std::make_pair(lexeme, func));
    }

    std::map<std::string, func_s>& GetFunctions() { return m_functions; }

    bool IsFunctionDefined(std::string lexeme) { return 0 != m_functions.count(lexeme); }

    int GetFunctionArity(std::string lexeme)
    {
        return m_functions.at(lexeme).arity;
    }

    struct var_s
    {
        std::string lexeme;
        bool is_const;
    };

    void DefineGlobalVariable(std::string lexeme, bool is_const)
    {
        // globals
        if (0 == m_globals.count(lexeme))
        {
            // offset on the global stack
            int idx = m_global_alloca.size();
            m_global_alloca.push_back({ lexeme, is_const });
            m_globals.insert(std::make_pair(lexeme, idx));
        }
    }

    void DefineLocalVariable(std::string lexeme, bool is_const)
    {
        // frame locals
        if (0 == m_locals.count(lexeme))
        {
            // relative offset on the frame stack
            int idx = 1 + m_local_alloca[m_parent_function].size();
            m_local_alloca[m_parent_function].push_back({ lexeme, is_const });
            m_locals.insert(std::make_pair(lexeme, -idx));
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

    std::vector<Environment::var_s>& GetGlobals() {
        return m_global_alloca;
    }

    std::vector<Environment::var_s>& GetLocals(std::string parent_function) {
        return m_local_alloca[parent_function];
    }

    int GetEnumValue(std::string name)
    {
        if (0 == m_enum_map.count(name))
        {
            m_enum_counter++;
            m_enum_map.insert(std::make_pair(name, m_enum_counter));
            m_enumReflectMap.insert(std::make_pair(m_enum_counter, name));
        }
        return m_enum_map.at(name);
    }

    std::string GetEnumAsString(int val)
    {
        if (0 == m_enumReflectMap.count(val)) return "<Enum Invalid>";
        return m_enumReflectMap.at(val);
    }

    size_t GetVariable(Token token, bool allow_const)
    {
        if (m_stack.empty())
        {
            Error(token, "Environment stack is empty.");
            return 0;
        }

        return m_stack.back()->GetVariable_Recursive(token, allow_const);
    }

    std::string NewLabel(std::string prefix)
    {
        return prefix + "_" + std::to_string(m_block_counter++);
    }

    std::string GetParentLoopBreak()
    {
        if (!m_parent) return "";
        if (!m_parent->m_loopBreak.empty()) return m_parent->m_loopBreak;
        return m_parent->GetParentLoopBreak();
    }

    std::string GetParentLoopContinue()
    {
        if (!m_parent) return "";
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

    static bool SymbolDefined(std::string name)
    {
        return 0 != m_symbols.count(name);
    }

private:

    Environment()
    {
        m_parent = nullptr;
    }

    size_t GetVariable_Recursive(Token token, bool allow_const)
    {
        std::string lex = token.Lexeme();
        if (0 != m_locals.count(lex))
        {
            int idx = m_locals.at(lex);
            if (!allow_const && idx >= 0 && m_local_alloca[m_parent_function].at(idx).is_const)
            {
                Error(token, "Cannot overwrite constant local.");
            }
            return idx;
        }
        if (m_parent) return m_parent->GetVariable_Recursive(token, allow_const);
        if (0 != m_globals.count(lex))
        {
            int idx = m_globals.at(lex);
            if (!allow_const && m_global_alloca.at(idx).is_const)
            {
                Error(token, "Cannot overwrite constant global.");
            }
            return idx;
        }

        Error(token, "Variable not found in environment.");
        return 0;
    }

    static ErrorHandler* m_errorHandler;
    static std::vector<Environment*> m_stack;
    static size_t m_block_counter;
    static std::set<std::string> m_static_strings;
    static std::map<std::string, size_t> m_enum_map;
    static size_t m_enum_counter;
    static std::map<int, std::string> m_enumReflectMap;

    Environment* m_parent;
    static std::map<std::string, std::vector<Environment::var_s>> m_local_alloca;
    std::map<std::string, size_t> m_locals;

    static std::vector<Environment::var_s> m_global_alloca;
    static std::map<std::string, size_t> m_globals;
    
    static std::set<std::string> m_symbols;
    static std::map<std::string, func_s> m_functions;

    std::string m_loopBreak;
    std::string m_loopContinue;
    std::string m_parent_function;
};

#endif // ENVIRONMENT_H