

























#include <ar.h>
#include <assert.h>
#include <string.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: libelf_memory.c 3738 2019-05-05 21:49:06Z jkoshy $");






Elf *
_libelf_memory(unsigned char *image, size_t sz, int reporterror)
{
Elf *e;
int e_class;
enum Elf_Error error;
unsigned int e_byteorder, e_version;

assert(image != NULL);
assert(sz > 0);

if ((e = _libelf_allocate_elf()) == NULL)
return (NULL);

e->e_cmd = ELF_C_READ;
e->e_rawfile = image;
e->e_rawsize = (off_t) sz;

#undef LIBELF_IS_ELF
#define LIBELF_IS_ELF(P) ((P)[EI_MAG0] == ELFMAG0 && (P)[EI_MAG1] == ELFMAG1 && (P)[EI_MAG2] == ELFMAG2 && (P)[EI_MAG3] == ELFMAG3)



if (sz > EI_NIDENT && LIBELF_IS_ELF(image)) {
e_byteorder = image[EI_DATA];
e_class = image[EI_CLASS];
e_version = image[EI_VERSION];

error = ELF_E_NONE;

if (e_version > EV_CURRENT)
error = ELF_E_VERSION;
else if ((e_byteorder != ELFDATA2LSB && e_byteorder !=
ELFDATA2MSB) || (e_class != ELFCLASS32 && e_class !=
ELFCLASS64))
error = ELF_E_HEADER;

if (error != ELF_E_NONE) {
if (reporterror) {
LIBELF_PRIVATE(error) = LIBELF_ERROR(error, 0);
_libelf_release_elf(e);
return (NULL);
}
} else {
_libelf_init_elf(e, ELF_K_ELF);

e->e_byteorder = e_byteorder;
e->e_class = e_class;
e->e_version = e_version;
}
} else if (sz >= SARMAG &&
strncmp((const char *) image, ARMAG, (size_t) SARMAG) == 0)
return (_libelf_ar_open(e, reporterror));

return (e);
}
