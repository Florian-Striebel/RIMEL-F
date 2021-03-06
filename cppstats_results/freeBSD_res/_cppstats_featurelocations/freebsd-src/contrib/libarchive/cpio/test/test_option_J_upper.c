























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_J_upper)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("echo f | %s -o -J >archive.out 2>archive.err",
testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (strstr(p, "compression not available") != NULL) {
skipping("This version of bsdcpio was compiled "
"without xz support");
return;
}
failure("-J option is broken");
assertEqualInt(r, 0);
goto done;
}
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "\3757zXZ", 5);
done:
free(p);
}
