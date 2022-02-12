
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_lzop)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("%s -cf - --lzop f >archive.out 2>archive.err", testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (!canLzop()) {
skipping("lzop is not supported on this platform");
goto done;
}
failure("--lzop option is broken");
assertEqualInt(r, 0);
goto done;
}
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "\x89\x4c\x5a\x4f\x00\x0d\x0a\x1a\x0a", 9);
done:
free(p);
}
