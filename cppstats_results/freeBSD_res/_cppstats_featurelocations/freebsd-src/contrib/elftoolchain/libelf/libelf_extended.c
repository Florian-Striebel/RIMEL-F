

























#include <assert.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: libelf_extended.c 3712 2019-03-16 22:23:34Z jkoshy $");




static Elf_Scn *
_libelf_getscn0(Elf *e)
{
Elf_Scn *s;

if ((s = RB_MIN(scntree, &e->e_u.e_elf.e_scn)) != NULL)
return (s);

return (_libelf_allocate_scn(e, (size_t) SHN_UNDEF));
}

int
_libelf_setshnum(Elf *e, void *eh, int ec, size_t shnum)
{
Elf_Scn *scn;

if (shnum >= SHN_LORESERVE) {
if ((scn = _libelf_getscn0(e)) == NULL)
return (0);

assert(scn->s_ndx == SHN_UNDEF);

if (ec == ELFCLASS32)
scn->s_shdr.s_shdr32.sh_size = (Elf32_Word) shnum;
else
scn->s_shdr.s_shdr64.sh_size = shnum;

(void) elf_flagshdr(scn, ELF_C_SET, ELF_F_DIRTY);

shnum = 0;
}

if (ec == ELFCLASS32)
((Elf32_Ehdr *) eh)->e_shnum = shnum & 0xFFFFU;
else
((Elf64_Ehdr *) eh)->e_shnum = shnum & 0xFFFFU;


return (1);
}

int
_libelf_setshstrndx(Elf *e, void *eh, int ec, size_t shstrndx)
{
Elf_Scn *scn;

if (shstrndx >= SHN_LORESERVE) {
if ((scn = _libelf_getscn0(e)) == NULL)
return (0);

assert(scn->s_ndx == SHN_UNDEF);

if (ec == ELFCLASS32)
scn->s_shdr.s_shdr32.sh_link = (Elf32_Word) shstrndx;
else
scn->s_shdr.s_shdr64.sh_link = (Elf64_Word) shstrndx;

(void) elf_flagshdr(scn, ELF_C_SET, ELF_F_DIRTY);

shstrndx = SHN_XINDEX;
}

if (ec == ELFCLASS32)
((Elf32_Ehdr *) eh)->e_shstrndx = shstrndx & 0xFFFFU;
else
((Elf64_Ehdr *) eh)->e_shstrndx = shstrndx & 0xFFFFU;

return (1);
}

int
_libelf_setphnum(Elf *e, void *eh, int ec, size_t phnum)
{
Elf_Scn *scn;

if (phnum >= PN_XNUM) {
if ((scn = _libelf_getscn0(e)) == NULL)
return (0);

assert(scn->s_ndx == SHN_UNDEF);

if (ec == ELFCLASS32)
scn->s_shdr.s_shdr32.sh_info = (Elf32_Word) phnum;
else
scn->s_shdr.s_shdr64.sh_info = (Elf64_Word) phnum;

(void) elf_flagshdr(scn, ELF_C_SET, ELF_F_DIRTY);

phnum = PN_XNUM;
}

if (ec == ELFCLASS32)
((Elf32_Ehdr *) eh)->e_phnum = phnum & 0xFFFFU;
else
((Elf64_Ehdr *) eh)->e_phnum = phnum & 0xFFFFU;

return (1);
}
