

























#include <sys/param.h>

#include <assert.h>
#include <gelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_strptr.c 2990 2014-03-17 09:56:58Z jkoshy $");





char *
elf_strptr(Elf *e, size_t scndx, size_t offset)
{
Elf_Scn *s;
Elf_Data *d;
GElf_Shdr shdr;
uint64_t alignment, count;

if (e == NULL || e->e_kind != ELF_K_ELF) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((s = elf_getscn(e, scndx)) == NULL ||
gelf_getshdr(s, &shdr) == NULL)
return (NULL);

if (shdr.sh_type != SHT_STRTAB ||
offset >= shdr.sh_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

d = NULL;
if (e->e_flags & ELF_F_LAYOUT) {







while ((d = elf_getdata(s, d)) != NULL) {

if (d->d_buf == 0 || d->d_size == 0)
continue;

if (d->d_type != ELF_T_BYTE) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}

if (offset >= d->d_off &&
offset < d->d_off + d->d_size)
return ((char *) d->d_buf + offset - d->d_off);
}
} else {






count = (uint64_t) 0;
while ((d = elf_getdata(s, d)) != NULL && count <= offset) {

if (d->d_buf == NULL || d->d_size == 0)
continue;

if (d->d_type != ELF_T_BYTE) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}

if ((alignment = d->d_align) > 1) {
if ((alignment & (alignment - 1)) != 0) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}
count = roundup2(count, alignment);
}

if (offset < count) {

LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if (offset < count + d->d_size) {
if (d->d_buf != NULL)
return ((char *) d->d_buf +
offset - count);
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}

count += d->d_size;
}
}

LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}
