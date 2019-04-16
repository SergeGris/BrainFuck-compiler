
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

    for (; tmp >= filename; --tmp) {
        if (*tmp == '.') {
            ++tmp;
            while (*new_extension != '\0') {
                *tmp++ = *new_extension++;
            }
            *tmp++ = '\0';
            return filename;
        }
    }

    tmp = filename + len;
    *tmp++ = '.';
    while (*new_extension != '\0') {
        *tmp++ = *new_extension++;
    }
    *tmp++ = '\0';

    return filename;
}

char *cut_path(char *str)
{
    char *tmp = strchr(str, '\0');

    while (tmp >= str && *tmp != '/') {
        --tmp;
    }

    return tmp + 1;
}

static char *in_filename       = NULL;
static char *out_filename      = "a.out";
static bool assemble_it        = true;
static bool link_it            = true;
static int  optimization_level = -1;

void __attribute__((__noreturn__))
usage(int status)
{
    if (status != EXIT_SUCCESS) {
        fputs("Try 'bfc --help' for more information.\n", stderr);
    }
    else {
        puts("Usage: bfc [-sco:O:]\n"
             "  --help                   Display this information and exit.\n"
             "  -s                       Compile only, do not assemble or link.\n"
             "  -c                       Compile and assemble, but do not link.\n"
             "  -o <file>                Place the output into <file>.\n"
             "  -On                      Level of optimization, default is 1.\n");
    }

    exit(status);
}

void
parseopt(int argc, char **argv)
{
    size_t argvi_len;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-o") && !strcmp(out_filename, "a.out")) {
            if (i == argc - 1) {
                puts("bfc: \x1b[31merror:\x1b[0m missing filename after \u2018\x1b[1m-o\x1b[0m\u2019 ");
                exit(EXIT_FAILURE);
            }
            out_filename = argv[i + 1];
        }
        else if (!strcmp(argv[i], "-s")) {
            assemble_it = false;
            link_it = false;
        }
        else if (!strcmp(argv[i], "-c")) {
            assemble_it = true;
            link_it = false;
        }
        else if (!strcmp(argv[i], "--help")) {
            usage(EXIT_SUCCESS);
        }
        else if (!strncmp(argv[i], "-O", 2)) {
            optimization_level = argv[i][2] - '0';
        }

        argvi_len = strlen(argv[i]);
        /* BrainFuck source code files must have '.bf' extension.  */
        if (argv[i][argvi_len - 1] == 'f'
         && argv[i][argvi_len - 2] == 'b'
         && argv[i][argvi_len - 3] == '.'
         && in_filename == NULL /* 'in_filename' by default is NULL */) {
            in_filename = argv[i];
        }
    }

    if (in_filename == NULL) {
        puts("bfc: \x1b[31mfatal error:\x1b[0m no input files");
        puts("compilation terminated.");
        exit(EXIT_FAILURE);
    }
    if (optimization_level == -1) {
        optimization_level = 1;
    }
}

int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    parseopt(argc, argv);

    bool out_filename_was_allocated = false;
    if ((!assemble_it || !link_it) && !strcmp(out_filename, "a.out")) {
        char *cut_in_filename = cut_path(in_filename);
        const size_t len = strlen(cut_in_filename) - 1;
        char *buf = malloc(len * sizeof(char)); /* Allocate one less byte, '.bf' -> '.s' or '.o' */

        snprintf(buf, len, cut_in_filename); /* Write in buf filename + '.' */
        if (!assemble_it) {
            buf[len - 1] = 's';
        }
        else {
            buf[len - 1] = 'o';
        }
        buf[len] = '\0';

        out_filename = buf;
        out_filename_was_allocated = true;
    }

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

    char *out_obj = malloc((strlen(out_filename) + 1) * sizeof(char)); /* Reserve 1 byte for '\0' */
    pid_t pid = -1;
    int status;
    strcpy(out_obj, out_filename);
    change_extension(out_obj, "o");

    /* Run GAS to compile it if 'assemble' == true */
    if (assemble_it) {
        char *as[] = { "as", "--32", "-o", out_obj, out_filename, (char *) NULL };

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
            printf("Assembler returned %d.\n", status);
        }
    }
    /* Run LD to link it if 'link' == true */
    if (link_it) {
        char *ld[] = { "ld", "-m", "elf_i386", "-s", "-o", out_filename, out_obj, (char *) NULL };

        if ((pid = fork()) < 0) {
            /* Fork error */
            puts("Error while fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            execvp(ld[0], ld);
        }
        else /* pid > 0 */ {
            /* Parent process */
            wait(&status);
            printf("Linker returned %d.\n", status);
        }
    }

    free(out_obj);

    if (out_filename_was_allocated) {
        free(out_filename);
    }

    return err;
}

