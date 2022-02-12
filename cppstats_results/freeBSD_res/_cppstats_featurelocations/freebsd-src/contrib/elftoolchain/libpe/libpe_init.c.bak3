

























#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: libpe_init.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

int
libpe_open_object(PE *pe)
{
struct stat sb;
mode_t mode;
char magic[sizeof(PE_DosHdr)];

if (fstat(pe->pe_fd, &sb) < 0)
return (-1);

mode = sb.st_mode;
pe->pe_fsize = (size_t) sb.st_size;


if (!S_ISREG(mode) && !S_ISCHR(mode) && !S_ISFIFO(mode) &&
!S_ISSOCK(mode)) {
errno = EINVAL;
return (-1);
}


if (pe->pe_cmd == PE_C_RDWR && !S_ISREG(mode)) {
errno = EINVAL;
return (-1);
}


if (S_ISREG(mode) && pe->pe_fsize < sizeof(PE_CoffHdr)) {
errno = ENOENT;
return (-1);
}





if (read(pe->pe_fd, magic, 2) != 2) {
errno = EIO;
return (-1);
}

if (magic[0] == 'M' && magic[1] == 'Z') {
pe->pe_obj = PE_O_PE32;
if (read(pe->pe_fd, &magic[2], sizeof(PE_DosHdr) - 2) !=
(ssize_t) sizeof(PE_DosHdr) - 2) {
errno = EIO;
return (-1);
}
return (libpe_parse_msdos_header(pe, magic));

} else if (magic[0] == 'P' && magic[1] == 'E') {
if (read(pe->pe_fd, magic, 2) != 2) {
errno = EIO;
return (-1);
}
if (magic[0] == '\0' && magic[1] == '\0') {
pe->pe_obj = PE_O_PE32;
if (read(pe->pe_fd, magic, sizeof(PE_CoffHdr)) !=
(ssize_t) sizeof(PE_CoffHdr)) {
errno = EIO;
return (-1);
}
return (libpe_parse_coff_header(pe, magic));
}
errno = ENOENT;
return (-1);

} else {
pe->pe_obj = PE_O_COFF;
if (read(pe->pe_fd, &magic[2], sizeof(PE_CoffHdr) - 2) !=
(ssize_t) sizeof(PE_CoffHdr) - 2) {
errno = EIO;
return (-1);
}
return (libpe_parse_coff_header(pe, magic));
}
}

void
libpe_release_object(PE *pe)
{
PE_Scn *ps, *_ps;

if (pe->pe_dh)
free(pe->pe_dh);

if (pe->pe_rh) {
free(pe->pe_rh->rh_compid);
free(pe->pe_rh->rh_cnt);
free(pe->pe_rh);
}

if (pe->pe_ch)
free(pe->pe_ch);

if (pe->pe_oh)
free(pe->pe_oh);

if (pe->pe_dd)
free(pe->pe_dd);

if (pe->pe_stub)
free(pe->pe_stub);

STAILQ_FOREACH_SAFE(ps, &pe->pe_scn, ps_next, _ps)
libpe_release_scn(ps);

free(pe);
}
