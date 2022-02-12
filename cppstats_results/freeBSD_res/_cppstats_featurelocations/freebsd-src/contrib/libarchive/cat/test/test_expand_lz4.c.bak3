
























#include "test.h"

DEFINE_TEST(test_expand_lz4)
{
const char *reffile = "test_expand.lz4";
int f;

extract_reference_file(reffile);
f = systemf("%s %s >test.out 2>test.err", testprog, reffile);
if (f == 0 || canLz4()) {
assertEqualInt(0, f);
assertTextFileContents("contents of test_expand.lz4.\n", "test.out");
assertEmptyFile("test.err");
} else {
skipping("It seems lz4 is not supported on this platform");
}
}
