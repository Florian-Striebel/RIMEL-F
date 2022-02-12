























#include "test.h"
__FBSDID("$FreeBSD$");


DEFINE_TEST(test_option_B_upper)
{
struct stat st;
int r;




assertMakeFile("file", 0644, NULL);


r = systemf("echo file | %s -o > small.cpio 2>small.err", testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "small.err");
assertEqualInt(0, stat("small.cpio", &st));
assertEqualInt(512, st.st_size);


r = systemf("echo file | %s -oB > large.cpio 2>large.err", testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "large.err");
assertEqualInt(0, stat("large.cpio", &st));
assertEqualInt(5120, st.st_size);
}
