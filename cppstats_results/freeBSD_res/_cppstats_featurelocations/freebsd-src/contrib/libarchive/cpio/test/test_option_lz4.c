























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_lz4)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("echo f | %s -o --lz4 >archive.out 2>archive.err",
testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (strstr(p, "compression not available") != NULL) {
skipping("This version of bsdcpio was compiled "
"without lz4 support");
free(p);
return;
}




if (strstr(p, "Can't launch") != NULL && !canLz4()) {
skipping("This version of bsdcpio uses an external lz4 program "
"but no such program is available on this system.");
free(p);
return;
}




if (strstr(p, "Can't write") != NULL && !canLz4()) {
skipping("This version of bsdcpio uses an external lz4 program "
"but no such program is available on this system.");
free(p);
return;
}


if (strstr(p, "Error closing") != NULL && !canLz4()) {
skipping("This version of bsdcpio uses an external lz4 program "
"but no such program is available on this system.");
free(p);
return;
}
failure("--lz4 option is broken: %s", p);
free(p);
assertEqualInt(r, 0);
return;
}
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "\x04\x22\x4d\x18", 4);
free(p);
}
