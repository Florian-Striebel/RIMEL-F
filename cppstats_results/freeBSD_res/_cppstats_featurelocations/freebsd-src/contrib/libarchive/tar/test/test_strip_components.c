























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_strip_components)
{
assertMakeDir("d0", 0755);
assertChdir("d0");
assertMakeDir("d1", 0755);
assertMakeDir("d1/d2", 0755);
assertMakeDir("d1/d2/d3", 0755);
assertMakeFile("d1/d2/f1", 0644, "");
assertMakeHardlink("l1", "d1/d2/f1");
assertMakeHardlink("d1/l2", "d1/d2/f1");
if (canSymlink()) {
assertMakeSymlink("s1", "d1/d2/f1", 0);
assertMakeSymlink("d1/s2", "d2/f1", 0);
}
assertChdir("..");




if (canSymlink())
assertEqualInt(0, systemf("%s -cf test.tar d0/l1 d0/s1 d0/d1",
testprog));
else
assertEqualInt(0, systemf("%s -cf test.tar d0/l1 d0/d1",
testprog));

assertMakeDir("target", 0755);
assertEqualInt(0, systemf("%s -x -C target --strip-components 2 "
"-f test.tar", testprog));

failure("d0/ is too short and should not get restored");
assertFileNotExists("target/d0");
failure("d0/d1/ is too short and should not get restored");
assertFileNotExists("target/d1");
failure("d0/s1 is too short and should not get restored");
assertFileNotExists("target/s1");
failure("d0/d1/s2 is a symlink to something that won't be extracted");


if (canSymlink())
assertIsSymlink("target/s2", "d2/f1", 0);
else
assertFileNotExists("target/s2");
failure("d0/d1/d2 should be extracted");
assertIsDir("target/d2", -1);
























failure("d0/l1 is too short and should not get restored");
assertFileNotExists("target/l1");
failure("d0/d1/l2 is a hardlink to file whose name was too short");
assertFileNotExists("target/l2");
failure("d0/d1/d2/f1 is a hardlink to file whose name was too short");
assertFileNotExists("target/d2/f1");




if (canSymlink())
assertEqualInt(0, systemf("%s --strip-components 2 -cf test2.tar "
"d0/l1 d0/s1 d0/d1", testprog));
else
assertEqualInt(0, systemf("%s --strip-components 2 -cf test2.tar "
"d0/l1 d0/d1", testprog));

assertMakeDir("target2", 0755);
assertEqualInt(0, systemf("%s -x -C target2 -f test2.tar", testprog));

failure("d0/ is too short and should not have been archived");
assertFileNotExists("target2/d0");
failure("d0/d1/ is too short and should not have been archived");
assertFileNotExists("target2/d1");
failure("d0/s1 is too short and should not get restored");
assertFileNotExists("target/s1");

if (canSymlink()) {
failure("d0/d1/s2 is a symlink to something included in archive");
assertIsSymlink("target2/s2", "d2/f1", 0);
}
failure("d0/d1/d2 should be archived");
assertIsDir("target2/d2", -1);




failure("d0/l1 is too short and should not have been archived");
assertFileNotExists("target/l1");
failure("d0/d1/l2 is a hardlink to file whose name was too short");
assertFileNotExists("target/l2");
failure("d0/d1/d2/f1 is a hardlink to file whose name was too short");
assertFileNotExists("target/d2/f1");
}
