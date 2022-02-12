























#include "test.h"

DEFINE_TEST(test_error)
{
const char *reffile = "test_expand.error";

assertFileNotExists(reffile);
assert(0 != systemf("%s %s >test.out 2>test.err", testprog, reffile));

assertEmptyFile("test.out");
assertNonEmptyFile("test.err");
}
