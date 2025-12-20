## T+3h: Scope
This project is for [Langjam Gamejam 2025](https://langjamgamejam.com/) where you program a custom language and make a game in that language in 7 days. Due to work/school commitments I started the weekend before and am logging my hours here to keep with the spirit of the jam. I'm also planning to create branches at hour milestones to keep track of what the code looked like along the way.

First things first, I need to come up with a plan of attack, primarily high level scoping decisions that have large impact on success and schedule.

Microsoft recently announced they're embedding AI into Windows so I finally made the jump to Linux (Mint on Beeline SER8). However, I'm not proficient in debugging on Linux yet so I'll unfortunately be working in C++ using MSVC.

I think it'd be fun to revisit an old project of mine where I embedded bytecode instructions into a texture and processed them on a shader. I'd ultimately like for this to run in a VM on a compute shader, but I'll start with compiling to an intermediate representation and then assembling to bytecode and running in a VM on the CPU. 

```
Design Point: Implemented in C++ using MSVC.
Design Point: High level language that compiles to IR, then assembles to bytecode.
Design Point: Bytecode that runs in a VM on the CPU with stretch goal to implement VM on the GPU.
```

I'm going to make a simplified dialect of my [Tentacode](https://github.com/Syn-Nine/tentacode/tree/llvm) language. I really enjoy using that language, but would like to explore some syntax changes and simplifications. This'll also make programming the game for the jam much faster since the language will be familiar. I also think that the LLVM codegen in tentacode will serve as a good template for the new compiler and bytecode generation.

I'm thinking the bytecode will be a stack based FORTH-like because that's super simple to implement and I've done it before.  I'll also need a standard library, so the VM will need a way to call back out to functions. I plan to adapt the unit tests from tentacode because they cover many edge cases and will help find compiler/VM bugs when regression testing new language features.

```
Design Point: Bytecode language and interpreter to be a stack-based FORTH-like.
Design Point: Ability to call out to standard library code and get data back.
Design Point: The ability to print the IR and assembly code for debugging.
Design Point: Test driven language development using unit tests I've already made for tentacode.
```

Since the goal is to write a game, I only need the bare minimum language features to write the jam game. Whenever I learn a new language, I write text games for guess-the-number, rock-paper-scissors, and tic-tac-toe. I've found that these 3 games get me up to speed with a language the fastest. Looking back at my previous implementations of these games, here are the critical features the language will need:

```
Design Point, required language features:
- Variables - int, float, bool, string
- Arrays
- Loops with break and continue
- Print to console
- Input from console
- Type casting
- If/else condition statements
- Boolean logic
- Math with order of operations
- Trig functions
- Random number generation
- Function calls with return values
- Unary negate and not
- String concatenation
```

Right now I'm not entirely sure how I'll be able to implement half of those features within a shader, but we'll see how it goes. I'm expecting that I'll need to use Shader Storage Buffer Objects to store the program bytecode, instruction pointer, and state data. But I'll cross that bridge later.

I also want to make a 2D graphical game for the jam game. At a minimum I'll remake my simple game Catfish Bouncer and hopefully my viking knot puzzle game Knots.

### High level language constructs:

```
// and /* ... */ for comments
var x                   // variable allocation
var x = [...]           // array allocation
x = 5                   // variable assignment
x[2] = "test"           // array assignment
y = x                   // variable access

// unary ops:
x = -x                  // negate
y = !y                  // boolean invert

// type casting examples
a = true as string
b = 5.5 as string
c = 2 as string
d = "5" as int
e = "33.3" as float
k = a + "--" + b + "--" + c     // "true--5.5--2"

// function definitions
def name (x, y, z) { ... }
pass and return values via a forth-like parameter stack

// conditional statements
if condition { ... } else { ... }
logic operators: <, <=, >, >=, ==, !=, &&, ||, () for grouping expressions

// loops
while condition { ... }
for i in 0..N { ... }
loop { ... }

// math
normal operator precedence for +, -, *, /, ^, %
grouping using ()
```

This point marks 3 hours into the jam.

## T+7h: Compute shader prototype
I have a prototype that uses GLSL 4.3 with raylib to put a value in an SSBO, increment the value on a compute shader, and use the value in the fragment shader to control rendering. This is effectively a one bytecode machine proof of concept.

I'm going to hold off on posting the code for this until I get the CPU based VM working just in case I have to cut this from the project.

## T+14h: Start of compiler and VM
I have a simple compiler and VM running on the CPU that can compile math expressions into bytecodes and execute them using a parameter stack. The VM basically has a big memory arena as a surrogate for the SSBO that I'm packing and unpacking bytes from. The plan is for the arena to contain parameter, return, and variable stacks, as well as the instructions and pointers for each.

The lexer was pulled directly from my [Tentacode](https://github.com/Syn-Nine/tentacode/tree/llvm) language to save time. The lexer and compiler are both based on the wonderful book [Crafting Interpreters](https://craftinginterpreters.com/). The file gar.cpp has the main() function which loads a source file, scans it for tokens using the Scanner class, compiles it using the Compiler class, then assembles and runs using the VM class. Expressions pop and push values to the parameter stack like in FORTH. At the end of execution, the VM shows the state of the parameter stack to confirm if the math expressions worked correctly.

Here's a screenshot of the output so far:

![t+14h screenshot](https://github.com/Syn-Nine/gar-lang/blob/T+14h/images/t+14h.png?raw=true)

## T+20h: Strings, Printing, Booling Logic, Variables
This update adds literals for true and false, as well as strings. Strings get stored in a static memory block during assembly. When pushed to the parameter stack the parameter holds the address to the static memory location. At this location is the length of the string and the string bytes in ascii. I've also started an Environment class to hold onto variable information during compiling so that the compiler can check if a variable exists before assignment and determine which location in the variable stack to store and load values from. These values can also be pushed/popped from the parameter stack and used in boolean logic expressions. If/else blocks are now supported with jump statements in the assembly based on the conditions. The comparison/and/or operators work like in FORTH where they pop two values from the parameter stack and push a boolean back to the parameter stack representing true or false. This boolean is then consumed using the if-jump bytecode.

Screenshot of boolean logic:

![t+20h screenshot logic](https://github.com/Syn-Nine/gar-lang/blob/T+20h/images/t+20h_logic.png?raw=true)

Screenshot of static string references:

![t+20h screenshot strings](https://github.com/Syn-Nine/gar-lang/blob/T+20h/images/t+20h_str.png?raw=true)


## T+25h: Casting, Loops, Stdlib, 2 Games
I now have the ability to cast from bool/float/int to string, and from string to float/int using the "as" keyword. Casting to string creates a temporary string in a scratch region just below the static data at the end of the arena. To support this I've added environment scoping for the scratch stack and variable stack. The compiler will search up the scope tree to find a variable if needed.

This update also includes loop, while, and for statements. All of these are implemented in a While loop statement under the hood. These are basically scope blocks that have jump logic to jump back to the top if a condition is met, or else jump to a tail label and continue on. Continue and Break keywords are provided to short circuit back to the top or bottom labels respectively.

I've started to thinking about how to implement standard library calls from within the code. There is an array of anonymous functions that is created when the VM initializes. The index into the array is used by the "call" bytecode, which calls back to the function. Inside the function it pops values from the parameter stack, performs the desired operation, and pushes a result if applicable.

This was stable enough that I was able to make a text games for guess-the-number and rock-paper-scissors. I found myself wanting to have semi-colons between instructions to improve readability so I made the lexer ignore them for now so I can use them but they don't mean anything syntactially.

At this point, the overall goal for the jam has been met, I've made a game in custom language in roughly 25 hours, but why stop here?

The next big features are arrays and functions, which means it's time to do the first big refactor using the lessons learned so far. I like that there is a temporary scratch arena for variable length temporary data at the back of the variable stack, but this was more of hack just to get temporary string operations in and should be replaced. I will need a moving function frame pointer so that I can call functions recursively. I want to add constants and need to differentiate between globals and local addresses so the variable stack needs major rework anyway with the inclusion of something like LLVM's alloca statement.

Right now I have a return stack that is holding the unwinding information for the temporary scratch arena so that it can get cleaned up automatically at the end of basic blocks. Since I need to put in function calling I'm going to need something better. I'm thinking I'll remove the forth-like return stack and turn it into a call stack that also holds scratch data. Then I'll explicitly track the block pointer, previous function frame pointer, return address, and temporary scratch pointer.

Screenshot of guess-the-number:

![t+25h guess-the-number](https://github.com/Syn-Nine/gar-lang/blob/T+25h/images/t+25h_guess.png?raw=true)

Screenshot of rock-paper-scissors:

![t+25h rock-paper-scissors](https://github.com/Syn-Nine/gar-lang/blob/T+25h/images/t+25h_rps.png?raw=true)

## T+32h: First Major Refactor
In this update I broke apart the Assembler from the VM and refactored the Stdlib interface to make it more scalable. I removed the forth-like return stack and moved stratch memory allocation to the call stack. As part of the progress toward having functions with local variables, I added a globals variable block and an alloca instruction with variable hoisting to allocate space on the call stack with a moveable frame pointer. I've added a pointer to keep track of the start of basic blocks so that temporary data inside a basic block scope can be freed by unwinding the pointer at the end of the block. I also started some prep work for an enum type that will be an auto incremented integer at compile time.


## T+43h: Enums, Lists, Functions, Tic-Tac-Toe
This update finished the work to support enums which are values in the form of :NAME. These values get pre-processed by the compiler into unique integer values through the Environment class. I also added constants, which is just a flag in the environment variable definitions to prevent overwriting except during declaration.

I removed scratch data from the frame stack and put into scratchpad ring buffer. This basically reverted a bunch of work that I did in the previous update which is unfortunate, but ended up much better overall. This removed the need to keep track of blocks and unwinding the frame pointer if you break/continue in a loop.

I learned about the pad ring buffer from Madgarden. It's an allocated block of memory that temporary allocations can live even as you move around through scope blocks and function jumps. This provides a small timeline where a temporary string or list can be created, passed around, and operated on. However, since it's a ring buffer that data will get overwritten soon so it's best to use it immediately. When something is stored in scratch a calculation is performed to determine if the scratch pointer + number of bytes needed will go past the end of the buffer, if so, the storage location is set to the start of the buffer to prevent having to add wrapping logic during reading and writing.

Because of this limited lifetime it was time to create a heap and a new() statement to move things out of scratch to the heap. For now data gets allocated on the heap in a stack and there is no delete() keyword. This is mainly for time since the jam is only 7 days, but it's also a good motivator to do your allocations early in the code execution. When using the new() keyword on a list or string, the contents will be deeply copied to the heap. This way if you have a list with lists or strings inside it they will make the jump as well. The scratch pad, heap, and call frame stack make up the final portion of the VM's memory block. I'll be adding an architecture section with pictures below before the jam is over. 

List operations use the new assembly tokens "load@" and "save@". Lists on the parameter stack store an int32 with the address of the data. At this address is an int16 length, followed by 5 byte values so that lists can hold any type, including other lists. A new assembly token "makelist" is used to unpack a certain quantity of values from the parameter stack and place them onto the scratchpad.

Screenshot of makelist in action:

![t+43h makelist in action](https://github.com/Syn-Nine/gar-lang/blob/T+43h/images/t+43h_makelist.png?raw=true)

To access a list the address is unpacked and placed on the parameter stack, then square brackets are used to identify where you want to load from or save to. The contents of the bracket get evaluated and pushed to the parameter stack, then "load@" and "save@" pop those two values to calculate the address and offset that will be used. "load@" works just like the other loads and puts the contents to the parameter stack. If the target is another list, then the value added to the parameter stack is the address that list points to, which allows brackets to be chained.

Screenshot of list index chaining:

![t+43h list index chaining](https://github.com/Syn-Nine/gar-lang/blob/T+43h/images/t+43h_loadat.png?raw=true)


This update also added Functions. During code generation, alloca and store statements reserve room on the frame stack for the incoming parameters and pop the parameter stack values into these variables. Addresses for the variables use negative numbers to differentiate them from addresses that point to the scratchpad or heap during lookup. A new "calldef" assembly token was created to differentiate jumps to user defined functions vs stdlib functions which use "call". The "calldef" token is followed by an int16 with the instruction address to jump to, whereas "call" is an index in the stdlib callback function vector. Returning a value from a function is as simple as having the last statement be an expression that results in a value being placed on the parameter stack.

Screenshot of function definition with return value:

![t+43h function definition with return value](https://github.com/Syn-Nine/gar-lang/blob/T+43h/images/t+43h_func.png?raw=true)

At this point the required features were in to support a Tic-Tac-Toe text game ([gar source code](https://github.com/Syn-Nine/gar-lang/blob/T+43h/games/tictactoe.g?raw=true)). The repo has all the games in the games/ folder.


Screenshot of Tic-Tac-Toe:

![t+43h tic-tac-toe](https://github.com/Syn-Nine/gar-lang/blob/T+43h/images/t+43h_tictactoe.png?raw=true)


## T+47h: Raylib support, 2D Minigame, and more unit tests
This update brings in raylib support so that I could remake my mini dual axis pong-like game Catfish Bouncer, where you use the mouse to bounce fish pellets to a cat to eat. The game experience is exactly the same as my previous ports to other languages, so I'm just including a gif of the Odin version below. A VM_Raylib.cpp file was added that has the stdlib functions for the bare minimum raylib functions needed for the game, like creating/closing the window, getting mouse input, colors, and drawing primitives.

You can find a bunch of unit tests in [unit_test.g](https://github.com/Syn-Nine/gar-lang/blob/T+47h/unit_test.g?raw=true) covering basic types, casting, variable redefinition, logic, loops, math, lists, and functions. This is a more thorough, consolidated set of tests that I've been using for test-driven-development throughout this project. Through making these tests I realized that I hadn't implemented the ^ and % operators and found various bugs that were cleaned up. I also added a "len()" keyword to get the length of lists and strings. This essentially just returns the value of the int16 at the object address that contains the data length.

GIF of Catfish Bouncer:

![t+47h catfish bouncer gif](https://github.com/Syn-Nine/odin-mini-games/raw/main/2d-games/catfish/screenshots/catfish.gif)

## T+52h: Include, Return, More Raylib, Knots Game
This update brings in the include statement, which injests a file and inserts the tokens in place immediately after the include statement. This works in a loop so that you can have nested includes. There is no namespacing in includes to save time. A return statement will now inject a RET bytecode to allow early return in a function. This also unwinds the function pointer. Returning a value works like regular function returns where you put the value on the parameter stack before the return statement. I also fixed some bugs, one with the main entry point instruction offset, and one with boolean varables pushing and popping from the parameter stack.

The big goal for this update was to port over my "Knots" puzzle game from Rust. This required adding support for loading and drawing raylib textures. To do this I added an array in the VM that holds pointers to Texture2D raylib structures. The index in this array gets stored in a variable after you call ray::LoadTexture() and is then used to find the pointer when calling ray::DrawTexturePro(). Which means the data lives in the compiler's heap instead of the VM's heap, which was a shortcut for time.

The language executable gar.exe now accepts two input parameters. You can specify the file to run, and alternatively add a -d flag before the filename to show the compiler, assembler, and stack output for debugging.

Here's a couple screenshots showing the Knots game in action and what the final solution looks like. Also, [here is the game code](https://github.com/Syn-Nine/gar-lang/blob/T+52h/games/knots.g?raw=true).

GIF of Knots Game:

![t+52h knots gif](https://github.com/Syn-Nine/gar-lang/blob/T+52h/images/t+52_knots-1.png?raw=true)

Knots Solution:

![t+52h knots solution](https://github.com/Syn-Nine/gar-lang/blob/T+52h/images/t+52_knots-solution.png?raw=true)
