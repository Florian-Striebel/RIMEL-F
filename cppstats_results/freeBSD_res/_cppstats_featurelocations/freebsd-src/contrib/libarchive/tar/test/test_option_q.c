























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_q)
{
int r;











assertMakeFile("foo", 0644, "foo1");

assertEqualInt(0, systemf("%s -cf archive.tar foo", testprog));

assertMakeFile("foo", 0644, "foo2");

assertEqualInt(0, systemf("%s -rf archive.tar foo", testprog));

assertMakeFile("bar", 0644, "bar1");

assertEqualInt(0, systemf("%s -rf archive.tar bar", testprog));

assertMakeFile("foo", 0644, "foo3");

assertEqualInt(0, systemf("%s -rf archive.tar foo", testprog));

assertMakeFile("bar", 0644, "bar2");

assertEqualInt(0, systemf("%s -rf archive.tar bar", testprog));







assertMakeDir("test1", 0755);
assertChdir("test1");
r = systemf("%s -xf ../archive.tar -q foo >test.out 2>test.err",
testprog);
failure("Fatal error trying to use -q option");
if (!assertEqualInt(0, r))
return;

assertFileContents("foo1", 4, "foo");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test2", 0755);
assertChdir("test2");
assertEqualInt(0,
systemf("%s -xf ../archive.tar -q foo bar >test.out 2>test.err", testprog));
assertFileContents("foo2", 4, "foo");
assertFileContents("bar1", 4, "bar");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test3", 0755);
assertChdir("test3");
assertEqualInt(0,
systemf("%s -xf ../archive.tar --fast-read foo bar >test.out 2>test.err", testprog));
assertFileContents("foo2", 4, "foo");
assertFileContents("bar1", 4, "bar");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");


assertMakeDir("test4", 0755);
assertChdir("test4");
assertEqualInt(0,
systemf("%s -xf ../archive.tar foo bar >test.out 2>test.err", testprog));
assertFileContents("foo3", 4, "foo");
assertFileContents("bar2", 4, "bar");
assertEmptyFile("test.out");
assertEmptyFile("test.err");
assertChdir("..");
}
