#include "VM.h"
#include "Environment.h"

#include <set>
#include <string>

void VM::Assemble()
{
    m_asm = "[addr]    [inst]  [params]\n";
    memset(&m_memory, 0, sizeof(m_memory));

    INST_PTR = 0;

    std::string row;
    int comment_param_offset = 46;

    // create static memory block
    auto static_strings = m_env->GetStaticStrings();
    std::map<std::string, size_t> string_addrs;
    size_t addr = MEM_BLOCK_SZ; // fill in from the end
    uint8_t* data = m_memory.block;
    for (auto& str : static_strings)
    {
        int len = str.length();
        if (len > 255) len = 255; // length must fit into one byte
        addr -= len + 1;
        data[addr] = len;
        string_addrs.insert(std::make_pair(str, addr));
        for (int i = 0; i < len; ++i)
        {
            data[addr + 1 + i] = str.at(i);
        }
    }

    // process byte codes
    for (size_t i = 0; i < m_bytecode.size(); ++i)
    {
        Token& token = m_bytecode[i];
        TokenTypeEnum type = token.GetType();
        if (TOKEN_LN == type || TOKEN_NOOP == type) continue;
        
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
        case TOKEN_INV:
        case TOKEN_NEG:
        case TOKEN_AND:
        case TOKEN_OR:
        case TOKEN_EQUAL_EQUAL:
        case TOKEN_BANG_EQUAL:
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_PRINT:
        case TOKEN_PRINTLN:
        case TOKEN_END_OF_FILE:
            StartComment(row);
            row.append(token.Lexeme());
            break;

        case TOKEN_LABEL:
        {
            std::string lex = token.Lexeme();
            m_pp_labels.insert(std::make_pair(lex, INST_PTR));
            StartComment(row);
            row.append(token.Lexeme() + ":");
            break;
        }

        case TOKEN_IF:
        {
            i++;
            Token& rhs = m_bytecode[i];
            std::string lex = rhs.Lexeme();
            pp_jmps_s jmp;
            jmp.to_label = lex;
            jmp.from_inst = INST_PTR;
            m_pp_jmps.push_back(jmp);
            PushInstInt16(0);
            row.append(rhs.Lexeme());

            StartComment(row);
            row.append("if");
            PadComment(row, comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_JMP:
        {
            i++;
            Token& rhs = m_bytecode[i];
            std::string lex = rhs.Lexeme();
            pp_jmps_s jmp;
            jmp.to_label = lex;
            jmp.from_inst = INST_PTR;
            m_pp_jmps.push_back(jmp);
            PushInstInt16(0);
            row.append(rhs.Lexeme());
            
            StartComment(row);
            row.append("jmp");
            PadComment(row, comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_LOAD_BOOL:
        {
            i++;
            Token& rhs = m_bytecode[i];
            bool val = false;
            if (rhs.GetType() == TOKEN_TRUE) val = true;
            PushInstBool(val);
            row.append(ToHex1(val));
            
            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }
        
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

        case TOKEN_LOAD_STRING:
        {
            i++;
            Token& rhs = m_bytecode[i];
            int val = string_addrs.at(rhs.StringValue());
            PushInstInt(val);
            row.append(ToHex4(val));

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_LOAD_VAR: // intentional fall-through
        case TOKEN_STORE_VAR:
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
        }

        row.append("\n");
        m_asm.append(row);
    }

    // post process replace labels with addresses
    for (size_t i = 0; i < m_pp_jmps.size(); ++i)
    {
        pp_jmps_s& jmp = m_pp_jmps[i];
        size_t from_addr = jmp.from_inst;
        size_t to_addr = m_pp_labels[jmp.to_label];
        SetInstInt(from_addr, to_addr);
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