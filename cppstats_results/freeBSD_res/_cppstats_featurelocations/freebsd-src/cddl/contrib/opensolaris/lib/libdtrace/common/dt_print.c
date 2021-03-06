


































































#include <sys/sysmacros.h>
#include <strings.h>
#include <stdlib.h>
#include <alloca.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <dt_module.h>
#include <dt_printf.h>
#include <dt_string.h>
#include <dt_impl.h>


#define CTF_IS_CHAR(e) (((e).cte_format & (CTF_INT_CHAR | CTF_INT_SIGNED)) == (CTF_INT_CHAR | CTF_INT_SIGNED) && (e).cte_bits == NBBY)



#define CTF_IS_STRUCTLIKE(k) ((k) == CTF_K_STRUCT || (k) == CTF_K_UNION)





typedef struct dt_printarg {
dtrace_hdl_t *pa_dtp;
caddr_t pa_addr;
ctf_file_t *pa_ctfp;
int pa_depth;
int pa_nest;
FILE *pa_file;
} dt_printarg_t;

static int dt_print_member(const char *, ctf_id_t, ulong_t, int, void *);





static void
dt_print_type_name(ctf_file_t *ctfp, ctf_id_t id, char *buf, size_t buflen)
{
if (ctf_type_name(ctfp, id, buf, buflen) == NULL)
(void) snprintf(buf, buflen, "<%ld>", id);
}






static void
dt_print_trailing_braces(dt_printarg_t *pap, int depth)
{
int d;

for (d = pap->pa_depth; d > depth; d--) {
(void) fprintf(pap->pa_file, "%*s}%s",
(d + pap->pa_nest - 1) * 4, "",
d == depth + 1 ? "" : "\n");
}
}





static void
dt_print_indent(dt_printarg_t *pap)
{
(void) fprintf(pap->pa_file, "%*s",
(pap->pa_depth + pap->pa_nest) * 4, "");
}







static void
print_bitfield(dt_printarg_t *pap, ulong_t off, ctf_encoding_t *ep)
{
FILE *fp = pap->pa_file;
caddr_t addr = pap->pa_addr + off / NBBY;
uint64_t mask = (1ULL << ep->cte_bits) - 1;
uint64_t value = 0;
size_t size = (ep->cte_bits + (NBBY - 1)) / NBBY;
uint8_t *buf = (uint8_t *)&value;
uint8_t shift;






#if BYTE_ORDER == _BIG_ENDIAN
buf += sizeof (value) - size;
off += ep->cte_bits;
#endif
bcopy(addr, buf, size);
shift = off % NBBY;





#if BYTE_ORDER == _BIG_ENDIAN
shift = NBBY - shift;
#endif





if (off % NBBY != 0)
value >>= shift;
value &= mask;

(void) fprintf(fp, "%#llx", (u_longlong_t)value);
}




static void
dt_print_hex(FILE *fp, caddr_t addr, size_t size)
{
switch (size) {
case sizeof (uint8_t):
(void) fprintf(fp, "%#x", *(uint8_t *)addr);
break;
case sizeof (uint16_t):

(void) fprintf(fp, "%#x", *(uint16_t *)addr);
break;
case sizeof (uint32_t):

(void) fprintf(fp, "%#x", *(uint32_t *)addr);
break;
case sizeof (uint64_t):
(void) fprintf(fp, "%#llx",

(unsigned long long)*(uint64_t *)addr);
break;
default:
(void) fprintf(fp, "<invalid size %u>", (uint_t)size);
}
}





static void
dt_print_int(ctf_id_t base, ulong_t off, dt_printarg_t *pap)
{
FILE *fp = pap->pa_file;
ctf_file_t *ctfp = pap->pa_ctfp;
ctf_encoding_t e;
size_t size;
caddr_t addr = pap->pa_addr + off / NBBY;

if (ctf_type_encoding(ctfp, base, &e) == CTF_ERR) {
(void) fprintf(fp, "<unknown encoding>");
return;
}





if (e.cte_format & CTF_INT_VARARGS) {
(void) fprintf(fp, "...");
return;
}





size = e.cte_bits / NBBY;
if (size > 8 || (e.cte_bits % NBBY) != 0 || (size & (size - 1)) != 0) {
print_bitfield(pap, off, &e);
return;
}




if (CTF_IS_CHAR(e)) {
char c = *(char *)addr;
if (isprint(c))
(void) fprintf(fp, "'%c'", c);
else if (c == 0)
(void) fprintf(fp, "'\\0'");
else
(void) fprintf(fp, "'\\%03o'", c);
return;
}

dt_print_hex(fp, addr, size);
}





static void
dt_print_float(ctf_id_t base, ulong_t off, dt_printarg_t *pap)
{
FILE *fp = pap->pa_file;
ctf_file_t *ctfp = pap->pa_ctfp;
ctf_encoding_t e;
caddr_t addr = pap->pa_addr + off / NBBY;

if (ctf_type_encoding(ctfp, base, &e) == 0) {
if (e.cte_format == CTF_FP_SINGLE &&
e.cte_bits == sizeof (float) * NBBY) {

(void) fprintf(fp, "%+.7e", *((float *)addr));
} else if (e.cte_format == CTF_FP_DOUBLE &&
e.cte_bits == sizeof (double) * NBBY) {

(void) fprintf(fp, "%+.7e", *((double *)addr));
} else if (e.cte_format == CTF_FP_LDOUBLE &&
e.cte_bits == sizeof (long double) * NBBY) {

(void) fprintf(fp, "%+.16LE", *((long double *)addr));
} else {
(void) fprintf(fp, "<unknown encoding>");
}
}
}





static void
dt_print_ptr(ctf_id_t base, ulong_t off, dt_printarg_t *pap)
{
FILE *fp = pap->pa_file;
ctf_file_t *ctfp = pap->pa_ctfp;
caddr_t addr = pap->pa_addr + off / NBBY;
size_t size = ctf_type_size(ctfp, base);
ctf_id_t bid = ctf_type_reference(ctfp, base);
uint64_t pc;
dtrace_syminfo_t dts;
GElf_Sym sym;

if (bid == CTF_ERR || ctf_type_kind(ctfp, bid) != CTF_K_FUNCTION) {
dt_print_hex(fp, addr, size);
} else {

pc = *((uint64_t *)addr);
if (dtrace_lookup_by_addr(pap->pa_dtp, pc, &sym, &dts) != 0) {
dt_print_hex(fp, addr, size);
} else {
(void) fprintf(fp, "%s`%s", dts.dts_object,
dts.dts_name);
}
}
}



























static void
dt_print_array(ctf_id_t base, ulong_t off, dt_printarg_t *pap)
{
FILE *fp = pap->pa_file;
ctf_file_t *ctfp = pap->pa_ctfp;
caddr_t addr = pap->pa_addr + off / NBBY;
ctf_arinfo_t car;
ssize_t eltsize;
ctf_encoding_t e;
int i;
boolean_t isstring;
int kind;
ctf_id_t rtype;

if (ctf_array_info(ctfp, base, &car) == CTF_ERR) {
(void) fprintf(fp, "%p", (void *)addr);
return;
}

if ((eltsize = ctf_type_size(ctfp, car.ctr_contents)) < 0 ||
(rtype = ctf_type_resolve(ctfp, car.ctr_contents)) == CTF_ERR ||
(kind = ctf_type_kind(ctfp, rtype)) == CTF_ERR) {
(void) fprintf(fp, "<invalid type %lu>", car.ctr_contents);
return;
}


isstring = B_FALSE;
if (kind == CTF_K_INTEGER &&
ctf_type_encoding(ctfp, rtype, &e) != CTF_ERR && CTF_IS_CHAR(e)) {
char c;
for (i = 0; i < car.ctr_nelems; i++) {
c = *((char *)addr + eltsize * i);
if (!isprint(c) || c == '\0')
break;
}

if (i != car.ctr_nelems && c == '\0')
isstring = B_TRUE;
}










if (!isstring || pap->pa_depth != 0)
(void) fprintf(fp, "[ ");

if (isstring)
(void) fprintf(fp, "\"");

for (i = 0; i < car.ctr_nelems; i++) {
if (isstring) {
char c = *((char *)addr + eltsize * i);
if (c == '\0')
break;
(void) fprintf(fp, "%c", c);
} else {





dt_printarg_t pa = *pap;
pa.pa_nest += pap->pa_depth + 1;
pa.pa_depth = 0;
pa.pa_addr = addr + eltsize * i;
(void) ctf_type_visit(ctfp, car.ctr_contents,
dt_print_member, &pa);

dt_print_trailing_braces(&pa, 0);
if (i != car.ctr_nelems - 1)
(void) fprintf(fp, ", ");
else if (CTF_IS_STRUCTLIKE(kind))
(void) fprintf(fp, "\n");
}
}

if (isstring)
(void) fprintf(fp, "\"");

if (!isstring || pap->pa_depth != 0) {
if (CTF_IS_STRUCTLIKE(kind))
dt_print_indent(pap);
else
(void) fprintf(fp, " ");
(void) fprintf(fp, "]");
}
}





static void
dt_print_structlike(ctf_id_t id, ulong_t off, dt_printarg_t *pap)
{
(void) fprintf(pap->pa_file, "{");
}






static void
dt_print_enum(ctf_id_t base, ulong_t off, dt_printarg_t *pap)
{
FILE *fp = pap->pa_file;
ctf_file_t *ctfp = pap->pa_ctfp;
const char *ename;
ssize_t size;
caddr_t addr = pap->pa_addr + off / NBBY;
int value = 0;






size = ctf_type_size(ctfp, base);
switch (size) {
case sizeof (uint8_t):
value = *(uint8_t *)addr;
break;
case sizeof (uint16_t):
value = *(uint16_t *)addr;
break;
case sizeof (int32_t):
value = *(int32_t *)addr;
break;
default:
(void) fprintf(fp, "<invalid enum size %u>", (uint_t)size);
return;
}

if ((ename = ctf_enum_name(ctfp, base, value)) != NULL)
(void) fprintf(fp, "%s", ename);
else
(void) fprintf(fp, "%d", value);
}






static void
dt_print_tag(ctf_id_t base, ulong_t off, dt_printarg_t *pap)
{
(void) fprintf(pap->pa_file, "<forward decl>");
}

typedef void dt_printarg_f(ctf_id_t, ulong_t, dt_printarg_t *);

static dt_printarg_f *const dt_printfuncs[] = {
dt_print_int,
dt_print_float,
dt_print_ptr,
dt_print_array,
dt_print_ptr,
dt_print_structlike,
dt_print_structlike,
dt_print_enum,
dt_print_tag
};





static int
dt_print_member(const char *name, ctf_id_t id, ulong_t off, int depth,
void *data)
{
char type[DT_TYPE_NAMELEN];
int kind;
dt_printarg_t *pap = data;
FILE *fp = pap->pa_file;
ctf_file_t *ctfp = pap->pa_ctfp;
boolean_t arraymember;
boolean_t brief;
ctf_encoding_t e;
ctf_id_t rtype;

dt_print_trailing_braces(pap, depth);




if (depth < pap->pa_depth)
(void) fprintf(fp, "\n");
pap->pa_depth = depth;

if ((rtype = ctf_type_resolve(ctfp, id)) == CTF_ERR ||
(kind = ctf_type_kind(ctfp, rtype)) == CTF_ERR ||
kind < CTF_K_INTEGER || kind > CTF_K_FORWARD) {
dt_print_indent(pap);
(void) fprintf(fp, "%s = <invalid type %lu>", name, id);
return (0);
}

dt_print_type_name(ctfp, id, type, sizeof (type));

arraymember = (pap->pa_nest != 0 && depth == 0);
brief = (arraymember && !CTF_IS_STRUCTLIKE(kind));

if (!brief) {






if (arraymember)
(void) fprintf(fp, "\n");
dt_print_indent(pap);


(void) fprintf(fp, "%s", type);
if (name[0] != '\0') {









if (type[strlen(type) - 1] != '*' &&
type[strlen(type) -1] != ' ') {
(void) fprintf(fp, " ");
}
(void) fprintf(fp, "%s", name);






if (kind == CTF_K_INTEGER &&
ctf_type_encoding(ctfp, id, &e) == 0) {
ulong_t bits = e.cte_bits;
ulong_t size = bits / NBBY;

if (bits % NBBY != 0 ||
off % NBBY != 0 ||
size > 8 ||
size != ctf_type_size(ctfp, id)) {
(void) fprintf(fp, " :%lu", bits);
}
}

(void) fprintf(fp, " =");
}
(void) fprintf(fp, " ");
}

dt_printfuncs[kind - 1](rtype, off, pap);


if (!brief)
(void) fprintf(fp, "\n");

return (0);
}




int
dtrace_print(dtrace_hdl_t *dtp, FILE *fp, const char *typename,
caddr_t addr, size_t len)
{
const char *s;
char *object;
dt_printarg_t pa;
ctf_id_t id;
dt_module_t *dmp;
ctf_file_t *ctfp;
int libid;









for (s = typename; *s != '\0' && *s != '`'; s++)
;

if (*s != '`')
return (0);

object = alloca(s - typename + 1);
bcopy(typename, object, s - typename);
object[s - typename] = '\0';
dmp = dt_module_lookup_by_name(dtp, object);
if (dmp == NULL)
return (0);

if (dmp->dm_pid != 0) {
libid = atoi(s + 1);
s = strchr(s + 1, '`');
if (s == NULL || libid > dmp->dm_nctflibs)
return (0);
ctfp = dmp->dm_libctfp[libid];
} else {
ctfp = dt_module_getctf(dtp, dmp);
}

id = atoi(s + 1);






if (ctfp == NULL || ctf_type_kind(ctfp, id) == CTF_ERR)
return (0);


pa.pa_dtp = dtp;
pa.pa_addr = addr;
pa.pa_ctfp = ctfp;
pa.pa_nest = 0;
pa.pa_depth = 0;
pa.pa_file = fp;
(void) ctf_type_visit(pa.pa_ctfp, id, dt_print_member, &pa);

dt_print_trailing_braces(&pa, 0);

return (len);
}
