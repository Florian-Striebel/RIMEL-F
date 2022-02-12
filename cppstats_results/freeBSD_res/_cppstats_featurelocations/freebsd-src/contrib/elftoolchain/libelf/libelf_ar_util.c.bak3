

























#include <assert.h>
#include <libelf.h>
#include <stdlib.h>
#include <string.h>

#include "_libelf.h"
#include "_libelf_ar.h"

ELFTC_VCSID("$Id: libelf_ar_util.c 3174 2015-03-27 17:13:41Z emaste $");





int
_libelf_ar_get_number(const char *src, size_t sz, unsigned int base,
size_t *ret)
{
size_t r;
unsigned int c, v;
const unsigned char *e, *s;

assert(base <= 10);

s = (const unsigned char *) src;
e = s + sz;


for (;s < e && (c = *s) == ' '; s++)
;

r = 0L;
for (;s < e; s++) {
if ((c = *s) == ' ')
break;
if (c < '0' || c > '9')
return (0);
v = c - '0';
if (v >= base)
break;
r *= base;
r += v;
}

*ret = r;

return (1);
}




char *
_libelf_ar_get_translated_name(const struct ar_hdr *arh, Elf *ar)
{
char *s;
unsigned char c;
size_t len, offset;
const unsigned char *buf, *p, *q, *r;
const size_t bufsize = sizeof(arh->ar_name);

assert(arh != NULL);
assert(ar->e_kind == ELF_K_AR);
assert((const unsigned char *) arh >= ar->e_rawfile &&
(const unsigned char *) arh < ar->e_rawfile + ar->e_rawsize);

buf = (const unsigned char *) arh->ar_name;








if (buf[0] == '/' && (c = buf[1]) >= '0' && c <= '9') {





if (_libelf_ar_get_number((const char *) (buf + 1),
bufsize - 1, 10, &offset) == 0) {
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}

if (offset > ar->e_u.e_ar.e_rawstrtabsz) {
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}

p = q = ar->e_u.e_ar.e_rawstrtab + offset;
r = ar->e_u.e_ar.e_rawstrtab + ar->e_u.e_ar.e_rawstrtabsz;

for (; p < r && *p != '/'; p++)
;
len = (size_t) (p - q + 1);

if ((s = malloc(len)) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}

(void) strncpy(s, (const char *) q, len - 1);
s[len - 1] = '\0';

return (s);
} else if (IS_EXTENDED_BSD_NAME(buf)) {
r = buf + LIBELF_AR_BSD_EXTENDED_NAME_PREFIX_SIZE;

if (_libelf_ar_get_number((const char *) r, bufsize -
LIBELF_AR_BSD_EXTENDED_NAME_PREFIX_SIZE, 10,
&len) == 0) {
LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}





if ((s = malloc(len + 1)) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}




q = (const unsigned char *) (arh + 1);

(void) strncpy(s, (const char *) q, len);
s[len] = '\0';

return (s);
}








for (q = buf + bufsize - 1; q >= buf && *q == ' '; --q)
;

if (q >= buf) {
if (*q == '/') {





if (q > buf + 1 ||
(q == (buf + 1) && *buf != '/'))
q--;
}

len = (size_t) (q - buf + 2);
} else {

buf = (const unsigned char *) "";
len = 1;
}

if ((s = malloc(len)) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}

(void) strncpy(s, (const char *) buf, len - 1);
s[len - 1] = '\0';

return (s);
}





char *
_libelf_ar_get_raw_name(const struct ar_hdr *arh)
{
char *rawname;
const size_t namesz = sizeof(arh->ar_name);

if ((rawname = malloc(namesz + 1)) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}

(void) strncpy(rawname, arh->ar_name, namesz);
rawname[namesz] = '\0';
return (rawname);
}




Elf *
_libelf_ar_open(Elf *e, int reporterror)
{
size_t sz;
int scanahead;
struct ar_hdr arh;
unsigned char *s, *end;

_libelf_init_elf(e, ELF_K_AR);

e->e_u.e_ar.e_nchildren = 0;
e->e_u.e_ar.e_next = (off_t) -1;





s = e->e_rawfile + SARMAG;
end = e->e_rawfile + e->e_rawsize;

assert(e->e_rawsize > 0);












#define READ_AR_HEADER(S, ARH, SZ, END) do { if ((S) + sizeof((ARH)) > (END)) goto error; (void) memcpy(&(ARH), (S), sizeof((ARH))); if ((ARH).ar_fmag[0] != '`' || (ARH).ar_fmag[1] != '\n') goto error; if (_libelf_ar_get_number((char *) (ARH).ar_size, sizeof((ARH).ar_size), 10, &(SZ)) == 0) goto error; } while (0)











READ_AR_HEADER(s, arh, sz, end);




if (arh.ar_name[0] == '/') {
if (sz == 0)
goto error;

e->e_flags |= LIBELF_F_AR_VARIANT_SVR4;

scanahead = 0;





if (arh.ar_name[1] == ' ') {

scanahead = 1;

s += sizeof(arh);
e->e_u.e_ar.e_rawsymtab = s;
e->e_u.e_ar.e_rawsymtabsz = sz;

sz = LIBELF_ADJUST_AR_SIZE(sz);
s += sz;

} else if (arh.ar_name[1] == '/' && arh.ar_name[2] == ' ') {

s += sizeof(arh);
e->e_u.e_ar.e_rawstrtab = s;
e->e_u.e_ar.e_rawstrtabsz = sz;

sz = LIBELF_ADJUST_AR_SIZE(sz);
s += sz;
}





if (scanahead) {
READ_AR_HEADER(s, arh, sz, end);


if (arh.ar_name[0] == '/' && arh.ar_name[1] == '/' &&
arh.ar_name[2] == ' ') {

s += sizeof(arh);

e->e_u.e_ar.e_rawstrtab = s;
e->e_u.e_ar.e_rawstrtabsz = sz;

sz = LIBELF_ADJUST_AR_SIZE(sz);
s += sz;
}
}
} else if (strncmp(arh.ar_name, LIBELF_AR_BSD_SYMTAB_NAME,
sizeof(LIBELF_AR_BSD_SYMTAB_NAME) - 1) == 0) {



s += sizeof(arh);
e->e_u.e_ar.e_rawsymtab = s;
e->e_u.e_ar.e_rawsymtabsz = sz;

sz = LIBELF_ADJUST_AR_SIZE(sz);
s += sz;
}





e->e_u.e_ar.e_next = (off_t) (s - e->e_rawfile);

return (e);

error:
if (!reporterror) {
e->e_kind = ELF_K_NONE;
return (e);
}

LIBELF_SET_ERROR(ARCHIVE, 0);
return (NULL);
}
