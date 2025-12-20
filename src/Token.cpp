#include "Token.h"

std::map<TokenTypeEnum, std::string> Token::m_token2lex;
std::map<std::string, TokenTypeEnum> Token::m_lex2token;
std::map<std::string, TokenTypeEnum> Token::m_keywords;


std::map<std::string, TokenTypeEnum> Token::GetKeywords()
{
    return m_keywords;
}

void Token::Initialize()
{
    // single character tokens.
    MapTokenLex(TOKEN_COLON, ":");
    MapTokenLex(TOKEN_COMMA, ",");
    MapTokenLex(TOKEN_MINUS, "-");
    MapTokenLex(TOKEN_HAT, "^");
    MapTokenLex(TOKEN_PLUS, "+");
    MapTokenLex(TOKEN_SLASH, "/");
    MapTokenLex(TOKEN_STAR, "*");
    MapTokenLex(TOKEN_BANG, "!");
    MapTokenLex(TOKEN_PERCENT, "%");
    MapTokenLex(TOKEN_EQUAL, "=");
    MapTokenLex(TOKEN_LEFT_PAREN, "(");
    MapTokenLex(TOKEN_RIGHT_PAREN, ")");
    MapTokenLex(TOKEN_LEFT_BRACE, "{");
    MapTokenLex(TOKEN_RIGHT_BRACE, "}");
    MapTokenLex(TOKEN_LEFT_BRACKET, "[");
    MapTokenLex(TOKEN_RIGHT_BRACKET, "]");
    

    // two character tokens
    MapTokenLex(TOKEN_BANG_EQUAL, "!=");
    MapTokenLex(TOKEN_EQUAL_EQUAL, "==");
    MapTokenLex(TOKEN_GREATER, ">");
    MapTokenLex(TOKEN_GREATER_EQUAL, ">=");
    MapTokenLex(TOKEN_LESS, "<");
    MapTokenLex(TOKEN_LESS_EQUAL, "<=");
    MapTokenLex(TOKEN_AND, "&&");
    MapTokenLex(TOKEN_OR, "||");
    MapTokenLex(TOKEN_DOT_DOT, "..");
    MapTokenLex(TOKEN_DOT_DOT_EQUAL, "..=");
    
    // literals
    MapTokenLex(TOKEN_IDENTIFIER, "");
    MapTokenLex(TOKEN_FLOAT, "");
    MapTokenLex(TOKEN_INTEGER, "");
    MapTokenLex(TOKEN_STRING, "");
    MapTokenLex(TOKEN_ENUM, "");


    
    // keywords
    MapTokenLex(TOKEN_TRUE, "true", true);
    MapTokenLex(TOKEN_FALSE, "false", true);
    //
    MapTokenLex(TOKEN_PI, "pi", true);
    //
    MapTokenLex(TOKEN_AS, "as", true);
    MapTokenLex(TOKEN_BREAK, "break", true);
    MapTokenLex(TOKEN_CONTINUE, "continue", true);
    MapTokenLex(TOKEN_CONST, "const", true);
    MapTokenLex(TOKEN_DEF, "def", true);
    MapTokenLex(TOKEN_FOR, "for", true);
    MapTokenLex(TOKEN_IF, "if", true);
    MapTokenLex(TOKEN_IN, "in", true);
    MapTokenLex(TOKEN_ELSE, "else", true);
    MapTokenLex(TOKEN_FILELINE, "FILELINE", true);
    MapTokenLex(TOKEN_INCLUDE, "include", true);
    MapTokenLex(TOKEN_LOOP, "loop", true);
    MapTokenLex(TOKEN_PRINT, "print", true);
    MapTokenLex(TOKEN_PRINTLN, "println", true);
    MapTokenLex(TOKEN_RETURN, "return", true);
    MapTokenLex(TOKEN_VAR, "var", true);
    MapTokenLex(TOKEN_VAR_INT, "int", true);
    MapTokenLex(TOKEN_VAR_FLOAT, "float", true);
    MapTokenLex(TOKEN_VAR_STRING, "string", true);
    MapTokenLex(TOKEN_WHILE, "while", true);



    // low level language tokens
    MapTokenLex(TOKEN_ALLOCA, "alloca");
    MapTokenLex(TOKEN_CALL, "call");
    MapTokenLex(TOKEN_CALL_DEF, "calldef");
    MapTokenLex(TOKEN_CAST_FLOAT, "castf");
    MapTokenLex(TOKEN_CAST_INT, "casti");
    MapTokenLex(TOKEN_CAST_STRING, "caststr");
    MapTokenLex(TOKEN_INV, "inv");
    MapTokenLex(TOKEN_JMP, "jmp");
    MapTokenLex(TOKEN_LABEL, "label");
    MapTokenLex(TOKEN_LOAD_AT, "load@");
    MapTokenLex(TOKEN_LOAD_BOOL, "loadb");
    MapTokenLex(TOKEN_LOAD_FLOAT, "loadf");
    MapTokenLex(TOKEN_LOAD_INT, "loadi");
    MapTokenLex(TOKEN_LOAD_VAR, "loadvar");
    MapTokenLex(TOKEN_LOAD_STRING, "loadstr");
    MapTokenLex(TOKEN_MAKE_LIST, "makelist");
    MapTokenLex(TOKEN_NEG, "neg");
    MapTokenLex(TOKEN_STORE_AT, "store@");
    MapTokenLex(TOKEN_STORE_VAR, "storevar");
    MapTokenLex(TOKEN_PRINT_BLANK, "prntblank");
    MapTokenLex(TOKEN_RET, "ret");
    

    MapTokenLex(TOKEN_NOOP, "noop");
    MapTokenLex(TOKEN_LN, "\n");

    //
    MapTokenLex(TOKEN_END_OF_FILE, "EOF");

    for (size_t i = 0; i < TOKEN_INVALID; ++i)
    {
        if (0 == m_token2lex.count((TokenTypeEnum)i))
        {
            printf("********************************\n");
            printf("Token missing from lex map: %d\n", i);
            printf("********************************\n");
            return;
        }
    }

}


void Token::MapTokenLex(TokenTypeEnum token, std::string lex, bool is_keyword /* = false */)
{
    m_token2lex.insert(std::make_pair(token, lex));
    m_lex2token.insert(std::make_pair(lex, token));
    if (is_keyword) m_keywords.insert(std::make_pair(lex, token));
}


std::string Token::Token2Lex(TokenTypeEnum token)
{
    if (0 == m_token2lex.count(token)) return "<INVALID>";
    return m_token2lex.at(token);
}


TokenTypeEnum Token::Lex2Token(std::string lex)
{
    if (0 == m_lex2token.count(lex)) return TOKEN_INVALID;
    return m_lex2token.at(lex);
}


std::string Token::Dump(std::vector<Token>& tokens)
{
    std::string ret;
    int sz = tokens.size();
    for (int i = 0; i < sz; ++i)
    {
        Token& t = tokens[i];
        switch (t.GetType())
        {
        case TOKEN_NOOP: continue;
        case TOKEN_LABEL: ret.append(t.m_lexeme + ":"); break;
        case TOKEN_IDENTIFIER: ret.append(t.m_lexeme); break;
        case TOKEN_STRING: ret.append("\"" + t.m_stringValue + "\""); break;
        case TOKEN_INTEGER: ret.append(std::to_string(t.m_intValue)); break;
        case TOKEN_FLOAT: ret.append(std::to_string(t.m_doubleValue)); break;
        default:
            ret.append(m_token2lex.at(t.GetType()));
        }
        if (TOKEN_LN != t.GetType() && i < sz - 1) ret.append(" ");
    }
    return ret;
}