
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_a)
{
size_t s;
char *p;


assertMakeFile("f", 0644, "a");


assertEqualInt(0,
systemf("%s -acf test1.tar.Z f 2>test1.err", testprog));
assertEmptyFile("test1.err");

p = slurpfile(&s, "test1.tar.Z");
assert(s > 2);
failure("The archive should be compressed");
assertEqualMem(p, "\x1f\x9d", 2);
free(p);


assertEqualInt(0,
systemf("%s -acf test2.taZ f 2>test2.err", testprog));
assertEmptyFile("test2.err");

p = slurpfile(&s, "test2.taZ");
assert(s > 2);
failure("The archive should be compressed");
assertEqualMem(p, "\x1f\x9d", 2);
free(p);


assertEqualInt(0,
systemf("%s -acf test3.tar.Z.uu f 2>test3.err", testprog));
assertEmptyFile("test3.err");

p = slurpfile(&s, "test3.tar.Z.uu");
assert(s > 12);
failure("The archive should be uuencoded");
assertEqualMem(p, "begin 644 -\n", 12);
free(p);


assertEqualInt(0,
systemf("%s -acf test4.zip f 2>test4.err", testprog));
assertEmptyFile("test4.err");

p = slurpfile(&s, "test4.zip");
assert(s > 4);
failure("The archive should be zipped");
assertEqualMem(p, "\x50\x4b\x03\x04", 4);
free(p);


assertEqualInt(0,
systemf("%s -acf test5.tar.Z --uuencode f 2>test5.err",
testprog));
assertEmptyFile("test5.err");

p = slurpfile(&s, "test5.tar.Z");
assert(s > 2);
failure("The archive should be compressed, ignoring --uuencode option");
assertEqualMem(p, "\x1f\x9d", 2);
free(p);



assertEqualInt(0,
systemf("%s -acf test6.xxx --uuencode f 2>test6.err",
testprog));
assertEmptyFile("test6.err");

p = slurpfile(&s, "test6.xxx");
assert(s > 12);
failure("The archive should be uuencoded");
assertEqualMem(p, "begin 644 -\n", 12);
free(p);


assertEqualInt(0,
systemf("%s --auto-compress -cf test7.tar.Z f 2>test7.err",
testprog));
assertEmptyFile("test7.err");

p = slurpfile(&s, "test7.tar.Z");
assert(s > 2);
failure("The archive should be compressed");
assertEqualMem(p, "\x1f\x9d", 2);
free(p);
}
