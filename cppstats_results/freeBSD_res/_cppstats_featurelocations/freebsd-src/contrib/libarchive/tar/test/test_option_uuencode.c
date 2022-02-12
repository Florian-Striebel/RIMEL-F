
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_uuencode)
{
char *p;
size_t s;


assertMakeFile("f", 0644, "a");


assertEqualInt(0,
systemf("%s -cf - -Z --uuencode f >archive.out 2>archive.err",
testprog));

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "begin 644", 9);
free(p);


assertEqualInt(0,
systemf("%s -cf - --uuencode f >archive.out 2>archive.err",
testprog));

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "begin 644", 9);
free(p);
}
