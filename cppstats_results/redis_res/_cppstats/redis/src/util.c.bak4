




























#include "fmacros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <float.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#include "util.h"
#include "sha256.h"
#include "config.h"


int stringmatchlen(const char *pattern, int patternLen,
const char *string, int stringLen, int nocase)
{
while(patternLen && stringLen) {
switch(pattern[0]) {
case '*':
while (patternLen && pattern[1] == '*') {
pattern++;
patternLen--;
}
if (patternLen == 1)
return 1;
while(stringLen) {
if (stringmatchlen(pattern+1, patternLen-1,
string, stringLen, nocase))
return 1;
string++;
stringLen--;
}
return 0;
break;
case '?':
string++;
stringLen--;
break;
case '[':
{
int not, match;

pattern++;
patternLen--;
not = pattern[0] == '^';
if (not) {
pattern++;
patternLen--;
}
match = 0;
while(1) {
if (pattern[0] == '\\' && patternLen >= 2) {
pattern++;
patternLen--;
if (pattern[0] == string[0])
match = 1;
} else if (pattern[0] == ']') {
break;
} else if (patternLen == 0) {
pattern--;
patternLen++;
break;
} else if (patternLen >= 3 && pattern[1] == '-') {
int start = pattern[0];
int end = pattern[2];
int c = string[0];
if (start > end) {
int t = start;
start = end;
end = t;
}
if (nocase) {
start = tolower(start);
end = tolower(end);
c = tolower(c);
}
pattern += 2;
patternLen -= 2;
if (c >= start && c <= end)
match = 1;
} else {
if (!nocase) {
if (pattern[0] == string[0])
match = 1;
} else {
if (tolower((int)pattern[0]) == tolower((int)string[0]))
match = 1;
}
}
pattern++;
patternLen--;
}
if (not)
match = !match;
if (!match)
return 0;
string++;
stringLen--;
break;
}
case '\\':
if (patternLen >= 2) {
pattern++;
patternLen--;
}

default:
if (!nocase) {
if (pattern[0] != string[0])
return 0;
} else {
if (tolower((int)pattern[0]) != tolower((int)string[0]))
return 0;
}
string++;
stringLen--;
break;
}
pattern++;
patternLen--;
if (stringLen == 0) {
while(*pattern == '*') {
pattern++;
patternLen--;
}
break;
}
}
if (patternLen == 0 && stringLen == 0)
return 1;
return 0;
}

int stringmatch(const char *pattern, const char *string, int nocase) {
return stringmatchlen(pattern,strlen(pattern),string,strlen(string),nocase);
}


int stringmatchlen_fuzz_test(void) {
char str[32];
char pat[32];
int cycles = 10000000;
int total_matches = 0;
while(cycles--) {
int strlen = rand() % sizeof(str);
int patlen = rand() % sizeof(pat);
for (int j = 0; j < strlen; j++) str[j] = rand() % 128;
for (int j = 0; j < patlen; j++) pat[j] = rand() % 128;
total_matches += stringmatchlen(pat, patlen, str, strlen, 0);
}
return total_matches;
}









unsigned long long memtoull(const char *p, int *err) {
const char *u;
char buf[128];
long mul;
unsigned long long val;
unsigned int digits;

if (err) *err = 0;


u = p;
if (*u == '-') {
if (err) *err = 1;
return 0;
}
while(*u && isdigit(*u)) u++;
if (*u == '\0' || !strcasecmp(u,"b")) {
mul = 1;
} else if (!strcasecmp(u,"k")) {
mul = 1000;
} else if (!strcasecmp(u,"kb")) {
mul = 1024;
} else if (!strcasecmp(u,"m")) {
mul = 1000*1000;
} else if (!strcasecmp(u,"mb")) {
mul = 1024*1024;
} else if (!strcasecmp(u,"g")) {
mul = 1000L*1000*1000;
} else if (!strcasecmp(u,"gb")) {
mul = 1024L*1024*1024;
} else {
if (err) *err = 1;
return 0;
}



digits = u-p;
if (digits >= sizeof(buf)) {
if (err) *err = 1;
return 0;
}
memcpy(buf,p,digits);
buf[digits] = '\0';

char *endptr;
errno = 0;
val = strtoull(buf,&endptr,10);
if ((val == 0 && errno == EINVAL) || *endptr != '\0') {
if (err) *err = 1;
return 0;
}
return val*mul;
}




const char *mempbrk(const char *s, size_t len, const char *chars, size_t charslen) {
for (size_t j = 0; j < len; j++) {
for (size_t n = 0; n < charslen; n++)
if (s[j] == chars[n]) return &s[j];
}

return NULL;
}




char *memmapchars(char *s, size_t len, const char *from, const char *to, size_t setlen) {
for (size_t j = 0; j < len; j++) {
for (size_t i = 0; i < setlen; i++) {
if (s[j] == from[i]) {
s[j] = to[i];
break;
}
}
}
return s;
}



uint32_t digits10(uint64_t v) {
if (v < 10) return 1;
if (v < 100) return 2;
if (v < 1000) return 3;
if (v < 1000000000000UL) {
if (v < 100000000UL) {
if (v < 1000000) {
if (v < 10000) return 4;
return 5 + (v >= 100000);
}
return 7 + (v >= 10000000UL);
}
if (v < 10000000000UL) {
return 9 + (v >= 1000000000UL);
}
return 11 + (v >= 100000000000UL);
}
return 12 + digits10(v / 1000000000000UL);
}


uint32_t sdigits10(int64_t v) {
if (v < 0) {

uint64_t uv = (v != LLONG_MIN) ?
(uint64_t)-v : ((uint64_t) LLONG_MAX)+1;
return digits10(uv)+1;
} else {
return digits10(v);
}
}




int ll2string(char *dst, size_t dstlen, long long svalue) {
unsigned long long value;
int negative = 0;



if (svalue < 0) {
if (svalue != LLONG_MIN) {
value = -svalue;
} else {
value = ((unsigned long long) LLONG_MAX)+1;
}
if (dstlen < 2)
return 0;
negative = 1;
dst[0] = '-';
dst++;
dstlen--;
} else {
value = svalue;
}


int length = ull2string(dst, dstlen, value);
if (length == 0) return 0;
return length + negative;
}









int ull2string(char *dst, size_t dstlen, unsigned long long value) {
static const char digits[201] =
"0001020304050607080910111213141516171819"
"2021222324252627282930313233343536373839"
"4041424344454647484950515253545556575859"
"6061626364656667686970717273747576777879"
"8081828384858687888990919293949596979899";


uint32_t length = digits10(value);
if (length >= dstlen) return 0;


uint32_t next = length - 1;
dst[next + 1] = '\0';
while (value >= 100) {
int const i = (value % 100) * 2;
value /= 100;
dst[next] = digits[i + 1];
dst[next - 1] = digits[i];
next -= 2;
}


if (value < 10) {
dst[next] = '0' + (uint32_t) value;
} else {
int i = (uint32_t) value * 2;
dst[next] = digits[i + 1];
dst[next - 1] = digits[i];
}

return length;
}













int string2ll(const char *s, size_t slen, long long *value) {
const char *p = s;
size_t plen = 0;
int negative = 0;
unsigned long long v;


if (plen == slen)
return 0;


if (slen == 1 && p[0] == '0') {
if (value != NULL) *value = 0;
return 1;
}



if (p[0] == '-') {
negative = 1;
p++; plen++;


if (plen == slen)
return 0;
}


if (p[0] >= '1' && p[0] <= '9') {
v = p[0]-'0';
p++; plen++;
} else {
return 0;
}


while (plen < slen && p[0] >= '0' && p[0] <= '9') {
if (v > (ULLONG_MAX / 10))
return 0;
v *= 10;

if (v > (ULLONG_MAX - (p[0]-'0')))
return 0;
v += p[0]-'0';

p++; plen++;
}


if (plen < slen)
return 0;



if (negative) {
if (v > ((unsigned long long)(-(LLONG_MIN+1))+1))
return 0;
if (value != NULL) *value = -v;
} else {
if (v > LLONG_MAX)
return 0;
if (value != NULL) *value = v;
}
return 1;
}






int string2ull(const char *s, unsigned long long *value) {
long long ll;
if (string2ll(s,strlen(s),&ll)) {
if (ll < 0) return 0;
*value = ll;
return 1;
}
errno = 0;
char *endptr = NULL;
*value = strtoull(s,&endptr,10);
if (errno == EINVAL || errno == ERANGE || !(*s != '\0' && *endptr == '\0'))
return 0;
return 1;
}




int string2l(const char *s, size_t slen, long *lval) {
long long llval;

if (!string2ll(s,slen,&llval))
return 0;

if (llval < LONG_MIN || llval > LONG_MAX)
return 0;

*lval = (long)llval;
return 1;
}








int string2ld(const char *s, size_t slen, long double *dp) {
char buf[MAX_LONG_DOUBLE_CHARS];
long double value;
char *eptr;

if (slen == 0 || slen >= sizeof(buf)) return 0;
memcpy(buf,s,slen);
buf[slen] = '\0';

errno = 0;
value = strtold(buf, &eptr);
if (isspace(buf[0]) || eptr[0] != '\0' ||
(size_t)(eptr-buf) != slen ||
(errno == ERANGE &&
(value == HUGE_VAL || value == -HUGE_VAL || value == 0)) ||
errno == EINVAL ||
isnan(value))
return 0;

if (dp) *dp = value;
return 1;
}








int string2d(const char *s, size_t slen, double *dp) {
errno = 0;
char *eptr;
*dp = strtod(s, &eptr);
if (slen == 0 ||
isspace(((const char*)s)[0]) ||
(size_t)(eptr-(char*)s) != slen ||
(errno == ERANGE &&
(*dp == HUGE_VAL || *dp == -HUGE_VAL || *dp == 0)) ||
isnan(*dp))
return 0;
return 1;
}






int d2string(char *buf, size_t len, double value) {
if (isnan(value)) {
len = snprintf(buf,len,"nan");
} else if (isinf(value)) {
if (value < 0)
len = snprintf(buf,len,"-inf");
else
len = snprintf(buf,len,"inf");
} else if (value == 0) {

if (1.0/value < 0)
len = snprintf(buf,len,"-0");
else
len = snprintf(buf,len,"0");
} else {
#if (DBL_MANT_DIG >= 52) && (LLONG_MAX == 0x7fffffffffffffffLL)









double min = -4503599627370495;
double max = 4503599627370496;
if (value > min && value < max && value == ((double)((long long)value)))
len = ll2string(buf,len,(long long)value);
else
#endif
len = snprintf(buf,len,"%.17g",value);
}

return len;
}


int trimDoubleString(char *buf, size_t len) {
if (strchr(buf,'.') != NULL) {
char *p = buf+len-1;
while(*p == '0') {
p--;
len--;
}
if (*p == '.') len--;
}
buf[len] = '\0';
return len;
}










int ld2string(char *buf, size_t len, long double value, ld2string_mode mode) {
size_t l = 0;

if (isinf(value)) {


if (len < 5) return 0;
if (value > 0) {
memcpy(buf,"inf",3);
l = 3;
} else {
memcpy(buf,"-inf",4);
l = 4;
}
} else {
switch (mode) {
case LD_STR_AUTO:
l = snprintf(buf,len,"%.17Lg",value);
if (l+1 > len) return 0;
break;
case LD_STR_HEX:
l = snprintf(buf,len,"%La",value);
if (l+1 > len) return 0;
break;
case LD_STR_HUMAN:





l = snprintf(buf,len,"%.17Lf",value);
if (l+1 > len) return 0;

if (strchr(buf,'.') != NULL) {
char *p = buf+l-1;
while(*p == '0') {
p--;
l--;
}
if (*p == '.') l--;
}
if (l == 2 && buf[0] == '-' && buf[1] == '0') {
buf[0] = '0';
l = 1;
}
break;
default: return 0;
}
}
buf[l] = '\0';
return l;
}






void getRandomBytes(unsigned char *p, size_t len) {

static int seed_initialized = 0;
static unsigned char seed[64];
static uint64_t counter = 0;

if (!seed_initialized) {




FILE *fp = fopen("/dev/urandom","r");
if (fp == NULL || fread(seed,sizeof(seed),1,fp) != 1) {


for (unsigned int j = 0; j < sizeof(seed); j++) {
struct timeval tv;
gettimeofday(&tv,NULL);
pid_t pid = getpid();
seed[j] = tv.tv_sec ^ tv.tv_usec ^ pid ^ (long)fp;
}
} else {
seed_initialized = 1;
}
if (fp) fclose(fp);
}

while(len) {

unsigned char digest[SHA256_BLOCK_SIZE];
unsigned char kxor[64];
unsigned int copylen =
len > SHA256_BLOCK_SIZE ? SHA256_BLOCK_SIZE : len;


memcpy(kxor,seed,sizeof(kxor));
for (unsigned int i = 0; i < sizeof(kxor); i++) kxor[i] ^= 0x36;


SHA256_CTX ctx;
sha256_init(&ctx);
sha256_update(&ctx,kxor,sizeof(kxor));
sha256_update(&ctx,(unsigned char*)&counter,sizeof(counter));
sha256_final(&ctx,digest);


memcpy(kxor,seed,sizeof(kxor));
for (unsigned int i = 0; i < sizeof(kxor); i++) kxor[i] ^= 0x5C;


sha256_init(&ctx);
sha256_update(&ctx,kxor,sizeof(kxor));
sha256_update(&ctx,digest,SHA256_BLOCK_SIZE);
sha256_final(&ctx,digest);


counter++;

memcpy(p,digest,copylen);
len -= copylen;
p += copylen;
}
}





void getRandomHexChars(char *p, size_t len) {
char *charset = "0123456789abcdef";
size_t j;

getRandomBytes((unsigned char*)p,len);
for (j = 0; j < len; j++) p[j] = charset[p[j] & 0x0F];
}








sds getAbsolutePath(char *filename) {
char cwd[1024];
sds abspath;
sds relpath = sdsnew(filename);

relpath = sdstrim(relpath," \r\n\t");
if (relpath[0] == '/') return relpath;


if (getcwd(cwd,sizeof(cwd)) == NULL) {
sdsfree(relpath);
return NULL;
}
abspath = sdsnew(cwd);
if (sdslen(abspath) && abspath[sdslen(abspath)-1] != '/')
abspath = sdscat(abspath,"/");







while (sdslen(relpath) >= 3 &&
relpath[0] == '.' && relpath[1] == '.' && relpath[2] == '/')
{
sdsrange(relpath,3,-1);
if (sdslen(abspath) > 1) {
char *p = abspath + sdslen(abspath)-2;
int trimlen = 1;

while(*p != '/') {
p--;
trimlen++;
}
sdsrange(abspath,0,-(trimlen+1));
}
}


abspath = sdscatsds(abspath,relpath);
sdsfree(relpath);
return abspath;
}





long getTimeZone(void) {
#if defined(__linux__) || defined(__sun)
return timezone;
#else
struct timeval tv;
struct timezone tz;

gettimeofday(&tv, &tz);

return tz.tz_minuteswest * 60L;
#endif
}





int pathIsBaseName(char *path) {
return strchr(path,'/') == NULL && strchr(path,'\\') == NULL;
}

int fileExist(char *filename) {
struct stat statbuf;
return stat(filename, &statbuf) == 0 && S_ISREG(statbuf.st_mode);
}

int dirExists(char *dname) {
struct stat statbuf;
return stat(dname, &statbuf) == 0 && S_ISDIR(statbuf.st_mode);
}

int dirCreateIfMissing(char *dname) {
if (mkdir(dname, 0755) != 0) {
if (errno != EEXIST) {
return -1;
} else if (!dirExists(dname)) {
errno = ENOTDIR;
return -1;
}
}
return 0;
}

int dirRemove(char *dname) {
DIR *dir;
struct stat stat_entry;
struct dirent *entry;
char full_path[PATH_MAX + 1];

if ((dir = opendir(dname)) == NULL) {
return -1;
}

while ((entry = readdir(dir)) != NULL) {
if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

snprintf(full_path, sizeof(full_path), "%s/%s", dname, entry->d_name);

int fd = open(full_path, O_RDONLY|O_NONBLOCK);
if (fd == -1) {
closedir(dir);
return -1;
}

if (fstat(fd, &stat_entry) == -1) {
close(fd);
closedir(dir);
return -1;
}
close(fd);

if (S_ISDIR(stat_entry.st_mode) != 0) {
if (dirRemove(full_path) == -1) {
return -1;
}
continue;
}

if (unlink(full_path) != 0) {
closedir(dir);
return -1;
}
}

if (rmdir(dname) != 0) {
closedir(dir);
return -1;
}

closedir(dir);
return 0;
}

sds makePath(char *path, char *filename) {
return sdscatfmt(sdsempty(), "%s/%s", path, filename);
}

#if defined(REDIS_TEST)
#include <assert.h>

static void test_string2ll(void) {
char buf[32];
long long v;


strcpy(buf,"+1");
assert(string2ll(buf,strlen(buf),&v) == 0);


strcpy(buf," 1");
assert(string2ll(buf,strlen(buf),&v) == 0);


strcpy(buf,"1 ");
assert(string2ll(buf,strlen(buf),&v) == 0);


strcpy(buf,"01");
assert(string2ll(buf,strlen(buf),&v) == 0);

strcpy(buf,"-1");
assert(string2ll(buf,strlen(buf),&v) == 1);
assert(v == -1);

strcpy(buf,"0");
assert(string2ll(buf,strlen(buf),&v) == 1);
assert(v == 0);

strcpy(buf,"1");
assert(string2ll(buf,strlen(buf),&v) == 1);
assert(v == 1);

strcpy(buf,"99");
assert(string2ll(buf,strlen(buf),&v) == 1);
assert(v == 99);

strcpy(buf,"-99");
assert(string2ll(buf,strlen(buf),&v) == 1);
assert(v == -99);

strcpy(buf,"-9223372036854775808");
assert(string2ll(buf,strlen(buf),&v) == 1);
assert(v == LLONG_MIN);

strcpy(buf,"-9223372036854775809");
assert(string2ll(buf,strlen(buf),&v) == 0);

strcpy(buf,"9223372036854775807");
assert(string2ll(buf,strlen(buf),&v) == 1);
assert(v == LLONG_MAX);

strcpy(buf,"9223372036854775808");
assert(string2ll(buf,strlen(buf),&v) == 0);
}

static void test_string2l(void) {
char buf[32];
long v;


strcpy(buf,"+1");
assert(string2l(buf,strlen(buf),&v) == 0);


strcpy(buf,"01");
assert(string2l(buf,strlen(buf),&v) == 0);

strcpy(buf,"-1");
assert(string2l(buf,strlen(buf),&v) == 1);
assert(v == -1);

strcpy(buf,"0");
assert(string2l(buf,strlen(buf),&v) == 1);
assert(v == 0);

strcpy(buf,"1");
assert(string2l(buf,strlen(buf),&v) == 1);
assert(v == 1);

strcpy(buf,"99");
assert(string2l(buf,strlen(buf),&v) == 1);
assert(v == 99);

strcpy(buf,"-99");
assert(string2l(buf,strlen(buf),&v) == 1);
assert(v == -99);

#if LONG_MAX != LLONG_MAX
strcpy(buf,"-2147483648");
assert(string2l(buf,strlen(buf),&v) == 1);
assert(v == LONG_MIN);

strcpy(buf,"-2147483649");
assert(string2l(buf,strlen(buf),&v) == 0);

strcpy(buf,"2147483647");
assert(string2l(buf,strlen(buf),&v) == 1);
assert(v == LONG_MAX);

strcpy(buf,"2147483648");
assert(string2l(buf,strlen(buf),&v) == 0);
#endif
}

static void test_ll2string(void) {
char buf[32];
long long v;
int sz;

v = 0;
sz = ll2string(buf, sizeof buf, v);
assert(sz == 1);
assert(!strcmp(buf, "0"));

v = -1;
sz = ll2string(buf, sizeof buf, v);
assert(sz == 2);
assert(!strcmp(buf, "-1"));

v = 99;
sz = ll2string(buf, sizeof buf, v);
assert(sz == 2);
assert(!strcmp(buf, "99"));

v = -99;
sz = ll2string(buf, sizeof buf, v);
assert(sz == 3);
assert(!strcmp(buf, "-99"));

v = -2147483648;
sz = ll2string(buf, sizeof buf, v);
assert(sz == 11);
assert(!strcmp(buf, "-2147483648"));

v = LLONG_MIN;
sz = ll2string(buf, sizeof buf, v);
assert(sz == 20);
assert(!strcmp(buf, "-9223372036854775808"));

v = LLONG_MAX;
sz = ll2string(buf, sizeof buf, v);
assert(sz == 19);
assert(!strcmp(buf, "9223372036854775807"));
}

#define UNUSED(x) (void)(x)
int utilTest(int argc, char **argv, int flags) {
UNUSED(argc);
UNUSED(argv);
UNUSED(flags);

test_string2ll();
test_string2l();
test_ll2string();
return 0;
}
#endif
