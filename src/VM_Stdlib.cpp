#include "VM.h"
#include "Environment.h"

#include "Stdlib.h"


int VM::GetFuncAddr(std::string name)
{
    if (0 == m_instance->m_stdlib_lookup.count(name)) return -1;
    return m_instance->m_stdlib_lookup.at(name);
}

void VM::LoadStdlib()
{
    // register standard library functions
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

