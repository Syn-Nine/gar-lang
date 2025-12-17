#include "VM.h"
#include "Environment.h"

#include "Stdlib.h"


int VM::GetStdlibAddr(std::string name)
{
    if (0 == m_instance->m_stdlib_lookup.count(name)) return -1;
    return m_instance->m_stdlib_lookup.at(name);
}

void VM::LoadStdlib()
{
    // register standard library functions
    RegisterFunc("%randi", 2, [this]() {
        uint8_t rhs_type = PopParamType();
        int rhs = PopParamInt();
        uint8_t lhs_type = PopParamType();
        int lhs = PopParamInt();
        int ret = __rand_range_impl(lhs, rhs);
        PushParamInt(ret);
        }
    );

    // register standard library functions
    RegisterFunc("%input", 0, [this]() {
        std::string str = console_input();
        int addr = NewScratchString(str);
        PushParamString(addr);
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

