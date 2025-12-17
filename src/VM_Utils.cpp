#include "VM.h"

// push, pop, peek, update

// param_ptr points to the type of the data available to pop

void VM::PushParamType(uint8_t type)
{
    m_memory.block[++PARAM_PTR] = type;
}

uint8_t VM::PopParamType()
{
    return m_memory.block[PARAM_PTR--];
}

uint8_t VM::PeekParamType()
{
    return m_memory.block[PARAM_PTR];
}

//

void VM::PushParamBool(bool val)
{
    if (val) { m_memory.block[++PARAM_PTR] = 1; }
    else { m_memory.block[++PARAM_PTR] = 0; }
    PushParamType(PARAM_BOOL);
    m_memory.param_cnt++;
}

void VM::PushParamFloat(float fval)
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
    m_memory.param_cnt++;
}

void VM::PushParamInt(int val)
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
    m_memory.param_cnt++;
}

void VM::PushParamString(int addr)
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
    m_memory.param_cnt++;
}

void VM::PushParamVar(int idx)
{
    if (idx < 0)
    {
        // frame local variable
        int offset = FRAME_BASE_PTR + idx * 5;
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 1];
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 2];
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 3];
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 4];
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 5];
    }
    else
    {
        // global variable
        int offset = MEM_STATIC_START + idx * 5;
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 4];
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 3];
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 2];
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 1];
        m_memory.block[++PARAM_PTR] = m_memory.block[offset + 0];
    }
    m_memory.param_cnt++;
}

//

bool VM::PopParamBool()
{
    m_memory.param_cnt--;
    int val = m_memory.block[PARAM_PTR--];
    if (1 == val) return true;
    return false;
}

float VM::PopParamFloat()
{
    m_memory.param_cnt--;
    int ival = PopParamInt();
    float fval;
    memcpy(&fval, &ival, 4);
    return fval;
}

int VM::PopParamInt()
{
    m_memory.param_cnt--;
    uint8_t* data = m_memory.block;
    int a = data[PARAM_PTR--];
    int b = data[PARAM_PTR--];
    int c = data[PARAM_PTR--];
    int d = data[PARAM_PTR--];

    int ret = (d << 24) | (c << 16) | (b << 8) | a;
    return ret;
}

std::string VM::PopParamString()
{
    m_memory.param_cnt--;
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


void VM::PopParamVar(int idx)
{
    m_memory.param_cnt--;
    
    if (idx < 0)
    {
        // frame local variable
        int offset = FRAME_BASE_PTR + idx * 5;
        m_memory.block[offset + 5] = m_memory.block[PARAM_PTR--];
        m_memory.block[offset + 4] = m_memory.block[PARAM_PTR--];
        m_memory.block[offset + 3] = m_memory.block[PARAM_PTR--];
        m_memory.block[offset + 2] = m_memory.block[PARAM_PTR--];
        m_memory.block[offset + 1] = m_memory.block[PARAM_PTR--];
    }
    else
    {
        // global variable
        int offset = MEM_STATIC_START + idx * 5;
        m_memory.block[offset + 0] = m_memory.block[PARAM_PTR--];
        m_memory.block[offset + 1] = m_memory.block[PARAM_PTR--];
        m_memory.block[offset + 2] = m_memory.block[PARAM_PTR--];
        m_memory.block[offset + 3] = m_memory.block[PARAM_PTR--];
        m_memory.block[offset + 4] = m_memory.block[PARAM_PTR--];
    }
}

//

int VM::PeekParamInt()
{
    uint8_t* data = m_memory.block;
    int a = data[PARAM_PTR];
    int b = data[PARAM_PTR - 1];
    int c = data[PARAM_PTR - 2];
    int d = data[PARAM_PTR - 3];

    int ret = (d << 24) | (c << 16) | (b << 8) | a;
    return ret;
}


// frame stack

void VM::PushFrameInt(int val)
{
    uint8_t a = val & 0xFF; val = val >> 8;
    uint8_t b = val & 0xFF; val = val >> 8;
    uint8_t c = val & 0xFF; val = val >> 8;
    uint8_t d = val & 0xFF;

    m_memory.block[FRAME_PTR--] = d;
    m_memory.block[FRAME_PTR--] = c;
    m_memory.block[FRAME_PTR--] = b;
    m_memory.block[FRAME_PTR--] = a;
}

int VM::PopFrameInt()
{
    uint8_t* data = m_memory.block;
    int a = data[++FRAME_PTR];
    int b = data[++FRAME_PTR];
    int c = data[++FRAME_PTR];
    int d = data[++FRAME_PTR];

    int ret = (d << 24) | (c << 16) | (b << 8) | a;
    return ret;
}


int VM::NewScratchString(std::string str)
{
    uint8_t* data = m_memory.block;
    int len = str.length();
    if (len > 255) len = 255; // length must fit into one byte
    for (int i = 0; i < len; ++i)
    {
        data[FRAME_PTR--] = str.at(len - i - 1);
    }
    data[FRAME_PTR] = len;
    return FRAME_PTR--;
}

// instruction block

uint8_t VM::PeekInst()
{
    return m_memory.block[INST_PTR];
}

int VM::ReadInstInt16()
{
    uint8_t* data = m_memory.block;
    int b = data[INST_PTR++];
    int a = data[INST_PTR++];

    int ret = (b << 8) | a;
    return ret;
}

int VM::ReadInstInt()
{
    uint8_t* data = m_memory.block;
    int d = data[INST_PTR++];
    int c = data[INST_PTR++];
    int b = data[INST_PTR++];
    int a = data[INST_PTR++];

    int ret = (d << 24) | (c << 16) | (b << 8) | a;
    return ret;
}



void VM::Error(const std::string& err)
{
    m_errorHandler->Error("VM", INST_PTR - 1, err);
}

