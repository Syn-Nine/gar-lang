#ifndef VM_H
#define VM_H

#include "Token.h"
#include "ErrorHandler.h"
#include "Utilities.h"

//#include <format>
#include <string>
#include <functional>


#define PARAM_PTR m_memory.param_ptr
#define INST_PTR m_memory.inst_ptr
#define FRAME_PTR m_memory.frame_ptr
#define FRAME_BASE_PTR m_memory.frame_base_ptr
#define BLOCK_BASE_PTR m_memory.block_base_ptr

class Environment;

class VM
{
public:
    VM() = delete;

    VM(Environment* environment, ErrorHandler* errorHandler)
    {
        m_errorHandler = errorHandler;
        //m_env = environment;
        LoadStdlib();
        m_instance = this;
    }

    static int GetStdlibAddr(std::string name);
    void LoadStdlib();
    void Execute(uint8_t* bytecode);
    
    static const int MEM_BLOCK_WIDTH = 128;
    static const int MEM_BLOCK_HEIGHT = 960;
    static const int MEM_BLOCK_SZ = MEM_BLOCK_WIDTH * MEM_BLOCK_HEIGHT * 3;
    static const int MEM_INST_SZ = 65536;   // size of instruction block
    static const int MEM_PARAM_START = MEM_INST_SZ;
    static const int MEM_PARAM_SZ = 256;
    static const int MEM_STATIC_START = MEM_PARAM_START + MEM_PARAM_SZ;
    
    std::string Dump();

private:

    static VM* m_instance;

    struct Memory
    {
        // registers
        uint32_t frame_ptr;
        uint32_t frame_base_ptr;
        uint32_t block_base_ptr;
        uint32_t param_ptr;
        uint16_t inst_ptr;
        uint8_t param_cnt;
        // program arena
        uint8_t block[MEM_BLOCK_SZ];
    };


    // parameter stack

    static const uint8_t PARAM_INVALID = 0;
    static const uint8_t PARAM_BOOL = 1;
    static const uint8_t PARAM_FLOAT = 2;
    static const uint8_t PARAM_INT = 3;
    static const uint8_t PARAM_STRING = 4;

    // push, pop, peek, update

    uint8_t PeekParamType();
    uint8_t PopParamType();
    bool PopParamBool();
    float PopParamFloat();
    int PeekParamInt();
    int PopParamInt();
    std::string PopParamString();
    void PushParamType(uint8_t type);
    void PushParamBool(bool val);
    void PushParamFloat(float fval);
    void PushParamInt(int val);
    void PushParamString(int addr);
    void PushParamVar(int idx);
    void PopParamVar(int idx);

    // frame stack
    void PushFrameInt(int val);
    int PopFrameInt();

    int NewScratchString(std::string str);
    
    

    uint8_t PeekInst();
    int ReadInstInt16();
    int ReadInstInt();

    // Executive
    void Alloca();
    void BlockStart();
    void BlockEnd();
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
    //

    void RegisterFunc(std::string name, int arity, std::function<void()> ftn);

    Memory m_memory;
    
    void Error(const std::string& err);

    ErrorHandler* m_errorHandler;

    struct func_s
    {
        std::string name;
        std::function<void()> ftn;
        int arity;
    };

    std::vector<func_s> m_stdlib;
    std::map<std::string, size_t> m_stdlib_lookup;
    
};

#endif // VM_H