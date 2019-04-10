
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "tokenizer.h"
#include "compiler.h"

char *read_file(const char *filename)
{
    char *buffer = NULL;
    int length = 0;
    FILE *handle = fopen(filename, "rb");

    if (handle != NULL) {
        fseek(handle, 0, SEEK_END);
        length = ftell(handle);
        fseek(handle, 0, SEEK_SET);
        buffer = malloc((length + 1) * sizeof(char));
        if (buffer) {
            int result = fread(buffer, sizeof(char), length, handle);
            if (result != length) {
                /* Error: Reading failed */
                free(buffer);
                return NULL;
            }
            buffer[length] = 0;
        }
        fclose(handle);
    }
    return buffer;
}

const char *change_extension(char *filename, const char *new_extension)
{
    if (filename == NULL || *filename == '\0') {
        return "";
    }
    char *tmp = filename + strlen(filename) - 1;
    for (; tmp >= filename; tmp--) {
        if (*tmp == '.') {
            for (; *new_extension != '\0';) {
                *tmp++ = *new_extension++;
            }
            *tmp++ = '\0';
            return filename;
        }
    }
    tmp = filename + strlen(filename);
    for (; *new_extension != '\0';) {
        *tmp++ = *new_extension++;
    }
    return filename;
}

int main(int argc, char *argv[])
{
    if (argc <= 1 || argc >= 4) {
        printf("Usage: ./%s [source filename] [output filename]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    char *out_filename = "a.out";
    bool assemble = true;

    char *in_filename = argv[1];
    if (argc == 3) {
        out_filename = argv[2];
    }

    int optimization_level = 1;
    ProgramSource tokenized_source;
    int err = 0;
    
    /* Open file */
    char *source = read_file(in_filename);
    if (source == NULL) {
        printf("Error: Failed to read file %s\n", in_filename);
        exit(EXIT_FAILURE);
    }

    /* Interpret symbols */
    err = tokenize_and_optimize(source, &tokenized_source, optimization_level);
    if (err != 0) {
        printf("Error %d\n", err);
        free(source);
        exit(err);
    }

    /* Write result file */
    err = compile_to_file(out_filename, FILETYPE_ASSEMBLY, &tokenized_source);
    if (err != 0) {
        printf("Error %d\n", err);
    }

    free(tokenized_source.tokens);
    free(source);

    /* Run NASM to compile it if 'assemble' == true */
    if (assemble) {
        char *out_obj = malloc((strlen(out_filename) + 1) * sizeof(char)); /* Reserve 1 byte for '\0' */

        strcpy(out_obj, out_filename);
        change_extension(out_obj, ".o");

        char *as = malloc((strlen("nasm -f elf32 -o  ") + strlen(out_obj) + strlen(out_filename) + 1) * sizeof(char));
        sprintf(as, "nasm -f elf32 -o %s %s", out_obj, out_filename);
        char *ld = malloc((strlen("ld -m elf_i386 -s -o  ") + strlen(out_obj) + strlen(out_filename) + 1) * sizeof(char));
        sprintf(ld, "ld -m elf_i386 -s -o %s %s", out_filename, out_obj);
        char *rm = malloc((strlen("rm ") + strlen(out_obj)) * sizeof(char));
        sprintf(rm, "rm %s", out_obj);

        printf("out_obj = %s\nout_filename = %s\n", out_obj, out_filename);
        printf("%s\n%s\n%s\n", as, ld, rm);
        
        system(as);
        system(ld);
        system(rm);

        /*
        char *as[] = { "nasm", "-f", "elf32", "-o", out_obj, out_filename, (char *) NULL };
        execvp(as[0], as);
        char *ld[] = { "ld", "-m", "elf_i386", "-s", "-o", out_filename, out_obj, (char *) NULL };
        execvp(ld[0], ld);
        char *rm[] = { "rm", out_obj, (char *) NULL };
        execvp(rm[0], rm);*/

        free(as);
        free(ld);
        free(rm);
        free(out_obj);
    }

    return err;
}
