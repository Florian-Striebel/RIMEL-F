

























#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_dos.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

PE_DosHdr *
pe_msdos_header(PE *pe)
{

if (pe == NULL) {
errno = EINVAL;
return (NULL);
}

if (pe->pe_dh == NULL) {
errno = ENOENT;
return (NULL);
}

return (pe->pe_dh);
}

char *
pe_msdos_stub(PE *pe, size_t *len)
{

if (pe == NULL || len == NULL) {
errno = EINVAL;
return (NULL);
}

if (pe->pe_stub_ex > 0 &&
(pe->pe_flags & LIBPE_F_LOAD_DOS_STUB) == 0) {
assert((pe->pe_flags & LIBPE_F_SPECIAL_FILE) == 0);
(void) libpe_read_msdos_stub(pe);
}

*len = sizeof(PE_DosHdr) + pe->pe_stub_ex;

return (pe->pe_stub);
}

int
ps_update_msdos_header(PE *pe, PE_DosHdr *dh)
{

if (pe == NULL || dh == NULL) {
errno = EINVAL;
return (-1);
}

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (-1);
}

if (pe->pe_dh == NULL) {
if ((pe->pe_dh = malloc(sizeof(PE_DosHdr))) == NULL) {
errno = ENOMEM;
return (-1);
}
}

*pe->pe_dh = *dh;

pe->pe_flags |= LIBPE_F_DIRTY_DOS_HEADER;

return (0);
}

int
ps_update_msdos_stub(PE *pe, char *dos_stub, size_t sz)
{

if (pe == NULL || dos_stub == NULL || sz == 0) {
errno = EINVAL;
return (-1);
}

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (-1);
}

pe->pe_stub_app = dos_stub;
pe->pe_stub_app_sz = sz;

return (0);
}
