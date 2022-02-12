























#include "test.h"
__FBSDID("$FreeBSD$");







DEFINE_TEST(test_symlink_dir)
{
assertUmask(0);

assertMakeDir("source", 0755);
assertMakeFile("source/file", 0755, "a");
assertMakeFile("source/file2", 0755, "ab");
assertMakeDir("source/dir", 0755);
assertMakeDir("source/dir/d", 0755);
assertMakeFile("source/dir/f", 0755, "abc");
assertMakeDir("source/dir2", 0755);
assertMakeDir("source/dir2/d2", 0755);
assertMakeFile("source/dir2/f2", 0755, "abcd");
assertMakeDir("source/dir3", 0755);
assertMakeDir("source/dir3/d3", 0755);
assertMakeFile("source/dir3/f3", 0755, "abcde");
assertMakeDir("source/dir4", 0755);
assertMakeFile("source/dir4/file3", 0755, "abcdef");
assertMakeHardlink("source/dir4/file4", "source/dir4/file3");

assertEqualInt(0,
systemf("%s -cf test.tar -C source dir dir2 dir3 file file2",
testprog));


assertEqualInt(0,
systemf("%s -cf test2.tar -C source dir4", testprog));




assertMakeDir("dest1", 0755);

assertMakeDir("dest1/real_dir", 0755);
if (canSymlink()) {
assertMakeSymlink("dest1/dir", "real_dir", 1);

assertMakeSymlink("dest1/dir2", "real_dir2", 1);
} else {
skipping("Symlinks are not supported on this platform");
}

assertMakeFile("dest1/non_dir3", 0755, "abcdef");
if (canSymlink())
assertMakeSymlink("dest1/dir3", "non_dir3", 1);

assertMakeFile("dest1/real_file", 0755, "abcdefg");
if (canSymlink()) {
assertMakeSymlink("dest1/file", "real_file", 0);

assertMakeSymlink("dest1/file2", "real_file2", 0);
}
assertEqualInt(0, systemf("%s -xf test.tar -C dest1", testprog));


failure("symlink to dir was followed when it shouldn't be");
assertIsDir("dest1/dir", -1);

failure("Broken symlink wasn't replaced with dir");
assertIsDir("dest1/dir2", -1);

failure("Symlink to non-dir wasn't replaced with dir");
assertIsDir("dest1/dir3", -1);

failure("Symlink to existing file should be replaced");
assertIsReg("dest1/file", -1);

failure("Symlink to non-existing file should be replaced");
assertIsReg("dest1/file2", -1);




assertMakeDir("dest2", 0755);

assertMakeDir("dest2/real_dir", 0755);
if (canSymlink())
assertMakeSymlink("dest2/dir", "real_dir", 1);

if (canSymlink())
assertMakeSymlink("dest2/dir2", "real_dir2", 1);

assertMakeFile("dest2/non_dir3", 0755, "abcdefgh");
if (canSymlink())
assertMakeSymlink("dest2/dir3", "non_dir3", 1);

assertMakeFile("dest2/real_file", 0755, "abcdefghi");
if (canSymlink())
assertMakeSymlink("dest2/file", "real_file", 0);

if (canSymlink())
assertMakeSymlink("dest2/file2", "real_file2", 0);
assertEqualInt(0, systemf("%s -xPf test.tar -C dest2", testprog));


if (canSymlink())
assertMakeSymlink("dest2/dir4", "real_dir", 1);
assertEqualInt(0, systemf("%s -xPf test2.tar -C dest2", testprog));


if (canSymlink()) {
assertIsSymlink("dest2/dir", "real_dir", 1);
assertIsSymlink("dest2/dir4", "real_dir", 1);
assertIsDir("dest2/real_dir", -1);
}


assertIsDir("dest2/dir/d", -1);
assertIsReg("dest2/dir/f", -1);
assertFileSize("dest2/dir/f", 3);

failure("Broken symlink wasn't replaced with dir");
assertIsDir("dest2/dir2", -1);

failure("Symlink to non-dir wasn't replaced with dir");
assertIsDir("dest2/dir3", -1);


failure("Symlink to existing file should be removed");
assertIsReg("dest2/file", -1);

failure("Symlink to non-existing file should be removed");
assertIsReg("dest2/file2", -1);


assertIsHardlink("dest2/dir4/file3", "dest2/dir4/file4");
}
