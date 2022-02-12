























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_l)
{
int r;


assertMakeFile("f", 0644, "a");


r = systemf("echo f | %s -pd copy >copy.out 2>copy.err",
testprog);
assertEqualInt(r, 0);


assertIsNotHardlink("f", "copy/f");


r = systemf("echo f | %s -pld link >link.out 2>link.err",
testprog);
assertEqualInt(r, 0);


assertIsHardlink("f", "link/f");
}
