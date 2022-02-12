























#include "test.h"
__FBSDID("$FreeBSD$");


DEFINE_TEST(test_option_C_upper)
{
int r;




assertMakeFile("file", 0644, NULL);


r = systemf("echo file | %s -o > small.cpio 2>small.err", testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "small.err");
assertFileSize("small.cpio", 512);


r = systemf("echo file | %s -o -C 513 > 513.cpio 2>513.err",
testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "513.err");
assertFileSize("513.cpio", 513);


r = systemf("echo file | %s -o -C12345 > 12345.cpio 2>12345.err",
testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "12345.err");
assertFileSize("12345.cpio", 12345);


assert(0 != systemf("echo file | %s -o -C > bad.cpio 2>bad.err",
testprog));
assertEmptyFile("bad.cpio");
}
