#include "VM.h"

#include <set>
#include <string>

void VM::Assemble()
{
    m_asm = "[addr]    [inst]  [params]\n";
    memset(&m_memory, 0, sizeof(m_memory));

    INST_PTR = 0;

    std::string row;
    int comment_param_offset = 46;

    // process byte codes
    for (size_t i = 0; i < m_bytecode.size(); ++i)
    {
        Token& token = m_bytecode[i];
        TokenTypeEnum type = token.GetType();
        if (TOKEN_LN == type) continue;
        
        char inst = int(type);

        row = ToHex2(INST_PTR);
        row.append("    ");
        row.append(ToHex1(inst));
        row.append("    ");

        PushInst(inst);
        
        switch (type)
        {
        case TOKEN_MINUS: // intentional fall-through
        case TOKEN_HAT:
        case TOKEN_PLUS:
        case TOKEN_SLASH:
        case TOKEN_STAR:
        case TOKEN_END_OF_FILE:
            StartComment(row);
            row.append(token.Lexeme());
            break;
        
        case TOKEN_LOAD_FLOAT:
        {
            i++;
            Token& rhs = m_bytecode[i];
            float fval = rhs.DoubleValue();
            int ival;
            memcpy(&ival, &fval, 4);
            PushInstInt(ival);
            row.append(ToHex4(ival));
            
            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }
        
        case TOKEN_LOAD_INT:
        {
            i++;
            Token& rhs = m_bytecode[i];
            int val = rhs.IntValue();
            PushInstInt(val);
            row.append(ToHex4(val));
            
            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        row.append("\n");
        m_asm.append(row);
    }
}


std::string VM::Dump()
{
    return m_asm;
}


void VM::Error(const std::string& err)
{
    m_errorHandler->Error("VM", INST_PTR, err);
}