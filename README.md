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
