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
![t+14h screenshot](/images/t+14h.png)
