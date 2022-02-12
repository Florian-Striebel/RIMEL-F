

























#include <assert.h>
#include <gelf.h>
#include <libelf.h>
#include <stdlib.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: libelf_phdr.c 3732 2019-04-22 11:08:38Z jkoshy $");

void *
_libelf_getphdr(Elf *e, int ec)
{
size_t phnum;
size_t fsz, msz;
uint64_t phoff;
Elf32_Ehdr *eh32;
Elf64_Ehdr *eh64;
void *ehdr, *phdr;
_libelf_translator_function *xlator;

assert(ec == ELFCLASS32 || ec == ELFCLASS64);

if (e == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((phdr = (ec == ELFCLASS32 ?
(void *) e->e_u.e_elf.e_phdr.e_phdr32 :
(void *) e->e_u.e_elf.e_phdr.e_phdr64)) != NULL)
return (phdr);





if ((ehdr = _libelf_ehdr(e, ec, 0)) == NULL)
return (NULL);

phnum = e->e_u.e_elf.e_nphdr;

if (ec == ELFCLASS32) {
eh32 = (Elf32_Ehdr *) ehdr;
phoff = (uint64_t) eh32->e_phoff;
} else {
eh64 = (Elf64_Ehdr *) ehdr;
phoff = (uint64_t) eh64->e_phoff;
}

fsz = gelf_fsize(e, ELF_T_PHDR, phnum, e->e_version);

assert(fsz > 0);

if (phoff + fsz < phoff) {
LIBELF_SET_ERROR(HEADER, 0);
return (NULL);
}

if ((uint64_t) e->e_rawsize < (phoff + fsz)) {
LIBELF_SET_ERROR(HEADER, 0);
return (NULL);
}

if ((msz = _libelf_msize(ELF_T_PHDR, ec, EV_CURRENT)) == 0)
return (NULL);

if ((phdr = calloc(phnum, msz)) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}

if (ec == ELFCLASS32)
e->e_u.e_elf.e_phdr.e_phdr32 = phdr;
else
e->e_u.e_elf.e_phdr.e_phdr64 = phdr;


xlator = _libelf_get_translator(ELF_T_PHDR, ELF_TOMEMORY, ec,
_libelf_elfmachine(e));
(*xlator)(phdr, phnum * msz, e->e_rawfile + phoff, phnum,
e->e_byteorder != LIBELF_PRIVATE(byteorder));

return (phdr);
}

void *
_libelf_newphdr(Elf *e, int ec, size_t count)
{
void *ehdr, *newphdr, *oldphdr;
size_t msz;

if (e == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((ehdr = _libelf_ehdr(e, ec, 0)) == NULL) {
LIBELF_SET_ERROR(SEQUENCE, 0);
return (NULL);
}

assert(e->e_class == ec);
assert(ec == ELFCLASS32 || ec == ELFCLASS64);
assert(e->e_version == EV_CURRENT);

if ((msz = _libelf_msize(ELF_T_PHDR, ec, e->e_version)) == 0)
return (NULL);

newphdr = NULL;
if (count > 0 && (newphdr = calloc(count, msz)) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}

if (ec == ELFCLASS32) {
if ((oldphdr = (void *) e->e_u.e_elf.e_phdr.e_phdr32) != NULL)
free(oldphdr);
e->e_u.e_elf.e_phdr.e_phdr32 = (Elf32_Phdr *) newphdr;
} else {
if ((oldphdr = (void *) e->e_u.e_elf.e_phdr.e_phdr64) != NULL)
free(oldphdr);
e->e_u.e_elf.e_phdr.e_phdr64 = (Elf64_Phdr *) newphdr;
}

e->e_u.e_elf.e_nphdr = count;

elf_flagphdr(e, ELF_C_SET, ELF_F_DIRTY);

return (newphdr);
}
