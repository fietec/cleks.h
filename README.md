# Cleks Lexer Library Documentation

**Cleks** is a configurable lexer library designed to tokenize strings based on user-defined rules for languages, data formats, or custom syntaxes.

## Table of Contents
- [Installation](#installation)
- [Configuration](#configuration)
  - [Token Configuration](#token-configuration)
  - [Predefined Tokens](#predefined-tokens)
  - [Custom Tokens](#custom-tokens)
  - [Flags](#flags)
  - [Comment Delimiters](#comment-delimiters)
- [Using Templates](#using-templates)
- [Usage](#usage)
  - [Basic Example](#basic-example)
  - [Custom Tokenization Example](#custom-tokenization-example)
- [API](#api)
  - [CleksConfig Structure](#cleksconfig-structure)
  - [Functions](#functions)

## Installation

1. Clone or download the **Cleks** repository.
2. Include `cleks.h` in your C project:

```c
#include "cleks.h"
```

## Configuration

Cleks allows full customization of the lexer by setting up a configuration (CleksConfig) struct. This struct defines the following:

    Predefined tokens like TOKEN_STRING, TOKEN_INT, etc.
    Custom tokens for your own specific symbols or words.
    Flags for enabling or disabling certain types of token parsing.
    Comment delimiters for ignoring parts of the input as comments.

### Token Configuration

Tokens are defined in two ways:

    Predefined tokens: Pre-included basic tokens like strings, words, integers, and floats.
    Custom tokens: User-defined tokens for specific words or symbols.

#### Predefined Tokens

The following tokens are predefined by default:

    `TOKEN_STRING`: Matches quoted string literals.
    `TOKEN_WORD`: Matches general words that are not strings or numbers.
    `TOKEN_INT`: Matches integer numbers.
    `TOKEN_FLOAT`: Matches floating-point numbers.

You can disable parsing of numbers by setting specific flags in the config (e.g., `CLEKS_NO_INTEGERS`).
Custom Tokens

You can define your own custom tokens using `CleksTokenConfig`. Custom tokens can match specific symbols or reserved words.

Example of adding custom tokens:

```c

enum JsonTokens {
    JSON_MAP_OPEN,
    JSON_MAP_CLOSE,
    JSON_ARRAY_OPEN,
    JSON_ARRAY_CLOSE
};

static CleksTokenConfig JsonTokens[] = {
    [JSON_MAP_OPEN] = {"JsonMapOpen", "", '{'},
    [JSON_MAP_CLOSE] = {"JsonMapClose", "", '}'},
    [JSON_ARRAY_OPEN] = {"JsonArrayOpen", "", '['},
    [JSON_ARRAY_CLOSE] = {"JsonArrayClose", "", ']'}
};
```

### Flags

Flags allow you to control which types of tokens are parsed. You can combine them using bitwise OR (|).

    CLEKS_DEFAULT: Default behavior.
    CLEKS_NO_INTEGERS: Disables parsing of integers.
    CLEKS_NO_FLOATS: Disables parsing of floats.

### Comment Delimiters

Define the comment delimiters to ignore comments in your input. Delimiters can currently only be single-line.

```c

const char* const JsonCommentDelimeters[] = {"//", "#"};
```

### Using Templates

You can simplify configuration by using templates. Templates are predefined files that contain all necessary config data for specific purposes, such as JSON parsing.

To use a template, include the corresponding file:

```c
#include "./templates/cleks_json_template.h"
```

This file will automatically set up the necessary token configuration, comment delimiters, and flags.

Here’s a basic example to lex a JSON string using the default config:

```c

#include "cleks.h"
#include "./templates/cleks_json_template.h"  // Using a template for JSON config

int main(void) {
    // Define the string to tokenize
    char *json_string = "{\"name\": \"John\", \"age\": 25}";
    
    // Load the default JSON config from template
    CleksConfig json_config = CleksJsonConfig;
    
    // Perform tokenization
    CleksTokens* tokens = Cleks_lex(json_string, strlen(json_string), json_config);
    
    // Print the tokens
    Cleks_print_tokens(tokens);
    
    // Free allocated tokens
    Cleks_free_tokens(tokens);
    
    return 0;
}
```

### Custom Tokenization Example

Here’s an example using custom tokens for a new syntax:

```c

#include "cleks.h"

// Custom tokens
enum MyTokens {
    MY_TOKEN_IF,
    MY_TOKEN_THEN,
    MY_TOKEN_PRINT,
    MY_TOKEN_LEFT_BRACKET,
    MY_TOKEN_RIGHT_BRACKET
};

static CleksTokenConfig MyTokens[] = {
    [MY_TOKEN_IF] = {"IfKeyword", "if", '\0'},
    [MY_TOKEN_THEN] = {"ThenKeyword", "then", '\0'},
    [MY_TOKEN_PRINT] = {"PrintKeyword", "print", '\0'},
    [MY_TOKEN_LEFT_BRACKET] = {"LBracketSymbol", "", '('},
    [MY_TOKEN_RIGHT_BRACKET] = {"RBracketSymbol", "", ')'}
};

int main(void) {
    char *code = "if x > 10 then print(x)";

    // Custom config
    CleksConfig custom_config = {
        .default_tokens = CleksDefaultTokenConfig,
        .default_token_count = DEFAULT_TOKEN_COUNT,
        .custom_tokens = MyTokens,
        .custom_token_count = CLEKS_ARR_LEN(MyTokens),
        .whitespaces = " \n",
        .string_delimters = "\"",
        .comment_delimeters = NULL,
        .comment_delimeter_count = 0,
        .token_mask = CLEKS_DEFAULT
    };
    
    CleksTokens* tokens = Cleks_lex(code, strlen(code), custom_config);
    Cleks_print_tokens(tokens);
    Cleks_free_tokens(tokens);
    
    return 0;
}
```

## API
### CleksConfig Structure

```c

typedef struct {
    CleksTokenConfig* default_tokens;       // Default token set (strings, words, numbers)
    size_t default_token_count;             // Number of default tokens
    CleksTokenConfig* custom_tokens;        // User-defined custom tokens
    size_t custom_token_count;              // Number of custom tokens
    const char* const whitespaces;          // Whitespace characters
    const char* const string_delimters;     // String delimiters (e.g., '"')
    const char* const* comment_delimeters;  // Comment delimiters (e.g., {"//", "/*", NULL})
    size_t comment_delimeter_count;         // Number of comment delimiters
    uint8_t token_mask;                     // Flags for parsing options
} CleksConfig;
```

### Functions
    ```c
    CleksTokens* Cleks_lex(char *buffer, size_t buffer_size, CleksConfig config)
    Tokenizes the input buffer according to the given config.

    void Cleks_free_tokens(CleksTokens *tokens)
    Frees all memory associated with the tokens structure.

    void Cleks_print_tokens(CleksTokens *tokens)
    Prints all tokens in a human-readable format.

    void Cleks_append_token(CleksTokens *tokens, CleksTokenType token_type, char *token_value)
    Appends a token of a given type to the tokens list.
    ```

## License

This project is licensed under the MIT License - see the `LICENSE` file for details.