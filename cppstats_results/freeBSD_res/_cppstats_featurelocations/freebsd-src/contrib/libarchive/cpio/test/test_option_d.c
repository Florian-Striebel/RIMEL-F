























#include "test.h"
__FBSDID("$FreeBSD$");


DEFINE_TEST(test_option_d)
{
int r;




assertMakeDir("dir", 0755);
assertMakeFile("dir/file", 0644, NULL);


r = systemf("echo dir/file | %s -o > archive.cpio 2>archive.err", testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "archive.err");
assertFileSize("archive.cpio", 512);


assertMakeDir("without-d", 0755);
assertChdir("without-d");
r = systemf("%s -i < ../archive.cpio >out 2>err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("out");

assertFileNotExists("dir/file");


assertChdir("..");
assertMakeDir("with-d", 0755);
assertChdir("with-d");
r = systemf("%s -id < ../archive.cpio >out 2>err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("out");
assertTextFileContents("1 block\n", "err");

assertFileExists("dir/file");
}
