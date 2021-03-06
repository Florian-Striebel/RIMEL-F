

























#include <assert.h>
#include <gelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_syminfo.c 3732 2019-04-22 11:08:38Z jkoshy $");

GElf_Syminfo *
gelf_getsyminfo(Elf_Data *ed, int ndx, GElf_Syminfo *dst)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
struct _Libelf_Data *d;
Elf32_Syminfo *syminfo32;
Elf64_Syminfo *syminfo64;

d = (struct _Libelf_Data *) ed;

if (d == NULL || ndx < 0 || dst == NULL ||
(scn = d->d_scn) == NULL ||
(e = scn->s_elf) == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

ec = e->e_class;
assert(ec == ELFCLASS32 || ec == ELFCLASS64);

if (ec == ELFCLASS32)
sh_type = scn->s_shdr.s_shdr32.sh_type;
else
sh_type = scn->s_shdr.s_shdr64.sh_type;

if (_libelf_xlate_shtype(sh_type) != ELF_T_SYMINFO) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((msz = _libelf_msize(ELF_T_SYMINFO, ec, e->e_version)) == 0)
return (NULL);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if (ec == ELFCLASS32) {

syminfo32 = (Elf32_Syminfo *) d->d_data.d_buf + ndx;

dst->si_boundto = syminfo32->si_boundto;
dst->si_flags = syminfo32->si_flags;

} else {

syminfo64 = (Elf64_Syminfo *) d->d_data.d_buf + ndx;

*dst = *syminfo64;
}

return (dst);
}

int
gelf_update_syminfo(Elf_Data *ed, int ndx, GElf_Syminfo *gs)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
struct _Libelf_Data *d;
Elf32_Syminfo *syminfo32;
Elf64_Syminfo *syminfo64;

d = (struct _Libelf_Data *) ed;

if (d == NULL || ndx < 0 || gs == NULL ||
(scn = d->d_scn) == NULL ||
(e = scn->s_elf) == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

ec = e->e_class;
assert(ec == ELFCLASS32 || ec == ELFCLASS64);

if (ec == ELFCLASS32)
sh_type = scn->s_shdr.s_shdr32.sh_type;
else
sh_type = scn->s_shdr.s_shdr64.sh_type;

if (_libelf_xlate_shtype(sh_type) != ELF_T_SYMINFO) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if ((msz = _libelf_msize(ELF_T_SYMINFO, ec, e->e_version)) == 0)
return (0);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if (ec == ELFCLASS32) {
syminfo32 = (Elf32_Syminfo *) d->d_data.d_buf + ndx;

syminfo32->si_boundto = gs->si_boundto;
syminfo32->si_flags = gs->si_flags;

} else {
syminfo64 = (Elf64_Syminfo *) d->d_data.d_buf + ndx;

*syminfo64 = *gs;
}

return (1);
}
