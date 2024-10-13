#include "cleks.h"

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

CleksTokenConfig TestTokenConfig[] = {
    [TEST_LT] = {"Less-Than", "", '<'},
    [TEST_GT] = {"Greater-Than", "", '>'},
    [TEST_EQ] = {"Equals", "", '='},
    [TEST_IF] = {"If", "if", '\0'},
    [TEST_THEN] = {"Then", "then", '\0'},
    [TEST_LB] = {"Left-Bracket", "", '('},
    [TEST_RB] = {"Right-Bracket", "", ')'},
    [TEST_PRINT] = {"Function-Print", "print", '\0'}
};

const char* const TestCommentDelimeters[] = {"//", "#"};

int main(void)
{
    CleksConfig TestConfig = {
        .default_tokens = CleksDefaultTokenConfig,
        .default_token_count = DEFAULT_TOKEN_COUNT,
        .custom_tokens = TestTokenConfig,
        .custom_token_count = CLEKS_ARR_LEN(TestTokenConfig),
        .whitespaces = " \n",
        .string_delimters = "\"'",
        .comment_delimeters = TestCommentDelimeters,
        .comment_delimeter_count = CLEKS_ARR_LEN(TestCommentDelimeters),
        .token_mask = CLEKS_DEFAULT
    };

    char buffer[] = "if(x < 2) then print('x+1\")";
    CleksTokens *tokens = Cleks_lex(buffer, strlen(buffer), TestConfig);
    Cleks_print_tokens(tokens);
    Cleks_free_tokens(tokens);
    return 0;
}