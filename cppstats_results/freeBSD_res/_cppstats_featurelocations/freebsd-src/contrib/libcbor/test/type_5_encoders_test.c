






#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

#include "cbor.h"

unsigned char buffer[512];

static void test_embedded_map_start(void **state) {
assert_int_equal(1, cbor_encode_map_start(1, buffer, 512));
assert_memory_equal(buffer, ((unsigned char[]){0xA1}), 1);
}

static void test_map_start(void **state) {
assert_int_equal(5, cbor_encode_map_start(1000000, buffer, 512));
assert_memory_equal(buffer, ((unsigned char[]){0xBA, 0x00, 0x0F, 0x42, 0x40}),
5);
}

static void test_indef_map_start(void **state) {
assert_int_equal(1, cbor_encode_indef_map_start(buffer, 512));
assert_int_equal(0, cbor_encode_indef_map_start(buffer, 0));
assert_memory_equal(buffer, ((unsigned char[]){0xBF}), 1);
}

int main(void) {
const struct CMUnitTest tests[] = {cmocka_unit_test(test_embedded_map_start),
cmocka_unit_test(test_map_start),
cmocka_unit_test(test_indef_map_start)};
return cmocka_run_group_tests(tests, NULL, NULL);
}
