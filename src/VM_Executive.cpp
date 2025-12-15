#include "VM.h"


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
        case TOKEN_LOAD_INT: LoadInt(); break;
        case TOKEN_LOAD_FLOAT: LoadFloat(); break;
        case TOKEN_MINUS: // intentional fall-through
        case TOKEN_HAT: 
        case TOKEN_PLUS: 
        case TOKEN_SLASH:
        case TOKEN_STAR:
            BinaryOp(inst);
            break;
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
            if (PARAM_FLOAT == type)
            {
                int ival = PopParamInt();
                float fval;
                memcpy(&fval, &ival, 4);
                printf("%d: FLOAT : %f\n", cc, fval);
            }
            else if (PARAM_INT == type)
            {
                int val = PopParamInt();
                printf("%d: INT   : %d\n", cc, val);
            }
            cc++;
        }
    }



    printf("Done.\n");
}


void VM::LoadFloat()
{
    PushParamInst4();
    PushParamType(PARAM_FLOAT);
}

void VM::LoadInt()
{
    PushParamInst4();
    PushParamType(PARAM_INT);
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

    uint8_t rhs_type = PopParamType();
    if (PARAM_FLOAT == rhs_type)
    {
        isfloat = true;
        int ival = PopParamInt();
        memcpy(&frhs, &ival, 4);
    }
    else if (PARAM_INT == rhs_type)
    {
        irhs = PopParamInt();
        frhs = (float)irhs;
    }

    uint8_t lhs_type = PopParamType();
    if (PARAM_FLOAT == lhs_type)
    {
        isfloat = true;
        int ival = PopParamInt();
        memcpy(&flhs, &ival, 4);
    }
    else if (PARAM_INT == lhs_type)
    {
        ilhs = PopParamInt();
        flhs = (float)ilhs;
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

