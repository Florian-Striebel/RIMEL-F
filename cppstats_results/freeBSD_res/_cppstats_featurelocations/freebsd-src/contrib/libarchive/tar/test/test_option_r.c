























#include "test.h"
__FBSDID("$FreeBSD$");




DEFINE_TEST(test_option_r)
{
char *buff;
char *p0, *p1;
size_t buff_size = 35000;
size_t s, buff_size_rounded;
int r, i;

buff = NULL;
p0 = NULL;
p1 = NULL;


assertMakeFile("f1", 0644, "abc");
r = systemf("%s cf archive.tar --format=ustar f1 >step1.out 2>step1.err", testprog);
failure("Error invoking %s cf archive.tar f1", testprog);
assertEqualInt(r, 0);
assertEmptyFile("step1.out");
assertEmptyFile("step1.err");


p0 = slurpfile(&s, "archive.tar");
if (!assert(p0 != NULL))
goto done;
if (!assert(s >= 2048))
goto done;
assertEqualMem(p0 + 0, "f1", 3);
assertEqualMem(p0 + 512, "abc", 3);
assertEqualMem(p0 + 1024, "\0\0\0\0\0\0\0\0", 8);
assertEqualMem(p0 + 1536, "\0\0\0\0\0\0\0\0", 8);


buff = malloc(buff_size);
assert(buff != NULL);
if (buff == NULL)
goto done;

for (i = 0; i < (int)buff_size; ++i)
buff[i] = "abcdefghijklmnopqrstuvwxyz"[rand() % 26];
buff[buff_size - 1] = '\0';
assertMakeFile("f1", 0644, buff);
r = systemf("%s rf archive.tar --format=ustar f1 >step2.out 2>step2.err", testprog);
failure("Error invoking %s rf archive.tar f1", testprog);
assertEqualInt(r, 0);
assertEmptyFile("step2.out");
assertEmptyFile("step2.err");


p1 = slurpfile(&s, "archive.tar");
if (!assert(p1 != NULL))
goto done;
buff_size_rounded = ((buff_size + 511) / 512) * 512;
assert(s >= 2560 + buff_size_rounded);

assertEqualMem(p0, p1, 1024);

assertEqualMem(p1 + 1024, "f1", 3);
assertEqualMem(p1 + 1536, buff, buff_size);

assertEqualMem(p1 + 1536 + buff_size_rounded, "\0\0\0\0\0\0\0\0", 8);
assertEqualMem(p1 + 2048 + buff_size_rounded, "\0\0\0\0\0\0\0\0", 8);

free(p0);
p0 = p1;


assertMakeFile("f2", 0644, "f2");
r = systemf("%s rf archive.tar --format=ustar f2 >step3.out 2>step3.err", testprog);
failure("Error invoking %s rf archive.tar f2", testprog);
assertEqualInt(r, 0);
assertEmptyFile("step3.out");
assertEmptyFile("step3.err");


p1 = slurpfile(&s, "archive.tar");
if (!assert(p1 != NULL))
goto done;
assert(s >= 3584 + buff_size_rounded);

assertEqualMem(p0, p1, 1536 + buff_size_rounded);

assertEqualMem(p1 + 1536 + buff_size_rounded, "f2", 3);
assertEqualMem(p1 + 2048 + buff_size_rounded, "f2", 3);

assertEqualMem(p1 + 2560 + buff_size_rounded, "\0\0\0\0\0\0\0\0", 8);
assertEqualMem(p1 + 3072 + buff_size_rounded, "\0\0\0\0\0\0\0\0", 8);
free(p1);


assertMakeDir("extract", 0775);
assertChdir("extract");
r = systemf("%s xf ../archive.tar >extract.out 2>extract.err", testprog);
failure("Error invoking %s xf archive.tar", testprog);
assertEqualInt(r, 0);
assertEmptyFile("extract.out");
assertEmptyFile("extract.err");


assertFileContents(buff, (int)strlen(buff), "f1");
done:
free(buff);
free(p0);
}
