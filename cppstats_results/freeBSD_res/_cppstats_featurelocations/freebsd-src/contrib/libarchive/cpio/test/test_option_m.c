























#include "test.h"
__FBSDID("$FreeBSD$");


DEFINE_TEST(test_option_m)
{
int r;







assertMakeDir("without-m", 0755);
assertChdir("without-m");
extract_reference_file("test_option_m.cpio");
r = systemf("%s --no-preserve-owner -i < test_option_m.cpio >out 2>err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("out");
assertTextFileContents("1 block\n", "err");

assertFileMtimeRecent("file");


assertChdir("..");
assertMakeDir("with-m", 0755);
assertChdir("with-m");
extract_reference_file("test_option_m.cpio");
r = systemf("%s --no-preserve-owner -im < test_option_m.cpio >out 2>err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("out");
assertTextFileContents("1 block\n", "err");




assertFileMtime("file", 1, 0);
}
