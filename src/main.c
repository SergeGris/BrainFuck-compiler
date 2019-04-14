
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <locale.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>

#include "tokenizer.h"
#include "compiler.h"

char *
read_file(const char *filename)
{
    char *buffer = NULL;
    size_t length = 0;
    FILE *handle = fopen(filename, "rb");

    if (handle != NULL) {
        fseek(handle, 0, SEEK_END);
        length = ftell(handle);
        fseek(handle, 0, SEEK_SET);
        buffer = malloc((length + 1) * sizeof(char));
        if (buffer) {
            size_t result = fread(buffer, sizeof(char), length, handle);
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

const char *
change_extension(char *filename, const char *new_extension)
{
    if (filename == NULL || *filename == '\0') {
        return "";
    }
    
    size_t len = strlen(filename);
    char *tmp = filename + len - 1;

    for (; tmp >= filename; tmp--) {
        if (*tmp == '.') {
            for (; *new_extension != '\0';) {
                *tmp++ = *new_extension++;
            }
            *tmp++ = '\0';
            return filename;
        }
    }
    tmp = filename + len;
    for (; *new_extension != '\0';) {
        *tmp++ = *new_extension++;
    }
    return filename;
}

static char *in_filename = NULL;
static char *out_filename = "a.out";
static bool assemble = true;

void
parseopt(int argc, char **argv)
{
    size_t argv_len;
    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "-o", 2)) {
            if (i == argc - 1) {
                puts("bfc: \x1b[31merror:\x1b[0m missing filename after \u2018\x1b[1m-o\x1b[0m\u2019 ");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[i + 1], "a.out")) {
                /* If filename after '-o' is 'a.out', do not change it.  */
                out_filename = argv[i + 1];
            }
        }
        else if (!strncmp(argv[i], "-s", 2)) {
            assemble = false;
        }

        argv_len = strlen(argv[i]);
        if (argv[i][argv_len - 1] == 'f'
         && argv[i][argv_len - 2] == 'b'
         && argv[i][argv_len - 3] == '.'
         && in_filename == NULL /* TODO: make linking 2 or more .bf files */) {
            in_filename = argv[i];
        }
    }

    if (in_filename == NULL) {
        puts("bfc: \x1b[31mfatal error:\x1b[0m no input files");
        puts("compilation terminated.");
        exit(EXIT_FAILURE);
    }
}

int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    parseopt(argc, argv);

    if (!assemble) {
        const size_t infn_len = strlen(in_filename);
        char *buf = malloc(infn_len - 1); /* Allocate one less byte, '.bf' -> '.s' */

        snprintf(buf, infn_len - 1, in_filename); /* Write in buf filename + '.' */
        buf[infn_len - 2] = 's';
        buf[infn_len - 1] = '\0';
        
        out_filename = buf;
    }
    
    int optimization_level = 1;
    ProgramSource tokenized_source;
    int err = 0;
    
    /* Open file */
    char *source = read_file(in_filename);
    if (source == NULL) {
        printf("bfc: \x1b[31mfatal error:\x1b[0m failed to read file %s\n", in_filename);
        exit(EXIT_FAILURE);
    }

    /* Interpret symbols */
    err = tokenize_and_optimize(source, &tokenized_source, optimization_level);
    if (err != 0) {
        printf("bfc: \x1b[31merror code:\x1b[0m %d\n", err);
        free(source);
        exit(err);
    }

    /* Write result file */
    err = compile_to_file(out_filename, FILETYPE_ASSEMBLY, &tokenized_source);
    if (err != 0) {
        printf("bfc: \x1b[31merror code:\x1b[0m %d\n", err);
    }

    free(tokenized_source.tokens);
    free(source);

    /* Run NASM to compile it if 'assemble' == true */
    if (assemble) {
        char *out_obj = malloc((strlen(out_filename) + 1) * sizeof(char)); /* Reserve 1 byte for '\0' */
        pid_t pid = -1;
        int status;

        strcpy(out_obj, out_filename);
        change_extension(out_obj, ".o");

        char *as[] = { "nasm", "-f", "elf32", "-o", out_obj, out_filename, (char *) NULL };
        char *ld[] = { "ld", "-m", "elf_i386", "-s", "-o", out_filename, out_obj, (char *) NULL };

        printf("Object filename = %s\nOutput filename = %s\n", out_obj, out_filename);

        if (assemble) {
            if ((pid = fork()) < 0) {
                /* Fork error */
                puts("Error while fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) {
                /* Child process */
                if ((pid = fork()) < 0) {
                    /* Fork error */
                    puts("Error while fork");
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0) {
                    /* Child process */
                    execvp(as[0], as);
                }
                else /* pid > 0 */ {
                    /* Parent process */
                    wait(&status);
                    execvp(ld[0], ld);
                }
            }
            else /* pid > 0 */ {
                /* Parent process */
                wait(&status);
            }
        }
        
        free(out_obj);
    }
    else {
        free(out_filename);
    }

    return err;
}

