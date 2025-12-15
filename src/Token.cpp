#include "Token.h"

std::map<TokenTypeEnum, std::string> Token::m_token2lex;
std::map<std::string, TokenTypeEnum> Token::m_lex2token;


std::map<std::string, TokenTypeEnum> Token::GetKeywords()
{
    return m_lex2token;
}

void Token::Initialize()
{
    // single character tokens.
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

    // two character tokens
    MapTokenLex(TOKEN_BANG_EQUAL, "!=");
    MapTokenLex(TOKEN_EQUAL_EQUAL, "==");
    MapTokenLex(TOKEN_GREATER, ">");
    MapTokenLex(TOKEN_GREATER_EQUAL, ">=");
    MapTokenLex(TOKEN_LESS, "<");
    MapTokenLex(TOKEN_LESS_EQUAL, "<=");
    MapTokenLex(TOKEN_AND, "&&");
    MapTokenLex(TOKEN_OR, "||");
    
    // literals
    MapTokenLex(TOKEN_IDENTIFIER, "");
    MapTokenLex(TOKEN_FLOAT, "");
    MapTokenLex(TOKEN_INTEGER, "");
    MapTokenLex(TOKEN_STRING, "");

    /*
    MapTokenLex(TOKEN_COLON, ":");
    MapTokenLex(TOKEN_SEMI_COLON, ";");
    MapTokenLex(TOKEN_APOSTROPHE, "'asdfasd'");
    

    MapTokenLex(TOKEN_AT, "@");
    MapTokenLex(TOKEN_AT_AT, "@@");
    MapTokenLex(TOKEN_BANG_BANG, "!!");
    MapTokenLex(TOKEN_EM_DASH, "--");
    MapTokenLex(TOKEN_HAT_BANG, "^!");
    MapTokenLex(TOKEN_MINUS_BANG, "-!");
    MapTokenLex(TOKEN_PLUS_BANG, "+!");
    MapTokenLex(TOKEN_SLASH_BANG, "/!");
    MapTokenLex(TOKEN_STAR_BANG, "*!");
    
    */
    
    // keywords
    MapTokenLex(TOKEN_TRUE, "true");
    MapTokenLex(TOKEN_FALSE, "false");
    //
    MapTokenLex(TOKEN_PI, "pi");
    //
    MapTokenLex(TOKEN_IF, "if");
    MapTokenLex(TOKEN_ELSE, "else");
    MapTokenLex(TOKEN_PRINT, "print");
    MapTokenLex(TOKEN_PRINTLN, "println");
    MapTokenLex(TOKEN_VAR, "var");

    /*
    MapTokenLex(TOKEN_ABS, "abs");
    MapTokenLex(TOKEN_ASSERT, "assert");
    MapTokenLex(TOKEN_AS_FLOAT, "asfloat");
    MapTokenLex(TOKEN_AS_INT, "asint");
    MapTokenLex(TOKEN_CALL, "call");
    MapTokenLex(TOKEN_ENUM, "enum");
    MapTokenLex(TOKEN_FILELINE, "FILELINE");
    MapTokenLex(TOKEN_INCLUDE, "include");
    MapTokenLex(TOKEN_INPUT, "input");
    MapTokenLex(TOKEN_MAX, "max");
    MapTokenLex(TOKEN_MIN, "min");
    MapTokenLex(TOKEN_NEG, "neg");
    MapTokenLex(TOKEN_NOOP, "noop");
    MapTokenLex(TOKEN_RAND, "rand");
    MapTokenLex(TOKEN_RAND_RANGE, "randrange");
    MapTokenLex(TOKEN_RETURN, "return");
    MapTokenLex(TOKEN_SRAND, "srand");
    MapTokenLex(TOKEN_TOCHAR, "tochar");
    //
    MapTokenLex(TOKEN_PI, "pi");
    MapTokenLex(TOKEN_DEG2RAD, "deg2rad");
    MapTokenLex(TOKEN_RAD2DEG, "rad2deg");
    MapTokenLex(TOKEN_SIN, "sin");
    MapTokenLex(TOKEN_COS, "cos");
    MapTokenLex(TOKEN_TAN, "tan");
    MapTokenLex(TOKEN_ASIN, "asin");
    MapTokenLex(TOKEN_ACOS, "acos");
    MapTokenLex(TOKEN_ATAN, "atan");
    MapTokenLex(TOKEN_ATAN2, "atan2");
    //
    MapTokenLex(TOKEN_CEIL, "ceil");
    MapTokenLex(TOKEN_CLAMP, "clamp");
    MapTokenLex(TOKEN_EXP, "exp");
    MapTokenLex(TOKEN_FLOOR, "floor");
    MapTokenLex(TOKEN_FRACT, "fract");
    MapTokenLex(TOKEN_LOG, "log");
    MapTokenLex(TOKEN_LOG10, "log10");
    MapTokenLex(TOKEN_MIX, "mix");
    MapTokenLex(TOKEN_POW, "pow");
    MapTokenLex(TOKEN_ROUND, "round");
    MapTokenLex(TOKEN_SQRT, "sqrt");
    MapTokenLex(TOKEN_SGN, "sgn");
    //
    MapTokenLex(TOKEN_CONST, "const");
    MapTokenLex(TOKEN_DIM, "dim");
    MapTokenLex(TOKEN_ERASE, "erase");
    MapTokenLex(TOKEN_FILL, "fill");
    MapTokenLex(TOKEN_LEN, "len");
    MapTokenLex(TOKEN_REDIM, "redim");
    MapTokenLex(TOKEN_VAR, "var");
    //
    MapTokenLex(TOKEN_CLEAR, "clr");
    MapTokenLex(TOKEN_P_CLEAR, "pclr");
    MapTokenLex(TOKEN_T_CLEAR, "tclr");
    MapTokenLex(TOKEN_V_CLEAR, "vclr");
    //
    MapTokenLex(TOKEN_DUMP, "dump");
    MapTokenLex(TOKEN_PRINT, "print");
    MapTokenLex(TOKEN_PRINTLN, "println");
    //
    MapTokenLex(TOKEN_BREAK, "break");
    MapTokenLex(TOKEN_CONTINUE, "continue");
    MapTokenLex(TOKEN_FOR, "for");
    MapTokenLex(TOKEN_ENDFOR, "endfor");
    MapTokenLex(TOKEN_ITER, "iter");
    MapTokenLex(TOKEN_ENDITER, "enditer");
    MapTokenLex(TOKEN_LOOP, "loop");
    MapTokenLex(TOKEN_ENDLOOP, "endloop");
    MapTokenLex(TOKEN_I, "i");
    MapTokenLex(TOKEN_J, "j");
    MapTokenLex(TOKEN_K, "k");
    //
    MapTokenLex(TOKEN_IF, "if");
    MapTokenLex(TOKEN_ELSE, "else");
    MapTokenLex(TOKEN_ENDIF, "endif");
    MapTokenLex(TOKEN_AND, "and");
    MapTokenLex(TOKEN_OR, "or");
    MapTokenLex(TOKEN_NOT, "not");
    MapTokenLex(TOKEN_FALSE, "false");
    MapTokenLex(TOKEN_TRUE, "true");
    //
    
    */
    


    // low level language tokens
    MapTokenLex(TOKEN_LABEL, "label");
    MapTokenLex(TOKEN_LOAD_BOOL, "loadb");
    MapTokenLex(TOKEN_LOAD_FLOAT, "loadf");
    MapTokenLex(TOKEN_LOAD_INT, "loadi");
    MapTokenLex(TOKEN_LOAD_VAR, "loadvar");
    MapTokenLex(TOKEN_LOAD_STRING, "loadstr");
    MapTokenLex(TOKEN_INV, "inv");
    MapTokenLex(TOKEN_JMP, "jmp");
    MapTokenLex(TOKEN_NEG, "neg");
    MapTokenLex(TOKEN_STORE_VAR, "storevar");
    
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


void Token::MapTokenLex(TokenTypeEnum token, std::string lex)
{
    m_token2lex.insert(std::make_pair(token, lex));
    m_lex2token.insert(std::make_pair(lex, token));
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