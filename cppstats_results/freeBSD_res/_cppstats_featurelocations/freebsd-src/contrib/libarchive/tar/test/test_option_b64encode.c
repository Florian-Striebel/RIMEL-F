
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_b64encode)
{
char *p;
size_t s;


assertMakeFile("f", 0644, "a");


assertEqualInt(0,
systemf("%s -cf - -Z --b64encode f >archive.out 2>archive.err",
testprog));

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "begin-base64 644", 16);
free(p);


assertEqualInt(0,
systemf("%s -cf - --b64encode f >archive.out 2>archive.err",
testprog));

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "begin-base64 644", 16);
free(p);
}
