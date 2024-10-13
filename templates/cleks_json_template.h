#ifndef _CLEKS_JSON_TEMPLATE_H
#define _CLEKS_JSON_TEMPLATE_H
#include "../cleks.h"

enum JsonTokens{
    JSON_MAP_OPEN,
    JSON_MAP_CLOSE,
    JSON_ARRAY_OPEN,
    JSON_ARRAY_CLOSE,
    JSON_MAP_SEP,
    JSON_ITER_SEP,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NULL
};

static CleksTokenConfig JsonTokenConfig[] = {
    [JSON_MAP_OPEN] = {"JsonMapOpen: '{'" , "", '{'},
    [JSON_MAP_CLOSE] = {"JsonMapClose: '}'", "", '}'},
    [JSON_ARRAY_OPEN] = {"JsonArrayOpen: '['", "", '['},
    [JSON_ARRAY_CLOSE] = {"JsonArrayClose: ']'", "", ']'},
    [JSON_MAP_SEP] = {"JsonMapSep: ':'", "", ':'},
    [JSON_ITER_SEP] = {"JsonIterSet: ','", "", ','},
    [JSON_TRUE] = {"JsonTrue: true", "true", '\0'},
    [JSON_FALSE] = {"JsonFalse: false", "false", '\0'},
    [JSON_NULL] = {"JsonNull: null", "null", '\0'}
};

CleksComment JsonComments[] = {
    {"//", "\n"},
    {"#", "\n"},
    {"/*", "*/"}
};

CleksConfig JsonConfig = {
    .default_tokens = CleksDefaultTokenConfig,
    .default_token_count = DEFAULT_TOKEN_COUNT,
    .custom_tokens = JsonTokenConfig,
    .custom_token_count = CLEKS_ARR_LEN(JsonTokenConfig),
    .whitespaces = " \n",
    .string_delimters = "\"",
    .comments = JsonComments,
    .comment_count = CLEKS_ARR_LEN(JsonComments),
    .token_mask = CLEKS_DEFAULT
};

#endif // _CLEKS_JSON_TEMPLATE_H