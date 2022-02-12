

























#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: libpe_rich.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

static char *
memfind(char *s, const char *find, size_t slen, size_t flen)
{
int i;

if (slen == 0 || flen == 0 || flen > slen)
return (NULL);

for (i = 0; (size_t) i <= slen - flen; i++) {
if (s[i] != find[0])
continue;
if (flen == 1)
return (&s[i]);
if (memcmp(&s[i + 1], &find[1], flen - 1) == 0)
return (&s[i]);
}

return (NULL);
}

int
libpe_parse_rich_header(PE *pe)
{
PE_RichHdr *rh;
char *p, *r, *s;
uint32_t x;
int found, i;

assert(pe->pe_stub != NULL && pe->pe_stub_ex > 0);


s = pe->pe_stub + sizeof(PE_DosHdr);
r = memfind(s, PE_RICH_TEXT, pe->pe_stub_ex, 4);
if (r == NULL || r + 8 > s + pe->pe_stub_ex) {
errno = ENOENT;
return (-1);
}

if ((rh = calloc(1, sizeof(*rh))) == NULL) {
errno = ENOMEM;
return (-1);
}

rh->rh_xor = le32dec(r + 4);





found = 0;
for (p = r - 4; p >= s; p -= 4) {
x = le32dec(p) ^ rh->rh_xor;
if (x == PE_RICH_HIDDEN) {
found = 1;
break;
}
}
if (!found) {
free(rh);
errno = ENOENT;
return (-1);
}






pe->pe_rh_start = p;
p += 16;
rh->rh_total = (r - p) / 8;
if ((rh->rh_compid = malloc(rh->rh_total * sizeof(*rh->rh_compid))) ==
NULL) {
free(rh);
errno = ENOMEM;
return (-1);
}
if ((rh->rh_cnt = malloc(rh->rh_total * sizeof(*rh->rh_cnt))) ==
NULL) {
free(rh->rh_compid);
free(rh);
errno = ENOMEM;
return (-1);
}
for (i = 0; (uint32_t) i < rh->rh_total; i++, p += 8) {
rh->rh_compid[i] = le32dec(p) ^ rh->rh_xor;
rh->rh_cnt[i] = le32dec(p + 4) ^ rh->rh_xor;
}

pe->pe_rh = rh;

return (0);
}
