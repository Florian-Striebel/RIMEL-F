























#include "test.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_LOCALE_H)
#include <locale.h>
#endif

DEFINE_TEST(test_option_t)
{
char *p;
int r;
time_t mtime;
char date[32];
char date2[32];


extract_reference_file("test_option_t.cpio");
r = systemf("%s -it < test_option_t.cpio >it.out 2>it.err", testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "it.err");
extract_reference_file("test_option_t.stdout");
p = slurpfile(NULL, "test_option_t.stdout");
assertTextFileContents(p, "it.out");
free(p);


r = systemf("%s -t < test_option_t.cpio >t.out 2>t.err", testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "t.err");
extract_reference_file("test_option_t.stdout");
p = slurpfile(NULL, "test_option_t.stdout");
assertTextFileContents(p, "t.out");
free(p);


assert(0 != systemf("%s -ot < test_option_t.cpio >ot.out 2>ot.err",
testprog));
assertEmptyFile("ot.out");


r = systemf("%s -itv < test_option_t.cpio >tv.out 2>tv.err", testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "tv.err");
extract_reference_file("test_option_tv.stdout");







r = systemf("%s -itnv < test_option_t.cpio >itnv.out 2>itnv.err",
testprog);
assertEqualInt(r, 0);
assertTextFileContents("1 block\n", "itnv.err");
p = slurpfile(NULL, "itnv.out");


assertEqualMem(p, "-rw-r--r-- 1 1000 1000 0 ",42);


mtime = 1;
#if defined(HAVE_LOCALE_H)
setlocale(LC_ALL, "");
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
strftime(date2, sizeof(date2)-1, "%b %d %Y", localtime(&mtime));
_snprintf(date, sizeof(date)-1, "%12.12s file", date2);
#else
strftime(date2, sizeof(date2)-1, "%b %e %Y", localtime(&mtime));
snprintf(date, sizeof(date)-1, "%12.12s file", date2);
#endif
assertEqualMem(p + 42, date, strlen(date));
free(p);


assert(0 != systemf("%s -in < test_option_t.cpio >in.out 2>in.err",
testprog));
assertEmptyFile("in.out");
}
