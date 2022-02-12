
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_j)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("%s -jcf archive.out f 2>archive.err", testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (!canBzip2()) {
skipping("bzip2 is not supported on this platform");
goto done;
}
failure("-j option is broken");
assertEqualInt(r, 0);
goto done;
}
free(p);
assertEmptyFile("archive.err");

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "BZh9", 4);
done:
free(p);
}
