#include "Assembler.h"


void Assembler::SetInstInt(int addr, int val)
{
    uint8_t a = val & 0xFF; val = val >> 8;
    uint8_t b = val & 0xFF;

    m_code[addr + 0] = b;
    m_code[addr + 1] = a;
}

void Assembler::PushInst(uint8_t inst)
{
    m_code[m_inst_ptr] = inst;
    m_inst_ptr++;
}


void Assembler::PushInstBool(bool val)
{
    if (val) { m_code[m_inst_ptr++] = 1; }
    else { m_code[m_inst_ptr++] = 0; }
}

void Assembler::PushInstInt16(int val)
{
    uint8_t a = val & 0xFF; val = val >> 8;
    uint8_t b = val & 0xFF;

    m_code[m_inst_ptr++] = b;
    m_code[m_inst_ptr++] = a;
}

void Assembler::PushInstInt(int val)
{
    uint8_t a = val & 0xFF; val = val >> 8;
    uint8_t b = val & 0xFF; val = val >> 8;
    uint8_t c = val & 0xFF; val = val >> 8;
    uint8_t d = val & 0xFF;

    m_code[m_inst_ptr++] = d;
    m_code[m_inst_ptr++] = c;
    m_code[m_inst_ptr++] = b;
    m_code[m_inst_ptr++] = a;
}

void Assembler::StartComment(std::string& row)
{
    int pad = 32 - row.length();
    if (pad > 0)
    {
        for (int i = 0; i < pad; ++i) row.append(" ");
    }
    row.append("// ");
}

void Assembler::PadComment(std::string& row, int len)
{
    int pad = len - row.length();
    if (pad > 0)
    {
        for (int i = 0; i < pad; ++i) row.append(" ");
    }
}


void Assembler::Error(const std::string& err)
{
    m_errorHandler->Error("Assembler", m_inst_ptr - 1, err);
}

