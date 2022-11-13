# YSL

Yeti's Simple Language

## Example of usage
```
#include "ysl.hh"

int main() {
	YSL::Environment env;

	env.Interpret("print hello world");
}
```

## syntax
\# for commands

function calls:

```
functionName foo bar baz
```

variables:

```
print $myvar
```

## builtins

### print
prints all arguments to stdout seperated by a space

### println
does the same as prints, but adds a newline character

### exit
no arguments, exits the program

### run
runs the code in program memory

### goto
takes one argument which is a line number, jumps to that line number

### goto_if
works the same as goto but only jumps if the last return value is nonzero

### wait
takes one integer argument, sleeps for as many milliseconds as you tell it to

### cmp
takes 2 arguments, compares both and returns nonzero if they are equal

### var
```
var varname = element1 element2 element3
```
^ assigning variables, elements must be integers

other operations include: +, -, *, /, %

these operations will only be performed on the variable's first element

#### Array operations
the f operation copies an element from an array, like this:
```
var array = 1 2 3 4 5
var element f array 0
println $element
```
if a variable `return` is used then it uses the return value of the last returning function

the a operation appends 1 element to an array, like this:
```
var array = 1 2 3 4 5
var array a 6
```

the r operation removes elements from an array, first number is start index and the second is the length
```
var array = 1 2 3 4 5
var array r 0 1
# array is now 2 3 4 5
```

## load
loads the contents of a file into program memory, takes 1 argument which is filename

## size
returns the size of an array
