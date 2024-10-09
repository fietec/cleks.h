#include "cleks.h"
#include <sys/stat.h>

/* This is an example of how to use cleks.h by reading a file's content and lexing it. 
   It is, of course, also possible to simply an other arbitrary char buffer.
   But since the default language of the lexer is JSON it is quite reasonable to have it parse a JSON file. (I guess) 
*/

// get the size of the file to read
unsigned long long file_size(const char* file_path){
    struct stat file;
    if (stat(file_path, &file) == -1){
        cleks_eprintln("Failed to open file!");
        return 0;
    }
    return (unsigned long long) file.st_size;
}

// allocate and populate a string with the file's content
char* read_entire_file(char *file_path)
{
    if (file_path == NULL) return NULL;
    FILE *file = fopen(file_path, "r");
    if (file == NULL) return NULL;
    unsigned long long size = file_size(file_path);
    char *content = (char*) calloc(size+1, sizeof(*content));
    if (!content){
        fclose(file);
        return NULL;
    }
    fread(content, 1, size, file);
    fclose(file);
    return content;
}

int main(void)
{
    char *buffer = read_entire_file("testing.json");
    CLEKS_ASSERT(buffer != NULL, "Failed to read file!");
    CleksTokens *tokens = Cleks_lex(buffer, strlen(buffer));  // Alternatively just provide any other buffer
    CLEKS_ASSERT(tokens != NULL, "Lexing failed!");
    Cleks_print_tokens(tokens);
    Cleks_free_tokens(tokens);
    free(buffer);
    return 0;
}