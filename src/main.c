
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "tokenizer.h"
#include "compiler.h"

static char *in_filename;
static char *out_filename = "a.out";
static bool assemble = true;

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
    const char *help;
    if (status != EXIT_SUCCESS) {
        help = "Try 'bfc -h' for more information.\n";
    } else {
        help =
            "Usage: bfc [options] <input filename> -o <output filename>"
            "\n"
            "  -t\t\t\tCompile only. Do not assemble or link.\n"
            "  -o <file>\t\tPlace the output into <file>.\n"
            "  -h\t\t\tDisplay this information.\n"
            "  -v\t\t\tDisplay compiler version information.\n"
            "\n"
            "Options must be after name of compiler\n";
    }
    fprintf(status ? stderr : stdout, help);
    exit(status);
}

void parseopt(int argc, char *argv[])
{
    for (;;)
    {
        int opt = getopt(argc, argv, "cto:hv");
        if (opt == -1) {
            break;
        }

        switch (opt)
        {
            case 'c':
                assemble = true;
                break;
            case 't':
                assemble = false;
                break;
            case 'o':
                out_filename = optarg;
                break;
            case 'h':
                usage(EXIT_SUCCESS);
                break;
            case 'v':
                fprintf(stdout, "bfc: version 0.9\n");
                exit(EXIT_SUCCESS);
                break;
            default:
                usage(EXIT_FAILURE);
                break;
        }

        if (optind != argc - 1) {
            usage(EXIT_FAILURE);
        }
        in_filename = argv[optind];
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        usage(EXIT_FAILURE);
    }
    parseopt(argc, argv); 
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
