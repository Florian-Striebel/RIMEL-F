

























#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_coff.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

PE_CoffHdr *
pe_coff_header(PE *pe)
{

if (pe->pe_ch == NULL) {
errno = ENOENT;
return (NULL);
}

return (pe->pe_ch);
}

PE_OptHdr *
pe_opt_header(PE *pe)
{

if (pe->pe_oh == NULL) {
errno = ENOENT;
return (NULL);
}

return (pe->pe_oh);
}

PE_DataDir *
pe_data_dir(PE *pe)
{

if (pe->pe_dd == NULL) {
errno = ENOENT;
return (NULL);
}

return (pe->pe_dd);
}

int
pe_update_coff_header(PE *pe, PE_CoffHdr *ch)
{

if (pe == NULL || ch == NULL) {
errno = EINVAL;
return (-1);
}

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (-1);
}

if (pe->pe_ch == NULL) {
if ((pe->pe_ch = malloc(sizeof(PE_CoffHdr))) == NULL) {
errno = ENOMEM;
return (-1);
}
} else {

if (pe->pe_ch->ch_optsize != ch->ch_optsize)
pe->pe_flags |= LIBPE_F_DIRTY_OPT_HEADER;
}

*pe->pe_ch = *ch;

pe->pe_flags |= LIBPE_F_DIRTY_COFF_HEADER;

return (0);
}

int
pe_update_opt_header(PE *pe, PE_OptHdr *oh)
{

if (pe == NULL || oh == NULL) {
errno = EINVAL;
return (-1);
}

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (-1);
}

if (pe->pe_oh == NULL) {
if ((pe->pe_oh = malloc(sizeof(PE_OptHdr))) == NULL) {
errno = ENOMEM;
return (-1);
}
}

*pe->pe_oh = *oh;

pe->pe_flags |= LIBPE_F_DIRTY_OPT_HEADER;

return (0);
}

int
pe_update_data_dir(PE *pe, PE_DataDir *dd)
{

if (pe == NULL || dd == NULL) {
errno = EINVAL;
return (-1);
}

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (-1);
}

if (pe->pe_dd == NULL) {
if ((pe->pe_dd = malloc(sizeof(PE_DataDir))) == NULL) {
errno = ENOMEM;
return (-1);
}
}

*pe->pe_dd = *dd;

pe->pe_flags |= LIBPE_F_DIRTY_OPT_HEADER;

return (0);
}
