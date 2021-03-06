























#include "test.h"
__FBSDID("$FreeBSD$");

static void
unpack_test(const char *from, const char *options, const char *se)
{
int r;


assertMakeDir(from, 0775);
assertChdir(from);




extract_reference_file(from);
r = systemf("%s -i %s < %s >unpack.out 2>unpack.err",
testprog, options, from);
failure("Error invoking %s -i %s < %s",
testprog, options, from);
assertEqualInt(r, 0);


if (canSymlink()) {
failure("Error invoking %s -i %s < %s",
testprog, options, from);
assertTextFileContents(se, "unpack.err");
}






assertIsReg("file", 0644);
failure("%s", from);
assertFileSize("file", 10);
assertFileSize("linkfile", 10);
failure("%s", from);
assertFileNLinks("file", 2);


failure("%s", from);
assertIsHardlink("linkfile", "file");
assertFileSize("file", 10);
assertFileSize("linkfile", 10);


if (canSymlink())
assertIsSymlink("symlink", "file", 0);


assertIsDir("dir", 0775);

assertChdir("..");
}

DEFINE_TEST(test_gcpio_compat)
{
assertUmask(0);


if (canSymlink()) {
unpack_test("test_gcpio_compat_ref.bin",
"--no-preserve-owner", "1 block\n");
unpack_test("test_gcpio_compat_ref.crc",
"--no-preserve-owner", "2 blocks\n");
unpack_test("test_gcpio_compat_ref.newc",
"--no-preserve-owner", "2 blocks\n");

unpack_test("test_gcpio_compat_ref.ustar",
"--no-preserve-owner", "7 blocks\n");
} else {
unpack_test("test_gcpio_compat_ref_nosym.bin",
"--no-preserve-owner", "1 block\n");
unpack_test("test_gcpio_compat_ref_nosym.crc",
"--no-preserve-owner", "2 blocks\n");
unpack_test("test_gcpio_compat_ref_nosym.newc",
"--no-preserve-owner", "2 blocks\n");

unpack_test("test_gcpio_compat_ref_nosym.ustar",
"--no-preserve-owner", "7 blocks\n");
}
}
