

























#include <errno.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_buffer.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

PE_Buffer *
pe_getbuffer(PE_Scn *ps, PE_Buffer *pb)
{
PE *pe;
PE_SecBuf *sb;

if (ps == NULL) {
errno = EINVAL;
return (NULL);
}

pe = ps->ps_pe;

if ((ps->ps_flags & LIBPE_F_LOAD_SECTION) == 0) {
if (pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (NULL);
}
if (pe->pe_flags & LIBPE_F_SPECIAL_FILE) {
if (libpe_load_all_sections(pe) < 0)
return (NULL);
} else {
if (libpe_load_section(pe, ps) < 0)
return (NULL);
}
}

sb = (PE_SecBuf *) pb;

if (sb == NULL)
sb = STAILQ_FIRST(&ps->ps_b);
else
sb = STAILQ_NEXT(sb, sb_next);

return ((PE_Buffer *) sb);
}

PE_Buffer *
pe_newbuffer(PE_Scn *ps)
{
PE *pe;
PE_SecBuf *sb;

if (ps == NULL) {
errno = EINVAL;
return (NULL);
}

pe = ps->ps_pe;

if (pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (NULL);
}

if ((ps->ps_flags & LIBPE_F_LOAD_SECTION) == 0) {
if (libpe_load_section(pe, ps) < 0)
return (NULL);
}

if ((sb = libpe_alloc_buffer(ps, 0)) == NULL)
return (NULL);

sb->sb_flags |= PE_F_DIRTY;
ps->ps_flags |= PE_F_DIRTY;

return ((PE_Buffer *) sb);
}
