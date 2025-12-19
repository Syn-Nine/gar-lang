#include <iostream>
#include <raylib.h>
#include <rlgl.h>
#include <fstream>

#include "ErrorHandler.h"
#include "Environment.h"
#include "Scanner.h"
#include "Token.h"
#include "Compiler.h"
#include "Assembler.h"
#include "VM.h"

int main()
{
    const char* filename = "unit_test.g";
    char* code = LoadFileText(filename);
    if (!code) {
        printf("Failed to open source: %s\n", filename);
        std::getchar();
        return 0;
    }

    ErrorHandler* errorHandler = new ErrorHandler();
    Token::Initialize();

    Scanner* scanner = new Scanner(code, errorHandler, filename, 1);
    TokenList tokens = scanner->ScanTokens();
    if (errorHandler->HasErrors()) {
        printf("Scanner Error:\n");
        errorHandler->Print();
        std::getchar();
        return 0;
    }

    //printf("%s:\n%s\n\n", filename, Token::Dump(tokens).c_str());
    printf("Input:\n%s\n\n", code);

    Environment* env = Environment::Push();
    Compiler* compiler = new Compiler(tokens, env, errorHandler);
    TokenList ir = compiler->Compile();
    if (errorHandler->HasErrors()) {
        printf("Compiler Error:\n");
        errorHandler->Print();
        std::getchar();
        return 0;
    }
    env = Environment::Pop();
    
    //printf("Compiler Output:\n%s\n\n", Token::Dump(ir).c_str());
    VM* vm = new VM(env, errorHandler);

    Assembler* assembler = new Assembler(ir, env, errorHandler);
    uint8_t* bytecode = assembler->Assemble();
    printf("Assembler Output:\n%s\n", assembler->Dump().c_str());
    if (errorHandler->HasErrors()) {
        printf("Assembler Error:\n");
        errorHandler->Print();
        std::getchar();
        return 0;
    }

    vm->Execute(bytecode, assembler->GetHeapAddr(), assembler->GetEntryAddr());
    if (errorHandler->HasErrors()) {
        printf("Runtime Error:\n");
        errorHandler->Print();
        std::getchar();
        return 0;
    }

    delete vm;
    delete assembler;
    delete compiler;
    delete scanner;
    delete errorHandler;


    std::getchar();

    return 0;
}

