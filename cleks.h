#ifndef _CLEKS_H
#define _CLEKS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* Customize here */

#define CLEKS_LEX_INTEGER // detect integers
#define CLEKS_LEX_FLOATS  // detect floats

#define CLEKS_PRINT_ID true // enable printing by cleks_info and cleks_debug

// all the known tokens
typedef enum{
    // mandatory tokens
    TOKEN_STRING, // everything within StringDelimeters
    TOKEN_WORD,   // unknown words
    #ifdef CLEKS_LEX_INTEGER
    TOKEN_INT,    // integer literals
    #endif // CLEKS_LEX_INTEGER
    #ifdef CLEKS_LEX_FLOATS
    TOKEN_FLOAT,  // float literals
    #endif // CLEKS_LEX_FLOATS

    // custom tokens
    TOKEN_MAP_OPEN,
    TOKEN_MAP_CLOSE,
    TOKEN_ARRAY_OPEN,
    TOKEN_ARRAY_CLOSE,
    TOKEN_MAP_SEP,
    TOKEN_ITER_SEP,
    TOKEN_TRUE,
    TOKEN_FALSE
} CleksTokenType;

// definiton of values for Cleks_print_tokens
const char* const TokenStrings[] = {
    [TOKEN_TRUE] = "Word: true",
    [TOKEN_FALSE] = "Word: false",
    #ifdef CLEKS_LEX_INTEGER
    [TOKEN_INT] = "Word: integer",
    #endif // CLEKS_LEX_INTEGER
    #ifdef CLEKS_LEX_FLOATS
    [TOKEN_FLOAT] = "Word: float",
    #endif // CLEKS_LEX_FLOATS
    [TOKEN_WORD] = "Word: unknown",
    [TOKEN_STRING] = "String",
    [TOKEN_MAP_OPEN] = "Symbol: {",
    [TOKEN_MAP_CLOSE] = "Symbol: }",
    [TOKEN_ARRAY_OPEN] = "Symbol: [",
    [TOKEN_ARRAY_CLOSE] = "Symbol: ]",
    [TOKEN_MAP_SEP] = "Symbol: :",
    [TOKEN_ITER_SEP] = "Symbol: ,"    
};

// definition of the characters corresponding to the symbol tokens
const char const Symbols[] = {
    [TOKEN_MAP_OPEN] = '{',
    [TOKEN_MAP_CLOSE] = '}',
    [TOKEN_ARRAY_OPEN] = '[',
    [TOKEN_ARRAY_CLOSE] = ']',
    [TOKEN_ITER_SEP] = ',',
    [TOKEN_MAP_SEP] = ':',

    // set non-symbol tokens to '\0'
    [TOKEN_STRING] = '\0',
    [TOKEN_TRUE] = '\0',
    [TOKEN_FALSE] = '\0',
    [TOKEN_WORD] = '\0',
    #ifdef CLEKS_LEX_INTEGER
    [TOKEN_INT] = '\0',
    #endif // CLEKS_LEX_INTEGER
    #ifdef CLEKS_LEX_FLOATS
    [TOKEN_FLOAT] = '\0',
    #endif // CLEKS_LEX_FLOATS
};

// definition of the strings corresponding to the word tokens
const char* const Words[] = {
    [TOKEN_WORD] = "", // has to be empty
    [TOKEN_TRUE] = "true",
    [TOKEN_FALSE] = "false",

    // set non-word tokens to ""
    #ifdef CLEKS_LEX_INTEGER
    [TOKEN_INT] = "",
    #endif // CLEKS_LEX_INTEGER
    #ifdef CLEKS_LEX_FLOATS
    [TOKEN_FLOAT] = "",
    #endif // CLEKS_LEX_FLOATS
    [TOKEN_STRING] = "",
    [TOKEN_MAP_OPEN] = "",
    [TOKEN_MAP_CLOSE] = "",
    [TOKEN_ARRAY_OPEN] = "",
    [TOKEN_ARRAY_CLOSE] = "",
    [TOKEN_MAP_SEP] = "",
    [TOKEN_ITER_SEP] = "",
};

// the characters, which encase a string
const char const StringDelimeters[] = {'"'};

// characters to be skipped when not in a string
const char const Whitespaces[] = {' ', '\n'};

/* Internal definitions */

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

#define CLEKS_ARR_LEN(arr) (sizeof((arr))/sizeof((arr)[0]))
#define CLEKS_ANSI_END "\e[0m"
#define CLEKS_ANSI_RGB(r, g, b) ("\e[38;2;" #r ";" #g ";" #b "m")

#define cleks_info(msg, ...) do{ if (CLEKS_PRINT_ID){(printf("%s%s:%d: " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 255, 255), __FILE__, __LINE__, ## __VA_ARGS__));}} while (0);
#define cleks_debug(msg, ...) do{ if (CLEKS_PRINT_ID){(printf("%s%s:%d: [DEBUG] " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 124, 0), __FILE__, __LINE__, ## __VA_ARGS__));}} while (0);
#define cleks_eprintln(msg, ...) (fprintf(stderr, "%s%s:%d: [ERROR] " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 0, 0), __FILE__, __LINE__, ## __VA_ARGS__))

#define CLEKS_ASSERT(statement, msg, ...) do{if (!(statement)) {cleks_eprintln(msg, ##__VA_ARGS__); exit(1);}} while (0);

#define cleks_c2t(c, arr) (Cleks_char2Token(c, arr, CLEKS_ARR_LEN(arr)))

void Cleks_print_token(CleksToken *token);
static char* strndup(char *s, size_t n);
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
                    free(token->value);
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
        if (cleks_c2t(c, StringDelimeters) != CLEKS_NOT_FOUND || cleks_c2t(c, Whitespaces) != CLEKS_NOT_FOUND || cleks_c2t(c, Symbols) != CLEKS_NOT_FOUND) break;
        clekser->index += 1;
    }
    clekser->mode = CLEKS_P_NONE;
    for (size_t i=0; i<CLEKS_ARR_LEN(Words); ++i){
        if (strncmp(clekser->buffer + word_start, Words[i], clekser->index - word_start) == 0){
            // words match
            Cleks_append_token(tokens, (CleksTokenType) i, NULL);
            return 0;
        }
    }
    char *word_value = strndup(clekser->buffer + word_start, clekser->index - word_start);
    if (word_value == NULL){
        cleks_eprintln("Failed to allocate word value!");
        return 1;
    }
    if (str_is_int(word_value)){
        #ifdef CLEKS_LEX_INTEGER
        Cleks_append_token(tokens, TOKEN_INT, word_value);
        return 0;
        #endif // CLEKS_LEX_INTEGER
    }
    else if (str_is_float(word_value)){
        #ifdef CLEKS_LEX_FLOATS
        Cleks_append_token(tokens, TOKEN_FLOAT, word_value);
        return 0;
        #endif // CLEKS_LEX_FLOATS
    }
    Cleks_append_token(tokens, TOKEN_WORD, word_value);
    return 0;
}

int Cleks_lex_string(Clekser *clekser, CleksTokens *tokens)
{
    CLEKS_ASSERT(clekser != NULL && tokens != NULL, "Invalid Arguments: clekser=%p, tokens=%p", clekser, tokens);
    size_t str_start = clekser->index;
    char c;
    while (clekser->index < clekser->buffer_size){
        c = clekser->buffer[++(clekser->index)];
        CLEKS_ASSERT(c != '\0', "[PARSING] Unclosed string delimeters at index %u!", str_start);
        if (cleks_c2t(c, StringDelimeters) != CLEKS_NOT_FOUND){
            break;
        }
    }

    size_t str_len = clekser->index - str_start;
    char *str_value = (char*) calloc(str_len+1, sizeof(char));
    if (str_value == NULL){
        cleks_eprintln("Failed to allocate string value!");
        return 1;
    }
    
    // handle escape codes
    char *buff_start = clekser->buffer + str_start;
    char *iB = buff_start;
    char *iS = str_value;
    while (iB - buff_start < str_len){
        if (*iB == '\\'){
            switch(*++iB){
                case '\'': *iS = 0x27; break;
                case '"':  *iS = 0x22; break;
                case '?':  *iS = 0x3f; break;
                case '\\': *iS = 0x5c; break;
                case 'a':  *iS = 0x07; break;
                case 'b':  *iS = 0x08; break;
                case 'f':  *iS = 0x0c; break;
                case 'n':  *iS = 0x0a; break;
                case 'r':  *iS = 0x0d; break;
                case 't':  *iS = 0x09; break;
                case 'v':  *iS = 0x0b; break;
            }
        }
        else{
            *iS = *iB;
        }
        iB++;   
        iS++;
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
        switch (clekser.mode){
            case CLEKS_P_NONE:{
                int int_token;
                if ((int_token = cleks_c2t(c, StringDelimeters)) != CLEKS_NOT_FOUND){
                    clekser.mode = CLEKS_P_STRING;
                    clekser.index += 1;
                    continue;
                }
                else if ((int_token = cleks_c2t(c, Symbols)) != CLEKS_NOT_FOUND){
                    Cleks_append_token(tokens, (CleksTokenType) int_token, NULL);
                    clekser.index += 1;
                    continue;
                }
                else if ((int_token = cleks_c2t(c, Whitespaces)) != CLEKS_NOT_FOUND){
                    clekser.index += 1;
                    continue;
                }
                else{
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
                if (Cleks_lex_word(&clekser, tokens) == 1){
                    Cleks_free_tokens(tokens);
                    return NULL;
                }
                continue;
            }break;
            default:{
                CLEKS_ASSERT(0, "[INTERNAL] Invalid parsing mode: %d!", clekser.mode);
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
        cleks_eprintln("Invalid tokens given!");
        return;
    }
    printf("Token count: %u\n  ", tokens->size);
    for (size_t i=0; i<tokens->size; ++i){
        Cleks_print_token(tokens->items[i]);
        printf("  ");
    }
}

// string functions

static char* strndup(char *s, size_t n)
{
    if (s == NULL) return NULL;
    char *n_str = (char*) calloc(n+1, sizeof(char));
    if (n_str != NULL){
        strncpy(n_str, s, n);
    }
    return n_str;
}

static bool str_is_int(char* s)
{
    if (!s) return false;
    char c;
    if (*s == '+' || *s == '-') s++;
    if (*s == '\0') return false;
    while ((c = *s++)){
        if (!isdigit(c)) return false;
    }
    return true;
}

static bool str_is_float(char* s)
{
    char* ep = NULL;
    strtod(s, &ep);
    return (ep && !*ep);
}

#endif // _CLEKS_H