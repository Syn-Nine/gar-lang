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
//#define BLOCK_BASE_PTR m_memory.block_base_ptr
#define SCRATCH_PTR m_memory.scratch_ptr
#define HEAP_PTR m_memory.heap_ptr

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
        m_memory.block = (uint8_t*)malloc(MEM_BLOCK_SZ);
    }

    ~VM()
    {
        free(m_memory.block);
    }

    static int GetFuncAddr(std::string name);
    void LoadStdlib();
    void Execute(uint8_t* bytecode, size_t heap_addr, size_t entry_addr);
    
    static const int MEM_BLOCK_WIDTH = 128;
    static const int MEM_BLOCK_HEIGHT = 960;
    static const int MEM_BLOCK_SZ = MEM_BLOCK_WIDTH * MEM_BLOCK_HEIGHT * 3;
    static const int MEM_INST_SZ = 65536;   // size of instruction block
    static const int MEM_SCRATCH_START = MEM_INST_SZ;
    static const int MEM_SCRATCH_SZ = 16348;
    static const int MEM_SCRATCH_END = MEM_SCRATCH_START + MEM_SCRATCH_SZ;
    static const int MEM_PARAM_START = MEM_SCRATCH_END;
    static const int MEM_PARAM_SZ = 16348;
    static const int MEM_STATIC_START = MEM_PARAM_START + MEM_PARAM_SZ;
    
    std::string Dump();

private:

    static VM* m_instance;

    struct Memory
    {
        // registers
        uint32_t frame_ptr;
        uint32_t frame_base_ptr;
        //uint32_t block_base_ptr;
        uint32_t param_ptr;
        uint32_t scratch_ptr;
        uint32_t heap_ptr;
        uint16_t inst_ptr;
        uint8_t param_cnt;
        // program arena
        uint8_t* block;
    };


    // parameter stack

    static const uint8_t PARAM_INVALID = 0;
    static const uint8_t PARAM_BOOL = 1;
    static const uint8_t PARAM_FLOAT = 2;
    static const uint8_t PARAM_INT = 3;
    static const uint8_t PARAM_STRING = 4;
    static const uint8_t PARAM_LIST = 5;

    // push, pop, peek, update

    uint8_t PeekParamType();
    bool PopParamBool();
    float PopParamFloat();
    int PeekParamInt();
    int PopParamInt();
    std::string PopParamString(int* addr_out = nullptr);
    void PushParamBool(bool val);
    void PushParamFloat(float fval);
    void PushParamInt(int val);
    void PushParamString(int addr);
    void PushParamList(int addr);
    int PopParamPointer();
    void PushParamVar(int idx);
    void PushParamAt(int addr, int at);
    void PopParamVar(int idx);
    void PopParamAt(int addr, int at);
    int PopParamList();
    
    // scratch pad
    int PushScratchList(int len);
    void PushScratchInt16(int val);
    void PushScratchParam();
    int PeekScratchInt16(int addr);
    int NewScratchString(std::string str);

    // heap
    int HeapNewList(int addr);
    int HeapNewString(int addr);

    // frame stack
    void PushFrameInt(int val);
    int PopFrameInt();
    
    

    uint8_t PeekInst();
    int ReadInstInt16();
    int ReadInstInt();

    // Executive
    void Alloca();
    void LoadAt();
    void LoadBool();
    void LoadInt();
    void LoadFloat();
    void LoadString();
    void LoadVar();
    void MakeList();
    void StoreAt();
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
    void CallDef();
    void Ret();
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