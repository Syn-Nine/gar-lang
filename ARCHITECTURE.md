# Architecture

This document describes how the organization and functionality of the Gar language repo.

## Files

The repo files are described below in order of utilization when running a program.

### gar.cpp

The main entry point is gar.cpp. This provides the terminal argument interface for specifying which file to compile and run. If you don't provide any arguments it'll automatically try to run "autoplay.g". If you specify 1 argument it'll treat the argument as a file to load. If you specify "-d" before the filename it'll enable debug output for the compiler, assembler, VM, and parameter stack.

After deciding what file to run it creates a Scanner (lexer), Environment, Compiler, Assembler, VM, and then executes the final bytecode in the VM.

## Scanner

The scanner (lexer) was implemented based on [Crafting Interpreters](https://craftinginterpreters.com/). Its job is to turn the input file text into a stream of tokens. The ScanToken() function does the heavy lifting to include parsing out comments.

## Token

These files hold the enumeration for the high level and low level language tokens. The high level tokens are created during the file scanning process, whereas the low level tokens are created during the compiling process. The Initialize function is used to map enumerations to the text values as well as specify which tokens are language keywords.

## Environment

The Environment class is used for associating memory addresses with variables, making unique enumeration symbols, searching for variables recursively using scope hierarchy rules, and keeping track of user defined function metadata and loop metadata. Moreover, loops use the environment to keep track of the nearest entry and tail labels for the loop to support the break and continue keywords. Scoping boundaries are created using the Environment::Push() and Pop() functions.

## Compiler

The Compiler class converts the input tokens into intermediate representation (IR) tokens primarily using a Visitor Pattern similar to the Crafting Interpreters book. The IR tokens use a mix of low level assembly bytecodes and high level language bytecodes as well as keep track of some extra metadata about the tokens. The compiler also heavily interacts with the environment class to hold information during code generation. Inside Compiler.cpp you'll find it to follow the same parsing patterns as Scanner.cpp, except operating on tokens instead of text characters.

The compiler's logic starts in Compile.h in the Compile function. You can essentially follow the code vertically downward in this file to walk the syntax tree. Each function returns a vector of tokens that get appended to the end of the instruction stack. Some statements return a NOOP (no operation) because the result is consumed, such as declaring a variable, or a function that holds on to its body code for inserting later in the assembly phase. Many other statements and expressions result in an instruction that pushes or pops a value to the parameter stack like in FORTH.

## Expressions and Statements

The two classes contain the code generation for statements and expressions. A statement performs an operation without returning a value on the parameter stack, whereas an expression typically involves popping and pushing to the parameter stack based on an operation.

## Assembler, Assembler_Utils

The assembler's job is to combine the IR tokens from the compiler with information in the environment to build a ROM instruction block. This is a 16 bit memory block to hold read only instruction code. All final program code is stored in this block and executed by the VM. This means that program instructions can only be addressed up to 65535, an intentional limitation for this project.

The Assembler creates a block of memory that will be later used by the VM. This block of memory also includes static data that is known at compile time, as well as room for various stacks during runtime.

All variables are 5 bytes wide with 1 byte for the type and 4 bytes for the data. At the end of the instruction block starts the static block where the assembler reserves space for const and global variables as well as static strings. Strings start with 2 bytes for the length, and then one byte for each character. Once all the required items are stored in the static block the final address becomes the start of the heap that will be used for dynamic allocation.

Next, function entry labels are stored for future lookup and then function contents are emitted. Functions add alloca statements first to make space on the call stack for incoming parameters, then the parameters are unloaded in reverse order off the stack into those local variable storage locations.

The ProcessIR() function does the bulk of the work inserting instructions into function bodies and in the global scope. This function walks through each of the IR tokens and emits one or more low level byte codes based on the IR token type and metadata. One of the important jobs of the assembler is to figure out what the destination addresses will be for jump and calldef instructions. This is done by first keep track of the destination label string names and then going back and replacing those instruction locations with int16 address values in a second pass.

Assembler_Utils.cpp includes some extra utilites for byte packing and unpacking in the instruction block. During assembly, a pretty-printed output is also created to aid in debugging.

## VM

The VM.h holds the definition for how large the executive memory block will be. This block has room for the instruction bytecode, scratchpad, parameter stack, static memory block, heap, and call stack, as well as storage for pointers for addressing into those locations.

VM_Utils.cpp has lots of utilities for pushing, popping, and peeking memory at different bit widths. It's organized into areas for the parameter stack, scratchpad, heap, and call (frame) stack. VM_Stdlib and VM_Raylib have the callback functions that get executed when the VM hits a call bytecode to execute a standard library function.

VM_Executive is where most of the action happens during program execution. This is a switch loop over the instruction block by reading the address currently pointed to by the instruction pointer and then performing an operation. Each fundamental instruction is associated with a short function that performs the operation, such as putting values to the parameter stack, performing math operations, get/set values in variables, cast data types, and boolean logic operations.

When boolean logic operations are encountered they behave like in FORTH where values are popped off the parameter stack and replaced by a true or false. Then the trailing "if" expression pops the boolean off the stack to determine if it should jump or not. Jmp, calldef, and ret simply move the instruction pointer so that the next loop will read and execute the instruction at the new location.

The instruction block is at the beginning of the memory block. Data stored in the heap grows upward toward the call stack and data in the call stack grows downward. Right now there is no error reporting for if they meet. Lists and strings are stored in the scratchpad ring buffer, but can be deeply copied into the heap using the "new" keyword. There is currently no delete keyword or any other heap management utilities. Here's a picture of the memory block structure with size in bytes.

![VM memory block](https://github.com/Syn-Nine/gar-lang/blob/main/images/memory.png?raw=true)

That's it, thanks for reading!
