























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_lz4)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("%s -cf - --lz4 f >archive.out 2>archive.err",
testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (strstr(p, "Unsupported compression") != NULL) {
skipping("This version of bsdtar was compiled "
"without lz4 support");
goto done;
}




if (strstr(p, "Can't launch") != NULL && !canLz4()) {
skipping("This version of bsdtar uses an external lz4 program "
"but no such program is available on this system.");
goto done;
}




if (strstr(p, "Can't write") != NULL && !canLz4()) {
skipping("This version of bsdtar uses an external lz4 program "
"but no such program is available on this system.");
goto done;
}


if (strstr(p, "Error closing") != NULL && !canLz4()) {
skipping("This version of bsdcpio uses an external lz4 program "
"but no such program is available on this system.");
return;
}
failure("--lz4 option is broken: %s", p);
assertEqualInt(r, 0);
goto done;
}
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "\x04\x22\x4d\x18", 4);

done:
free(p);
}
