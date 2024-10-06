#ifndef _CLEKS_H
#define _CLEKS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef enum{
    TOKEN_STRING,
    TOKEN_MAP_OPEN,
    TOKEN_MAP_CLOSE,
    TOKEN_ARRAY_OPEN,
    TOKEN_ARRAY_CLOSE,
    TOKEN_MAP_SEP,
    TOKEN_ITER_SEP,
    TOKEN_WORD,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_TRUE,
    TOKEN_FALSE
} CleksTokenType;

const char* const TokenStrings[] = {
    [TOKEN_TRUE] = "Word: true",
    [TOKEN_FALSE] = "Word: false",
    [TOKEN_INT] = "Word: integer",
    [TOKEN_FLOAT] = "Word: float",
    [TOKEN_WORD] = "Word: unknown",
    [TOKEN_STRING] = "String",
    [TOKEN_MAP_OPEN] = "Symbol: {",
    [TOKEN_MAP_CLOSE] = "Symbol: }",
    [TOKEN_ARRAY_OPEN] = "Symbol: [",
    [TOKEN_ARRAY_CLOSE] = "Symbol: ]",
    [TOKEN_MAP_SEP] = "Symbol: :",
    [TOKEN_ITER_SEP] = "Symbol: ,"    
};

const char const Symbols[] = {
    [TOKEN_MAP_OPEN] = '{',
    [TOKEN_MAP_CLOSE] = '}',
    [TOKEN_ARRAY_OPEN] = '[',
    [TOKEN_ARRAY_CLOSE] = ']',
    [TOKEN_ITER_SEP] = ',',
    [TOKEN_MAP_SEP] = ':',
    [TOKEN_STRING] = '\0',
    [TOKEN_TRUE] = '\0',
    [TOKEN_FALSE] = '\0',
    [TOKEN_WORD] = '\0',
    [TOKEN_INT] = '\0',
    [TOKEN_FLOAT] = '\0',
};

const char* const Words[] = {
    [TOKEN_TRUE] = "true",
    [TOKEN_FALSE] = "false",
    [TOKEN_INT] = "",
    [TOKEN_FLOAT] = "",
    [TOKEN_WORD] = "",
    [TOKEN_STRING] = "",
    [TOKEN_MAP_OPEN] = "",
    [TOKEN_MAP_CLOSE] = "",
    [TOKEN_ARRAY_OPEN] = "",
    [TOKEN_ARRAY_CLOSE] = "",
    [TOKEN_MAP_SEP] = "",
    [TOKEN_ITER_SEP] = "",
};

const char const StringDelimeters[] = {'"'};

const char const Whitespaces[] = {' ', '\n'};

// internals

typedef struct{
    CleksTokenType type;
    char *value;
} CleksToken;

typedef enum{
    CLEKS_P_NONE,
    CLEKS_P_STRING,
    CLEKS_P_WORD
} CleksMode;

typedef struct{
    CleksToken **items;
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

#define CLEKS_PRINT_ID true

#define CLEKS_ARR_LEN(arr) (sizeof((arr))/sizeof((arr)[0]))
#define CLEKS_ANSI_END "\e[0m"
#define CLEKS_ANSI_RGB(r, g, b) ("\e[38;2;" #r ";" #g ";" #b "m")

#define cleks_info(msg, ...) do{ if (CLEKS_PRINT_ID){(printf("%s%s:%d: " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 255, 255), __FILE__, __LINE__, ## __VA_ARGS__));}} while (0);
#define cleks_debug(msg, ...) do{ if (CLEKS_PRINT_ID){(printf("%s%s:%d: [DEBUG] " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 124, 0), __FILE__, __LINE__, ## __VA_ARGS__));}} while (0);
#define cleks_eprintln(msg, ...) (fprintf(stderr, "%s%s:%d: [ERROR] " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 0, 0), __FILE__, __LINE__, ## __VA_ARGS__))

#define CLEKS_ASSERT(statement, msg, ...) do{if (!(statement)) {cleks_eprintln(msg, ##__VA_ARGS__); exit(1);}} while (0);

#define cleks_c2t(c, arr) (Cleks_char2Token(c, arr, CLEKS_ARR_LEN(arr)))

void Cleks_print_token(CleksToken *token);
static char* strndup(char* s, size_t n);
static bool str_is_int(char *s);
static bool str_is_float(char *s);

CleksTokens* Cleks_create_tokens(size_t capacity)
{
    CleksTokens *tokens = (CleksTokens*) malloc(sizeof(*tokens));
    CLEKS_ASSERT(tokens != NULL, "Failed to allocate CleksTokens!");
    tokens->items = (CleksToken**) calloc(capacity, sizeof(CleksToken*));
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
            for (size_t i=0; i<tokens->size; ++i){
                CleksToken *token = tokens->items[i];
                if (token != NULL){
                    free(token);
                }
            }
            free(tokens->items);
        }
        free(tokens);
    }
}

void Cleks_append_token(CleksTokens *tokens, CleksTokenType token_type, char *token_value)
{
    if (!tokens){
        cleks_eprintln("Invalid CleksTokens pointer!");
        exit(1);
    } 
    if (tokens->size >= tokens->capacity){
        size_t new_capacity = tokens->capacity * CLEKSTOKENS_RSF;
        CleksToken **new_items = (CleksToken**) realloc(tokens->items, new_capacity*sizeof(CleksToken*));
        CLEKS_ASSERT(new_items != NULL, "Failed to resize CleksTokens!");
        tokens->items = new_items;
        tokens->capacity = new_capacity;
    }
    CleksToken *token = calloc(1, sizeof(CleksToken));
    CLEKS_ASSERT(token != NULL, "Failed to allocate new CleksToken!");
    token->type = token_type;
    token->value = token_value; 
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
            Cleks_append_token(tokens, (CleksTokenType) i, NULL);
            return 0;
        }
    }
    char *word_value = strndup(clekser->buffer + word_start, clekser->index - word_start);
    if (word_value == NULL){
        cleks_eprintln("Failed to allocate word value!");
        return 1;
    }
    // TODO: check for types int and float
    if (str_is_int(word_value)){
        Cleks_append_token(tokens, TOKEN_INT, word_value);
    }
    else if (str_is_float(word_value)){
        Cleks_append_token(tokens, TOKEN_FLOAT, word_value);
    }
    else {
        Cleks_append_token(tokens, TOKEN_WORD, word_value);
    }
    return 0;
}

int Cleks_lex_string(Clekser *clekser, CleksTokens *tokens)
{
    // TODO: add escape code support
    CLEKS_ASSERT(clekser != NULL && tokens != NULL, "Invalid Arguments: clekser=%p, tokens=%p", clekser, tokens);
    size_t str_start = clekser->index;
    while (clekser->index < clekser->buffer_size){
        if (clekser->buffer[++(clekser->index)] == '"'){
            break;
        }
    }

    char *str_value = strndup(clekser->buffer + str_start, clekser->index - str_start);
    if (str_value == NULL){
        cleks_eprintln("Failed to allocate string value!");
        return 1;
    }
    clekser->mode = CLEKS_P_NONE;
    Cleks_append_token(tokens, TOKEN_STRING, str_value);
    return 0;
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
                    Cleks_append_token(tokens, (CleksTokenType) int_token, NULL);
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
                if (Cleks_lex_string(&clekser, tokens) == 1){
                    Cleks_free_tokens(tokens);
                    return NULL;
                }
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

void Cleks_print_token(CleksToken *token)
{
    if (token == NULL || token->type >= CLEKS_ARR_LEN(TokenStrings)){
        cleks_eprintln("Invalid token: token: %p, type: %d", token, (int) token->type);
        return;
    }
    printf("Token: %s", TokenStrings[token->type]);
    if (token->value != NULL){
        printf(" \"%s\"", token->value);
    }
    putchar('\n');
}

void Cleks_print_tokens(CleksTokens *tokens)
{
    if (tokens == NULL){
        cleks_eprintln("Invalid tokens: %p", tokens);
        return;
    }
    printf("Token count: %u\n  ", tokens->size);
    for (size_t i=0; i<tokens->size; ++i){
        Cleks_print_token(tokens->items[i]);
        printf("  ");
    }
}

// library functions

static char* strndup(char *s, size_t n)
{
    char *n_str = (char*) calloc(n+1, sizeof(*n_str));
    if (n_str != NULL){
        strncpy(n_str, s, n);
    }
    return n_str;
}

static bool str_is_int(char* s){
    if (!s) return false;
    char c;
    if (*s == '+' || *s == '-') s++;
    if (*s == '\0') return false;
    while ((c = *s++)){
        if (!isdigit(c)) return false;
    }
    return true;
}

static bool str_is_float(char* s){
    char* ep = NULL;
    strtod(s, &ep);
    return (ep && !*ep);
}

#endif // _CLEKS_H