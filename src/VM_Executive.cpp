#include "VM.h"

#include <stdexcept>

VM* VM::m_instance = nullptr;

void VM::Execute(uint8_t* bytecode, size_t heap_addr, size_t entry_addr)
{
    // load bytecode into VM memory
    memcpy(m_memory.block, bytecode, MEM_BLOCK_SZ);

    INST_PTR = entry_addr;
    PARAM_PTR = MEM_PARAM_START;
    FRAME_BASE_PTR = MEM_BLOCK_SZ - 1;
    FRAME_PTR = FRAME_BASE_PTR;
    //BLOCK_BASE_PTR = FRAME_PTR;
    SCRATCH_PTR = MEM_SCRATCH_START;
    HEAP_PTR = heap_addr;

    //
    
    m_memory.block[PARAM_PTR] = PARAM_INVALID;

    printf("Executing...\n");
    bool quit = false;
    
    while (true)
    {
        TokenTypeEnum inst = (TokenTypeEnum)m_memory.block[INST_PTR];
        if (TOKEN_END_OF_FILE == inst) break;

        //printf("%d(%s), %d, %d\n", INST_PTR, ToHex2(INST_PTR).c_str(), PeekParamType(), FRAME_PTR);

        INST_PTR++;
        
        switch (inst)
        {
        case TOKEN_ALLOCA: Alloca(); break;
        case TOKEN_LOAD_AT: LoadAt(); break;
        case TOKEN_LOAD_BOOL: LoadBool(); break;
        case TOKEN_LOAD_INT: LoadInt(); break;
        case TOKEN_LOAD_FLOAT: LoadFloat(); break;
        case TOKEN_LOAD_STRING: LoadString(); break;
        case TOKEN_LOAD_VAR: LoadVar(); break;
        case TOKEN_MAKE_LIST: MakeList(); break;
        case TOKEN_STORE_AT: StoreAt(); break;
        case TOKEN_STORE_VAR: StoreVar(); break;
        case TOKEN_MINUS: // intentional fall-through
        case TOKEN_HAT:
        case TOKEN_PLUS: 
        case TOKEN_SLASH:
        case TOKEN_STAR:
            BinaryOp(inst);
            break;
        case TOKEN_CAST_FLOAT: ToFloat(); break;
        case TOKEN_CAST_INT: ToInt(); break;
        case TOKEN_CAST_STRING: ToString(); break;
        case TOKEN_AND: // intentional fall-through
        case TOKEN_OR:
            AndOr(inst);
            break;
        case TOKEN_EQUAL_EQUAL: // intentional fall-through
        case TOKEN_BANG_EQUAL:
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
            ComparisonOp(inst);
            break;
        case TOKEN_NEG: Negate(); break;
        case TOKEN_INV: Invert(); break;
        case TOKEN_PRINT: Print(false); break;
        case TOKEN_PRINTLN: Print(true); break;
        case TOKEN_IF: IfJmp(); break;
        case TOKEN_JMP: Jmp(); break;
        case TOKEN_CALL: Call(); break;
        case TOKEN_CALL_DEF: CallDef(); break;
        case TOKEN_RET: Ret(); break;
        default:
            Error("Bytecode not handled: " + std::to_string(inst));
            quit = true;
        }
        
        if (quit) break;
    }

    printf("\nParameter Stack:\n");
    if (m_memory.block[PARAM_PTR] == PARAM_INVALID)
    {
        printf("-- Empty\n");
    }
    else
    {
        int cc = 0;
        while (PARAM_PTR > MEM_PARAM_START)
        {
            uint8_t type = PeekParamType();
            if (PARAM_BOOL == type)
            {
                bool val = PopParamBool();
                if (val) { printf("%d: BOOL  : true\n", cc); }
                else { printf("%d: BOOL  : false\n", cc); }
            }
            else if (PARAM_FLOAT == type)
            {
                float fval = PopParamFloat();
                printf("%d: FLOAT : %f\n", cc, fval);
            }
            else if (PARAM_INT == type)
            {
                int val = PopParamInt();
                printf("%d: INT   : %d\n", cc, val);
            }
            else if (PARAM_STRING == type)
            {
                int addr;
                std::string str = PopParamString(&addr);
                printf("%d: STRING: @%d=\"%s\"\n", cc, addr, str.c_str());
            }
            else if (PARAM_LIST == type)
            {
                int addr = PopParamList();
                int count = PeekScratchInt16(addr);
                printf("%d: List[%d]: @%d\n", cc, count, addr);
            }
            cc++;
        }
    }

    printf("Done.\n");
}

void VM::Alloca()
{
    // allocate space on the frame stack for a variable
    FRAME_PTR -= 5;
}


void VM::LoadAt()
{
    int at = PopParamInt();
    int addr = PopParamList();
    PushParamAt(addr, at);
}

void VM::LoadBool()
{
    m_memory.param_cnt++;
    m_memory.block[++PARAM_PTR] = m_memory.block[INST_PTR++];
    m_memory.block[++PARAM_PTR] = PARAM_BOOL;
}

void VM::LoadFloat()
{
    m_memory.param_cnt++;
    PushParamInst4();
    m_memory.block[++PARAM_PTR] = PARAM_FLOAT;
}

void VM::LoadString()
{
    m_memory.param_cnt++;
    PushParamInst4();
    m_memory.block[++PARAM_PTR] = PARAM_STRING;
}

void VM::LoadInt()
{
    m_memory.param_cnt++;
    PushParamInst4();
    m_memory.block[++PARAM_PTR] = PARAM_INT;
}

void VM::LoadVar()
{
    int idx = ReadInstInt();
    PushParamVar(idx);
}

void VM::MakeList()
{
    int count = ReadInstInt16();
    int addr = PushScratchList(count);
    PushParamList(addr);
}


void VM::StoreAt()
{
    int at = PopParamInt();
    int addr = PopParamList();
    PopParamAt(addr, at);
}

void VM::StoreVar()
{
    int idx = ReadInstInt();
    PopParamVar(idx);
}


void VM::PushParamInst4()
{
    m_memory.block[++PARAM_PTR] = m_memory.block[INST_PTR++];
    m_memory.block[++PARAM_PTR] = m_memory.block[INST_PTR++];
    m_memory.block[++PARAM_PTR] = m_memory.block[INST_PTR++];
    m_memory.block[++PARAM_PTR] = m_memory.block[INST_PTR++];
}

void VM::BinaryOp(TokenTypeEnum oper)
{
    int irhs, ilhs;
    float frhs, flhs;
    bool isfloat = false;
    std::string srhs;
    std::string slhs;

    uint8_t rhs_type = PeekParamType();
    if (PARAM_FLOAT == rhs_type)
    {
        isfloat = true;
        frhs = PopParamFloat();
    }
    else if (PARAM_INT == rhs_type)
    {
        irhs = PopParamInt();
        frhs = (float)irhs;
    }
    else if (PARAM_STRING == rhs_type)
    {
        srhs = PopParamString();
    }
    else
    {
        Error("Invalid rhs parameter type for boolean operation.");
        return;
    }

    uint8_t lhs_type = PeekParamType();
    if (PARAM_FLOAT == lhs_type)
    {
        isfloat = true;
        flhs = PopParamFloat();
    }
    else if (PARAM_INT == lhs_type)
    {
        ilhs = PopParamInt();
        flhs = (float)ilhs;
    }
    else if (PARAM_STRING == lhs_type)
    {
        slhs = PopParamString();
    }
    else
    {
        Error("Invalid lhs parameter type for boolean operation.");
        return;
    }

    if (rhs_type == PARAM_STRING || lhs_type == PARAM_STRING)
    {
        if (rhs_type != PARAM_STRING || lhs_type != PARAM_STRING)
        {
            Error("String parameter type mismatch in binary op.");
            return;
        }
        if (TOKEN_PLUS != oper)
        {
            Error("Operator mismatch in string binary op.");
            return;
        }
        int addr = NewScratchString(slhs + srhs);
        PushParamString(addr);
        return;
    }

    if (isfloat || TOKEN_SLASH == oper)
    {
        float tot;
        if (TOKEN_MINUS == oper) { tot = flhs - frhs; }
        else if (TOKEN_PLUS == oper) { tot = flhs + frhs; }
        else if (TOKEN_SLASH == oper) { tot = flhs / frhs; }
        else if (TOKEN_STAR == oper) { tot = flhs * frhs; }
        PushParamFloat(tot);
    }
    else
    {
        int tot;
        if (TOKEN_MINUS == oper) { tot = ilhs - irhs; }
        else if (TOKEN_PLUS == oper) { tot = ilhs + irhs; }
        else if (TOKEN_STAR == oper) { tot = ilhs * irhs; }
        PushParamInt(tot);
    }
}


void VM::AndOr(TokenTypeEnum oper)
{
    uint8_t rhs_type = PeekParamType();
    if (PARAM_BOOL != rhs_type)
    {
        Error("AndOr parameter is not boolean.");
        return;
    }
    bool rval = PopParamBool();
    

    uint8_t lhs_type = PeekParamType();
    if (PARAM_BOOL != lhs_type)
    {
        Error("AndOr parameter is not boolean.");
        return;
    }
    bool lval = PopParamBool();

    switch (oper)
    {
    case TOKEN_AND: PushParamBool(lval && rval); break;
    case TOKEN_OR: PushParamBool(lval || rval); break;
    }
}


void VM::ComparisonOp(TokenTypeEnum oper)
{
    int irhs, ilhs;
    float frhs, flhs;
    bool brhs, blhs;
    bool isfloat = false;
    bool isbool = false;
    std::string srhs;
    std::string slhs;

    uint8_t rhs_type = PeekParamType();
    if (PARAM_FLOAT == rhs_type)
    {
        isfloat = true;
        frhs = PopParamFloat();
    }
    else if (PARAM_INT == rhs_type)
    {
        irhs = PopParamInt();
        frhs = (float)irhs;
    }
    else if (PARAM_BOOL == rhs_type)
    {
        isbool = true;
        brhs = PopParamBool();
    }
    else if (PARAM_STRING == rhs_type)
    {
        srhs = PopParamString();
    }

    uint8_t lhs_type = PeekParamType();
    if (PARAM_FLOAT == lhs_type)
    {
        isfloat = true;
        flhs = PopParamFloat();
    }
    else if (PARAM_INT == lhs_type)
    {
        ilhs = PopParamInt();
        flhs = (float)ilhs;
    }
    else if (PARAM_BOOL == lhs_type)
    {
        isbool = true;
        blhs = PopParamBool();
    }
    else if (PARAM_STRING == lhs_type)
    {
        slhs = PopParamString();
    }

    if (rhs_type == PARAM_STRING || lhs_type == PARAM_STRING)
    {
        if (rhs_type != PARAM_STRING || lhs_type != PARAM_STRING)
        {
            Error("String parameter type mismatch in comparison.");
            return;
        }
        if (TOKEN_EQUAL_EQUAL != oper && TOKEN_BANG_EQUAL != oper)
        {
            Error("Operator mismatch in string comparison.");
            return;
        }
        bool eq = slhs.compare(srhs) == 0;
        TOKEN_EQUAL_EQUAL == oper ? PushParamBool(eq) : PushParamBool(!eq);
        return;
    }

    if (isbool)
    {
        if (lhs_type != rhs_type)
        {
            Error("Bool type mismatch in comparison.");
            return;
        }
        switch (oper)
        {
        case TOKEN_EQUAL_EQUAL: PushParamBool(flhs == frhs); break;
        case TOKEN_BANG_EQUAL: PushParamBool(flhs != frhs); break;
        default:
            Error("Invalid comparison for bool types.");
            return;
        }
    }
    else if (isfloat)
    {
        switch (oper)
        {
        case TOKEN_LESS:  // intentional fall-through
        case TOKEN_LESS_EQUAL: PushParamBool(flhs < frhs); break;
        case TOKEN_GREATER: // intentional fall-through
        case TOKEN_GREATER_EQUAL: PushParamBool(flhs > frhs); break;
        default:
            Error("Invalid comparison for float types.");
            return;
        }
    }
    else
    {
        switch (oper)
        {
        case TOKEN_EQUAL_EQUAL: PushParamBool(ilhs == irhs); break;
        case TOKEN_BANG_EQUAL: PushParamBool(ilhs != irhs); break;
        case TOKEN_LESS: PushParamBool(ilhs < irhs); break;
        case TOKEN_LESS_EQUAL: PushParamBool(ilhs <= irhs); break;
        case TOKEN_GREATER: PushParamBool(ilhs > irhs); break;
        case TOKEN_GREATER_EQUAL: PushParamBool(ilhs >= irhs); break;
        }
    }   
}


void VM::Negate()
{
    uint8_t rhs_type = PeekParamType();
    if (PARAM_FLOAT == rhs_type)
    {
        float val = -PopParamFloat();
        PushParamFloat(val);
    }
    else if (PARAM_INT == rhs_type)
    {
        int val = -PopParamInt();
        PushParamInt(val);
    }
}


void VM::Invert()
{
    uint8_t rhs_type = PeekParamType();
    if (PARAM_BOOL != rhs_type)
    {
        Error("Inv parameter is not boolean.");
        return;
    }

    bool val = !PopParamBool();
    PushParamBool(val);
}


void VM::Print(bool newline)
{
    if ((uint8_t)TOKEN_PRINT_BLANK == PeekInst())
    {
        INST_PTR++;
        // empty print statement
        if (newline) printf("\n");
        return;
    }

    uint8_t rhs_type = PeekParamType();
    if (PARAM_BOOL == rhs_type)
    {
        bool val = PopParamBool();
        if (val) { printf("true"); }
        else { printf("false"); }
    }
    else if (PARAM_FLOAT == rhs_type)
    {
        float fval = PopParamFloat();
        printf("%f", fval);
    }
    else if (PARAM_INT == rhs_type)
    {
        int ival = PopParamInt();
        printf("%d", ival);
    }
    else if (PARAM_STRING == rhs_type)
    {
        std::string str = PopParamString();
        printf("%s", str.c_str());
    }
    else if (PARAM_LIST == rhs_type)
    {
        printf("[");
        int addr = PopParamList();
        int len = PeekScratchInt16(addr);
        uint8_t* data = m_memory.block;
        addr -= 5;
        for (int i = 0; i < len; ++i)
        {
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
            if (PARAM_STRING == type)
            {
                printf("\""); VM::Print(false); printf("\"");
            }
            else
            {
                VM::Print(false); // recursive popping and printing
            }
            if (i < len - 1) printf(", ");
            addr -= 5;
        }
        printf("]");
    }
    else
    {
        Error("Parameter type not recognized.");
    }

    if (newline) printf("\n");
    
}

void VM::IfJmp()
{
    uint8_t rhs_type = PeekParamType();
    if (PARAM_BOOL != rhs_type)
    {
        Error("If parameter is not boolean.");
        return;
    }
    bool rval = PopParamBool();

    if (!rval)
    {
        // consume then address
        INST_PTR += 2;
        return;
    }
    else
    {
        // perform the then jump
        Jmp();
    }
}

void VM::Jmp()
{
    INST_PTR = ReadInstInt16();
}


void VM::Call()
{
    uint16_t addr = ReadInstInt16();
    if (m_memory.param_cnt < m_stdlib[addr].arity)
    {
        Error("Parameter count not met for stdlib function: " + m_stdlib[addr].name);
    }
    m_stdlib[addr].ftn();
}

void VM::CallDef()
{
    int addr = ReadInstInt16();
    PushFrameInt(INST_PTR);
    PushFrameInt(FRAME_BASE_PTR);
    FRAME_BASE_PTR = FRAME_PTR;
    INST_PTR = addr;
}

void VM::Ret()
{
    FRAME_PTR = FRAME_BASE_PTR;
    FRAME_BASE_PTR = PopFrameInt();
    INST_PTR = PopFrameInt();
}

void VM::ToFloat()
{
    uint8_t rhs_type = PeekParamType();
    if (PARAM_FLOAT == rhs_type) return; // noop

    if (PARAM_INT == rhs_type)
    {
        int val = PopParamInt();
        PushParamFloat(val);
    }
    else if (PARAM_STRING == rhs_type)
    {
        std::string str = PopParamString();
        PushParamFloat(std::stod(str));
    }
    else
    {
        Error("Invalid cast to float.");
    }
}

void VM::ToInt()
{
    uint8_t rhs_type = PeekParamType();
    if (PARAM_INT == rhs_type) return; // noop

    if (PARAM_FLOAT == rhs_type)
    {
        float val = PopParamFloat();
        PushParamInt(val);
    }
    else if (PARAM_BOOL == rhs_type)
    {
        bool val = PopParamBool();
        if (val) { PushParamInt(1); }
        else { PushParamInt(0); }
    }
    else if (PARAM_STRING == rhs_type)
    {
        std::string str = PopParamString();
        int64_t x = 0;
        try
        {
            x = std::stol(str);
        }
        catch (std::invalid_argument const& ex)
        {
            x = 0;
        }
        catch (std::out_of_range const& ex)
        {
            x = 0;
        }
        PushParamInt(x);
    }
    else
    {
        Error("Invalid cast to int.");
    }
}


void VM::ToString()
{
    uint8_t rhs_type = PeekParamType();
    if (PARAM_STRING == rhs_type) return; // noop

    if (PARAM_FLOAT == rhs_type)
    {
        float val = PopParamFloat();
        std::string str = std::to_string(val);
        int addr = NewScratchString(str);
        PushParamString(addr);
    }
    else if (PARAM_BOOL == rhs_type)
    {
        bool val = PopParamBool();
        std::string str;
        if (val) { str = "true"; }
        else { str = "false"; }
        int addr = NewScratchString(str);
        PushParamString(addr);
    }
    else if (PARAM_INT == rhs_type)
    {
        int val = PopParamInt();
        std::string str = std::to_string(val);
        int addr = NewScratchString(str);
        PushParamString(addr);
    }
    else
    {
        Error("Invalid cast to string.");
    }
}


