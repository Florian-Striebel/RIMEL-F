#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libelf.h>
#include <gelf.h>
#include <unistd.h>
#include "ctftools.h"
#include "list.h"
#include "memory.h"
#include "traverse.h"
#include "symbol.h"
typedef struct iidesc_match {
int iim_fuzzy;
iidesc_t *iim_ret;
char *iim_name;
char *iim_file;
uchar_t iim_bind;
} iidesc_match_t;
static int
burst_iitypes(void *data, void *arg)
{
iidesc_t *ii = data;
iiburst_t *iiburst = arg;
switch (ii->ii_type) {
case II_GFUN:
case II_SFUN:
case II_GVAR:
case II_SVAR:
if (!(ii->ii_flags & IIDESC_F_USED))
return (0);
break;
default:
break;
}
ii->ii_dtype->t_flags |= TDESC_F_ISROOT;
(void) iitraverse_td(ii, iiburst->iib_tdtd);
return (1);
}
static int
save_type_by_id(tdesc_t *tdp, tdesc_t **tdpp __unused, void *private)
{
iiburst_t *iiburst = private;
if (tdp->t_id > iiburst->iib_maxtypeid)
iiburst->iib_maxtypeid = tdp->t_id;
slist_add(&iiburst->iib_types, tdp, tdesc_idcmp);
return (1);
}
static tdtrav_cb_f burst_types_cbs[] = {
NULL,
save_type_by_id,
save_type_by_id,
save_type_by_id,
save_type_by_id,
save_type_by_id,
save_type_by_id,
save_type_by_id,
save_type_by_id,
save_type_by_id,
tdtrav_assert,
save_type_by_id,
save_type_by_id,
save_type_by_id
};
static iiburst_t *
iiburst_new(tdata_t *td, int max)
{
iiburst_t *iiburst = xcalloc(sizeof (iiburst_t));
iiburst->iib_td = td;
iiburst->iib_funcs = xcalloc(sizeof (iidesc_t *) * max);
iiburst->iib_nfuncs = 0;
iiburst->iib_objts = xcalloc(sizeof (iidesc_t *) * max);
iiburst->iib_nobjts = 0;
return (iiburst);
}
static void
iiburst_types(iiburst_t *iiburst)
{
tdtrav_data_t tdtd;
tdtrav_init(&tdtd, &iiburst->iib_td->td_curvgen, NULL, burst_types_cbs,
NULL, (void *)iiburst);
iiburst->iib_tdtd = &tdtd;
(void) hash_iter(iiburst->iib_td->td_iihash, burst_iitypes, iiburst);
}
static void
iiburst_free(iiburst_t *iiburst)
{
free(iiburst->iib_funcs);
free(iiburst->iib_objts);
list_free(iiburst->iib_types, NULL, NULL);
free(iiburst);
}
static int
matching_iidesc(void *arg1, void *arg2)
{
iidesc_t *iidesc = arg1;
iidesc_match_t *match = arg2;
if (streq(iidesc->ii_name, match->iim_name) == 0)
return (0);
switch (iidesc->ii_type) {
case II_GFUN:
case II_GVAR:
if (match->iim_bind == STB_GLOBAL) {
match->iim_ret = iidesc;
return (-1);
} else if (match->iim_fuzzy && match->iim_ret == NULL) {
match->iim_ret = iidesc;
return (0);
}
break;
case II_SFUN:
case II_SVAR:
if (match->iim_bind == STB_LOCAL &&
match->iim_file != NULL &&
streq(iidesc->ii_owner, match->iim_file)) {
match->iim_ret = iidesc;
return (-1);
}
break;
default:
break;
}
return (0);
}
static iidesc_t *
find_iidesc(tdata_t *td, iidesc_match_t *match)
{
match->iim_ret = NULL;
iter_iidescs_by_name(td, match->iim_name,
matching_iidesc, match);
return (match->iim_ret);
}
static int
check_for_weak(GElf_Sym *weak, char const *weakfile,
Elf_Data *data, int nent, Elf_Data *strdata,
GElf_Sym *retsym, char **curfilep)
{
char *curfile = NULL;
char *tmpfile1 = NULL;
GElf_Sym tmpsym;
int candidate = 0;
int i;
tmpsym.st_info = 0;
tmpsym.st_name = 0;
if (GELF_ST_BIND(weak->st_info) != STB_WEAK)
return (0);
for (i = 0; i < nent; i++) {
GElf_Sym sym;
uchar_t type;
if (gelf_getsym(data, i, &sym) == NULL)
continue;
type = GELF_ST_TYPE(sym.st_info);
if (type == STT_FILE)
curfile = (char *)strdata->d_buf + sym.st_name;
if (GELF_ST_TYPE(weak->st_info) != type ||
weak->st_value != sym.st_value)
continue;
if (weak->st_size != sym.st_size)
continue;
if (GELF_ST_BIND(sym.st_info) == STB_WEAK)
continue;
if (sym.st_shndx != weak->st_shndx)
continue;
if (GELF_ST_BIND(sym.st_info) == STB_LOCAL &&
(curfile == NULL || weakfile == NULL ||
strcmp(curfile, weakfile) != 0)) {
candidate = 1;
tmpfile1 = curfile;
tmpsym = sym;
continue;
}
*curfilep = curfile;
*retsym = sym;
return (1);
}
if (candidate) {
*curfilep = tmpfile1;
*retsym = tmpsym;
return (1);
}
return (0);
}
static iidesc_t *
copy_from_strong(tdata_t *td, GElf_Sym *sym, iidesc_t *strongdesc,
const char *weakname, const char *weakfile)
{
iidesc_t *new = iidesc_dup_rename(strongdesc, weakname, weakfile);
uchar_t type = GELF_ST_TYPE(sym->st_info);
switch (type) {
case STT_OBJECT:
new->ii_type = II_GVAR;
break;
case STT_FUNC:
new->ii_type = II_GFUN;
break;
}
hash_add(td->td_iihash, new);
return (new);
}
static iiburst_t *
sort_iidescs(Elf *elf, const char *file, tdata_t *td, int fuzzymatch,
int dynsym)
{
iiburst_t *iiburst;
Elf_Scn *scn;
GElf_Shdr shdr;
Elf_Data *data, *strdata;
int i, stidx;
int nent;
iidesc_match_t match;
match.iim_fuzzy = fuzzymatch;
match.iim_file = NULL;
if ((stidx = findelfsecidx(elf, file,
dynsym ? ".dynsym" : ".symtab")) < 0)
terminate("%s: Can't open symbol table\n", file);
scn = elf_getscn(elf, stidx);
data = elf_getdata(scn, NULL);
gelf_getshdr(scn, &shdr);
nent = shdr.sh_size / shdr.sh_entsize;
scn = elf_getscn(elf, shdr.sh_link);
strdata = elf_getdata(scn, NULL);
iiburst = iiburst_new(td, nent);
for (i = 0; i < nent; i++) {
GElf_Sym sym;
char *bname;
iidesc_t **tolist;
GElf_Sym ssym;
iidesc_match_t smatch;
int *curr;
iidesc_t *iidesc;
if (gelf_getsym(data, i, &sym) == NULL)
elfterminate(file, "Couldn't read symbol %d", i);
match.iim_name = (char *)strdata->d_buf + sym.st_name;
match.iim_bind = GELF_ST_BIND(sym.st_info);
switch (GELF_ST_TYPE(sym.st_info)) {
case STT_FILE:
bname = strrchr(match.iim_name, '/');
match.iim_file = bname == NULL ? match.iim_name : bname + 1;
continue;
case STT_OBJECT:
tolist = iiburst->iib_objts;
curr = &iiburst->iib_nobjts;
break;
case STT_FUNC:
tolist = iiburst->iib_funcs;
curr = &iiburst->iib_nfuncs;
break;
default:
continue;
}
if (ignore_symbol(&sym, match.iim_name))
continue;
iidesc = find_iidesc(td, &match);
if (iidesc != NULL) {
tolist[*curr] = iidesc;
iidesc->ii_flags |= IIDESC_F_USED;
(*curr)++;
continue;
}
if (!check_for_weak(&sym, match.iim_file, data, nent, strdata,
&ssym, &smatch.iim_file)) {
(*curr)++;
continue;
}
smatch.iim_fuzzy = fuzzymatch;
smatch.iim_name = (char *)strdata->d_buf + ssym.st_name;
smatch.iim_bind = GELF_ST_BIND(ssym.st_info);
debug(3, "Weak symbol %s resolved to %s\n", match.iim_name,
smatch.iim_name);
iidesc = find_iidesc(td, &smatch);
if (iidesc != NULL) {
tolist[*curr] = copy_from_strong(td, &sym,
iidesc, match.iim_name, match.iim_file);
tolist[*curr]->ii_flags |= IIDESC_F_USED;
}
(*curr)++;
}
iiburst_types(iiburst);
tdata_label_newmax(td, iiburst->iib_maxtypeid);
return (iiburst);
}
static void
write_file(Elf *src, const char *srcname, Elf *dst, const char *dstname,
caddr_t ctfdata, size_t ctfsize, int flags)
{
GElf_Ehdr sehdr, dehdr;
Elf_Scn *sscn, *dscn;
Elf_Data *sdata, *ddata;
GElf_Shdr shdr;
GElf_Word symtab_type;
int symtab_idx = -1;
off_t new_offset = 0;
off_t ctfnameoff = 0;
int dynsym = (flags & CTF_USE_DYNSYM);
int keep_stabs = (flags & CTF_KEEP_STABS);
int *secxlate;
int srcidx, dstidx;
int curnmoff = 0;
int changing = 0;
int pad;
int i;
if (gelf_newehdr(dst, gelf_getclass(src)) == NULL)
elfterminate(dstname, "Cannot copy ehdr to temp file");
gelf_getehdr(src, &sehdr);
memcpy(&dehdr, &sehdr, sizeof (GElf_Ehdr));
gelf_update_ehdr(dst, &dehdr);
symtab_type = dynsym ? SHT_DYNSYM : SHT_SYMTAB;
if (sehdr.e_phnum != 0) {
(void) elf_flagelf(dst, ELF_C_SET, ELF_F_LAYOUT);
if (gelf_newphdr(dst, sehdr.e_phnum) == NULL)
elfterminate(dstname, "Cannot make phdrs in temp file");
for (i = 0; i < sehdr.e_phnum; i++) {
GElf_Phdr phdr;
gelf_getphdr(src, i, &phdr);
gelf_update_phdr(dst, i, &phdr);
}
}
secxlate = xmalloc(sizeof (int) * sehdr.e_shnum);
for (srcidx = dstidx = 0; srcidx < sehdr.e_shnum; srcidx++) {
Elf_Scn *scn = elf_getscn(src, srcidx);
GElf_Shdr shdr1;
char *sname;
gelf_getshdr(scn, &shdr1);
sname = elf_strptr(src, sehdr.e_shstrndx, shdr1.sh_name);
if (sname == NULL) {
elfterminate(srcname, "Can't find string at %u",
shdr1.sh_name);
}
if (strcmp(sname, CTF_ELF_SCN_NAME) == 0) {
secxlate[srcidx] = -1;
} else if (!keep_stabs &&
(strncmp(sname, ".stab", 5) == 0 ||
strncmp(sname, ".debug", 6) == 0 ||
strncmp(sname, ".rel.debug", 10) == 0 ||
strncmp(sname, ".rela.debug", 11) == 0)) {
secxlate[srcidx] = -1;
} else if (dynsym && shdr1.sh_type == SHT_SYMTAB) {
secxlate[srcidx] = -1;
} else {
secxlate[srcidx] = dstidx++;
curnmoff += strlen(sname) + 1;
}
new_offset = (off_t)dehdr.e_phoff;
}
for (srcidx = 1; srcidx < sehdr.e_shnum; srcidx++) {
char *sname;
sscn = elf_getscn(src, srcidx);
gelf_getshdr(sscn, &shdr);
if (secxlate[srcidx] == -1) {
changing = 1;
continue;
}
dscn = elf_newscn(dst);
if (changing && sehdr.e_phnum != 0) {
pad = new_offset % shdr.sh_addralign;
if (pad)
new_offset += shdr.sh_addralign - pad;
shdr.sh_offset = new_offset;
}
shdr.sh_link = secxlate[shdr.sh_link];
if (shdr.sh_type == SHT_REL || shdr.sh_type == SHT_RELA)
shdr.sh_info = secxlate[shdr.sh_info];
sname = elf_strptr(src, sehdr.e_shstrndx, shdr.sh_name);
if (sname == NULL) {
elfterminate(srcname, "Can't find string at %u",
shdr.sh_name);
}
#if !defined(illumos)
if (gelf_update_shdr(dscn, &shdr) == 0)
elfterminate(dstname, "Cannot update sect %s", sname);
#endif
if ((sdata = elf_getdata(sscn, NULL)) == NULL)
elfterminate(srcname, "Cannot get sect %s data", sname);
if ((ddata = elf_newdata(dscn)) == NULL)
elfterminate(dstname, "Can't make sect %s data", sname);
#if defined(illumos)
bcopy(sdata, ddata, sizeof (Elf_Data));
#else
ddata->d_align = sdata->d_align;
ddata->d_off = sdata->d_off;
ddata->d_size = sdata->d_size;
ddata->d_type = sdata->d_type;
ddata->d_version = sdata->d_version;
#endif
if (srcidx == sehdr.e_shstrndx) {
char seclen = strlen(CTF_ELF_SCN_NAME);
ddata->d_buf = xmalloc(ddata->d_size + shdr.sh_size +
seclen + 1);
bcopy(sdata->d_buf, ddata->d_buf, shdr.sh_size);
strcpy((caddr_t)ddata->d_buf + shdr.sh_size,
CTF_ELF_SCN_NAME);
ctfnameoff = (off_t)shdr.sh_size;
shdr.sh_size += seclen + 1;
ddata->d_size += seclen + 1;
if (sehdr.e_phnum != 0)
changing = 1;
}
if (shdr.sh_type == symtab_type && shdr.sh_entsize != 0) {
int nsym = shdr.sh_size / shdr.sh_entsize;
symtab_idx = secxlate[srcidx];
ddata->d_buf = xmalloc(shdr.sh_size);
bcopy(sdata->d_buf, ddata->d_buf, shdr.sh_size);
for (i = 0; i < nsym; i++) {
GElf_Sym sym;
short newscn;
if (gelf_getsym(ddata, i, &sym) == NULL)
printf("Could not get symbol %d\n",i);
if (sym.st_shndx >= SHN_LORESERVE)
continue;
if ((newscn = secxlate[sym.st_shndx]) !=
sym.st_shndx) {
sym.st_shndx =
(newscn == -1 ? 1 : newscn);
gelf_update_sym(ddata, i, &sym);
}
}
}
#if !defined(illumos)
if (ddata->d_buf == NULL && sdata->d_buf != NULL) {
ddata->d_buf = xmalloc(shdr.sh_size);
bcopy(sdata->d_buf, ddata->d_buf, shdr.sh_size);
}
#endif
if (gelf_update_shdr(dscn, &shdr) == 0)
elfterminate(dstname, "Cannot update sect %s", sname);
new_offset = (off_t)shdr.sh_offset;
if (shdr.sh_type != SHT_NOBITS)
new_offset += shdr.sh_size;
}
if (symtab_idx == -1) {
terminate("%s: Cannot find %s section\n", srcname,
dynsym ? "SHT_DYNSYM" : "SHT_SYMTAB");
}
dscn = elf_newscn(dst);
gelf_getshdr(dscn, &shdr);
shdr.sh_name = ctfnameoff;
shdr.sh_type = SHT_PROGBITS;
shdr.sh_size = ctfsize;
shdr.sh_link = symtab_idx;
shdr.sh_addralign = 4;
if (changing && sehdr.e_phnum != 0) {
pad = new_offset % shdr.sh_addralign;
if (pad)
new_offset += shdr.sh_addralign - pad;
shdr.sh_offset = new_offset;
new_offset += shdr.sh_size;
}
ddata = elf_newdata(dscn);
ddata->d_buf = ctfdata;
ddata->d_size = ctfsize;
ddata->d_align = shdr.sh_addralign;
ddata->d_off = 0;
gelf_update_shdr(dscn, &shdr);
if (sehdr.e_phnum != 0) {
size_t align = gelf_fsize(dst, ELF_T_ADDR, 1, EV_CURRENT);
size_t r = new_offset % align;
if (r)
new_offset += align - r;
dehdr.e_shoff = new_offset;
}
dehdr.e_shstrndx = secxlate[sehdr.e_shstrndx];
gelf_update_ehdr(dst, &dehdr);
if (elf_update(dst, ELF_C_WRITE) < 0)
elfterminate(dstname, "Cannot finalize temp file");
free(secxlate);
}
static caddr_t
make_ctf_data(tdata_t *td, Elf *elf, const char *file, size_t *lenp, int flags)
{
iiburst_t *iiburst;
caddr_t data;
iiburst = sort_iidescs(elf, file, td, flags & CTF_FUZZY_MATCH,
flags & CTF_USE_DYNSYM);
data = ctf_gen(iiburst, lenp, flags & (CTF_COMPRESS | CTF_SWAP_BYTES));
iiburst_free(iiburst);
return (data);
}
void
write_ctf(tdata_t *td, const char *curname, const char *newname, int flags)
{
struct stat st;
Elf *elf = NULL;
Elf *telf = NULL;
GElf_Ehdr ehdr;
caddr_t data;
size_t len;
int fd = -1;
int tfd = -1;
int byteorder;
(void) elf_version(EV_CURRENT);
if ((fd = open(curname, O_RDONLY)) < 0 || fstat(fd, &st) < 0)
terminate("%s: Cannot open for re-reading", curname);
if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
elfterminate(curname, "Cannot re-read");
if ((tfd = open(newname, O_RDWR | O_CREAT | O_TRUNC, st.st_mode)) < 0)
terminate("Cannot open temp file %s for writing", newname);
if ((telf = elf_begin(tfd, ELF_C_WRITE, NULL)) == NULL)
elfterminate(curname, "Cannot write");
if (gelf_getehdr(elf, &ehdr)) {
#if BYTE_ORDER == _BIG_ENDIAN
byteorder = ELFDATA2MSB;
#else
byteorder = ELFDATA2LSB;
#endif
if (ehdr.e_ident[EI_DATA] == byteorder)
flags &= ~CTF_SWAP_BYTES;
}
else
elfterminate(curname, "Failed to get EHDR");
data = make_ctf_data(td, elf, curname, &len, flags);
write_file(elf, curname, telf, newname, data, len, flags);
free(data);
elf_end(telf);
elf_end(elf);
(void) close(fd);
(void) close(tfd);
}
