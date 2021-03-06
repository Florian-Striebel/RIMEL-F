





























#include <assert.h>
#include <errno.h>
#include <libelf.h>
#include <stdlib.h>
#include <string.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: libelf_allocate.c 3738 2019-05-05 21:49:06Z jkoshy $");

Elf *
_libelf_allocate_elf(void)
{
Elf *e;

if ((e = calloc((size_t) 1, sizeof(*e))) == NULL) {
LIBELF_SET_ERROR(RESOURCE, errno);
return NULL;
}

e->e_activations = 1;
e->e_byteorder = ELFDATANONE;
e->e_class = ELFCLASSNONE;
e->e_cmd = ELF_C_NULL;
e->e_fd = -1;
e->e_kind = ELF_K_NONE;
e->e_version = LIBELF_PRIVATE(version);

return (e);
}

void
_libelf_init_elf(Elf *e, Elf_Kind kind)
{
assert(e != NULL);
assert(e->e_kind == ELF_K_NONE);

e->e_kind = kind;

switch (kind) {
case ELF_K_ELF:
RB_INIT(&e->e_u.e_elf.e_scn);
break;
default:
break;
}
}

void
_libelf_release_elf(Elf *e)
{
Elf_Arhdr *arh;

switch (e->e_kind) {
case ELF_K_AR:
free(e->e_u.e_ar.e_symtab);
break;

case ELF_K_ELF:
switch (e->e_class) {
case ELFCLASS32:
free(e->e_u.e_elf.e_ehdr.e_ehdr32);
free(e->e_u.e_elf.e_phdr.e_phdr32);
break;
case ELFCLASS64:
free(e->e_u.e_elf.e_ehdr.e_ehdr64);
free(e->e_u.e_elf.e_phdr.e_phdr64);
break;
}

assert(RB_EMPTY(&e->e_u.e_elf.e_scn));

if (e->e_flags & LIBELF_F_AR_HEADER) {
arh = e->e_hdr.e_arhdr;
free(arh->ar_name);
free(arh->ar_rawname);
free(arh);
}

break;

default:
break;
}

free(e);
}

struct _Libelf_Data *
_libelf_allocate_data(Elf_Scn *s)
{
struct _Libelf_Data *d;

if ((d = calloc((size_t) 1, sizeof(*d))) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}

d->d_scn = s;

return (d);
}

struct _Libelf_Data *
_libelf_release_data(struct _Libelf_Data *d)
{

if (d->d_flags & LIBELF_F_DATA_MALLOCED)
free(d->d_data.d_buf);

free(d);

return (NULL);
}

Elf_Scn *
_libelf_allocate_scn(Elf *e, size_t ndx)
{
Elf_Scn *s;

if ((s = calloc((size_t) 1, sizeof(Elf_Scn))) == NULL) {
LIBELF_SET_ERROR(RESOURCE, errno);
return (NULL);
}

s->s_elf = e;
s->s_ndx = ndx;

STAILQ_INIT(&s->s_data);
STAILQ_INIT(&s->s_rawdata);

RB_INSERT(scntree, &e->e_u.e_elf.e_scn, s);

return (s);
}

Elf_Scn *
_libelf_release_scn(Elf_Scn *s)
{
Elf *e;
struct _Libelf_Data *d, *td;

assert(s != NULL);

STAILQ_FOREACH_SAFE(d, &s->s_data, d_next, td) {
STAILQ_REMOVE(&s->s_data, d, _Libelf_Data, d_next);
d = _libelf_release_data(d);
}

STAILQ_FOREACH_SAFE(d, &s->s_rawdata, d_next, td) {
assert((d->d_flags & LIBELF_F_DATA_MALLOCED) == 0);
STAILQ_REMOVE(&s->s_rawdata, d, _Libelf_Data, d_next);
d = _libelf_release_data(d);
}

e = s->s_elf;

assert(e != NULL);

RB_REMOVE(scntree, &e->e_u.e_elf.e_scn, s);

free(s);

return (NULL);
}
