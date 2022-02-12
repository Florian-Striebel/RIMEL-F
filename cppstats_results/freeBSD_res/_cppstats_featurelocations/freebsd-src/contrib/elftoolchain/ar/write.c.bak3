

























#include <sys/queue.h>
#include <sys/stat.h>

#include <archive.h>
#include <archive_entry.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ar.h"

ELFTC_VCSID("$Id: write.c 3629 2018-09-30 19:26:28Z jkoshy $");

#define _ARMAG_LEN 8
#define _ARHDR_LEN 60
#define _INIT_AS_CAP 128
#define _INIT_SYMOFF_CAP (256*(sizeof(uint32_t)))
#define _INIT_SYMNAME_CAP 1024
#define _MAXNAMELEN_SVR4 15
#define _MAXNAMELEN_BSD 16
#define _TRUNCATE_LEN 15

static void add_to_ar_str_table(struct bsdar *bsdar, const char *name);
static void add_to_ar_sym_table(struct bsdar *bsdar, const char *name);
static struct ar_obj *create_obj_from_file(struct bsdar *bsdar,
const char *name, time_t mtime);
static void create_symtab_entry(struct bsdar *bsdar, Elf *e);
static void free_obj(struct ar_obj *obj);
static void insert_obj(struct bsdar *bsdar, struct ar_obj *obj,
struct ar_obj *pos);
static void read_objs(struct bsdar *bsdar, const char *archive,
int checkargv);
static void write_cleanup(struct bsdar *bsdar);
static void write_data(struct bsdar *bsdar, struct archive *a,
const void *buf, size_t s);
static void write_objs(struct bsdar *bsdar);







static struct ar_obj *
create_obj_from_file(struct bsdar *bsdar, const char *name, time_t mtime)
{
struct ar_obj *obj;
struct stat sb;
const char *bname;
char *tmpname;
int fd;

if (name == NULL)
return (NULL);

obj = malloc(sizeof(struct ar_obj));
if (obj == NULL)
bsdar_errc(bsdar, errno, "malloc failed");

obj->elf = NULL;

if ((fd = open(name, O_RDONLY, 0)) < 0) {
bsdar_warnc(bsdar, errno, "can't open file: %s", name);
free(obj);
return (NULL);
}

tmpname = strdup(name);
if ((bname = basename(tmpname)) == NULL)
bsdar_errc(bsdar, errno, "basename failed");
if (bsdar->options & AR_TR && strlen(bname) > _TRUNCATE_LEN) {
if ((obj->name = malloc(_TRUNCATE_LEN + 1)) == NULL)
bsdar_errc(bsdar, errno, "malloc failed");
(void)strncpy(obj->name, bname, _TRUNCATE_LEN);
obj->name[_TRUNCATE_LEN] = '\0';
} else
if ((obj->name = strdup(bname)) == NULL)
bsdar_errc(bsdar, errno, "strdup failed");
free(tmpname);

if (fstat(fd, &sb) < 0) {
bsdar_warnc(bsdar, errno, "can't fstat file: %s", obj->name);
goto giveup;
}
if (!S_ISREG(sb.st_mode)) {
bsdar_warnc(bsdar, 0, "%s is not an ordinary file", obj->name);
goto giveup;
}

if (sb.st_dev == bsdar->ar_dev && sb.st_ino == bsdar->ar_ino) {
bsdar_warnc(bsdar, 0, "cannot add archive \"%s\" to itself",
obj->name);
goto giveup;
}







if (mtime != 0 && bsdar->options & AR_U && sb.st_mtime <= mtime)
goto giveup;







if (bsdar->options & AR_D) {
obj->uid = 0;
obj->gid = 0;
obj->mtime = 0;
obj->md = S_IFREG | 0644;
} else {
obj->uid = sb.st_uid;
obj->gid = sb.st_gid;
obj->mtime = sb.st_mtime;
obj->md = sb.st_mode;
}
obj->size = sb.st_size;
obj->dev = sb.st_dev;
obj->ino = sb.st_ino;

if (obj->size == 0) {
return (obj);
}

if ((obj->elf = elf_open(fd)) == NULL) {
bsdar_warnc(bsdar, 0, "file initialization failed for %s: %s",
obj->name, elf_errmsg(-1));
goto giveup;
}





if (elf_cntl(obj->elf, ELF_C_FDREAD) < 0) {
bsdar_warnc(bsdar, 0, "%s could not be read in: %s",
obj->name, elf_errmsg(-1));
goto giveup;
}

if (close(fd) < 0)
bsdar_errc(bsdar, errno, "close failed: %s",
obj->name);

return (obj);

giveup:
if (obj->elf)
elf_end(obj->elf);

if (close(fd) < 0)
bsdar_errc(bsdar, errno, "close failed: %s",
obj->name);
free(obj->name);
free(obj);
return (NULL);
}




static void
free_obj(struct ar_obj *obj)
{
if (obj->elf)
elf_end(obj->elf);

free(obj->name);
free(obj);
}





static void
insert_obj(struct bsdar *bsdar, struct ar_obj *obj, struct ar_obj *pos)
{
if (obj == NULL)
bsdar_errc(bsdar, 0, "try to insert a null obj");

if (pos == NULL || obj == pos)





goto tail;

if (bsdar->options & AR_B) {
TAILQ_INSERT_BEFORE(pos, obj, objs);
return;
}
if (bsdar->options & AR_A) {
TAILQ_INSERT_AFTER(&bsdar->v_obj, pos, obj, objs);
return;
}

tail:
TAILQ_INSERT_TAIL(&bsdar->v_obj, obj, objs);

}







static void
read_objs(struct bsdar *bsdar, const char *archive, int checkargv)
{
struct archive *a;
struct archive_entry *entry;
struct ar_obj *obj;
const char *name;
const char *bname;
char *buff;
char **av;
size_t size;
int i, r, find;

if ((a = archive_read_new()) == NULL)
bsdar_errc(bsdar, 0, "archive_read_new failed");
archive_read_support_format_ar(a);
AC(archive_read_open_filename(a, archive, DEF_BLKSZ));
for (;;) {
r = archive_read_next_header(a, &entry);
if (r == ARCHIVE_FATAL)
bsdar_errc(bsdar, 0, "%s", archive_error_string(a));
if (r == ARCHIVE_EOF)
break;
if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
bsdar_warnc(bsdar, 0, "%s", archive_error_string(a));
if (r == ARCHIVE_RETRY) {
bsdar_warnc(bsdar, 0, "Retrying...");
continue;
}

name = archive_entry_pathname(entry);




if (bsdar_is_pseudomember(bsdar, name))
continue;





if (checkargv && bsdar->argc > 0) {
find = 0;
for(i = 0; i < bsdar->argc; i++) {
av = &bsdar->argv[i];
if (*av == NULL)
continue;
if ((bname = basename(*av)) == NULL)
bsdar_errc(bsdar, errno,
"basename failed");
if (strcmp(bname, name) != 0)
continue;

*av = NULL;
find = 1;
break;
}
if (!find)
continue;
}

size = archive_entry_size(entry);

if (size > 0) {
if ((buff = malloc(size)) == NULL)
bsdar_errc(bsdar, errno, "malloc failed");
if (archive_read_data(a, buff, size) != (ssize_t)size) {
bsdar_warnc(bsdar, 0, "%s",
archive_error_string(a));
free(buff);
continue;
}
} else
buff = NULL;

obj = malloc(sizeof(struct ar_obj));
if (obj == NULL)
bsdar_errc(bsdar, errno, "malloc failed");
obj->elf = NULL;
if (buff) {
obj->elf = elf_openmemory(buff, size);
if (obj->elf == NULL) {
bsdar_warnc(bsdar, 0, "elf_openmemory() "
"failed for %s: %s", name,
elf_errmsg(-1));
free(buff);
free(obj);
continue;
}
}
if ((obj->name = strdup(name)) == NULL)
bsdar_errc(bsdar, errno, "strdup failed");
obj->size = size;
obj->uid = archive_entry_uid(entry);
obj->gid = archive_entry_gid(entry);
obj->md = archive_entry_mode(entry);
obj->mtime = archive_entry_mtime(entry);
obj->dev = 0;
obj->ino = 0;

TAILQ_INSERT_TAIL(&bsdar->v_obj, obj, objs);
}
AC(archive_read_close(a));
ACV(archive_read_free(a));
}






int
ar_write_archive(struct bsdar *bsdar, int mode)
{
struct ar_obj *nobj, *obj, *obj_temp, *pos;
struct stat sb;
const char *bname;
char **av;
int exitcode, i;

TAILQ_INIT(&bsdar->v_obj);
exitcode = EXIT_SUCCESS;
nobj = NULL;
pos = NULL;
memset(&sb, 0, sizeof(sb));

assert(mode == 'A' || mode == 'd' || mode == 'm' || mode == 'q' ||
mode == 'r' || mode == 's');





if (stat(bsdar->filename, &sb) != 0) {
if (errno != ENOENT) {
bsdar_warnc(bsdar, errno, "stat %s failed",
bsdar->filename);
return (EXIT_FAILURE);
}


if (mode != 'r' && mode != 'q') {
bsdar_warnc(bsdar, 0, "%s: no such file",
bsdar->filename);
return (EXIT_FAILURE);
}


if (!(bsdar->options & AR_C))
bsdar_warnc(bsdar, 0, "creating %s", bsdar->filename);
goto new_archive;
}

bsdar->ar_dev = sb.st_dev;
bsdar->ar_ino = sb.st_ino;




read_objs(bsdar, bsdar->filename, 0);




if (mode == 's')
goto write_objs;






if (mode == 'q')
goto new_archive;








if (mode == 'A') {






read_objs(bsdar, bsdar->addlib, 1);
goto write_objs;
}




if (bsdar->options & AR_A || bsdar->options & AR_B) {
TAILQ_FOREACH(obj, &bsdar->v_obj, objs) {
if (strcmp(obj->name, bsdar->posarg) == 0) {
pos = obj;
break;
}
}






if (pos == NULL)
bsdar->options &= ~(AR_A | AR_B);
}

for (i = 0; i < bsdar->argc; i++) {
av = &bsdar->argv[i];

TAILQ_FOREACH_SAFE(obj, &bsdar->v_obj, objs, obj_temp) {
if ((bname = basename(*av)) == NULL)
bsdar_errc(bsdar, errno, "basename failed");
if (bsdar->options & AR_TR) {
if (strncmp(bname, obj->name, _TRUNCATE_LEN))
continue;
} else
if (strcmp(bname, obj->name) != 0)
continue;

if (mode == 'r') {




nobj = create_obj_from_file(bsdar, *av,
obj->mtime);
if (nobj == NULL) {
exitcode = EXIT_FAILURE;
goto skip_obj;
}
}

if (bsdar->options & AR_V)
(void)fprintf(bsdar->output, "%c - %s\n",
mode, *av);

TAILQ_REMOVE(&bsdar->v_obj, obj, objs);
if (mode == 'd' || mode == 'r')
free_obj(obj);

if (mode == 'm')
insert_obj(bsdar, obj, pos);
if (mode == 'r')
insert_obj(bsdar, nobj, pos);

skip_obj:
*av = NULL;
break;
}

}

new_archive:







for (i = 0; i < bsdar->argc; i++) {
av = &bsdar->argv[i];
if (*av != NULL && (mode == 'r' || mode == 'q')) {
nobj = create_obj_from_file(bsdar, *av, 0);
if (nobj == NULL) {
exitcode = EXIT_FAILURE;
*av = NULL;
continue;
}
insert_obj(bsdar, nobj, pos);
if (bsdar->options & AR_V)
(void)fprintf(bsdar->output, "a - %s\n", *av);
*av = NULL;
}
}

write_objs:
write_objs(bsdar);
write_cleanup(bsdar);

return (exitcode);
}




static void
write_cleanup(struct bsdar *bsdar)
{
struct ar_obj *obj, *obj_temp;

TAILQ_FOREACH_SAFE(obj, &bsdar->v_obj, objs, obj_temp) {
TAILQ_REMOVE(&bsdar->v_obj, obj, objs);
free_obj(obj);
}

free(bsdar->as);
free(bsdar->s_so);
free(bsdar->s_sn);
bsdar->as = NULL;
bsdar->s_so = NULL;
bsdar->s_sn = NULL;
}




static void
write_data(struct bsdar *bsdar, struct archive *a, const void *buf, size_t s)
{
if (archive_write_data(a, buf, s) != (ssize_t)s)
bsdar_errc(bsdar, 0, "%s", archive_error_string(a));
}




static size_t
bsdar_symtab_size(struct bsdar *bsdar)
{
size_t sz;

if (bsdar->options & AR_BSD) {








sz = 2 * sizeof(long) + (bsdar->s_cnt * 2 * sizeof(long)) +
bsdar->s_sn_sz;
} else {






sz = sizeof(uint32_t) + bsdar->s_cnt * sizeof(uint32_t) +
bsdar->s_sn_sz;
}

return (sz);
}

static void
write_svr4_symtab_entry(struct bsdar *bsdar, struct archive *a)
{
int nr;
uint32_t i;


for (i = 0; i < bsdar->s_cnt; i++)
bsdar->s_so[i] = htobe32(bsdar->s_so[i]);

nr = htobe32(bsdar->s_cnt);
write_data(bsdar, a, &nr, sizeof(uint32_t));
write_data(bsdar, a, bsdar->s_so, sizeof(uint32_t) *
bsdar->s_cnt);
write_data(bsdar, a, bsdar->s_sn, bsdar->s_sn_sz);
}

static void
write_bsd_symtab_entry(struct bsdar *bsdar, struct archive *a)
{
long br_sz, br_off, br_strx;
char *s;
uint32_t i;






br_sz = (long) (bsdar->s_cnt * 2 * sizeof(long));
write_data(bsdar, a, &br_sz, sizeof(long));






for (i = 0, s = bsdar->s_sn; i < bsdar->s_cnt; i++) {
br_strx = (long) (s - bsdar->s_sn);
br_off = (long) bsdar->s_so[i];
write_data(bsdar, a, &br_strx, sizeof(long));
write_data(bsdar, a, &br_off, sizeof(long));


while (*s++ != '\0')
;
}





br_sz = (long) bsdar->s_sn_sz;
write_data(bsdar, a, &br_sz, sizeof(long));
write_data(bsdar, a, bsdar->s_sn, bsdar->s_sn_sz);
}





static void
write_objs(struct bsdar *bsdar)
{
struct ar_obj *obj;
struct archive *a;
struct archive_entry *entry;
size_t s_sz;
size_t pm_sz;
size_t namelen;
size_t obj_sz;
int i;
char *buf;
const char *entry_name;

bsdar->rela_off = 0;





TAILQ_FOREACH(obj, &bsdar->v_obj, objs) {
if (!(bsdar->options & AR_SS) && obj->elf != NULL)
create_symtab_entry(bsdar, obj->elf);

obj_sz = 0;
namelen = strlen(obj->name);
if (bsdar->options & AR_BSD) {

if (namelen > _MAXNAMELEN_BSD ||
strchr(obj->name, ' '))
obj_sz += namelen;
} else if (namelen > _MAXNAMELEN_SVR4)
add_to_ar_str_table(bsdar, obj->name);

obj_sz += obj->size;


bsdar->rela_off += _ARHDR_LEN + obj_sz + (obj_sz & 1);
}







if (bsdar->s_cnt != 0 && bsdar->s_sn_sz % 2 != 0)
bsdar->s_sn[bsdar->s_sn_sz++] = '\0';







if (bsdar->as != NULL && bsdar->as_sz % 2 != 0)
bsdar->as[bsdar->as_sz++] = '\n';









s_sz = bsdar_symtab_size(bsdar);
if (bsdar->s_cnt != 0) {
pm_sz = _ARMAG_LEN + (_ARHDR_LEN + s_sz);
if (bsdar->as != NULL)
pm_sz += _ARHDR_LEN + bsdar->as_sz;
for (i = 0; (size_t) i < bsdar->s_cnt; i++)
bsdar->s_so[i] = bsdar->s_so[i] + pm_sz;
}

if ((a = archive_write_new()) == NULL)
bsdar_errc(bsdar, 0, "archive_write_new failed");

if (bsdar->options & AR_BSD)
archive_write_set_format_ar_bsd(a);
else
archive_write_set_format_ar_svr4(a);

AC(archive_write_open_filename(a, bsdar->filename));






if ((bsdar->s_cnt != 0 && !(bsdar->options & AR_SS)) ||
bsdar->options & AR_S) {
if (bsdar->options & AR_BSD)
entry_name = AR_SYMTAB_NAME_BSD;
else
entry_name = AR_SYMTAB_NAME_SVR4;

entry = archive_entry_new();
archive_entry_copy_pathname(entry, entry_name);
if ((bsdar->options & AR_D) == 0)
archive_entry_set_mtime(entry, time(NULL), 0);
archive_entry_set_size(entry, s_sz);
AC(archive_write_header(a, entry));
if (bsdar->options & AR_BSD)
write_bsd_symtab_entry(bsdar, a);
else
write_svr4_symtab_entry(bsdar, a);
archive_entry_free(entry);
}


if (bsdar->as != NULL) {
entry = archive_entry_new();
archive_entry_copy_pathname(entry, AR_STRINGTAB_NAME_SVR4);
archive_entry_set_size(entry, bsdar->as_sz);
AC(archive_write_header(a, entry));
write_data(bsdar, a, bsdar->as, bsdar->as_sz);
archive_entry_free(entry);
}


TAILQ_FOREACH(obj, &bsdar->v_obj, objs) {
if ((buf = elf_rawfile(obj->elf, NULL)) == NULL) {
bsdar_warnc(bsdar, 0, "elf_rawfile() failed: %s",
elf_errmsg(-1));
continue;
}

entry = archive_entry_new();
archive_entry_copy_pathname(entry, obj->name);
archive_entry_set_uid(entry, obj->uid);
archive_entry_set_gid(entry, obj->gid);
archive_entry_set_mode(entry, obj->md);
archive_entry_set_size(entry, obj->size);
archive_entry_set_mtime(entry, obj->mtime, 0);
archive_entry_set_dev(entry, obj->dev);
archive_entry_set_ino(entry, obj->ino);
archive_entry_set_filetype(entry, AE_IFREG);
AC(archive_write_header(a, entry));
write_data(bsdar, a, buf, obj->size);
archive_entry_free(entry);
}

AC(archive_write_close(a));
ACV(archive_write_free(a));
}




static void
create_symtab_entry(struct bsdar *bsdar, Elf *e)
{
Elf_Scn *scn;
GElf_Shdr shdr;
GElf_Sym sym;
Elf_Data *data;
char *name;
size_t n, shstrndx;
int elferr, tabndx, len, i;

if (elf_kind(e) != ELF_K_ELF) {

return;
}
if (elf_getshstrndx(e, &shstrndx) == 0) {
bsdar_warnc(bsdar, 0, "elf_getshstrndx failed: %s",
elf_errmsg(-1));
return;
}

tabndx = -1;
scn = NULL;
while ((scn = elf_nextscn(e, scn)) != NULL) {
if (gelf_getshdr(scn, &shdr) != &shdr) {
bsdar_warnc(bsdar, 0,
"elf_getshdr failed: %s", elf_errmsg(-1));
continue;
}
if ((name = elf_strptr(e, shstrndx, shdr.sh_name)) == NULL) {
bsdar_warnc(bsdar, 0,
"elf_strptr failed: %s", elf_errmsg(-1));
continue;
}
if (strcmp(name, ".strtab") == 0) {
tabndx = elf_ndxscn(scn);
break;
}
}
elferr = elf_errno();
if (elferr != 0)
bsdar_warnc(bsdar, 0, "elf_nextscn failed: %s",
elf_errmsg(elferr));
if (tabndx == -1) {
bsdar_warnc(bsdar, 0, "can't find .strtab section");
return;
}

scn = NULL;
while ((scn = elf_nextscn(e, scn)) != NULL) {
if (gelf_getshdr(scn, &shdr) != &shdr) {
bsdar_warnc(bsdar, 0, "elf_getshdr failed: %s",
elf_errmsg(-1));
continue;
}
if (shdr.sh_type != SHT_SYMTAB)
continue;

data = NULL;
n = 0;
while (n < shdr.sh_size &&
(data = elf_getdata(scn, data)) != NULL) {
len = data->d_size / shdr.sh_entsize;
for (i = 0; i < len; i++) {
if (gelf_getsym(data, i, &sym) != &sym) {
bsdar_warnc(bsdar, 0,
"gelf_getsym failed: %s",
elf_errmsg(-1));
continue;
}


if (GELF_ST_BIND(sym.st_info) != STB_GLOBAL &&
GELF_ST_BIND(sym.st_info) != STB_WEAK)
continue;


if (sym.st_shndx == SHN_UNDEF)
continue;

if ((name = elf_strptr(e, tabndx,
sym.st_name)) == NULL) {
bsdar_warnc(bsdar, 0,
"elf_strptr failed: %s",
elf_errmsg(-1));
continue;
}

add_to_ar_sym_table(bsdar, name);
}
}
}
elferr = elf_errno();
if (elferr != 0)
bsdar_warnc(bsdar, 0, "elf_nextscn failed: %s",
elf_errmsg(elferr));
}




static void
add_to_ar_str_table(struct bsdar *bsdar, const char *name)
{

if (bsdar->as == NULL) {
bsdar->as_cap = _INIT_AS_CAP;
bsdar->as_sz = 0;
if ((bsdar->as = malloc(bsdar->as_cap)) == NULL)
bsdar_errc(bsdar, errno, "malloc failed");
}






while (bsdar->as_sz + strlen(name) + 3 > bsdar->as_cap) {
bsdar->as_cap *= 2;
bsdar->as = realloc(bsdar->as, bsdar->as_cap);
if (bsdar->as == NULL)
bsdar_errc(bsdar, errno, "realloc failed");
}
strncpy(&bsdar->as[bsdar->as_sz], name, strlen(name));
bsdar->as_sz += strlen(name);
bsdar->as[bsdar->as_sz++] = '/';
bsdar->as[bsdar->as_sz++] = '\n';
}




static void
add_to_ar_sym_table(struct bsdar *bsdar, const char *name)
{

if (bsdar->s_so == NULL) {
if ((bsdar->s_so = malloc(_INIT_SYMOFF_CAP)) ==
NULL)
bsdar_errc(bsdar, errno, "malloc failed");
bsdar->s_so_cap = _INIT_SYMOFF_CAP;
bsdar->s_cnt = 0;
}

if (bsdar->s_sn == NULL) {
if ((bsdar->s_sn = malloc(_INIT_SYMNAME_CAP)) == NULL)
bsdar_errc(bsdar, errno, "malloc failed");
bsdar->s_sn_cap = _INIT_SYMNAME_CAP;
bsdar->s_sn_sz = 0;
}

if (bsdar->s_cnt * sizeof(uint32_t) >= bsdar->s_so_cap) {
bsdar->s_so_cap *= 2;
bsdar->s_so = realloc(bsdar->s_so, bsdar->s_so_cap);
if (bsdar->s_so == NULL)
bsdar_errc(bsdar, errno, "realloc failed");
}
bsdar->s_so[bsdar->s_cnt] = bsdar->rela_off;
bsdar->s_cnt++;






while (bsdar->s_sn_sz + strlen(name) + 2 > bsdar->s_sn_cap) {
bsdar->s_sn_cap *= 2;
bsdar->s_sn = realloc(bsdar->s_sn, bsdar->s_sn_cap);
if (bsdar->s_sn == NULL)
bsdar_errc(bsdar, errno, "realloc failed");
}
strncpy(&bsdar->s_sn[bsdar->s_sn_sz], name, strlen(name));
bsdar->s_sn_sz += strlen(name);
bsdar->s_sn[bsdar->s_sn_sz++] = '\0';
}
