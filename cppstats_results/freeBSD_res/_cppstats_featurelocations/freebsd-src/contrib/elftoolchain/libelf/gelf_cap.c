

























#include <assert.h>
#include <gelf.h>
#include <limits.h>
#include <stdint.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_cap.c 3732 2019-04-22 11:08:38Z jkoshy $");

GElf_Cap *
gelf_getcap(Elf_Data *ed, int ndx, GElf_Cap *dst)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
Elf32_Cap *cap32;
Elf64_Cap *cap64;
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

if (_libelf_xlate_shtype(sh_type) != ELF_T_CAP) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((msz = _libelf_msize(ELF_T_CAP, ec, e->e_version)) == 0)
return (NULL);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if (ec == ELFCLASS32) {

cap32 = (Elf32_Cap *) d->d_data.d_buf + ndx;

dst->c_tag = cap32->c_tag;
dst->c_un.c_val = (Elf64_Xword) cap32->c_un.c_val;

} else {

cap64 = (Elf64_Cap *) d->d_data.d_buf + ndx;

*dst = *cap64;
}

return (dst);
}

int
gelf_update_cap(Elf_Data *ed, int ndx, GElf_Cap *gc)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
Elf32_Cap *cap32;
Elf64_Cap *cap64;
uint32_t sh_type;
struct _Libelf_Data *d;

d = (struct _Libelf_Data *) ed;

if (d == NULL || ndx < 0 || gc == NULL ||
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

if (_libelf_xlate_shtype(sh_type) != ELF_T_CAP) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if ((msz = _libelf_msize(ELF_T_CAP, ec, e->e_version)) == 0)
return (0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if (ec == ELFCLASS32) {
cap32 = (Elf32_Cap *) d->d_data.d_buf + ndx;

LIBELF_COPY_U32(cap32, gc, c_tag);
LIBELF_COPY_U32(cap32, gc, c_un.c_val);
} else {
cap64 = (Elf64_Cap *) d->d_data.d_buf + ndx;

*cap64 = *gc;
}

return (1);
}
