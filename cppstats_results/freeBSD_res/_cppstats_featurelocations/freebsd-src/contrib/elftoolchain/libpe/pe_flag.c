

























#include <errno.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_flag.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

int
pe_flag(PE *pe, PE_Cmd c, unsigned int flags)
{

if (pe == NULL || (c != PE_C_SET && c != PE_C_CLR)) {
errno = EINVAL;
return (-1);
}

if ((flags & ~(PE_F_STRIP_DOS_STUB | PE_F_STRIP_RICH_HEADER |
PE_F_STRIP_SYMTAB | PE_F_STRIP_DEBUG)) != 0) {
errno = EINVAL;
return (-1);
}

if (c == PE_C_SET)
pe->pe_flags |= flags;
else
pe->pe_flags &= ~flags;

return (0);
}

int
pe_flag_dos_header(PE *pe, PE_Cmd c, unsigned int flags)
{

if (pe == NULL || (c != PE_C_SET && c != PE_C_CLR) ||
(flags & ~PE_F_DIRTY) != 0) {
errno = EINVAL;
return (-1);
}

if (c == PE_C_SET)
pe->pe_flags |= LIBPE_F_DIRTY_DOS_HEADER;
else
pe->pe_flags &= ~LIBPE_F_DIRTY_DOS_HEADER;

return (0);
}

int
pe_flag_coff_header(PE *pe, PE_Cmd c, unsigned int flags)
{

if (pe == NULL || (c != PE_C_SET && c != PE_C_CLR) ||
(flags & ~PE_F_DIRTY) != 0) {
errno = EINVAL;
return (-1);
}

if (c == PE_C_SET)
pe->pe_flags |= LIBPE_F_DIRTY_COFF_HEADER;
else
pe->pe_flags &= ~LIBPE_F_DIRTY_COFF_HEADER;

return (0);
}

int
pe_flag_opt_header(PE *pe, PE_Cmd c, unsigned int flags)
{

if (pe == NULL || (c != PE_C_SET && c != PE_C_CLR) ||
(flags & ~PE_F_DIRTY) != 0) {
errno = EINVAL;
return (-1);
}

if (c == PE_C_SET)
pe->pe_flags |= LIBPE_F_DIRTY_OPT_HEADER;
else
pe->pe_flags &= ~LIBPE_F_DIRTY_OPT_HEADER;

return (0);
}

int
pe_flag_data_dir(PE *pe, PE_Cmd c, unsigned int flags)
{

if (pe == NULL || (c != PE_C_SET && c != PE_C_CLR) ||
(flags & ~PE_F_DIRTY) != 0) {
errno = EINVAL;
return (-1);
}

if (c == PE_C_SET)
pe->pe_flags |= LIBPE_F_DIRTY_OPT_HEADER;
else
pe->pe_flags &= ~LIBPE_F_DIRTY_OPT_HEADER;

return (0);
}

int
pe_flag_scn(PE_Scn *ps, PE_Cmd c, unsigned int flags)
{

if (ps == NULL || (c != PE_C_SET && c != PE_C_CLR) ||
(flags & ~(PE_F_DIRTY | PE_F_STRIP_SECTION)) == 0) {
errno = EINVAL;
return (-1);
}

if (c == PE_C_SET)
ps->ps_flags |= flags;
else
ps->ps_flags &= ~flags;

return (0);
}

int
pe_flag_section_header(PE_Scn *ps, PE_Cmd c, unsigned int flags)
{
PE *pe;

if (ps == NULL || (c != PE_C_SET && c != PE_C_CLR) ||
(flags & ~PE_F_DIRTY) != 0) {
errno = EINVAL;
return (-1);
}

pe = ps->ps_pe;


if (c == PE_C_SET)
pe->pe_flags |= LIBPE_F_DIRTY_SEC_HEADER;
else
pe->pe_flags &= ~LIBPE_F_DIRTY_SEC_HEADER;

return (0);
}

int
pe_flag_buffer(PE_Buffer *pb, PE_Cmd c, unsigned int flags)
{
PE_SecBuf *sb;

if (pb == NULL || (c != PE_C_SET && c != PE_C_CLR) ||
(flags & ~PE_F_DIRTY) != 0) {
errno = EINVAL;
return (-1);
}

sb = (PE_SecBuf *) pb;

if (c == PE_C_SET)
sb->sb_flags |= flags;
else
sb->sb_flags &= ~flags;

return (0);
}
