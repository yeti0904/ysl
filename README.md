<img src="/img/logo.png">

# YSL

Yeti's Simple Language

## build options
### type=lib
compiles YSL as a library

### graphics=off
disables the gfx library so raylib is not required for compilation

### curl=off
disables the curl extension so libcurl is not required for compilation

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


## standard library documentation
[Documentation](http://ysl-lang.mesyeti.uk)
