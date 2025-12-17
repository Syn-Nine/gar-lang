#ifndef SCANNER_H
#define SCANNER_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <stdint.h>

#include "Token.h"
#include "ErrorHandler.h"

class Scanner
{
public:
	Scanner() = delete;
	Scanner(const char* buf,
		ErrorHandler* errorHandler,
		const char* filename,
		size_t line
	);

	static size_t LineCount() { return m_linecount; }

	TokenList ScanTokens();

private:
	void AddToken(TokenTypeEnum type);
	void AddToken(TokenTypeEnum type, std::string value);
	void AddToken(TokenTypeEnum type, int32_t value);
	void AddToken(TokenTypeEnum type, double value);

	char Advance();
	void Decimal();
    void Enum();
	void Identifier();
	bool IsAlpha(char c);
	bool IsAlphaNumeric(char c);
	bool IsAtEnd();
	bool IsDigit(char c);
	bool IsColon(char c);
	bool Match(char c);
	void Number();
	char Peek();
	char PeekNext();
	void ScanString();
	void ScanToken();

	std::string m_buffer;
	int m_length;
	int m_current;
	int m_start;
	int m_line;
	std::string m_filename;

	static size_t m_linecount;

	TokenList m_tokens;
	ErrorHandler* m_errorHandler;
	std::map<std::string, TokenTypeEnum> m_keywordList;
};


#endif // SCANNER_H