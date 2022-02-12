
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_lrzip)
{
char *p;
size_t s;

if (!canLrzip()) {
skipping("lrzip is not supported on this platform");
return;
}


assertMakeFile("f", 0644, "a");


assertEqualInt(0,
systemf("%s -cf - --lrzip f >archive.out 2>archive.err",
testprog));
p = slurpfile(&s, "archive.err");
p[s] = '\0';
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "LRZI\x00", 5);
free(p);
}
