#include "Compiler.h"
#include "Scanner.h"

#include <cstdarg>

ErrorHandler* Compiler::m_errorHandler = nullptr;

Token Compiler::Advance()
{
    if (!IsAtEnd()) m_current++;
    return Previous();
}

bool Compiler::Check(TokenTypeEnum tokenType)
{
    if (IsAtEnd()) return false;
    return Peek().GetType() == tokenType;
}

bool Compiler::CheckNext(TokenTypeEnum tokenType)
{
    if (IsAtEnd()) return false;
    if (m_current + 1 == m_tokenList.size()) return false;
    TokenTypeEnum next = m_tokenList.at(m_current + 1).GetType();
    if (TOKEN_END_OF_FILE == next) return false;
    return next == tokenType;
}

bool Compiler::CheckNextNext(TokenTypeEnum tokenType)
{
    if (IsAtEnd()) return false;
    if (m_current + 1 == m_tokenList.size()) return false;
    if (m_current + 2 == m_tokenList.size()) return false;
    TokenTypeEnum next = m_tokenList.at(m_current + 2).GetType();
    if (TOKEN_END_OF_FILE == next) return false;
    return next == tokenType;
}

bool Compiler::Consume(TokenTypeEnum tokenType, std::string err)
{
    if (Check(tokenType))
    {
        Advance();
        return true;
    }
    else
    {
        Error(Peek(), "Compiler Error: " + err);
        return false;
    }
}

void Compiler::Error(Token token, const std::string& err)
{
    if (token.GetType() == TOKEN_END_OF_FILE)
    {
        m_errorHandler->Error(Peek().Filename(), Peek().Line(), "at end", err);
    }
    else
    {
        m_errorHandler->Error(Peek().Filename(), Peek().Line(), "at '" + token.Lexeme() + "'", err);
    }
}

void Compiler::Include()
{
    //if (!Consume(TOKEN_STRING, "Expected filename after include.")) return;

    //std::string filename = Previous().StringValue();

    //std::string namespc;

    ///*if (Match(1, TOKEN_AS))
    //{
    //    if (!Consume(TOKEN_IDENTIFIER, "Expected identifier after as.")) return;
    //    namespc = Previous().Lexeme();
    //}*/

    //if (!Consume(TOKEN_SEMICOLON, "Expected ';' after include.")) return;

    //// check if already included
    //if (0 != m_includes.count(filename))
    //{
    //    Error(Previous(), filename + "Already included in project.");
    //    return;
    //}

    ////int incLine = Previous().Line();

    ////printf("Including file... %s\n", filename.c_str());

    //std::ifstream f;
    //f.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

    //if (!f.is_open())
    //{
    //    printf("Failed to open file: '%s'\n", filename.c_str());
    //    return;
    //}

    //const std::size_t n = f.tellg();
    //char* buffer = new char[n + 1];
    //buffer[n] = '\0';

    //f.seekg(0, std::ios::beg);
    //f.read(buffer, n);
    //f.close();

    //const char* fname = filename.c_str();
    //Scanner scanner(buffer, m_errorHandler, fname);
    //TokenList tokens = scanner.ScanTokens();

    //if (tokens.size() > 1)
    //{
    //    /*if (!namespc.empty())
    //    {
    //        // wrap tokens in a namespace
    //        TokenList t;
    //        t.push_back(Token(TOKEN_NAMESPACE_PUSH, namespc, incLine, fname));
    //        t.insert(t.begin() + 1, tokens.begin(), tokens.begin() + tokens.size() - 1);
    //        t.push_back(Token(TOKEN_NAMESPACE_POP, namespc, incLine, fname));
    //        t.push_back(Token(TOKEN_END_OF_FILE, "", incLine, fname));
    //        tokens = t;
    //    }*/

    //    m_tokenList.insert(m_tokenList.begin() + m_current, tokens.begin(), tokens.begin() + tokens.size() - 1);
    //}
}

bool Compiler::IsAtEnd()
{
    return Peek().GetType() == TOKEN_END_OF_FILE;
}

bool Compiler::Match(TokenTypeEnum tokenType)
{
    if (Check(tokenType))
    {
        Advance();
        return true;
    }
    return false;
}

bool Compiler::MatchVar(int count, ...)
{
    std::va_list args;
    va_start(args, count);
    for (size_t i = 0; i < count; ++i)
    {
        TokenTypeEnum t = (TokenTypeEnum)(va_arg(args, int));

        if (Check(t))
        {
            Advance();
            va_end(args);
            return true;
        }
    }

    va_end(args);
    return false;
}


Token Compiler::Peek()
{
    return m_tokenList.at(m_current);
}

Token Compiler::Previous()
{
    return m_tokenList.at(m_current - 1);
}



void Compiler::Append(IRCode& lhs, IRCode rhs)
{
    for (size_t i = 0; i < rhs.size(); ++i)
    {
        lhs.push_back(rhs[i]);
    }
}

void Compiler::Push(IRCode& lhs, Token rhs)
{
    lhs.push_back(rhs);
}

void Compiler::PushLn(IRCode& lhs)
{
    Push(lhs, Token(TOKEN_LN, 0, ""));
}

void Compiler::PushJmp(IRCode& lhs, std::string label)
{
    Push(lhs, Token(TOKEN_JMP, 0, ""));
    Push(lhs, Token(TOKEN_IDENTIFIER, label, 0, ""));
    PushLn(lhs);
}

void Compiler::PushLabel(IRCode& lhs, std::string label)
{
    Push(lhs, Token(TOKEN_LABEL, label, 0, ""));
    PushLn(lhs);
}

void Compiler::PushNoop(IRCode& lhs)
{
    Push(lhs, Token(TOKEN_NOOP, 0, ""));
    PushLn(lhs);
}
/*
void Compiler::PushBlockStart(IRCode& lhs, Token oper)
{
    Push(lhs, Token(TOKEN_BLOCK_START, oper.Line(), oper.Filename()));
    PushLn(lhs);
}

void Compiler::PushBlockEnd(IRCode& lhs, Token oper)
{
    Push(lhs, Token(TOKEN_BLOCK_END, oper.Line(), oper.Filename()));
    PushLn(lhs);
}*/