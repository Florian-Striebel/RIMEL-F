























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_nodump)
{

if (!canNodump()) {
skipping("Can't test nodump on this filesystem");
return;
}

assertMakeFile("file1", 0644, "file1");
assertMakeFile("file2", 0644, "file2");
assertMakeFile("file3", 0644, "file3");
assertSetNodump("file2");


assertEqualInt(0, systemf("%s -cf test1.tar file1 file2 file3",
testprog));
assertMakeDir("test1", 0755);
assertChdir("test1");
assertEqualInt(0,
systemf("%s -xf ../test1.tar >test.out 2>test.err", testprog));
assertFileContents("file1", 5, "file1");
assertFileContents("file2", 5, "file2");
assertFileContents("file3", 5, "file3");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertEqualInt(0, systemf("%s -cf test2.tar --nodump file1 file2 file3",
testprog));
assertMakeDir("test2", 0755);
assertChdir("test2");
assertEqualInt(0,
systemf("%s -xf ../test2.tar >test.out 2>test.err", testprog));
assertFileContents("file1", 5, "file1");
assertFileNotExists("file2");
assertFileContents("file3", 5, "file3");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");
}
