# cleks.h

A simplistic and customizable lexer in C. 

## How to use

Simply include the header file like this:
```c
#include "cleks.h"
```

For lexing, simply use `Cleks_lex` like this:
```c
char buffer[] = "[1, 2, 3]"; // the buffer to parse
Cleks_lex(buffer, strlen(buffer));
```

This will return a dynamically allocated `CleksTokens` structure with a `size` and `items` field.
To parse the tokens, iterate over `items`.

Tokens are stored in the `CleksToken` structure. Each has a `type` field indicating the token type (defined in `CleksTokenType` in `cleks.h`) and the `value` field which consits of a string literal containing the value of the tokens which are dynamic, such as TOKEN_WORD, TOKEN_INT, TOKEN_FLOAT and TOKEN_STRING.

You can print a representation of the found tokens via:
```c
Cleks_print_tokens(<CleksTokens*>);
```

To free the allocated memory use:
```c
Cleks_free_tokens(<CleksTokens*>);
```

## Known Issues

- No support for escape codes in strings
- Features such as parsing integers and floats can't be disabled