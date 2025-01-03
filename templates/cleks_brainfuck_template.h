#ifndef _CLEKS_BRAINFUCK_TEMPLATE
#define _CLEKS_BRAINFUCK_TEMPLATE
#include "../cleks.h"

enum BrainfuckTokens{
    INCR_DP,
    DECR_DP,
    INCR_DATA,
    DECR_DATA,
    DATA_OUT,
    DATA_IN,
    JUMP_FWD,
    JUMP_BWD
};

static CleksTokenConfig BrainfuckTokenConfig[] = {
    [INCR_DP] = {"IncrDP", "", '>'},
    [DECR_DP] = {"DecrDP", "", '<'},
    [INCR_DATA] = {"IncrData", "", '+'},
    [DECR_DATA] = {"DecrData", "", '-'},
    [DATA_OUT] = {"DataOut", "", '.'},
    [DATA_IN] = {"DataIn", "", ','},
    [JUMP_FWD] = {"JumpFwd", "", '['},
    [JUMP_BWD] = {"JumpBwd", "", ']'}
};

/*
    Brainfuck interprets every character that is not one of the commands as a comment. 
    Cleks does not support this feature directly, yet every comment will be lexed as a 'TOKEN_WORD'.
    When using the tokens, simply ignore these tokens and everything should behave as usual.
*/

CleksComment BrainfuckComments[] = {};
CleksString BrainfuckStrings[] = {};

CleksConfig BrainfuckConfig = {
    .default_tokens = CleksDefaultTokenConfig,
    .default_token_count = CLEKS_TOKEN_COUNT,
    .custom_tokens = BrainfuckTokenConfig,
    .custom_token_count = CLEKS_ARR_LEN(BrainfuckTokenConfig),
    .whitespaces = "",
    .strings = BrainfuckStrings,
    .string_count = CLEKS_ARR_LEN(BrainfuckStrings),
    .comments = BrainfuckComments,
    .comment_count = CLEKS_ARR_LEN(BrainfuckComments),
    .flags = CLEKS_FLAG_NO_INTEGERS | CLEKS_FLAG_NO_FLOATS
};

#endif // _CLEKS_BRAINFUCK_TEMPLATE