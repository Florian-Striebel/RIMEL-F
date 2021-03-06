























#include "test.h"






static int
in_first_line(const char *p, const char *substring)
{
size_t l = strlen(substring);

while (*p != '\0' && *p != '\n') {
if (memcmp(p, substring, l) == 0)
return (1);
++p;
}
return (0);
}

DEFINE_TEST(test_help)
{
int r;
char *p;
size_t plen;


r = systemf("%s --help >help.stdout 2>help.stderr", testprog);
assertEqualInt(r, 0);
failure("--help should generate nothing to stderr.");
assertEmptyFile("help.stderr");

p = slurpfile(&plen, "help.stdout");
failure("Help output should be long enough.");
assert(plen >= 6);
failure("First line of help output should contain 'bsdcat': %s", p);
assert(in_first_line(p, "bsdcat"));




free(p);


r = systemf("%s -h >h.stdout 2>h.stderr", testprog);
assertEqualInt(r, 0);
failure("-h should generate nothing to stderr.");
assertEmptyFile("h.stderr");
failure("stdout should be same for -h and --help");
assertEqualFile("h.stdout", "help.stdout");
}
