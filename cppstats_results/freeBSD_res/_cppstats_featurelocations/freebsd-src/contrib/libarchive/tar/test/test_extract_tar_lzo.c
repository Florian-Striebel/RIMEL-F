























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_extract_tar_lzo)
{
const char *reffile = "test_extract.tar.lzo";
int f;

extract_reference_file(reffile);
f = systemf("%s -tf %s >test.out 2>test.err", testprog, reffile);
if (f == 0 || canLzop()) {
assertEqualInt(0, systemf("%s -xf %s >test.out 2>test.err",
testprog, reffile));

assertFileExists("file1");
assertTextFileContents("contents of file1.\n", "file1");
assertFileExists("file2");
assertTextFileContents("contents of file2.\n", "file2");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
} else {
skipping("It seems lzop is not supported on this platform");
}
}
