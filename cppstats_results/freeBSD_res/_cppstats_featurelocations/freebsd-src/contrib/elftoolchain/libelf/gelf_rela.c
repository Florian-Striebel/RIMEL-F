

























#include <assert.h>
#include <gelf.h>
#include <limits.h>
#include <stdint.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_rela.c 3739 2019-05-06 05:18:15Z jkoshy $");

GElf_Rela *
gelf_getrela(Elf_Data *ed, int ndx, GElf_Rela *dst)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
Elf32_Rela *rela32;
Elf64_Rela *rela64;
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

if (_libelf_xlate_shtype(sh_type) != ELF_T_RELA) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((msz = _libelf_msize(ELF_T_RELA, ec, e->e_version)) == 0)
return (NULL);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if (ec == ELFCLASS32) {
rela32 = (Elf32_Rela *) d->d_data.d_buf + ndx;

dst->r_offset = (Elf64_Addr) rela32->r_offset;
dst->r_info = ELF64_R_INFO(
(Elf64_Xword) ELF32_R_SYM(rela32->r_info),
ELF32_R_TYPE(rela32->r_info));
dst->r_addend = (Elf64_Sxword) rela32->r_addend;

} else {

rela64 = (Elf64_Rela *) d->d_data.d_buf + ndx;

*dst = *rela64;

if (_libelf_is_mips64el(e))
dst->r_info =
_libelf_mips64el_r_info_tom(rela64->r_info);
}

return (dst);
}

int
gelf_update_rela(Elf_Data *ed, int ndx, GElf_Rela *dr)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
Elf32_Rela *rela32;
Elf64_Rela *rela64;
struct _Libelf_Data *d;

d = (struct _Libelf_Data *) ed;

if (d == NULL || ndx < 0 || dr == NULL ||
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

if (_libelf_xlate_shtype(sh_type) != ELF_T_RELA) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if ((msz = _libelf_msize(ELF_T_RELA, ec, e->e_version)) == 0)
return (0);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if (ec == ELFCLASS32) {
rela32 = (Elf32_Rela *) d->d_data.d_buf + ndx;

LIBELF_COPY_U32(rela32, dr, r_offset);

if (ELF64_R_SYM(dr->r_info) > ELF32_R_SYM(~0U) ||
ELF64_R_TYPE(dr->r_info) > ELF32_R_TYPE(~0U)) {
LIBELF_SET_ERROR(RANGE, 0);
return (0);
}
rela32->r_info = ELF32_R_INFO(
(Elf32_Word) ELF64_R_SYM(dr->r_info),
(Elf32_Word) ELF64_R_TYPE(dr->r_info));

LIBELF_COPY_S32(rela32, dr, r_addend);
} else {
rela64 = (Elf64_Rela *) d->d_data.d_buf + ndx;

*rela64 = *dr;

if (_libelf_is_mips64el(e))
rela64->r_info = _libelf_mips64el_r_info_tof(dr->r_info);
}

return (1);
}
