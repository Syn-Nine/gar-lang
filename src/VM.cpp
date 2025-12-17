#include "VM.h"
#include "Environment.h"


#include <set>
#include <string>
#include <random>
#include <iostream>


double __rand_impl()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dist(0.0, 1.0);
    return dist(gen);
}

int64_t __rand_range_impl(int64_t lhs, int64_t rhs)
{
    if (lhs == rhs) return lhs;
    if (lhs > rhs) {
        int64_t temp = lhs;
        lhs = rhs;
        rhs = temp;
    }
    // todo - go back and profile
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(lhs, rhs);
    return dist(gen);
}

std::string console_input()
{
    char p[256];
    memset(p, 0, 256);
    std::cin.getline(p, 255);
    return std::string(p);
}

void VM::Initialize()
{
    // register standard library functions

    {
        StdFunc func = [this]() {
            uint8_t rhs_type = PopParamType();
            if (PARAM_INT != rhs_type) { Error("Invalid parameter type."); return; }
            int rhs = PopParamInt();
            uint8_t lhs_type = PopParamType();
            if (PARAM_INT != lhs_type) { Error("Invalid parameter type."); return; }
            int lhs = PopParamInt();
            int ret = __rand_range_impl(lhs, rhs);
            PushParamInt(ret);
            };
        size_t idx = m_stdlib.size();
        m_stdlib.push_back(func);
        m_stdlib_lookup.insert(std::make_pair("%randi", idx));
    }

    {
        StdFunc func = [this]() {
            std::string str = console_input();
            int addr = NewScratchString(str);
            PushParamString(addr);
            };
        size_t idx = m_stdlib.size();
        m_stdlib.push_back(func);
        m_stdlib_lookup.insert(std::make_pair("%input", idx));
    }
}


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
    SCRATCH_PTR = addr;

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
        case TOKEN_PUSH_SCRATCH_PTR:
        case TOKEN_POP_SCRATCH_PTR:
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

        case TOKEN_CALL:
        {
            i++;
            Token& rhs = m_bytecode[i];
            std::string lex = rhs.Lexeme();
            if (0 == m_stdlib_lookup.count(lex))
            {
                Error("Failed to find function: " + lex);
                break;
            }
            int idx = m_stdlib_lookup.at(lex);
            PushInstInt16(idx);
            row.append(ToHex2(idx));

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

    m_asm.append("\Instruction Load: " + std::to_string(INST_PTR) + "/" + std::to_string(MEM_INST_SZ) + " (" + std::to_string(int(INST_PTR * 100 / float(MEM_INST_SZ))) + "%)\n");
}


std::string VM::Dump()
{
    return m_asm;
}


void VM::Error(const std::string& err)
{
    m_errorHandler->Error("VM", INST_PTR - 1, err);
}

