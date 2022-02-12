























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_lzma)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("echo f | %s -o --lzma >archive.out 2>archive.err",
testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (strstr(p, "compression not available") != NULL) {
skipping("This version of bsdcpio was compiled "
"without lzma support");
free(p);
return;
}
failure("--lzma option is broken");
assertEqualInt(r, 0);
free(p);
return;
}
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "\x5d\00\00", 3);
free(p);
}
