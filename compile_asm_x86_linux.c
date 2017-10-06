#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "compile_asm_x86_linux.h"
#include "compiler.h"

// Linux kernel system calls
static const int syscall_stdin = 0;
static const int syscall_stdout = 1;
static const int syscall_sys_exit = 1;
static const int syscall_sys_read = 3;
static const int syscall_sys_write = 4;

static int str_append(char** str, const char* format, ...)
{
	// This is only used to combine arguments, so fixed-size string should
	// be safe to use
	char formatted_str[1000];
	va_list arg_ptr;
	va_start(arg_ptr, format);
	vsprintf(formatted_str, format, arg_ptr);

	const unsigned int old_length = (*str == NULL ? 0 : strlen(*str));
	char* new_str = calloc(old_length + strlen(formatted_str) + 1, sizeof(char));
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

int tokens_to_asm_x86_linux(ProgramSource* const source,
	char** final_output, unsigned int* final_output_length)
{
	char* output = NULL;
	*final_output = NULL;
	*final_output_length = 0;

	// Initialize variables
	str_append(&output, "bits 32\n\n");
	str_append(&output, "section .data\nglobal array\narray times %d db 0\nbuffer db 0\n\n", DATA_ARRAY_SIZE);

	// Beginning of the code block
	str_append(&output, "section .text\nglobal _start\n\n");

	// Subroutines for I/O
	if (!source->no_print_commands) {
		str_append(&output, "print_char:\n"
			"push eax\npush ebx\npush ecx\npush edx\nxor ebx, ebx\nmov bl, [eax]\nmov [buffer], bl\n"
			"mov eax, %d\nmov ebx, %d\nmov ecx, buffer\nmov edx, 1\nint 0x80\n"
			"pop edx\npop ecx\npop ebx\npop eax\nret\n\n", syscall_sys_write, syscall_stdout);
	}
	if (!source->no_input_commands) {
		str_append(&output, "input_char:\n"
			"push eax\npush ebx\npush ecx\npush edx\nmov eax, %d\nmov ebx, %d\nmov ecx, buffer\n"
			"mov edx, 1\nint 0x80\npop edx\npop ecx\npop ebx\npop eax\nmov cl, [buffer]\n"
			"mov [eax], cl\nret\n\n", syscall_sys_read, syscall_stdin);
	}

	// Execution starts at this point
	str_append(&output, "_start:\nmov eax, array\n");

	// Convert tokens to machine code
	int errorcode = 0;
	for (unsigned int i = 0; i < source->length && errorcode == 0; i++) {
		const Command current = source->tokens[i];
		switch (current.token) {
		case T_INC:
			if (current.value > 0) {
				str_append(&output, "mov bl, %d\nadd [eax], bl\n", current.value & 0xFF);
			}
			else if (current.value < 0) {
				str_append(&output, "mov bl, %d\nsub [eax], bl\n", (-current.value) & 0xFF);
			}
			else {
				// Command has no effect
			}
			break;
		case T_POINTER_INC:
			if (current.value > 0) {
				str_append(&output, "mov ebx, %d\nadd eax, ebx\n", current.value);
			}
			else if (current.value < 0) {
				str_append(&output, "mov ebx, %d\nsub eax, ebx\n", -current.value);
			}
			else {
				// Command has no effect
			}
			break;
		case T_LABEL:
			str_append(&output, "\nlabel_%d_begin:\ncmp byte [eax], 0\nje label_%d_end\n",
				current.value, current.value);
			break;
		case T_JUMP:
			str_append(&output, "\nlabel_%d_end:\ncmp byte [eax], 0\njne label_%d_begin\n",
				current.value, current.value);
			break;
		case T_READ:
			if (source->no_input_commands) {
				// Error: Unexpected token
				errorcode = 202;
			}
			else {
				str_append(&output, "call input_char\n");
			}
			break;
		case T_PRINT:
			if (source->no_print_commands) {
				// Error: Unexpected token
				errorcode = 202;
			}
			else {
				str_append(&output, "call print_char\n");
			}
			break;
		default:
			break;
		}
	}

	// Write quit commands
	if (errorcode == 0) {
		str_append(&output, "\nmov eax, %d\nmov ebx, 0\nint 0x80\n", syscall_sys_exit);
	}
	if (errorcode != 0) {
		free(output);
		return errorcode;
	}

	*final_output = output;
	*final_output_length = strlen(output);

	return 0;
}
