#ifndef COMPILE_ASM_X86_LINUX_H
#define COMPILE_ASM_X86_LINUX_H

#include "tokenizer.h"

/* Compiles tokenized source to assembly source code */
int tokens_to_asm_x86_linux(Command* const source, const unsigned int source_length,
	char** output, unsigned int* output_length);


#endif
