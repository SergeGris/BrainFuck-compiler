#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "compile_asm_i386_linux.h"
#include "compiler.h"

// Linux kernel system calls on x86 system
static const int syscall_stdin     = 0;
static const int syscall_stdout    = 1;
static const int syscall_sys_exit  = 1;
static const int syscall_sys_read  = 3;
static const int syscall_sys_write = 4;

static int str_append(char **str, const char *format, ...)
{
    // This is only used to combine arguments, so fixed-size string should
    // be safe to use
    char formatted_str[1000];
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(formatted_str, format, arg_ptr);

    const size_t old_length = (*str == NULL ? 0 : strlen(*str));
    char *new_str = calloc(old_length + strlen(formatted_str) + 1, sizeof(char));
    if (new_str == NULL) {
        // Error: Out of memory
        return 201;
    }

    if (*str != NULL) {
        strcat(new_str, *str);
    }
    strcat(new_str, formatted_str);

    free(*str);
    *str = new_str;

    return 0;
}

int tokens_to_asm_i386_linux(ProgramSource *const source, char **final_output, size_t *final_output_length)
{
    char *output = NULL;
    *final_output = NULL;
    *final_output_length = 0;

    // Initialize variables
    str_append(&output, "section .data\nglobal array\narray times %d db 0\nbuffer db 0\n\n", DATA_ARRAY_SIZE);

    // Beginning of the code block
    str_append(&output, "section .text\nglobal _start\n\n");

    // Subroutines for I/O
    if (!source->no_print_commands) {
        str_append
        (
            &output, "print_char:\n"
            "\t\tpush\teax\n\t\tpush\tebx\n\t\tpush\tecx\n\t\tpush\tedx\n\t\txor\t\tebx,ebx\n\t\tmov\t\tbl,[eax]\n\t\tmov\t\t[buffer],bl\n"
            "\t\tmov\t\teax,%d\n\t\tmov\t\tebx,%d\n\t\tmov\t\tecx,buffer\n\t\tmov\t\tedx,1\n\t\tint\t\t0x80\n"
            "\t\tpop\t\tedx\n\t\tpop\t\tecx\n\t\tpop\t\tebx\n\t\tpop\t\teax\n\t\tret\n\n", syscall_sys_write, syscall_stdout
        );
    }
    if (!source->no_input_commands) {
        str_append
        (
            &output, "input_char:\n"
            "\t\tpush\t\teax\n\t\tpush\t\tebx\n\t\tpush\t\tecx\n\t\tpush\t\tedx\n\t\tmov\t\teax,%d\n\t\tmov\t\tebx,%d\n\t\tmov\t\tecx,buffer\n"
            "\t\tmov\t\tedx,1\n\t\tint\t\t0x80\n\t\tpop edx\n\t\tpop\t\tecx\n\t\tpop\t\tebx\n\t\tpop\t\teax\n\t\tmov\t\tcl,[buffer]\n"
            "\t\tmov\t\t[eax],cl\n\t\tret\n\n", syscall_sys_read, syscall_stdin
        );
    }

    // Execution starts at this point
    str_append(&output, "_start:\n\t\tmov\t\teax,array\n");

    // Convert tokens to machine code
    int errorcode = 0;
    for (size_t i = 0; i < source->length && errorcode == 0; i++) {
        const Command current = source->tokens[i];
        switch (current.token)
        {
        case T_INC:
            if (current.value > 0) {
                str_append(&output, "\t\tmov\t\tbl,%d\n\t\tadd\t\t[eax],bl\n", current.value & 0xFF);
            }
            else if (current.value < 0) {
                str_append(&output, "\t\tmov\t\tbl,%d\n\t\tsub\t\t[eax],bl\n", (-current.value) & 0xFF);
            }
            else {
                /* Command has no effect */
            }
            break;
        case T_POINTER_INC:
            if (current.value > 0) {
                str_append(&output, "\t\tmov\t\tebx,%d\n\t\tadd\t\teax,ebx\n", current.value);
            }
            else if (current.value < 0) {
                str_append(&output, "\t\tmov\t\tebx,%d\n\t\tsub\t\teax,ebx\n", -current.value);
            }
            else {
                /* Command has no effect */
            }
            break;
        case T_LABEL:
            str_append(&output, "\nlabel_%d_begin:\n\t\tcmp\t\tbyte [eax],0\n\t\tje\t\tlabel_%d_end\n",
                       current.value, current.value);
            break;
        case T_JUMP:
            str_append(&output, "\nlabel_%d_end:\n\t\tcmp\t\tbyte [eax],0\n\t\tjne\t\tlabel_%d_begin\n",
                       current.value, current.value);
            break;
        case T_INPUT:
            if (source->no_input_commands) {
                /* Error: Unexpected token */
                errorcode = 202;
            }
            else {
                str_append(&output, "\t\tcall\tinput_char\n");
            }
            break;
        case T_PRINT:
            if (source->no_print_commands) {
                /* Error: Unexpected token */
                errorcode = 202;
            }
            else {
                str_append(&output, "\t\tcall\tprint_char\n");
            }
            break;
        default:
            break;
        }
    }

    /* Write quit commands */
    if (errorcode == 0) {
        str_append(&output, "\n\t\tmov\t\teax,%d\n\t\tmov\t\tebx,0\n\t\tint\t\t0x80\n", syscall_sys_exit);
    }
    if (errorcode != 0) {
        free(output);
        return errorcode;
    }

    *final_output = output;
    *final_output_length = strlen(output);

    return 0;
}
