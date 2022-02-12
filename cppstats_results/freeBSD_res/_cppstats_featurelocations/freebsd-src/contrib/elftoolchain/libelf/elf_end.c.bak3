

























#include <assert.h>
#include <libelf.h>
#include <stdlib.h>

#include "_libelf.h"

#if ELFTC_HAVE_MMAP
#include <sys/mman.h>
#endif

ELFTC_VCSID("$Id: elf_end.c 3738 2019-05-05 21:49:06Z jkoshy $");

int
elf_end(Elf *e)
{
Elf *sv;
Elf_Scn *scn, *tscn;

if (e == NULL || e->e_activations == 0)
return (0);

if (--e->e_activations > 0)
return (e->e_activations);

assert(e->e_activations == 0);

while (e && e->e_activations == 0) {
switch (e->e_kind) {
case ELF_K_AR:






if (e->e_u.e_ar.e_nchildren > 0)
return (0);
break;
case ELF_K_ELF:



RB_FOREACH_SAFE(scn, scntree, &e->e_u.e_elf.e_scn, tscn)
scn = _libelf_release_scn(scn);
break;
case ELF_K_NUM:
assert(0);
default:
break;
}

if (e->e_rawfile) {
if (e->e_flags & LIBELF_F_RAWFILE_MALLOC)
free(e->e_rawfile);
#if ELFTC_HAVE_MMAP
else if (e->e_flags & LIBELF_F_RAWFILE_MMAP)
(void) munmap(e->e_rawfile, (size_t) e->e_rawsize);
#endif
}

sv = e;
if ((e = e->e_parent) != NULL)
e->e_u.e_ar.e_nchildren--;
_libelf_release_elf(sv);
}

return (0);
}
