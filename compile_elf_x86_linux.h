#ifndef COMPILE_ELF_X86_LINUX_H
#define COMPILE_ELF_X86_LINUX_H

#include "tokenizer.h"

/* Compiles tokenized source to machine code */
int tokens_to_machinecode_x86_linux(Command* const source, const unsigned int source_length,
	char** output, unsigned int* output_length);

/* Compiles machine code to ELF executable */
int tokens_to_elf_x86_linux(Command* const source, const unsigned int source_length,
	char** output, unsigned int* output_length);


#endif
