# RPC-Lang
A simple, toy programming language and compiler created using LLVM

# Requirements
 - LLVM version 16 or later. I installed using the shell script on https://apt.llvm.org/ which made it very easy
 - A C++ compiler. I used g++ though I'm sure clang++ would work too.

# Features
* 3 primitive types: 32 bit integer, 64 bit float (double), character (8 bit integer)
  - Implicit casting between types
* Arrays of primitive types, capable of supporting an arbitrary number of dimensions
* Global and local variables
* Control flow
	- For loops
	- While loops
	- If/else statements
* Functions
* Support for all major arithmetic and logical operators
* I/O using existing C++ functions
* And more!

# Syntax
Single line comments are declared with $

## Declaring Variables

To declare a variable, use the ‘var’ keyword, followed by a type and identifier. Primitive variables must be initialized with a value, while arrays cannot be initialized with a value.
The size of an array must also be declared with an integer literal value, as all arrays are stack allocated.
```
…
var i32 integer <- 0;
var f64 double <- 0.0;
var ch8 character <- '0';
…

var i32 x;  $ error
```

```
…
var i32[5] integer;
integer <- {1 2 3};

var f64[4] double;
double <- {1.0 2.5};

var ch8[3] character;
character <- {'a' 'b'};
…

var i32 x <- {1 2 3};  $ error
```
Character arrays can also be initialized with double quotes like in C, to automatically append a null character to the end of the array.

```
…
var ch8[3] character;
character <- "Hi";
…
```

When initializing an array with brackets, values in the bracket must be literals of the same type, meaning no variables. This is also one of the
only situations where primitive types will not be implicitly casted.

```
var f64[4] double;
double <- {1.0 2};  $ Error, 2 is an i32
```

## Control Flow
All control flow statements must have braces, even if there is only 1 statement in the loop.
### For Loops
For loops are defined similar to any C-like language:
```
for (var i32 i <- 0; i < 10; i <- i + 1) {
  $ stuff
}
```
### While Loops
While loops run as long as the condition holds true. 1 = true while 0 = false. Logical operators
can also be used to evaluate conditions.
```
while (1) { $ same as while (true) in C++
  $ stuff
}
```
### If Statements
If statements act like those in any other language, except instead of else statements, I made it "otherwise" statements, which are optional
```
var i32 bool <- 0;
if (bool) {
  $ stuff
} otherwise {
  $ other stuff
}
```
## Functions
Functions can either be declared or defined. Declared functions can be defined later, or defined in a separate translation unit. The benefit of
this is that a function can be declared, and then defined in C++. This is what allows for I/O functions to be used. <br> <br>

Declaring a function
```
func f() i32;
```
Defining a function
```
def func f() i32 {
  $ body
}
```

In the parenthesis, function parameters can be listed. The type after the parenthesis is the return type. A return type can exist an if statement,
but the language is not sophisticated enough to detect return expressions within a statement. This means that a return value must be specified in the
main body.
```
def func f(i32 x) i32 {
  if (x > 5) {
    return 5;
  } otherwise {
    return -1
  }
  return -1; $ required, if not Error: No return type will be posted
}
```
A function can also return void. A return expression is not required in this case, but if you do wish to return, you may use "return void;"
```
def func f() void {
  $ body
  $ optionally:
  return void;
}
```

## Operators
There are two types of operators, arithmetic and logical.
### Arithmetic
  - + and -
  - \* and /
### Logical
  - > and <
  - >= and <=
  - "and"
  - "or"
  - =
  - "not"
"not" works as follows
```
if (not 1 = 2) {
  $ will enter
}
```
Logical and Arithmetic operators cannot be combined in one expression, i.e.
```
...
x <- 1 > 2 + 1;  $ Error
```
# Compilation
The compiler can be compiled using the makefile, specifically rule "make rpc". <br> <br>
As I did not make a linker, an external linker must be used to generate an executable from a source text file. Furthermore, if you use any
of the I/O functions from standard_lib.txt/cpp, you need to link using a C++ compiler, compiling and linking standard_lib.cpp.
So, to compile a source text file, run the compiler executable as follow:
```
g++ <output_name>.o standard_lib.cpp <g++ args>
```
Or leave off standard_lip.cpp if you wish to have no I/O.
# Bugs
While I tried to test as much as I could, I surely missed some bugs. If you find any, please let me know by means of an issue.









