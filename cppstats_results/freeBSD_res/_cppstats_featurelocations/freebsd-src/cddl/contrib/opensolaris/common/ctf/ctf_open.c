





























#include <ctf_impl.h>
#include <sys/mman.h>
#include <sys/zmod.h>

static const ctf_dmodel_t _libctf_models[] = {
{ "ILP32", CTF_MODEL_ILP32, 4, 1, 2, 4, 4 },
{ "LP64", CTF_MODEL_LP64, 8, 1, 2, 4, 8 },
{ NULL, 0, 0, 0, 0, 0, 0 }
};

const char _CTF_SECTION[] = ".SUNW_ctf";
const char _CTF_NULLSTR[] = "";

int _libctf_version = CTF_VERSION;
int _libctf_debug = 0;

static ushort_t
get_kind_v1(ushort_t info)
{
return (CTF_INFO_KIND_V1(info));
}

static ushort_t
get_kind_v2(ushort_t info)
{
return (CTF_INFO_KIND(info));
}

static ushort_t
get_root_v1(ushort_t info)
{
return (CTF_INFO_ISROOT_V1(info));
}

static ushort_t
get_root_v2(ushort_t info)
{
return (CTF_INFO_ISROOT(info));
}

static ushort_t
get_vlen_v1(ushort_t info)
{
return (CTF_INFO_VLEN_V1(info));
}

static ushort_t
get_vlen_v2(ushort_t info)
{
return (CTF_INFO_VLEN(info));
}

static const ctf_fileops_t ctf_fileops[] = {
{ NULL, NULL },
{ get_kind_v1, get_root_v1, get_vlen_v1 },
{ get_kind_v2, get_root_v2, get_vlen_v2 },
};




static Elf64_Sym *
sym_to_gelf(const Elf32_Sym *src, Elf64_Sym *dst)
{
dst->st_name = src->st_name;
dst->st_value = src->st_value;
dst->st_size = src->st_size;
dst->st_info = src->st_info;
dst->st_other = src->st_other;
dst->st_shndx = src->st_shndx;

return (dst);
}






static int
init_symtab(ctf_file_t *fp, const ctf_header_t *hp,
const ctf_sect_t *sp, const ctf_sect_t *strp)
{
const uchar_t *symp = sp->cts_data;
uint_t *xp = fp->ctf_sxlate;
uint_t *xend = xp + fp->ctf_nsyms;

uint_t objtoff = hp->cth_objtoff;
uint_t funcoff = hp->cth_funcoff;

ushort_t info, vlen;
Elf64_Sym sym, *gsp;
const char *name;








for (; xp < xend; xp++, symp += sp->cts_entsize) {
if (sp->cts_entsize == sizeof (Elf32_Sym))
gsp = sym_to_gelf((Elf32_Sym *)(uintptr_t)symp, &sym);
else
gsp = (Elf64_Sym *)(uintptr_t)symp;

if (gsp->st_name < strp->cts_size)
name = (const char *)strp->cts_data + gsp->st_name;
else
name = _CTF_NULLSTR;

if (gsp->st_name == 0 || gsp->st_shndx == SHN_UNDEF ||
strcmp(name, "_START_") == 0 ||
strcmp(name, "_END_") == 0) {
*xp = -1u;
continue;
}

switch (ELF64_ST_TYPE(gsp->st_info)) {
case STT_OBJECT:
if (objtoff >= hp->cth_funcoff ||
(gsp->st_shndx == SHN_ABS && gsp->st_value == 0)) {
*xp = -1u;
break;
}

*xp = objtoff;
objtoff += sizeof (ushort_t);
break;

case STT_FUNC:
if (funcoff >= hp->cth_typeoff) {
*xp = -1u;
break;
}

*xp = funcoff;

info = *(ushort_t *)((uintptr_t)fp->ctf_buf + funcoff);
vlen = LCTF_INFO_VLEN(fp, info);






if (LCTF_INFO_KIND(fp, info) == CTF_K_UNKNOWN &&
vlen == 0)
funcoff += sizeof (ushort_t);
else
funcoff += sizeof (ushort_t) * (vlen + 2);
break;

default:
*xp = -1u;
break;
}
}

ctf_dprintf("loaded %lu symtab entries\n", fp->ctf_nsyms);
return (0);
}





static int
init_types(ctf_file_t *fp, const ctf_header_t *cth)
{

const ctf_type_t *tbuf = (ctf_type_t *)(fp->ctf_buf + cth->cth_typeoff);

const ctf_type_t *tend = (ctf_type_t *)(fp->ctf_buf + cth->cth_stroff);

ulong_t pop[CTF_K_MAX + 1] = { 0 };
const ctf_type_t *tp;
ctf_hash_t *hp;
ushort_t id, dst;
uint_t *xp;







int child = cth->cth_parname != 0;
int nlstructs = 0, nlunions = 0;
int err;





for (tp = tbuf; tp < tend; fp->ctf_typemax++) {
ushort_t kind = LCTF_INFO_KIND(fp, tp->ctt_info);
ulong_t vlen = LCTF_INFO_VLEN(fp, tp->ctt_info);
ssize_t size, increment;

size_t vbytes;
uint_t n;

(void) ctf_get_ctt_size(fp, tp, &size, &increment);

switch (kind) {
case CTF_K_INTEGER:
case CTF_K_FLOAT:
vbytes = sizeof (uint_t);
break;
case CTF_K_ARRAY:
vbytes = sizeof (ctf_array_t);
break;
case CTF_K_FUNCTION:
vbytes = sizeof (ushort_t) * (vlen + (vlen & 1));
break;
case CTF_K_STRUCT:
case CTF_K_UNION:
if (fp->ctf_version == CTF_VERSION_1 ||
size < CTF_LSTRUCT_THRESH) {
ctf_member_t *mp = (ctf_member_t *)
((uintptr_t)tp + increment);

vbytes = sizeof (ctf_member_t) * vlen;
for (n = vlen; n != 0; n--, mp++)
child |= CTF_TYPE_ISCHILD(mp->ctm_type);
} else {
ctf_lmember_t *lmp = (ctf_lmember_t *)
((uintptr_t)tp + increment);

vbytes = sizeof (ctf_lmember_t) * vlen;
for (n = vlen; n != 0; n--, lmp++)
child |=
CTF_TYPE_ISCHILD(lmp->ctlm_type);
}
break;
case CTF_K_ENUM:
vbytes = sizeof (ctf_enum_t) * vlen;
break;
case CTF_K_FORWARD:





if (tp->ctt_type == CTF_K_UNKNOWN ||
tp->ctt_type >= CTF_K_MAX)
pop[CTF_K_STRUCT]++;
else
pop[tp->ctt_type]++;

case CTF_K_UNKNOWN:
vbytes = 0;
break;
case CTF_K_POINTER:
case CTF_K_TYPEDEF:
case CTF_K_VOLATILE:
case CTF_K_CONST:
case CTF_K_RESTRICT:
child |= CTF_TYPE_ISCHILD(tp->ctt_type);
vbytes = 0;
break;
default:
ctf_dprintf("detected invalid CTF kind -- %u\n", kind);
return (ECTF_CORRUPT);
}
tp = (ctf_type_t *)((uintptr_t)tp + increment + vbytes);
pop[kind]++;
}





if (child) {
ctf_dprintf("CTF container %p is a child\n", (void *)fp);
fp->ctf_flags |= LCTF_CHILD;
} else
ctf_dprintf("CTF container %p is a parent\n", (void *)fp);





if ((err = ctf_hash_create(&fp->ctf_structs, pop[CTF_K_STRUCT])) != 0)
return (err);

if ((err = ctf_hash_create(&fp->ctf_unions, pop[CTF_K_UNION])) != 0)
return (err);

if ((err = ctf_hash_create(&fp->ctf_enums, pop[CTF_K_ENUM])) != 0)
return (err);

if ((err = ctf_hash_create(&fp->ctf_names,
pop[CTF_K_INTEGER] + pop[CTF_K_FLOAT] + pop[CTF_K_FUNCTION] +
pop[CTF_K_TYPEDEF] + pop[CTF_K_POINTER] + pop[CTF_K_VOLATILE] +
pop[CTF_K_CONST] + pop[CTF_K_RESTRICT])) != 0)
return (err);

fp->ctf_txlate = ctf_alloc(sizeof (uint_t) * (fp->ctf_typemax + 1));
fp->ctf_ptrtab = ctf_alloc(sizeof (ushort_t) * (fp->ctf_typemax + 1));

if (fp->ctf_txlate == NULL || fp->ctf_ptrtab == NULL)
return (EAGAIN);

xp = fp->ctf_txlate;
*xp++ = 0;

bzero(fp->ctf_txlate, sizeof (uint_t) * (fp->ctf_typemax + 1));
bzero(fp->ctf_ptrtab, sizeof (ushort_t) * (fp->ctf_typemax + 1));





for (id = 1, tp = tbuf; tp < tend; xp++, id++) {
ushort_t kind = LCTF_INFO_KIND(fp, tp->ctt_info);
ulong_t vlen = LCTF_INFO_VLEN(fp, tp->ctt_info);
ssize_t size, increment;

const char *name;
size_t vbytes;
ctf_helem_t *hep;
ctf_encoding_t cte;

(void) ctf_get_ctt_size(fp, tp, &size, &increment);
name = ctf_strptr(fp, tp->ctt_name);

switch (kind) {
case CTF_K_INTEGER:
case CTF_K_FLOAT:





if ((hep = ctf_hash_lookup(&fp->ctf_names, fp,
name, strlen(name))) == NULL) {
err = ctf_hash_insert(&fp->ctf_names, fp,
CTF_INDEX_TO_TYPE(id, child), tp->ctt_name);
if (err != 0 && err != ECTF_STRTAB)
return (err);
} else if (ctf_type_encoding(fp, hep->h_type,
&cte) == 0 && cte.cte_bits == 0) {




hep->h_type = CTF_INDEX_TO_TYPE(id, child);
}
vbytes = sizeof (uint_t);
break;

case CTF_K_ARRAY:
vbytes = sizeof (ctf_array_t);
break;

case CTF_K_FUNCTION:
err = ctf_hash_insert(&fp->ctf_names, fp,
CTF_INDEX_TO_TYPE(id, child), tp->ctt_name);
if (err != 0 && err != ECTF_STRTAB)
return (err);
vbytes = sizeof (ushort_t) * (vlen + (vlen & 1));
break;

case CTF_K_STRUCT:
err = ctf_hash_define(&fp->ctf_structs, fp,
CTF_INDEX_TO_TYPE(id, child), tp->ctt_name);

if (err != 0 && err != ECTF_STRTAB)
return (err);

if (fp->ctf_version == CTF_VERSION_1 ||
size < CTF_LSTRUCT_THRESH)
vbytes = sizeof (ctf_member_t) * vlen;
else {
vbytes = sizeof (ctf_lmember_t) * vlen;
nlstructs++;
}
break;

case CTF_K_UNION:
err = ctf_hash_define(&fp->ctf_unions, fp,
CTF_INDEX_TO_TYPE(id, child), tp->ctt_name);

if (err != 0 && err != ECTF_STRTAB)
return (err);

if (fp->ctf_version == CTF_VERSION_1 ||
size < CTF_LSTRUCT_THRESH)
vbytes = sizeof (ctf_member_t) * vlen;
else {
vbytes = sizeof (ctf_lmember_t) * vlen;
nlunions++;
}
break;

case CTF_K_ENUM:
err = ctf_hash_define(&fp->ctf_enums, fp,
CTF_INDEX_TO_TYPE(id, child), tp->ctt_name);

if (err != 0 && err != ECTF_STRTAB)
return (err);

vbytes = sizeof (ctf_enum_t) * vlen;
break;

case CTF_K_TYPEDEF:
err = ctf_hash_insert(&fp->ctf_names, fp,
CTF_INDEX_TO_TYPE(id, child), tp->ctt_name);
if (err != 0 && err != ECTF_STRTAB)
return (err);
vbytes = 0;
break;

case CTF_K_FORWARD:




switch (tp->ctt_type) {
case CTF_K_STRUCT:
hp = &fp->ctf_structs;
break;
case CTF_K_UNION:
hp = &fp->ctf_unions;
break;
case CTF_K_ENUM:
hp = &fp->ctf_enums;
break;
default:
hp = &fp->ctf_structs;
}

if (ctf_hash_lookup(hp, fp,
name, strlen(name)) == NULL) {
err = ctf_hash_insert(hp, fp,
CTF_INDEX_TO_TYPE(id, child), tp->ctt_name);
if (err != 0 && err != ECTF_STRTAB)
return (err);
}
vbytes = 0;
break;

case CTF_K_POINTER:





if (CTF_TYPE_ISCHILD(tp->ctt_type) == child &&
CTF_TYPE_TO_INDEX(tp->ctt_type) <= fp->ctf_typemax)
fp->ctf_ptrtab[
CTF_TYPE_TO_INDEX(tp->ctt_type)] = id;


case CTF_K_VOLATILE:
case CTF_K_CONST:
case CTF_K_RESTRICT:
err = ctf_hash_insert(&fp->ctf_names, fp,
CTF_INDEX_TO_TYPE(id, child), tp->ctt_name);
if (err != 0 && err != ECTF_STRTAB)
return (err);


default:
vbytes = 0;
break;
}

*xp = (uint_t)((uintptr_t)tp - (uintptr_t)fp->ctf_buf);
tp = (ctf_type_t *)((uintptr_t)tp + increment + vbytes);
}

ctf_dprintf("%lu total types processed\n", fp->ctf_typemax);
ctf_dprintf("%u enum names hashed\n", ctf_hash_size(&fp->ctf_enums));
ctf_dprintf("%u struct names hashed (%d long)\n",
ctf_hash_size(&fp->ctf_structs), nlstructs);
ctf_dprintf("%u union names hashed (%d long)\n",
ctf_hash_size(&fp->ctf_unions), nlunions);
ctf_dprintf("%u base type names hashed\n",
ctf_hash_size(&fp->ctf_names));







for (id = 1; id <= fp->ctf_typemax; id++) {
if ((dst = fp->ctf_ptrtab[id]) != 0) {
tp = LCTF_INDEX_TO_TYPEPTR(fp, id);

if (LCTF_INFO_KIND(fp, tp->ctt_info) == CTF_K_TYPEDEF &&
strcmp(ctf_strptr(fp, tp->ctt_name), "") == 0 &&
CTF_TYPE_ISCHILD(tp->ctt_type) == child &&
CTF_TYPE_TO_INDEX(tp->ctt_type) <= fp->ctf_typemax)
fp->ctf_ptrtab[
CTF_TYPE_TO_INDEX(tp->ctt_type)] = dst;
}
}

return (0);
}







ctf_file_t *
ctf_bufopen(const ctf_sect_t *ctfsect, const ctf_sect_t *symsect,
const ctf_sect_t *strsect, int *errp)
{
const ctf_preamble_t *pp;
ctf_header_t hp;
ctf_file_t *fp;
void *buf, *base;
size_t size, hdrsz;
int err;

if (ctfsect == NULL || ((symsect == NULL) != (strsect == NULL)))
return (ctf_set_open_errno(errp, EINVAL));

if (symsect != NULL && symsect->cts_entsize != sizeof (Elf32_Sym) &&
symsect->cts_entsize != sizeof (Elf64_Sym))
return (ctf_set_open_errno(errp, ECTF_SYMTAB));

if (symsect != NULL && symsect->cts_data == NULL)
return (ctf_set_open_errno(errp, ECTF_SYMBAD));

if (strsect != NULL && strsect->cts_data == NULL)
return (ctf_set_open_errno(errp, ECTF_STRBAD));

if (ctfsect->cts_size < sizeof (ctf_preamble_t))
return (ctf_set_open_errno(errp, ECTF_NOCTFBUF));

pp = (const ctf_preamble_t *)ctfsect->cts_data;

ctf_dprintf("ctf_bufopen: magic=0x%x version=%u\n",
pp->ctp_magic, pp->ctp_version);







if (pp->ctp_magic != CTF_MAGIC)
return (ctf_set_open_errno(errp, ECTF_NOCTFBUF));

if (pp->ctp_version == CTF_VERSION_2) {
if (ctfsect->cts_size < sizeof (ctf_header_t))
return (ctf_set_open_errno(errp, ECTF_NOCTFBUF));

bcopy(ctfsect->cts_data, &hp, sizeof (hp));
hdrsz = sizeof (ctf_header_t);

} else if (pp->ctp_version == CTF_VERSION_1) {
const ctf_header_v1_t *h1p =
(const ctf_header_v1_t *)ctfsect->cts_data;

if (ctfsect->cts_size < sizeof (ctf_header_v1_t))
return (ctf_set_open_errno(errp, ECTF_NOCTFBUF));

bzero(&hp, sizeof (hp));
hp.cth_preamble = h1p->cth_preamble;
hp.cth_objtoff = h1p->cth_objtoff;
hp.cth_funcoff = h1p->cth_funcoff;
hp.cth_typeoff = h1p->cth_typeoff;
hp.cth_stroff = h1p->cth_stroff;
hp.cth_strlen = h1p->cth_strlen;

hdrsz = sizeof (ctf_header_v1_t);
} else
return (ctf_set_open_errno(errp, ECTF_CTFVERS));

size = hp.cth_stroff + hp.cth_strlen;

ctf_dprintf("ctf_bufopen: uncompressed size=%lu\n", (ulong_t)size);

if (hp.cth_lbloff > size || hp.cth_objtoff > size ||
hp.cth_funcoff > size || hp.cth_typeoff > size ||
hp.cth_stroff > size)
return (ctf_set_open_errno(errp, ECTF_CORRUPT));

if (hp.cth_lbloff > hp.cth_objtoff ||
hp.cth_objtoff > hp.cth_funcoff ||
hp.cth_funcoff > hp.cth_typeoff ||
hp.cth_typeoff > hp.cth_stroff)
return (ctf_set_open_errno(errp, ECTF_CORRUPT));

if ((hp.cth_lbloff & 3) || (hp.cth_objtoff & 1) ||
(hp.cth_funcoff & 1) || (hp.cth_typeoff & 3))
return (ctf_set_open_errno(errp, ECTF_CORRUPT));






if (hp.cth_flags & CTF_F_COMPRESS) {
size_t srclen, dstlen;
const void *src;
int rc = Z_OK;

if (ctf_zopen(errp) == NULL)
return (NULL);

if ((base = ctf_data_alloc(size + hdrsz)) == MAP_FAILED)
return (ctf_set_open_errno(errp, ECTF_ZALLOC));

bcopy(ctfsect->cts_data, base, hdrsz);
((ctf_preamble_t *)base)->ctp_flags &= ~CTF_F_COMPRESS;
buf = (uchar_t *)base + hdrsz;

src = (uchar_t *)ctfsect->cts_data + hdrsz;
srclen = ctfsect->cts_size - hdrsz;
dstlen = size;

if ((rc = z_uncompress(buf, &dstlen, src, srclen)) != Z_OK) {
ctf_dprintf("zlib inflate err: %s\n", z_strerror(rc));
ctf_data_free(base, size + hdrsz);
return (ctf_set_open_errno(errp, ECTF_DECOMPRESS));
}

if (dstlen != size) {
ctf_dprintf("zlib inflate short -- got %lu of %lu "
"bytes\n", (ulong_t)dstlen, (ulong_t)size);
ctf_data_free(base, size + hdrsz);
return (ctf_set_open_errno(errp, ECTF_CORRUPT));
}

ctf_data_protect(base, size + hdrsz);

} else {
base = (void *)ctfsect->cts_data;
buf = (uchar_t *)base + hdrsz;
}





if ((fp = ctf_alloc(sizeof (ctf_file_t))) == NULL)
return (ctf_set_open_errno(errp, EAGAIN));

bzero(fp, sizeof (ctf_file_t));
fp->ctf_version = hp.cth_version;
fp->ctf_fileops = &ctf_fileops[hp.cth_version];
bcopy(ctfsect, &fp->ctf_data, sizeof (ctf_sect_t));

if (symsect != NULL) {
bcopy(symsect, &fp->ctf_symtab, sizeof (ctf_sect_t));
bcopy(strsect, &fp->ctf_strtab, sizeof (ctf_sect_t));
}

if (fp->ctf_data.cts_name != NULL)
fp->ctf_data.cts_name = ctf_strdup(fp->ctf_data.cts_name);
if (fp->ctf_symtab.cts_name != NULL)
fp->ctf_symtab.cts_name = ctf_strdup(fp->ctf_symtab.cts_name);
if (fp->ctf_strtab.cts_name != NULL)
fp->ctf_strtab.cts_name = ctf_strdup(fp->ctf_strtab.cts_name);

if (fp->ctf_data.cts_name == NULL)
fp->ctf_data.cts_name = _CTF_NULLSTR;
if (fp->ctf_symtab.cts_name == NULL)
fp->ctf_symtab.cts_name = _CTF_NULLSTR;
if (fp->ctf_strtab.cts_name == NULL)
fp->ctf_strtab.cts_name = _CTF_NULLSTR;

fp->ctf_str[CTF_STRTAB_0].cts_strs = (const char *)buf + hp.cth_stroff;
fp->ctf_str[CTF_STRTAB_0].cts_len = hp.cth_strlen;

if (strsect != NULL) {
fp->ctf_str[CTF_STRTAB_1].cts_strs = strsect->cts_data;
fp->ctf_str[CTF_STRTAB_1].cts_len = strsect->cts_size;
}

fp->ctf_base = base;
fp->ctf_buf = buf;
fp->ctf_size = size + hdrsz;





if (hp.cth_parlabel != 0)
fp->ctf_parlabel = ctf_strptr(fp, hp.cth_parlabel);
if (hp.cth_parname != 0)
fp->ctf_parname = ctf_strptr(fp, hp.cth_parname);

ctf_dprintf("ctf_bufopen: parent name %s (label %s)\n",
fp->ctf_parname ? fp->ctf_parname : "<NULL>",
fp->ctf_parlabel ? fp->ctf_parlabel : "<NULL>");





if (symsect != NULL) {
fp->ctf_nsyms = symsect->cts_size / symsect->cts_entsize;
fp->ctf_sxlate = ctf_alloc(fp->ctf_nsyms * sizeof (uint_t));

if (fp->ctf_sxlate == NULL) {
(void) ctf_set_open_errno(errp, EAGAIN);
goto bad;
}

if ((err = init_symtab(fp, &hp, symsect, strsect)) != 0) {
(void) ctf_set_open_errno(errp, err);
goto bad;
}
}

if ((err = init_types(fp, &hp)) != 0) {
(void) ctf_set_open_errno(errp, err);
goto bad;
}






fp->ctf_lookups[0].ctl_prefix = "struct";
fp->ctf_lookups[0].ctl_len = strlen(fp->ctf_lookups[0].ctl_prefix);
fp->ctf_lookups[0].ctl_hash = &fp->ctf_structs;
fp->ctf_lookups[1].ctl_prefix = "union";
fp->ctf_lookups[1].ctl_len = strlen(fp->ctf_lookups[1].ctl_prefix);
fp->ctf_lookups[1].ctl_hash = &fp->ctf_unions;
fp->ctf_lookups[2].ctl_prefix = "enum";
fp->ctf_lookups[2].ctl_len = strlen(fp->ctf_lookups[2].ctl_prefix);
fp->ctf_lookups[2].ctl_hash = &fp->ctf_enums;
fp->ctf_lookups[3].ctl_prefix = _CTF_NULLSTR;
fp->ctf_lookups[3].ctl_len = strlen(fp->ctf_lookups[3].ctl_prefix);
fp->ctf_lookups[3].ctl_hash = &fp->ctf_names;
fp->ctf_lookups[4].ctl_prefix = NULL;
fp->ctf_lookups[4].ctl_len = 0;
fp->ctf_lookups[4].ctl_hash = NULL;

if (symsect != NULL) {
if (symsect->cts_entsize == sizeof (Elf64_Sym))
(void) ctf_setmodel(fp, CTF_MODEL_LP64);
else
(void) ctf_setmodel(fp, CTF_MODEL_ILP32);
} else
(void) ctf_setmodel(fp, CTF_MODEL_NATIVE);

fp->ctf_refcnt = 1;
return (fp);

bad:
ctf_close(fp);
return (NULL);
}








ctf_file_t *
ctf_dup(ctf_file_t *ofp)
{
ctf_file_t *fp;
ctf_sect_t ctfsect, symsect, strsect;
ctf_sect_t *ctp, *symp, *strp;
void *cbuf, *symbuf, *strbuf;
int err;

cbuf = symbuf = strbuf = NULL;





bcopy(&ofp->ctf_data, &ctfsect, sizeof (ctf_sect_t));
cbuf = ctf_data_alloc(ctfsect.cts_size);
if (cbuf == NULL) {
(void) ctf_set_errno(ofp, ECTF_MMAP);
return (NULL);
}

bcopy(ctfsect.cts_data, cbuf, ctfsect.cts_size);
ctf_data_protect(cbuf, ctfsect.cts_size);
ctfsect.cts_data = cbuf;
ctfsect.cts_offset = 0;
ctp = &ctfsect;

if (ofp->ctf_symtab.cts_data != NULL) {
bcopy(&ofp->ctf_symtab, &symsect, sizeof (ctf_sect_t));
symbuf = ctf_data_alloc(symsect.cts_size);
if (symbuf == NULL) {
(void) ctf_set_errno(ofp, ECTF_MMAP);
goto err;
}
bcopy(symsect.cts_data, symbuf, symsect.cts_size);
ctf_data_protect(symbuf, symsect.cts_size);
symsect.cts_data = symbuf;
symsect.cts_offset = 0;
symp = &symsect;
} else {
symp = NULL;
}

if (ofp->ctf_strtab.cts_data != NULL) {
bcopy(&ofp->ctf_strtab, &strsect, sizeof (ctf_sect_t));
strbuf = ctf_data_alloc(strsect.cts_size);
if (strbuf == NULL) {
(void) ctf_set_errno(ofp, ECTF_MMAP);
goto err;
}
bcopy(strsect.cts_data, strbuf, strsect.cts_size);
ctf_data_protect(strbuf, strsect.cts_size);
strsect.cts_data = strbuf;
strsect.cts_offset = 0;
strp = &strsect;
} else {
strp = NULL;
}

fp = ctf_bufopen(ctp, symp, strp, &err);
if (fp == NULL) {
(void) ctf_set_errno(ofp, err);
goto err;
}

fp->ctf_flags |= LCTF_MMAP;

return (fp);

err:
ctf_data_free(cbuf, ctfsect.cts_size);
if (symbuf != NULL)
ctf_data_free(symbuf, symsect.cts_size);
if (strbuf != NULL)
ctf_data_free(strbuf, strsect.cts_size);
return (NULL);
}







void
ctf_close(ctf_file_t *fp)
{
ctf_dtdef_t *dtd, *ntd;

if (fp == NULL)
return;

ctf_dprintf("ctf_close(%p) refcnt=%u\n", (void *)fp, fp->ctf_refcnt);

if (fp->ctf_refcnt > 1) {
fp->ctf_refcnt--;
return;
}

if (fp->ctf_parent != NULL)
ctf_close(fp->ctf_parent);





for (dtd = ctf_list_prev(&fp->ctf_dtdefs); dtd != NULL; dtd = ntd) {
ntd = ctf_list_prev(dtd);
ctf_dtd_delete(fp, dtd);
}

ctf_free(fp->ctf_dthash, fp->ctf_dthashlen * sizeof (ctf_dtdef_t *));

if (fp->ctf_flags & LCTF_MMAP) {
if (fp->ctf_data.cts_data != NULL)
ctf_sect_munmap(&fp->ctf_data);
if (fp->ctf_symtab.cts_data != NULL)
ctf_sect_munmap(&fp->ctf_symtab);
if (fp->ctf_strtab.cts_data != NULL)
ctf_sect_munmap(&fp->ctf_strtab);
}

if (fp->ctf_data.cts_name != _CTF_NULLSTR &&
fp->ctf_data.cts_name != NULL) {
ctf_free((char *)fp->ctf_data.cts_name,
strlen(fp->ctf_data.cts_name) + 1);
}

if (fp->ctf_symtab.cts_name != _CTF_NULLSTR &&
fp->ctf_symtab.cts_name != NULL) {
ctf_free((char *)fp->ctf_symtab.cts_name,
strlen(fp->ctf_symtab.cts_name) + 1);
}

if (fp->ctf_strtab.cts_name != _CTF_NULLSTR &&
fp->ctf_strtab.cts_name != NULL) {
ctf_free((char *)fp->ctf_strtab.cts_name,
strlen(fp->ctf_strtab.cts_name) + 1);
}

if (fp->ctf_base != fp->ctf_data.cts_data && fp->ctf_base != NULL)
ctf_data_free((void *)fp->ctf_base, fp->ctf_size);

if (fp->ctf_sxlate != NULL)
ctf_free(fp->ctf_sxlate, sizeof (uint_t) * fp->ctf_nsyms);

if (fp->ctf_txlate != NULL) {
ctf_free(fp->ctf_txlate,
sizeof (uint_t) * (fp->ctf_typemax + 1));
}

if (fp->ctf_ptrtab != NULL) {
ctf_free(fp->ctf_ptrtab,
sizeof (ushort_t) * (fp->ctf_typemax + 1));
}

ctf_hash_destroy(&fp->ctf_structs);
ctf_hash_destroy(&fp->ctf_unions);
ctf_hash_destroy(&fp->ctf_enums);
ctf_hash_destroy(&fp->ctf_names);

ctf_free(fp, sizeof (ctf_file_t));
}





ctf_file_t *
ctf_parent_file(ctf_file_t *fp)
{
return (fp->ctf_parent);
}





const char *
ctf_parent_name(ctf_file_t *fp)
{
return (fp->ctf_parname);
}






int
ctf_import(ctf_file_t *fp, ctf_file_t *pfp)
{
if (fp == NULL || fp == pfp || (pfp != NULL && pfp->ctf_refcnt == 0))
return (ctf_set_errno(fp, EINVAL));

if (pfp != NULL && pfp->ctf_dmodel != fp->ctf_dmodel)
return (ctf_set_errno(fp, ECTF_DMODEL));

if (fp->ctf_parent != NULL)
ctf_close(fp->ctf_parent);

if (pfp != NULL) {
fp->ctf_flags |= LCTF_CHILD;
pfp->ctf_refcnt++;
}

fp->ctf_parent = pfp;
return (0);
}




int
ctf_setmodel(ctf_file_t *fp, int model)
{
const ctf_dmodel_t *dp;

for (dp = _libctf_models; dp->ctd_name != NULL; dp++) {
if (dp->ctd_code == model) {
fp->ctf_dmodel = dp;
return (0);
}
}

return (ctf_set_errno(fp, EINVAL));
}




int
ctf_getmodel(ctf_file_t *fp)
{
return (fp->ctf_dmodel->ctd_code);
}

void
ctf_setspecific(ctf_file_t *fp, void *data)
{
fp->ctf_specific = data;
}

void *
ctf_getspecific(ctf_file_t *fp)
{
return (fp->ctf_specific);
}
