

























#include <ar.h>
#include <assert.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_getident.c 3712 2019-03-16 22:23:34Z jkoshy $");

char *
elf_getident(Elf *e, size_t *sz)
{

if (e == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
goto error;
}

if (e->e_cmd == ELF_C_WRITE && e->e_rawfile == NULL) {
LIBELF_SET_ERROR(SEQUENCE, 0);
goto error;
}

assert(e->e_kind != ELF_K_AR || e->e_cmd == ELF_C_READ);

if (sz) {
if (e->e_kind == ELF_K_AR)
*sz = SARMAG;
else if (e->e_kind == ELF_K_ELF)
*sz = EI_NIDENT;
else
*sz = (size_t) e->e_rawsize;
}

return ((char *) e->e_rawfile);

error:
if (sz)
*sz = 0;
return (NULL);
}
