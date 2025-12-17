#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>

#include "Utilities.h"

class ErrorHandler
{
public:
	void Clear()
	{
		if (!m_errorList.empty()) m_errorList.clear();
	}

	void Error(std::string filename, int line, std::string text)
	{
		if (m_errorList.size() > 10) return;

		m_errorList.push_back(error_struct(filename, line, text));
		
		if (11 == m_errorList.size())
			m_errorList.push_back(error_struct(filename, 0, "Error limit exceeded."));
	}

	void Error(std::string filename, int line, std::string at, std::string text)
	{
		if (m_errorList.size() > 10) return;

		m_errorList.push_back(error_struct(filename, line, at + ": " + text));

		if (11 == m_errorList.size())
			m_errorList.push_back(error_struct(filename, 0, "Error limit exceeded."));
	}

	bool HasErrors()
	{
		if (m_errorList.empty()) return false;
		return true;
	}

	void Print()
	{
		printf("\n");
		for (auto& e : m_errorList)
		{
			printf("File: %s, Line %d %s : %s\n", e.filename.c_str(), e.line, ToHex2(e.line).c_str(), e.text.c_str());
		}
	}

private:
	struct error_struct
	{
		int line;
		std::string text;
		std::string filename;
		error_struct(std::string inFilename, int inLine, std::string inText) : line(inLine), text(inText), filename(inFilename) {}
	};

	std::vector<error_struct> m_errorList;
};

#endif // ERROR_HANDLER_H