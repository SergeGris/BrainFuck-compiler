#ifndef COMPILER_H
#define COMPILER_H

#include "tokenizer.h"

// Output file type
typedef enum {
	FILETYPE_ASSEMBLY,
	FILETYPE_ELF
} FileType;

// Maximum size of the data array
#define DATA_ARRAY_SIZE 30000

/* Compiles tokenized source to executable */
int compile_to_file(const char* filename, const FileType filetype,
	Command* const source, const unsigned int source_length);

#endif
