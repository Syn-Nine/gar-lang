#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "VM.h"
#include "Environment.h"
#include "ErrorHandler.h"
#include "Token.h"
#include "Utilities.h"

#include <string>

class Assembler
{
public:
    Assembler() = delete;

    Assembler(IRCode ircode, Environment* environment, ErrorHandler* errorHandler)
    {
        m_ircode = ircode;
        m_errorHandler = errorHandler;
        m_env = environment;
        m_code = (uint8_t*)malloc(VM::MEM_BLOCK_SZ);
    }

    ~Assembler()
    {
        free(m_code);
    }

    uint8_t* Assemble();

    size_t GetHeapAddr() { return m_heap_start; }
    size_t GetEntryAddr() { return m_main_entry; }
    std::string Dump();

private:

    void ProcessIR(IRCode codircodee);

    // push, pop, peak, update

    void SetInstInt(int addr, int val);
    void PushInst(uint8_t inst);
    void PushInstBool(bool val);
    void PushInstInt16(int val);
    void PushInstInt(int val);
    
    void StartComment(std::string& row);
    void PadComment(std::string& row, int len);

    void Error(const std::string& err);

    IRCode m_ircode;
    ErrorHandler* m_errorHandler;
    Environment* m_env;
    std::string m_asm;
    int m_comment_param_offset;

    struct pp_jmps_s
    {
        std::string to_label;
        size_t from_inst;
    };

    std::vector<pp_jmps_s> m_pp_jmps;
    std::map<std::string, size_t> m_pp_labels;

    std::map<std::string, size_t> m_string_addrs;
    std::map<std::string, std::string> m_function_entries;

    size_t m_heap_start;
    uint16_t m_inst_ptr;
    uint16_t m_main_entry;
    uint8_t* m_code;

};





#endif // ASSEMBLER_H