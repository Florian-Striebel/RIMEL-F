

























#include <gelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: libelf_checksum.c 3174 2015-03-27 17:13:41Z emaste $");

static unsigned long
_libelf_sum(unsigned long c, const unsigned char *s, size_t size)
{
if (s == NULL || size == 0)
return (c);

while (size--)
c += *s++;

return (c);
}

long
_libelf_checksum(Elf *e, int elfclass)
{
size_t shn;
Elf_Scn *scn;
Elf_Data *d;
unsigned long checksum;
GElf_Ehdr eh;
GElf_Shdr shdr;

if (e == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0L);
}

if (e->e_class != elfclass) {
LIBELF_SET_ERROR(CLASS, 0);
return (0L);
}

if (gelf_getehdr(e, &eh) == NULL)
return (0);










checksum = 0;
for (shn = 1; shn < e->e_u.e_elf.e_nscn; shn++) {
if ((scn = elf_getscn(e, shn)) == NULL)
return (0);
if (gelf_getshdr(scn, &shdr) == NULL)
return (0);
if ((shdr.sh_flags & SHF_ALLOC) == 0 ||
shdr.sh_type == SHT_DYNAMIC ||
shdr.sh_type == SHT_DYNSYM)
continue;

d = NULL;
while ((d = elf_rawdata(scn, d)) != NULL)
checksum = _libelf_sum(checksum,
(unsigned char *) d->d_buf, (size_t) d->d_size);
}




return (long) (((checksum >> 16) & 0xFFFFUL) + (checksum & 0xFFFFUL));
}
