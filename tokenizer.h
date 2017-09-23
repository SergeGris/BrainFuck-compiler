#ifndef TOKENIZER_H
#define TOKENIZER_H

/*
	For optimized result, Brainfuck code is first converted to token-value pairs:
	increment, amount
	pointer increment, amount
	label, index
	jump to label, index
	read input
	print output
*/
typedef enum {
	T_COMMENT,
	T_INC,
	T_POINTER_INC,
	T_LABEL,
	T_JUMP,
	T_READ,
	T_PRINT
} Token;

typedef struct {
	Token token;
	int value;
} Command;

Token parse_token(const char symbol);
int parse_value(const char symbol);
char* strip_comments(char* const source);

int tokenize(char* const source, Command** out_result, unsigned int* out_result_len);

#endif
