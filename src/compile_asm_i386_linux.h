
#ifndef _COMPILE_ASM_I386_LINUX_H_
#define _COMPILE_ASM_I386_LINUX_H_

#include "tokenizer.h"

/* Compiles tokenized source to assembly source code */
int tokens_to_asm_i386_linux(ProgramSource *const source, char **final_output, size_t *final_output_length);

#endif

