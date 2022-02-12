























#include "test.h"
__FBSDID("$FreeBSD$");

#if defined(__CYGWIN__)
#include <limits.h>
#include <sys/cygwin.h>
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#endif























static int
compute_loop_max(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
static int LOOP_MAX = 0;
char buf[MAX_PATH];
size_t cwdlen;

if (LOOP_MAX == 0) {
assert(_getcwd(buf, MAX_PATH) != NULL);
cwdlen = strlen(buf);



LOOP_MAX = MAX_PATH - (int)cwdlen - 12 - 4 - 1;
}
return LOOP_MAX;
#elif defined(__CYGWIN__) && !defined(HAVE_CYGWIN_CONV_PATH)
static int LOOP_MAX = 0;
if (LOOP_MAX == 0) {
char wbuf[PATH_MAX];
char pbuf[PATH_MAX];
size_t wcwdlen;
size_t pcwdlen;
size_t cwdlen;
assert(getcwd(pbuf, PATH_MAX) != NULL);
pcwdlen = strlen(pbuf);
cygwin_conv_to_full_win32_path(pbuf, wbuf);
wcwdlen = strlen(wbuf);
cwdlen = ((wcwdlen > pcwdlen) ? wcwdlen : pcwdlen);

LOOP_MAX = PATH_MAX - (int)cwdlen - 12 - 4 - 4;
}
return LOOP_MAX;
#else

return 200;
#endif
}







static char *filenames[201];

static void
compute_filenames(void)
{
char buff[250];
size_t i,j;

filenames[0] = strdup("");
filenames[1] = strdup("1");
filenames[2] = strdup("a2");
for (i = 3; i < sizeof(filenames)/sizeof(filenames[0]); ++i) {

for (j = 0; j < i; ++j)
buff[j] = 'a' + (j % 26);
buff[j--] = '\0';

buff[j--] = '0' + (i % 10);
if (i > 9) {
buff[j--] = '0' + ((i / 10) % 10);
if (i > 99)
buff[j--] = '0' + (char)(i / 100);
}
buff[j] = '_';

assertEqualInt(strlen(buff), i);
filenames[i] = strdup(buff);
}
}

static void
create_tree(void)
{
char buff[260];
char buff2[260];
int i;
int LOOP_MAX;

compute_filenames();


if (!canSymlink()) {
skipping("Symlink checks");
}

assertMakeDir("original", 0775);
assertEqualInt(0, chdir("original"));
LOOP_MAX = compute_loop_max();

assertMakeDir("f", 0775);
assertMakeDir("l", 0775);
assertMakeDir("m", 0775);
assertMakeDir("s", 0775);
assertMakeDir("d", 0775);

for (i = 1; i < LOOP_MAX; i++) {
failure("Internal sanity check failed: i = %d", i);
assert(filenames[i] != NULL);

sprintf(buff, "f/%s", filenames[i]);
assertMakeFile(buff, 0777, buff);


sprintf(buff2, "l/%s", filenames[i]);
assertMakeHardlink(buff2, buff);


sprintf(buff2, "m/%s", filenames[i]);
assertMakeHardlink(buff2, buff);

if (canSymlink()) {

sprintf(buff, "s/%s", filenames[i]);
sprintf(buff2, "../f/%s", filenames[i]);
failure("buff=\"%s\" buff2=\"%s\"", buff, buff2);
assertMakeSymlink(buff, buff2, 0);
}

buff[0] = 'd';
failure("buff=\"%s\"", buff);
assertMakeDir(buff, 0775);
}

assertEqualInt(0, chdir(".."));
}

#define LIMIT_NONE 200
#define LIMIT_USTAR 100

static void
verify_tree(size_t limit)
{
char name1[260];
char name2[260];
size_t i, LOOP_MAX;

LOOP_MAX = compute_loop_max();


for (i = 1; i < LOOP_MAX; i++) {

sprintf(name1, "f/%s", filenames[i]);
if (i <= limit) {
assertFileExists(name1);
assertFileContents(name1, (int)strlen(name1), name1);
}

sprintf(name2, "l/%s", filenames[i]);
if (i + 2 <= limit) {

assertIsHardlink(name1, name2);

name2[0] = 'm';
assertIsHardlink(name1, name2);
}

if (canSymlink()) {

sprintf(name1, "s/%s", filenames[i]);
sprintf(name2, "../f/%s", filenames[i]);
if (strlen(name2) <= limit)
assertIsSymlink(name1, name2, 0);
}


sprintf(name1, "d/%s", filenames[i]);
if (i + 1 <= limit) {
if (assertIsDir(name1, -1)) {




}
}
}

#if !defined(_WIN32) || defined(__CYGWIN__)
{
const char *dp;

for (dp = "dflms"; *dp != '\0'; ++dp) {
DIR *d;
struct dirent *de;
char dir[2];
dir[0] = *dp; dir[1] = '\0';
d = opendir(dir);
failure("Unable to open dir '%s'", dir);
if (!assert(d != NULL))
continue;
while ((de = readdir(d)) != NULL) {
char *p = de->d_name;
if (p[0] == '.')
continue;
switch(dp[0]) {
case 'l': case 'm': case 'd':
failure("strlen(p)=%zu", strlen(p));
assert(strlen(p) < limit);
assertEqualString(p,
filenames[strlen(p)]);
break;
case 'f': case 's':
failure("strlen(p)=%zu", strlen(p));
assert(strlen(p) < limit + 1);
assertEqualString(p,
filenames[strlen(p)]);
break;
default:
failure("File %s shouldn't be here", p);
assert(0);
}
}
closedir(d);
}
}
#endif
}

static void
copy_basic(void)
{
int r;







assertMakeDir("plain", 0775);
assertEqualInt(0, chdir("plain"));




r = systemf("%s cf archive -C ../original f d l m s >pack.out 2>pack.err",
testprog);
failure("Error invoking \"%s cf\"", testprog);
assertEqualInt(r, 0);


assertEmptyFile("pack.err");
assertEmptyFile("pack.out");




r = systemf("%s xf archive >unpack.out 2>unpack.err", testprog);
failure("Error invoking %s xf archive", testprog);
assertEqualInt(r, 0);


assertEmptyFile("unpack.err");
assertEmptyFile("unpack.out");

verify_tree(LIMIT_NONE);
assertEqualInt(0, chdir(".."));
}

static void
copy_ustar(void)
{
const char *target = "ustar";
int r;







assertMakeDir(target, 0775);
assertEqualInt(0, chdir(target));




r = systemf("%s cf archive --format=ustar -C ../original f d l m s >pack.out 2>pack.err",
testprog);
failure("Error invoking \"%s cf archive --format=ustar\"", testprog);
assertEqualInt(r, 0);


assertEmptyFile("pack.out");






r = systemf("%s xf archive >unpack.out 2>unpack.err", testprog);
failure("Error invoking %s xf archive", testprog);
assertEqualInt(r, 0);


assertEmptyFile("unpack.err");
assertEmptyFile("unpack.out");

verify_tree(LIMIT_USTAR);
assertEqualInt(0, chdir("../.."));
}

DEFINE_TEST(test_copy)
{
assertUmask(0);
create_tree();


copy_basic();


copy_ustar();
}
