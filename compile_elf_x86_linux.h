#ifndef COMPILE_ELF_X86_LINUX_H
#define COMPILE_ELF_X86_LINUX_H

#include "tokenizer.h"

/* Compiles tokenized source to machine code */
int tokens_to_machinecode_x86_linux(Command* const source, const int source_length,
	char** output, int* output_length);

/* Compiles machine code to ELF executable */
int write_elf_file_x86(const char* filename, const char* source,
	const int source_length);


#endif
