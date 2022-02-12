


























#if defined(illumos)
#include <sys/sysmacros.h>
#else
#define ABS(a) ((a) < 0 ? -(a) : (a))
#endif
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#if defined(illumos)
#include <alloca.h>
#endif
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/byteorder.h>
#include <dt_printf.h>
#include <dt_string.h>
#include <dt_impl.h>

#if !defined(NS_IN6ADDRSZ)
#define NS_IN6ADDRSZ 16
#endif

#if !defined(NS_INADDRSZ)
#define NS_INADDRSZ 4
#endif


static int
pfcheck_addr(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
return (dt_node_is_pointer(dnp) || dt_node_is_integer(dnp));
}


static int
pfcheck_kaddr(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
return (dt_node_is_pointer(dnp) || dt_node_is_integer(dnp) ||
dt_node_is_symaddr(dnp));
}


static int
pfcheck_uaddr(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
dtrace_hdl_t *dtp = pfv->pfv_dtp;
dt_ident_t *idp = dt_idhash_lookup(dtp->dt_macros, "target");

if (dt_node_is_usymaddr(dnp))
return (1);

if (idp == NULL || idp->di_id == 0)
return (0);

return (dt_node_is_pointer(dnp) || dt_node_is_integer(dnp));
}


static int
pfcheck_stack(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
return (dt_node_is_stack(dnp));
}


static int
pfcheck_time(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
return (dt_node_is_integer(dnp) &&
dt_node_type_size(dnp) == sizeof (uint64_t));
}


static int
pfcheck_str(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
ctf_file_t *ctfp;
ctf_encoding_t e;
ctf_arinfo_t r;
ctf_id_t base;
uint_t kind;

if (dt_node_is_string(dnp))
return (1);

ctfp = dnp->dn_ctfp;
base = ctf_type_resolve(ctfp, dnp->dn_type);
kind = ctf_type_kind(ctfp, base);

return (kind == CTF_K_ARRAY && ctf_array_info(ctfp, base, &r) == 0 &&
(base = ctf_type_resolve(ctfp, r.ctr_contents)) != CTF_ERR &&
ctf_type_encoding(ctfp, base, &e) == 0 && IS_CHAR(e));
}


static int
pfcheck_wstr(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
ctf_file_t *ctfp = dnp->dn_ctfp;
ctf_id_t base = ctf_type_resolve(ctfp, dnp->dn_type);
uint_t kind = ctf_type_kind(ctfp, base);

ctf_encoding_t e;
ctf_arinfo_t r;

return (kind == CTF_K_ARRAY && ctf_array_info(ctfp, base, &r) == 0 &&
(base = ctf_type_resolve(ctfp, r.ctr_contents)) != CTF_ERR &&
ctf_type_kind(ctfp, base) == CTF_K_INTEGER &&
ctf_type_encoding(ctfp, base, &e) == 0 && e.cte_bits == 32);
}


static int
pfcheck_csi(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
return (dt_node_is_integer(dnp) &&
dt_node_type_size(dnp) <= sizeof (int));
}


static int
pfcheck_fp(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
return (dt_node_is_float(dnp));
}


static int
pfcheck_xint(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
return (dt_node_is_integer(dnp));
}


static int
pfcheck_dint(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
if (dnp->dn_flags & DT_NF_SIGNED)
pfd->pfd_fmt[strlen(pfd->pfd_fmt) - 1] = 'i';
else
pfd->pfd_fmt[strlen(pfd->pfd_fmt) - 1] = 'u';

return (dt_node_is_integer(dnp));
}


static int
pfcheck_xshort(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
ctf_file_t *ctfp = dnp->dn_ctfp;
ctf_id_t type = ctf_type_resolve(ctfp, dnp->dn_type);
char n[DT_TYPE_NAMELEN];

return (ctf_type_name(ctfp, type, n, sizeof (n)) != NULL && (
strcmp(n, "short") == 0 || strcmp(n, "signed short") == 0 ||
strcmp(n, "unsigned short") == 0));
}


static int
pfcheck_xlong(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
ctf_file_t *ctfp = dnp->dn_ctfp;
ctf_id_t type = ctf_type_resolve(ctfp, dnp->dn_type);
char n[DT_TYPE_NAMELEN];

return (ctf_type_name(ctfp, type, n, sizeof (n)) != NULL && (
strcmp(n, "long") == 0 || strcmp(n, "signed long") == 0 ||
strcmp(n, "unsigned long") == 0));
}


static int
pfcheck_xlonglong(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
ctf_file_t *ctfp = dnp->dn_ctfp;
ctf_id_t type = dnp->dn_type;
char n[DT_TYPE_NAMELEN];

if (ctf_type_name(ctfp, ctf_type_resolve(ctfp, type), n,
sizeof (n)) != NULL && (strcmp(n, "long long") == 0 ||
strcmp(n, "signed long long") == 0 ||
strcmp(n, "unsigned long long") == 0))
return (1);







while (ctf_type_kind(ctfp, type) == CTF_K_TYPEDEF) {
if (ctf_type_name(ctfp, type, n, sizeof (n)) != NULL &&
(strcmp(n, "int64_t") == 0 || strcmp(n, "uint64_t") == 0))
return (1);

type = ctf_type_reference(ctfp, type);
}

return (0);
}


static int
pfcheck_type(dt_pfargv_t *pfv, dt_pfargd_t *pfd, dt_node_t *dnp)
{
return (ctf_type_compat(dnp->dn_ctfp, ctf_type_resolve(dnp->dn_ctfp,
dnp->dn_type), pfd->pfd_conv->pfc_dctfp, pfd->pfd_conv->pfc_dtype));
}


static int
pfprint_sint(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t unormal)
{
int64_t normal = (int64_t)unormal;
int32_t n = (int32_t)normal;

switch (size) {
case sizeof (int8_t):
return (dt_printf(dtp, fp, format,
(int32_t)*((int8_t *)addr) / n));
case sizeof (int16_t):
return (dt_printf(dtp, fp, format,
(int32_t)*((int16_t *)addr) / n));
case sizeof (int32_t):
return (dt_printf(dtp, fp, format,
*((int32_t *)addr) / n));
case sizeof (int64_t):
return (dt_printf(dtp, fp, format,
*((int64_t *)addr) / normal));
default:
return (dt_set_errno(dtp, EDT_DMISMATCH));
}
}


static int
pfprint_uint(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
uint32_t n = (uint32_t)normal;

switch (size) {
case sizeof (uint8_t):
return (dt_printf(dtp, fp, format,
(uint32_t)*((uint8_t *)addr) / n));
case sizeof (uint16_t):
return (dt_printf(dtp, fp, format,
(uint32_t)*((uint16_t *)addr) / n));
case sizeof (uint32_t):
return (dt_printf(dtp, fp, format,
*((uint32_t *)addr) / n));
case sizeof (uint64_t):
return (dt_printf(dtp, fp, format,
*((uint64_t *)addr) / normal));
default:
return (dt_set_errno(dtp, EDT_DMISMATCH));
}
}

static int
pfprint_dint(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
if (pfd->pfd_flags & DT_PFCONV_SIGNED)
return (pfprint_sint(dtp, fp, format, pfd, addr, size, normal));
else
return (pfprint_uint(dtp, fp, format, pfd, addr, size, normal));
}


static int
pfprint_fp(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
double n = (double)normal;
long double ldn = (long double)normal;

switch (size) {
case sizeof (float):
return (dt_printf(dtp, fp, format,
(double)*((float *)addr) / n));
case sizeof (double):
return (dt_printf(dtp, fp, format,
*((double *)addr) / n));
#if !defined(__arm__) && !defined(__powerpc__) && !defined(__mips__) && !defined(__riscv)

case sizeof (long double):
return (dt_printf(dtp, fp, format,
*((long double *)addr) / ldn));
#endif
default:
return (dt_set_errno(dtp, EDT_DMISMATCH));
}
}


static int
pfprint_addr(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
char *s;
int n, len = 256;
uint64_t val;

switch (size) {
case sizeof (uint32_t):
val = *((uint32_t *)addr);
break;
case sizeof (uint64_t):
val = *((uint64_t *)addr);
break;
default:
return (dt_set_errno(dtp, EDT_DMISMATCH));
}

do {
n = len;
s = alloca(n);
} while ((len = dtrace_addr2str(dtp, val, s, n)) > n);

return (dt_printf(dtp, fp, format, s));
}


static int
pfprint_mod(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
return (dt_print_mod(dtp, fp, format, (caddr_t)addr));
}


static int
pfprint_umod(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
return (dt_print_umod(dtp, fp, format, (caddr_t)addr));
}


static int
pfprint_uaddr(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
char *s;
int n, len = 256;
uint64_t val, pid = 0;

dt_ident_t *idp = dt_idhash_lookup(dtp->dt_macros, "target");

switch (size) {
case sizeof (uint32_t):
val = (u_longlong_t)*((uint32_t *)addr);
break;
case sizeof (uint64_t):
val = (u_longlong_t)*((uint64_t *)addr);
break;
case sizeof (uint64_t) * 2:
pid = ((uint64_t *)(uintptr_t)addr)[0];
val = ((uint64_t *)(uintptr_t)addr)[1];
break;
default:
return (dt_set_errno(dtp, EDT_DMISMATCH));
}

if (pid == 0 && dtp->dt_vector == NULL && idp != NULL)
pid = idp->di_id;

do {
n = len;
s = alloca(n);
} while ((len = dtrace_uaddr2str(dtp, pid, val, s, n)) > n);

return (dt_printf(dtp, fp, format, s));
}


static int
pfprint_stack(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *vaddr, size_t size, uint64_t normal)
{
int width;
dtrace_optval_t saved = dtp->dt_options[DTRACEOPT_STACKINDENT];
const dtrace_recdesc_t *rec = pfd->pfd_rec;
caddr_t addr = (caddr_t)vaddr;
int err = 0;










if (pfd->pfd_dynwidth < 0) {
assert(pfd->pfd_flags & DT_PFCONV_DYNWIDTH);
width = -pfd->pfd_dynwidth;
} else if (pfd->pfd_flags & DT_PFCONV_LEFT) {
width = pfd->pfd_dynwidth ? pfd->pfd_dynwidth : pfd->pfd_width;
} else {
width = 0;
}

dtp->dt_options[DTRACEOPT_STACKINDENT] = width;

switch (rec->dtrd_action) {
case DTRACEACT_USTACK:
case DTRACEACT_JSTACK:
err = dt_print_ustack(dtp, fp, format, addr, rec->dtrd_arg);
break;

case DTRACEACT_STACK:
err = dt_print_stack(dtp, fp, format, addr, rec->dtrd_arg,
rec->dtrd_size / rec->dtrd_arg);
break;

default:
assert(0);
}

dtp->dt_options[DTRACEOPT_STACKINDENT] = saved;

return (err);
}


static int
pfprint_time(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
char src[32], buf[32], *dst = buf;
hrtime_t time = *((uint64_t *)addr);
time_t sec = (time_t)(time / NANOSEC);
int i;






#if defined(illumos)
(void) ctime_r(&sec, src, sizeof (src));
#else
(void) ctime_r(&sec, src);
#endif




for (i = 20; i < 24; i++)
*dst++ = src[i];




for (i = 3; i < 19; i++)
*dst++ = src[i];

*dst = '\0';
return (dt_printf(dtp, fp, format, buf));
}







static int
pfprint_time822(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
hrtime_t time = *((uint64_t *)addr);
time_t sec = (time_t)(time / NANOSEC);
struct tm tm;
char buf[64];

(void) localtime_r(&sec, &tm);
(void) strftime(buf, sizeof (buf), "%a, %d %b %G %T %Z", &tm);
return (dt_printf(dtp, fp, format, buf));
}


static int
pfprint_port(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
uint16_t port = htons(*((uint16_t *)addr));
char buf[256];
struct servent *sv, res;

#if defined(illumos)
if ((sv = getservbyport_r(port, NULL, &res, buf, sizeof (buf))) != NULL)
#else
if (getservbyport_r(port, NULL, &res, buf, sizeof (buf), &sv) > 0)
#endif
return (dt_printf(dtp, fp, format, sv->s_name));

(void) snprintf(buf, sizeof (buf), "%d", *((uint16_t *)addr));
return (dt_printf(dtp, fp, format, buf));
}


static int
pfprint_inetaddr(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
char *s = alloca(size + 1);
struct hostent *host, res;
char inetaddr[NS_IN6ADDRSZ];
char buf[1024];
int e;

bcopy(addr, s, size);
s[size] = '\0';

if (strchr(s, ':') == NULL && inet_pton(AF_INET, s, inetaddr) != -1) {
#if defined(illumos)
if ((host = gethostbyaddr_r(inetaddr, NS_INADDRSZ,
AF_INET, &res, buf, sizeof (buf), &e)) != NULL)
#else
if (gethostbyaddr_r(inetaddr, NS_INADDRSZ,
AF_INET, &res, buf, sizeof (buf), &host, &e) > 0)
#endif
return (dt_printf(dtp, fp, format, host->h_name));
} else if (inet_pton(AF_INET6, s, inetaddr) != -1) {
if ((host = getipnodebyaddr(inetaddr, NS_IN6ADDRSZ,
AF_INET6, &e)) != NULL)
return (dt_printf(dtp, fp, format, host->h_name));
}

return (dt_printf(dtp, fp, format, s));
}


static int
pfprint_cstr(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
char *s = alloca(size + 1);

bcopy(addr, s, size);
s[size] = '\0';
return (dt_printf(dtp, fp, format, s));
}


static int
pfprint_wstr(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
wchar_t *ws = alloca(size + sizeof (wchar_t));

bcopy(addr, ws, size);
ws[size / sizeof (wchar_t)] = L'\0';
return (dt_printf(dtp, fp, format, ws));
}


static int
pfprint_estr(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
char *s;
int n;

if ((s = strchr2esc(addr, size)) == NULL)
return (dt_set_errno(dtp, EDT_NOMEM));

n = dt_printf(dtp, fp, format, s);
free(s);
return (n);
}

static int
pfprint_echr(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
char c;

switch (size) {
case sizeof (int8_t):
c = *(int8_t *)addr;
break;
case sizeof (int16_t):
c = *(int16_t *)addr;
break;
case sizeof (int32_t):
c = *(int32_t *)addr;
break;
default:
return (dt_set_errno(dtp, EDT_DMISMATCH));
}

return (pfprint_estr(dtp, fp, format, pfd, &c, 1, normal));
}


static int
pfprint_pct(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
return (dt_printf(dtp, fp, "%%"));
}

static const char pfproto_xint[] = "char, short, int, long, or long long";
static const char pfproto_csi[] = "char, short, or int";
static const char pfproto_fp[] = "float, double, or long double";
static const char pfproto_addr[] = "pointer or integer";
static const char pfproto_uaddr[] =
"pointer or integer (with -p/-c) or _usymaddr (without -p/-c)";
static const char pfproto_cstr[] = "char [] or string (or use stringof)";
static const char pfproto_wstr[] = "wchar_t []";








static const dt_pfconv_t _dtrace_conversions[] = {
{ "a", "s", pfproto_addr, pfcheck_kaddr, pfprint_addr },
{ "A", "s", pfproto_uaddr, pfcheck_uaddr, pfprint_uaddr },
{ "c", "c", pfproto_csi, pfcheck_csi, pfprint_sint },
{ "C", "s", pfproto_csi, pfcheck_csi, pfprint_echr },
{ "d", "d", pfproto_xint, pfcheck_dint, pfprint_dint },
{ "e", "e", pfproto_fp, pfcheck_fp, pfprint_fp },
{ "E", "E", pfproto_fp, pfcheck_fp, pfprint_fp },
{ "f", "f", pfproto_fp, pfcheck_fp, pfprint_fp },
{ "g", "g", pfproto_fp, pfcheck_fp, pfprint_fp },
{ "G", "G", pfproto_fp, pfcheck_fp, pfprint_fp },
{ "hd", "d", "short", pfcheck_type, pfprint_sint },
{ "hi", "i", "short", pfcheck_type, pfprint_sint },
{ "ho", "o", "unsigned short", pfcheck_type, pfprint_uint },
{ "hu", "u", "unsigned short", pfcheck_type, pfprint_uint },
{ "hx", "x", "short", pfcheck_xshort, pfprint_uint },
{ "hX", "X", "short", pfcheck_xshort, pfprint_uint },
{ "i", "i", pfproto_xint, pfcheck_xint, pfprint_sint },
{ "I", "s", pfproto_cstr, pfcheck_str, pfprint_inetaddr },
{ "k", "s", "stack", pfcheck_stack, pfprint_stack },
{ "lc", "lc", "int", pfcheck_type, pfprint_sint },
{ "ld", "d", "long", pfcheck_type, pfprint_sint },
{ "li", "i", "long", pfcheck_type, pfprint_sint },
{ "lo", "o", "unsigned long", pfcheck_type, pfprint_uint },
{ "lu", "u", "unsigned long", pfcheck_type, pfprint_uint },
{ "ls", "ls", pfproto_wstr, pfcheck_wstr, pfprint_wstr },
{ "lx", "x", "long", pfcheck_xlong, pfprint_uint },
{ "lX", "X", "long", pfcheck_xlong, pfprint_uint },
{ "lld", "d", "long long", pfcheck_type, pfprint_sint },
{ "lli", "i", "long long", pfcheck_type, pfprint_sint },
{ "llo", "o", "unsigned long long", pfcheck_type, pfprint_uint },
{ "llu", "u", "unsigned long long", pfcheck_type, pfprint_uint },
{ "llx", "x", "long long", pfcheck_xlonglong, pfprint_uint },
{ "llX", "X", "long long", pfcheck_xlonglong, pfprint_uint },
{ "Le", "e", "long double", pfcheck_type, pfprint_fp },
{ "LE", "E", "long double", pfcheck_type, pfprint_fp },
{ "Lf", "f", "long double", pfcheck_type, pfprint_fp },
{ "Lg", "g", "long double", pfcheck_type, pfprint_fp },
{ "LG", "G", "long double", pfcheck_type, pfprint_fp },
{ "o", "o", pfproto_xint, pfcheck_xint, pfprint_uint },
{ "p", "x", pfproto_addr, pfcheck_addr, pfprint_uint },
{ "P", "s", "uint16_t", pfcheck_type, pfprint_port },
{ "s", "s", "char [] or string (or use stringof)", pfcheck_str, pfprint_cstr },
{ "S", "s", pfproto_cstr, pfcheck_str, pfprint_estr },
{ "T", "s", "int64_t", pfcheck_time, pfprint_time822 },
{ "u", "u", pfproto_xint, pfcheck_xint, pfprint_uint },
#if defined(illumos)
{ "wc", "wc", "int", pfcheck_type, pfprint_sint },
{ "ws", "ws", pfproto_wstr, pfcheck_wstr, pfprint_wstr },
#else
{ "wc", "lc", "int", pfcheck_type, pfprint_sint },
{ "ws", "ls", pfproto_wstr, pfcheck_wstr, pfprint_wstr },
#endif
{ "x", "x", pfproto_xint, pfcheck_xint, pfprint_uint },
{ "X", "X", pfproto_xint, pfcheck_xint, pfprint_uint },
{ "Y", "s", "int64_t", pfcheck_time, pfprint_time },
{ "%", "%", "void", pfcheck_type, pfprint_pct },
{ NULL, NULL, NULL, NULL, NULL }
};

int
dt_pfdict_create(dtrace_hdl_t *dtp)
{
uint_t n = _dtrace_strbuckets;
const dt_pfconv_t *pfd;
dt_pfdict_t *pdi;

if ((pdi = malloc(sizeof (dt_pfdict_t))) == NULL ||
(pdi->pdi_buckets = malloc(sizeof (dt_pfconv_t *) * n)) == NULL) {
free(pdi);
return (dt_set_errno(dtp, EDT_NOMEM));
}

dtp->dt_pfdict = pdi;
bzero(pdi->pdi_buckets, sizeof (dt_pfconv_t *) * n);
pdi->pdi_nbuckets = n;

for (pfd = _dtrace_conversions; pfd->pfc_name != NULL; pfd++) {
dtrace_typeinfo_t dtt;
dt_pfconv_t *pfc;
uint_t h;

if ((pfc = malloc(sizeof (dt_pfconv_t))) == NULL) {
dt_pfdict_destroy(dtp);
return (dt_set_errno(dtp, EDT_NOMEM));
}

bcopy(pfd, pfc, sizeof (dt_pfconv_t));
h = dt_strtab_hash(pfc->pfc_name, NULL) % n;
pfc->pfc_next = pdi->pdi_buckets[h];
pdi->pdi_buckets[h] = pfc;

dtt.dtt_ctfp = NULL;
dtt.dtt_type = CTF_ERR;






if (pfc->pfc_check == &pfcheck_type && dtrace_lookup_by_type(
dtp, DTRACE_OBJ_DDEFS, pfc->pfc_tstr, &dtt) != 0) {
dt_pfdict_destroy(dtp);
return (dt_set_errno(dtp, EDT_NOCONV));
}

pfc->pfc_dctfp = dtt.dtt_ctfp;
pfc->pfc_dtype = dtt.dtt_type;






if (pfc->pfc_check == &pfcheck_type && dtrace_lookup_by_type(
dtp, DTRACE_OBJ_CDEFS, pfc->pfc_tstr, &dtt) == 0) {
pfc->pfc_cctfp = dtt.dtt_ctfp;
pfc->pfc_ctype = dtt.dtt_type;
} else {
pfc->pfc_cctfp = pfc->pfc_dctfp;
pfc->pfc_ctype = pfc->pfc_dtype;
}

if (pfc->pfc_check == NULL || pfc->pfc_print == NULL ||
pfc->pfc_ofmt == NULL || pfc->pfc_tstr == NULL) {
dt_pfdict_destroy(dtp);
return (dt_set_errno(dtp, EDT_BADCONV));
}

dt_dprintf("loaded printf conversion %%%s\n", pfc->pfc_name);
}

return (0);
}

void
dt_pfdict_destroy(dtrace_hdl_t *dtp)
{
dt_pfdict_t *pdi = dtp->dt_pfdict;
dt_pfconv_t *pfc, *nfc;
uint_t i;

if (pdi == NULL)
return;

for (i = 0; i < pdi->pdi_nbuckets; i++) {
for (pfc = pdi->pdi_buckets[i]; pfc != NULL; pfc = nfc) {
nfc = pfc->pfc_next;
free(pfc);
}
}

free(pdi->pdi_buckets);
free(pdi);
dtp->dt_pfdict = NULL;
}

static const dt_pfconv_t *
dt_pfdict_lookup(dtrace_hdl_t *dtp, const char *name)
{
dt_pfdict_t *pdi = dtp->dt_pfdict;
uint_t h = dt_strtab_hash(name, NULL) % pdi->pdi_nbuckets;
const dt_pfconv_t *pfc;

for (pfc = pdi->pdi_buckets[h]; pfc != NULL; pfc = pfc->pfc_next) {
if (strcmp(pfc->pfc_name, name) == 0)
break;
}

return (pfc);
}

static dt_pfargv_t *
dt_printf_error(dtrace_hdl_t *dtp, int err)
{
if (yypcb != NULL)
longjmp(yypcb->pcb_jmpbuf, err);

(void) dt_set_errno(dtp, err);
return (NULL);
}

dt_pfargv_t *
dt_printf_create(dtrace_hdl_t *dtp, const char *s)
{
dt_pfargd_t *pfd, *nfd = NULL;
dt_pfargv_t *pfv;
const char *p, *q;
char *format;

if ((pfv = malloc(sizeof (dt_pfargv_t))) == NULL ||
(format = strdup(s)) == NULL) {
free(pfv);
return (dt_printf_error(dtp, EDT_NOMEM));
}

pfv->pfv_format = format;
pfv->pfv_argv = NULL;
pfv->pfv_argc = 0;
pfv->pfv_flags = 0;
pfv->pfv_dtp = dtp;

for (q = format; (p = strchr(q, '%')) != NULL; q = *p ? p + 1 : p) {
uint_t namelen = 0;
int digits = 0;
int dot = 0;

char name[8];
char c;
int n;

if ((pfd = malloc(sizeof (dt_pfargd_t))) == NULL) {
dt_printf_destroy(pfv);
return (dt_printf_error(dtp, EDT_NOMEM));
}

if (pfv->pfv_argv != NULL)
nfd->pfd_next = pfd;
else
pfv->pfv_argv = pfd;

bzero(pfd, sizeof (dt_pfargd_t));
pfv->pfv_argc++;
nfd = pfd;

if (p > q) {
pfd->pfd_preflen = (size_t)(p - q);
pfd->pfd_prefix = q;
}

fmt_switch:
switch (c = *++p) {
case '0': case '1': case '2': case '3': case '4':
case '5': case '6': case '7': case '8': case '9':
if (dot == 0 && digits == 0 && c == '0') {
pfd->pfd_flags |= DT_PFCONV_ZPAD;
pfd->pfd_flags &= ~DT_PFCONV_LEFT;
goto fmt_switch;
}

for (n = 0; isdigit(c); c = *++p)
n = n * 10 + c - '0';

if (dot)
pfd->pfd_prec = n;
else
pfd->pfd_width = n;

p--;
digits++;
goto fmt_switch;

case '#':
pfd->pfd_flags |= DT_PFCONV_ALT;
goto fmt_switch;

case '*':
n = dot ? DT_PFCONV_DYNPREC : DT_PFCONV_DYNWIDTH;

if (pfd->pfd_flags & n) {
yywarn("format conversion #%u has more than "
"one '*' specified for the output %s\n",
pfv->pfv_argc, n ? "precision" : "width");

dt_printf_destroy(pfv);
return (dt_printf_error(dtp, EDT_COMPILER));
}

pfd->pfd_flags |= n;
goto fmt_switch;

case '+':
pfd->pfd_flags |= DT_PFCONV_SPOS;
goto fmt_switch;

case '-':
pfd->pfd_flags |= DT_PFCONV_LEFT;
pfd->pfd_flags &= ~DT_PFCONV_ZPAD;
goto fmt_switch;

case '.':
if (dot++ != 0) {
yywarn("format conversion #%u has more than "
"one '.' specified\n", pfv->pfv_argc);

dt_printf_destroy(pfv);
return (dt_printf_error(dtp, EDT_COMPILER));
}
digits = 0;
goto fmt_switch;

case '?':
if (dtp->dt_conf.dtc_ctfmodel == CTF_MODEL_LP64)
pfd->pfd_width = 16;
else
pfd->pfd_width = 8;
goto fmt_switch;

case '@':
pfd->pfd_flags |= DT_PFCONV_AGG;
goto fmt_switch;

case '\'':
pfd->pfd_flags |= DT_PFCONV_GROUP;
goto fmt_switch;

case ' ':
pfd->pfd_flags |= DT_PFCONV_SPACE;
goto fmt_switch;

case '$':
yywarn("format conversion #%u uses unsupported "
"positional format (%%n$)\n", pfv->pfv_argc);

dt_printf_destroy(pfv);
return (dt_printf_error(dtp, EDT_COMPILER));

case '%':
if (p[-1] == '%')
goto default_lbl;

yywarn("format conversion #%u cannot be combined "
"with other format flags: %%%%\n", pfv->pfv_argc);

dt_printf_destroy(pfv);
return (dt_printf_error(dtp, EDT_COMPILER));

case '\0':
yywarn("format conversion #%u name expected before "
"end of format string\n", pfv->pfv_argc);

dt_printf_destroy(pfv);
return (dt_printf_error(dtp, EDT_COMPILER));

case 'h':
case 'l':
case 'L':
case 'w':
if (namelen < sizeof (name) - 2)
name[namelen++] = c;
goto fmt_switch;

default_lbl:
default:
name[namelen++] = c;
name[namelen] = '\0';
}

pfd->pfd_conv = dt_pfdict_lookup(dtp, name);

if (pfd->pfd_conv == NULL) {
yywarn("format conversion #%u is undefined: %%%s\n",
pfv->pfv_argc, name);
dt_printf_destroy(pfv);
return (dt_printf_error(dtp, EDT_COMPILER));
}
}

if (*q != '\0' || *format == '\0') {
if ((pfd = malloc(sizeof (dt_pfargd_t))) == NULL) {
dt_printf_destroy(pfv);
return (dt_printf_error(dtp, EDT_NOMEM));
}

if (pfv->pfv_argv != NULL)
nfd->pfd_next = pfd;
else
pfv->pfv_argv = pfd;

bzero(pfd, sizeof (dt_pfargd_t));
pfv->pfv_argc++;

pfd->pfd_prefix = q;
pfd->pfd_preflen = strlen(q);
}

return (pfv);
}

void
dt_printf_destroy(dt_pfargv_t *pfv)
{
dt_pfargd_t *pfd, *nfd;

for (pfd = pfv->pfv_argv; pfd != NULL; pfd = nfd) {
nfd = pfd->pfd_next;
free(pfd);
}

free(pfv->pfv_format);
free(pfv);
}

void
dt_printf_validate(dt_pfargv_t *pfv, uint_t flags,
dt_ident_t *idp, int foff, dtrace_actkind_t kind, dt_node_t *dnp)
{
dt_pfargd_t *pfd = pfv->pfv_argv;
const char *func = idp->di_name;

char n[DT_TYPE_NAMELEN];
dtrace_typeinfo_t dtt;
const char *aggtype;
dt_node_t aggnode;
int i, j;

if (pfv->pfv_format[0] == '\0') {
xyerror(D_PRINTF_FMT_EMPTY,
"%s( ) format string is empty\n", func);
}

pfv->pfv_flags = flags;






if (kind != DTRACEAGG_COUNT)
aggtype = "int64_t";
else
aggtype = "uint64_t";

if (dt_type_lookup(aggtype, &dtt) != 0)
xyerror(D_TYPE_ERR, "failed to lookup agg type %s\n", aggtype);

bzero(&aggnode, sizeof (aggnode));
dt_node_type_assign(&aggnode, dtt.dtt_ctfp, dtt.dtt_type, B_FALSE);

for (i = 0, j = 0; i < pfv->pfv_argc; i++, pfd = pfd->pfd_next) {
const dt_pfconv_t *pfc = pfd->pfd_conv;
const char *dyns[2];
int dync = 0;

char vname[64];
dt_node_t *vnp;

if (pfc == NULL)
continue;

if (pfc->pfc_print == &pfprint_pct) {
(void) strcat(pfd->pfd_fmt, pfc->pfc_ofmt);
continue;
}

if (pfd->pfd_flags & DT_PFCONV_DYNPREC)
dyns[dync++] = ".*";
if (pfd->pfd_flags & DT_PFCONV_DYNWIDTH)
dyns[dync++] = "*";

for (; dync != 0; dync--) {
if (dnp == NULL) {
xyerror(D_PRINTF_DYN_PROTO,
"%s( ) prototype mismatch: conversion "
"#%d (%%%s) is missing a corresponding "
"\"%s\" argument\n", func, i + 1,
pfc->pfc_name, dyns[dync - 1]);
}

if (dt_node_is_integer(dnp) == 0) {
xyerror(D_PRINTF_DYN_TYPE,
"%s( ) argument #%d is incompatible "
"with conversion #%d prototype:\n"
"\tconversion: %% %s %s\n"
"\t prototype: int\n\t argument: %s\n",
func, j + foff + 1, i + 1,
dyns[dync - 1], pfc->pfc_name,
dt_node_type_name(dnp, n, sizeof (n)));
}

dnp = dnp->dn_list;
j++;
}







if (pfd->pfd_flags & DT_PFCONV_AGG) {
if (!(flags & DT_PRINTF_AGGREGATION)) {
xyerror(D_PRINTF_AGG_CONV,
"%%@ conversion requires an aggregation"
" and is not for use with %s( )\n", func);
}
(void) strlcpy(vname, "aggregating action",
sizeof (vname));
vnp = &aggnode;
} else if (dnp == NULL) {
xyerror(D_PRINTF_ARG_PROTO,
"%s( ) prototype mismatch: conversion #%d (%%"
"%s) is missing a corresponding value argument\n",
func, i + 1, pfc->pfc_name);
} else {
(void) snprintf(vname, sizeof (vname),
"argument #%d", j + foff + 1);
vnp = dnp;
dnp = dnp->dn_list;
j++;
}







if (pfc->pfc_print == &pfprint_sint ||
pfc->pfc_print == &pfprint_uint ||
pfc->pfc_print == &pfprint_dint) {
if (dt_node_type_size(vnp) == sizeof (uint64_t))
(void) strcpy(pfd->pfd_fmt, "ll");
} else if (pfc->pfc_print == &pfprint_fp) {
if (dt_node_type_size(vnp) == sizeof (long double))
(void) strcpy(pfd->pfd_fmt, "L");
}

(void) strcat(pfd->pfd_fmt, pfc->pfc_ofmt);







if (pfc->pfc_check(pfv, pfd, vnp) == 0) {
xyerror(D_PRINTF_ARG_TYPE,
"%s( ) %s is incompatible with "
"conversion #%d prototype:\n\tconversion: %%%s\n"
"\t prototype: %s\n\t argument: %s\n", func,
vname, i + 1, pfc->pfc_name, pfc->pfc_tstr,
dt_node_type_name(vnp, n, sizeof (n)));
}
}

if ((flags & DT_PRINTF_EXACTLEN) && dnp != NULL) {
xyerror(D_PRINTF_ARG_EXTRA,
"%s( ) prototype mismatch: only %d arguments "
"required by this format string\n", func, j);
}
}

void
dt_printa_validate(dt_node_t *lhs, dt_node_t *rhs)
{
dt_ident_t *lid, *rid;
dt_node_t *lproto, *rproto;
int largc, rargc, argn;
char n1[DT_TYPE_NAMELEN];
char n2[DT_TYPE_NAMELEN];

assert(lhs->dn_kind == DT_NODE_AGG);
assert(rhs->dn_kind == DT_NODE_AGG);

lid = lhs->dn_ident;
rid = rhs->dn_ident;

lproto = ((dt_idsig_t *)lid->di_data)->dis_args;
rproto = ((dt_idsig_t *)rid->di_data)->dis_args;




for (largc = 0; lproto != NULL; lproto = lproto->dn_list)
largc++;

for (rargc = 0; rproto != NULL; rproto = rproto->dn_list)
rargc++;

if (largc != rargc) {
xyerror(D_PRINTA_AGGKEY, "printa( ): @%s and @%s do not have "
"matching key signatures: @%s has %d key%s, @%s has %d "
"key%s", lid->di_name, rid->di_name,
lid->di_name, largc, largc == 1 ? "" : "s",
rid->di_name, rargc, rargc == 1 ? "" : "s");
}




lproto = ((dt_idsig_t *)lid->di_data)->dis_args;
rproto = ((dt_idsig_t *)rid->di_data)->dis_args;

for (argn = 1; lproto != NULL; argn++, lproto = lproto->dn_list,
rproto = rproto->dn_list) {
assert(rproto != NULL);

if (dt_node_is_argcompat(lproto, rproto))
continue;

xyerror(D_PRINTA_AGGPROTO, "printa( ): @%s[ ] key #%d is "
"incompatible with @%s:\n%9s key #%d: %s\n"
"%9s key #%d: %s\n",
rid->di_name, argn, lid->di_name, lid->di_name, argn,
dt_node_type_name(lproto, n1, sizeof (n1)), rid->di_name,
argn, dt_node_type_name(rproto, n2, sizeof (n2)));
}
}

static int
dt_printf_getint(dtrace_hdl_t *dtp, const dtrace_recdesc_t *recp,
uint_t nrecs, const void *buf, size_t len, int *ip)
{
uintptr_t addr;

if (nrecs == 0)
return (dt_set_errno(dtp, EDT_DMISMATCH));

addr = (uintptr_t)buf + recp->dtrd_offset;

if (addr + sizeof (int) > (uintptr_t)buf + len)
return (dt_set_errno(dtp, EDT_DOFFSET));

if (addr & (recp->dtrd_alignment - 1))
return (dt_set_errno(dtp, EDT_DALIGN));

switch (recp->dtrd_size) {
case sizeof (int8_t):
*ip = (int)*((int8_t *)addr);
break;
case sizeof (int16_t):
*ip = (int)*((int16_t *)addr);
break;
case sizeof (int32_t):
*ip = (int)*((int32_t *)addr);
break;
case sizeof (int64_t):
*ip = (int)*((int64_t *)addr);
break;
default:
return (dt_set_errno(dtp, EDT_DMISMATCH));
}

return (0);
}


static int
pfprint_average(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
const uint64_t *data = addr;

if (size != sizeof (uint64_t) * 2)
return (dt_set_errno(dtp, EDT_DMISMATCH));

return (dt_printf(dtp, fp, format,
data[0] ? data[1] / normal / data[0] : 0));
}


static int
pfprint_stddev(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
const uint64_t *data = addr;

if (size != sizeof (uint64_t) * 4)
return (dt_set_errno(dtp, EDT_DMISMATCH));

return (dt_printf(dtp, fp, format,
dt_stddev((uint64_t *)data, normal)));
}


static int
pfprint_quantize(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
return (dt_print_quantize(dtp, fp, addr, size, normal));
}


static int
pfprint_lquantize(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
return (dt_print_lquantize(dtp, fp, addr, size, normal));
}


static int
pfprint_llquantize(dtrace_hdl_t *dtp, FILE *fp, const char *format,
const dt_pfargd_t *pfd, const void *addr, size_t size, uint64_t normal)
{
return (dt_print_llquantize(dtp, fp, addr, size, normal));
}

static int
dt_printf_format(dtrace_hdl_t *dtp, FILE *fp, const dt_pfargv_t *pfv,
const dtrace_recdesc_t *recs, uint_t nrecs, const void *buf,
size_t len, const dtrace_aggdata_t **aggsdata, int naggvars)
{
dt_pfargd_t *pfd = pfv->pfv_argv;
const dtrace_recdesc_t *recp = recs;
const dtrace_aggdata_t *aggdata;
dtrace_aggdesc_t *agg;
caddr_t lim = (caddr_t)buf + len, limit;
char format[64] = "%";
size_t ret;
int i, aggrec, curagg = -1;
uint64_t normal;










if (pfv->pfv_flags & DT_PRINTF_AGGREGATION) {
assert(aggsdata != NULL);
assert(naggvars > 0);

if (nrecs == 0)
return (dt_set_errno(dtp, EDT_DMISMATCH));

curagg = naggvars > 1 ? 1 : 0;
aggdata = aggsdata[0];
aggrec = aggdata->dtada_desc->dtagd_nrecs - 1;
nrecs--;
}

for (i = 0; i < pfv->pfv_argc; i++, pfd = pfd->pfd_next) {
const dt_pfconv_t *pfc = pfd->pfd_conv;
int width = pfd->pfd_width;
int prec = pfd->pfd_prec;
int rval;

const char *start;
char *f = format + 1;
size_t fmtsz = sizeof(format) - 1;
const dtrace_recdesc_t *rec;
dt_pfprint_f *func;
caddr_t addr;
size_t size;
uint32_t flags;

if (pfd->pfd_preflen != 0) {
char *tmp = alloca(pfd->pfd_preflen + 1);

bcopy(pfd->pfd_prefix, tmp, pfd->pfd_preflen);
tmp[pfd->pfd_preflen] = '\0';

if ((rval = dt_printf(dtp, fp, tmp)) < 0)
return (rval);

if (pfv->pfv_flags & DT_PRINTF_AGGREGATION) {





flags = DTRACE_BUFDATA_AGGFORMAT;

if (pfc == NULL && i == pfv->pfv_argc - 1)
flags |= DTRACE_BUFDATA_AGGLAST;

if (dt_buffered_flush(dtp, NULL, NULL,
aggdata, flags) < 0)
return (-1);
}
}

if (pfc == NULL) {
if (pfv->pfv_argc == 1)
return (nrecs != 0);
continue;
}





if (pfc->pfc_print == &pfprint_pct) {
if (pfc->pfc_print(dtp, fp, NULL, pfd, NULL, 0, 1) >= 0)
continue;
return (-1);
}

if (pfd->pfd_flags & DT_PFCONV_DYNWIDTH) {
if (dt_printf_getint(dtp, recp++, nrecs--, buf,
len, &width) == -1)
return (-1);
pfd->pfd_dynwidth = width;
} else {
pfd->pfd_dynwidth = 0;
}

if ((pfd->pfd_flags & DT_PFCONV_DYNPREC) && dt_printf_getint(
dtp, recp++, nrecs--, buf, len, &prec) == -1)
return (-1);

if (pfd->pfd_flags & DT_PFCONV_AGG) {






if (aggsdata == NULL)
return (dt_set_errno(dtp, EDT_DMISMATCH));

aggdata = aggsdata[curagg];
agg = aggdata->dtada_desc;









if (curagg < naggvars - 1)
curagg++;

rec = &agg->dtagd_rec[aggrec];
addr = aggdata->dtada_data + rec->dtrd_offset;
limit = addr + aggdata->dtada_size;
normal = aggdata->dtada_normal;
flags = DTRACE_BUFDATA_AGGVAL;
} else {
if (nrecs == 0)
return (dt_set_errno(dtp, EDT_DMISMATCH));

if (pfv->pfv_flags & DT_PRINTF_AGGREGATION) {








aggdata = aggsdata[0];
flags = DTRACE_BUFDATA_AGGKEY;
}

rec = recp++;
nrecs--;
addr = (caddr_t)buf + rec->dtrd_offset;
limit = lim;
normal = 1;
}

size = rec->dtrd_size;

if (addr + size > limit) {
dt_dprintf("bad size: addr=%p size=0x%x lim=%p\n",
(void *)addr, rec->dtrd_size, (void *)lim);
return (dt_set_errno(dtp, EDT_DOFFSET));
}

if (rec->dtrd_alignment != 0 &&
((uintptr_t)addr & (rec->dtrd_alignment - 1)) != 0) {
dt_dprintf("bad align: addr=%p size=0x%x align=0x%x\n",
(void *)addr, rec->dtrd_size, rec->dtrd_alignment);
return (dt_set_errno(dtp, EDT_DALIGN));
}

switch (rec->dtrd_action) {
case DTRACEAGG_AVG:
func = pfprint_average;
break;
case DTRACEAGG_STDDEV:
func = pfprint_stddev;
break;
case DTRACEAGG_QUANTIZE:
func = pfprint_quantize;
break;
case DTRACEAGG_LQUANTIZE:
func = pfprint_lquantize;
break;
case DTRACEAGG_LLQUANTIZE:
func = pfprint_llquantize;
break;
case DTRACEACT_MOD:
func = pfprint_mod;
break;
case DTRACEACT_UMOD:
func = pfprint_umod;
break;
default:
func = pfc->pfc_print;
break;
}

start = f;
if (pfd->pfd_flags & DT_PFCONV_ALT)
*f++ = '#';
if (pfd->pfd_flags & DT_PFCONV_ZPAD)
*f++ = '0';
if (width < 0 || (pfd->pfd_flags & DT_PFCONV_LEFT))
*f++ = '-';
if (pfd->pfd_flags & DT_PFCONV_SPOS)
*f++ = '+';
if (pfd->pfd_flags & DT_PFCONV_GROUP)
*f++ = '\'';
if (pfd->pfd_flags & DT_PFCONV_SPACE)
*f++ = ' ';
fmtsz -= f - start;







if (func == pfprint_stack && (pfd->pfd_flags & DT_PFCONV_LEFT))
width = 0;

if (width != 0) {
ret = snprintf(f, fmtsz, "%d", ABS(width));
f += ret;
fmtsz = MAX(0, fmtsz - ret);
}

if (prec > 0) {
ret = snprintf(f, fmtsz, ".%d", prec);
f += ret;
fmtsz = MAX(0, fmtsz - ret);
}

if (strlcpy(f, pfd->pfd_fmt, fmtsz) >= fmtsz)
return (dt_set_errno(dtp, EDT_COMPILER));
pfd->pfd_rec = rec;

if (func(dtp, fp, format, pfd, addr, size, normal) < 0)
return (-1);

if (pfv->pfv_flags & DT_PRINTF_AGGREGATION) {





if (i == pfv->pfv_argc - 1)
flags |= DTRACE_BUFDATA_AGGLAST;

if (dt_buffered_flush(dtp, NULL,
rec, aggdata, flags) < 0)
return (-1);
}
}

return ((int)(recp - recs));
}

int
dtrace_sprintf(dtrace_hdl_t *dtp, FILE *fp, void *fmtdata,
const dtrace_recdesc_t *recp, uint_t nrecs, const void *buf, size_t len)
{
dtrace_optval_t size;
int rval;

rval = dtrace_getopt(dtp, "strsize", &size);
assert(rval == 0);
assert(dtp->dt_sprintf_buflen == 0);

if (dtp->dt_sprintf_buf != NULL)
free(dtp->dt_sprintf_buf);

if ((dtp->dt_sprintf_buf = malloc(size)) == NULL)
return (dt_set_errno(dtp, EDT_NOMEM));

bzero(dtp->dt_sprintf_buf, size);
dtp->dt_sprintf_buflen = size;
rval = dt_printf_format(dtp, fp, fmtdata, recp, nrecs, buf, len,
NULL, 0);
dtp->dt_sprintf_buflen = 0;

if (rval == -1)
free(dtp->dt_sprintf_buf);

return (rval);
}


int
dtrace_system(dtrace_hdl_t *dtp, FILE *fp, void *fmtdata,
const dtrace_probedata_t *data, const dtrace_recdesc_t *recp,
uint_t nrecs, const void *buf, size_t len)
{
int rval = dtrace_sprintf(dtp, fp, fmtdata, recp, nrecs, buf, len);

if (rval == -1)
return (rval);






(void) fflush(fp);

if (system(dtp->dt_sprintf_buf) == -1)
return (dt_set_errno(dtp, errno));

return (rval);
}

int
dtrace_freopen(dtrace_hdl_t *dtp, FILE *fp, void *fmtdata,
const dtrace_probedata_t *data, const dtrace_recdesc_t *recp,
uint_t nrecs, const void *buf, size_t len)
{
char selfbuf[40], restorebuf[40], *filename;
FILE *nfp;
int rval, errval;
dt_pfargv_t *pfv = fmtdata;
dt_pfargd_t *pfd = pfv->pfv_argv;

rval = dtrace_sprintf(dtp, fp, fmtdata, recp, nrecs, buf, len);

if (rval == -1 || fp == NULL)
return (rval);

#if defined(illumos)
if (pfd->pfd_preflen != 0 &&
strcmp(pfd->pfd_prefix, DT_FREOPEN_RESTORE) == 0) {





assert(strcmp(dtp->dt_sprintf_buf, DT_FREOPEN_RESTORE) == 0);

if (dtp->dt_stdout_fd == -1) {







return (rval);
}

(void) snprintf(restorebuf, sizeof (restorebuf),
"/dev/fd/%d", dtp->dt_stdout_fd);
filename = restorebuf;
} else {
filename = dtp->dt_sprintf_buf;
}

















if ((nfp = fopen(filename, "aF")) == NULL) {
char *msg = strerror(errno);
char *faultstr;
int len = 80;

len += strlen(msg) + strlen(filename);
faultstr = alloca(len);

(void) snprintf(faultstr, len, "couldn't freopen() \"%s\": %s",
filename, strerror(errno));

if ((errval = dt_handle_liberr(dtp, data, faultstr)) == 0)
return (rval);

return (errval);
}

(void) snprintf(selfbuf, sizeof (selfbuf), "/dev/fd/%d", fileno(nfp));

if (dtp->dt_stdout_fd == -1) {






if ((dtp->dt_stdout_fd = dup(fileno(fp))) == -1) {
(void) fclose(nfp);
return (dt_set_errno(dtp, errno));
}
}

if (freopen(selfbuf, "aF", fp) == NULL) {
(void) fclose(nfp);
return (dt_set_errno(dtp, errno));
}

(void) fclose(nfp);
#else





















if (pfd->pfd_preflen != 0 &&
strcmp(pfd->pfd_prefix, DT_FREOPEN_RESTORE) == 0) {





assert(strcmp(dtp->dt_sprintf_buf, DT_FREOPEN_RESTORE) == 0);

if (dtp->dt_freopen_fp == NULL) {







return (rval);
}






(void) fclose(dtp->dt_freopen_fp);
dtp->dt_freopen_fp = NULL;

return (rval);
}

if ((nfp = fopen(dtp->dt_sprintf_buf, "a")) == NULL) {
char *msg = strerror(errno);
char *faultstr;
int len = 80;

len += strlen(msg) + strlen(dtp->dt_sprintf_buf);
faultstr = alloca(len);

(void) snprintf(faultstr, len, "couldn't freopen() \"%s\": %s",
dtp->dt_sprintf_buf, strerror(errno));

if ((errval = dt_handle_liberr(dtp, data, faultstr)) == 0)
return (rval);

return (errval);
}

if (dtp->dt_freopen_fp != NULL)
(void) fclose(dtp->dt_freopen_fp);


dtp->dt_freopen_fp = nfp;
#endif

return (rval);
}


int
dtrace_fprintf(dtrace_hdl_t *dtp, FILE *fp, void *fmtdata,
const dtrace_probedata_t *data, const dtrace_recdesc_t *recp,
uint_t nrecs, const void *buf, size_t len)
{
return (dt_printf_format(dtp, fp, fmtdata,
recp, nrecs, buf, len, NULL, 0));
}

void *
dtrace_printf_create(dtrace_hdl_t *dtp, const char *s)
{
dt_pfargv_t *pfv = dt_printf_create(dtp, s);
dt_pfargd_t *pfd;
int i;

if (pfv == NULL)
return (NULL);

pfd = pfv->pfv_argv;

for (i = 0; i < pfv->pfv_argc; i++, pfd = pfd->pfd_next) {
const dt_pfconv_t *pfc = pfd->pfd_conv;

if (pfc == NULL)
continue;









if (strcmp(pfc->pfc_ofmt, "s") != 0)
(void) strcat(pfd->pfd_fmt, pfc->pfc_name);
else
(void) strcat(pfd->pfd_fmt, pfc->pfc_ofmt);
}

return (pfv);
}

void *
dtrace_printa_create(dtrace_hdl_t *dtp, const char *s)
{
dt_pfargv_t *pfv = dtrace_printf_create(dtp, s);

if (pfv == NULL)
return (NULL);

pfv->pfv_flags |= DT_PRINTF_AGGREGATION;

return (pfv);
}


size_t
dtrace_printf_format(dtrace_hdl_t *dtp, void *fmtdata, char *s, size_t len)
{
dt_pfargv_t *pfv = fmtdata;
dt_pfargd_t *pfd = pfv->pfv_argv;







size_t formatlen = strlen(pfv->pfv_format) + 3 * pfv->pfv_argc + 1;
char *format = alloca(formatlen);
char *f = format;
int i, j;

for (i = 0; i < pfv->pfv_argc; i++, pfd = pfd->pfd_next) {
const dt_pfconv_t *pfc = pfd->pfd_conv;
const char *str;
int width = pfd->pfd_width;
int prec = pfd->pfd_prec;

if (pfd->pfd_preflen != 0) {
for (j = 0; j < pfd->pfd_preflen; j++)
*f++ = pfd->pfd_prefix[j];
}

if (pfc == NULL)
continue;

*f++ = '%';

if (pfd->pfd_flags & DT_PFCONV_ALT)
*f++ = '#';
if (pfd->pfd_flags & DT_PFCONV_ZPAD)
*f++ = '0';
if (pfd->pfd_flags & DT_PFCONV_LEFT)
*f++ = '-';
if (pfd->pfd_flags & DT_PFCONV_SPOS)
*f++ = '+';
if (pfd->pfd_flags & DT_PFCONV_DYNWIDTH)
*f++ = '*';
if (pfd->pfd_flags & DT_PFCONV_DYNPREC) {
*f++ = '.';
*f++ = '*';
}
if (pfd->pfd_flags & DT_PFCONV_GROUP)
*f++ = '\'';
if (pfd->pfd_flags & DT_PFCONV_SPACE)
*f++ = ' ';
if (pfd->pfd_flags & DT_PFCONV_AGG)
*f++ = '@';

if (width != 0)
f += snprintf(f, sizeof (format), "%d", width);

if (prec != 0)
f += snprintf(f, sizeof (format), ".%d", prec);








if (strcmp(pfc->pfc_ofmt, "s") == 0)
str = pfc->pfc_name;
else
str = pfd->pfd_fmt;

for (j = 0; str[j] != '\0'; j++)
*f++ = str[j];
}

*f = '\0';

assert(f < format + formatlen);
(void) strncpy(s, format, len);

return ((size_t)(f - format));
}

static int
dt_fprinta(const dtrace_aggdata_t *adp, void *arg)
{
const dtrace_aggdesc_t *agg = adp->dtada_desc;
const dtrace_recdesc_t *recp = &agg->dtagd_rec[0];
uint_t nrecs = agg->dtagd_nrecs;
dt_pfwalk_t *pfw = arg;
dtrace_hdl_t *dtp = pfw->pfw_argv->pfv_dtp;
int id;

if (dt_printf_getint(dtp, recp++, nrecs--,
adp->dtada_data, adp->dtada_size, &id) != 0 || pfw->pfw_aid != id)
return (0);

if (dt_printf_format(dtp, pfw->pfw_fp, pfw->pfw_argv,
recp, nrecs, adp->dtada_data, adp->dtada_size, &adp, 1) == -1)
return (pfw->pfw_err = dtp->dt_errno);





((dtrace_aggdesc_t *)agg)->dtagd_flags |= DTRACE_AGD_PRINTED;

return (0);
}

static int
dt_fprintas(const dtrace_aggdata_t **aggsdata, int naggvars, void *arg)
{
const dtrace_aggdata_t *aggdata = aggsdata[0];
const dtrace_aggdesc_t *agg = aggdata->dtada_desc;
const dtrace_recdesc_t *rec = &agg->dtagd_rec[1];
uint_t nrecs = agg->dtagd_nrecs - 1;
dt_pfwalk_t *pfw = arg;
dtrace_hdl_t *dtp = pfw->pfw_argv->pfv_dtp;
int i;

if (dt_printf_format(dtp, pfw->pfw_fp, pfw->pfw_argv,
rec, nrecs, aggdata->dtada_data, aggdata->dtada_size,
aggsdata, naggvars) == -1)
return (pfw->pfw_err = dtp->dt_errno);





for (i = 1; i < naggvars; i++) {
agg = aggsdata[i]->dtada_desc;
((dtrace_aggdesc_t *)agg)->dtagd_flags |= DTRACE_AGD_PRINTED;
}

return (0);
}

int
dtrace_fprinta(dtrace_hdl_t *dtp, FILE *fp, void *fmtdata,
const dtrace_probedata_t *data, const dtrace_recdesc_t *recs,
uint_t nrecs, const void *buf, size_t len)
{
dt_pfwalk_t pfw;
int i, naggvars = 0;
dtrace_aggvarid_t *aggvars;

aggvars = alloca(nrecs * sizeof (dtrace_aggvarid_t));






for (i = 0; i < nrecs; i++) {
const dtrace_recdesc_t *nrec = &recs[i];

if (nrec->dtrd_uarg != recs->dtrd_uarg)
break;

if (nrec->dtrd_action != recs->dtrd_action)
return (dt_set_errno(dtp, EDT_BADAGG));

aggvars[naggvars++] =

*((dtrace_aggvarid_t *)((caddr_t)buf + nrec->dtrd_offset));
}

if (naggvars == 0)
return (dt_set_errno(dtp, EDT_BADAGG));

pfw.pfw_argv = fmtdata;
pfw.pfw_fp = fp;
pfw.pfw_err = 0;

if (naggvars == 1) {
pfw.pfw_aid = aggvars[0];

if (dtrace_aggregate_walk_sorted(dtp,
dt_fprinta, &pfw) == -1 || pfw.pfw_err != 0)
return (-1);
} else {
if (dtrace_aggregate_walk_joined(dtp, aggvars, naggvars,
dt_fprintas, &pfw) == -1 || pfw.pfw_err != 0)
return (-1);
}

return (i);
}
