#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"
#include "compiler.h"

char* read_file(const char* filename)
{
	char* buffer = NULL;
	int length = 0;
	FILE* handle = fopen(filename, "rb");

	if (handle) {
		fseek(handle, 0, SEEK_END);
		length = ftell (handle);
		fseek(handle, 0, SEEK_SET);
		buffer = malloc((length + 1) * sizeof(char));
		if (buffer) {
			int result = fread(buffer, sizeof(char), length, handle);
			if (result != length) {
				// Error: Reading failed
				free(buffer);
				return NULL;
			}
			buffer[length] = 0;
		}
		fclose(handle);
	}

	return buffer;
}

int main(int argc, char **argv)
{
	if (argc <= 2) {
		printf("Usage: ./brainfuck [source filename] [output filename]\n");
		exit(1);
	}
	char* in_filename = argv[1];
	char* out_filename = argv[2];

	// Open file
	char* source = read_file(in_filename);
	if (source == NULL) {
		printf("Error: Failed to read file %s\n", in_filename);
		exit(1);
	}

	// Interpret symbols
	Command *tokenized_source;
	unsigned int tokenized_source_length = 0;
	int err = tokenize(source, &tokenized_source, &tokenized_source_length);
	if (err != 0) {
		printf("Error %d\n", err);
		free(source);
		exit(err);
	}

	// Write result file
	err = compile_to_file(out_filename, FILETYPE_ASSEMBLY, tokenized_source, tokenized_source_length);
	if (err != 0) {
		printf("Error %d\n", err);
	}
	free(tokenized_source);
	free(source);

	return err;
}
