























#include "test.h"
__FBSDID("$FreeBSD$");





#if !defined(_WIN32) || defined(__CYGWIN__)
#define DEV_NULL "/dev/null"
#else
#define DEV_NULL "NUL"
#endif

DEFINE_TEST(test_0)
{
struct stat st;

failure("File %s does not exist?!", testprogfile);
if (!assertEqualInt(0, stat(testprogfile, &st))) {
fprintf(stderr,
"\nFile %s does not exist; aborting test.\n\n",
testprog);
exit(1);
}

failure("%s is not executable?!", testprogfile);
if (!assert((st.st_mode & 0111) != 0)) {
fprintf(stderr,
"\nFile %s not executable; aborting test.\n\n",
testprog);
exit(1);
}





if (0 == systemf("%s --version >" DEV_NULL, testprog)) {

} else if (0 == systemf("%s -W version >" DEV_NULL, testprog)) {

} else {
failure("Unable to successfully run any of the following:\n"
" * %s --version\n"
" * %s -W version\n",
testprog, testprog);
assert(0);
}


}
