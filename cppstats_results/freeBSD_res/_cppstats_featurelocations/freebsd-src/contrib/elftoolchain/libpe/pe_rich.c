

























#include <assert.h>
#include <errno.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_rich.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

PE_RichHdr *
pe_rich_header(PE *pe)
{

if (pe == NULL) {
errno = EINVAL;
return (NULL);
}

if (pe->pe_rh == NULL && pe->pe_stub_ex > 0 &&
(pe->pe_flags & LIBPE_F_LOAD_DOS_STUB) == 0) {
assert((pe->pe_flags & LIBPE_F_SPECIAL_FILE) == 0);
(void) libpe_read_msdos_stub(pe);
}

if (pe->pe_rh == NULL) {
errno = ENOENT;
return (NULL);
}

return (pe->pe_rh);
}

static uint32_t
rol32(uint32_t n, int c)
{

c &= 0x1f;

return ((n << c) | (n >> (0x20 - c)));
}

int
pe_rich_header_validate(PE *pe)
{
PE_RichHdr *rh;
uint32_t cksum;
char *p;
int i, off;

if (pe_rich_header(pe) == NULL)
return (-1);

assert(pe->pe_rh_start != NULL);





cksum = pe->pe_rh_start - pe->pe_stub;





for (p = pe->pe_stub; p < pe->pe_rh_start; p++) {

off = p - pe->pe_stub;
if (off >= 0x3c && off < 0x40)
continue;
cksum += rol32((unsigned char) *p, off);
}


rh = pe->pe_rh;
for (i = 0; (uint32_t) i < rh->rh_total; i++)
cksum += rol32(rh->rh_compid[i], rh->rh_cnt[i]);


if (cksum == rh->rh_xor)
return (1);

return (0);
}
