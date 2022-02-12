























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_extract_cpio_zstd)
{
const char *reffile = "test_extract.cpio.zst";
int f;

extract_reference_file(reffile);
f = systemf("%s -it < %s >test.out 2>test.err", testprog, reffile);
if (f == 0 || canZstd()) {
assertEqualInt(0, systemf("%s -i < %s >test.out 2>test.err",
testprog, reffile));

assertFileExists("file1");
assertTextFileContents("contents of file1.\n", "file1");
assertFileExists("file2");
assertTextFileContents("contents of file2.\n", "file2");
assertEmptyFile("test.out");
assertTextFileContents("1 block\n", "test.err");
} else {
skipping("It seems zstd is not supported on this platform");
}
}
