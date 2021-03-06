

























#pragma ident "%Z%%M% %I% %E% SMI"























#include <ctf_impl.h>

void
ctf_decl_init(ctf_decl_t *cd, char *buf, size_t len)
{
int i;

bzero(cd, sizeof (ctf_decl_t));

for (i = CTF_PREC_BASE; i < CTF_PREC_MAX; i++)
cd->cd_order[i] = CTF_PREC_BASE - 1;

cd->cd_qualp = CTF_PREC_BASE;
cd->cd_ordp = CTF_PREC_BASE;

cd->cd_buf = buf;
cd->cd_ptr = buf;
cd->cd_end = buf + len;
}

void
ctf_decl_fini(ctf_decl_t *cd)
{
ctf_decl_node_t *cdp, *ndp;
int i;

for (i = CTF_PREC_BASE; i < CTF_PREC_MAX; i++) {
for (cdp = ctf_list_next(&cd->cd_nodes[i]);
cdp != NULL; cdp = ndp) {
ndp = ctf_list_next(cdp);
ctf_free(cdp, sizeof (ctf_decl_node_t));
}
}
}

void
ctf_decl_push(ctf_decl_t *cd, ctf_file_t *fp, ctf_id_t type)
{
ctf_decl_node_t *cdp;
ctf_decl_prec_t prec;
uint_t kind, n = 1;
int is_qual = 0;

const ctf_type_t *tp;
ctf_arinfo_t ar;

if ((tp = ctf_lookup_by_id(&fp, type)) == NULL) {
cd->cd_err = fp->ctf_errno;
return;
}

switch (kind = LCTF_INFO_KIND(fp, tp->ctt_info)) {
case CTF_K_ARRAY:
(void) ctf_array_info(fp, type, &ar);
ctf_decl_push(cd, fp, ar.ctr_contents);
n = ar.ctr_nelems;
prec = CTF_PREC_ARRAY;
break;

case CTF_K_TYPEDEF:
if (ctf_strptr(fp, tp->ctt_name)[0] == '\0') {
ctf_decl_push(cd, fp, tp->ctt_type);
return;
}
prec = CTF_PREC_BASE;
break;

case CTF_K_FUNCTION:
ctf_decl_push(cd, fp, tp->ctt_type);
prec = CTF_PREC_FUNCTION;
break;

case CTF_K_POINTER:
ctf_decl_push(cd, fp, tp->ctt_type);
prec = CTF_PREC_POINTER;
break;

case CTF_K_VOLATILE:
case CTF_K_CONST:
case CTF_K_RESTRICT:
ctf_decl_push(cd, fp, tp->ctt_type);
prec = cd->cd_qualp;
is_qual++;
break;

default:
prec = CTF_PREC_BASE;
}

if ((cdp = ctf_alloc(sizeof (ctf_decl_node_t))) == NULL) {
cd->cd_err = EAGAIN;
return;
}

cdp->cd_type = type;
cdp->cd_kind = kind;
cdp->cd_n = n;

if (ctf_list_next(&cd->cd_nodes[prec]) == NULL)
cd->cd_order[prec] = cd->cd_ordp++;





if (prec > cd->cd_qualp && prec < CTF_PREC_ARRAY)
cd->cd_qualp = prec;






if (kind == CTF_K_ARRAY || (is_qual && prec == CTF_PREC_BASE))
ctf_list_prepend(&cd->cd_nodes[prec], cdp);
else
ctf_list_append(&cd->cd_nodes[prec], cdp);
}


void
ctf_decl_sprintf(ctf_decl_t *cd, const char *format, ...)
{
size_t len = (size_t)(cd->cd_end - cd->cd_ptr);
va_list ap;
size_t n;

va_start(ap, format);
n = vsnprintf(cd->cd_ptr, len, format, ap);
va_end(ap);

cd->cd_ptr += MIN(n, len);
cd->cd_len += n;
}
