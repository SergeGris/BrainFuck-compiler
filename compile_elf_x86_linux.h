#ifndef COMPILE_ELF_X86_LINUX_H
#define COMPILE_ELF_X86_LINUX_H

#include "tokenizer.h"

/* Compiles tokenized source to machine code */
int tokens_to_machinecode_x86_linux(ProgramSource* const source,
	char** final_output, unsigned int* final_output_length);

/* Compiles machine code to ELF executable */
int tokens_to_elf_x86_linux(ProgramSource* const source,
	char** final_output, unsigned int* final_output_length);


#endif
