























#include "test.h"
__FBSDID("$FreeBSD$");

static const char *
make_files(void)
{
FILE *f;


f = fopen("file", "wb");
assert(f != NULL);
assertEqualInt(10, fwrite("123456789", 1, 10, f));
fclose(f);


assertMakeHardlink("linkfile", "file");
assertIsHardlink("file", "linkfile");


if (canSymlink())
assertMakeSymlink("symlink", "file", 0);


assertMakeDir("dir", 0775);

return canSymlink()
? "file linkfile symlink dir"
: "file linkfile dir";
}

static void
verify_files(const char *target)
{
assertChdir(target);


failure("%s", target);
assertIsReg("file", -1);
failure("%s", target);
assertFileSize("file", 10);
failure("%s", target);
assertFileContents("123456789", 10, "file");
failure("%s", target);
assertFileNLinks("file", 2);


failure("%s", target);
assertIsReg("linkfile", -1);
failure("%s", target);
assertFileSize("linkfile", 10);
assertFileContents("123456789", 10, "linkfile");
assertFileNLinks("linkfile", 2);
assertIsHardlink("file", "linkfile");


if (canSymlink())
assertIsSymlink("symlink", "file", 0);


failure("%s", target);
assertIsDir("dir", 0775);
assertChdir("..");
}

static void
run_tar(const char *target, const char *pack_options,
const char *unpack_options, const char *flist)
{
int r;

assertMakeDir(target, 0775);


r = systemf("%s cf - %s %s >%s/archive 2>%s/pack.err", testprog, pack_options, flist, target, target);
failure("Error invoking %s cf -%s", testprog, pack_options);
assertEqualInt(r, 0);

assertChdir(target);


assertEmptyFile("pack.err");




r = systemf("%s xf archive %s >unpack.out 2>unpack.err",
testprog, unpack_options);
failure("Error invoking %s xf archive %s", testprog, unpack_options);
assertEqualInt(r, 0);


assertEmptyFile("unpack.err");
assertChdir("..");
}

DEFINE_TEST(test_basic)
{
const char *flist;

assertUmask(0);
flist = make_files();

run_tar("copy", "", "", flist);
verify_files("copy");

run_tar("copy_ustar", "--format=ustar", "", flist);
verify_files("copy_ustar");



}
