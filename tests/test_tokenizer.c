#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdlib.h>

#include "tokenizer.h"

void test_symbol_parsing(void **state)
{
	assert_true(parse_token('+') == T_INC);
	assert_true(parse_token('-') == T_INC);
	assert_true(parse_token('<') == T_POINTER_INC);
	assert_true(parse_token('>') == T_POINTER_INC);
	assert_true(parse_token('[') == T_LABEL);
	assert_true(parse_token(']') == T_JUMP);
	assert_true(parse_token(',') == T_INPUT);
	assert_true(parse_token('.') == T_PRINT);

	// Unknown symbols should be comments
	assert_true(parse_token('a') == T_COMMENT);
	assert_true(parse_token('b') == T_COMMENT);

	// Test only tokens that should have a value
	// For other tokens, return value does not matter
	assert_true(parse_value('+') == 1);
	assert_true(parse_value('-') == -1);
	assert_true(parse_value('>') == 1);
	assert_true(parse_value('<') == -1);
}

void test_strip_comments(void **state)
{
	// No comments
	{
		const char* input = "++++-[<.,>]";
		const char* correct = input;
		char* result = strip_comments(input);

		assert_true(strcmp(result, correct) == 0);
		free(result);
	}

	// Comments only
	{
		const char* input = "abffd87%g";
		const char* correct = "";
		char* result = strip_comments(input);

		assert_true(strcmp(result, correct) == 0);
		free(result);
	}

	// Both
	{
		const char* input = "dfd54#d+-[]dasd.>,[78]<";
		const char* correct = "+-[].>,[]<";
		char* result = strip_comments(input);

		assert_true(strcmp(result, correct) == 0);
		free(result);
	}
}
