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
    }

    uint8_t* Assemble();

    std::string Dump();

private:

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

    struct pp_jmps_s
    {
        std::string to_label;
        size_t from_inst;
    };

    std::vector<pp_jmps_s> m_pp_jmps;
    std::map<std::string, size_t> m_pp_labels;

    uint16_t m_inst_ptr;
    uint8_t m_code[VM::MEM_BLOCK_SZ];

};





#endif // ASSEMBLER_H