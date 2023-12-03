# Crisp lisp Interpreter

My attempt at implementing a Scheme lisp interpreter in C.

After watching the excellent Structure and Interpretation of Computer Programs
video series on [YouTube](https://www.youtube.com/playlist?list=PLB63C06FAF154F047) 
and reading the [book](https://web.mit.edu/6.001/6.037/sicp.pdf), 
I decided to try my hand at writing my own interpreter.

There is still plenty of work to go!

## Building

The code is writen in C99 with no external dependencies and can 
be built using CMake 3.16 or newer. Currently compiles and runs on
both Windows and Linux (Clang and GCC).

Unit tests can be executed using CTest.

## What works

 - Lexing and parsing of the Scheme grammar (partially).
 - Evaluation of basic types (number, strings, atoms).
 - A hash table based environment to associate keys with variables.
 - All strings and symbols are interned via a hash table.
 - Quoting of values.
 - Building up of lists via `cons`, and accessing them via `car` and `cdr`.
 - Evaluation of lisp expressions.
 - Mathematical functions, `+`, `-`, etc.
 - A small set of built in functions, such as `list`, `list?`, `length`.
 - Building and evaluating lambda functions.

## TODO

 - Definition of variables - lambdas can only be immediately invoked at
   the moment.
 - Garbage collection. Nothing is reclaimed at the moment.
 - Syntactic extensions.
 - Lots of other things that I don't know I'm even missing yet.

## References

The scanner, parser and hashtable are based on the ones described in
[crafting interpreters](https://craftinginterpreters.com).

Some of the test cases and the approach to building up a lisp interpreter are
based on [mal - Make a Lisp](https://github.com/kanaka/mal).

The sytax and grammar of the lisp interpreter is based on [The Scheme Programming Language](https://www.scheme.com/tspl4/)

