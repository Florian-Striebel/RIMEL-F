























#include "test.h"
__FBSDID("$FreeBSD$");

static int
is_octal(const char *p, size_t l)
{
while (l > 0) {
if (*p < '0' || *p > '7')
return (0);
--l;
++p;
}
return (1);
}

static int
from_octal(const char *p, size_t l)
{
int r = 0;

while (l > 0) {
r *= 8;
r += *p - '0';
--l;
++p;
}
return (r);
}

#if !defined(_WIN32) || defined(__CYGWIN__)
static int
nlinks(const char *p)
{
struct stat st;
assertEqualInt(0, stat(p, &st));
return st.st_nlink;
}
#endif

DEFINE_TEST(test_option_c)
{
FILE *filelist;
int r;
int uid = 1000;
int dev, ino, gid = 1000;
time_t t, now;
char *p, *e;
size_t s;

assertUmask(0);





filelist = fopen("filelist", "w");


assertMakeFile("file", 0644, "1234567890");
fprintf(filelist, "file\n");


if (canSymlink()) {
assertMakeSymlink("symlink", "file", 0);
fprintf(filelist, "symlink\n");
}


assertMakeDir("dir", 0775);

now = time(NULL);
fprintf(filelist, "dir\n");


fclose(filelist);
r = systemf("%s -R 1000:1000 -oc <filelist >basic.out 2>basic.err", testprog);

assertTextFileContents("1 block\n", "basic.err");


failure("%s -oc crashed", testprog);
if (!assertEqualInt(r, 0))
return;


p = slurpfile(&s, "basic.out");
assertEqualInt(s, 512);
e = p;







assert(is_octal(e, 76));
assertEqualMem(e + 0, "070707", 6);
assert(is_octal(e + 6, 6));
dev = from_octal(e + 6, 6);
assert(is_octal(e + 12, 6));
ino = from_octal(e + 12, 6);
#if defined(_WIN32) && !defined(__CYGWIN__)

assertEqualMem(e + 18, "100666", 6);
#else
assertEqualMem(e + 18, "100644", 6);
#endif
if (uid < 0)
uid = from_octal(e + 24, 6);
assertEqualInt(from_octal(e + 24, 6), uid);
assert(is_octal(e + 30, 6));
gid = from_octal(e + 30, 6);
assertEqualMem(e + 36, "000001", 6);
failure("file entries should not have rdev set (dev field was 0%o)",
dev);
assertEqualMem(e + 42, "000000", 6);
t = from_octal(e + 48, 11);
assert(t <= now);
assert(t >= now - 2);
assertEqualMem(e + 59, "000005", 6);
assertEqualMem(e + 65, "00000000012", 11);
assertEqualMem(e + 76, "file\0", 5);
assertEqualMem(e + 81, "1234567890", 10);
e += 91;


if (canSymlink()) {
assert(is_octal(e, 76));
assertEqualMem(e + 0, "070707", 6);
assertEqualInt(dev, from_octal(e + 6, 6));
assert(ino != from_octal(e + 12, 6));
#if !defined(_WIN32) || defined(__CYGWIN__)


assertEqualMem(e + 18, "120777", 6);
#endif
assertEqualInt(from_octal(e + 24, 6), uid);
assertEqualInt(gid, from_octal(e + 30, 6));
assertEqualMem(e + 36, "000001", 6);
failure("file entries should have rdev == 0 (dev was 0%o)",
from_octal(e + 6, 6));
assertEqualMem(e + 42, "000000", 6);
t = from_octal(e + 48, 11);
assert(t <= now);
assert(t >= now - 2);
assertEqualMem(e + 59, "000010", 6);
assertEqualMem(e + 65, "00000000004", 11);
assertEqualMem(e + 76, "symlink\0", 8);
assertEqualMem(e + 84, "file", 4);
e += 88;
}


assert(is_octal(e, 76));
assertEqualMem(e + 0, "070707", 6);

assert(is_octal(e + 6, 6));
assertEqualInt(dev, from_octal(e + 6, 6));

assert(is_octal(e + 12, 6));
assert(ino != from_octal(e + 12, 6));
#if defined(_WIN32) && !defined(__CYGWIN__)

assertEqualMem(e + 18, "040777", 6);
#else

if (memcmp(e + 18, "042775", 6) != 0)
assertEqualMem(e + 18, "040775", 6);
#endif
assertEqualInt(uid, from_octal(e + 24, 6));

assert(is_octal(e + 30, 6));
assertEqualInt(gid, from_octal(e + 30, 6));

#if !defined(_WIN32) || defined(__CYGWIN__)
assertEqualInt(nlinks("dir"), from_octal(e + 36, 6));
#endif

t = from_octal(e + 48, 11);
assert(t <= now);
assert(t >= now - 2);
assertEqualMem(e + 59, "000004", 6);
assertEqualMem(e + 65, "00000000000", 11);
assertEqualMem(e + 76, "dir\0", 4);
e += 80;




assert(is_octal(e, 76));
assertEqualMem(e + 0, "070707", 6);
assertEqualMem(e + 6, "000000", 6);
assertEqualMem(e + 12, "000000", 6);
assertEqualMem(e + 18, "000000", 6);
assertEqualMem(e + 24, "000000", 6);
assertEqualMem(e + 30, "000000", 6);
assertEqualMem(e + 36, "000001", 6);
assertEqualMem(e + 42, "000000", 6);
assertEqualMem(e + 48, "00000000000", 11);
assertEqualMem(e + 59, "000013", 6);
assertEqualMem(e + 65, "00000000000", 11);
assertEqualMem(e + 76, "TRAILER!!!\0", 11);

free(p);
}
