

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_getarhdr.c 3174 2015-03-27 17:13:41Z emaste $");

Elf_Arhdr *
elf_getarhdr(Elf *e)
{
if (e == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if (e->e_flags & LIBELF_F_AR_HEADER)
return (e->e_hdr.e_arhdr);

return (_libelf_ar_gethdr(e));
}
