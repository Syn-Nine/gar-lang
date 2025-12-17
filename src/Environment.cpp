#include "Environment.h"

ErrorHandler* Environment::m_errorHandler = nullptr;
std::vector<Environment*> Environment::m_stack;
size_t Environment::m_block_counter = 0;
size_t Environment::m_enum_counter = 0;
std::set<std::string> Environment::m_static_strings;
std::map<std::string, std::vector<std::string>> Environment::m_local_alloca;
std::vector<std::string> Environment::m_global_alloca;
std::map<std::string, int> Environment::m_globals;