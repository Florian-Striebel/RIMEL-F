

























#include <ar.h>
#include <assert.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_next.c 3710 2019-03-12 09:42:35Z jkoshy $");

Elf_Cmd
elf_next(Elf *e)
{
off_t next;
Elf *parent;

if (e == NULL)
return (ELF_C_NULL);

if ((parent = e->e_parent) == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (ELF_C_NULL);
}

assert(parent->e_kind == ELF_K_AR);
assert(parent->e_cmd == ELF_C_READ);
assert(e->e_rawfile > parent->e_rawfile);

next = e->e_rawfile - parent->e_rawfile + (off_t) e->e_rawsize;
next = (next + 1) & ~1;





parent->e_u.e_ar.e_next = (next >= (off_t) parent->e_rawsize) ?
(off_t) 0 : next;










if (next > (off_t) parent->e_rawsize) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (ELF_C_NULL);
}

return (ELF_C_READ);
}
