
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "tokenizer.h"
#include "compiler.h"

char *read_file(const char *filename)
{
    char *buffer = NULL;
    int length = 0;
    FILE *handle = fopen(filename, "rb");

    if (handle)
    {
        fseek(handle, 0, SEEK_END);
        length = ftell(handle);
        fseek(handle, 0, SEEK_SET);
        buffer = malloc((length + 1) * sizeof(char));
        if (buffer)
        {
            int result = fread(buffer, sizeof(char), length, handle);
            if (result != length)
            {   // Error: Reading failed
                free(buffer);
                return NULL;
            }
            buffer[length] = 0;
        }
        fclose(handle);
    }
    return buffer;
}

void usage(int status)
{
    if (status != EXIT_SUCCESS) {
        //emit_try_help();
    } else {
        printf("Usage: %s [source filename] [assembler output]\nor: %s [source filename] [ELF output]", "bft", "bfc");
        exit(status);
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1 || argc > 3) {
        usage(EXIT_FAILURE);
    }
    char *in_filename = argv[1];
    
    char *out_filename = "a.out";
    if (argc == 3) {
        char *out_filename = argv[2];
    }
    int optimization_level = 1;

    // Open file
    char *source = read_file(in_filename);
    if (source == NULL) {
        printf("Error: Failed to read file %s\n", in_filename);
        exit(EXIT_FAILURE);
    }

    // Interpret symbols
    ProgramSource tokenized_source;
    int err = tokenize_and_optimize(source, &tokenized_source, optimization_level);
    if (err != 0) {
        printf("Error %d\n", err);
        free(source);
        exit(err);
    }

    // Write result file
    err = compile_to_file(out_filename, FILETYPE_ASSEMBLY, &tokenized_source);
    if (err != 0) {
        printf("Error %d\n", err);
    }
    free(tokenized_source.tokens);
    free(source);

    return err;
}
