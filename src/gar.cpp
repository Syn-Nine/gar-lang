#include <iostream>
#include <raylib.h>
#include <rlgl.h>
#include <fstream>

#include "ErrorHandler.h"
#include "Environment.h"
#include "Scanner.h"
#include "Token.h"
#include "Compiler.h"
#include "VM.h"

int main()
{
    const char* filename = "rps.g";
    char* code = LoadFileText(filename);
    if (!code) {
        printf("Failed to open source: %s\n", filename);
        std::getchar();
        return 0;
    }

    ErrorHandler* errorHandler = new ErrorHandler();
    Token::Initialize();

    Scanner scanner(code, errorHandler, filename, 1);
    TokenList tokens = scanner.ScanTokens();
    if (errorHandler->HasErrors()) {
        printf("Scanner Error:\n");
        errorHandler->Print();
        std::getchar();
        return 0;
    }

    //printf("%s:\n%s\n\n", filename, Token::Dump(tokens).c_str());
    printf("Input:\n%s\n\n", code);

    Environment* env = Environment::Push();
    Compiler compiler(tokens, env, errorHandler);
    TokenList bytecode = compiler.Compile();
    if (errorHandler->HasErrors()) {
        printf("Compiler Error:\n");
        errorHandler->Print();
        std::getchar();
        return 0;
    }
    env = Environment::Pop();
    
    //printf("Compiler Output:\n%s\n\n", Token::Dump(bytecode).c_str());

    VM vm(bytecode, env, errorHandler);
    vm.Initialize();
    vm.Assemble();
    printf("Assembler Output:\n%s\n", vm.Dump().c_str());
    if (errorHandler->HasErrors()) {
        printf("Assembler Error:\n");
        errorHandler->Print();
        std::getchar();
        return 0;
    }

    vm.Execute();
    if (errorHandler->HasErrors()) {
        printf("Runtime Error:\n");
        errorHandler->Print();
        std::getchar();
        return 0;
    }



    std::getchar();
}

