























#include "test.h"
__FBSDID("$FreeBSD$");






DEFINE_TEST(test_empty_mtree)
{
int r;

assertMakeFile("test1.mtree", 0777, "#mtree\n");

r = systemf("%s cf test1.tar @test1.mtree >test1.out 2>test1.err",
testprog);
failure("Error invoking %s cf", testprog);
assertEqualInt(r, 0);
assertEmptyFile("test1.out");
assertEmptyFile("test1.err");
}
