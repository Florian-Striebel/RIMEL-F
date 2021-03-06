

























#include <assert.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: libelf_xlate.c 3732 2019-04-22 11:08:38Z jkoshy $");













Elf_Data *
_libelf_xlate(Elf_Data *dst, const Elf_Data *src, unsigned int encoding,
int elfclass, int elfmachine, int direction)
{
int byteswap;
size_t cnt, dsz, fsz, msz;
uintptr_t sb, se, db, de;
_libelf_translator_function *xlator;

if (encoding == ELFDATANONE)
encoding = LIBELF_PRIVATE(byteorder);

if ((encoding != ELFDATA2LSB && encoding != ELFDATA2MSB) ||
dst == NULL || src == NULL || dst == src) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

assert(elfclass == ELFCLASS32 || elfclass == ELFCLASS64);
assert(direction == ELF_TOFILE || direction == ELF_TOMEMORY);

if (dst->d_version != src->d_version) {
LIBELF_SET_ERROR(UNIMPL, 0);
return (NULL);
}

if (src->d_buf == NULL || dst->d_buf == NULL) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}

if ((int) src->d_type < 0 || src->d_type >= ELF_T_NUM) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}

if ((fsz = (elfclass == ELFCLASS32 ? elf32_fsize : elf64_fsize)
(src->d_type, (size_t) 1, src->d_version)) == 0)
return (NULL);

if ((msz = _libelf_msize(src->d_type, elfclass, src->d_version)) == 0)
return (NULL);

if (src->d_size % (direction == ELF_TOMEMORY ? fsz : msz)) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}






if (direction == ELF_TOMEMORY) {
cnt = (size_t) src->d_size / fsz;
dsz = cnt * msz;
} else {
cnt = (size_t) src->d_size / msz;
dsz = cnt * fsz;
}

if (dst->d_size < dsz) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}

sb = (uintptr_t) src->d_buf;
se = sb + (size_t) src->d_size;
db = (uintptr_t) dst->d_buf;
de = db + (size_t) dst->d_size;





if (db != sb && de > sb && se > db) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}

if ((direction == ELF_TOMEMORY ? db : sb) %
_libelf_malign(src->d_type, elfclass)) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}

dst->d_type = src->d_type;
dst->d_size = dsz;

byteswap = encoding != LIBELF_PRIVATE(byteorder);

if (src->d_size == 0 ||
(db == sb && !byteswap && fsz == msz))
return (dst);

xlator = _libelf_get_translator(src->d_type, direction, elfclass,
elfmachine);
if (!xlator(dst->d_buf, dsz, src->d_buf, cnt, byteswap)) {
LIBELF_SET_ERROR(DATA, 0);
return (NULL);
}

return (dst);
}
