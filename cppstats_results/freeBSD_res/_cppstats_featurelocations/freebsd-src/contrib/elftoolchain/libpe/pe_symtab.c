

























#include <errno.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_symtab.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

int
pe_update_symtab(PE *pe, char *symtab, size_t sz, unsigned int nsym)
{
PE_Scn *ps, *pstmp;
PE_SecBuf *sb;
PE_SecHdr *sh;

if (pe == NULL || symtab == NULL || sz == 0) {
errno = EINVAL;
return (-1);
}

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (-1);
}


STAILQ_FOREACH_SAFE(ps, &pe->pe_scn, ps_next, pstmp) {
if (ps->ps_ndx == 0xFFFFFFFFU)
libpe_release_scn(ps);
}





if ((ps = libpe_alloc_scn(pe)) == NULL)
return (-1);

STAILQ_INSERT_TAIL(&pe->pe_scn, ps, ps_next);
ps->ps_ndx = 0xFFFFFFFFU;
ps->ps_flags |= PE_F_DIRTY;






sh = &ps->ps_sh;
sh->sh_rawptr = 0xFFFFFFFFU;
sh->sh_rawsize = sz;


if ((sb = libpe_alloc_buffer(ps, 0)) == NULL)
return (-1);
sb->sb_flags |= PE_F_DIRTY;
sb->sb_pb.pb_size = sz;
sb->sb_pb.pb_buf = symtab;

pe->pe_nsym = nsym;

return (0);
}
