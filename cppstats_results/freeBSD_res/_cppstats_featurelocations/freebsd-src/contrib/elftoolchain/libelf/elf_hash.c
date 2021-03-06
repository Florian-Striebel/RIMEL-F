

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_hash.c 3174 2015-03-27 17:13:41Z emaste $");





unsigned long
elf_hash(const char *name)
{
unsigned long h, t;
const unsigned char *s;

s = (const unsigned char *) name;
h = t = 0;

for (; *s != '\0'; h = h & ~t) {
h = (h << 4) + *s++;
t = h & 0xF0000000UL;
if (t)
h ^= t >> 24;
}

return (h);
}
