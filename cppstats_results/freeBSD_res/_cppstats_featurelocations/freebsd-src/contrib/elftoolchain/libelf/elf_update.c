

























#include <sys/param.h>
#include <sys/stat.h>

#include <assert.h>
#include <errno.h>
#include <gelf.h>
#include <libelf.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "_libelf.h"

#if ELFTC_HAVE_MMAP
#include <sys/mman.h>
#endif

ELFTC_VCSID("$Id: elf_update.c 3763 2019-06-28 21:43:27Z emaste $");








































enum elf_extent {
ELF_EXTENT_EHDR,
ELF_EXTENT_PHDR,
ELF_EXTENT_SECTION,
ELF_EXTENT_SHDR
};




struct _Elf_Extent {
SLIST_ENTRY(_Elf_Extent) ex_next;
uint64_t ex_start;
uint64_t ex_size;
enum elf_extent ex_type;
void *ex_desc;
};

SLIST_HEAD(_Elf_Extent_List, _Elf_Extent);






static int
_libelf_compute_section_extents(Elf *e, Elf_Scn *s, off_t rc)
{
Elf_Data *d;
size_t fsz, msz;
int ec, elftype;
uint32_t sh_type;
uint64_t d_align;
Elf32_Shdr *shdr32;
Elf64_Shdr *shdr64;
struct _Libelf_Data *ld;
uint64_t scn_size, scn_alignment;
uint64_t sh_align, sh_entsize, sh_offset, sh_size;

ec = e->e_class;

shdr32 = &s->s_shdr.s_shdr32;
shdr64 = &s->s_shdr.s_shdr64;
if (ec == ELFCLASS32) {
sh_type = shdr32->sh_type;
sh_align = (uint64_t) shdr32->sh_addralign;
sh_entsize = (uint64_t) shdr32->sh_entsize;
sh_offset = (uint64_t) shdr32->sh_offset;
sh_size = (uint64_t) shdr32->sh_size;
} else {
sh_type = shdr64->sh_type;
sh_align = shdr64->sh_addralign;
sh_entsize = shdr64->sh_entsize;
sh_offset = shdr64->sh_offset;
sh_size = shdr64->sh_size;
}

assert(sh_type != SHT_NULL && sh_type != SHT_NOBITS);

elftype = _libelf_xlate_shtype(sh_type);
if (elftype < ELF_T_FIRST || elftype > ELF_T_LAST) {
LIBELF_SET_ERROR(SECTION, 0);
return (0);
}

if (sh_align == 0)
sh_align = _libelf_falign(elftype, ec);





if (STAILQ_EMPTY(&s->s_data)) {






if ((s->s_flags & ELF_F_DIRTY) == 0) {










if (e->e_flags & ELF_F_LAYOUT)
goto updatedescriptor;
else
goto computeoffset;
}





if (e->e_cmd != ELF_C_WRITE && elf_getdata(s, NULL) == NULL)
return (0);
}




scn_size = 0L;
scn_alignment = 0;
STAILQ_FOREACH(ld, &s->s_data, d_next) {

d = &ld->d_data;




if (d->d_type >= ELF_T_NUM) {
LIBELF_SET_ERROR(DATA, 0);
return (0);
}




if (d->d_version != e->e_version) {
LIBELF_SET_ERROR(VERSION, 0);
return (0);
}





if ((d_align = d->d_align) == 0 ||
(d_align & (d_align - 1))) {
LIBELF_SET_ERROR(DATA, 0);
return (0);
}





if ((msz = _libelf_msize(d->d_type, ec, e->e_version)) == 0)
return (0);





if (d->d_size % msz) {
LIBELF_SET_ERROR(DATA, 0);
return (0);
}






if ((e->e_flags & ELF_F_LAYOUT) &&
(d->d_off & (d_align - 1))) {
LIBELF_SET_ERROR(LAYOUT, 0);
return (0);
}




if (e->e_flags & ELF_F_LAYOUT) {
if ((uint64_t) d->d_off + d->d_size > scn_size)
scn_size = d->d_off + d->d_size;
} else {
scn_size = roundup2(scn_size, d->d_align);
d->d_off = scn_size;
fsz = _libelf_fsize(d->d_type, ec, d->d_version,
(size_t) d->d_size / msz);
scn_size += fsz;
}





if (d_align > scn_alignment)
scn_alignment = d_align;
}







if (e->e_flags & ELF_F_LAYOUT) {
if (scn_alignment > sh_align ||
sh_offset % sh_align ||
sh_size < scn_size ||
sh_offset % _libelf_falign(elftype, ec)) {
LIBELF_SET_ERROR(LAYOUT, 0);
return (0);
}
goto updatedescriptor;
}







if (scn_alignment > sh_align)
sh_align = scn_alignment;







if (sh_entsize == 0 &&
(sh_entsize = _libelf_fsize(elftype, ec, e->e_version,
(size_t) 1)) == 1)
sh_entsize = 0;

sh_size = scn_size;

computeoffset:




sh_offset = roundup((uint64_t) rc, sh_align);




if (ec == ELFCLASS32) {
shdr32->sh_addralign = (uint32_t) sh_align;
shdr32->sh_entsize = (uint32_t) sh_entsize;
shdr32->sh_offset = (uint32_t) sh_offset;
shdr32->sh_size = (uint32_t) sh_size;
} else {
shdr64->sh_addralign = sh_align;
shdr64->sh_entsize = sh_entsize;
shdr64->sh_offset = sh_offset;
shdr64->sh_size = sh_size;
}

updatedescriptor:



s->s_size = sh_size;
s->s_offset = sh_offset;

return (1);
}





static void
_libelf_release_extents(struct _Elf_Extent_List *extents)
{
struct _Elf_Extent *ex;

while ((ex = SLIST_FIRST(extents)) != NULL) {
SLIST_REMOVE_HEAD(extents, ex_next);
free(ex);
}
}







static int
_libelf_extent_is_unused(struct _Elf_Extent_List *extents,
const uint64_t start, const uint64_t size, struct _Elf_Extent **prevt)
{
uint64_t tmax, tmin;
struct _Elf_Extent *t, *pt;
const uint64_t smax = start + size;


pt = NULL;
SLIST_FOREACH(t, extents, ex_next) {
tmin = t->ex_start;
tmax = tmin + t->ex_size;

if (tmax <= start) {



pt = t;
continue;
} else if (smax <= tmin) {




assert(pt == NULL ||
pt->ex_start + pt->ex_size <= start);
break;
} else

return (0);
}

if (prevt)
*prevt = pt;
return (1);
}





static int
_libelf_insert_extent(struct _Elf_Extent_List *extents, int type,
uint64_t start, uint64_t size, void *desc)
{
struct _Elf_Extent *ex, *prevt;

assert(type >= ELF_EXTENT_EHDR && type <= ELF_EXTENT_SHDR);

prevt = NULL;





if (!_libelf_extent_is_unused(extents, start, size, &prevt)) {
LIBELF_SET_ERROR(LAYOUT, 0);
return (0);
}


if ((ex = malloc(sizeof(struct _Elf_Extent))) == NULL) {
LIBELF_SET_ERROR(RESOURCE, errno);
return (0);
}
ex->ex_start = start;
ex->ex_size = size;
ex->ex_desc = desc;
ex->ex_type = type;


if (prevt)
SLIST_INSERT_AFTER(prevt, ex, ex_next);
else
SLIST_INSERT_HEAD(extents, ex, ex_next);
return (1);
}





static off_t
_libelf_resync_sections(Elf *e, off_t rc, struct _Elf_Extent_List *extents)
{
int ec;
Elf_Scn *s;
size_t sh_type;

ec = e->e_class;





RB_FOREACH(s, scntree, &e->e_u.e_elf.e_scn) {
if (ec == ELFCLASS32)
sh_type = s->s_shdr.s_shdr32.sh_type;
else
sh_type = s->s_shdr.s_shdr64.sh_type;

if (sh_type == SHT_NOBITS || sh_type == SHT_NULL)
continue;

if (_libelf_compute_section_extents(e, s, rc) == 0)
return ((off_t) -1);

if (s->s_size == 0)
continue;

if (!_libelf_insert_extent(extents, ELF_EXTENT_SECTION,
s->s_offset, s->s_size, s))
return ((off_t) -1);

if ((size_t) rc < s->s_offset + s->s_size)
rc = (off_t) (s->s_offset + s->s_size);
}

return (rc);
}























static off_t
_libelf_resync_elf(Elf *e, struct _Elf_Extent_List *extents)
{
int ec, eh_class;
unsigned int eh_byteorder, eh_version;
size_t align, fsz;
size_t phnum, shnum;
off_t rc, phoff, shoff;
void *ehdr, *phdr;
Elf32_Ehdr *eh32;
Elf64_Ehdr *eh64;

rc = 0;

ec = e->e_class;

assert(ec == ELFCLASS32 || ec == ELFCLASS64);




if ((ehdr = _libelf_ehdr(e, ec, 0)) == NULL)
return ((off_t) -1);

eh32 = ehdr;
eh64 = ehdr;

if (ec == ELFCLASS32) {
eh_byteorder = eh32->e_ident[EI_DATA];
eh_class = eh32->e_ident[EI_CLASS];
phoff = (off_t) eh32->e_phoff;
shoff = (off_t) eh32->e_shoff;
eh_version = eh32->e_version;
} else {
eh_byteorder = eh64->e_ident[EI_DATA];
eh_class = eh64->e_ident[EI_CLASS];
phoff = (off_t) eh64->e_phoff;
shoff = (off_t) eh64->e_shoff;
eh_version = eh64->e_version;
}

if (phoff < 0 || shoff < 0) {
LIBELF_SET_ERROR(HEADER, 0);
return ((off_t) -1);
}

if (eh_version == EV_NONE)
eh_version = EV_CURRENT;

if (eh_version != e->e_version) {
LIBELF_SET_ERROR(VERSION, 0);
return ((off_t) -1);
}

if (eh_class != e->e_class) {
LIBELF_SET_ERROR(CLASS, 0);
return ((off_t) -1);
}

if (e->e_cmd != ELF_C_WRITE && eh_byteorder != e->e_byteorder) {
LIBELF_SET_ERROR(HEADER, 0);
return ((off_t) -1);
}

shnum = e->e_u.e_elf.e_nscn;
phnum = e->e_u.e_elf.e_nphdr;

e->e_byteorder = eh_byteorder;

#define INITIALIZE_EHDR(E,EC,V) do { unsigned int _version = (unsigned int) (V); (E)->e_ident[EI_MAG0] = ELFMAG0; (E)->e_ident[EI_MAG1] = ELFMAG1; (E)->e_ident[EI_MAG2] = ELFMAG2; (E)->e_ident[EI_MAG3] = ELFMAG3; (E)->e_ident[EI_CLASS] = (unsigned char) (EC); (E)->e_ident[EI_VERSION] = (_version & 0xFFU); (E)->e_ehsize = (uint16_t) _libelf_fsize(ELF_T_EHDR, (EC), _version, (size_t) 1); (E)->e_phentsize = (uint16_t) ((phnum == 0) ? 0 : _libelf_fsize(ELF_T_PHDR, (EC), _version, (size_t) 1)); (E)->e_shentsize = (uint16_t) _libelf_fsize(ELF_T_SHDR, (EC), _version, (size_t) 1); } while (0)
















if (ec == ELFCLASS32)
INITIALIZE_EHDR(eh32, ec, eh_version);
else
INITIALIZE_EHDR(eh64, ec, eh_version);

(void) elf_flagehdr(e, ELF_C_SET, ELF_F_DIRTY);

rc += (off_t) _libelf_fsize(ELF_T_EHDR, ec, eh_version, (size_t) 1);

if (!_libelf_insert_extent(extents, ELF_EXTENT_EHDR, 0, (uint64_t) rc,
ehdr))
return ((off_t) -1);






if (phnum) {
fsz = _libelf_fsize(ELF_T_PHDR, ec, eh_version, phnum);
align = _libelf_falign(ELF_T_PHDR, ec);

if (e->e_flags & ELF_F_LAYOUT) {



if (rc > phoff) {
LIBELF_SET_ERROR(LAYOUT, 0);
return ((off_t) -1);
}

if (phoff % (off_t) align) {
LIBELF_SET_ERROR(LAYOUT, 0);
return ((off_t) -1);
}

} else
phoff = roundup(rc, (off_t) align);

rc = phoff + (off_t) fsz;

phdr = _libelf_getphdr(e, ec);

if (!_libelf_insert_extent(extents, ELF_EXTENT_PHDR,
(uint64_t) phoff, fsz, phdr))
return ((off_t) -1);
} else
phoff = 0;






if (e->e_cmd != ELF_C_WRITE &&
(e->e_flags & LIBELF_F_SHDRS_LOADED) == 0 &&
_libelf_load_section_headers(e, ehdr) == 0)
return ((off_t) -1);

if ((rc = _libelf_resync_sections(e, rc, extents)) < 0)
return ((off_t) -1);













if (shnum) {
fsz = _libelf_fsize(ELF_T_SHDR, ec, eh_version, shnum);
align = _libelf_falign(ELF_T_SHDR, ec);

if (e->e_flags & ELF_F_LAYOUT) {
if (shoff % (off_t) align) {
LIBELF_SET_ERROR(LAYOUT, 0);
return ((off_t) -1);
}
} else
shoff = roundup(rc, (off_t) align);

if (shoff + (off_t) fsz > rc)
rc = shoff + (off_t) fsz;

if (!_libelf_insert_extent(extents, ELF_EXTENT_SHDR,
(uint64_t) shoff, fsz, NULL))
return ((off_t) -1);
} else
shoff = 0;





_libelf_setphnum(e, ehdr, ec, phnum);
_libelf_setshnum(e, ehdr, ec, shnum);





if ((e->e_flags & ELF_F_LAYOUT) == 0) {
if (ec == ELFCLASS32) {
eh32->e_phoff = (uint32_t) phoff;
eh32->e_shoff = (uint32_t) shoff;
} else {
eh64->e_phoff = (uint64_t) phoff;
eh64->e_shoff = (uint64_t) shoff;
}
}

return (rc);
}





static off_t
_libelf_write_scn(Elf *e, unsigned char *nf, struct _Elf_Extent *ex)
{
off_t rc;
int ec, em;
Elf_Scn *s;
int elftype;
Elf_Data *d, dst;
uint32_t sh_type;
struct _Libelf_Data *ld;
uint64_t sh_off, sh_size;
size_t fsz, msz, nobjects;

assert(ex->ex_type == ELF_EXTENT_SECTION);

s = ex->ex_desc;
rc = (off_t) ex->ex_start;

if ((ec = e->e_class) == ELFCLASS32) {
sh_type = s->s_shdr.s_shdr32.sh_type;
sh_size = (uint64_t) s->s_shdr.s_shdr32.sh_size;
} else {
sh_type = s->s_shdr.s_shdr64.sh_type;
sh_size = s->s_shdr.s_shdr64.sh_size;
}




if (sh_type == SHT_NOBITS || sh_type == SHT_NULL || sh_size == 0)
return (rc);

elftype = _libelf_xlate_shtype(sh_type);
assert(elftype >= ELF_T_FIRST && elftype <= ELF_T_LAST);

sh_off = s->s_offset;
assert(sh_off % _libelf_falign(elftype, ec) == 0);

em = _libelf_elfmachine(e);
#if 0
assert(em >= EM_NONE && em < EM__LAST__);
#endif









if (STAILQ_EMPTY(&s->s_data)) {

if ((d = elf_rawdata(s, NULL)) == NULL)
return ((off_t) -1);

STAILQ_FOREACH(ld, &s->s_rawdata, d_next) {

d = &ld->d_data;

if ((uint64_t) rc < sh_off + d->d_off)
(void) memset(nf + rc,
LIBELF_PRIVATE(fillchar),
(size_t) (sh_off + d->d_off -
(uint64_t) rc));
rc = (off_t) (sh_off + d->d_off);

assert(d->d_buf != NULL);
assert(d->d_type == ELF_T_BYTE);
assert(d->d_version == e->e_version);

(void) memcpy(nf + rc,
e->e_rawfile + s->s_rawoff + d->d_off,
(size_t) d->d_size);

rc += (off_t) d->d_size;
}

return (rc);
}







dst.d_version = e->e_version;

STAILQ_FOREACH(ld, &s->s_data, d_next) {

d = &ld->d_data;

if ((msz = _libelf_msize(d->d_type, ec, e->e_version)) == 0)
return ((off_t) -1);

if ((uint64_t) rc < sh_off + d->d_off)
(void) memset(nf + rc,
LIBELF_PRIVATE(fillchar),
(size_t) (sh_off + d->d_off - (uint64_t) rc));

rc = (off_t) (sh_off + d->d_off);

assert(d->d_buf != NULL);
assert(d->d_version == e->e_version);
assert(d->d_size % msz == 0);
assert(msz != 0);

nobjects = (size_t) (d->d_size / msz);

fsz = _libelf_fsize(d->d_type, ec, e->e_version, nobjects);

dst.d_buf = nf + rc;
dst.d_size = fsz;

if (_libelf_xlate(&dst, d, e->e_byteorder, ec, em, ELF_TOFILE)
== NULL)
return ((off_t) -1);

rc += (off_t) fsz;
}

return (rc);
}





static off_t
_libelf_write_ehdr(Elf *e, unsigned char *nf, struct _Elf_Extent *ex)
{
int ec, em;
void *ehdr;
size_t fsz, msz;
Elf_Data dst, src;

assert(ex->ex_type == ELF_EXTENT_EHDR);
assert(ex->ex_start == 0);

ec = e->e_class;

ehdr = _libelf_ehdr(e, ec, 0);
assert(ehdr != NULL);

fsz = _libelf_fsize(ELF_T_EHDR, ec, e->e_version, (size_t) 1);
if ((msz = _libelf_msize(ELF_T_EHDR, ec, e->e_version)) == 0)
return ((off_t) -1);

em = _libelf_elfmachine(e);

(void) memset(&dst, 0, sizeof(dst));
(void) memset(&src, 0, sizeof(src));

src.d_buf = ehdr;
src.d_size = msz;
src.d_type = ELF_T_EHDR;
src.d_version = dst.d_version = e->e_version;

dst.d_buf = nf;
dst.d_size = fsz;

if (_libelf_xlate(&dst, &src, e->e_byteorder, ec, em, ELF_TOFILE) ==
NULL)
return ((off_t) -1);

return ((off_t) fsz);
}





static off_t
_libelf_write_phdr(Elf *e, unsigned char *nf, struct _Elf_Extent *ex)
{
int ec, em;
void *ehdr;
Elf32_Ehdr *eh32;
Elf64_Ehdr *eh64;
Elf_Data dst, src;
size_t fsz, msz, phnum;
uint64_t phoff;

assert(ex->ex_type == ELF_EXTENT_PHDR);

ec = e->e_class;

ehdr = _libelf_ehdr(e, ec, 0);
assert(ehdr != NULL);

phnum = e->e_u.e_elf.e_nphdr;
assert(phnum > 0);

if (ec == ELFCLASS32) {
eh32 = (Elf32_Ehdr *) ehdr;
phoff = (uint64_t) eh32->e_phoff;
} else {
eh64 = (Elf64_Ehdr *) ehdr;
phoff = eh64->e_phoff;
}

em = _libelf_elfmachine(e);

assert(phoff > 0);
assert(ex->ex_start == phoff);
assert(phoff % _libelf_falign(ELF_T_PHDR, ec) == 0);

(void) memset(&dst, 0, sizeof(dst));
(void) memset(&src, 0, sizeof(src));

if ((msz = _libelf_msize(ELF_T_PHDR, ec, e->e_version)) == 0)
return ((off_t) -1);
fsz = _libelf_fsize(ELF_T_PHDR, ec, e->e_version, phnum);
assert(fsz > 0);

src.d_buf = _libelf_getphdr(e, ec);
src.d_version = dst.d_version = e->e_version;
src.d_type = ELF_T_PHDR;
src.d_size = phnum * msz;

dst.d_size = fsz;
dst.d_buf = nf + ex->ex_start;

if (_libelf_xlate(&dst, &src, e->e_byteorder, ec, em, ELF_TOFILE) ==
NULL)
return ((off_t) -1);

return ((off_t) (phoff + fsz));
}





static off_t
_libelf_write_shdr(Elf *e, unsigned char *nf, struct _Elf_Extent *ex)
{
int ec, em;
void *ehdr;
Elf_Scn *scn;
uint64_t shoff;
Elf32_Ehdr *eh32;
Elf64_Ehdr *eh64;
size_t fsz, msz, nscn;
Elf_Data dst, src;

assert(ex->ex_type == ELF_EXTENT_SHDR);

ec = e->e_class;

ehdr = _libelf_ehdr(e, ec, 0);
assert(ehdr != NULL);

nscn = e->e_u.e_elf.e_nscn;

if (ec == ELFCLASS32) {
eh32 = (Elf32_Ehdr *) ehdr;
shoff = (uint64_t) eh32->e_shoff;
} else {
eh64 = (Elf64_Ehdr *) ehdr;
shoff = eh64->e_shoff;
}

em = _libelf_elfmachine(e);

assert(nscn > 0);
assert(shoff % _libelf_falign(ELF_T_SHDR, ec) == 0);
assert(ex->ex_start == shoff);

(void) memset(&dst, 0, sizeof(dst));
(void) memset(&src, 0, sizeof(src));

if ((msz = _libelf_msize(ELF_T_SHDR, ec, e->e_version)) == 0)
return ((off_t) -1);

src.d_type = ELF_T_SHDR;
src.d_size = msz;
src.d_version = dst.d_version = e->e_version;

fsz = _libelf_fsize(ELF_T_SHDR, ec, e->e_version, (size_t) 1);

RB_FOREACH(scn, scntree, &e->e_u.e_elf.e_scn) {
if (ec == ELFCLASS32)
src.d_buf = &scn->s_shdr.s_shdr32;
else
src.d_buf = &scn->s_shdr.s_shdr64;

dst.d_size = fsz;
dst.d_buf = nf + ex->ex_start + scn->s_ndx * fsz;

if (_libelf_xlate(&dst, &src, e->e_byteorder, ec, em,
ELF_TOFILE) == NULL)
return ((off_t) -1);
}

return ((off_t) (ex->ex_start + nscn * fsz));
}


















static off_t
_libelf_write_elf(Elf *e, off_t newsize, struct _Elf_Extent_List *extents)
{
off_t nrc, rc;
Elf_Scn *scn, *tscn;
struct _Elf_Extent *ex;
unsigned char *newfile;

assert(e->e_kind == ELF_K_ELF);
assert(e->e_cmd == ELF_C_RDWR || e->e_cmd == ELF_C_WRITE);
assert(e->e_fd >= 0);

if ((newfile = malloc((size_t) newsize)) == NULL) {
LIBELF_SET_ERROR(RESOURCE, errno);
return ((off_t) -1);
}

nrc = rc = 0;
SLIST_FOREACH(ex, extents, ex_next) {


if (ex->ex_start > (size_t) rc)
(void) memset(newfile + rc, LIBELF_PRIVATE(fillchar),
(size_t) (ex->ex_start - (uint64_t) rc));

switch (ex->ex_type) {
case ELF_EXTENT_EHDR:
if ((nrc = _libelf_write_ehdr(e, newfile, ex)) < 0)
goto error;
break;

case ELF_EXTENT_PHDR:
if ((nrc = _libelf_write_phdr(e, newfile, ex)) < 0)
goto error;
break;

case ELF_EXTENT_SECTION:
if ((nrc = _libelf_write_scn(e, newfile, ex)) < 0)
goto error;
break;

case ELF_EXTENT_SHDR:
if ((nrc = _libelf_write_shdr(e, newfile, ex)) < 0)
goto error;
break;

default:
assert(0);
break;
}

assert(ex->ex_start + ex->ex_size == (size_t) nrc);
assert(rc < nrc);

rc = nrc;
}

assert(rc == newsize);





if ((e->e_flags & LIBELF_F_SPECIAL_FILE) == 0) {
if (ftruncate(e->e_fd, (off_t) 0) < 0 ||
lseek(e->e_fd, (off_t) 0, SEEK_SET)) {
LIBELF_SET_ERROR(IO, errno);
goto error;
}
#if ELFTC_HAVE_MMAP
if (e->e_flags & LIBELF_F_RAWFILE_MMAP) {
assert(e->e_rawfile != NULL);
assert(e->e_cmd == ELF_C_RDWR);
if (munmap(e->e_rawfile, (size_t) e->e_rawsize) < 0) {
LIBELF_SET_ERROR(IO, errno);
goto error;
}
}
#endif
}




if (write(e->e_fd, newfile, (size_t) newsize) != newsize) {
LIBELF_SET_ERROR(IO, errno);
goto error;
}





if (e->e_cmd == ELF_C_RDWR) {
assert(e->e_rawfile != NULL);
assert((e->e_flags & LIBELF_F_RAWFILE_MALLOC) ||
(e->e_flags & LIBELF_F_RAWFILE_MMAP));
if (e->e_flags & LIBELF_F_RAWFILE_MALLOC) {
assert((e->e_flags & LIBELF_F_RAWFILE_MMAP) == 0);
free(e->e_rawfile);
e->e_rawfile = newfile;
newfile = NULL;
}
#if ELFTC_HAVE_MMAP
else if (e->e_flags & LIBELF_F_RAWFILE_MMAP) {
assert((e->e_flags & LIBELF_F_RAWFILE_MALLOC) == 0);
if ((e->e_rawfile = mmap(NULL, (size_t) newsize,
PROT_READ, MAP_PRIVATE, e->e_fd, (off_t) 0)) ==
MAP_FAILED) {
LIBELF_SET_ERROR(IO, errno);
goto error;
}
}
#endif


e->e_rawsize = newsize;
} else {

assert(e->e_rawfile == NULL);
}







e->e_flags &= ~ELF_F_DIRTY;

RB_FOREACH_SAFE(scn, scntree, &e->e_u.e_elf.e_scn, tscn)
_libelf_release_scn(scn);

if (e->e_class == ELFCLASS32) {
free(e->e_u.e_elf.e_ehdr.e_ehdr32);
if (e->e_u.e_elf.e_phdr.e_phdr32)
free(e->e_u.e_elf.e_phdr.e_phdr32);

e->e_u.e_elf.e_ehdr.e_ehdr32 = NULL;
e->e_u.e_elf.e_phdr.e_phdr32 = NULL;
} else {
free(e->e_u.e_elf.e_ehdr.e_ehdr64);
if (e->e_u.e_elf.e_phdr.e_phdr64)
free(e->e_u.e_elf.e_phdr.e_phdr64);

e->e_u.e_elf.e_ehdr.e_ehdr64 = NULL;
e->e_u.e_elf.e_phdr.e_phdr64 = NULL;
}


if (newfile)
free(newfile);

return (rc);

error:
free(newfile);

return ((off_t) -1);
}





off_t
elf_update(Elf *e, Elf_Cmd c)
{
int ec;
off_t rc;
struct _Elf_Extent_List extents;

rc = (off_t) -1;

if (e == NULL || e->e_kind != ELF_K_ELF ||
(c != ELF_C_NULL && c != ELF_C_WRITE)) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (rc);
}

if ((ec = e->e_class) != ELFCLASS32 && ec != ELFCLASS64) {
LIBELF_SET_ERROR(CLASS, 0);
return (rc);
}

if (e->e_version == EV_NONE)
e->e_version = EV_CURRENT;

if (c == ELF_C_WRITE && e->e_cmd == ELF_C_READ) {
LIBELF_SET_ERROR(MODE, 0);
return (rc);
}

SLIST_INIT(&extents);

if ((rc = _libelf_resync_elf(e, &extents)) < 0)
goto done;

if (c == ELF_C_NULL)
goto done;

if (e->e_fd < 0) {
rc = (off_t) -1;
LIBELF_SET_ERROR(SEQUENCE, 0);
goto done;
}

rc = _libelf_write_elf(e, rc, &extents);

done:
_libelf_release_extents(&extents);
e->e_flags &= ~LIBELF_F_SHDRS_LOADED;
return (rc);
}
