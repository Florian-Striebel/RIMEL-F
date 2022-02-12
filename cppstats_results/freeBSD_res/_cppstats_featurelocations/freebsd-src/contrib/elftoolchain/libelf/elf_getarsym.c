

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_getarsym.c 3174 2015-03-27 17:13:41Z emaste $");

Elf_Arsym *
elf_getarsym(Elf *ar, size_t *ptr)
{
size_t n;
Elf_Arsym *symtab;

n = 0;
symtab = NULL;

if (ar == NULL || ar->e_kind != ELF_K_AR)
LIBELF_SET_ERROR(ARGUMENT, 0);
else if ((symtab = ar->e_u.e_ar.e_symtab) != NULL)
n = ar->e_u.e_ar.e_symtabsz;
else if (ar->e_u.e_ar.e_rawsymtab)
symtab = (ar->e_flags & LIBELF_F_AR_VARIANT_SVR4) ?
_libelf_ar_process_svr4_symtab(ar, &n) :
_libelf_ar_process_bsd_symtab(ar, &n);
else
LIBELF_SET_ERROR(ARCHIVE, 0);

if (ptr)
*ptr = n;
return (symtab);
}
