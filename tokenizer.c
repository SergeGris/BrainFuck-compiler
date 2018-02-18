#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tokenizer.h"

Token parse_token(const char symbol)
{
	switch (symbol) {
	case '+':
	case '-':
		return T_INC;
	case '<':
	case '>':
		return T_POINTER_INC;
	case '[':
		return T_LABEL;
	case ']':
		return T_JUMP;
	case ',':
		return T_INPUT;
	case '.':
		return T_PRINT;
	default:
		return T_COMMENT;
	}
}

int parse_value(const char symbol)
{
	switch (symbol) {
	case '+':
	case '>':
		return 1;
	case '-':
	case '<':
		return -1;
	default:
		return 0;
		break;
	}
}

char* strip_comments(const char* const source)
{
	const unsigned int size = strlen(source);
	char* result = malloc((size + 1) * sizeof(char));
	if (result == NULL) {
		// Error: Out of memory
		return NULL;
	}
	int index = 0;
	for (int i = 0; i < size; i++) {
		if (parse_token(source[i]) != T_COMMENT) {
			result[index++] = source[i];
		}
	}
	result[index++] = 0;
	return result;
}

static int append_to_array(const Command cmd, Command** out_result, unsigned int* out_result_len)
{
	unsigned int new_size = *out_result_len + 1;
	Command *tmp = realloc(*out_result, new_size * sizeof(Command));
	if (tmp == NULL) {
		// Error: Out of memory
		return 1;
	}
	tmp[new_size - 1] = cmd;

	*out_result = tmp;
	*out_result_len = new_size;
	return 0;
}

int tokenize(const char* const source, Command** out_result, unsigned int* out_result_len)
{
	// Count [ and ] commands. Difference should be 0 at the end of the program, so
	// that all jumps have a matching label
	int opening_label_count = 0;
	int closing_label_count = 0;

	// Initialize final result
	*out_result_len = 0;
	*out_result = NULL;
	unsigned int result_len = 0;
	Command *result = NULL;

	// Strip comments from the source
	char* cleaned_source = strip_comments(source);
	if (cleaned_source == NULL) {
		// Error: Out of memory
		return 101;
	}

	// Command that is currently being constructed
	Command command = {T_COMMENT, 0};

	int errorcode = 0;
	for (int i = 0; i < strlen(cleaned_source); i++) {
		char c_current = cleaned_source[i];
		char c_next = cleaned_source[i + 1];

		Token current = parse_token(c_current);
		Token next = parse_token(c_next);

		command.token = current;

		// Set value for this command:
		// Data increment and pointer increment are added to previous symbol.
		// Labels and jumps need a number.
		// Read and print need nothing.
		if (current == T_INC) {
			command.value += parse_value(c_current);
		}
		else if (current == T_POINTER_INC) {
			const int value = parse_value(c_current);
			command.value += value;
		}
		else if (current == T_LABEL) {
			command.value = opening_label_count++;
		}
		else if (current == T_JUMP) {
			closing_label_count++;
			if (closing_label_count > opening_label_count) {
				// Error: Label mismatch
				errorcode = 102;
				break;
			}

			// Traverse final result backwards to find the correct label
			int correct_label = -1;
			int open_labels_to_skip = 0;
			for (int kk = result_len - 1; kk >= 0; kk--) {
				if (result[kk].token == T_JUMP) {
					open_labels_to_skip++;
				}
				else if (result[kk].token == T_LABEL) {
					if (open_labels_to_skip-- == 0) {
						correct_label = result[kk].value;
						break;
					}
				}
			}
			if (correct_label < 0) {
				// Error: Label mismatch
				errorcode = 102;
				break;
			}
			command.value = correct_label;
		}

		// Expecting new command: Push previous command to the final result and make a new one
		if (current != next
			|| (current != T_INC && current != T_POINTER_INC))
		{
			if (command.token != T_COMMENT) {
				int err = append_to_array(command, &result, &result_len);
				if (err) {
					// Error: Out of memory
					errorcode = 101;
					break;
				}
			}
			command.token = current;
			command.value = 0;
		}
	}
	if (errorcode == 0) {
		if (opening_label_count != closing_label_count) {
			// Error: Label mismatch
			errorcode = 102;
		}
	}
	if (errorcode != 0) {
		free(result);
		free(cleaned_source);
		return errorcode;
	}
	free(cleaned_source);

	// Copy allocated final result to the arguments
	*out_result = result;
	*out_result_len = result_len;

	return 0;
}

int tokenize_and_optimize(const char* const source, ProgramSource* out_result, const int level)
{
	out_result->tokens = NULL;
	out_result->length = 0;

	Command *tokenized_source;
	unsigned int tokenized_source_length = 0;
	int err = tokenize(source, &tokenized_source, &tokenized_source_length);
	if (err != 0) {
		return err;
	}
	bool no_print_commands = false, no_input_commands = false;

	// Level 0: Return code as is

	// Level 1:
	// Remove inactive loops (no +-, before the loop start)
	// Check if there are no print commands
	// Check if there are no input commands
	if (level >= 1) {
		// Remove inactive loops
		const int max_passes = 10;
		bool finished = false;
		for (int round = 0; round < max_passes && !finished; round++) {
			int inactive_loop_index = -1;
			for (unsigned int i = 0; i < tokenized_source_length; i++) {
				const Command current = tokenized_source[i];
				if (current.token == T_INC || current.token == T_INPUT) {
					// Not inactive
					finished = true;
					break;
				}
				else if (current.token == T_LABEL) {
					inactive_loop_index = current.value;
					break;
				}
			}
			bool inside_loop = false;
			for (unsigned int i = 0; i < tokenized_source_length; i++) {
				const Command current = tokenized_source[i];
				if (current.token == T_LABEL && current.value == inactive_loop_index) {
					inside_loop = true;
				}
				if (inside_loop) {
					tokenized_source[i].token = T_COMMENT;
					if (current.token == T_JUMP && current.value == inactive_loop_index) {
						break;
					}
				}
			}
		}

		// Find input and print commands
		bool found_input = false, found_print = false;
		for (unsigned int i = 0; i < tokenized_source_length; i++) {
			const Command current = tokenized_source[i];
			if (current.token == T_INPUT) {
				found_input = true;
			}
			else if (current.token == T_PRINT) {
				found_print = true;
			}
			if (found_input && found_print) {
				break;
			}
		}
		no_print_commands = !found_print;
		no_input_commands = !found_input;
	}
	// TODO: Level 2:
	// If no print or input commands, program has no effect
	// => Either remove everything or replace with [] if infinite loop
	if (level >= 2) {
		// Error: Not implemented
		err = 103;
	}

	if (err != 0) {
		free(tokenized_source);
		return err;
	}
	out_result->tokens = tokenized_source;
	out_result->length = tokenized_source_length;
	out_result->no_print_commands = no_print_commands;
	out_result->no_input_commands = no_input_commands;

	return 0;

}
