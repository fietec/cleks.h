#include "cleks.h"
#include <sys/stat.h>

/* This is an example of how to use cleks.h by reading a file's content and lexing it. 
   It is, of course, also possible to simply provide an other arbitrary char buffer.
   In this example the file is of the JSON format and we therefore include the JSON template. 
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

#include "./templates/cleks_json_template.h" // we use include the definitions and use the CleksJsonConfig below

int main(void)
{
    char *buffer = read_entire_file("testing.json");
    CleksTokens* tokens = Cleks_lex(buffer, strlen(buffer), CleksJsonConfig);
    Cleks_print_tokens(tokens);
    Cleks_free_tokens(tokens);

    return 0;
}