























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_y)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("echo f | %s -oy >archive.out 2>archive.err",
testprog);
p = slurpfile(&s, "archive.err");
free(p);
if (r != 0) {
if (!canBzip2()) {
skipping("bzip2 is not supported on this platform");
return;
}
failure("-y option is broken");
assertEqualInt(r, 0);
return;
}
assertTextFileContents("1 block\n", "archive.err");

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "BZh9", 4);
free(p);
}
