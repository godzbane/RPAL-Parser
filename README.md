# RPAL-Parser
A parser for the RPAL language, written in C++.

HOW TO RUN: Compile the project using the makefile. The input is in the form:

(switch list) (target filename)

SWITCHES:

-ast shows the abstract syntax tree (passing this switch)
-st shows the standardized tree
-l shows the input file
-debug shows step by step evaluation of the process

For some cases the program partially works. For this, the optional parameter -debug shows the internal workings of the program.
Please refer below for a detailed summary of what is functional.

WHAT WORKS:

- AST (-ast switch) works as required but might not match in DIFFTEST because of some formatting issues noted in the end of this file. Please check manually, it works for all given input.
- Tree standardizer (-st switch) for all cases works as required but might not match in DIFFTEST because of the reason above.
- The simpler programs in the tests folder give the correct output.

WHAT WORKS IN CSE MACHINE:

- All inbuilt unary and binary operators like arithmentic, string operations etc. (eg, +, -, Stem, Stern, Conc)
- Functions with 0 or 1 parameters (eg let a = 5 in Print (a) ... also let f x = x+1 in Print (f 3)).
- Nested functions.
- Conditionals.
- Everything else not mentioned below.

WHAT DOES NOT WORK IN CSE MACHINE:

- Multilayer tuples (eg let a = (1, (2,3), 4) in Print (a))
- Multiple parameter functions (eg f x y = x+y....but note that a single tuple input parameter like f(x,y,z) = x+y+z works)
- Recursive functions.

BUG:

- In the AST and onwards, binops are sometimes parsed in a different order, but this leads to no effective difference in calculation:

Eg:

+
.+
..+
...x
...y
..z
.w

Becomes:

+
.x
.+
..y
..+
...z
...w
