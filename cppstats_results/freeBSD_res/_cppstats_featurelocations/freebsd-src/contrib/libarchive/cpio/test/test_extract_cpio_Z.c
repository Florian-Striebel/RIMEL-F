























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_extract_cpio_Z)
{
const char *reffile = "test_extract.cpio.Z";

extract_reference_file(reffile);
assertEqualInt(0, systemf("%s -i < %s >test.out 2>test.err",
testprog, reffile));

assertFileExists("file1");
assertTextFileContents("contents of file1.\n", "file1");
assertFileExists("file2");
assertTextFileContents("contents of file2.\n", "file2");
assertEmptyFile("test.out");
assertTextFileContents("1 block\n", "test.err");
}
