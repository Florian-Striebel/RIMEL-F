
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_grzip)
{
char *p;
size_t s;

if (!canGrzip()) {
skipping("grzip is not supported on this platform");
return;
}


assertMakeFile("f", 0644, "a");


assertEqualInt(0,
systemf("%s -cf - --grzip f >archive.out 2>archive.err",
testprog));
p = slurpfile(&s, "archive.err");
p[s] = '\0';
free(p);


p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "GRZipII\x00\x02\x04:)", 12);
free(p);
}
