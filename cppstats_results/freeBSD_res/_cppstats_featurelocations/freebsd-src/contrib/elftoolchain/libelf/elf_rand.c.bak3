

























#include <ar.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_rand.c 3716 2019-03-18 22:01:01Z jkoshy $");

off_t
elf_rand(Elf *ar, off_t offset)
{
struct ar_hdr *arh;
off_t offset_of_member;

if (ar == NULL || ar->e_kind != ELF_K_AR ||
(offset & 1) || offset < SARMAG ||
offset >= ar->e_rawsize) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return 0;
}

offset_of_member = offset + (off_t) sizeof(struct ar_hdr);

if (offset_of_member <= 0 ||
offset_of_member >= ar->e_rawsize) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return 0;
}

arh = (struct ar_hdr *) (ar->e_rawfile + offset);


if (arh->ar_fmag[0] != '`' || arh->ar_fmag[1] != '\n') {
LIBELF_SET_ERROR(ARCHIVE, 0);
return 0;
}

ar->e_u.e_ar.e_next = offset;

return (offset);
}
