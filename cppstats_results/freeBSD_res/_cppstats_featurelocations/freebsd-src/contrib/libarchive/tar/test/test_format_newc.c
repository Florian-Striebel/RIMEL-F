























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_format_newc)
{

assertMakeFile("file1", 0644, "file1");
assertMakeFile("file2", 0644, "file2");
assertMakeHardlink("file3", "file1");


assertEqualInt(0,
systemf("%s -cf test1.cpio --format newc file1 file2 file3",
testprog));
assertMakeDir("test1", 0755);
assertChdir("test1");
assertEqualInt(0,
systemf("%s -xf ../test1.cpio >test.out 2>test.err", testprog));
assertFileContents("file1", 5, "file1");
assertFileContents("file2", 5, "file2");
assertFileContents("file1", 5, "file3");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertEqualInt(0,
systemf("%s -cf test2.cpio --format newc file1 file2",
testprog));
assertMakeDir("test2", 0755);
assertChdir("test2");
assertEqualInt(0,
systemf("%s -xf ../test2.cpio >test.out 2>test.err", testprog));
assertFileContents("file1", 5, "file1");
assertFileContents("file2", 5, "file2");
assertFileNotExists("file3");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");
}
