

























#include <assert.h>
#include <gelf.h>
#include <limits.h>
#include <stdint.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_dyn.c 3732 2019-04-22 11:08:38Z jkoshy $");

GElf_Dyn *
gelf_getdyn(Elf_Data *ed, int ndx, GElf_Dyn *dst)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
Elf32_Dyn *dyn32;
Elf64_Dyn *dyn64;
uint32_t sh_type;
struct _Libelf_Data *d;

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

if (_libelf_xlate_shtype(sh_type) != ELF_T_DYN) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((msz = _libelf_msize(ELF_T_DYN, ec, e->e_version)) == 0)
return (NULL);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if (ec == ELFCLASS32) {
dyn32 = (Elf32_Dyn *) d->d_data.d_buf + ndx;

dst->d_tag = dyn32->d_tag;
dst->d_un.d_val = (Elf64_Xword) dyn32->d_un.d_val;

} else {

dyn64 = (Elf64_Dyn *) d->d_data.d_buf + ndx;

*dst = *dyn64;
}

return (dst);
}

int
gelf_update_dyn(Elf_Data *ed, int ndx, GElf_Dyn *ds)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
Elf32_Dyn *dyn32;
Elf64_Dyn *dyn64;
uint32_t sh_type;
struct _Libelf_Data *d;

d = (struct _Libelf_Data *) ed;

if (d == NULL || ndx < 0 || ds == NULL ||
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

if (_libelf_xlate_shtype(sh_type) != ELF_T_DYN) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if ((msz = _libelf_msize(ELF_T_DYN, ec, e->e_version)) == 0)
return (0);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if (ec == ELFCLASS32) {
dyn32 = (Elf32_Dyn *) d->d_data.d_buf + ndx;

LIBELF_COPY_S32(dyn32, ds, d_tag);
LIBELF_COPY_U32(dyn32, ds, d_un.d_val);
} else {
dyn64 = (Elf64_Dyn *) d->d_data.d_buf + ndx;

*dyn64 = *ds;
}

return (1);
}
