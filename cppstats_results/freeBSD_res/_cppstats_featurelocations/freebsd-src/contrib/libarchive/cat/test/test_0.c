























#include "test.h"





#if !defined(_WIN32) || defined(__CYGWIN__)
#define DEV_NULL "/dev/null"
#else
#define DEV_NULL "NUL"
#endif

DEFINE_TEST(test_0)
{
struct stat st;

failure("File %s does not exist?!", testprog);
if (!assertEqualInt(0, stat(testprogfile, &st))) {
fprintf(stderr,
"\nFile %s does not exist; aborting test.\n\n",
testprog);
exit(1);
}

failure("%s is not executable?!", testprog);
if (!assert((st.st_mode & 0111) != 0)) {
fprintf(stderr,
"\nFile %s not executable; aborting test.\n\n",
testprog);
exit(1);
}





if (0 != systemf("%s --version >" DEV_NULL, testprog)) {
failure("Unable to successfully run: %s --version\n", testprog);
assert(0);
}


}
