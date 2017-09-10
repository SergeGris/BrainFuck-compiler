#ifndef COMPILER_H
#define COMPILER_H

#include "tokenizer.h"

// Output file type
typedef enum {
	FILETYPE_ASSEMBLY,
	FILETYPE_ELF
} FileType;

/* Compiles tokenized source to assembly source code */
int tokens_to_assembly(Command* const source, const int source_length,
	char** output, int* output_length);

/* Writes assembly source code to a file */
int write_text_file(const char* filename, const char* source,
	const int source_length);

/* Compiles tokenized source to machine code */
int tokens_to_machinecode(Command* const source, const int source_length,
	char** output, int* output_length);

/* Compiles machine code to ELF executable */
int write_elf_file(const char* filename, const char* source,
	const int source_length);

/* Compiles tokenized source to ELF executable */
int compile_to_file(const char* filename, const FileType filetype,
	Command* const source, const int source_length);

#endif
