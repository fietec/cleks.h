# Cleks.h Documentation
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/fietec/cleks.h/blob/master/LICENSE)

**Cleks** is a highly configurable header-only c lexer library capable of tokenizing strings based on user-defined rules for any language, data format, or custom syntax.

## Table of Contents
- [Installation](#installation)
- [How to use](#how-to-use)
    - [Default Tokens](#default-tokens)
    - [Configuration](#configuration)
        - [Custom Tokens](#custom-tokens)
        - [Comments](#comments)
        - [Strings](#strings)
        - [Flags](#token-mask)
    - [Templates](#templates)
- [Examples](#examples)
    - [Custom Example](#custom-example)
    - [Template Example](#template-example)
- [Function API](#api)
    
## Installation

1. Download `cleks.h` or clone the repository for all templates and examples.
2. Include `cleks.h` in your project.

```c
#include "cleks.h"
```

## How to use

To tokenize a string, use the `Cleks_lex` function:
```c
CleksTokens* Cleks_lex(char *buffer, size_t buffer_size, CleksConfig config);
```
This function takes a buffer and its size as arguments, together with a [`CleksConfig`](#configuration) struct.
Based on the provided rules the lexer will allocate a `CleksTokens` structure, which is a dynamic array of `CleksToken`s. 
```c
typedef struct{
    CleksToken **items;  // the array of CleksTokens
    size_t size;         // the amount of tokens
    size_t capacity;     // the token capacity
    CleksConfig config;  // the CleksConfig struct provided by the user
} CleksTokens;
```
Each token has a `type` and `value` field which contains the string content for [default tokens](#default-tokens).

```c
typedef int CleksTokenType;
typedef struct{
    CleksTokenType type;
    char *value;
} CleksToken;
```
### Default Tokens

**Cleks** defines four tokens by default with respective `CleksTokenType`s:

- TokenType = -4: `TOKEN_STRING` -> a string was found (content in `CleksToken::value`) 
- TokenType = -3: `TOKEN_WORD` -> an unknown word was found (content stored in `CleksToken::value`)
- TokenType = -2: `TOKEN_INT` -> an integer word was found (content as string in `CleksToken::value`)
- TokenType = -1: `TOKEN_FLOAT` -> an float word was found (content as string in `CleksToken::value`)

Custom tokens therefore start with TokenType=0, which means you can index custom tokens directly by the type of the token, as long as it is positive.

For this you can also use the macro provided by **Cleks**:
```c
for (size_t i=0; i<tokens->size; ++i){
    CleksToken *token = tokens->items[i];
    if (!CLEKS_IS_CUSTOM_TOKEN(token)){
        printf("Token Found: %s\n", <CustomTokenConfig>[token].print_string);
    }
}
```

### Configuration

**Cleks** allows full customization of the lexer via the `CleksConfig` struct.
```c
typedef struct{
    CleksTokenConfig *default_tokens;      
    size_t default_token_count;            
    CleksTokenConfig *custom_tokens;       
    size_t custom_token_count;             
    const char* const  whitespaces;        
    CleksString *strings;                  
    size_t string_count;                   
    CleksComment *comments;                
    size_t comment_count;                  
    uint8_t token_mask;                 
} CleksConfig; 
```
#### Field Documentation:
- `default_tokens` - the default tokens of the lexer (always provide `CleksDefaultTokenConfig`)
- `default_token_count` - the amount of default tokens (use `CLEKS_TOKEN_COUNT`)
- `custom_tokens` - an array of custom [`CleksTokenConfig`s](#custom-tokens)
- `custom_token_count` - the amount of custom tokens (can be obtained via the `CLEKS_ARR_LEN` macro)
- `whitespaces` - a string of characters for the lexer to skip
- `strings` - an array of [`CleksString`s](#strings) to define string beginning and end delimeters
- `string_count` - the amount of comments
- `comments` - an array of [`CleksComment`s](#comments) to define comment beginning and end delimeters
- `comment_count` - the amount of comments
- `token_mask` - a single-byte mask containing further lexing rules (`CLEKS_DEFAULT` as default)

#### Custom Tokens
To define custom tokens, provide an array of `CleksTokenConfig`s.
```c
typedef struct{
    char* print_string; // the string to print for Cleks_print_tokens
    char* word;         // the string defining a word, "" for non-words
    char  symbol;       // the character defining a symbol, '\0' non-symbols
} CleksTokenConfig;
```
As can be seen above, there are two types of custom tokens, *WORDS* and *SYMBOLS*.
- *SYMBOL*: a single character token (prioritized)
- *WORD*: a multi-character string token

For example:
```c
CleksTokenConfig TestTokenConfig[] = {
    {"If", "if", '\0'},
    {"Left-Bracket", "", '('},
    {"Right-Bracket", "", ')'}
};
```

#### Comments
To define custom comments, provide an array of `CleksComment`s.
```c
typedef struct{
    const char* const start_del;  // the string defining a comment's beginning
    const char* const end_del;    // the string defining a comment's end
} CleksComment;
```
For example:
```c
CleksComment TestComments[] = {
    {"//", "\n"},
    {"#", "\n"},
    {"/*", "*/"}
};
```

#### Strings
To define custom strings, provide an array of `CleksString`s.
```c
typedef struct{
    const char const start_del; // the character defining a string's beginning
    const char const end_del;   // the character defining a string's end
} CleksString;
```
For example:
```c
CleksString TestStrings[] = {
    {'"', '"'},
    {'[', ']'}
};
```

#### Token Mask
This mask is used to further customize the behaviour of the lexer.

Currently these flags are available:
- `CLEKS_DEFAULT` - the default behaviour
- `CLEKS_NO_INTEGERS` - don't recognize integers, instead use `TOKEN_WORD`
- `CLEKS_NO_FLOATS` - dont recognize floats, insted use `TOKEN_WORD`

You can combine these flags by using Bitwise-OR:

`.token_mask = CLEKS_NO_INTEGERS | CLEKS_NO_FLOATS`

### Templates
**Cleks** provides several templates for well-known formats, such as *JSON* and *Brainfuck*.
Templates can be found in the `templates` directory in the repository.
When [using a template](#template-example), there is no need of creating a CleksConfig by yourself.

## Examples
Here are a few examples of how to use **Cleks**: 
### Custom Example
```c
#include "cleks.h"

// to make life a lot easier
enum TestTokens{
    TEST_LT,
    TEST_GT,
    TEST_EQ,
    TEST_IF,
    TEST_THEN,
    TEST_LB,
    TEST_RB,
    TEST_PRINT
};

// define our custom tokens
CleksTokenConfig TestTokenConfig[] = {
    [TEST_LT] = {.print_string = "Less-Than", .word="", .symbol='<'},
    [TEST_GT] = {"Greater-Than", "", '>'},
    [TEST_EQ] = {"Equals", "", '='},
    [TEST_IF] = {"If", "if", '\0'},
    [TEST_THEN] = {"Then", "then", '\0'},
    [TEST_LB] = {"Left-Bracket", "", '('},
    [TEST_RB] = {"Right-Bracket", "", ')'},
    [TEST_PRINT] = {"Function-Print", "print", '\0'}
};

// define our custom comments
CleksComment TestComments[] = {
    {"//", "\n"},  // single-line comment from "//" to the end of the line
    {"#", "\n"},   // single-line comment form "#" to the end of the line
    {"/*", "*/"}   // multi-line comment from "/*" to "*/"
};

// define our custom strings
CleksString TestStrings[] = {
    {'"', '"'},    // string from '"' to '"'
    {'[', ']'}     // string from '[' to ']'
};

int main(void)
{
    CleksConfig TestConfig = {
        .default_tokens = CleksDefaultTokenConfig,
        .default_token_count = CLEKS_TOKEN_COUNT,
        .custom_tokens = TestTokenConfig,
        .custom_token_count = CLEKS_ARR_LEN(TestTokenConfig),
        .whitespaces = " \n",  // we skip ' ' and '\n'
        .strings = TestStrings,
        .string_count = CLEKS_ARR_LEN(TestStrings),
        .comments = TestComments,
        .comment_count = CLEKS_ARR_LEN(TestComments),
        .token_mask = CLEKS_DEFAULT
    };

    char buffer[] = "if(x < 2)/*this is a comment */ // this is also a comment \nthen print(\"x+1\"[is greater than x by 1])"; // our test buffer
    CleksTokens *tokens = Cleks_lex(buffer, strlen(buffer), TestConfig); // lex our buffer
    Cleks_print_tokens(tokens);  // print the found tokens using their `print_string` value
    Cleks_free_tokens(tokens);  // free the allocated memory
    return 0;
}
```
This will generate the following output:
```
Token count: 12
  Token   3: If
  Token   5: Left-Bracket
  Token  -3: Word "x"
  Token   0: Less-Than
  Token  -2: Integer "2"
  Token   6: Right-Bracket
  Token   4: Then
  Token   7: Function-Print
  Token   5: Left-Bracket
  Token  -4: String "x+1"
  Token  -4: String "is greater than x by 1"
  Token   6: Right-Bracket
```

### Template Example
In this example we use the template for *JSON*.
```c
#include "cleks.h"
#include "templates/cleks_json_template.h" // include the template

int main(void)
{
    char buffer[] = "{\"nums\": [1, 2, 3], \"truth\": [true, false, null]}"; // our test buffer
    CleksTokens* tokens = Cleks_lex(buffer, strlen(buffer), JsonConfig); // use the JsonConfig provided by the template
    Cleks_print_tokens(tokens);
    Cleks_free_tokens(tokens);
    return 0;
}
```
This will generate the following output:
```
Token count: 21
  Token   0: JsonMapOpen: '{'
  Token  -4: String "nums"
  Token   4: JsonMapSep: ':'
  Token   2: JsonArrayOpen: '['
  Token  -2: Integer "1"
  Token   5: JsonIterSet: ','
  Token  -2: Integer "2"
  Token   5: JsonIterSet: ','
  Token  -2: Integer "3"
  Token   3: JsonArrayClose: ']'
  Token   5: JsonIterSet: ','
  Token  -4: String "truth"
  Token   4: JsonMapSep: ':'
  Token   2: JsonArrayOpen: '['
  Token   6: JsonTrue: true
  Token   5: JsonIterSet: ','
  Token   7: JsonFalse: false
  Token   5: JsonIterSet: ','
  Token   8: JsonNull: null
  Token   3: JsonArrayClose: ']'
  Token   1: JsonMapClose: '}'
```
## API
```c
// Tokenizes the input buffer according to the given config.
CleksTokens* Cleks_lex(char *buffer, size_t buffer_size, CleksConfig config);

// Frees all memory associated with the tokens structure.
void Cleks_free_tokens(CleksTokens *tokens);

// Prints all tokens in a human-readable format.
void Cleks_print_tokens(CleksTokens *tokens);

// Appends a token of a given type to the tokens list.
void Cleks_append_token(CleksTokens *tokens, CleksTokenType token_type, char *token_value);

// check if a token is a custom token
CLEKS_IS_CUSTOM_TOKEN(token);
```