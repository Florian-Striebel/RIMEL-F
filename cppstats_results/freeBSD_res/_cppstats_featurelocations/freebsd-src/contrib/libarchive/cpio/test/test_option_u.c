























#include "test.h"
#if defined(HAVE_UTIME_H)
#include <utime.h>
#elif defined(HAVE_SYS_UTIME_H)
#include <sys/utime.h>
#endif
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_u)
{
struct utimbuf times;
char *p;
size_t s;
int r;


assertMakeFile("f", 0644, "a");


r = systemf("echo f| %s -pd copy >copy.out 2>copy.err",
testprog);
assertEqualInt(r, 0);


p = slurpfile(&s, "copy/f");
assertEqualInt(s, 1);
assertEqualMem(p, "a", 1);
free(p);


assertMakeFile("f", 0644, "b");


memset(&times, 0, sizeof(times));
times.actime = 1;
times.modtime = 3;
assertEqualInt(0, utime("f", &times));


r = systemf("echo f| %s -pd copy >copy.out 2>copy.err",
testprog);
assertEqualInt(r, 0);


p = slurpfile(&s, "copy/f");
assertEqualInt(s, 1);
assertEqualMem(p, "a", 1);
free(p);


r = systemf("echo f| %s -pud copy >copy.out 2>copy.err",
testprog);
assertEqualInt(r, 0);


p = slurpfile(&s, "copy/f");
assertEqualInt(s, 1);
assertEqualMem(p, "b", 1);
free(p);
}
