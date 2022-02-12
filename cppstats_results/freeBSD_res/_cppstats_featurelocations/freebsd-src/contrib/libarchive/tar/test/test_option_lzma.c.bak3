
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_lzma)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("%s -cf - --lzma f >archive.out 2>archive.err",
testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (strstr(p, "Unsupported compression") != NULL) {
skipping("This version of bsdtar was compiled "
"without lzma support");
return;
}
failure("--lzma option is broken");
assertEqualInt(r, 0);
goto done;
}
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "\x5d\00\00", 3);
done:
free(p);
}
