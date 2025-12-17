#include "Assembler.h"


uint8_t* Assembler::Assemble()
{
    m_asm = "";
    memset(m_code, 0, VM::MEM_INST_SZ);

    m_inst_ptr = 0;

    std::string row;
    int comment_param_offset = 46;

    size_t addr = VM::MEM_STATIC_START;

    // reserve memory for globals
    auto globals = m_env->GetGlobals();
    for (auto& var : globals)
    {
        m_asm.append("global %" + var + "\n");
        addr += 5;
    }
    
    // fill in constants
    

    // fill in static strings
    auto static_strings = m_env->GetStaticStrings();
    std::map<std::string, size_t> string_addrs;
    
    for (auto& str : static_strings)
    {
        m_asm.append("static \"" + str + "\"\n");
        int len = str.length();
        if (len > 255) len = 255; // length must fit into one byte
        m_code[addr] = len;
        string_addrs.insert(std::make_pair(str, addr));
        addr += 1;
        for (int i = 0; i < len; ++i)
        {
            m_code[addr] = str.at(i);
            addr += 1;
        }
    }

    m_asm.append("\n[addr]    [inst]  [params]\n");
    
    // reserve memory for function locals
    auto locals = m_env->GetLocals("");
    for (auto& var : locals)
    {
        char inst = int(TOKEN_ALLOCA);
        row = ToHex2(m_inst_ptr);
        row.append("    ");
        row.append(ToHex1(inst));
        row.append("    ");

        PushInst(inst);

        StartComment(row);
        row.append("alloca %" + var);

        row.append("\n");
        m_asm.append(row);
    }


    // process byte codes
    for (size_t i = 0; i < m_ircode.size(); ++i)
    {
        Token& token = m_ircode[i];
        TokenTypeEnum type = token.GetType();
        if (TOKEN_LN == type || TOKEN_NOOP == type) continue;

        char inst = int(type);

        row = ToHex2(m_inst_ptr);
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
        case TOKEN_CAST_FLOAT:
        case TOKEN_CAST_INT:
        case TOKEN_CAST_STRING:
        case TOKEN_EQUAL_EQUAL:
        case TOKEN_BANG_EQUAL:
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_PRINT:
        case TOKEN_PRINTLN:
        case TOKEN_PRINT_BLANK:
        case TOKEN_BLOCK_START:
        case TOKEN_BLOCK_END:
        case TOKEN_END_OF_FILE:
            StartComment(row);
            row.append(token.Lexeme());
            break;

        case TOKEN_LABEL:
        {
            std::string lex = token.Lexeme();
            m_pp_labels.insert(std::make_pair(lex, m_inst_ptr));
            StartComment(row);
            row.append(token.Lexeme() + ":");
            break;
        }

        case TOKEN_IF:
        {
            i++;
            Token& rhs = m_ircode[i];
            std::string lex = rhs.Lexeme();
            pp_jmps_s jmp;
            jmp.to_label = lex;
            jmp.from_inst = m_inst_ptr;
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
            Token& rhs = m_ircode[i];
            std::string lex = rhs.Lexeme();
            pp_jmps_s jmp;
            jmp.to_label = lex;
            jmp.from_inst = m_inst_ptr;
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
            Token& rhs = m_ircode[i];
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
            Token& rhs = m_ircode[i];
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
            Token& rhs = m_ircode[i];
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
            Token& rhs = m_ircode[i];
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
            Token& rhs = m_ircode[i];
            int val = rhs.IntValue();
            PushInstInt(val);
            row.append(ToHex4(val));

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_CALL:
        {
            i++;
            Token& rhs = m_ircode[i];
            std::string lex = rhs.Lexeme();
            int addr = VM::GetStdlibAddr(lex);
            if (-1 == addr)
            {
                Error("Failed to find function: " + lex);
                break;
            }
            PushInstInt16(addr);
            row.append(ToHex2(addr));

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

    m_asm.append("\Instruction Load: " + std::to_string(m_inst_ptr) + "/" + std::to_string(VM::MEM_INST_SZ) + " (" + std::to_string(int(m_inst_ptr * 100 / float(VM::MEM_INST_SZ))) + "%)\n");

    return m_code;
}


std::string Assembler::Dump()
{
    return m_asm;
}
