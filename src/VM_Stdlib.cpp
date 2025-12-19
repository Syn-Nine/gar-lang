#include "VM.h"
#include "VM_Stdlib.h"

int VM::GetFuncAddr(std::string name)
{
    if (0 == m_instance->m_stdlib_lookup.count(name)) return -1;
    return m_instance->m_stdlib_lookup.at(name);
}

void VM::RegisterFunc(std::string name, int arity, std::function<void()> ftn)
{
    func_s func;
    func.name = name;
    func.ftn = ftn;
    func.arity = arity;
    size_t idx = m_stdlib.size();
    m_stdlib.push_back(func);
    m_stdlib_lookup.insert(std::make_pair(name, idx));
}


void VM::LoadStdlib()
{
    // register standard library functions

    RegisterFunc("%fill", 2, [this]() {
        int len = PopParamInt();
        int type = PeekParamType();

        // make a new list in scratch
        if (type == PARAM_BOOL)
        {
            bool val = PopParamBool();
            for (int i = 0; i < len; ++i) { PushParamBool(val); }
        }
        else if (type == PARAM_INT)
        {
            int val = PopParamInt();
            for (int i = 0; i < len; ++i) { PushParamInt(val); }
        }
        else if (type == PARAM_FLOAT)
        {
            float val = PopParamFloat();
            for (int i = 0; i < len; ++i) { PushParamFloat(val); }
        }
        else if (type == PARAM_STRING || type == PARAM_LIST)
        {
            int addr = PopParamPointer();
            for (int i = 0; i < len; ++i) {
                if (type == PARAM_STRING) PushParamString(addr);
                else PushParamList(addr);
            }
        }
        
        int addr = PushScratchList(len);
        PushParamList(addr);
        }
    );

    RegisterFunc("%len", 1, [this]() {
        int type = PeekParamType();
        if (type == PARAM_STRING || type == PARAM_LIST)
        {
            int addr = PopParamPointer();
            int len = PeekScratchInt16(addr);
            PushParamInt(len);
        }
        else
        {
            Error("Parameter type mismatch in len().");
        }
        }
    );

    RegisterFunc("%abs", 1, [this]() {
        float in0 = PopParamFloat();
        PushParamFloat(abs(in0));
        }
    );

    RegisterFunc("%cos", 1, [this]() {
        float in0 = PopParamFloat();
        PushParamFloat(cos(in0));
        }
    );

    RegisterFunc("%sin", 1, [this]() {
        float in0 = PopParamFloat();
        PushParamFloat(sin(in0));
        }
    );

    RegisterFunc("%rand", 0, [this]() {
        float ret = __rand_impl();
        PushParamFloat(ret);
        }
    );

    RegisterFunc("%randi", 2, [this]() {
        int rhs = PopParamInt();
        int lhs = PopParamInt();
        int ret = __rand_range_impl(lhs, rhs);
        PushParamInt(ret);
        }
    );

    RegisterFunc("%input", 0, [this]() {
        std::string str = console_input();
        int addr = NewScratchString(str);
        PushParamString(addr);
        }
    );

    RegisterFunc("%new", 0, [this]() {
        int type = PeekParamType();
        if (type == PARAM_LIST)
        {
            int addr = HeapNewList(PopParamPointer());
            PushParamList(addr);
        }
        else if (type == PARAM_STRING)
        {
            int addr = HeapNewString(PopParamPointer());
            PushParamString(addr);
        }
        }
    );

}

