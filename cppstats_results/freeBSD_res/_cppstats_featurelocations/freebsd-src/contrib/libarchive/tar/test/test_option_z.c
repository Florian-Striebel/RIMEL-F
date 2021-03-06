
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_z)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("%s -zcf archive.out f 2>archive.err", testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (!canGzip()) {
skipping("gzip is not supported on this platform");
goto done;
}
failure("-z option is broken");
assertEqualInt(r, 0);
goto done;
}
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 4);
assertEqualMem(p, "\x1f\x8b\x08\x00", 4);
done:
free(p);
}
