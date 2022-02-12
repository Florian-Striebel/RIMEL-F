























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_leading_slash)
{
const char *reffile = "test_leading_slash.tar";
char *errfile;
size_t errfile_size;
const char *expected_errmsg = "Removing leading '/' from member names";

extract_reference_file(reffile);
assertEqualInt(0, systemf("%s -xf %s >test.out 2>test.err", testprog, reffile));
assertFileExists("foo/file");
assertTextFileContents("foo\x0a", "foo/file");
assertTextFileContents("foo\x0a", "foo/hardlink");
assertIsHardlink("foo/file", "foo/hardlink");
assertEmptyFile("test.out");


if (assertFileExists("test.err")) {
errfile = slurpfile(&errfile_size, "test.err");
assert(strstr(errfile, expected_errmsg) != NULL);
free(errfile);
}
}

