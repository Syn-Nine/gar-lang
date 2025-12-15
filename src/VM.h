#ifndef VM_H
#define VM_H

#include "Token.h"
#include "ErrorHandler.h"

#include <format>
#include <string>

#define RET_PTR m_memory.ret_ptr
#define PARAM_PTR m_memory.param_ptr
#define INST_PTR m_memory.inst_ptr

class Environment;

class VM
{
public:
    VM(TokenList bytecode, Environment* environment, ErrorHandler* errorHandler)
    {
        m_bytecode = bytecode;
        m_errorHandler = errorHandler;
        m_env = environment;
    }

    void Assemble();
    void Execute();

    static const int MEM_BLOCK_WIDTH = 128;
    static const int MEM_BLOCK_HEIGHT = 960;
    static const int MEM_BLOCK_SZ = MEM_BLOCK_WIDTH * MEM_BLOCK_HEIGHT;
    
    struct Memory
    {
        uint32_t ret_ptr;
        uint32_t param_ptr;
        uint16_t inst_ptr;
        uint8_t block[MEM_BLOCK_SZ];
    };

    std::string Dump();

private:

    static const int MEM_INST_SZ = 65536;   // size of instruction block
    static const int MEM_RET_START = MEM_INST_SZ;
    static const int MEM_RET_SZ = 2 * 16;
    static const int MEM_PARAM_START = MEM_RET_START + MEM_RET_SZ;
    static const int MEM_PARAM_SZ = 5 * 16;
    static const int MEM_START_VARS = MEM_PARAM_START + MEM_PARAM_SZ;

    
    static const uint8_t PARAM_INVALID = 0;
    static const uint8_t PARAM_BOOL = 1;
    static const uint8_t PARAM_FLOAT = 2;
    static const uint8_t PARAM_INT = 3;
    static const uint8_t PARAM_STRING = 4;

    void SetInstInt(int addr, int val)
    {
        uint8_t a = val & 0xFF; val = val >> 8;
        uint8_t b = val & 0xFF;

        m_memory.block[addr + 0] = b;
        m_memory.block[addr + 1] = a;
    }

    void PushInst(uint8_t inst)
    {
        m_memory.block[INST_PTR] = inst;
        INST_PTR++;
    }

    void PushInstBool(bool val)
    {
        if (val) { m_memory.block[INST_PTR++] = 1; }
        else { m_memory.block[INST_PTR++] = 0; }
    }

    void PushInstInt16(int val)
    {
        uint8_t a = val & 0xFF; val = val >> 8;
        uint8_t b = val & 0xFF;

        m_memory.block[INST_PTR++] = b;
        m_memory.block[INST_PTR++] = a;
    }

    void PushInstInt(int val)
    {
        uint8_t a = val & 0xFF; val = val >> 8;
        uint8_t b = val & 0xFF; val = val >> 8;
        uint8_t c = val & 0xFF; val = val >> 8;
        uint8_t d = val & 0xFF;

        m_memory.block[INST_PTR++] = d;
        m_memory.block[INST_PTR++] = c;
        m_memory.block[INST_PTR++] = b;
        m_memory.block[INST_PTR++] = a;
    }

    int ReadInstInt16()
    {
        uint8_t* data = m_memory.block;
        int b = data[INST_PTR++];
        int a = data[INST_PTR++];

        int ret = (b << 8) | a;
        return ret;
    }

    int ReadInstInt()
    {
        uint8_t* data = m_memory.block;
        int d = data[INST_PTR++];
        int c = data[INST_PTR++];
        int b = data[INST_PTR++];
        int a = data[INST_PTR++];

        int ret = (d << 24) | (c << 16) | (b << 8) | a;
        return ret;
    }

    uint8_t PopParamType()
    {
        return m_memory.block[PARAM_PTR--];
    }

    bool PopParamBool()
    {
        int val = m_memory.block[PARAM_PTR--];
        if (1 == val) return true;
        return false;
    }

    int PopParamInt()
    {
        uint8_t* data = m_memory.block;
        int a = data[PARAM_PTR--];
        int b = data[PARAM_PTR--];
        int c = data[PARAM_PTR--];
        int d = data[PARAM_PTR--];

        int ret = (d << 24) | (c << 16) | (b << 8) | a;
        return ret;
    }

    void PushParamType(uint8_t type)
    {
        m_memory.block[++PARAM_PTR] = type;
    }

    void PushParamBool(bool val)
    {
        if (val) { m_memory.block[++PARAM_PTR] = 1; }
        else { m_memory.block[++PARAM_PTR] = 0; }
        PushParamType(PARAM_BOOL);
    }

    void PushParamFloat(float fval)
    {
        int val;
        memcpy(&val, &fval, 4);
        uint8_t a = val & 0xFF; val = val >> 8;
        uint8_t b = val & 0xFF; val = val >> 8;
        uint8_t c = val & 0xFF; val = val >> 8;
        uint8_t d = val & 0xFF;

        m_memory.block[++PARAM_PTR] = d;
        m_memory.block[++PARAM_PTR] = c;
        m_memory.block[++PARAM_PTR] = b;
        m_memory.block[++PARAM_PTR] = a;
        PushParamType(PARAM_FLOAT);
    }

    void PushParamInt(int val)
    {
        uint8_t a = val & 0xFF; val = val >> 8;
        uint8_t b = val & 0xFF; val = val >> 8;
        uint8_t c = val & 0xFF; val = val >> 8;
        uint8_t d = val & 0xFF;

        m_memory.block[++PARAM_PTR] = d;
        m_memory.block[++PARAM_PTR] = c;
        m_memory.block[++PARAM_PTR] = b;
        m_memory.block[++PARAM_PTR] = a;
        PushParamType(PARAM_INT);
    }

    void PushParamVar(int idx)
    {
        m_memory.block[++PARAM_PTR] = m_memory.block[MEM_START_VARS + idx + 4];
        m_memory.block[++PARAM_PTR] = m_memory.block[MEM_START_VARS + idx + 3];
        m_memory.block[++PARAM_PTR] = m_memory.block[MEM_START_VARS + idx + 2];
        m_memory.block[++PARAM_PTR] = m_memory.block[MEM_START_VARS + idx + 1];
        m_memory.block[++PARAM_PTR] = m_memory.block[MEM_START_VARS + idx + 0];
    }

    void PopParamVar(int idx)
    {
        m_memory.block[MEM_START_VARS + idx + 0] = m_memory.block[PARAM_PTR--];
        m_memory.block[MEM_START_VARS + idx + 1] = m_memory.block[PARAM_PTR--];
        m_memory.block[MEM_START_VARS + idx + 2] = m_memory.block[PARAM_PTR--];
        m_memory.block[MEM_START_VARS + idx + 3] = m_memory.block[PARAM_PTR--];
        m_memory.block[MEM_START_VARS + idx + 4] = m_memory.block[PARAM_PTR--];
    }


    void StartComment(std::string& row)
    {
        int pad = 32 - row.length();
        if (pad > 0)
        {
            for (int i = 0; i < pad; ++i) row.append(" ");
        }
        row.append("// ");
    }

    void PadComment(std::string& row, int len)
    {
        int pad = len - row.length();
        if (pad > 0)
        {
            for (int i = 0; i < pad; ++i) row.append(" ");
        }
    }

    // Executive
    void LoadBool();
    void LoadInt();
    void LoadFloat();
    void LoadString();
    void LoadVar();
    void StoreVar();
    void PushParamInst4();
    void BinaryOp(TokenTypeEnum oper);
    void AndOr(TokenTypeEnum oper);
    void ComparisonOp(TokenTypeEnum oper);
    void Negate();
    void Invert();
    void Print(bool newline);
    void IfJmp();
    void Jmp();
    
    //

    std::string ToHex1(char val)
    {
        char buf[5];
        snprintf(buf, 5, "0x%02x", val);
        return std::string(buf);
    }

    std::string ToHex2(int val)
    {
        char buf[7];
        snprintf(buf, 7, "0x%04x", val);
        return std::string(buf);
    }

    std::string ToHex4(int val)
    {
        char buf[11];
        snprintf(buf, 11, "0x%08x", val);
        return std::string(buf);
    }

    void Error(const std::string& err);

    TokenList m_bytecode;
    ErrorHandler* m_errorHandler;
    Environment* m_env;
    Memory m_memory;
    std::string m_asm;

    struct pp_jmps_s
    {
        std::string to_label;
        size_t from_inst;
    };

    std::vector<pp_jmps_s> m_pp_jmps;
    std::map<std::string, size_t> m_pp_labels;

};

#endif // VM_H