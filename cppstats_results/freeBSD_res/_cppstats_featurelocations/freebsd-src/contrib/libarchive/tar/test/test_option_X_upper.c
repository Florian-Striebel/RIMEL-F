























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_X_upper)
{
int r;




assertMakeFile("file1", 0644, "file1");
assertMakeFile("file2", 0644, "file2");
assertMakeFile("file3a", 0644, "file3a");
assertMakeFile("file4a", 0644, "file4a");
assertEqualInt(0,
systemf("%s -cf archive.tar file1 file2 file3a file4a", testprog));






assertMakeDir("test1", 0755);
assertChdir("test1");
r = systemf("%s -xf ../archive.tar >test.out 2>test.err",
testprog);
if (!assertEqualInt(0, r))
return;

assertFileContents("file1", 5, "file1");
assertFileContents("file2", 5, "file2");
assertFileContents("file3a", 6, "file3a");
assertFileContents("file4a", 6, "file4a");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test2", 0755);
assertChdir("test2");
assertMakeFile("exclusions", 0644, "file1\n");
assertEqualInt(0,
systemf("%s -xf ../archive.tar -X exclusions >test.out 2>test.err", testprog));
assertFileNotExists("file1");
assertFileContents("file2", 5, "file2");
assertFileContents("file3a", 6, "file3a");
assertFileContents("file4a", 6, "file4a");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test3", 0755);
assertChdir("test3");
assertMakeFile("exclusions", 0644, "file1\nfile2\n");
assertEqualInt(0,
systemf("%s -xf ../archive.tar -X exclusions >test.out 2>test.err", testprog));
assertFileNotExists("file1");
assertFileNotExists("file2");
assertFileContents("file3a", 6, "file3a");
assertFileContents("file4a", 6, "file4a");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test4", 0755);
assertChdir("test4");
assertMakeFile("exclusions", 0644, "file1\nfile2");
assertEqualInt(0,
systemf("%s -xf ../archive.tar -X exclusions >test.out 2>test.err", testprog));
assertFileNotExists("file1");
assertFileNotExists("file2");
assertFileContents("file3a", 6, "file3a");
assertFileContents("file4a", 6, "file4a");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test5", 0755);
assertChdir("test5");
assertMakeFile("exclusions", 0644, "file1\nfile2");
assertEqualInt(0,
systemf("%s -xf ../archive.tar -X exclusions file3a >test.out 2>test.err", testprog));
assertFileNotExists("file1");
assertFileNotExists("file2");
assertFileContents("file3a", 6, "file3a");
assertFileNotExists("file4a");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test6", 0755);
assertChdir("test6");
assertMakeFile("exclusions", 0644, "file1\nfile2");
assertEqualInt(0,
systemf("%s -xf ../archive.tar -X exclusions file1 file3a >test.out 2>test.err", testprog));
assertFileNotExists("file1");
assertFileNotExists("file2");
assertFileContents("file3a", 6, "file3a");
assertFileNotExists("file4a");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test7", 0755);
assertChdir("test7");
assertMakeFile("exclusions", 0644, "file*a\nfile1");
assertEqualInt(0,
systemf("%s -xf ../archive.tar -X exclusions >test.out 2>test.err", testprog));
assertFileNotExists("file1");
assertFileContents("file2", 5, "file2");
assertFileNotExists("file3a");
assertFileNotExists("file4a");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test8", 0755);
assertChdir("test8");
assertMakeFile("exclusions", 0644, "");
assertEqualInt(0,
systemf("%s -xf ../archive.tar -X exclusions >test.out 2>test.err", testprog));
assertFileContents("file1", 5, "file1");
assertFileContents("file2", 5, "file2");
assertFileContents("file3a", 6, "file3a");
assertFileContents("file4a", 6, "file4a");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");
}
