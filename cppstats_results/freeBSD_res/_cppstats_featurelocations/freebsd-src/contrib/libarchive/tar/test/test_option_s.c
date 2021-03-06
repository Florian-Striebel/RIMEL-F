























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_s)
{
struct stat st;


assertMakeDir("in", 0755);
assertMakeDir("in/d1", 0755);
assertMakeFile("in/d1/foo", 0644, "foo");
assertMakeFile("in/d1/bar", 0644, "bar");
if (canSymlink()) {
assertMakeFile("in/d1/realfile", 0644, "realfile");
assertMakeSymlink("in/d1/symlink", "realfile", 0);
}
assertMakeFile("in/d1/hardlink1", 0644, "hardlinkedfile");
assertMakeHardlink("in/d1/hardlink2", "in/d1/hardlink1");


systemf("%s -cf - -s /foo/bar/ in/d1/foo > NUL 2> check.err",
testprog);
assertEqualInt(0, stat("check.err", &st));
if (st.st_size != 0) {
skipping("%s does not support -s option on this platform",
testprog);
return;
}




assertMakeDir("test1", 0755);
systemf("%s -cf test1_1.tar -s /foo/bar/ in/d1/foo", testprog);
systemf("%s -xf test1_1.tar -C test1", testprog);
assertFileContents("foo", 3, "test1/in/d1/bar");
systemf("%s -cf test1_2.tar -s /d1/d2/ in/d1/foo", testprog);
systemf("%s -xf test1_2.tar -C test1", testprog);
assertFileContents("foo", 3, "test1/in/d2/foo");




assertMakeDir("test2", 0755);
systemf("%s -cf test2.tar in/d1/foo", testprog);
systemf("%s -xf test2.tar -s /foo/bar/ -C test2", testprog);
assertFileContents("foo", 3, "test2/in/d1/bar");




systemf("%s -cf test3.tar -s ,in/d1/foo,, in/d1/foo", testprog);
systemf("%s -tvf test3.tar > in.lst", testprog);
assertEmptyFile("in.lst");




assertMakeDir("test4", 0755);
systemf("%s -cf test4.tar in/d1/foo in/d1/bar",
testprog);
systemf("%s -xf test4.tar -s /foo/bar/ -s }bar}baz} -C test4",
testprog);
assertFileContents("foo", 3, "test4/in/d1/bar");
assertFileContents("bar", 3, "test4/in/d1/baz");




assertMakeDir("test5", 0755);
systemf("%s -cf test5.tar in/d1/foo in/d1/bar", testprog);
systemf("%s -xf test5.tar -s /foo/bar/ -s }bar}foo} -C test5", testprog);
assertFileContents("foo", 3, "test5/in/d1/bar");
assertFileContents("bar", 3, "test5/in/d1/foo");




if (canSymlink()) {

assertMakeDir("test6a", 0755);
systemf("%s -cf - in/d1 | %s -xf - -s /d1/d2/ -C test6a",
testprog, testprog);
assertFileContents("realfile", 8, "test6a/in/d2/realfile");
assertFileContents("realfile", 8, "test6a/in/d2/symlink");
assertIsSymlink("test6a/in/d2/symlink", "realfile", 0);

assertMakeDir("test6b", 0755);
systemf("%s -cf - -s /d1/d2/ in/d1 | %s -xf - -C test6b",
testprog, testprog);
assertFileContents("realfile", 8, "test6b/in/d2/realfile");
assertFileContents("realfile", 8, "test6b/in/d2/symlink");
assertIsSymlink("test6b/in/d2/symlink", "realfile", 0);
}




if (canSymlink()) {

assertMakeDir("test7a", 0755);
systemf("%s -cf - in/d1 | %s -xf - -s /realfile/realfile-renamed/ -C test7a",
testprog, testprog);
assertFileContents("realfile", 8, "test7a/in/d1/realfile-renamed");
assertFileContents("realfile", 8, "test7a/in/d1/symlink");
assertIsSymlink("test7a/in/d1/symlink", "realfile-renamed", 0);

assertMakeDir("test7b", 0755);
systemf("%s -cf - -s /realfile/realfile-renamed/ in/d1 | %s -xf - -C test7b",
testprog, testprog);
assertFileContents("realfile", 8, "test7b/in/d1/realfile-renamed");
assertFileContents("realfile", 8, "test7b/in/d1/symlink");
assertIsSymlink("test7b/in/d1/symlink", "realfile-renamed", 0);
}





assertMakeDir("test8a", 0755);
systemf("%s -cf test8a.tar in/d1", testprog);
systemf("%s -xf test8a.tar -s /d1/d2/ -C test8a", testprog);
assertIsHardlink("test8a/in/d2/hardlink1", "test8a/in/d2/hardlink2");

assertMakeDir("test8b", 0755);
systemf("%s -cf test8b.tar -s /d1/d2/ in/d1", testprog);
systemf("%s -xf test8b.tar -C test8b", testprog);
assertIsHardlink("test8b/in/d2/hardlink1", "test8b/in/d2/hardlink2");





assertMakeDir("test9a", 0755);
systemf("%s -cf test9a.tar in/d1", testprog);
systemf("%s -xf test9a.tar -s /hardlink1/hardlink1-renamed/ -C test9a",
testprog);
assertIsHardlink("test9a/in/d1/hardlink1-renamed", "test9a/in/d1/hardlink2");

assertMakeDir("test9b", 0755);
systemf("%s -cf test9b.tar in/d1", testprog);
systemf("%s -xf test9b.tar -s /hardlink2/hardlink2-renamed/ -C test9b",
testprog);
assertIsHardlink("test9b/in/d1/hardlink1", "test9b/in/d1/hardlink2-renamed");

assertMakeDir("test9c", 0755);
systemf("%s -cf test9c.tar -s /hardlink1/hardlink1-renamed/ in/d1",
testprog);
systemf("%s -xf test9c.tar -C test9c", testprog);
assertIsHardlink("test9c/in/d1/hardlink1-renamed", "test9c/in/d1/hardlink2");

assertMakeDir("test9d", 0755);
systemf("%s -cf test9d.tar -s /hardlink2/hardlink2-renamed/ in/d1",
testprog);
systemf("%s -xf test9d.tar -C test9d", testprog);
assertIsHardlink("test9d/in/d1/hardlink1", "test9d/in/d1/hardlink2-renamed");




if (canSymlink()) {

assertMakeDir("test10a", 0755);
systemf("%s -cf - in/d1 | %s -xf - -s /realfile/foo/S -s /foo/realfile/ -C test10a",
testprog, testprog);
assertFileContents("realfile", 8, "test10a/in/d1/foo");
assertFileContents("foo", 3, "test10a/in/d1/realfile");
assertFileContents("foo", 3, "test10a/in/d1/symlink");
assertIsSymlink("test10a/in/d1/symlink", "realfile", 0);

assertMakeDir("test10b", 0755);
systemf("%s -cf - -s /realfile/foo/S -s /foo/realfile/ in/d1 | %s -xf - -C test10b",
testprog, testprog);
assertFileContents("realfile", 8, "test10b/in/d1/foo");
assertFileContents("foo", 3, "test10b/in/d1/realfile");
assertFileContents("foo", 3, "test10b/in/d1/symlink");
assertIsSymlink("test10b/in/d1/symlink", "realfile", 0);
}




if (canSymlink()) {

assertMakeDir("test11a", 0755);
systemf("%s -cf - in/d1 | %s -xf - -s /realfile/foo/sR -C test11a",
testprog, testprog);
assertFileContents("foo", 3, "test11a/in/d1/foo");
assertFileContents("realfile", 8, "test11a/in/d1/realfile");
assertFileContents("foo", 3, "test11a/in/d1/symlink");
assertIsSymlink("test11a/in/d1/symlink", "foo", 0);

assertMakeDir("test11b", 0755);
systemf("%s -cf - -s /realfile/foo/R in/d1 | %s -xf - -C test11b",
testprog, testprog);
assertFileContents("foo", 3, "test11b/in/d1/foo");
assertFileContents("realfile", 8, "test11b/in/d1/realfile");
assertFileContents("foo", 3, "test11b/in/d1/symlink");
assertIsSymlink("test11b/in/d1/symlink", "foo", 0);
}






extract_reference_file("test_option_s.tar.Z");
assertMakeDir("test12a", 0755);
systemf("%s -xf test_option_s.tar.Z -s /hardlink1/foo/H -s /foo/hardlink1/ %s -C test12a",
testprog, canSymlink()?"":"--exclude in/d1/symlink");
assertFileContents("foo", 3, "test12a/in/d1/hardlink1");
assertFileContents("hardlinkedfile", 14, "test12a/in/d1/foo");
assertFileContents("foo", 3, "test12a/in/d1/hardlink2");
assertIsHardlink("test12a/in/d1/hardlink1", "test12a/in/d1/hardlink2");











extract_reference_file("test_option_s.tar.Z");
assertMakeDir("test13a", 0755);
systemf("%s -xf test_option_s.tar.Z -s /hardlink1/foo/Rh -s /foo/hardlink1/Rh %s -C test13a",
testprog, canSymlink()?"":"--exclude in/d1/symlink");
assertFileContents("foo", 3, "test13a/in/d1/foo");
assertFileContents("hardlinkedfile", 14, "test13a/in/d1/hardlink1");
assertFileContents("foo", 3, "test13a/in/d1/hardlink2");
assertIsHardlink("test13a/in/d1/foo", "test13a/in/d1/hardlink2");






assertMakeDir("test14", 0755);
systemf("%s -cf test14.tar in/d1/foo in/d1/bar",
testprog);
systemf("%s -xf test14.tar -s /o/z/g -s /bar/baz/ -C test14",
testprog);
assertFileContents("foo", 3, "test14/in/d1/fzz");
assertFileContents("bar", 3, "test14/in/d1/baz");

systemf("%s -cf test14.tar in/d1/foo in/d1/bar",
testprog);
systemf("%s -xf test14.tar -s /o/z/ -s /bar/baz/ -C test14",
testprog);
assertFileContents("foo", 3, "test14/in/d1/fzo");
assertFileContents("bar", 3, "test14/in/d1/baz");
}
