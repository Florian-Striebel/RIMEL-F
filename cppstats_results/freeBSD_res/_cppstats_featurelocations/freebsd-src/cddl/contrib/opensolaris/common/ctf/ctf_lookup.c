


























#pragma ident "%Z%%M% %I% %E% SMI"

#include <sys/sysmacros.h>
#include <ctf_impl.h>















static int
isqualifier(const char *s, size_t len)
{
static const struct qual {
const char *q_name;
size_t q_len;
} qhash[] = {
{ "static", 6 }, { "", 0 }, { "", 0 }, { "", 0 },
{ "volatile", 8 }, { "", 0 }, { "", 0 }, { "", 0 }, { "", 0 },
{ "", 0 }, { "auto", 4 }, { "extern", 6 }, { "", 0 }, { "", 0 },
{ "", 0 }, { "", 0 }, { "const", 5 }, { "register", 8 },
{ "", 0 }, { "restrict", 8 }, { "_Restrict", 9 }
};

int h = s[len - 1] + (int)len - 105;
const struct qual *qp;

if (h < 0 || h >= sizeof (qhash) / sizeof (qhash[0]))
return (0);
qp = &qhash[h];
return (len == qp->q_len && strncmp(qp->q_name, s, qp->q_len) == 0);
}











ctf_id_t
ctf_lookup_by_name(ctf_file_t *fp, const char *name)
{
static const char delimiters[] = " \t\n\r\v\f*";

const ctf_lookup_t *lp;
const ctf_helem_t *hp;
const char *p, *q, *end;
ctf_id_t type = 0;
ctf_id_t ntype, ptype;

if (name == NULL)
return (ctf_set_errno(fp, EINVAL));

for (p = name, end = name + strlen(name); *p != '\0'; p = q) {
while (isspace(*p))
p++;

if (p == end)
break;

if ((q = strpbrk(p + 1, delimiters)) == NULL)
q = end;

if (*p == '*') {









ntype = fp->ctf_ptrtab[CTF_TYPE_TO_INDEX(type)];
if (ntype == 0) {
ntype = ctf_type_resolve(fp, type);
if (ntype == CTF_ERR || (ntype = fp->ctf_ptrtab[
CTF_TYPE_TO_INDEX(ntype)]) == 0) {
(void) ctf_set_errno(fp, ECTF_NOTYPE);
goto err;
}
}

type = CTF_INDEX_TO_TYPE(ntype,
(fp->ctf_flags & LCTF_CHILD));

q = p + 1;
continue;
}

if (isqualifier(p, (size_t)(q - p)))
continue;

for (lp = fp->ctf_lookups; lp->ctl_prefix != NULL; lp++) {
if (lp->ctl_prefix[0] == '\0' ||
((size_t)(q - p) >= lp->ctl_len && strncmp(p,
lp->ctl_prefix, (size_t)(q - p)) == 0)) {
for (p += lp->ctl_len; isspace(*p); p++)
continue;

if ((q = strchr(p, '*')) == NULL)
q = end;

while (isspace(q[-1]))
q--;

if ((hp = ctf_hash_lookup(lp->ctl_hash, fp, p,
(size_t)(q - p))) == NULL) {
(void) ctf_set_errno(fp, ECTF_NOTYPE);
goto err;
}

type = hp->h_type;
break;
}
}

if (lp->ctl_prefix == NULL) {
(void) ctf_set_errno(fp, ECTF_NOTYPE);
goto err;
}
}

if (*p != '\0' || type == 0)
return (ctf_set_errno(fp, ECTF_SYNTAX));

return (type);

err:
if (fp->ctf_parent != NULL &&
(ptype = ctf_lookup_by_name(fp->ctf_parent, name)) != CTF_ERR)
return (ptype);

return (CTF_ERR);
}





ctf_id_t
ctf_lookup_by_symbol(ctf_file_t *fp, ulong_t symidx)
{
const ctf_sect_t *sp = &fp->ctf_symtab;
ctf_id_t type;

if (sp->cts_data == NULL)
return (ctf_set_errno(fp, ECTF_NOSYMTAB));

if (symidx >= fp->ctf_nsyms)
return (ctf_set_errno(fp, EINVAL));

if (sp->cts_entsize == sizeof (Elf32_Sym)) {
const Elf32_Sym *symp = (Elf32_Sym *)sp->cts_data + symidx;
if (ELF32_ST_TYPE(symp->st_info) != STT_OBJECT)
return (ctf_set_errno(fp, ECTF_NOTDATA));
} else {
const Elf64_Sym *symp = (Elf64_Sym *)sp->cts_data + symidx;
if (ELF64_ST_TYPE(symp->st_info) != STT_OBJECT)
return (ctf_set_errno(fp, ECTF_NOTDATA));
}

if (fp->ctf_sxlate[symidx] == -1u)
return (ctf_set_errno(fp, ECTF_NOTYPEDAT));

type = *(ushort_t *)((uintptr_t)fp->ctf_buf + fp->ctf_sxlate[symidx]);
if (type == 0)
return (ctf_set_errno(fp, ECTF_NOTYPEDAT));

return (type);
}






const ctf_type_t *
ctf_lookup_by_id(ctf_file_t **fpp, ctf_id_t type)
{
ctf_file_t *fp = *fpp;

if ((fp->ctf_flags & LCTF_CHILD) && CTF_TYPE_ISPARENT(type) &&
(fp = fp->ctf_parent) == NULL) {
(void) ctf_set_errno(*fpp, ECTF_NOPARENT);
return (NULL);
}

type = CTF_TYPE_TO_INDEX(type);
if (type > 0 && type <= fp->ctf_typemax) {
*fpp = fp;
return (LCTF_INDEX_TO_TYPEPTR(fp, type));
}

(void) ctf_set_errno(fp, ECTF_BADID);
return (NULL);
}





int
ctf_func_info(ctf_file_t *fp, ulong_t symidx, ctf_funcinfo_t *fip)
{
const ctf_sect_t *sp = &fp->ctf_symtab;
const ushort_t *dp;
ushort_t info, kind, n;

if (sp->cts_data == NULL)
return (ctf_set_errno(fp, ECTF_NOSYMTAB));

if (symidx >= fp->ctf_nsyms)
return (ctf_set_errno(fp, EINVAL));

if (sp->cts_entsize == sizeof (Elf32_Sym)) {
const Elf32_Sym *symp = (Elf32_Sym *)sp->cts_data + symidx;
if (ELF32_ST_TYPE(symp->st_info) != STT_FUNC)
return (ctf_set_errno(fp, ECTF_NOTFUNC));
} else {
const Elf64_Sym *symp = (Elf64_Sym *)sp->cts_data + symidx;
if (ELF64_ST_TYPE(symp->st_info) != STT_FUNC)
return (ctf_set_errno(fp, ECTF_NOTFUNC));
}

if (fp->ctf_sxlate[symidx] == -1u)
return (ctf_set_errno(fp, ECTF_NOFUNCDAT));

dp = (ushort_t *)((uintptr_t)fp->ctf_buf + fp->ctf_sxlate[symidx]);

info = *dp++;
kind = LCTF_INFO_KIND(fp, info);
n = LCTF_INFO_VLEN(fp, info);

if (kind == CTF_K_UNKNOWN && n == 0)
return (ctf_set_errno(fp, ECTF_NOFUNCDAT));

if (kind != CTF_K_FUNCTION)
return (ctf_set_errno(fp, ECTF_CORRUPT));

fip->ctc_return = *dp++;
fip->ctc_argc = n;
fip->ctc_flags = 0;

if (n != 0 && dp[n - 1] == 0) {
fip->ctc_flags |= CTF_FUNC_VARARG;
fip->ctc_argc--;
}

return (0);
}





int
ctf_func_args(ctf_file_t *fp, ulong_t symidx, uint_t argc, ctf_id_t *argv)
{
const ushort_t *dp;
ctf_funcinfo_t f;

if (ctf_func_info(fp, symidx, &f) == CTF_ERR)
return (CTF_ERR);





dp = (ushort_t *)((uintptr_t)fp->ctf_buf + fp->ctf_sxlate[symidx]) + 2;

for (argc = MIN(argc, f.ctc_argc); argc != 0; argc--)
*argv++ = *dp++;

return (0);
}
