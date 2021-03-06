

























#include <assert.h>
#include <gelf.h>
#include <limits.h>
#include <stdint.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_sym.c 3732 2019-04-22 11:08:38Z jkoshy $");

GElf_Sym *
gelf_getsym(Elf_Data *ed, int ndx, GElf_Sym *dst)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
Elf32_Sym *sym32;
Elf64_Sym *sym64;
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

if (_libelf_xlate_shtype(sh_type) != ELF_T_SYM) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((msz = _libelf_msize(ELF_T_SYM, ec, e->e_version)) == 0)
return (NULL);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if (ec == ELFCLASS32) {
sym32 = (Elf32_Sym *) d->d_data.d_buf + ndx;

dst->st_name = sym32->st_name;
dst->st_value = (Elf64_Addr) sym32->st_value;
dst->st_size = (Elf64_Xword) sym32->st_size;
dst->st_info = sym32->st_info;
dst->st_other = sym32->st_other;
dst->st_shndx = sym32->st_shndx;
} else {
sym64 = (Elf64_Sym *) d->d_data.d_buf + ndx;

*dst = *sym64;
}

return (dst);
}

int
gelf_update_sym(Elf_Data *ed, int ndx, GElf_Sym *gs)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
Elf32_Sym *sym32;
Elf64_Sym *sym64;
struct _Libelf_Data *d;

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

if (_libelf_xlate_shtype(sh_type) != ELF_T_SYM) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if ((msz = _libelf_msize(ELF_T_SYM, ec, e->e_version)) == 0)
return (0);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if (ec == ELFCLASS32) {
sym32 = (Elf32_Sym *) d->d_data.d_buf + ndx;

sym32->st_name = gs->st_name;
sym32->st_info = gs->st_info;
sym32->st_other = gs->st_other;
sym32->st_shndx = gs->st_shndx;

LIBELF_COPY_U32(sym32, gs, st_value);
LIBELF_COPY_U32(sym32, gs, st_size);
} else {
sym64 = (Elf64_Sym *) d->d_data.d_buf + ndx;

*sym64 = *gs;
}

return (1);
}
