#ifndef VM_H
#define VM_H

#include "Token.h"
#include "ErrorHandler.h"

#include <format>
#include <string>
#include <functional>

#define RET_PTR m_memory.ret_ptr
#define PARAM_PTR m_memory.param_ptr
#define INST_PTR m_memory.inst_ptr
#define SCRATCH_PTR m_memory.scratch_ptr

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

    void Initialize();
    void Assemble();
    void Execute();

    static const int MEM_BLOCK_WIDTH = 128;
    static const int MEM_BLOCK_HEIGHT = 960;
    static const int MEM_BLOCK_SZ = MEM_BLOCK_WIDTH * MEM_BLOCK_HEIGHT;
    
    struct Memory
    {
        uint32_t ret_ptr;
        uint32_t param_ptr;
        uint32_t scratch_ptr;
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

    uint8_t PeekInst()
    {
        return m_memory.block[INST_PTR];
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

    uint8_t PeekParamType()
    {
        return m_memory.block[PARAM_PTR];
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

    float PopParamFloat()
    {
        int ival = PopParamInt();
        float fval;
        memcpy(&fval, &ival, 4);
        return fval;
    }
    
    int PeekParamInt()
    {
        uint8_t* data = m_memory.block;
        int a = data[PARAM_PTR];
        int b = data[PARAM_PTR - 1];
        int c = data[PARAM_PTR - 2];
        int d = data[PARAM_PTR - 3];

        int ret = (d << 24) | (c << 16) | (b << 8) | a;
        return ret;
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

    std::string PopParamString()
    {
        int addr = PopParamInt();
        // copy string from arena address
        uint8_t* data = m_memory.block;
        int len = data[addr];
        char buf[256];
        for (int i = 0; i < len; ++i)
        {
            buf[i] = data[addr + i + 1];
        }
        buf[len] = 0;
        return std::string(buf);
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

    void PushParamString(int addr)
    {
        uint8_t a = addr & 0xFF; addr = addr >> 8;
        uint8_t b = addr & 0xFF; addr = addr >> 8;
        uint8_t c = addr & 0xFF; addr = addr >> 8;
        uint8_t d = addr & 0xFF;

        m_memory.block[++PARAM_PTR] = d;
        m_memory.block[++PARAM_PTR] = c;
        m_memory.block[++PARAM_PTR] = b;
        m_memory.block[++PARAM_PTR] = a;
        PushParamType(PARAM_STRING);
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

    int NewScratchString(std::string str)
    {
        uint8_t* data = m_memory.block;
        int len = str.length();
        if (len > 255) len = 255; // length must fit into one byte
        SCRATCH_PTR -= len + 1;
        data[SCRATCH_PTR] = len;
        for (int i = 0; i < len; ++i)
        {
            data[SCRATCH_PTR + 1 + i] = str.at(i);
        }
        return SCRATCH_PTR;
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

    void PushReturnInt(int val)
    {
        uint8_t a = val & 0xFF; val = val >> 8;
        uint8_t b = val & 0xFF; val = val >> 8;
        uint8_t c = val & 0xFF; val = val >> 8;
        uint8_t d = val & 0xFF;

        m_memory.block[RET_PTR++] = d;
        m_memory.block[RET_PTR++] = c;
        m_memory.block[RET_PTR++] = b;
        m_memory.block[RET_PTR++] = a;
    }

    int PopReturnInt()
    {
        uint8_t* data = m_memory.block;
        int a = data[--RET_PTR];
        int b = data[--RET_PTR];
        int c = data[--RET_PTR];
        int d = data[--RET_PTR];

        int ret = (d << 24) | (c << 16) | (b << 8) | a;
        return ret;
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
    void Call();
    void ToFloat();
    void ToInt();
    void ToString();
    void PushScratchPtr();
    void PopScratchPtr();
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

    typedef std::function<void()> StdFunc;
    std::vector<StdFunc> m_stdlib;
    std::map<std::string, size_t> m_stdlib_lookup;
};

#endif // VM_H