

























#include <errno.h>
#include <string.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_section.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

PE_Scn *
pe_getscn(PE *pe, size_t ndx)
{
PE_Scn *ps;

if (pe == NULL || ndx < 1 || ndx > 0xFFFFU) {
errno = EINVAL;
return (NULL);
}

STAILQ_FOREACH(ps, &pe->pe_scn, ps_next) {
if (ps->ps_ndx == ndx)
return (ps);
}

errno = ENOENT;

return (NULL);
}

size_t
pe_ndxscn(PE_Scn *ps)
{

if (ps == NULL) {
errno = EINVAL;
return (0);
}

return (ps->ps_ndx);
}

PE_Scn *
pe_nextscn(PE *pe, PE_Scn *ps)
{

if (pe == NULL) {
errno = EINVAL;
return (NULL);
}

if (ps == NULL)
ps = STAILQ_FIRST(&pe->pe_scn);
else
ps = STAILQ_NEXT(ps, ps_next);

while (ps != NULL) {
if (ps->ps_ndx >= 1 && ps->ps_ndx <= 0xFFFFU)
return (ps);
ps = STAILQ_NEXT(ps, ps_next);
}

return (NULL);
}

PE_Scn *
pe_newscn(PE *pe)
{
PE_Scn *ps, *tps, *_tps;

if (pe == NULL) {
errno = EINVAL;
return (NULL);
}

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (NULL);
}

if ((ps = libpe_alloc_scn(pe)) == NULL)
return (NULL);

if (pe->pe_flags & LIBPE_F_BAD_SEC_HEADER) {
STAILQ_FOREACH_SAFE(tps, &pe->pe_scn, ps_next, _tps)
libpe_release_scn(tps);
pe->pe_flags &= ~LIBPE_F_BAD_SEC_HEADER;
}

STAILQ_INSERT_TAIL(&pe->pe_scn, ps, ps_next);

ps->ps_flags |= PE_F_DIRTY | LIBPE_F_LOAD_SECTION;
pe->pe_flags |= LIBPE_F_DIRTY_SEC_HEADER;

return (ps);
}

PE_Scn *
pe_insertscn(PE *pe, size_t ndx)
{
PE_Scn *ps, *a, *b;

if (pe == NULL || ndx < 1 || ndx > 0xFFFFU) {
errno = EINVAL;
return (NULL);
}

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (NULL);
}

if ((ps = libpe_alloc_scn(pe)) == NULL)
return (NULL);

if (pe->pe_flags & LIBPE_F_BAD_SEC_HEADER) {
STAILQ_FOREACH_SAFE(a, &pe->pe_scn, ps_next, b)
libpe_release_scn(a);
pe->pe_flags &= ~LIBPE_F_BAD_SEC_HEADER;
}

b = NULL;
STAILQ_FOREACH(a, &pe->pe_scn, ps_next) {
if (a->ps_ndx & 0xFFFF0000U)
continue;
if (a->ps_ndx == ndx)
break;
b = a;
}

if (a == NULL) {
STAILQ_INSERT_TAIL(&pe->pe_scn, ps, ps_next);
if (b == NULL)
ps->ps_ndx = 1;
else
ps->ps_ndx = b->ps_ndx + 1;
} else if (b == NULL) {
STAILQ_INSERT_HEAD(&pe->pe_scn, ps, ps_next);
ps->ps_ndx = 1;
} else {
STAILQ_INSERT_AFTER(&pe->pe_scn, b, ps, ps_next);
ps->ps_ndx = ndx;
}

a = ps;
while ((a = STAILQ_NEXT(a, ps_next)) != NULL) {
if ((a->ps_ndx & 0xFFFF0000U) == 0)
a->ps_ndx++;
}

ps->ps_flags |= PE_F_DIRTY | LIBPE_F_LOAD_SECTION;
pe->pe_flags |= LIBPE_F_DIRTY_SEC_HEADER;

return (ps);
}

PE_SecHdr *
pe_section_header(PE_Scn *ps)
{

if (ps == NULL) {
errno = EINVAL;
return (NULL);
}

return (&ps->ps_sh);
}

int
pe_update_section_header(PE_Scn *ps, PE_SecHdr *sh)
{
PE *pe;

if (ps == NULL || sh == NULL) {
errno = EINVAL;
return (-1);
}

pe = ps->ps_pe;

if (pe->pe_cmd == PE_C_READ || pe->pe_flags & LIBPE_F_FD_DONE) {
errno = EACCES;
return (-1);
}

ps->ps_sh = *sh;
pe->pe_flags |= LIBPE_F_DIRTY_SEC_HEADER;

return (0);
}
