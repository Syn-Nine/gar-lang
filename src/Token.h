#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <map>
#include <stdint.h>

enum TokenTypeEnum
{
    // high level language tokens
    // ------------------------------------------------------------------------
    
    // single character tokens.
    TOKEN_COMMA,
    TOKEN_MINUS,
    TOKEN_HAT,
    TOKEN_PLUS,
    TOKEN_SLASH,
    TOKEN_STAR,
    TOKEN_BANG,
    TOKEN_PERCENT,
    TOKEN_EQUAL,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,

    /*TOKEN_COLON,
    TOKEN_SEMI_COLON,
    TOKEN_APOSTROPHE,
    */

    // two character tokens
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_OR,
    TOKEN_AND,
    TOKEN_DOT_DOT,
    TOKEN_DOT_DOT_EQUAL,

    /*TOKEN_AT,
    TOKEN_AT_AT,
    TOKEN_BANG_BANG,
    TOKEN_EM_DASH,
    TOKEN_HAT_BANG,
    TOKEN_MINUS_BANG,
    TOKEN_PLUS_BANG,
    TOKEN_SLASH_BANG,
    TOKEN_STAR_BANG,
    */


    // literals
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_INTEGER,
    TOKEN_FLOAT,

    // keywords
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_PI,
    //
    TOKEN_AS,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    //TOKEN_ENUM,
    TOKEN_FOR,
    TOKEN_IF,
    TOKEN_IN,
    TOKEN_ELSE,
    TOKEN_LOOP,
    TOKEN_PRINT,
    TOKEN_PRINTLN,
    TOKEN_VAR,
    TOKEN_VAR_INT,
    TOKEN_VAR_FLOAT,
    TOKEN_VAR_STRING,
    TOKEN_WHILE,
    /*
    TOKEN_ABS,
    TOKEN_ASSERT,
    TOKEN_CALL,
    TOKEN_ENUM,
    TOKEN_FILELINE,
    TOKEN_INCLUDE,
    TOKEN_INPUT,
    TOKEN_MAX,
    TOKEN_MIN,
    TOKEN_NEG,
    TOKEN_NOOP,
    TOKEN_RAND,
    TOKEN_RAND_RANGE,
    TOKEN_RETURN,
    TOKEN_SRAND,
    TOKEN_TOCHAR,
    //
    TOKEN_DEG2RAD,
    TOKEN_RAD2DEG,
    TOKEN_SIN,
    TOKEN_COS,
    TOKEN_TAN,
    TOKEN_ASIN,
    TOKEN_ACOS,
    TOKEN_ATAN,
    TOKEN_ATAN2,
    //
    TOKEN_CEIL,
    TOKEN_CLAMP,
    TOKEN_EXP,
    TOKEN_FLOOR,
    TOKEN_FRACT,
    TOKEN_LOG,
    TOKEN_LOG10,
    TOKEN_MIX,
    TOKEN_POW,
    TOKEN_ROUND,
    TOKEN_SQRT,
    TOKEN_SGN,
    //
    */

    // low level language tokens
    // ------------------------------------------------------------------------
    TOKEN_CALL,
    TOKEN_CAST_FLOAT,
    TOKEN_CAST_INT,
    TOKEN_CAST_STRING,
    TOKEN_INV,
    TOKEN_JMP,
    TOKEN_LABEL,
    TOKEN_LOAD_BOOL,
    TOKEN_LOAD_FLOAT,
    TOKEN_LOAD_INT,
    TOKEN_LOAD_VAR,
    TOKEN_LOAD_STRING,
    TOKEN_NEG,
    TOKEN_STORE_VAR,
    TOKEN_PUSH_SCRATCH_PTR,
    TOKEN_POP_SCRATCH_PTR,
    TOKEN_PRINT_BLANK,


    TOKEN_NOOP,
    TOKEN_LN,
    
    //
    TOKEN_END_OF_FILE,
    TOKEN_INVALID,
};

class Token
{
public:
    Token() = delete;
    Token(TokenTypeEnum type, int line, std::string filename)
    {
        m_type = type;
        m_lexeme = m_token2lex[type];
        m_line = line;
        m_filename = filename;
    }
    
    Token(TokenTypeEnum type, std::string lexeme, int line, std::string filename)
    {
        m_type = type;
        m_lexeme = lexeme;
        m_line = line;
        m_filename = filename;
    }

    Token(TokenTypeEnum type, std::string lexeme, std::string value, int line, std::string filename)
    {
        m_type = type;
        m_lexeme = lexeme;
        m_stringValue = value;
        m_line = line;
        m_filename = filename;
    }

    Token(TokenTypeEnum type, std::string lexeme, int32_t ival, double dval, int line, std::string filename)
    {
        m_type = type;
        m_lexeme = lexeme;
        m_intValue = ival;
        m_doubleValue = dval;
        m_line = line;
        m_filename = filename;
    }

    inline TokenTypeEnum GetType() { return m_type; }
    inline std::string Lexeme() { return m_lexeme; }
    inline double DoubleValue() { return m_doubleValue; }
    inline int32_t IntValue() { return m_intValue; }
    inline std::string StringValue() { return m_stringValue; }
    inline int Line() { return m_line; }
    inline std::string Filename() { return m_filename; }
    inline bool IsInteger() { return TOKEN_INTEGER == m_type; }
    inline bool IsFloat() { return TOKEN_FLOAT == m_type; }

    static std::string Dump(std::vector<Token>& tokens);
    static void Initialize();
    static std::map<std::string, TokenTypeEnum> GetKeywords();

    static TokenTypeEnum Lex2Token(std::string lex);
    static std::string Token2Lex(TokenTypeEnum token);

private:

    static void MapTokenLex(TokenTypeEnum token, std::string lex);


    TokenTypeEnum m_type;
    std::string m_lexeme;
    int m_line;
    double m_doubleValue;
    int32_t m_intValue;
    std::string m_stringValue;
    std::string m_filename;

    static std::map<TokenTypeEnum, std::string> m_token2lex;
    static std::map<std::string, TokenTypeEnum> m_lex2token;
};

typedef std::vector<Token> TokenList;
typedef TokenList Bytecode;


static void Append(Bytecode& lhs, Bytecode rhs)
{
    for (size_t i = 0; i < rhs.size(); ++i)
    {
        lhs.push_back(rhs[i]);
    }
}

static void Push(Bytecode& lhs, Token rhs)
{
    lhs.push_back(rhs);
}

static void PushLn(Bytecode& lhs)
{
    Push(lhs, Token(TOKEN_LN, 0, ""));
}

static void PushJmp(Bytecode& lhs, std::string label)
{
    Push(lhs, Token(TOKEN_JMP, 0, ""));
    Push(lhs, Token(TOKEN_IDENTIFIER, label, 0, ""));
    PushLn(lhs);
}

static void PushLabel(Bytecode& lhs, std::string label)
{
    Push(lhs, Token(TOKEN_LABEL, label, 0, ""));
    PushLn(lhs);
}

static void PushNoop(Bytecode& lhs)
{
    Push(lhs, Token(TOKEN_NOOP, 0, ""));
}

#endif // TOKEN_H