





























#include <assert.h>
#include <gelf.h>
#include <libelf.h>
#include <limits.h>
#include <stdint.h>

#include "_libelf.h"

Elf32_Chdr *
elf32_getchdr(Elf_Scn *s)
{
return (_libelf_getchdr(s, ELFCLASS32));
}

Elf64_Chdr *
elf64_getchdr(Elf_Scn *s)
{
return (_libelf_getchdr(s, ELFCLASS64));
}

GElf_Chdr *
gelf_getchdr(Elf_Scn *s, GElf_Chdr *d)
{
int ec;
void *ch;
Elf32_Chdr *ch32;
Elf64_Chdr *ch64;

if (d == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((ch = _libelf_getchdr(s, ELFCLASSNONE)) == NULL)
return (NULL);

ec = s->s_elf->e_class;
assert(ec == ELFCLASS32 || ec == ELFCLASS64);

if (ec == ELFCLASS32) {
ch32 = (Elf32_Chdr *)ch;

d->ch_type = (Elf64_Word)ch32->ch_type;
d->ch_size = (Elf64_Xword)ch32->ch_size;
d->ch_addralign = (Elf64_Xword)ch32->ch_addralign;
} else {
ch64 = (Elf64_Chdr *)ch;
*d = *ch64;
}

return (d);
}
