#include "Environment.h"

ErrorHandler* Environment::m_errorHandler = nullptr;
std::vector<Environment*> Environment::m_stack;
size_t Environment::m_block_counter = 0;
size_t Environment::m_enum_counter = 0;
std::set<std::string> Environment::m_static_strings;
std::vector<std::string> Environment::m_var_names;