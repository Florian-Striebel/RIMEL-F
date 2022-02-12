























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_print_longpath)
{
const char *reffile = "test_print_longpath.tar.Z";
char buff[2048];
int i, j, k;


extract_reference_file(reffile);


memset(buff, 0, sizeof(buff));
for (k = 0; k < 4; k++) {
for (j = 0; j < k+1; j++) {
for (i = 0; i < 10; i++)
strncat(buff, "0123456789",
sizeof(buff) - strlen(buff) -1);
strncat(buff, "/", sizeof(buff) - strlen(buff) -1);
}
strncat(buff, "\n", sizeof(buff) - strlen(buff) -1);
}
buff[sizeof(buff)-1] = '\0';

assertEqualInt(0,
systemf("%s -tf %s >test.out 2>test.err", testprog, reffile));
assertTextFileContents(buff, "test.out");
assertEmptyFile("test.err");
}
