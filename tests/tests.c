#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

void test_symbol_parsing(void **state);
void test_strip_comments(void **state);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_symbol_parsing),
        cmocka_unit_test(test_strip_comments)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
