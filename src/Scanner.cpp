#include "Scanner.h"

size_t Scanner::m_linecount = 0;

Scanner::Scanner(const char* buf,
	ErrorHandler* errorHandler,
	const char* filename,
	size_t line
)
{
	m_buffer = std::string(buf);
	m_length = m_buffer.length();
	m_current = 0;
	m_start = 0;
	m_line = line;
	m_errorHandler = errorHandler;
	m_filename = filename;
    m_keywordList = Token::GetKeywords();
}

void Scanner::AddToken(TokenTypeEnum type)
{
	std::string lexeme = m_buffer.substr(m_start, m_current - m_start);
	m_tokens.push_back(Token(type, lexeme, m_line, m_filename));
}

void Scanner::AddToken(TokenTypeEnum type, std::string value)
{
	std::string lexeme = m_buffer.substr(m_start, m_current - m_start);
	m_tokens.push_back(Token(type, lexeme, value, m_line, m_filename));
}

void Scanner::AddToken(TokenTypeEnum type, int32_t value)
{
	std::string lexeme = m_buffer.substr(m_start, m_current - m_start);
	m_tokens.push_back(Token(type, lexeme, value, 0, m_line, m_filename));
}

void Scanner::AddToken(TokenTypeEnum type, double value)
{
	std::string lexeme = m_buffer.substr(m_start, m_current - m_start);
	m_tokens.push_back(Token(type, lexeme, 0, value, m_line, m_filename));
}

char Scanner::Advance()
{
	m_current++;
	return m_buffer[m_current - 1];
}

void Scanner::Identifier()
{
	while (true)
	{
		if (IsAlphaNumeric(Peek()))
		{
			Advance();
		}
		else if (IsColon(Peek()) && IsColon(PeekNext()))
		{
			// standard library namespacing
			Advance();
			Advance();
		}
		else if (IsColon(Peek()) && IsAlphaNumeric(PeekNext()))
		{
			// user namespacing
			Advance();
		}
		else
		{
			break;
		}
	}

	std::string text = m_buffer.substr(m_start, m_current - m_start);

	TokenTypeEnum type = TOKEN_IDENTIFIER;
	if (m_keywordList.count(text)) type = m_keywordList.at(text);

	AddToken(type);
}

bool Scanner::IsAlpha(char c)
{
	if (c >= 'A' && c <= 'Z') return true;
	if (c >= 'a' && c <= 'z') return true;
	if (c == '_') return true;
	return false;
}

bool Scanner::IsAlphaNumeric(char c)
{
	return (IsAlpha(c) || IsDigit(c));
}

bool Scanner::IsDigit(char c)
{
	if (c >= '0' && c <= '9') return true;
	return false;
}

bool Scanner::IsColon(char c)
{
	return c == ':';
}

bool Scanner::IsAtEnd()
{
	if (m_current >= m_length) return true;
	return false;
}

bool Scanner::Match(char c)
{
	if (IsAtEnd()) return false;
	if (m_buffer.at(m_current) != c) return false;

	Advance();
	return true;
}

void Scanner::Number()
{
	while (IsDigit(Peek())) { Advance(); }

	if ('.' == Peek() && IsDigit(PeekNext()))
	{
		Decimal();
	}
	else
	{
		std::string t = m_buffer.substr(m_start, m_current - m_start);
		AddToken(TOKEN_INTEGER, int32_t(std::stoi(t)));
	}
}

void Scanner::Decimal()
{
	// consume . and continue;
	Advance();
	while (IsDigit(Peek())) { Advance(); }

	std::string t = m_buffer.substr(m_start, m_current - m_start);
	AddToken(TOKEN_FLOAT, std::stod(t));
}

char Scanner::Peek()
{
	if (IsAtEnd()) return '\0';
	return m_buffer[m_current];
}

char Scanner::PeekNext()
{
	if (m_current + 1 < m_length) return m_buffer[m_current + 1];
	return '\0';
}

TokenList Scanner::ScanTokens()
{
	while (!IsAtEnd())
	{
		m_start = m_current;
		ScanToken();
	}

	m_tokens.push_back(Token(TOKEN_END_OF_FILE, "", m_line, m_filename));
	return m_tokens;
}

void Scanner::ScanToken()
{
	char c = Advance();

	char tmp[2];
	sprintf(tmp, "%c", c);
	std::string unexpected_character = "Unexpected character `" + std::string(tmp) + "`.";

	switch (c)
	{
	// single character tokens
	/*case ':': AddToken(TOKEN_COLON); break;
	case ';': AddToken(TOKEN_SEMI_COLON); break;
	case '\'': AddToken(TOKEN_APOSTROPHE); break;*/
	case '(': AddToken(TOKEN_LEFT_PAREN); break;
	case ')': AddToken(TOKEN_RIGHT_PAREN); break;
    case '{': AddToken(TOKEN_LEFT_BRACE); break;
    case '}': AddToken(TOKEN_RIGHT_BRACE); break;
    case '^': AddToken(TOKEN_HAT); break;
    case '%': AddToken(TOKEN_PERCENT); break;
    case '+': AddToken(TOKEN_PLUS); break;
    case '*': AddToken(TOKEN_STAR); break;

	case '-':
		if (IsDigit(Peek()))
		{
			Number();
		}
		else
		{
			AddToken(TOKEN_MINUS);
		}
		break;

	// two character tokens
	case '=': AddToken(Match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL); break;
    case '!': AddToken(Match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG); break;
    case '>': AddToken(Match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER); break;
    case '<': AddToken(Match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS); break;
    case '&': Match('&') ? AddToken(TOKEN_AND) : m_errorHandler->Error(m_filename, m_line, unexpected_character); break;
    case '|': Match('|') ? AddToken(TOKEN_OR) : m_errorHandler->Error(m_filename, m_line, unexpected_character); break;

	// comments or divide
	case '/':
		if (Match('/'))
		{
			while (Peek() != '\n' && !IsAtEnd()) { Advance(); }
		}
		else if (Match('*'))
		{
			while (true)
			{
				if (Peek() == '\n')
				{
					m_linecount++;
					m_line++;
				}
				if (Peek() == '*' && PeekNext() == '/')
				{
					Advance(); Advance(); break;
				}
				else if (IsAtEnd())
				{
					m_errorHandler->Error(m_filename, m_line, "Unexpected end of file after block comment.");
					break;
				}
				Advance();
			}
		}
		else
		{
			AddToken(TOKEN_SLASH); break;
		}
		break;

	// whitespace
	case ' ':
	case '\t':
	case '\r':
		break;

	case '\n':
		m_linecount++;
		m_line++;
		break;

	// string literals
	case '"': ScanString(); break;

	default:
		if (IsDigit(c))
		{
			Number();
			break;
		}
		if (IsAlpha(c))
		{
			Identifier();
			break;
		}
		m_errorHandler->Error(m_filename, m_line, unexpected_character);
	}
}

void Scanner::ScanString()
{
	char c = Peek();
	while (c != '"' && !IsAtEnd())
	{
		if (c == '\n')
		{
			m_linecount++;
			m_line++;
		}
		Advance();
		c = Peek();
	}

	if (IsAtEnd())
	{
		m_errorHandler->Error(m_filename, m_line, "Unexpected end of file.");
		return;
	}

	Advance();

	std::string text = m_buffer.substr(m_start + 1, m_current - m_start - 2);
	AddToken(TOKEN_STRING, text);
}
