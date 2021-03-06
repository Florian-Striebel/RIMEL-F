























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_stdio)
{
FILE *filelist;
char *p;
size_t s;
int r;

assertUmask(0);





assertMakeFile("f", 0755, "abc");

assertMakeHardlink("l", "f");


filelist = fopen("filelist", "w");
assert(filelist != NULL);
fprintf(filelist, "f\n");
fprintf(filelist, "l\n");
fclose(filelist);







r = systemf("%s cf archive f l >cf.out 2>cf.err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("cf.out");
assertEmptyFile("cf.err");


r = systemf("%s cvf archive f l >cvf.out 2>cvf.err", testprog);
assertEqualInt(r, 0);
failure("'cv' writes filenames to stderr, nothing to stdout (SUSv2)\n"
"Note that GNU tar writes the file list to stdout by default.");
assertEmptyFile("cvf.out");



r = systemf("%s cvf - f l >cvf-.out 2>cvf-.err", testprog);
assertEqualInt(r, 0);
failure("cvf - should write archive to stdout");

failure("cvf - should write file list to stderr (SUSv2)");



r = systemf("%s tf archive >tf.out 2>tf.err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("tf.err");
failure("'t' mode should write results to stdout");



r = systemf("%s tvf archive >tvf.out 2>tvf.err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("tvf.err");
failure("'tv' mode should write results to stdout");



r = systemf("%s tvf - < archive >tvf-.out 2>tvf-.err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("tvf-.err");



r = systemf("%s xf archive >xf.out 2>xf.err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("xf.err");
assertEmptyFile("xf.out");


r = systemf("%s xvf archive >xvf.out 2>xvf.err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("xvf.out");



r = systemf("%s xvOf archive >xvOf.out 2>xvOf.err", testprog);
assertEqualInt(r, 0);

p = slurpfile(&s, "xvOf.out");
assertEqualInt((int)s, 3);
assertEqualMem(p, "abc", 3);

free(p);


r = systemf("%s xvf - < archive >xvf-.out 2>xvf-.err", testprog);
assertEqualInt(r, 0);
assertEmptyFile("xvf-.out");

}
