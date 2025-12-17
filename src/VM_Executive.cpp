#include "VM.h"

#include <stdexcept>

void VM::Execute()
{
    INST_PTR = 0;
    PARAM_PTR = MEM_PARAM_START;
    RET_PTR = MEM_RET_START;

    m_memory.block[PARAM_PTR] = PARAM_INVALID;

    printf("Executing...\n");
    bool quit = false;

    while (true)
    {
        TokenTypeEnum inst = (TokenTypeEnum)m_memory.block[INST_PTR];
        if (TOKEN_END_OF_FILE == inst) break;

        INST_PTR++;

        switch (inst)
        {
        case TOKEN_LOAD_BOOL: LoadBool(); break;
        case TOKEN_LOAD_INT: LoadInt(); break;
        case TOKEN_LOAD_FLOAT: LoadFloat(); break;
        case TOKEN_LOAD_STRING: LoadString(); break;
        case TOKEN_LOAD_VAR: LoadVar(); break;
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
        case TOKEN_PUSH_SCRATCH_PTR: PushScratchPtr(); break;
        case TOKEN_POP_SCRATCH_PTR: PopScratchPtr(); break;
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
            uint8_t type = PopParamType();
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
                int addr = PeekParamInt();
                std::string str = PopParamString();
                printf("%d: STRING: @%d=\"%s\"\n", cc, addr, str.c_str());
            }
            cc++;
        }
    }



    printf("Done.\n");
}


void VM::LoadBool()
{
    m_memory.block[++PARAM_PTR] = m_memory.block[INST_PTR++];
    PushParamType(PARAM_BOOL);
}

void VM::LoadFloat()
{
    PushParamInst4();
    PushParamType(PARAM_FLOAT);
}

void VM::LoadString()
{
    PushParamInst4();
    PushParamType(PARAM_STRING);
}

void VM::LoadInt()
{
    PushParamInst4();
    PushParamType(PARAM_INT);
}

void VM::LoadVar()
{
    size_t idx = ReadInstInt() * 5;
    PushParamVar(idx);
}

void VM::StoreVar()
{
    size_t idx = ReadInstInt() * 5;
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

    uint8_t rhs_type = PopParamType();
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

    uint8_t lhs_type = PopParamType();
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
    uint8_t rhs_type = PopParamType();
    if (PARAM_BOOL != rhs_type)
    {
        Error("AndOr parameter is not boolean.");
        return;
    }
    bool rval = PopParamBool();
    

    uint8_t lhs_type = PopParamType();
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

    uint8_t rhs_type = PopParamType();
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

    uint8_t lhs_type = PopParamType();
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
    uint8_t rhs_type = PopParamType();
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
    uint8_t rhs_type = PopParamType();
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

    uint8_t rhs_type = PopParamType();
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
    else
    {
        Error("Parameter type not recognized.");
    }

    if (newline) printf("\n");
    
}

void VM::IfJmp()
{
    uint8_t rhs_type = PopParamType();
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
    m_stdlib[addr]();
}

void VM::ToFloat()
{
    uint8_t rhs_type = PeekParamType();
    if (PARAM_FLOAT == rhs_type) return; // noop

    PopParamType();
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

    PopParamType();
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

    PopParamType();
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


void VM::PushScratchPtr()
{
    PushReturnInt(SCRATCH_PTR);
}

void VM::PopScratchPtr()
{
    SCRATCH_PTR = PopReturnInt();
}