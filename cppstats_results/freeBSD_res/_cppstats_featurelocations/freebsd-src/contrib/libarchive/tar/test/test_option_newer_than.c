























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_newer_than)
{
struct stat st;






assertMakeDir("test1in", 0755);
assertChdir("test1in");
assertMakeDir("a", 0755);
assertMakeDir("a/b", 0755);
assertMakeFile("old.txt", 0644, "old.txt");
assertEqualInt(0, stat("old.txt", &st));
sleepUntilAfter(st.st_mtime);
assertMakeFile("middle.txt", 0644, "middle.txt");
assertEqualInt(0, stat("middle.txt", &st));
sleepUntilAfter(st.st_mtime);
assertMakeFile("new.txt", 0644, "new");
assertMakeFile("a/b/new.txt", 0644, "new file in old directory");


assertEqualInt(0,
systemf("%s --format pax -cf ../test1.tar "
"--newer-than middle.txt *.txt a", testprog));
assertEqualInt(0,
systemf("%s --format pax -cf ../test2.tar *.txt a", testprog));
assertChdir("..");


assertMakeDir("test1out", 0755);
assertChdir("test1out");
assertEqualInt(0, systemf("%s xf ../test1.tar", testprog));
assertFileExists("new.txt");
assertFileExists("a/b/new.txt");
assertFileNotExists("middle.txt");
assertFileNotExists("old.txt");
assertChdir("..");


assertMakeDir("test2out", 0755);
assertChdir("test2out");
assertEqualInt(0, systemf("%s xf ../test2.tar --newer-than ../test1in/middle.txt", testprog));
assertFileExists("new.txt");
assertFileExists("a/b/new.txt");
assertFileNotExists("middle.txt");
assertFileNotExists("old.txt");
assertChdir("..");

}
