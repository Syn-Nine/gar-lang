#include "VM.h"

// push, pop, peek, update

// param_ptr points to the type of the data available to pop

/*void VM::PushParamType(uint8_t type)
{
    m_memory.block[++PARAM_PTR] = type;
}

uint8_t VM::PopParamType()
{
    return m_memory.block[PARAM_PTR--];
}*/

uint8_t VM::PeekParamType()
{
    return m_memory.block[PARAM_PTR];
}

//

void VM::PushParamAt(int addr, int at)
{
    addr -= (at + 1) * 5;
    uint8_t* data = m_memory.block;
    int type = data[addr];
    if (PARAM_BOOL == type)
    {
        m_memory.block[++PARAM_PTR] = data[addr + 1];
    }
    else
    {
        m_memory.block[++PARAM_PTR] = data[addr + 4];
        m_memory.block[++PARAM_PTR] = data[addr + 3];
        m_memory.block[++PARAM_PTR] = data[addr + 2];
        m_memory.block[++PARAM_PTR] = data[addr + 1];
    }
    m_memory.block[++PARAM_PTR] = type;
    m_memory.param_cnt++;
}

void VM::PushParamBool(bool val)
{
    if (val) { m_memory.block[++PARAM_PTR] = 1; }
    else { m_memory.block[++PARAM_PTR] = 0; }
    m_memory.block[++PARAM_PTR] = PARAM_BOOL;
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
    m_memory.block[++PARAM_PTR] = PARAM_FLOAT;
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
    m_memory.block[++PARAM_PTR] = PARAM_INT;
    m_memory.param_cnt++;
}

void VM::PushParamRay(uint8_t type, int idx)
{
    uint8_t a = idx & 0xFF; idx = idx >> 8;
    uint8_t b = idx & 0xFF; idx = idx >> 8;
    uint8_t c = idx & 0xFF; idx = idx >> 8;
    uint8_t d = idx & 0xFF;

    m_memory.block[++PARAM_PTR] = d;
    m_memory.block[++PARAM_PTR] = c;
    m_memory.block[++PARAM_PTR] = b;
    m_memory.block[++PARAM_PTR] = a;
    m_memory.block[++PARAM_PTR] = type;
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
    m_memory.block[++PARAM_PTR] = PARAM_STRING;
    m_memory.param_cnt++;
}

void VM::PushParamList(int addr)
{
    uint8_t a = addr & 0xFF; addr = addr >> 8;
    uint8_t b = addr & 0xFF; addr = addr >> 8;
    uint8_t c = addr & 0xFF; addr = addr >> 8;
    uint8_t d = addr & 0xFF;

    m_memory.block[++PARAM_PTR] = d;
    m_memory.block[++PARAM_PTR] = c;
    m_memory.block[++PARAM_PTR] = b;
    m_memory.block[++PARAM_PTR] = a;
    m_memory.block[++PARAM_PTR] = PARAM_LIST;
    m_memory.param_cnt++;
}

void VM::PushParamVar(int idx)
{
    if (idx < 0)
    {
        // frame local variable
        int offset = FRAME_BASE_PTR + idx * 5;
        int type = m_memory.block[offset + 5];
        if (PARAM_BOOL == type)
        {
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 4];
            m_memory.block[++PARAM_PTR] = type;
        }
        else
        {
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 1];
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 2];
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 3];
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 4];
            m_memory.block[++PARAM_PTR] = type;
        }
    }
    else
    {
        // global variable
        int offset = MEM_STATIC_START + idx * 5;
        int type = m_memory.block[offset + 0];
        if (PARAM_BOOL == type)
        {
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 1];
            m_memory.block[++PARAM_PTR] = type;
        }
        else
        {
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 4];
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 3];
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 2];
            m_memory.block[++PARAM_PTR] = m_memory.block[offset + 1];
            m_memory.block[++PARAM_PTR] = type;
        }
    }
    m_memory.param_cnt++;
}

//

void VM::PopParamAt(int addr, int at)
{
    m_memory.param_cnt--;
    addr -= (at + 1) * 5;

    int type = m_memory.block[PARAM_PTR--];
    if (PARAM_BOOL == type)
    {
        m_memory.block[addr + 4] = 0; // padding to align values to 5 bytes
        m_memory.block[addr + 3] = 0;
        m_memory.block[addr + 2] = 0;
        m_memory.block[addr + 1] = m_memory.block[PARAM_PTR--];
    }
    else
    {
        m_memory.block[addr + 1] = m_memory.block[PARAM_PTR--];
        m_memory.block[addr + 2] = m_memory.block[PARAM_PTR--];
        m_memory.block[addr + 3] = m_memory.block[PARAM_PTR--];
        m_memory.block[addr + 4] = m_memory.block[PARAM_PTR--];
    }
    m_memory.block[addr] = type;
}

bool VM::PopParamBool()
{
    m_memory.param_cnt--;
    if (m_memory.block[PARAM_PTR--] != PARAM_BOOL) Error("Popped unexpected type.");
    int val = m_memory.block[PARAM_PTR--];
    if (1 == val) return true;
    return false;
}

float VM::PopParamFloat()
{
    m_memory.param_cnt--;
    int type = m_memory.block[PARAM_PTR--];
    if (type != PARAM_FLOAT && type != PARAM_INT) Error("Popped unexpected type.");
    uint8_t* data = m_memory.block;
    int a = data[PARAM_PTR--];
    int b = data[PARAM_PTR--];
    int c = data[PARAM_PTR--];
    int d = data[PARAM_PTR--];
    int ival = (d << 24) | (c << 16) | (b << 8) | a;
    if (type == PARAM_INT) return (float)ival;

    float fval;
    memcpy(&fval, &ival, 4);
    return fval;
}

int VM::PopParamInt()
{
    m_memory.param_cnt--;
    int type = m_memory.block[PARAM_PTR--];
    if (type != PARAM_FLOAT && type != PARAM_INT) Error("Popped unexpected type.");
    uint8_t* data = m_memory.block;
    int a = data[PARAM_PTR--];
    int b = data[PARAM_PTR--];
    int c = data[PARAM_PTR--];
    int d = data[PARAM_PTR--];
    int ival = (d << 24) | (c << 16) | (b << 8) | a;
    if (type == PARAM_INT) return ival;

    float fval;
    memcpy(&fval, &ival, 4);
    return (int)fval;
}

int VM::PopParamRay(uint8_t type)
{
    m_memory.param_cnt--;
    if (type != m_memory.block[PARAM_PTR--]) Error("Popped unexpected type.");
    uint8_t* data = m_memory.block;
    int a = data[PARAM_PTR--];
    int b = data[PARAM_PTR--];
    int c = data[PARAM_PTR--];
    int d = data[PARAM_PTR--];
    int ival = (d << 24) | (c << 16) | (b << 8) | a;
    return ival;
}

std::string VM::PopParamString(int* addr_out /* = nullptr */)
{
    m_memory.param_cnt--;
    if (m_memory.block[PARAM_PTR--] != PARAM_STRING) Error("Popped unexpected type.");
    uint8_t* data = m_memory.block;
    
    // pop arena address
    int a = data[PARAM_PTR--];
    int b = data[PARAM_PTR--];
    int c = data[PARAM_PTR--];
    int d = data[PARAM_PTR--];
    int addr = (d << 24) | (c << 16) | (b << 8) | a;
    if (addr_out) *addr_out = addr;

    // copy string from arena address
    b = data[addr++];
    a = data[addr++];
    int len = b << 8 | a;

    char* buf = (char*)malloc(len + 1);
    for (int i = 0; i < len; ++i)
    {
        buf[i] = data[addr++];
    }
    buf[len] = 0;
    std::string ret = std::string(buf);
    free(buf);
    return ret;
}


int VM::PopParamList()
{
    m_memory.param_cnt--;
    if (m_memory.block[PARAM_PTR--] != PARAM_LIST) Error("Popped unexpected type.");
    uint8_t* data = m_memory.block;

    // pop arena address
    int a = data[PARAM_PTR--];
    int b = data[PARAM_PTR--];
    int c = data[PARAM_PTR--];
    int d = data[PARAM_PTR--];
    int addr = (d << 24) | (c << 16) | (b << 8) | a;
    return addr;
}


int VM::PopParamPointer()
{
    m_memory.param_cnt--;
    PARAM_PTR--; // ignore type
    uint8_t* data = m_memory.block;

    // pop arena address
    int a = data[PARAM_PTR--];
    int b = data[PARAM_PTR--];
    int c = data[PARAM_PTR--];
    int d = data[PARAM_PTR--];
    int addr = (d << 24) | (c << 16) | (b << 8) | a;
    return addr;
}


void VM::PopParamVar(int idx)
{
    m_memory.param_cnt--;
    int type = m_memory.block[PARAM_PTR--];

    if (idx < 0)
    {
        // frame local variable
        int offset = FRAME_BASE_PTR + idx * 5;
        m_memory.block[offset + 4] = m_memory.block[PARAM_PTR--];
        if (PARAM_BOOL != type)
        {
            m_memory.block[offset + 3] = m_memory.block[PARAM_PTR--];
            m_memory.block[offset + 2] = m_memory.block[PARAM_PTR--];
            m_memory.block[offset + 1] = m_memory.block[PARAM_PTR--];
        }
        m_memory.block[offset + 5] = type;
    }
    else
    {
        // global variable
        int offset = MEM_STATIC_START + idx * 5;
        m_memory.block[offset + 1] = m_memory.block[PARAM_PTR--];
        if (PARAM_BOOL != type)
        {
            m_memory.block[offset + 2] = m_memory.block[PARAM_PTR--];
            m_memory.block[offset + 3] = m_memory.block[PARAM_PTR--];
            m_memory.block[offset + 4] = m_memory.block[PARAM_PTR--];
        }
        m_memory.block[offset + 0] = type;
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


// scratch pad

int VM::PushScratchList(int len)
{
    int sz = 2 + len * 5;
    if (SCRATCH_PTR + sz > MEM_SCRATCH_END) SCRATCH_PTR = MEM_SCRATCH_START;
    for (int i = 0; i < len; ++i)
    {
        PushScratchParam();
    }
    PushScratchInt16(len);
    return SCRATCH_PTR - 2;
}


void VM::PushScratchInt16(int val)
{
    uint8_t a = val & 0xFF; val = val >> 8;
    uint8_t b = val & 0xFF;

    m_memory.block[SCRATCH_PTR++] = b;
    m_memory.block[SCRATCH_PTR++] = a;
}

void VM::PushScratchParam()
{
    int type = m_memory.block[PARAM_PTR--];
    m_memory.block[SCRATCH_PTR++] = type;
    if (PARAM_BOOL == type)
    {
        m_memory.block[SCRATCH_PTR++] = m_memory.block[PARAM_PTR--];
        m_memory.block[SCRATCH_PTR++] = 0; // padding to align values to 5 bytes
        m_memory.block[SCRATCH_PTR++] = 0;
        m_memory.block[SCRATCH_PTR++] = 0;
    }
    else
    {
        m_memory.block[SCRATCH_PTR++] = m_memory.block[PARAM_PTR--];
        m_memory.block[SCRATCH_PTR++] = m_memory.block[PARAM_PTR--];
        m_memory.block[SCRATCH_PTR++] = m_memory.block[PARAM_PTR--];
        m_memory.block[SCRATCH_PTR++] = m_memory.block[PARAM_PTR--];
    }
}


int VM::PeekScratchInt16(int addr)
{
    uint8_t* data = m_memory.block;
    int b = data[addr++];
    int a = data[addr++];

    int ret = (b << 8) | a;
    return ret;
}


int VM::NewScratchString(std::string str)
{
    uint8_t* data = m_memory.block;
    int len = str.length();

    if (len > MEM_SCRATCH_SZ - 2) len = MEM_SCRATCH_SZ - 2; // length must fit into scratch
    if (SCRATCH_PTR + len + 2 > MEM_SCRATCH_END) SCRATCH_PTR = MEM_SCRATCH_START;

    int ret = SCRATCH_PTR;
    PushScratchInt16(len);

    for (int i = 0; i < len; ++i)
    {
        data[SCRATCH_PTR++] = str.at(i);
    }
    return ret;
}

// heap

int VM::HeapNewList(int src)
{
    uint8_t* data = m_memory.block;

    int len = PeekScratchInt16(src);
    int sz = len * 5 + 2;

    HEAP_PTR += sz;
    int dst = HEAP_PTR - 2;
    uint8_t a = len & 0xFF;
    uint8_t b = (len >> 8) & 0xFF;
    data[dst] = b;
    data[dst + 1] = a;

    // copy bytes walking backward
    int src_addr = src - 5;
    int dst_addr = dst - 5;
    for (int i = 0; i < len; ++i)
    {
        int type = data[src_addr];
        if (PARAM_STRING == type || PARAM_LIST == type)
        {
            int a = data[src_addr + 1];
            int b = data[src_addr + 2];
            int c = data[src_addr + 3];
            int d = data[src_addr + 4];
            int addr = (d << 24) | (c << 16) | (b << 8) | a;
            if (PARAM_STRING == type) addr = HeapNewString(addr);
            else if (PARAM_LIST == type) addr = HeapNewList(addr);
            a = addr & 0xFF; addr = addr >> 8;
            b = addr & 0xFF; addr = addr >> 8;
            c = addr & 0xFF; addr = addr >> 8;
            d = addr & 0xFF;
            data[dst_addr + 1] = a;
            data[dst_addr + 2] = b;
            data[dst_addr + 3] = c;
            data[dst_addr + 4] = d;
        }
        else
        {
            data[dst_addr + 1] = data[src_addr + 1];
            data[dst_addr + 2] = data[src_addr + 2];
            data[dst_addr + 3] = data[src_addr + 3];
            data[dst_addr + 4] = data[src_addr + 4];
        }
        data[dst_addr] = type;
        src_addr -= 5;
        dst_addr -= 5;
    }


    return dst;
}

int VM::HeapNewString(int src)
{
    int dst = HEAP_PTR;

    uint8_t* data = m_memory.block;
    int b = data[src++];
    int a = data[src++];
    int len = (b << 8) | a;

    m_memory.block[HEAP_PTR++] = b;
    m_memory.block[HEAP_PTR++] = a;

    for (int i = 0; i < len; ++i)
    {
        m_memory.block[HEAP_PTR++] = data[src++];
    }

    return dst;
}

// frame


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

