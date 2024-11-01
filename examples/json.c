#include "../cleks.h"
#include "../templates/cleks_json_template.h" // include the template

int main(void)
{
    char buffer[] = "{\"nums\": [1, 2, 3], \"truth\": [true, false, null]}"; // our test buffer
    CleksTokens* tokens = Cleks_lex(buffer, strlen(buffer), JsonConfig); // use the JsonConfig provided by the template
    Cleks_print_tokens(tokens);
    Cleks_free_tokens(tokens);
    return 0;
}