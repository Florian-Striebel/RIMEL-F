
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_patterns)
{
FILE *f;
int r;
const char *reffile2 = "test_patterns_2.tar";
const char *reffile3 = "test_patterns_3.tar";
const char *reffile4 = "test_patterns_4.tar";

const char *tar2aExpected[] = {
"/tmp/foo/bar/",
"/tmp/foo/bar/baz",
NULL
};











f = fopen("foo", "w");
assert(f != NULL);
fclose(f);
r = systemf("%s cfv tar1.tgz foo > tar1a.out 2> tar1a.err", testprog);
assertEqualInt(r, 0);
r = systemf("%s xv --no-same-owner -f tar1.tgz foo bar > tar1b.out 2> tar1b.err", testprog);
failure("tar should return non-zero because a file was given on the command line that's not in the archive");
assert(r != 0);




extract_reference_file(reffile2);

r = systemf("%s tf %s /tmp/foo/bar > tar2a.out 2> tar2a.err",
testprog, reffile2);
assertEqualInt(r, 0);
assertFileContainsLinesAnyOrder("tar2a.out", tar2aExpected);
assertEmptyFile("tar2a.err");




extract_reference_file(reffile3);


r = systemf("%s x --no-same-owner -f %s tmp/foo/bar > tar3a.out 2> tar3a.err",
testprog, reffile3);
assert(r != 0);
assertEmptyFile("tar3a.out");


assertNonEmptyFile("tar3a.err");

r = systemf("%s x --no-same-owner -f %s /tmp/foo/baz > tar3b.out 2> tar3b.err",
testprog, reffile3);
assert(r != 0);
assertEmptyFile("tar3b.out");
assertNonEmptyFile("tar3b.err");


r = systemf("%s x --no-same-owner -f %s ./tmp/foo/bar > tar3c.out 2> tar3c.err",
testprog, reffile3);
assert(r != 0);
assertEmptyFile("tar3c.out");
assertNonEmptyFile("tar3c.err");


r = systemf("%s x --no-same-owner -f %s ./tmp/foo/baz > tar3d.out 2> tar3d.err",
testprog, reffile3);
assertEqualInt(r, 0);
assertEmptyFile("tar3d.out");
assertEmptyFile("tar3d.err");
assertFileExists("tmp/foo/baz/bar");





extract_reference_file(reffile4);

r = systemf("%s x --no-same-owner -f %s -C tmp > tar4.out 2> tar4.err",
testprog, reffile4);
assert(r != 0);
assertEmptyFile("tar4.out");
assertNonEmptyFile("tar4.err");

for (r = 1; r <= 54; r++) {
char file_a[] = "tmp/fileXX";
char file_b1[] = "tmp/server/share/fileXX";
char file_b2[] = "tmp/server\\share\\fileXX";
char file_c[] = "tmp/../fileXX";
char file_d[] = "tmp/../../fileXX";
char *filex;
int xsize;

switch (r) {
case 15: case 18:





filex = file_b1;
xsize = sizeof(file_b1);
break;
case 35: case 38: case 52:






filex = file_b2;
xsize = sizeof(file_b2);
break;
default:
filex = file_a;
xsize = sizeof(file_a);
break;
}
filex[xsize-3] = '0' + r / 10;
filex[xsize-2] = '0' + r % 10;
switch (r) {
case 5: case 6: case 17: case 20: case 25:
case 26: case 37: case 40: case 43: case 54:













assertFileNotExists(filex);
if (r == 6 || r == 26 || r == 43) {
filex = file_d;
xsize = sizeof(file_d);
} else {
filex = file_c;
xsize = sizeof(file_c);
}
filex[xsize-3] = '0' + r / 10;
filex[xsize-2] = '0' + r % 10;
assertFileNotExists(filex);
break;
default:

assertFileExists(filex);
break;
}
}
}
