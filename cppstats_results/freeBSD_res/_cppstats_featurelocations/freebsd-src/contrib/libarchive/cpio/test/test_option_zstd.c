























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_zstd)
{
char *p;
int r;
size_t s;


assertMakeFile("f", 0644, "a");


r = systemf("echo f | %s -o --zstd >archive.out 2>archive.err",
testprog);
p = slurpfile(&s, "archive.err");
p[s] = '\0';
if (r != 0) {
if (strstr(p, "Unsupported compression") != NULL) {
skipping("This version of bsdcpio was compiled "
"without zstd support");
goto done;
}




if (strstr(p, "Can't launch") != NULL && !canZstd()) {
skipping("This version of bsdcpio uses an external zstd program "
"but no such program is available on this system.");
goto done;
}




if (strstr(p, "Can't write") != NULL && !canZstd()) {
skipping("This version of bsdcpio uses an external zstd program "
"but no such program is available on this system.");
goto done;
}


if (strstr(p, "Error closing") != NULL && !canZstd()) {
skipping("This version of bsdcpio uses an external zstd program "
"but no such program is available on this system.");
return;
}
failure("--zstd option is broken: %s", p);
assertEqualInt(r, 0);
goto done;
}
free(p);

p = slurpfile(&s, "archive.out");
assert(s > 2);
assertEqualMem(p, "\x28\xb5\x2f\xfd", 4);

done:
free(p);
}
