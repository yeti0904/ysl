<img src="/img/logo.png">

# YSL

Yeti's Simple Language

## syntax
\# for comments

function calls:

```
functionName foo bar baz
```

variables:

```
print $myvar
```

string literals from variables
```
print !myvar
```

character to integer
```
print &A
# prints 65
```

## interpreter defined variables
`__platform` is a variable to show what platform YSL is running on, it can be one of the following
- `__platform_windows`
- `__platform_apple`
- `__platform_linux`
- `__platform_unix`
- `__platform_unknown`


## builtin functions

### print
prints all arguments to stdout seperated by a space

### println
does the same as prints, but adds a newline character

### exit
no arguments, exits the program

### run
runs the code in program memory

### goto
takes one argument which is a line number/label, jumps to that line number

### goto_if
works the same as goto but only jumps if the last return value is nonzero

### gosub
works the same as goto but will jump back to where the function was called when `return` is called

### gosub_if
works the same as gosub but only calls if the last return value is nonzero

### wait
takes one integer argument, sleeps for as many milliseconds as you tell it to

### cmp
takes 2 arguments, compares both and returns nonzero if they are equal

### var
```
var varname = element1 element2 element3
```
^ assigning variables, elements must be integers

other operations include: +, -, *, /, %, ^

these operations will only be performed on the variable's first element

#### Array operations
the f operation copies an element from an array, like this:
```
var array = 1 2 3 4 5
var element f array 0
println $element
```
if the last argument is not given then the first element is used

if a variable `return` is used then it uses the return value of the last returning function

the c operation copies an entire array to a new array, like this:
```
input
var userInput c return
#return works here too
```

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

the s operation sets 1 element in the array, first number is the index and the second number is the new value
```
var array = 1 2 3 4 5
var array s 0 2
# array is now 2 2 3 4 5
```

## load
loads the contents of a file into program memory, takes 1 argument which is filename

## size
returns the size of an array

## getch
gets 1 character from stdin and returns it, no parameters

## input
gets a line from stdin and returns it, no parameters

## putch
takes in an integer argument and prints out the ASCII character of that integer

## set_size
takes in 2 parameters: array and new size

it changes the size of the array

## return
sets the current line the interpreter is running at to the last place a user-defined function was called

when a parameter is passed:

if it is an integer, that is put into return values stack as a 1 element array

if it is not, it will be interpreted as a variable name and the contents of the variable are returned

## swap
takes in 2 arguments: variable names and swaps the contents with each other

## gt
takes in 2 arguments and returns true if the first argument is greater than the second argument

## lt
takes in 2 arguments and returns false if the first argument is smaller than the second argument

## pow
takes in 2 integer arguments and returns the first integer multiplied to the power of the second integer

## string_array
this is a function that can be used to interact with string arrays
### n operator
this creates a new string array and returns it, for example:
```
string_array n "item1" "item2" "item3"
var myArray c return
```

### g operator
this gets 1 string from an array, it takes in the variable name and the index
```
string_array n "item1" "item2" "item3"
var myArray c return
string_array g myArray 0
var element c return
println !element
# prints item1
```

### l operator
this takes in 1 variable name and returns how many strings are in the string array

### a operator
takes in a variable name and 1 string and appends the string to the array, returns nothing

### s operator
takes in a variable name, index and a string and overwrites the string at the index of the array

example:
```
string_array n "item1" "item2" "item3"
var myArray c return
string_array s myArray 0 "foo"
string_array g myArray 0
var element c return
println !return
# prints foo
```

### r operator
takes in a variable name, index and it removes the element at index from the array

## split
takes in 2 arguments, a string and 1 character to act as the splitter

returns a string array

## not
performs a NOT operation on a value, if no parameters are given it uses the last return value and it can also use a value from 1 parameter

## and
performs an AND operation on 2 values, if no parameters are given it uses the last 2 return values,
 if 2 parameters are given it will use those 2 parameters

## or
performs an OR operation on 2 values, if no parameters are given it uses the last 2 return values,
 if 2 parameters are given it will use those 2 parameters

## is_num
takes in 1 string argument, and returns true if it is numerical, false if not

## atoi
takes in 1 numeric string and returns that string as 1 integer

## itoa
takes in 1 integer, and returns it as a string

## load_end
loads a file relative to the current working directory at the end of program memory

can be used for including files, like in C programs

## error
takes no arguments

prints out where it was called + a backtrace and exits

## sqrt
takes in 1 integer argument and computes the square root

## local
takes in any amount of arguments, which are variable names

when the function returns then these variables are reset to what they were before the function was called

## matrix
syntax:
```
matrix varname (operation) (parameters)
```

__Operations:__
### c
2 extra parameters: width and height

creates a matrix in the variable

### g
2 extra parameters: x and y index

returns the integer in that position of the matrix

### s
3 extra parameters: x and y index and value

sets the value at that position to the value
