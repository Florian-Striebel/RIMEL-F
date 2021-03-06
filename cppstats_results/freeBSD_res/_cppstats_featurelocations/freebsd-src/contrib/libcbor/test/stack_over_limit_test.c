#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

#include "assertions.h"
#include "cbor.h"
#include "stream_expectations.h"

static size_t generate_overflow_data(unsigned char **overflow_data) {
int i;
*overflow_data = (unsigned char *)malloc(CBOR_MAX_STACK_SIZE + 3);
for (i = 0; i < CBOR_MAX_STACK_SIZE + 1; i++) {
(*overflow_data)[i] = 0xC2;
}
(*overflow_data)[CBOR_MAX_STACK_SIZE + 1] = 0x41;
(*overflow_data)[CBOR_MAX_STACK_SIZE + 2] = 0x01;
return CBOR_MAX_STACK_SIZE + 3;
}

static void test_stack_over_limit(void **state) {
unsigned char *overflow_data;
size_t overflow_data_len;
struct cbor_load_result res;
overflow_data_len = generate_overflow_data(&overflow_data);
cbor_load(overflow_data, overflow_data_len, &res);
free(overflow_data);
assert_int_equal(res.error.code, CBOR_ERR_MEMERROR);
}

int main() {
const struct CMUnitTest tests[] = {cmocka_unit_test(test_stack_over_limit)};
return cmocka_run_group_tests(tests, NULL, NULL);
}
