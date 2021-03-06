#include <stdint.h>
#include "sds.h"
#define MAX_LONG_DOUBLE_CHARS 5*1024
typedef enum {
LD_STR_AUTO,
LD_STR_HUMAN,
LD_STR_HEX
} ld2string_mode;
int stringmatchlen(const char *p, int plen, const char *s, int slen, int nocase);
int stringmatch(const char *p, const char *s, int nocase);
int stringmatchlen_fuzz_test(void);
unsigned long long memtoull(const char *p, int *err);
const char *mempbrk(const char *s, size_t len, const char *chars, size_t charslen);
char *memmapchars(char *s, size_t len, const char *from, const char *to, size_t setlen);
uint32_t digits10(uint64_t v);
uint32_t sdigits10(int64_t v);
int ll2string(char *s, size_t len, long long value);
int ull2string(char *s, size_t len, unsigned long long value);
int string2ll(const char *s, size_t slen, long long *value);
int string2ull(const char *s, unsigned long long *value);
int string2l(const char *s, size_t slen, long *value);
int string2ld(const char *s, size_t slen, long double *dp);
int string2d(const char *s, size_t slen, double *dp);
int trimDoubleString(char *buf, size_t len);
int d2string(char *buf, size_t len, double value);
int ld2string(char *buf, size_t len, long double value, ld2string_mode mode);
sds getAbsolutePath(char *filename);
long getTimeZone(void);
int pathIsBaseName(char *path);
int dirCreateIfMissing(char *dname);
int dirExists(char *dname);
int dirRemove(char *dname);
int fileExist(char *filename);
sds makePath(char *path, char *filename);
#if defined(REDIS_TEST)
int utilTest(int argc, char **argv, int flags);
#endif
