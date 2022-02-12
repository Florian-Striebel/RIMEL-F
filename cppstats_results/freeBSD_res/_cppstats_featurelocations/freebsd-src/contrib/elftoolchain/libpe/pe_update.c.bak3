

























#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_update.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

off_t
pe_update(PE *pe)
{
off_t off;

if (pe == NULL) {
errno = EINVAL;
return (-1);
}

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (-1);
}

if (pe->pe_cmd == PE_C_RDWR || (pe->pe_cmd == PE_C_WRITE &&
(pe->pe_flags & LIBPE_F_SPECIAL_FILE) == 0)) {
if (lseek(pe->pe_fd, 0, SEEK_SET) < 0) {
errno = EIO;
return (-1);
}
}

off = 0;

if (pe->pe_obj == PE_O_PE32 || pe->pe_obj == PE_O_PE32P) {
if ((off = libpe_write_msdos_stub(pe, off)) < 0)
return (-1);

if ((off = libpe_write_pe_header(pe, off)) < 0)
return (-1);
}

if (libpe_resync_sections(pe, off) < 0)
return (-1);

if ((off = libpe_write_coff_header(pe, off)) < 0)
return (-1);

if ((off = libpe_write_section_headers(pe, off)) < 0)
return (-1);

if ((off = libpe_write_sections(pe, off)) < 0)
return (-1);

if (ftruncate(pe->pe_fd, off) < 0) {
errno = EIO;
return (-1);
}

return (off);
}
