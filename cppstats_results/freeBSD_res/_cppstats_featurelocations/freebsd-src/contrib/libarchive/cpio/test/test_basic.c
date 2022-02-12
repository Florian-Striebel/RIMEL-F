























#include "test.h"
__FBSDID("$FreeBSD$");

static void
verify_files(const char *msg)
{





failure("%s", msg);
assertIsReg("file", 0644);
failure("%s", msg);
assertFileSize("file", 10);
failure("%s", msg);
assertFileNLinks("file", 2);


failure("%s", msg);
assertIsHardlink("linkfile", "file");


if (canSymlink())
assertIsSymlink("symlink", "file", 0);


failure("%s", msg);
assertIsReg("file2", 0777);
failure("%s", msg);
assertFileSize("file2", 10);
failure("%s", msg);
assertFileNLinks("file2", 1);


assertIsDir("dir", 0775);
}

static void
basic_cpio(const char *target,
const char *pack_options,
const char *unpack_options,
const char *se, const char *se2)
{
int r;

if (!assertMakeDir(target, 0775))
return;


r = systemf("%s -R 1000:1000 -o %s < filelist >%s/archive 2>%s/pack.err",
testprog, pack_options, target, target);
failure("Error invoking %s -o %s", testprog, pack_options);
assertEqualInt(r, 0);

assertChdir(target);


failure("Expected: %s, options=%s", se, pack_options);
assertTextFileContents(se, "pack.err");




r = systemf("%s -i %s< archive >unpack.out 2>unpack.err",
testprog, unpack_options);
failure("Error invoking %s -i %s", testprog, unpack_options);
assertEqualInt(r, 0);


failure("Error invoking %s -i %s in dir %s", testprog, unpack_options, target);
assertTextFileContents(se2, "unpack.err");

verify_files(pack_options);

assertChdir("..");
}

static void
passthrough(const char *target)
{
int r;

if (!assertMakeDir(target, 0775))
return;




r = systemf("%s -p %s <filelist >%s/stdout 2>%s/stderr",
testprog, target, target, target);
failure("Error invoking %s -p", testprog);
assertEqualInt(r, 0);

assertChdir(target);


failure("Error invoking %s -p in dir %s",
testprog, target);
assertTextFileContents("1 block\n", "stderr");

verify_files("passthrough");
assertChdir("..");
}

DEFINE_TEST(test_basic)
{
FILE *filelist;
const char *msg;
char result[1024];

assertUmask(0);




filelist = fopen("filelist", "w");
memset(result, 0, sizeof(result));


assertMakeFile("file", 0644, "1234567890");
fprintf(filelist, "file\n");
if (is_LargeInode("file")) {
strncat(result,
"bsdcpio: file: large inode number truncated: ",
sizeof(result) - strlen(result) -1);
strncat(result,
strerror(ERANGE),
sizeof(result) - strlen(result) -1);
strncat(result,
"\n",
sizeof(result) - strlen(result) -1);
}


assertMakeHardlink("linkfile", "file");
fprintf(filelist, "linkfile\n");
if (is_LargeInode("linkfile")) {
strncat(result,
"bsdcpio: linkfile: large inode number truncated: ",
sizeof(result) - strlen(result) -1);
strncat(result,
strerror(ERANGE),
sizeof(result) - strlen(result) -1);
strncat(result,
"\n",
sizeof(result) - strlen(result) -1);
}


if (canSymlink()) {
assertMakeSymlink("symlink", "file", 0);
fprintf(filelist, "symlink\n");
if (is_LargeInode("symlink")) {
strncat(result,
"bsdcpio: symlink: large inode number truncated: ",
sizeof(result) - strlen(result) -1);
strncat(result,
strerror(ERANGE),
sizeof(result) - strlen(result) -1);
strncat(result,
"\n",
sizeof(result) - strlen(result) -1);
}
}


assertMakeFile("file2", 0777, "1234567890");
fprintf(filelist, "file2\n");
if (is_LargeInode("file2")) {
strncat(result,
"bsdcpio: file2: large inode number truncated: ",
sizeof(result) - strlen(result) -1);
strncat(result,
strerror(ERANGE),
sizeof(result) - strlen(result) -1);
strncat(result,
"\n",
sizeof(result) - strlen(result) -1);
}


assertMakeDir("dir", 0775);
fprintf(filelist, "dir\n");
if (is_LargeInode("dir")) {
strncat(result,
"bsdcpio: dir: large inode number truncated: ",
sizeof(result) - strlen(result) -1);
strncat(result,
strerror(ERANGE),
sizeof(result) - strlen(result) -1);
strncat(result,
"\n",
sizeof(result) - strlen(result) -1);
}
strncat(result, "2 blocks\n", sizeof(result) - strlen(result) -1);


fclose(filelist);

assertUmask(022);


msg = canSymlink() ? "2 blocks\n" : "1 block\n";
basic_cpio("copy", "", "", msg, msg);
basic_cpio("copy_odc", "--format=odc", "", msg, msg);
basic_cpio("copy_newc", "-H newc", "", result, "2 blocks\n");
basic_cpio("copy_cpio", "-H odc", "", msg, msg);
msg = "1 block\n";
basic_cpio("copy_bin", "-H bin", "", msg, msg);
msg = canSymlink() ? "9 blocks\n" : "8 blocks\n";
basic_cpio("copy_ustar", "-H ustar", "", msg, msg);


passthrough("passthrough");
}
