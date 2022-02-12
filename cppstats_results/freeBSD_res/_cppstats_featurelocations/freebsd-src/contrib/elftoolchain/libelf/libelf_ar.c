

























#include <assert.h>
#include <ctype.h>
#include <libelf.h>
#include <stdlib.h>
#include <string.h>

#include "_libelf.h"
#include "_libelf_ar.h"

ELFTC_VCSID("$Id: libelf_ar.c 3712 2019-03-16 22:23:34Z jkoshy $");

#define LIBELF_NALLOC_SIZE 16




































































Elf_Arhdr *
_libelf_ar_gethdr(Elf *e)
{
Elf *parent;
Elf_Arhdr *eh;
char *namelen;
size_t n, nlen;
struct ar_hdr *arh;

if ((parent = e->e_parent) == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

assert((e->e_flags & LIBELF_F_AR_HEADER) == 0);

arh = (struct ar_hdr *) (uintptr_t) e->e_hdr.e_rawhdr;

assert((uintptr_t) arh >= (uintptr_t) parent->e_rawfile + SARMAG);





if ((uintptr_t) arh > (uintptr_t) parent->e_rawfile +
(uintptr_t) parent->e_rawsize - sizeof(struct ar_hdr)) {
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}

if ((eh = malloc(sizeof(Elf_Arhdr))) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}

e->e_hdr.e_arhdr = eh;
e->e_flags |= LIBELF_F_AR_HEADER;

eh->ar_name = eh->ar_rawname = NULL;

if ((eh->ar_name = _libelf_ar_get_translated_name(arh, parent)) ==
NULL)
goto error;

if (_libelf_ar_get_number(arh->ar_uid, sizeof(arh->ar_uid), 10,
&n) == 0)
goto error;
eh->ar_uid = (uid_t) n;

if (_libelf_ar_get_number(arh->ar_gid, sizeof(arh->ar_gid), 10,
&n) == 0)
goto error;
eh->ar_gid = (gid_t) n;

if (_libelf_ar_get_number(arh->ar_mode, sizeof(arh->ar_mode), 8,
&n) == 0)
goto error;
eh->ar_mode = (mode_t) n;

if (_libelf_ar_get_number(arh->ar_size, sizeof(arh->ar_size), 10,
&n) == 0)
goto error;




if (IS_EXTENDED_BSD_NAME(arh->ar_name)) {
namelen = arh->ar_name +
LIBELF_AR_BSD_EXTENDED_NAME_PREFIX_SIZE;
if (_libelf_ar_get_number(namelen, sizeof(arh->ar_name) -
LIBELF_AR_BSD_EXTENDED_NAME_PREFIX_SIZE, 10, &nlen) == 0)
goto error;
n -= nlen;
}

eh->ar_size = n;

if ((eh->ar_rawname = _libelf_ar_get_raw_name(arh)) == NULL)
goto error;

eh->ar_flags = 0;

return (eh);

error:
if (eh) {
if (eh->ar_name)
free(eh->ar_name);
if (eh->ar_rawname)
free(eh->ar_rawname);
free(eh);
}

e->e_flags &= ~LIBELF_F_AR_HEADER;
e->e_hdr.e_rawhdr = (unsigned char *) arh;

return (NULL);
}

Elf *
_libelf_ar_open_member(int fd, Elf_Cmd c, Elf *elf)
{
Elf *e;
size_t nsz, sz;
off_t next, end;
struct ar_hdr *arh;
char *member, *namelen;

assert(elf->e_kind == ELF_K_AR);

next = elf->e_u.e_ar.e_next;





if (next == (off_t) 0)
return (NULL);

assert((next & 1) == 0);





end = next + (off_t) sizeof(struct ar_hdr);
if ((uintmax_t) end < (uintmax_t) next ||
end > (off_t) elf->e_rawsize) {
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}

arh = (struct ar_hdr *) (elf->e_rawfile + next);




if (_libelf_ar_get_number(arh->ar_size, sizeof(arh->ar_size), 10,
&sz) == 0) {
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}





end += (off_t) sz;
if (end < next ||
end > (off_t) elf->e_rawsize) {
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}





if (IS_EXTENDED_BSD_NAME(arh->ar_name)) {
namelen = arh->ar_name +
LIBELF_AR_BSD_EXTENDED_NAME_PREFIX_SIZE;
if (_libelf_ar_get_number(namelen, sizeof(arh->ar_name) -
LIBELF_AR_BSD_EXTENDED_NAME_PREFIX_SIZE, 10, &nsz) == 0) {
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}

member = (char *) (arh + 1) + nsz;
sz -= nsz;
} else
member = (char *) (arh + 1);


if ((e = elf_memory(member, sz)) == NULL)
return (NULL);

e->e_fd = fd;
e->e_cmd = c;
e->e_hdr.e_rawhdr = (unsigned char *) arh;

elf->e_u.e_ar.e_nchildren++;
e->e_parent = elf;

return (e);
}


















#define GET_LONG(P, V)do { memcpy(&(V), (P), sizeof(long)); (P) += sizeof(long); } while (0)




Elf_Arsym *
_libelf_ar_process_bsd_symtab(Elf *e, size_t *count)
{
Elf_Arsym *symtab, *sym;
unsigned int n;
size_t nentries;
unsigned char *end, *p, *p0, *s, *s0;
const size_t entrysize = 2 * sizeof(long);
long arraysize, fileoffset, stroffset, strtabsize;

assert(e != NULL);
assert(count != NULL);
assert(e->e_u.e_ar.e_symtab == NULL);

symtab = NULL;





if (e->e_u.e_ar.e_rawsymtabsz < 2 * sizeof(long))
goto symtaberror;

p = p0 = (unsigned char *) e->e_u.e_ar.e_rawsymtab;
end = p0 + e->e_u.e_ar.e_rawsymtabsz;





GET_LONG(p, arraysize);

if (arraysize < 0 || p0 + arraysize >= end ||
((size_t) arraysize % entrysize != 0))
goto symtaberror;




s = p + arraysize;
GET_LONG(s, strtabsize);

s0 = s;
if (strtabsize < 0 || s0 + strtabsize > end)
goto symtaberror;

nentries = (size_t) arraysize / entrysize;




if ((symtab = malloc(sizeof(Elf_Arsym) * (nentries + 1))) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}


for (n = 0, sym = symtab; n < nentries; n++, sym++) {
GET_LONG(p, stroffset);
GET_LONG(p, fileoffset);

if (stroffset < 0 || fileoffset < 0 ||
(off_t) fileoffset >= e->e_rawsize)
goto symtaberror;

s = s0 + stroffset;

if (s >= end)
goto symtaberror;

sym->as_off = (off_t) fileoffset;
sym->as_hash = elf_hash((char *) s);
sym->as_name = (char *) s;
}


sym->as_name = NULL;
sym->as_hash = ~0UL;
sym->as_off = (off_t) 0;


e->e_u.e_ar.e_symtab = symtab;

*count = e->e_u.e_ar.e_symtabsz = nentries + 1;

return (symtab);

symtaberror:
if (symtab)
free(symtab);
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}










#define GET_WORD(P, V) do { (V) = 0; (V) = (P)[0]; (V) <<= 8; (V) += (P)[1]; (V) <<= 8; (V) += (P)[2]; (V) <<= 8; (V) += (P)[3]; } while (0)







#define INTSZ 4


Elf_Arsym *
_libelf_ar_process_svr4_symtab(Elf *e, size_t *count)
{
uint32_t off;
size_t n, nentries;
Elf_Arsym *symtab, *sym;
unsigned char *p, *s, *end;

assert(e != NULL);
assert(count != NULL);
assert(e->e_u.e_ar.e_symtab == NULL);

symtab = NULL;

if (e->e_u.e_ar.e_rawsymtabsz < INTSZ)
goto symtaberror;

p = (unsigned char *) e->e_u.e_ar.e_rawsymtab;
end = p + e->e_u.e_ar.e_rawsymtabsz;

GET_WORD(p, nentries);
p += INTSZ;

if (nentries == 0 || p + nentries * INTSZ >= end)
goto symtaberror;


if ((symtab = malloc(sizeof(Elf_Arsym) * (nentries+1))) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}

s = p + (nentries * INTSZ);

for (n = nentries, sym = symtab; n > 0; n--) {
if (s >= end)
goto symtaberror;

GET_WORD(p, off);
if (off >= e->e_rawsize)
goto symtaberror;

sym->as_off = (off_t) off;
sym->as_hash = elf_hash((char *) s);
sym->as_name = (char *) s;

p += INTSZ;
sym++;

for (; s < end && *s++ != '\0';)
;
}


sym->as_name = NULL;
sym->as_hash = ~0UL;
sym->as_off = (off_t) 0;

*count = e->e_u.e_ar.e_symtabsz = nentries + 1;
e->e_u.e_ar.e_symtab = symtab;

return (symtab);

symtaberror:
if (symtab)
free(symtab);
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}
