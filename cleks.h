#ifndef _CLEKS_H
#define _CLEKS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// customize (default: json)

#define CLEKS_PRINT_ID false

typedef enum{
    TOKEN_IF,
    TOKEN_WHILE,
    TOKEN_ELSE,
    TOKEN_STRING,
    TOKEN_MAP_OPEN,
    TOKEN_MAP_CLOSE,
    TOKEN_ARRAY_OPEN,
    TOKEN_ARRAY_CLOSE,
    TOKEN_MAP_SEP
} CleksToken;

const char* const TokenStrings[] = {
    [TOKEN_IF] = "Word: if",
    [TOKEN_WHILE] = "Word: while",
    [TOKEN_ELSE] = "Word: else",
    [TOKEN_STRING] = "String",
    [TOKEN_MAP_OPEN] = "Symbol: {",
    [TOKEN_MAP_CLOSE] = "Symbol: }",
    [TOKEN_ARRAY_OPEN] = "Symbol: [",
    [TOKEN_ARRAY_CLOSE] = "Symbol: ]",
    [TOKEN_MAP_SEP] = "Symbol: :"    
};

const char const Symbols[] = {
    [TOKEN_MAP_OPEN] = '{',
    [TOKEN_MAP_CLOSE] = '}',
    [TOKEN_ARRAY_OPEN] = '[',
    [TOKEN_ARRAY_CLOSE] = ']',
    [TOKEN_MAP_SEP] = ':',
    [TOKEN_STRING] = '\0',
    [TOKEN_IF] = '\0',
    [TOKEN_WHILE] = '\0',
    [TOKEN_ELSE] = '\0',
};

const char* const Words[] = {
    [TOKEN_IF] = "if",
    [TOKEN_WHILE] = "while",
    [TOKEN_ELSE] = "else",
    [TOKEN_STRING] = "",
    [TOKEN_MAP_OPEN] = "",
    [TOKEN_MAP_CLOSE] = "",
    [TOKEN_ARRAY_OPEN] = "",
    [TOKEN_ARRAY_CLOSE] = "",
    [TOKEN_MAP_SEP] = ""
};

const char const StringDelimeters[] = {'"'};

const char const Whitespaces[] = {' ', '\n'};

// internals

typedef enum{
    CLEKS_P_NONE,
    CLEKS_P_STRING,
    CLEKS_P_WORD
} CleksMode;

typedef struct{
    CleksToken *items;
    size_t size;
    size_t capacity;
} CleksTokens;

typedef struct{
    char *buffer;
    size_t buffer_size;
    CleksMode mode;
    size_t index;
} Clekser;

#define CLEKS_NOT_FOUND -1
#define CLEKSTOKENS_RSF 2

#define CLEKS_ARR_LEN(arr) (sizeof((arr))/sizeof((arr)[0]))
#define CLEKS_ANSI_END "\e[0m"
#define CLEKS_ANSI_RGB(r, g, b) ("\e[38;2;" #r ";" #g ";" #b "m")

#define cleks_info(msg, ...) do{ if (CLEKS_PRINT_ID){(printf("%s%s:%d: " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 255, 255), __FILE__, __LINE__, ## __VA_ARGS__));}} while (0);
#define cleks_debug(msg, ...) do{ if (CLEKS_PRINT_ID){(printf("%s%s:%d: [DEBUG] " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 124, 0), __FILE__, __LINE__, ## __VA_ARGS__));}} while (0);
#define cleks_eprintln(msg, ...) (fprintf(stderr, "%s%s:%d: [ERROR] " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 0, 0), __FILE__, __LINE__, ## __VA_ARGS__))

#define CLEKS_ASSERT(statement, msg, ...) do{if (!(statement)) {cleks_eprintln(msg, ##__VA_ARGS__); exit(1);}} while (0);

#define cleks_c2t(c, arr) (Cleks_char2Token(c, arr, CLEKS_ARR_LEN(arr)))

void Cleks_print_token(CleksToken token);

CleksTokens* Cleks_create_tokens(size_t capacity)
{
    CleksTokens *tokens = (CleksTokens*) malloc(sizeof(*tokens));
    CLEKS_ASSERT(tokens != NULL, "Failed to allocate CleksTokens!");
    tokens->items = (CleksToken*) calloc(capacity, sizeof(CleksToken));
    if (tokens->items == NULL){
        free(tokens);
        cleks_eprintln("Failed to allocate CleksTokens items!");
        exit(1);
    }
    tokens->capacity = capacity;
    tokens->size = 0;
    return tokens;
}

void Cleks_free_tokens(CleksTokens *tokens)
{
    if (tokens != NULL){
        if (tokens->items != NULL){
            free(tokens->items);
        }
        free(tokens);
    }
}

void Cleks_append_token(CleksTokens *tokens, CleksToken token)
{
    if (!tokens){
        cleks_eprintln("Invalid CleksTokens pointer!");
        exit(1);
    } 
    if (tokens->size >= tokens->capacity){
        size_t new_capacity = tokens->capacity * CLEKSTOKENS_RSF*sizeof(CleksToken);
        CleksToken *new_items = (CleksToken*) realloc(tokens->items, new_capacity);
        CLEKS_ASSERT(new_items != NULL, "Failed to resize CleksTokens!");
        tokens->items = new_items;
        tokens->capacity = new_capacity;
    }
    tokens->items[tokens->size++] = token;
}

int Cleks_char2Token(char c, char const* arr, int size)
{
    for (int i=0; i<size; ++i){
        if (arr[i] == c) return i;
    }
    return CLEKS_NOT_FOUND;
}

int Cleks_lex_word(Clekser *clekser, CleksTokens *tokens)
{
    CLEKS_ASSERT(clekser != NULL && tokens != NULL, "Invalid Arguments: clekser=%p, tokens=%p", clekser, tokens);
    size_t word_start = clekser->index;
    while (clekser->index < clekser->buffer_size){
        char c = clekser->buffer[clekser->index];
        if (cleks_c2t(c, StringDelimeters) != CLEKS_NOT_FOUND || cleks_c2t(c, Whitespaces) != CLEKS_NOT_FOUND || cleks_c2t(c, Symbols) != CLEKS_NOT_FOUND){
            cleks_info("        %c is not part of the word: %d %d %d\n", c, cleks_c2t(c, StringDelimeters), cleks_c2t(c, Whitespaces), cleks_c2t(c, Symbols));
            break;
        }
        clekser->index += 1;
    }
    cleks_info("        found word of length %d: \"%.*s\"\n", clekser->index - word_start, clekser->index - word_start, clekser->buffer + word_start);
    clekser->mode = CLEKS_P_NONE;
    // clekser->index -= 1;
    for (size_t i=0; i<CLEKS_ARR_LEN(Words); ++i){
        if (strncmp(clekser->buffer + word_start, Words[i], clekser->index - word_start) == 0){
            // words match
            cleks_debug("Found word: '%s'", Words[i]);
            Cleks_append_token(tokens, (CleksToken) i);
            return 0;
        }
    }
    cleks_eprintln("Found unknown word: \"%.*s\"", clekser->index - word_start, clekser->buffer + word_start);
    return 1;
}

CleksTokens* Cleks_lex(char *buffer, size_t buffer_size)
{
    CLEKS_ASSERT(buffer != NULL && buffer_size != 0, "Invalid arguments!");
    Clekser clekser = {.buffer=buffer, .buffer_size=buffer_size, .mode = CLEKS_P_NONE, .index=0};
    CleksTokens *tokens = Cleks_create_tokens(16);
    char c;
    while (clekser.index <= buffer_size && (c = buffer[clekser.index]) != '\0'){
        cleks_debug("Lexing: index=%u, character='%c'", clekser.index, c);
        switch (clekser.mode){
            case CLEKS_P_NONE:{
                int int_token;
                if ((int_token = cleks_c2t(c, StringDelimeters)) != CLEKS_NOT_FOUND){
                    cleks_info("    [INFO] is string del\n");
                    clekser.mode = CLEKS_P_STRING;
                    clekser.index += 1;
                    continue;
                }
                else if ((int_token = cleks_c2t(c, Symbols)) != CLEKS_NOT_FOUND){
                    cleks_info("    [INFO] is symbol\n");
                    Cleks_append_token(tokens, (CleksToken) int_token);
                    clekser.index += 1;
                    continue;
                }
                else if ((int_token = cleks_c2t(c, Whitespaces)) != CLEKS_NOT_FOUND){
                    cleks_info("    [INFO] is whitespace\n");
                    clekser.index += 1;
                    continue;
                }
                else{
                    cleks_info("    [INFO] is beginning of word\n");
                    clekser.mode = CLEKS_P_WORD;
                    continue;
                }
            };
            case CLEKS_P_STRING:{
                clekser.index += 1;
            } break;
            case CLEKS_P_WORD:{
                cleks_info("    lexing word\n");
                if (Cleks_lex_word(&clekser, tokens) == 1){
                    Cleks_free_tokens(tokens);
                    return NULL;
                }
                cleks_info("    finished lexing word at index %d\n", clekser.index);
                continue;
            }break;
            default:{
                cleks_eprintln("[INTERNAL] Invalid parsing mode: %d", clekser.mode);
                exit(1);
            }
        }
        clekser.index += 1;
    }
    return tokens;
}

void Cleks_print_token(CleksToken token)
{
    CLEKS_ASSERT((int) token < CLEKS_ARR_LEN(TokenStrings), "This token does not exist! %d/%d", token, CLEKS_ARR_LEN(TokenStrings));
    printf("Token: %s\n", TokenStrings[token]);
}

void Cleks_print_tokens(CleksTokens *tokens)
{
    CLEKS_ASSERT(tokens != NULL, "Invalid parameter!");
    printf("Token count: %u\n", tokens->size);
    for (size_t i=0; i<tokens->size; ++i){
        Cleks_print_token(tokens->items[i]);
        printf("  ");
    }
}

#endif // _CLEKS_H