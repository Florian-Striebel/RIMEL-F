

































#include <sys/types.h>

#include <limits.h>

#include "_libelftc.h"

ELFTC_VCSID("$Id: libelftc_hash.c 2870 2013-01-07 10:38:43Z jkoshy $");






#if INT_MAX == 2147483647UL
#define FNV_PRIME 16777619UL
#define FNV_OFFSET 2166136261UL
#elif INT_MAX == 18446744073709551615ULL
#define FNV_PRIME 1099511628211ULL
#define FNV_OFFSET 14695981039346656037ULL
#else
#error sizeof(int) is unknown.
#endif

unsigned int
libelftc_hash_string(const char *s)
{
char c;
unsigned int hash;

for (hash = FNV_OFFSET; (c = *s) != '\0'; s++) {
hash ^= c;
hash *= FNV_PRIME;
}

return (hash);
}
