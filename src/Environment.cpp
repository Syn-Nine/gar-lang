#include "Environment.h"

ErrorHandler* Environment::m_errorHandler = nullptr;
std::vector<Environment*> Environment::m_stack;
size_t Environment::m_block_counter = 0;
std::map<std::string, size_t> Environment::m_enum_map;
std::map<int, std::string> Environment::m_enumReflectMap;
size_t Environment::m_enum_counter = 0;
std::set<std::string> Environment::m_static_strings;
std::map<std::string, std::vector<Environment::var_s>> Environment::m_local_alloca;
std::vector<Environment::var_s> Environment::m_global_alloca;
std::map<std::string, size_t> Environment::m_globals;
std::set<std::string> Environment::m_symbols;
std::map<std::string, Environment::func_s> Environment::m_functions;