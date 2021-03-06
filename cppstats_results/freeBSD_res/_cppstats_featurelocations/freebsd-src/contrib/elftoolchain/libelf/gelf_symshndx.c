

























#include <assert.h>
#include <gelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_symshndx.c 3732 2019-04-22 11:08:38Z jkoshy $");

GElf_Sym *
gelf_getsymshndx(Elf_Data *d, Elf_Data *id, int ndx, GElf_Sym *dst,
Elf32_Word *shindex)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
struct _Libelf_Data *ld, *lid;

ld = (struct _Libelf_Data *) d;
lid = (struct _Libelf_Data *) id;

if (gelf_getsym(d, ndx, dst) == 0)
return (NULL);

if (lid == NULL || (scn = lid->d_scn) == NULL ||
(e = scn->s_elf) == NULL || (e != ld->d_scn->s_elf) ||
shindex == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

ec = e->e_class;
assert(ec == ELFCLASS32 || ec == ELFCLASS64);

if (ec == ELFCLASS32)
sh_type = scn->s_shdr.s_shdr32.sh_type;
else
sh_type = scn->s_shdr.s_shdr64.sh_type;

if (_libelf_xlate_shtype(sh_type) != ELF_T_WORD ||
id->d_type != ELF_T_WORD) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((msz = _libelf_msize(ELF_T_WORD, ec, e->e_version)) == 0)
return (NULL);

assert(ndx >= 0);

if (msz * (size_t) ndx >= id->d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

*shindex = ((Elf32_Word *) id->d_buf)[ndx];

return (dst);
}

int
gelf_update_symshndx(Elf_Data *d, Elf_Data *id, int ndx, GElf_Sym *gs,
Elf32_Word xindex)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
struct _Libelf_Data *ld, *lid;

ld = (struct _Libelf_Data *) d;
lid = (struct _Libelf_Data *) id;

if (gelf_update_sym(d, ndx, gs) == 0)
return (0);

if (lid == NULL || (scn = lid->d_scn) == NULL ||
(e = scn->s_elf) == NULL || (e != ld->d_scn->s_elf)) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

ec = e->e_class;
assert(ec == ELFCLASS32 || ec == ELFCLASS64);

if (ec == ELFCLASS32)
sh_type = scn->s_shdr.s_shdr32.sh_type;
else
sh_type = scn->s_shdr.s_shdr64.sh_type;

if (_libelf_xlate_shtype(sh_type) != ELF_T_WORD ||
d->d_type != ELF_T_WORD) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if ((msz = _libelf_msize(ELF_T_WORD, ec, e->e_version)) == 0)
return (0);

assert(ndx >= 0);

if (msz * (size_t) ndx >= id->d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

*(((Elf32_Word *) id->d_buf) + ndx) = xindex;

return (1);
}
