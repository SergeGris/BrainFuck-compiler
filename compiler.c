#include <stdio.h>

#include "compiler.h"
#include "compile_asm_x86_linux.h"
#include "compile_elf_x86_linux.h"

static int write_text_file(const char* filename, const char* source,
	const unsigned int source_length)
{
	FILE* handle = fopen(filename, "w");
	if (!handle) {
		// Error: Writing failed
		return 203;
	}
	int result = fwrite(source , sizeof(char), source_length, handle);
	if (result != source_length) {
		// Error: Writing failed
		return 203;
	}
	fclose(handle);
	return 0;
}

int compile_to_file(const char* filename, const FileType filetype,
	Command* const source, const unsigned int source_length)
{
	char *instructions = NULL;
	int instructions_length = 0;
	int err = 0;
	if (filetype == FILETYPE_ELF) {
		err = tokens_to_machinecode_x86_linux(source, source_length, &instructions, &instructions_length);
		if (err != 0) {
			return err;
		}
		err = write_elf_file_x86(filename, instructions, instructions_length);
	}
	else if (filetype == FILETYPE_ASSEMBLY) {
		err = tokens_to_asm_x86_linux(source, source_length, &instructions, &instructions_length);
		if (err != 0) {
			return err;
		}
		err = write_text_file(filename, instructions, instructions_length);
	}
	free(instructions);

	return err;
}
