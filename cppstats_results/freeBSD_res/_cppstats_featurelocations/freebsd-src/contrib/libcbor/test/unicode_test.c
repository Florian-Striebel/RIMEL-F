






#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

#include "../src/cbor/internal/unicode.h"

struct _cbor_unicode_status status;

unsigned char missing_bytes_data[] = {0xC4, 0x8C};


static void test_missing_bytes(void **state) {
_cbor_unicode_codepoint_count(missing_bytes_data, 1, &status);
assert_true(status.status == _CBOR_UNICODE_BADCP);
_cbor_unicode_codepoint_count(missing_bytes_data, 2, &status);
assert_true(status.status == _CBOR_UNICODE_OK);
}

unsigned char invalid_sequence_data[] = {0x65, 0xC4, 0x00};


static void test_invalid_sequence(void **state) {
_cbor_unicode_codepoint_count(invalid_sequence_data, 3, &status);
assert_true(status.status == _CBOR_UNICODE_BADCP);
assert_true(status.location == 2);
}

int main(void) {
const struct CMUnitTest tests[] = {cmocka_unit_test(test_missing_bytes),
cmocka_unit_test(test_invalid_sequence)};
return cmocka_run_group_tests(tests, NULL, NULL);
}
