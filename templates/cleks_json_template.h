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

static CleksTokenConfig JsonTokens[] = {
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

const char* const JsonWhitespaces = " \n";
const char* const JsonStringDelimeters = "\"";
const char* const* JsonCommentDelimeters = NULL;

uint8_t JsonTokenMask = CLEKS_DEFAULT;

#endif // _CLEKS_JSON_TEMPLATE_H