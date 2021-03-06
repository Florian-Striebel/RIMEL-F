























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_safe_writes)
{

assertMakeDir("in", 0755);
assertEqualInt(0, chdir("in"));
assertMakeFile("f", 0644, "a");
assertMakeFile("fh", 0644, "b");
assertMakeFile("d", 0644, "c");
assertMakeFile("fs", 0644, "d");
assertMakeFile("ds", 0644, "e");
assertEqualInt(0, chdir(".."));


assertEqualInt(0,
systemf("%s -c -C in -f t.tar f fh d fs ds "
">pack.out 2>pack.err", testprog));


assertEmptyFile("pack.err");
assertEmptyFile("pack.out");


assertMakeDir("out", 0755);
assertEqualInt(0, chdir("out"));
assertMakeFile("f", 0644, "a");
assertMakeHardlink("fh", "f");
assertMakeDir("d", 0755);
if (canSymlink()) {
assertMakeSymlink("fs", "f", 0);
assertMakeSymlink("ds", "d", 1);
}
assertEqualInt(0, chdir(".."));


assertEqualInt(0,
systemf("%s -x -C out --safe-writes -f t.tar "
">unpack.out 2>unpack.err", testprog));


assertEmptyFile("unpack.err");
assertEmptyFile("unpack.out");


assertEqualInt(0, chdir("out"));
assertTextFileContents("a","f");
assertTextFileContents("b","fh");
assertTextFileContents("c","d");
assertTextFileContents("d","fs");
assertTextFileContents("e","ds");
}
