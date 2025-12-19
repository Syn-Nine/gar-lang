#include "Assembler.h"
#include "VM.h"

uint8_t* Assembler::Assemble()
{
    m_asm = "";
    memset(m_code, 0, VM::MEM_INST_SZ);

    m_inst_ptr = 0;

    std::string row;
    m_comment_param_offset = 46;

    size_t addr = VM::MEM_STATIC_START;

    // reserve memory for globals
    auto globals = m_env->GetGlobals();
    for (auto& var : globals)
    {
        if (var.is_const) m_asm.append("const global %" + var.lexeme + "\n");
        else m_asm.append("global %" + var.lexeme + "\n");
        addr += 5;
    }
    
    // fill in constants
    

    // fill in static strings
    auto static_strings = m_env->GetStaticStrings();
    
    for (auto& str : static_strings)
    {
        m_asm.append("static \"" + str + "\"\n");
        int len = str.size();
        
        if (len > VM::MEM_SCRATCH_SZ-2) len = VM::MEM_SCRATCH_SZ-2; // length must fit into two bytes

        uint8_t a = len & 0xFF;
        uint8_t b = (len >> 8) & 0xFF;
        m_string_addrs.insert(std::make_pair(str, addr));
        
        m_code[addr++] = b;
        m_code[addr++] = a;

        for (int i = 0; i < len; ++i)
        {
            m_code[addr++] = str.at(i);
        }
    }

    m_heap_start = addr;

    m_asm.append("\n[addr]    [inst]  [params]\n");

    
    // get function entry labels
    auto functions = m_env->GetFunctions();
    for (auto& f : functions)
    {
        std::string name = f.first;
        Environment::func_s ftn = f.second;
        m_function_entries.insert(std::make_pair(name, ftn.entry));
    }
    
    // insert function code
    for (auto& f : functions)
    {
        std::string name = f.first;
        Environment::func_s ftn = f.second;
        
        m_asm.append("\ndef " + name + ":\n");
        // insert prototype
        ProcessIR(ftn.proto);

        // reserve memory for function locals
        auto locals = m_env->GetLocals(f.first);
        for (auto& var : locals)
        {
            char inst = int(TOKEN_ALLOCA);
            row = ToHex2(m_inst_ptr);
            row.append("    ");
            row.append(ToHex1(inst));
            row.append("    ");

            PushInst(inst);

            StartComment(row);
            row.append("alloca %" + var.lexeme);

            row.append("\n");
            m_asm.append(row);
        }

        // insert code block
        ProcessIR(ftn.body);
    }

    // reserve memory for main entry locals
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
        if (var.is_const) row.append("const alloca %" + var.lexeme);
        else row.append("alloca %" + var.lexeme);

        row.append("\n");
        m_asm.append(row);
    }

    m_asm.append("\nmain:\n");
    ProcessIR(m_ircode);

    // post process replace labels with addresses
    for (size_t i = 0; i < m_pp_jmps.size(); ++i)
    {
        pp_jmps_s& jmp = m_pp_jmps[i];
        size_t from_addr = jmp.from_inst;
        size_t to_addr = m_pp_labels[jmp.to_label];
        SetInstInt(from_addr, to_addr);
    }
    
    m_main_entry = m_pp_labels.at("__main");

    m_asm.append("Instruction Load: " + std::to_string(m_inst_ptr) + "/" + std::to_string(VM::MEM_INST_SZ) + " (" + std::to_string(int(m_inst_ptr * 100 / float(VM::MEM_INST_SZ))) + "%)\n");

    return m_code;
}


void Assembler::ProcessIR(IRCode ircode)
{
    std::string row;

    // process byte codes
    for (size_t i = 0; i < ircode.size(); ++i)
    {
        Token& token = ircode[i];
        TokenTypeEnum type = token.GetType();
        if (TOKEN_LN == type || TOKEN_NOOP == type) continue;
        if (TOKEN_CALL == type)
        {
            if (0 != m_function_entries.count(ircode[i + 1].Lexeme()))
            {
                // replace with calldef to user definition
                token = Token(TOKEN_CALL_DEF, token.Line(), token.Filename());
                type = token.GetType();
            }
        }

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
        case TOKEN_PERCENT:
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
        case TOKEN_LOAD_AT:
        case TOKEN_STORE_AT:
        case TOKEN_RET:
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
            Token& rhs = ircode[i];
            std::string lex = rhs.Lexeme();
            pp_jmps_s jmp;
            jmp.to_label = lex;
            jmp.from_inst = m_inst_ptr;
            m_pp_jmps.push_back(jmp);
            PushInstInt16(0);
            row.append(rhs.Lexeme());

            StartComment(row);
            row.append("if");
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_JMP:
        {
            i++;
            Token& rhs = ircode[i];
            std::string lex = rhs.Lexeme();
            pp_jmps_s jmp;
            jmp.to_label = lex;
            jmp.from_inst = m_inst_ptr;
            m_pp_jmps.push_back(jmp);
            PushInstInt16(0);
            row.append(lex);

            StartComment(row);
            row.append("jmp");
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_LOAD_BOOL:
        {
            i++;
            Token& rhs = ircode[i];
            bool val = false;
            if (rhs.GetType() == TOKEN_TRUE) val = true;
            PushInstBool(val);
            row.append(ToHex1(val));

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_LOAD_FLOAT:
        {
            i++;
            Token& rhs = ircode[i];
            float fval = rhs.DoubleValue();
            int ival;
            memcpy(&ival, &fval, 4);
            PushInstInt(ival);
            row.append(ToHex4(ival));

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_LOAD_INT:
        {
            i++;
            Token& rhs = ircode[i];
            int val = rhs.IntValue();
            PushInstInt(val);
            row.append(ToHex4(val));

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_LOAD_STRING:
        {
            i++;
            Token& rhs = ircode[i];
            int val = m_string_addrs.at(rhs.StringValue());
            PushInstInt(val);
            row.append(ToHex4(val));

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_LOAD_VAR: // intentional fall-through
        case TOKEN_STORE_VAR:
        {
            i++;
            Token& rhs = ircode[i];
            int val = rhs.IntValue();
            PushInstInt(val);
            row.append(ToHex4(val));

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_MAKE_LIST:
        {
            i++;
            Token& rhs = ircode[i];
            int val = rhs.IntValue();
            PushInstInt16(val);
            row.append(ToHex2(val));

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_CALL:
        {
            i++;
            Token& rhs = ircode[i];
            std::string lex = rhs.Lexeme();
            int addr = VM::GetFuncAddr(lex);
            if (-1 == addr)
            {
                Error("Failed to find function: " + lex);
                break;
            }
            PushInstInt16(addr);
            row.append(ToHex2(addr));

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }

        case TOKEN_CALL_DEF:
        {
            i++;
            Token& rhs = ircode[i];
            std::string lex = rhs.Lexeme();
            pp_jmps_s jmp;
            jmp.to_label = m_function_entries.at(lex);
            jmp.from_inst = m_inst_ptr;
            m_pp_jmps.push_back(jmp);
            PushInstInt16(0);
            row.append(lex);

            StartComment(row);
            row.append(token.Lexeme());
            PadComment(row, m_comment_param_offset);
            row.append(rhs.Lexeme());
            break;
        }
        }

        row.append("\n");
        m_asm.append(row);
    }
}


std::string Assembler::Dump()
{
    return m_asm;
}
