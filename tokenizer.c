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
		return T_READ;
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

char* strip_comments(char* const source)
{
	const int size = strlen(source);
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

static int append_to_array(const Command cmd, Command** out_result, int* out_result_len)
{
	int new_size = *out_result_len + 1;
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

int tokenize(char* const source, Command** out_result, int* out_result_len)
{
	// Count [ and ] commands. Difference should be 0 at the end of the program, so
	// that all jumps have a matching label
	int opening_label_count = 0;
	int closing_label_count = 0;

	// Initialize final result
	*out_result_len = 0;
	*out_result = NULL;
	int result_len = 0;
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

	// Copy allocated final result to the arguments
	*out_result = result;
	*out_result_len = result_len;

	return 0;
}
