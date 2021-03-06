

























#include <gelf.h>
#include <libelf.h>
#include <string.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_xlate.c 3632 2018-10-10 21:12:43Z jkoshy $");

Elf_Data *
elf32_xlatetof(Elf_Data *dst, const Elf_Data *src, unsigned int encoding)
{
return _libelf_xlate(dst, src, encoding, ELFCLASS32, EM_NONE,
ELF_TOFILE);
}

Elf_Data *
elf64_xlatetof(Elf_Data *dst, const Elf_Data *src, unsigned int encoding)
{
return _libelf_xlate(dst, src, encoding, ELFCLASS64, EM_NONE,
ELF_TOFILE);
}

Elf_Data *
elf32_xlatetom(Elf_Data *dst, const Elf_Data *src, unsigned int encoding)
{
return _libelf_xlate(dst, src, encoding, ELFCLASS32, EM_NONE,
ELF_TOMEMORY);
}

Elf_Data *
elf64_xlatetom(Elf_Data *dst, const Elf_Data *src, unsigned int encoding)
{
return _libelf_xlate(dst, src, encoding, ELFCLASS64, EM_NONE,
ELF_TOMEMORY);
}

Elf_Data *
gelf_xlatetom(Elf *e, Elf_Data *dst, const Elf_Data *src,
unsigned int encoding)
{
if (e != NULL)
return (_libelf_xlate(dst, src, encoding, e->e_class,
_libelf_elfmachine(e), ELF_TOMEMORY));
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

Elf_Data *
gelf_xlatetof(Elf *e, Elf_Data *dst, const Elf_Data *src,
unsigned int encoding)
{
if (e != NULL)
return (_libelf_xlate(dst, src, encoding, e->e_class,
_libelf_elfmachine(e), ELF_TOFILE));
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}
