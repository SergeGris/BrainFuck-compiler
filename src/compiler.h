
#ifndef _COMPILER_H_
#define _COMPILER_H_

#include "tokenizer.h"

// Output file type
typedef enum { FILETYPE_ASSEMBLY } FileType;

// Maximum size of the data array
#define DATA_ARRAY_SIZE 30000

// Compiles tokenized source to executable
int compile_to_file(const char *filename, const FileType filetype, ProgramSource *const source);

#endif
