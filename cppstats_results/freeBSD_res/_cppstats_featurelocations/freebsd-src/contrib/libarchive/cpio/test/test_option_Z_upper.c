























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_Z_upper)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("echo f | %s -oZ >archive.out 2>archive.err",
testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (strstr(p, "compression not available") != NULL) {
skipping("This version of bsdcpio was compiled "
"without compress support");
free(p);
return;
}
failure("-Z option is broken");
assertEqualInt(r, 0);
free(p);
return;
}
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "\x1f\x9d", 2);
free(p);
}
