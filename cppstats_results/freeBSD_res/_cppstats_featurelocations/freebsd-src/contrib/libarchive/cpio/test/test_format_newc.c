























#include "test.h"
__FBSDID("$FreeBSD$");






#define PAD(n, block) ((~(n) + 1) & ((block) - 1))

static int
is_hex(const char *p, size_t l)
{
while (l > 0) {
if ((*p >= '0' && *p <= '9')
|| (*p >= 'a' && *p <= 'f')
|| (*p >= 'A' && *p <= 'F'))
{
--l;
++p;
} else
return (0);

}
return (1);
}


static uint32_t
from_hex(const char *p, size_t l)
{
uint32_t r = 0;

while (l > 0) {
r *= 16;
if (*p >= 'a' && *p <= 'f')
r += *p + 10 - 'a';
else if (*p >= 'A' && *p <= 'F')
r += *p + 10 - 'A';
else
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

DEFINE_TEST(test_format_newc)
{
FILE *list;
int r;
uint32_t devmajor, devminor, ino, gid, uid;
time_t t, t2, now;
char *p, *e;
size_t s;
uint64_t fs, ns;
char result[1024];

assertUmask(0);

#if !defined(_WIN32)
uid = getuid();
#endif





list = fopen("list", "w");


assertMakeFile("file1", 0644, "1234567890");
fprintf(list, "file1\n");


assertMakeHardlink("hardlink", "file1");
fprintf(list, "hardlink\n");


assertMakeHardlink("hardlink2", "file1");


if (canSymlink()) {
assertMakeSymlink("symlink", "file1", 0);
fprintf(list, "symlink\n");
}


assertMakeDir("dir", 0775);
fprintf(list, "dir\n");


memset(result, 0, sizeof(result));
if (is_LargeInode("file1")) {
strncat(result,
"bsdcpio: file1: large inode number truncated: ",
sizeof(result) - strlen(result) -1);
strncat(result, strerror(ERANGE),
sizeof(result) - strlen(result) -1);
strncat(result, "\n",
sizeof(result) - strlen(result) -1);
}
if (canSymlink() && is_LargeInode("symlink")) {
strncat(result,
"bsdcpio: symlink: large inode number truncated: ",
sizeof(result) - strlen(result) -1);
strncat(result, strerror(ERANGE),
sizeof(result) - strlen(result) -1);
strncat(result, "\n",
sizeof(result) - strlen(result) -1);
}
if (is_LargeInode("dir")) {
strncat(result,
"bsdcpio: dir: large inode number truncated: ",
sizeof(result) - strlen(result) -1);
strncat(result, strerror(ERANGE),
sizeof(result) - strlen(result) -1);
strncat(result, "\n",
sizeof(result) - strlen(result) -1);
}
if (is_LargeInode("hardlink")) {
strncat(result,
"bsdcpio: hardlink: large inode number truncated: ",
sizeof(result) - strlen(result) -1);
strncat(result, strerror(ERANGE),
sizeof(result) - strlen(result) -1);
strncat(result, "\n",
sizeof(result) - strlen(result) -1);
}


now = time(NULL);


fclose(list);
r = systemf("%s -o --format=newc <list >newc.out 2>newc.err",
testprog);
if (!assertEqualInt(r, 0))
return;


if (canSymlink()) {
strncat(result, "2 blocks\n", sizeof(result) - strlen(result) -1);
} else {
strncat(result, "1 block\n", sizeof(result) - strlen(result) -1);
}
assertTextFileContents(result, "newc.err");


p = slurpfile(&s, "newc.out");
assertEqualInt(s, canSymlink() ? 1024 : 512);
e = p;







assert(is_hex(e, 110));
assertEqualMem(e + 0, "070701", 6);
ino = from_hex(e + 6, 8);
#if defined(_WIN32) && !defined(__CYGWIN__)

assertEqualInt(0x8180, from_hex(e + 14, 8) & 0xffc0);
#else
assertEqualInt(0x81a4, from_hex(e + 14, 8));
#endif
#if defined(_WIN32)
uid = from_hex(e + 22, 8);
#else
assertEqualInt(from_hex(e + 22, 8), uid);
#endif
gid = from_hex(e + 30, 8);
assertEqualMem(e + 38, "00000003", 8);
t = from_hex(e + 46, 8);
failure("t=%#08jx now=%#08jx=%jd", (intmax_t)t, (intmax_t)now,
(intmax_t)now);
assert(t <= now);
failure("t=%#08jx now - 2=%#08jx=%jd", (intmax_t)t, (intmax_t)now - 2,
(intmax_t)now - 2);
assert(t >= now - 2);
failure("newc format stores body only with last appearance of a link\n"
" first appearance should be empty, so this file size\n"
" field should be zero");
assertEqualInt(0, from_hex(e + 54, 8));
fs = (uint64_t)from_hex(e + 54, 8);
fs += PAD(fs, 4);
devmajor = from_hex(e + 62, 8);
devminor = from_hex(e + 70, 8);
assert(is_hex(e + 78, 8));
assert(is_hex(e + 86, 8));
assertEqualMem(e + 94, "00000006", 8);
ns = (uint64_t)from_hex(e + 94, 8);
ns += PAD(ns + 2, 4);
assertEqualInt(0, from_hex(e + 102, 8));
assertEqualMem(e + 110, "file1\0", 6);


e += 110 + fs + ns;

if (canSymlink()) {

assert(is_hex(e, 110));
assertEqualMem(e + 0, "070701", 6);
assert(is_hex(e + 6, 8));
#if defined(_WIN32) && !defined(CYGWIN)

assertEqualInt(0xa180, from_hex(e + 14, 8) & 0xffc0);
#else
assertEqualInt(0xa1ff, from_hex(e + 14, 8));
#endif
assertEqualInt(from_hex(e + 22, 8), uid);
assertEqualInt(gid, from_hex(e + 30, 8));
assertEqualMem(e + 38, "00000001", 8);
t2 = from_hex(e + 46, 8);
failure("First entry created at t=%#08jx this entry created"
" at t2=%#08jx", (intmax_t)t, (intmax_t)t2);
assert(t2 == t || t2 == t + 1);
assertEqualMem(e + 54, "00000005", 8);
fs = (uint64_t)from_hex(e + 54, 8);
fs += PAD(fs, 4);
assertEqualInt(devmajor, from_hex(e + 62, 8));
assertEqualInt(devminor, from_hex(e + 70, 8));
assert(is_hex(e + 78, 8));
assert(is_hex(e + 86, 8));
assertEqualMem(e + 94, "00000008", 8);
ns = (uint64_t)from_hex(e + 94, 8);
ns += PAD(ns + 2, 4);
assertEqualInt(0, from_hex(e + 102, 8));
assertEqualMem(e + 110, "symlink\0\0\0", 10);
assertEqualMem(e + 110 + ns, "file1\0\0\0", 8);
e += 110 + fs + ns;
}


assert(is_hex(e, 110));
assertEqualMem(e + 0, "070701", 6);
assert(is_hex(e + 6, 8));
#if defined(_WIN32) && !defined(__CYGWIN__)

assertEqualInt(0x41c0, from_hex(e + 14, 8) & 0xffc0);
#else

assertEqualInt(040775, from_hex(e + 14, 8) & ~02000);
#endif
assertEqualInt(uid, from_hex(e + 22, 8));
assertEqualInt(gid, from_hex(e + 30, 8));
#if !defined(_WIN32) || defined(__CYGWIN__)
assertEqualInt(nlinks("dir"), from_hex(e + 38, 8));
#endif
t2 = from_hex(e + 46, 8);
failure("First entry created at t=%#08jx this entry created at"
"t2=%#08jx", (intmax_t)t, (intmax_t)t2);
assert(t2 == t || t2 == t + 1);
assertEqualMem(e + 54, "00000000", 8);
fs = (uint64_t)from_hex(e + 54, 8);
fs += PAD(fs, 4);
assertEqualInt(devmajor, from_hex(e + 62, 8));
assertEqualInt(devminor, from_hex(e + 70, 8));
assert(is_hex(e + 78, 8));
assert(is_hex(e + 86, 8));
assertEqualMem(e + 94, "00000004", 8);
ns = (uint64_t)from_hex(e + 94, 8);
ns += PAD(ns + 2, 4);
assertEqualInt(0, from_hex(e + 102, 8));
assertEqualMem(e + 110, "dir\0\0\0", 6);
e += 110 + fs + ns;




assert(is_hex(e, 110));
assertEqualMem(e + 0, "070701", 6);
failure("If these aren't the same, then the hardlink detection failed to match them.");
assertEqualInt(ino, from_hex(e + 6, 8));
#if defined(_WIN32) && !defined(__CYGWIN__)

assertEqualInt(0x8180, from_hex(e + 14, 8) & 0xffc0);
#else
assertEqualInt(0x81a4, from_hex(e + 14, 8));
#endif
assertEqualInt(from_hex(e + 22, 8), uid);
assertEqualInt(gid, from_hex(e + 30, 8));
assertEqualMem(e + 38, "00000003", 8);
t2 = from_hex(e + 46, 8);
failure("First entry created at t=%#08jx this entry created at"
"t2=%#08jx", (intmax_t)t, (intmax_t)t2);
assert(t2 == t || t2 == t + 1);
assertEqualInt(10, from_hex(e + 54, 8));
fs = (uint64_t)from_hex(e + 54, 8);
fs += PAD(fs, 4);
assertEqualInt(devmajor, from_hex(e + 62, 8));
assertEqualInt(devminor, from_hex(e + 70, 8));
assert(is_hex(e + 78, 8));
assert(is_hex(e + 86, 8));
assertEqualMem(e + 94, "00000009", 8);
ns = (uint64_t)from_hex(e + 94, 8);
ns += PAD(ns + 2, 4);
assertEqualInt(0, from_hex(e + 102, 8));
assertEqualMem(e + 110, "hardlink\0\0", 10);
assertEqualMem(e + 110 + ns, "1234567890\0\0", 12);
e += 110 + ns + fs;


assert(is_hex(e, 110));
assertEqualMem(e + 0, "070701", 6);
assertEqualMem(e + 8, "00000000", 8);
assertEqualMem(e + 14, "00000000", 8);
assertEqualMem(e + 22, "00000000", 8);
assertEqualMem(e + 30, "00000000", 8);
assertEqualMem(e + 38, "00000001", 8);
assertEqualMem(e + 46, "00000000", 8);
assertEqualMem(e + 54, "00000000", 8);
assertEqualMem(e + 62, "00000000", 8);
assertEqualMem(e + 70, "00000000", 8);
assertEqualMem(e + 78, "00000000", 8);
assertEqualMem(e + 86, "00000000", 8);
assertEqualInt(11, from_hex(e + 94, 8));
assertEqualMem(e + 102, "00000000", 8);
assertEqualMem(e + 110, "TRAILER!!!\0\0", 12);

free(p);
}
