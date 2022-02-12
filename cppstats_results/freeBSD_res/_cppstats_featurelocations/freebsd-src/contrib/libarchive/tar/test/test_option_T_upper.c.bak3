























#include "test.h"
__FBSDID("$FreeBSD$");

static int
tryMakeFile(const char *fn)
{
FILE *f = fopen(fn, "w");
if (f == NULL)
return (0);
fclose(f);
return (1);
}

DEFINE_TEST(test_option_T_upper)
{
FILE *f;
int r;
int gnarlyFilesSupported;


if (!assertMakeDir("d1", 0755)) return;
if (!assertMakeDir("d1/d2", 0755)) return;
if (!assertMakeFile("f", 0644, "")) return;
if (!assertMakeFile("d1/f1", 0644, "")) return;
if (!assertMakeFile("d1/f2", 0644, "")) return;
if (!assertMakeFile("d1/d2/f3", 0644, "")) return;
if (!assertMakeFile("d1/d2/f4", 0644, "")) return;
if (!assertMakeFile("d1/d2/f5", 0644, "")) return;
if (!assertMakeFile("d1/d2/f6", 0644, "")) return;

gnarlyFilesSupported = tryMakeFile("d1/d2/f\x0a");


f = fopen("filelist", "w+");
if (!assert(f != NULL))
return;

fprintf(f, "f\x0d");
fprintf(f, "d1/f1\x0d\x0a");
fprintf(f, "d1/d2/f4\x0a");
fprintf(f, "d1/d2/f6");
fclose(f);


f = fopen("filelist2", "w+");
if (!assert(f != NULL))
return;

fprintf(f, "d1/d2/f3");
assertEqualInt(1, fwrite("\0", 1, 1, f));
fprintf(f, "d1/d2/f5");
assertEqualInt(1, fwrite("\0", 1, 1, f));
if (gnarlyFilesSupported) {
fprintf(f, "d1/d2/f\x0a");
assertEqualInt(1, fwrite("\0", 1, 1, f));
}
fclose(f);


r = systemf("%s -c -f test1.tar -T filelist > test1.out 2> test1.err",
testprog);
assert(r == 0);
assertEmptyFile("test1.out");
assertEmptyFile("test1.err");


if (!assertMakeDir("test1", 0755)) return;
systemf("%s -x -f test1.tar -T filelist -C test1"
" > test1b.out 2> test1b.err", testprog);
assertEmptyFile("test1b.out");
assertEmptyFile("test1b.err");


assertFileExists("test1/f");
assertFileExists("test1/d1/f1");
assertFileNotExists("test1/d1/f2");
assertFileNotExists("test1/d1/d2/f3");
assertFileExists("test1/d1/d2/f4");
assertFileNotExists("test1/d1/d2/f5");
assertFileExists("test1/d1/d2/f6");
if (gnarlyFilesSupported) {
assertFileNotExists("test1/d1/d2/f\x0a");
}


systemf("%s -r -f test1.tar --null -T filelist2 > test2.out 2> test2.err",
testprog);
assertEmptyFile("test2.out");
assertEmptyFile("test2.err");


if (!assertMakeDir("test3", 0755)) return;
systemf("%s -x -f test1.tar -C test3"
" > test3.out 2> test3.err", testprog);
assertEmptyFile("test3.out");
assertEmptyFile("test3.err");

assertFileExists("test3/f");
assertFileExists("test3/d1/f1");
assertFileNotExists("test3/d1/f2");
assertFileExists("test3/d1/d2/f3");
assertFileExists("test3/d1/d2/f4");
assertFileExists("test3/d1/d2/f5");
assertFileExists("test3/d1/d2/f6");
if (gnarlyFilesSupported) {
assertFileExists("test3/d1/d2/f\x0a");
}


if (!assertMakeDir("test2", 0755)) return;
systemf("%s -x -f test1.tar -T filelist -C test2"
" > test2b.out 2> test2b.err", testprog);
assertEmptyFile("test2b.out");
assertEmptyFile("test2b.err");

assertFileExists("test2/f");
assertFileExists("test2/d1/f1");
assertFileNotExists("test2/d1/f2");
assertFileNotExists("test2/d1/d2/f3");
assertFileExists("test2/d1/d2/f4");
assertFileNotExists("test2/d1/d2/f5");
assertFileExists("test2/d1/d2/f6");
if (gnarlyFilesSupported) {
assertFileNotExists("test2/d1/d2/f\x0a");
}

assertMakeDir("test4", 0755);
assertMakeDir("test4_out", 0755);
assertMakeDir("test4_out2", 0755);
assertMakeDir("test4/d1", 0755);
assertMakeFile("test4/d1/foo", 0644, "");




}
