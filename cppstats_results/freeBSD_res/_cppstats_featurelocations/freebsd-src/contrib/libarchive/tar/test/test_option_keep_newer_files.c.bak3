























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_keep_newer_files)
{
const char *reffile = "test_option_keep_newer_files.tar.Z";


extract_reference_file(reffile);


assertMakeDir("test1", 0755);
assertChdir("test1");
assertMakeFile("file", 0644, "new");
assertEqualInt(0,
systemf("%s -xf ../%s >test.out 2>test.err", testprog, reffile));
assertFileContents("old\n", 4, "file");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test2", 0755);
assertChdir("test2");
assertMakeFile("file", 0644, "new");
assertEqualInt(0,
systemf("%s -xf ../%s --keep-newer-files >test.out 2>test.err", testprog, reffile));
assertFileContents("new", 3, "file");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");
}
