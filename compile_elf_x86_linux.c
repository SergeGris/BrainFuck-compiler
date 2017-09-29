#include <stdio.h>
#include <stdlib.h>

#include "compile_elf_x86_linux.h"
#include "compiler.h"

// Linux kernel system calls
static const char syscall_stdin = 0;
static const char syscall_stdout = 1;
static const char syscall_sys_exit = 1;
static const char syscall_sys_read = 3;
static const char syscall_sys_write = 4;

static unsigned int count_labels(Command* const source, const unsigned int source_length)
{
	// TODO
	return 0;
}

static int buffer_append(char** output, unsigned int* output_length,
	const char* input, const unsigned int input_length)
{
	// TODO
	return 202;
}

int tokens_to_machinecode_x86_linux(Command* const source, const unsigned int source_length,
	char** final_output, unsigned int* final_output_length)
{
	char* output = NULL;
	unsigned int output_length = 0;
	*final_output = NULL;
	*final_output_length = 0;

	// Addresses of variables and subroutines
	unsigned int addr_array_0 = 0;
	unsigned int addr_buffer = 0;
	unsigned int addr_print_char = 0;
	unsigned int addr_input_char = 0;

	// Addresses of labels
	/*const unsigned int label_count = count_labels(source, source_length);
	unsigned int *addr_label = NULL;
	if (label_count > 0) {
		addr_label = malloc(label_count * sizeof(unsigned int));
		if (addr_label == NULL) {
			// Error: Out of memory
			return 201;
		}
	}*/

	// TODO: Write start point for the program

	// Initialize pointer to the array
	{
		// TODO: addr_array_0 is not yet known
		const char a1 = addr_array_0 & 0XFF;
		const char a2 = (addr_array_0 >> 8) & 0xFF;
		const char a3 = (addr_array_0 >> 16) & 0xFF;
		const char a4 = (addr_array_0 >> 24) & 0xFF;
		char commands[] = {a1, 0xB8, a3, a2,
			// 0x90 = nop
			0x90, a4};
		buffer_append(&output, &output_length, commands, sizeof(commands));
	}

	// TODO: Move these to the end of the file
	// Write subroutines
	addr_print_char = output_length + 1;
	{
		// TODO: addr_buffer is not yet known
		const char b1 = addr_buffer & 0XFF;
		const char b2 = (addr_buffer >> 8) & 0xFF;
		const char b3 = (addr_buffer >> 16) & 0xFF;
		const char b4 = (addr_buffer >> 24) & 0xFF;
		char commands[] = {0x53, 0x50, 0x52, 0x51, 0xDB, 0x31, 0x18, 0x8A, 0x1D, 0x89,
			b2, b1, b4, b3, syscall_sys_write, 0xB8, 0x00, 0x00, 0xBB, 0x00, 0x00, syscall_stdout,
			0x00, 0x00, b1, 0xB9, b3, b2, 0xBA, b4, 0x00, 0x00, 0x01, 0x00, 0x00,
			0x80, 0xCD, 0x59, 0x5A, 0x58, 0x5B,
			// 0x90 = nop
			0x90, 0xC3};
		buffer_append(&output, &output_length, commands, sizeof(commands));
	}

	addr_input_char = output_length + 1;
	{
		// TODO: addr_buffer is not yet known
		const char b1 = addr_buffer & 0XFF;
		const char b2 = (addr_buffer >> 8) & 0xFF;
		const char b3 = (addr_buffer >> 16) & 0xFF;
		const char b4 = (addr_buffer >> 24) & 0xFF;
		char commands[] = {0x53, 0x50, 0x52, 0x51, syscall_sys_read, 0x00, 0x00, 0xBB, 0x00,
			0x00, 0x00, 0x00, 0x00, b1, 0xB9, b3, b2, 0xBA, b4, 0x00, syscall_stdin, 0x00, 0x00,
			0x80, 0xCD, 0x59, 0x5A, 0x58, 0x5B, 0x0D, 0x8B, b2, b1, b4, b3, 0x08, 0x88,
			// 0x90 = nop
			0x90, 0xC3};
		buffer_append(&output, &output_length, commands, sizeof(commands));
	}

	// Convert tokens to machine code
	int errorcode = 0;
	for (unsigned int i = 0; i < source_length && errorcode == 0; i++) {
		switch (source[i].token) {
		case T_INC:
			if (source[i].value == 0) {
				// Command has no effect
				continue;
			}
			else {
				const char value = (source[i].value > 0 ? source[i].value : -source[i].value) & 0xFF;
				const char cmd1 = source[i].value > 0 ? 0x00 : 0x28;
				char commands[] = {value, 0xB3, 0x18, cmd1};
				buffer_append(&output, &output_length, commands, sizeof(commands));
			}
			break;
		case T_POINTER_INC:
			if (source[i].value == 0) {
				// Command has no effect
				continue;
			}
			else {
				const char value = source[i].value > 0 ? source[i].value : -source[i].value;
				const char v1 = value & 0XFF;
				const char v2 = (value >> 8) & 0xFF;
				const char v3 = (value >> 16) & 0xFF;
				const char v4 = (value >> 24) & 0xFF;
				const char cmd1 = source[i].value > 0 ? 0x01 : 0x29;
				char commands[] = {v1, 0xBB, v3, v2, cmd1, v4, 0x00, 0xD8};
				buffer_append(&output, &output_length, commands, sizeof(commands));
			}
			break;
		case T_LABEL:
			//str_append(&output, "\nlabel_%d_begin:\ncmp byte [eax], 0\nje label_%d_end\n",
				//source[i].value, source[i].value);
			break;
		case T_JUMP:
			//str_append(&output, "\nlabel_%d_end:\ncmp byte [eax], 0\njne label_%d_begin\n",
				//source[i].value, source[i].value);
			break;
		case T_READ:
			// relative call = E8
			//str_append(&output, "call input_char\n");
			break;
		case T_PRINT:
			//str_append(&output, "call print_char\n");
			break;
		default:
			break;
		}
	}

	if (errorcode == 0) {
		// Write quit commands
		char commands[] = {syscall_sys_exit, 0xB8, 0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xCD};
		buffer_append(&output, &output_length, commands, sizeof(commands));

		// Write zeros for array and buffer
		addr_buffer = output_length + 1;
		addr_array_0 = addr_buffer + 1;
		char zeros[DATA_ARRAY_SIZE + 1] = {};
		buffer_append(&output, &output_length, zeros, sizeof(zeros));
	}


	//free(addr_label);
	if (errorcode != 0) {
		free(output);
		return errorcode;
	}

	*final_output = output;
	*final_output_length = output_length;

	// Error: Not implemented
	return 202;
}

int tokens_to_elf_x86_linux(Command* const source, const unsigned int source_length,
	char** output, unsigned int* output_length)
{
	// Error: Not implemented
	return 202;
}
