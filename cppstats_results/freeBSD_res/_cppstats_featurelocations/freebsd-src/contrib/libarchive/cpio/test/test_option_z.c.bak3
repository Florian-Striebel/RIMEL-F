























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_z)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("echo f | %s -oz >archive.out 2>archive.err",
testprog);
p = slurpfile(&s, "archive.err");
free(p);
if (r != 0) {
if (!canGzip()) {
skipping("gzip is not supported on this platform");
return;
}
failure("-z option is broken");
assertEqualInt(r, 0);
return;
}

p = slurpfile(&s, "archive.out");
assert(s > 4);
assertEqualMem(p, "\x1f\x8b\x08\x00", 4);
free(p);
}
