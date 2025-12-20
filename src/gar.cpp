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

void RunFile(std::string filename, bool debug)
{
    char* code = LoadFileText(filename.c_str());
    if (!code) {
        printf("Failed to open source: %s\n", filename);
        return;
    }

    ErrorHandler* errorHandler = new ErrorHandler();
    Token::Initialize();

    Scanner* scanner = new Scanner(code, errorHandler, filename.c_str(), 1);
    TokenList tokens = scanner->ScanTokens();
    if (errorHandler->HasErrors()) {
        printf("Scanner Error:\n");
        errorHandler->Print();
        return;
    }

    //printf("%s:\n%s\n\n", filename, Token::Dump(tokens).c_str());
    if (debug) printf("Input:\n%s\n\n", code);

    Environment* env = Environment::Push();
    Compiler* compiler = new Compiler(tokens, env, errorHandler);
    TokenList ir = compiler->Compile();
    if (errorHandler->HasErrors()) {
        printf("Compiler Error:\n");
        errorHandler->Print();
        return;
    }
    env = Environment::Pop();

    //printf("Compiler Output:\n%s\n\n", Token::Dump(ir).c_str());
    VM* vm = new VM(env, errorHandler);

    Assembler* assembler = new Assembler(ir, env, errorHandler);
    uint8_t* bytecode = assembler->Assemble();
    if (debug) printf("Assembler Output:\n%s\n", assembler->Dump().c_str());
    if (errorHandler->HasErrors()) {
        printf("Assembler Error:\n");
        errorHandler->Print();
        return;
    }

    vm->Execute(bytecode, assembler->GetHeapAddr(), assembler->GetEntryAddr(), debug);
    if (errorHandler->HasErrors()) {
        printf("Runtime Error:\n");
        errorHandler->Print();
        return;
    }

    delete vm;
    delete assembler;
    delete compiler;
    delete scanner;
    delete errorHandler;


}

int main(int nargs, char* argsv[])
{
    std::string filename;
    bool debug = false;

    if (1 == nargs)
    {
        std::ifstream f;
        f.open("autoplay.g", std::ios::in | std::ios::binary | std::ios::ate);
        if (f.is_open())
        {
            filename = "autoplay.g";
        }
        else
        {
            printf("Failed to open autoplay.g\n");
            return 0;
        }
    }
    else if (2 == nargs)
    {
        filename = argsv[1];
        printf("Run File: %s\n", filename.c_str());
    }
    else if (3 == nargs)
    {
        filename = argsv[2];
        if (0 == std::string(argsv[1]).compare("-d")) debug = true;
        printf("Run File: %s\n", filename.c_str());
    }

    RunFile(filename, debug);

    //std::getchar();

    return 0;
}

