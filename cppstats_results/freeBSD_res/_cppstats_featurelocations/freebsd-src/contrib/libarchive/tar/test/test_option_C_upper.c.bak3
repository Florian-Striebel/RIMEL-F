























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_C_upper)
{
int r;

assertMakeDir("d1", 0755);
assertMakeDir("d2", 0755);
assertMakeFile("d1/file1", 0644, "d1/file1");
assertMakeFile("d1/file2", 0644, "d1/file2");
assertMakeFile("d2/file1", 0644, "d2/file1");
assertMakeFile("d2/file2", 0644, "d2/file2");




assertMakeDir("test1", 0755);
assertChdir("test1");
assertEqualInt(0, systemf("%s -cf archive.tar -C ../d1 file1 -C ../d2 file2", testprog));
assertEqualInt(0,
systemf("%s -xf archive.tar >test.out 2>test.err", testprog));
assertFileContents("d1/file1", 8, "file1");
assertFileContents("d2/file2", 8, "file2");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");





assertMakeDir("test2", 0755);
assertChdir("test2");
assertEqualInt(0, systemf("%s -cf archive.tar -C .. -C d1 file1 -C .. -C d2 file2", testprog));
assertEqualInt(0,
systemf("%s -xf archive.tar >test.out 2>test.err", testprog));
assertFileContents("d1/file1", 8, "file1");
assertFileContents("d2/file2", 8, "file2");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");




assertMakeDir("test3", 0755);
assertChdir("test3");
r = systemf("%s -cf archive.tar -C ../XXX file1 -C ../d2 file2 2>write.err", testprog);
assert(r != 0);
assertNonEmptyFile("write.err");
assertEqualInt(0,
systemf("%s -xf archive.tar >test.out 2>test.err", testprog));
assertFileNotExists("file1");
assertFileNotExists("file2");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");




assertMakeDir("test4", 0755);
assertChdir("test4");
assertEqualInt(0,
systemf("%s -cf archive.tar -C %s/d1 file1",
testprog, testworkdir));
assertEqualInt(0,
systemf("%s -xf archive.tar >test.out 2>test.err", testprog));
assertFileContents("d1/file1", 8, "file1");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");




assertMakeDir("test5", 0755);
assertChdir("test5");
assertEqualInt(0,
systemf("%s -cf archive.tar -C XXX -C %s/d1 file1",
testprog, testworkdir));
assertEqualInt(0,
systemf("%s -xf archive.tar >test.out 2>test.err", testprog));
assertFileContents("d1/file1", 8, "file1");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");




assertMakeDir("test6", 0755);
assertChdir("test6");
r = systemf("%s -cf archive.tar -C XXX -C ../d1 file1 2>write.err",
testprog);
assert(r != 0);
assertNonEmptyFile("write.err");
assertEqualInt(0,
systemf("%s -xf archive.tar >test.out 2>test.err", testprog));
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");




assertMakeDir("test7", 0755);
assertChdir("test7");
r = systemf("%s -cf archive.tar ../d1/file1 -C 2>write.err", testprog);
assert(r != 0);
assertNonEmptyFile("write.err");
assertChdir("..");




assertMakeDir("test8", 0755);
assertChdir("test8");
r = systemf("%s -cf archive.tar ../d1/file1 -C \"\" 2>write.err",
testprog);
assert(r != 0);
assertNonEmptyFile("write.err");
assertChdir("..");
}
