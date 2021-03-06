





















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap-types.h>
#if defined(_WIN32)
#include <ws2tcpip.h>
#else
#include <sys/socket.h>

#if defined(__NetBSD__)
#include <sys/param.h>
#endif

#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <setjmp.h>
#include <stdarg.h>

#if defined(MSDOS)
#include "pcap-dos.h"
#endif

#include "pcap-int.h"

#include "extract.h"

#include "ethertype.h"
#include "nlpid.h"
#include "llc.h"
#include "gencode.h"
#include "ieee80211.h"
#include "atmuni31.h"
#include "sunatmpos.h"
#include "ppp.h"
#include "pcap/sll.h"
#include "pcap/ipnet.h"
#include "arcnet.h"

#include "grammar.h"
#include "scanner.h"

#if defined(linux) && defined(PF_PACKET) && defined(SO_ATTACH_FILTER)
#include <linux/types.h>
#include <linux/if_packet.h>
#include <linux/filter.h>
#endif

#if defined(HAVE_NET_PFVAR_H)
#include <sys/socket.h>
#include <net/if.h>
#include <net/pfvar.h>
#include <net/if_pflog.h>
#endif

#if !defined(offsetof)
#define offsetof(s, e) ((size_t)&((s *)0)->e)
#endif

#if defined(_WIN32)
#if defined(INET6)
#if defined(__MINGW32__) && defined(DEFINE_ADDITIONAL_IPV6_STUFF)

struct in6_addr
{
union
{
uint8_t u6_addr8[16];
uint16_t u6_addr16[8];
uint32_t u6_addr32[4];
} in6_u;
#define s6_addr in6_u.u6_addr8
#define s6_addr16 in6_u.u6_addr16
#define s6_addr32 in6_u.u6_addr32
#define s6_addr64 in6_u.u6_addr64
};

typedef unsigned short sa_family_t;

#define __SOCKADDR_COMMON(sa_prefix) sa_family_t sa_prefix##family



struct sockaddr_in6
{
__SOCKADDR_COMMON (sin6_);
uint16_t sin6_port;
uint32_t sin6_flowinfo;
struct in6_addr sin6_addr;
};

#if !defined(EAI_ADDRFAMILY)
struct addrinfo {
int ai_flags;
int ai_family;
int ai_socktype;
int ai_protocol;
size_t ai_addrlen;
char *ai_canonname;
struct sockaddr *ai_addr;
struct addrinfo *ai_next;
};
#endif
#endif
#endif
#else
#include <netdb.h>
#endif
#include <pcap/namedb.h>

#include "nametoaddr.h"

#define ETHERMTU 1500

#if !defined(ETHERTYPE_TEB)
#define ETHERTYPE_TEB 0x6558
#endif

#if !defined(IPPROTO_HOPOPTS)
#define IPPROTO_HOPOPTS 0
#endif
#if !defined(IPPROTO_ROUTING)
#define IPPROTO_ROUTING 43
#endif
#if !defined(IPPROTO_FRAGMENT)
#define IPPROTO_FRAGMENT 44
#endif
#if !defined(IPPROTO_DSTOPTS)
#define IPPROTO_DSTOPTS 60
#endif
#if !defined(IPPROTO_SCTP)
#define IPPROTO_SCTP 132
#endif

#define GENEVE_PORT 6081

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif

#define JMP(c) ((c)|BPF_JMP|BPF_K)






#define PUSH_LINKHDR(cs, new_linktype, new_is_variable, new_constant_part, new_reg) { (cs)->prevlinktype = (cs)->linktype; (cs)->off_prevlinkhdr = (cs)->off_linkhdr; (cs)->linktype = (new_linktype); (cs)->off_linkhdr.is_variable = (new_is_variable); (cs)->off_linkhdr.constant_part = (new_constant_part); (cs)->off_linkhdr.reg = (new_reg); (cs)->is_geneve = 0; }













#define OFFSET_NOT_SET 0xffffffffU


















typedef struct {
int is_variable;
u_int constant_part;
int reg;
} bpf_abs_offset;





enum e_offrel {
OR_PACKET,
OR_LINKHDR,
OR_PREVLINKHDR,
OR_LLC,
OR_PREVMPLSHDR,
OR_LINKTYPE,
OR_LINKPL,
OR_LINKPL_NOSNAP,
OR_TRAN_IPV4,
OR_TRAN_IPV6
};









#define NCHUNKS 16
#define CHUNK0SIZE 1024
struct chunk {
size_t n_left;
void *m;
};



struct _compiler_state {
jmp_buf top_ctx;
pcap_t *bpf_pcap;

struct icode ic;

int snaplen;

int linktype;
int prevlinktype;
int outermostlinktype;

bpf_u_int32 netmask;
int no_optimize;


u_int label_stack_depth;
u_int vlan_stack_depth;


u_int pcap_fddipad;










struct addrinfo *ai;






u_char *e;










bpf_abs_offset off_linkhdr;







bpf_abs_offset off_prevlinkhdr;





bpf_abs_offset off_outermostlinkhdr;




bpf_abs_offset off_linkpl;
























bpf_abs_offset off_linktype;




int is_atm;






int is_geneve;




int is_vlan_vloffset;




u_int off_vpi;
u_int off_vci;
u_int off_proto;




u_int off_li;
u_int off_li_hsl;




u_int off_sio;
u_int off_opc;
u_int off_dpc;
u_int off_sls;





u_int off_payload;


























u_int off_nl;
u_int off_nl_nosnap;





int regused[BPF_MEMWORDS];
int curreg;




struct chunk chunks[NCHUNKS];
int cur_chunk;
};





void
bpf_set_error(compiler_state_t *cstate, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
(void)pcap_vsnprintf(cstate->bpf_pcap->errbuf, PCAP_ERRBUF_SIZE,
fmt, ap);
va_end(ap);
}




static void PCAP_NORETURN bpf_error(compiler_state_t *, const char *, ...)
PCAP_PRINTFLIKE(2, 3);


static void PCAP_NORETURN
bpf_error(compiler_state_t *cstate, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
(void)pcap_vsnprintf(cstate->bpf_pcap->errbuf, PCAP_ERRBUF_SIZE,
fmt, ap);
va_end(ap);
longjmp(cstate->top_ctx, 1);

}

static int init_linktype(compiler_state_t *, pcap_t *);

static void init_regs(compiler_state_t *);
static int alloc_reg(compiler_state_t *);
static void free_reg(compiler_state_t *, int);

static void initchunks(compiler_state_t *cstate);
static void *newchunk_nolongjmp(compiler_state_t *cstate, size_t);
static void *newchunk(compiler_state_t *cstate, size_t);
static void freechunks(compiler_state_t *cstate);
static inline struct block *new_block(compiler_state_t *cstate, int);
static inline struct slist *new_stmt(compiler_state_t *cstate, int);
static struct block *gen_retblk(compiler_state_t *cstate, int);
static inline void syntax(compiler_state_t *cstate);

static void backpatch(struct block *, struct block *);
static void merge(struct block *, struct block *);
static struct block *gen_cmp(compiler_state_t *, enum e_offrel, u_int,
u_int, bpf_int32);
static struct block *gen_cmp_gt(compiler_state_t *, enum e_offrel, u_int,
u_int, bpf_int32);
static struct block *gen_cmp_ge(compiler_state_t *, enum e_offrel, u_int,
u_int, bpf_int32);
static struct block *gen_cmp_lt(compiler_state_t *, enum e_offrel, u_int,
u_int, bpf_int32);
static struct block *gen_cmp_le(compiler_state_t *, enum e_offrel, u_int,
u_int, bpf_int32);
static struct block *gen_mcmp(compiler_state_t *, enum e_offrel, u_int,
u_int, bpf_int32, bpf_u_int32);
static struct block *gen_bcmp(compiler_state_t *, enum e_offrel, u_int,
u_int, const u_char *);
static struct block *gen_ncmp(compiler_state_t *, enum e_offrel, bpf_u_int32,
bpf_u_int32, bpf_u_int32, bpf_u_int32, int, bpf_int32);
static struct slist *gen_load_absoffsetrel(compiler_state_t *, bpf_abs_offset *,
u_int, u_int);
static struct slist *gen_load_a(compiler_state_t *, enum e_offrel, u_int,
u_int);
static struct slist *gen_loadx_iphdrlen(compiler_state_t *);
static struct block *gen_uncond(compiler_state_t *, int);
static inline struct block *gen_true(compiler_state_t *);
static inline struct block *gen_false(compiler_state_t *);
static struct block *gen_ether_linktype(compiler_state_t *, int);
static struct block *gen_ipnet_linktype(compiler_state_t *, int);
static struct block *gen_linux_sll_linktype(compiler_state_t *, int);
static struct slist *gen_load_prism_llprefixlen(compiler_state_t *);
static struct slist *gen_load_avs_llprefixlen(compiler_state_t *);
static struct slist *gen_load_radiotap_llprefixlen(compiler_state_t *);
static struct slist *gen_load_ppi_llprefixlen(compiler_state_t *);
static void insert_compute_vloffsets(compiler_state_t *, struct block *);
static struct slist *gen_abs_offset_varpart(compiler_state_t *,
bpf_abs_offset *);
static int ethertype_to_ppptype(int);
static struct block *gen_linktype(compiler_state_t *, int);
static struct block *gen_snap(compiler_state_t *, bpf_u_int32, bpf_u_int32);
static struct block *gen_llc_linktype(compiler_state_t *, int);
static struct block *gen_hostop(compiler_state_t *, bpf_u_int32, bpf_u_int32,
int, int, u_int, u_int);
#if defined(INET6)
static struct block *gen_hostop6(compiler_state_t *, struct in6_addr *,
struct in6_addr *, int, int, u_int, u_int);
#endif
static struct block *gen_ahostop(compiler_state_t *, const u_char *, int);
static struct block *gen_ehostop(compiler_state_t *, const u_char *, int);
static struct block *gen_fhostop(compiler_state_t *, const u_char *, int);
static struct block *gen_thostop(compiler_state_t *, const u_char *, int);
static struct block *gen_wlanhostop(compiler_state_t *, const u_char *, int);
static struct block *gen_ipfchostop(compiler_state_t *, const u_char *, int);
static struct block *gen_dnhostop(compiler_state_t *, bpf_u_int32, int);
static struct block *gen_mpls_linktype(compiler_state_t *, int);
static struct block *gen_host(compiler_state_t *, bpf_u_int32, bpf_u_int32,
int, int, int);
#if defined(INET6)
static struct block *gen_host6(compiler_state_t *, struct in6_addr *,
struct in6_addr *, int, int, int);
#endif
#if !defined(INET6)
static struct block *gen_gateway(compiler_state_t *, const u_char *,
struct addrinfo *, int, int);
#endif
static struct block *gen_ipfrag(compiler_state_t *);
static struct block *gen_portatom(compiler_state_t *, int, bpf_int32);
static struct block *gen_portrangeatom(compiler_state_t *, int, bpf_int32,
bpf_int32);
static struct block *gen_portatom6(compiler_state_t *, int, bpf_int32);
static struct block *gen_portrangeatom6(compiler_state_t *, int, bpf_int32,
bpf_int32);
struct block *gen_portop(compiler_state_t *, int, int, int);
static struct block *gen_port(compiler_state_t *, int, int, int);
struct block *gen_portrangeop(compiler_state_t *, int, int, int, int);
static struct block *gen_portrange(compiler_state_t *, int, int, int, int);
struct block *gen_portop6(compiler_state_t *, int, int, int);
static struct block *gen_port6(compiler_state_t *, int, int, int);
struct block *gen_portrangeop6(compiler_state_t *, int, int, int, int);
static struct block *gen_portrange6(compiler_state_t *, int, int, int, int);
static int lookup_proto(compiler_state_t *, const char *, int);
static struct block *gen_protochain(compiler_state_t *, int, int, int);
static struct block *gen_proto(compiler_state_t *, int, int, int);
static struct slist *xfer_to_x(compiler_state_t *, struct arth *);
static struct slist *xfer_to_a(compiler_state_t *, struct arth *);
static struct block *gen_mac_multicast(compiler_state_t *, int);
static struct block *gen_len(compiler_state_t *, int, int);
static struct block *gen_check_802_11_data_frame(compiler_state_t *);
static struct block *gen_geneve_ll_check(compiler_state_t *cstate);

static struct block *gen_ppi_dlt_check(compiler_state_t *);
static struct block *gen_atmfield_code_internal(compiler_state_t *, int,
bpf_int32, bpf_u_int32, int);
static struct block *gen_atmtype_llc(compiler_state_t *);
static struct block *gen_msg_abbrev(compiler_state_t *, int type);

static void
initchunks(compiler_state_t *cstate)
{
int i;

for (i = 0; i < NCHUNKS; i++) {
cstate->chunks[i].n_left = 0;
cstate->chunks[i].m = NULL;
}
cstate->cur_chunk = 0;
}

static void *
newchunk_nolongjmp(compiler_state_t *cstate, size_t n)
{
struct chunk *cp;
int k;
size_t size;

#if !defined(__NetBSD__)

n = (n + sizeof(long) - 1) & ~(sizeof(long) - 1);
#else

n = ALIGN(n);
#endif

cp = &cstate->chunks[cstate->cur_chunk];
if (n > cp->n_left) {
++cp;
k = ++cstate->cur_chunk;
if (k >= NCHUNKS) {
bpf_set_error(cstate, "out of memory");
return (NULL);
}
size = CHUNK0SIZE << k;
cp->m = (void *)malloc(size);
if (cp->m == NULL) {
bpf_set_error(cstate, "out of memory");
return (NULL);
}
memset((char *)cp->m, 0, size);
cp->n_left = size;
if (n > size) {
bpf_set_error(cstate, "out of memory");
return (NULL);
}
}
cp->n_left -= n;
return (void *)((char *)cp->m + cp->n_left);
}

static void *
newchunk(compiler_state_t *cstate, size_t n)
{
void *p;

p = newchunk_nolongjmp(cstate, n);
if (p == NULL) {
longjmp(cstate->top_ctx, 1);

}
return (p);
}

static void
freechunks(compiler_state_t *cstate)
{
int i;

for (i = 0; i < NCHUNKS; ++i)
if (cstate->chunks[i].m != NULL)
free(cstate->chunks[i].m);
}







char *
sdup(compiler_state_t *cstate, const char *s)
{
size_t n = strlen(s) + 1;
char *cp = newchunk_nolongjmp(cstate, n);

if (cp == NULL)
return (NULL);
pcap_strlcpy(cp, s, n);
return (cp);
}

static inline struct block *
new_block(compiler_state_t *cstate, int code)
{
struct block *p;

p = (struct block *)newchunk(cstate, sizeof(*p));
p->s.code = code;
p->head = p;

return p;
}

static inline struct slist *
new_stmt(compiler_state_t *cstate, int code)
{
struct slist *p;

p = (struct slist *)newchunk(cstate, sizeof(*p));
p->s.code = code;

return p;
}

static struct block *
gen_retblk(compiler_state_t *cstate, int v)
{
struct block *b = new_block(cstate, BPF_RET|BPF_K);

b->s.k = v;
return b;
}

static inline PCAP_NORETURN_DEF void
syntax(compiler_state_t *cstate)
{
bpf_error(cstate, "syntax error in filter expression");
}

int
pcap_compile(pcap_t *p, struct bpf_program *program,
const char *buf, int optimize, bpf_u_int32 mask)
{
#if defined(_WIN32)
static int done = 0;
#endif
compiler_state_t cstate;
const char * volatile xbuf = buf;
yyscan_t scanner = NULL;
volatile YY_BUFFER_STATE in_buffer = NULL;
u_int len;
int rc;





if (!p->activated) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"not-yet-activated pcap_t passed to pcap_compile");
return (-1);
}

#if defined(_WIN32)
if (!done)
pcap_wsockinit();
done = 1;
#endif

#if defined(ENABLE_REMOTE)
















if (p->save_current_filter_op != NULL)
(p->save_current_filter_op)(p, buf);
#endif

initchunks(&cstate);
cstate.no_optimize = 0;
#if defined(INET6)
cstate.ai = NULL;
#endif
cstate.e = NULL;
cstate.ic.root = NULL;
cstate.ic.cur_mark = 0;
cstate.bpf_pcap = p;
init_regs(&cstate);

cstate.netmask = mask;

cstate.snaplen = pcap_snapshot(p);
if (cstate.snaplen == 0) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"snaplen of 0 rejects all packets");
rc = -1;
goto quit;
}

if (pcap_lex_init(&scanner) != 0)
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "can't initialize scanner");
in_buffer = pcap__scan_string(xbuf ? xbuf : "", scanner);





pcap_set_extra(&cstate, scanner);

if (init_linktype(&cstate, p) == -1) {
rc = -1;
goto quit;
}
if (pcap_parse(scanner, &cstate) != 0) {
#if defined(INET6)
if (cstate.ai != NULL)
freeaddrinfo(cstate.ai);
#endif
if (cstate.e != NULL)
free(cstate.e);
rc = -1;
goto quit;
}

if (cstate.ic.root == NULL) {



if (setjmp(cstate.top_ctx)) {
rc = -1;
goto quit;
}
cstate.ic.root = gen_retblk(&cstate, cstate.snaplen);
}

if (optimize && !cstate.no_optimize) {
if (bpf_optimize(&cstate.ic, p->errbuf) == -1) {

rc = -1;
goto quit;
}
if (cstate.ic.root == NULL ||
(cstate.ic.root->s.code == (BPF_RET|BPF_K) && cstate.ic.root->s.k == 0)) {
(void)pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"expression rejects all packets");
rc = -1;
goto quit;
}
}
program->bf_insns = icode_to_fcode(&cstate.ic,
cstate.ic.root, &len, p->errbuf);
if (program->bf_insns == NULL) {

rc = -1;
goto quit;
}
program->bf_len = len;

rc = 0;

quit:



if (in_buffer != NULL)
pcap__delete_buffer(in_buffer, scanner);
if (scanner != NULL)
pcap_lex_destroy(scanner);




freechunks(&cstate);

return (rc);
}





int
pcap_compile_nopcap(int snaplen_arg, int linktype_arg,
struct bpf_program *program,
const char *buf, int optimize, bpf_u_int32 mask)
{
pcap_t *p;
int ret;

p = pcap_open_dead(linktype_arg, snaplen_arg);
if (p == NULL)
return (-1);
ret = pcap_compile(p, program, buf, optimize, mask);
pcap_close(p);
return (ret);
}





void
pcap_freecode(struct bpf_program *program)
{
program->bf_len = 0;
if (program->bf_insns != NULL) {
free((char *)program->bf_insns);
program->bf_insns = NULL;
}
}







static void
backpatch(struct block *list, struct block *target)
{
struct block *next;

while (list) {
if (!list->sense) {
next = JT(list);
JT(list) = target;
} else {
next = JF(list);
JF(list) = target;
}
list = next;
}
}





static void
merge(struct block *b0, struct block *b1)
{
register struct block **p = &b0;


while (*p)
p = !((*p)->sense) ? &JT(*p) : &JF(*p);


*p = b1;
}

int
finish_parse(compiler_state_t *cstate, struct block *p)
{
struct block *ppi_dlt_check;





if (setjmp(cstate->top_ctx))
return (-1);




















insert_compute_vloffsets(cstate, p->head);














ppi_dlt_check = gen_ppi_dlt_check(cstate);
if (ppi_dlt_check != NULL)
gen_and(ppi_dlt_check, p);

backpatch(p, gen_retblk(cstate, cstate->snaplen));
p->sense = !p->sense;
backpatch(p, gen_retblk(cstate, 0));
cstate->ic.root = p->head;
return (0);
}

void
gen_and(struct block *b0, struct block *b1)
{
backpatch(b0, b1->head);
b0->sense = !b0->sense;
b1->sense = !b1->sense;
merge(b1, b0);
b1->sense = !b1->sense;
b1->head = b0->head;
}

void
gen_or(struct block *b0, struct block *b1)
{
b0->sense = !b0->sense;
backpatch(b0, b1->head);
b0->sense = !b0->sense;
merge(b1, b0);
b1->head = b0->head;
}

void
gen_not(struct block *b)
{
b->sense = !b->sense;
}

static struct block *
gen_cmp(compiler_state_t *cstate, enum e_offrel offrel, u_int offset,
u_int size, bpf_int32 v)
{
return gen_ncmp(cstate, offrel, offset, size, 0xffffffff, BPF_JEQ, 0, v);
}

static struct block *
gen_cmp_gt(compiler_state_t *cstate, enum e_offrel offrel, u_int offset,
u_int size, bpf_int32 v)
{
return gen_ncmp(cstate, offrel, offset, size, 0xffffffff, BPF_JGT, 0, v);
}

static struct block *
gen_cmp_ge(compiler_state_t *cstate, enum e_offrel offrel, u_int offset,
u_int size, bpf_int32 v)
{
return gen_ncmp(cstate, offrel, offset, size, 0xffffffff, BPF_JGE, 0, v);
}

static struct block *
gen_cmp_lt(compiler_state_t *cstate, enum e_offrel offrel, u_int offset,
u_int size, bpf_int32 v)
{
return gen_ncmp(cstate, offrel, offset, size, 0xffffffff, BPF_JGE, 1, v);
}

static struct block *
gen_cmp_le(compiler_state_t *cstate, enum e_offrel offrel, u_int offset,
u_int size, bpf_int32 v)
{
return gen_ncmp(cstate, offrel, offset, size, 0xffffffff, BPF_JGT, 1, v);
}

static struct block *
gen_mcmp(compiler_state_t *cstate, enum e_offrel offrel, u_int offset,
u_int size, bpf_int32 v, bpf_u_int32 mask)
{
return gen_ncmp(cstate, offrel, offset, size, mask, BPF_JEQ, 0, v);
}

static struct block *
gen_bcmp(compiler_state_t *cstate, enum e_offrel offrel, u_int offset,
u_int size, const u_char *v)
{
register struct block *b, *tmp;











b = NULL;
while (size >= 4) {
register const u_char *p = &v[size - 4];

tmp = gen_cmp(cstate, offrel, offset + size - 4, BPF_W,
(bpf_int32)EXTRACT_32BITS(p));
if (b != NULL)
gen_and(b, tmp);
b = tmp;
size -= 4;
}
while (size >= 2) {
register const u_char *p = &v[size - 2];

tmp = gen_cmp(cstate, offrel, offset + size - 2, BPF_H,
(bpf_int32)EXTRACT_16BITS(p));
if (b != NULL)
gen_and(b, tmp);
b = tmp;
size -= 2;
}
if (size > 0) {
tmp = gen_cmp(cstate, offrel, offset, BPF_B, (bpf_int32)v[0]);
if (b != NULL)
gen_and(b, tmp);
b = tmp;
}
return b;
}







static struct block *
gen_ncmp(compiler_state_t *cstate, enum e_offrel offrel, bpf_u_int32 offset,
bpf_u_int32 size, bpf_u_int32 mask, bpf_u_int32 jtype, int reverse,
bpf_int32 v)
{
struct slist *s, *s2;
struct block *b;

s = gen_load_a(cstate, offrel, offset, size);

if (mask != 0xffffffff) {
s2 = new_stmt(cstate, BPF_ALU|BPF_AND|BPF_K);
s2->s.k = mask;
sappend(s, s2);
}

b = new_block(cstate, JMP(jtype));
b->stmts = s;
b->s.k = v;
if (reverse && (jtype == BPF_JGT || jtype == BPF_JGE))
gen_not(b);
return b;
}

static int
init_linktype(compiler_state_t *cstate, pcap_t *p)
{
cstate->pcap_fddipad = p->fddipad;




cstate->outermostlinktype = pcap_datalink(p);
cstate->off_outermostlinkhdr.constant_part = 0;
cstate->off_outermostlinkhdr.is_variable = 0;
cstate->off_outermostlinkhdr.reg = -1;

cstate->prevlinktype = cstate->outermostlinktype;
cstate->off_prevlinkhdr.constant_part = 0;
cstate->off_prevlinkhdr.is_variable = 0;
cstate->off_prevlinkhdr.reg = -1;

cstate->linktype = cstate->outermostlinktype;
cstate->off_linkhdr.constant_part = 0;
cstate->off_linkhdr.is_variable = 0;
cstate->off_linkhdr.reg = -1;




cstate->off_linkpl.constant_part = 0;
cstate->off_linkpl.is_variable = 0;
cstate->off_linkpl.reg = -1;

cstate->off_linktype.constant_part = 0;
cstate->off_linktype.is_variable = 0;
cstate->off_linktype.reg = -1;




cstate->is_atm = 0;
cstate->off_vpi = OFFSET_NOT_SET;
cstate->off_vci = OFFSET_NOT_SET;
cstate->off_proto = OFFSET_NOT_SET;
cstate->off_payload = OFFSET_NOT_SET;




cstate->is_geneve = 0;




cstate->is_vlan_vloffset = 0;




cstate->off_li = OFFSET_NOT_SET;
cstate->off_li_hsl = OFFSET_NOT_SET;
cstate->off_sio = OFFSET_NOT_SET;
cstate->off_opc = OFFSET_NOT_SET;
cstate->off_dpc = OFFSET_NOT_SET;
cstate->off_sls = OFFSET_NOT_SET;

cstate->label_stack_depth = 0;
cstate->vlan_stack_depth = 0;

switch (cstate->linktype) {

case DLT_ARCNET:
cstate->off_linktype.constant_part = 2;
cstate->off_linkpl.constant_part = 6;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_ARCNET_LINUX:
cstate->off_linktype.constant_part = 4;
cstate->off_linkpl.constant_part = 8;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_EN10MB:
cstate->off_linktype.constant_part = 12;
cstate->off_linkpl.constant_part = 14;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 3;
break;

case DLT_SLIP:




cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = 16;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_SLIP_BSDOS:

cstate->off_linktype.constant_part = OFFSET_NOT_SET;

cstate->off_linkpl.constant_part = 24;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_NULL:
case DLT_LOOP:
cstate->off_linktype.constant_part = 0;
cstate->off_linkpl.constant_part = 4;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_ENC:
cstate->off_linktype.constant_part = 0;
cstate->off_linkpl.constant_part = 12;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_PPP:
case DLT_PPP_PPPD:
case DLT_C_HDLC:
case DLT_PPP_SERIAL:
cstate->off_linktype.constant_part = 2;
cstate->off_linkpl.constant_part = 4;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_PPP_ETHER:




cstate->off_linktype.constant_part = 6;
cstate->off_linkpl.constant_part = 8;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_PPP_BSDOS:
cstate->off_linktype.constant_part = 5;
cstate->off_linkpl.constant_part = 24;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_FDDI:








cstate->off_linktype.constant_part = 13;
cstate->off_linktype.constant_part += cstate->pcap_fddipad;
cstate->off_linkpl.constant_part = 13;
cstate->off_linkpl.constant_part += cstate->pcap_fddipad;
cstate->off_nl = 8;
cstate->off_nl_nosnap = 3;
break;

case DLT_IEEE802:























cstate->off_linktype.constant_part = 14;
cstate->off_linkpl.constant_part = 14;
cstate->off_nl = 8;
cstate->off_nl_nosnap = 3;
break;

case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
cstate->off_linkhdr.is_variable = 1;




case DLT_IEEE802_11:


















cstate->off_linktype.constant_part = 24;
cstate->off_linkpl.constant_part = 0;
cstate->off_linkpl.is_variable = 1;
cstate->off_nl = 8;
cstate->off_nl_nosnap = 3;
break;

case DLT_PPI:









cstate->off_linktype.constant_part = 24;
cstate->off_linkpl.constant_part = 0;
cstate->off_linkpl.is_variable = 1;
cstate->off_linkhdr.is_variable = 1;
cstate->off_nl = 8;
cstate->off_nl_nosnap = 3;
break;

case DLT_ATM_RFC1483:
case DLT_ATM_CLIP:











cstate->off_linktype.constant_part = 0;
cstate->off_linkpl.constant_part = 0;
cstate->off_nl = 8;
cstate->off_nl_nosnap = 3;
break;

case DLT_SUNATM:




cstate->is_atm = 1;
cstate->off_vpi = SUNATM_VPI_POS;
cstate->off_vci = SUNATM_VCI_POS;
cstate->off_proto = PROTO_POS;
cstate->off_payload = SUNATM_PKT_BEGIN_POS;
cstate->off_linktype.constant_part = cstate->off_payload;
cstate->off_linkpl.constant_part = cstate->off_payload;
cstate->off_nl = 8;
cstate->off_nl_nosnap = 3;
break;

case DLT_RAW:
case DLT_IPV4:
case DLT_IPV6:
cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = 0;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_LINUX_SLL:
cstate->off_linktype.constant_part = 14;
cstate->off_linkpl.constant_part = 16;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_LINUX_SLL2:
cstate->off_linktype.constant_part = 0;
cstate->off_linkpl.constant_part = 20;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_LTALK:





cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = 0;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_IP_OVER_FC:










cstate->off_linktype.constant_part = 16;
cstate->off_linkpl.constant_part = 16;
cstate->off_nl = 8;
cstate->off_nl_nosnap = 3;
break;

case DLT_FRELAY:




cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = 0;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;






case DLT_MFR:
cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = 0;
cstate->off_nl = 4;
cstate->off_nl_nosnap = 0;
break;

case DLT_APPLE_IP_OVER_IEEE1394:
cstate->off_linktype.constant_part = 16;
cstate->off_linkpl.constant_part = 18;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_SYMANTEC_FIREWALL:
cstate->off_linktype.constant_part = 6;
cstate->off_linkpl.constant_part = 44;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

#if defined(HAVE_NET_PFVAR_H)
case DLT_PFLOG:
cstate->off_linktype.constant_part = 0;
cstate->off_linkpl.constant_part = PFLOG_HDRLEN;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;
#endif

case DLT_JUNIPER_MFR:
case DLT_JUNIPER_MLFR:
case DLT_JUNIPER_MLPPP:
case DLT_JUNIPER_PPP:
case DLT_JUNIPER_CHDLC:
case DLT_JUNIPER_FRELAY:
cstate->off_linktype.constant_part = 4;
cstate->off_linkpl.constant_part = 4;
cstate->off_nl = 0;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_JUNIPER_ATM1:
cstate->off_linktype.constant_part = 4;
cstate->off_linkpl.constant_part = 4;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 10;
break;

case DLT_JUNIPER_ATM2:
cstate->off_linktype.constant_part = 8;
cstate->off_linkpl.constant_part = 8;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 10;
break;



case DLT_JUNIPER_PPPOE:
case DLT_JUNIPER_ETHER:
cstate->off_linkpl.constant_part = 14;
cstate->off_linktype.constant_part = 16;
cstate->off_nl = 18;
cstate->off_nl_nosnap = 21;
break;

case DLT_JUNIPER_PPPOE_ATM:
cstate->off_linktype.constant_part = 4;
cstate->off_linkpl.constant_part = 6;
cstate->off_nl = 0;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_JUNIPER_GGSN:
cstate->off_linktype.constant_part = 6;
cstate->off_linkpl.constant_part = 12;
cstate->off_nl = 0;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_JUNIPER_ES:
cstate->off_linktype.constant_part = 6;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_JUNIPER_MONITOR:
cstate->off_linktype.constant_part = 12;
cstate->off_linkpl.constant_part = 12;
cstate->off_nl = 0;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_BACNET_MS_TP:
cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_JUNIPER_SERVICES:
cstate->off_linktype.constant_part = 12;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_JUNIPER_VP:
cstate->off_linktype.constant_part = 18;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_JUNIPER_ST:
cstate->off_linktype.constant_part = 18;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_JUNIPER_ISM:
cstate->off_linktype.constant_part = 8;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_JUNIPER_VS:
case DLT_JUNIPER_SRX_E2E:
case DLT_JUNIPER_FIBRECHANNEL:
case DLT_JUNIPER_ATM_CEMIC:
cstate->off_linktype.constant_part = 8;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_MTP2:
cstate->off_li = 2;
cstate->off_li_hsl = 4;
cstate->off_sio = 3;
cstate->off_opc = 4;
cstate->off_dpc = 4;
cstate->off_sls = 7;
cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_MTP2_WITH_PHDR:
cstate->off_li = 6;
cstate->off_li_hsl = 8;
cstate->off_sio = 7;
cstate->off_opc = 8;
cstate->off_dpc = 8;
cstate->off_sls = 11;
cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_ERF:
cstate->off_li = 22;
cstate->off_li_hsl = 24;
cstate->off_sio = 23;
cstate->off_opc = 24;
cstate->off_dpc = 24;
cstate->off_sls = 27;
cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_PFSYNC:
cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = 4;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;
break;

case DLT_AX25_KISS:



cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_IPNET:
cstate->off_linktype.constant_part = 1;
cstate->off_linkpl.constant_part = 24;
cstate->off_nl = 0;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
break;

case DLT_NETANALYZER:
cstate->off_linkhdr.constant_part = 4;
cstate->off_linktype.constant_part = cstate->off_linkhdr.constant_part + 12;
cstate->off_linkpl.constant_part = cstate->off_linkhdr.constant_part + 14;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 3;
break;

case DLT_NETANALYZER_TRANSPARENT:
cstate->off_linkhdr.constant_part = 12;
cstate->off_linktype.constant_part = cstate->off_linkhdr.constant_part + 12;
cstate->off_linkpl.constant_part = cstate->off_linkhdr.constant_part + 14;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 3;
break;

default:




if (cstate->linktype >= DLT_MATCHING_MIN &&
cstate->linktype <= DLT_MATCHING_MAX) {
cstate->off_linktype.constant_part = OFFSET_NOT_SET;
cstate->off_linkpl.constant_part = OFFSET_NOT_SET;
cstate->off_nl = OFFSET_NOT_SET;
cstate->off_nl_nosnap = OFFSET_NOT_SET;
} else {
bpf_set_error(cstate, "unknown data link type %d", cstate->linktype);
return (-1);
}
break;
}

cstate->off_outermostlinkhdr = cstate->off_prevlinkhdr = cstate->off_linkhdr;
return (0);
}




static struct slist *
gen_load_absoffsetrel(compiler_state_t *cstate, bpf_abs_offset *abs_offset,
u_int offset, u_int size)
{
struct slist *s, *s2;

s = gen_abs_offset_varpart(cstate, abs_offset);










if (s != NULL) {





s2 = new_stmt(cstate, BPF_LD|BPF_IND|size);
s2->s.k = abs_offset->constant_part + offset;
sappend(s, s2);
} else {




s = new_stmt(cstate, BPF_LD|BPF_ABS|size);
s->s.k = abs_offset->constant_part + offset;
}
return s;
}




static struct slist *
gen_load_a(compiler_state_t *cstate, enum e_offrel offrel, u_int offset,
u_int size)
{
struct slist *s, *s2;












s = NULL;

switch (offrel) {

case OR_PACKET:
s = new_stmt(cstate, BPF_LD|BPF_ABS|size);
s->s.k = offset;
break;

case OR_LINKHDR:
s = gen_load_absoffsetrel(cstate, &cstate->off_linkhdr, offset, size);
break;

case OR_PREVLINKHDR:
s = gen_load_absoffsetrel(cstate, &cstate->off_prevlinkhdr, offset, size);
break;

case OR_LLC:
s = gen_load_absoffsetrel(cstate, &cstate->off_linkpl, offset, size);
break;

case OR_PREVMPLSHDR:
s = gen_load_absoffsetrel(cstate, &cstate->off_linkpl, cstate->off_nl - 4 + offset, size);
break;

case OR_LINKPL:
s = gen_load_absoffsetrel(cstate, &cstate->off_linkpl, cstate->off_nl + offset, size);
break;

case OR_LINKPL_NOSNAP:
s = gen_load_absoffsetrel(cstate, &cstate->off_linkpl, cstate->off_nl_nosnap + offset, size);
break;

case OR_LINKTYPE:
s = gen_load_absoffsetrel(cstate, &cstate->off_linktype, offset, size);
break;

case OR_TRAN_IPV4:






s = gen_loadx_iphdrlen(cstate);












s2 = new_stmt(cstate, BPF_LD|BPF_IND|size);
s2->s.k = cstate->off_linkpl.constant_part + cstate->off_nl + offset;
sappend(s, s2);
break;

case OR_TRAN_IPV6:
s = gen_load_absoffsetrel(cstate, &cstate->off_linkpl, cstate->off_nl + 40 + offset, size);
break;
}
return s;
}






static struct slist *
gen_loadx_iphdrlen(compiler_state_t *cstate)
{
struct slist *s, *s2;

s = gen_abs_offset_varpart(cstate, &cstate->off_linkpl);
if (s != NULL) {










s2 = new_stmt(cstate, BPF_LD|BPF_IND|BPF_B);
s2->s.k = cstate->off_linkpl.constant_part + cstate->off_nl;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ALU|BPF_AND|BPF_K);
s2->s.k = 0xf;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ALU|BPF_LSH|BPF_K);
s2->s.k = 2;
sappend(s, s2);







sappend(s, new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_X));
sappend(s, new_stmt(cstate, BPF_MISC|BPF_TAX));
} else {












s = new_stmt(cstate, BPF_LDX|BPF_MSH|BPF_B);
s->s.k = cstate->off_linkpl.constant_part + cstate->off_nl;
}
return s;
}


static struct block *
gen_uncond(compiler_state_t *cstate, int rsense)
{
struct block *b;
struct slist *s;

s = new_stmt(cstate, BPF_LD|BPF_IMM);
s->s.k = !rsense;
b = new_block(cstate, JMP(BPF_JEQ));
b->stmts = s;

return b;
}

static inline struct block *
gen_true(compiler_state_t *cstate)
{
return gen_uncond(cstate, 1);
}

static inline struct block *
gen_false(compiler_state_t *cstate)
{
return gen_uncond(cstate, 0);
}






#define SWAPLONG(y) ((((y)&0xff)<<24) | (((y)&0xff00)<<8) | (((y)&0xff0000)>>8) | (((y)>>24)&0xff))











static struct block *
gen_ether_linktype(compiler_state_t *cstate, int proto)
{
struct block *b0, *b1;

switch (proto) {

case LLCSAP_ISONS:
case LLCSAP_IP:
case LLCSAP_NETBEUI:












b0 = gen_cmp_gt(cstate, OR_LINKTYPE, 0, BPF_H, ETHERMTU);
gen_not(b0);
b1 = gen_cmp(cstate, OR_LLC, 0, BPF_H, (bpf_int32)
((proto << 8) | proto));
gen_and(b0, b1);
return b1;

case LLCSAP_IPX:
































b0 = gen_cmp(cstate, OR_LLC, 0, BPF_B, (bpf_int32)LLCSAP_IPX);
b1 = gen_cmp(cstate, OR_LLC, 0, BPF_H, (bpf_int32)0xFFFF);
gen_or(b0, b1);





b0 = gen_snap(cstate, 0x000000, ETHERTYPE_IPX);
gen_or(b0, b1);





b0 = gen_cmp_gt(cstate, OR_LINKTYPE, 0, BPF_H, ETHERMTU);
gen_not(b0);







gen_and(b0, b1);






b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, (bpf_int32)ETHERTYPE_IPX);
gen_or(b0, b1);
return b1;

case ETHERTYPE_ATALK:
case ETHERTYPE_AARP:










b0 = gen_cmp_gt(cstate, OR_LINKTYPE, 0, BPF_H, ETHERMTU);
gen_not(b0);












if (proto == ETHERTYPE_ATALK)
b1 = gen_snap(cstate, 0x080007, ETHERTYPE_ATALK);
else
b1 = gen_snap(cstate, 0x000000, ETHERTYPE_AARP);
gen_and(b0, b1);






b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, (bpf_int32)proto);

gen_or(b0, b1);
return b1;

default:
if (proto <= ETHERMTU) {








b0 = gen_cmp_gt(cstate, OR_LINKTYPE, 0, BPF_H, ETHERMTU);
gen_not(b0);
b1 = gen_cmp(cstate, OR_LINKTYPE, 2, BPF_B, (bpf_int32)proto);
gen_and(b0, b1);
return b1;
} else {









return gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H,
(bpf_int32)proto);
}
}
}

static struct block *
gen_loopback_linktype(compiler_state_t *cstate, int proto)
{













if (cstate->linktype == DLT_NULL || cstate->linktype == DLT_ENC) {











if (cstate->bpf_pcap->rfile != NULL && cstate->bpf_pcap->swapped)
proto = SWAPLONG(proto);
proto = htonl(proto);
}
return (gen_cmp(cstate, OR_LINKHDR, 0, BPF_W, (bpf_int32)proto));
}





static struct block *
gen_ipnet_linktype(compiler_state_t *cstate, int proto)
{
switch (proto) {

case ETHERTYPE_IP:
return gen_cmp(cstate, OR_LINKTYPE, 0, BPF_B, (bpf_int32)IPH_AF_INET);


case ETHERTYPE_IPV6:
return gen_cmp(cstate, OR_LINKTYPE, 0, BPF_B,
(bpf_int32)IPH_AF_INET6);


default:
break;
}

return gen_false(cstate);
}









static struct block *
gen_linux_sll_linktype(compiler_state_t *cstate, int proto)
{
struct block *b0, *b1;

switch (proto) {

case LLCSAP_ISONS:
case LLCSAP_IP:
case LLCSAP_NETBEUI:












b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, LINUX_SLL_P_802_2);
b1 = gen_cmp(cstate, OR_LLC, 0, BPF_H, (bpf_int32)
((proto << 8) | proto));
gen_and(b0, b1);
return b1;

case LLCSAP_IPX:

























b0 = gen_cmp(cstate, OR_LLC, 0, BPF_B, (bpf_int32)LLCSAP_IPX);
b1 = gen_snap(cstate, 0x000000, ETHERTYPE_IPX);
gen_or(b0, b1);
b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, LINUX_SLL_P_802_2);
gen_and(b0, b1);





b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, LINUX_SLL_P_802_3);
gen_or(b0, b1);






b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, (bpf_int32)ETHERTYPE_IPX);
gen_or(b0, b1);
return b1;

case ETHERTYPE_ATALK:
case ETHERTYPE_AARP:










b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, LINUX_SLL_P_802_2);












if (proto == ETHERTYPE_ATALK)
b1 = gen_snap(cstate, 0x080007, ETHERTYPE_ATALK);
else
b1 = gen_snap(cstate, 0x000000, ETHERTYPE_AARP);
gen_and(b0, b1);






b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, (bpf_int32)proto);

gen_or(b0, b1);
return b1;

default:
if (proto <= ETHERMTU) {







b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, LINUX_SLL_P_802_2);
b1 = gen_cmp(cstate, OR_LINKHDR, cstate->off_linkpl.constant_part, BPF_B,
(bpf_int32)proto);
gen_and(b0, b1);
return b1;
} else {









return gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, (bpf_int32)proto);
}
}
}

static struct slist *
gen_load_prism_llprefixlen(compiler_state_t *cstate)
{
struct slist *s1, *s2;
struct slist *sjeq_avs_cookie;
struct slist *sjcommon;






cstate->no_optimize = 1;




















if (cstate->off_linkhdr.reg != -1) {



s1 = new_stmt(cstate, BPF_LD|BPF_W|BPF_ABS);
s1->s.k = 0;




s2 = new_stmt(cstate, BPF_ALU|BPF_AND|BPF_K);
s2->s.k = 0xFFFFF000;
sappend(s1, s2);




sjeq_avs_cookie = new_stmt(cstate, JMP(BPF_JEQ));
sjeq_avs_cookie->s.k = 0x80211000;
sappend(s1, sjeq_avs_cookie);








s2 = new_stmt(cstate, BPF_LD|BPF_W|BPF_ABS);
s2->s.k = 4;
sappend(s1, s2);
sjeq_avs_cookie->s.jt = s2;









sjcommon = new_stmt(cstate, JMP(BPF_JA));
sjcommon->s.k = 1;
sappend(s1, sjcommon);







s2 = new_stmt(cstate, BPF_LD|BPF_W|BPF_IMM);
s2->s.k = 144;
sappend(s1, s2);
sjeq_avs_cookie->s.jf = s2;






s2 = new_stmt(cstate, BPF_ST);
s2->s.k = cstate->off_linkhdr.reg;
sappend(s1, s2);
sjcommon->s.jf = s2;




s2 = new_stmt(cstate, BPF_MISC|BPF_TAX);
sappend(s1, s2);

return (s1);
} else
return (NULL);
}

static struct slist *
gen_load_avs_llprefixlen(compiler_state_t *cstate)
{
struct slist *s1, *s2;








if (cstate->off_linkhdr.reg != -1) {





s1 = new_stmt(cstate, BPF_LD|BPF_W|BPF_ABS);
s1->s.k = 4;





s2 = new_stmt(cstate, BPF_ST);
s2->s.k = cstate->off_linkhdr.reg;
sappend(s1, s2);




s2 = new_stmt(cstate, BPF_MISC|BPF_TAX);
sappend(s1, s2);

return (s1);
} else
return (NULL);
}

static struct slist *
gen_load_radiotap_llprefixlen(compiler_state_t *cstate)
{
struct slist *s1, *s2;








if (cstate->off_linkhdr.reg != -1) {











s1 = new_stmt(cstate, BPF_LD|BPF_B|BPF_ABS);
s1->s.k = 3;
s2 = new_stmt(cstate, BPF_ALU|BPF_LSH|BPF_K);
sappend(s1, s2);
s2->s.k = 8;
s2 = new_stmt(cstate, BPF_MISC|BPF_TAX);
sappend(s1, s2);





s2 = new_stmt(cstate, BPF_LD|BPF_B|BPF_ABS);
sappend(s1, s2);
s2->s.k = 2;
s2 = new_stmt(cstate, BPF_ALU|BPF_OR|BPF_X);
sappend(s1, s2);





s2 = new_stmt(cstate, BPF_ST);
s2->s.k = cstate->off_linkhdr.reg;
sappend(s1, s2);




s2 = new_stmt(cstate, BPF_MISC|BPF_TAX);
sappend(s1, s2);

return (s1);
} else
return (NULL);
}










static struct slist *
gen_load_ppi_llprefixlen(compiler_state_t *cstate)
{
struct slist *s1, *s2;






if (cstate->off_linkhdr.reg != -1) {











s1 = new_stmt(cstate, BPF_LD|BPF_B|BPF_ABS);
s1->s.k = 3;
s2 = new_stmt(cstate, BPF_ALU|BPF_LSH|BPF_K);
sappend(s1, s2);
s2->s.k = 8;
s2 = new_stmt(cstate, BPF_MISC|BPF_TAX);
sappend(s1, s2);





s2 = new_stmt(cstate, BPF_LD|BPF_B|BPF_ABS);
sappend(s1, s2);
s2->s.k = 2;
s2 = new_stmt(cstate, BPF_ALU|BPF_OR|BPF_X);
sappend(s1, s2);





s2 = new_stmt(cstate, BPF_ST);
s2->s.k = cstate->off_linkhdr.reg;
sappend(s1, s2);




s2 = new_stmt(cstate, BPF_MISC|BPF_TAX);
sappend(s1, s2);

return (s1);
} else
return (NULL);
}








static struct slist *
gen_load_802_11_header_len(compiler_state_t *cstate, struct slist *s, struct slist *snext)
{
struct slist *s2;
struct slist *sjset_data_frame_1;
struct slist *sjset_data_frame_2;
struct slist *sjset_qos;
struct slist *sjset_radiotap_flags_present;
struct slist *sjset_radiotap_ext_present;
struct slist *sjset_radiotap_tsft_present;
struct slist *sjset_tsft_datapad, *sjset_notsft_datapad;
struct slist *s_roundup;

if (cstate->off_linkpl.reg == -1) {






return (s);
}






cstate->no_optimize = 1;









if (s == NULL) {









s = new_stmt(cstate, BPF_LDX|BPF_IMM);
s->s.k = cstate->off_outermostlinkhdr.constant_part;
}








s2 = new_stmt(cstate, BPF_MISC|BPF_TXA);
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s2->s.k = 24;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ST);
s2->s.k = cstate->off_linkpl.reg;
sappend(s, s2);

s2 = new_stmt(cstate, BPF_LD|BPF_IND|BPF_B);
s2->s.k = 0;
sappend(s, s2);






sjset_data_frame_1 = new_stmt(cstate, JMP(BPF_JSET));
sjset_data_frame_1->s.k = 0x08;
sappend(s, sjset_data_frame_1);





sjset_data_frame_1->s.jt = sjset_data_frame_2 = new_stmt(cstate, JMP(BPF_JSET));
sjset_data_frame_2->s.k = 0x04;
sappend(s, sjset_data_frame_2);
sjset_data_frame_1->s.jf = snext;






sjset_data_frame_2->s.jt = snext;
sjset_data_frame_2->s.jf = sjset_qos = new_stmt(cstate, JMP(BPF_JSET));
sjset_qos->s.k = 0x80;
sappend(s, sjset_qos);







sjset_qos->s.jt = s2 = new_stmt(cstate, BPF_LD|BPF_MEM);
s2->s.k = cstate->off_linkpl.reg;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_IMM);
s2->s.k = 2;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ST);
s2->s.k = cstate->off_linkpl.reg;
sappend(s, s2);




















if (cstate->linktype == DLT_IEEE802_11_RADIO) {




sjset_qos->s.jf = s2 = new_stmt(cstate, BPF_LD|BPF_ABS|BPF_W);
s2->s.k = 4;
sappend(s, s2);

sjset_radiotap_flags_present = new_stmt(cstate, JMP(BPF_JSET));
sjset_radiotap_flags_present->s.k = SWAPLONG(0x00000002);
sappend(s, sjset_radiotap_flags_present);




sjset_radiotap_flags_present->s.jf = snext;




sjset_radiotap_ext_present = new_stmt(cstate, JMP(BPF_JSET));
sjset_radiotap_ext_present->s.k = SWAPLONG(0x80000000);
sappend(s, sjset_radiotap_ext_present);
sjset_radiotap_flags_present->s.jt = sjset_radiotap_ext_present;




sjset_radiotap_ext_present->s.jt = snext;




sjset_radiotap_tsft_present = new_stmt(cstate, JMP(BPF_JSET));
sjset_radiotap_tsft_present->s.k = SWAPLONG(0x00000001);
sappend(s, sjset_radiotap_tsft_present);
sjset_radiotap_ext_present->s.jf = sjset_radiotap_tsft_present;










s2 = new_stmt(cstate, BPF_LD|BPF_ABS|BPF_B);
s2->s.k = 16;
sappend(s, s2);
sjset_radiotap_tsft_present->s.jt = s2;

sjset_tsft_datapad = new_stmt(cstate, JMP(BPF_JSET));
sjset_tsft_datapad->s.k = 0x20;
sappend(s, sjset_tsft_datapad);









s2 = new_stmt(cstate, BPF_LD|BPF_ABS|BPF_B);
s2->s.k = 8;
sappend(s, s2);
sjset_radiotap_tsft_present->s.jf = s2;

sjset_notsft_datapad = new_stmt(cstate, JMP(BPF_JSET));
sjset_notsft_datapad->s.k = 0x20;
sappend(s, sjset_notsft_datapad);








s_roundup = new_stmt(cstate, BPF_LD|BPF_MEM);
s_roundup->s.k = cstate->off_linkpl.reg;
sappend(s, s_roundup);
s2 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_IMM);
s2->s.k = 3;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ALU|BPF_AND|BPF_IMM);
s2->s.k = ~3;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ST);
s2->s.k = cstate->off_linkpl.reg;
sappend(s, s2);

sjset_tsft_datapad->s.jt = s_roundup;
sjset_tsft_datapad->s.jf = snext;
sjset_notsft_datapad->s.jt = s_roundup;
sjset_notsft_datapad->s.jf = snext;
} else
sjset_qos->s.jf = snext;

return s;
}

static void
insert_compute_vloffsets(compiler_state_t *cstate, struct block *b)
{
struct slist *s;






if (cstate->off_linkpl.reg != -1 && cstate->off_linkhdr.is_variable &&
cstate->off_linkhdr.reg == -1)
cstate->off_linkhdr.reg = alloc_reg(cstate);












switch (cstate->outermostlinktype) {

case DLT_PRISM_HEADER:
s = gen_load_prism_llprefixlen(cstate);
break;

case DLT_IEEE802_11_RADIO_AVS:
s = gen_load_avs_llprefixlen(cstate);
break;

case DLT_IEEE802_11_RADIO:
s = gen_load_radiotap_llprefixlen(cstate);
break;

case DLT_PPI:
s = gen_load_ppi_llprefixlen(cstate);
break;

default:
s = NULL;
break;
}






switch (cstate->outermostlinktype) {

case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
case DLT_PPI:
s = gen_load_802_11_header_len(cstate, s, b->stmts);
break;
}





if (s == NULL && cstate->is_vlan_vloffset) {
struct slist *s2;

if (cstate->off_linkpl.reg == -1)
cstate->off_linkpl.reg = alloc_reg(cstate);
if (cstate->off_linktype.reg == -1)
cstate->off_linktype.reg = alloc_reg(cstate);

s = new_stmt(cstate, BPF_LD|BPF_W|BPF_IMM);
s->s.k = 0;
s2 = new_stmt(cstate, BPF_ST);
s2->s.k = cstate->off_linkpl.reg;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ST);
s2->s.k = cstate->off_linktype.reg;
sappend(s, s2);
}







if (s != NULL) {
sappend(s, b->stmts);
b->stmts = s;
}
}

static struct block *
gen_ppi_dlt_check(compiler_state_t *cstate)
{
struct slist *s_load_dlt;
struct block *b;

if (cstate->linktype == DLT_PPI)
{


s_load_dlt = new_stmt(cstate, BPF_LD|BPF_W|BPF_ABS);
s_load_dlt->s.k = 4;

b = new_block(cstate, JMP(BPF_JEQ));

b->stmts = s_load_dlt;
b->s.k = SWAPLONG(DLT_IEEE802_11);
}
else
{
b = NULL;
}

return b;
}














static struct slist *
gen_abs_offset_varpart(compiler_state_t *cstate, bpf_abs_offset *off)
{
struct slist *s;

if (off->is_variable) {
if (off->reg == -1) {





off->reg = alloc_reg(cstate);
}





s = new_stmt(cstate, BPF_LDX|BPF_MEM);
s->s.k = off->reg;
return s;
} else {




return NULL;
}
}




static int
ethertype_to_ppptype(int proto)
{
switch (proto) {

case ETHERTYPE_IP:
proto = PPP_IP;
break;

case ETHERTYPE_IPV6:
proto = PPP_IPV6;
break;

case ETHERTYPE_DN:
proto = PPP_DECNET;
break;

case ETHERTYPE_ATALK:
proto = PPP_APPLE;
break;

case ETHERTYPE_NS:
proto = PPP_NS;
break;

case LLCSAP_ISONS:
proto = PPP_OSI;
break;

case LLCSAP_8021D:





proto = PPP_BRPDU;
break;

case LLCSAP_IPX:
proto = PPP_IPX;
break;
}
return (proto);
}







static struct block *
gen_prevlinkhdr_check(compiler_state_t *cstate)
{
struct block *b0;

if (cstate->is_geneve)
return gen_geneve_ll_check(cstate);

switch (cstate->prevlinktype) {

case DLT_SUNATM:







b0 = gen_cmp(cstate, OR_PREVLINKHDR, SUNATM_PKT_BEGIN_POS, BPF_H, 0xFF00);
gen_not(b0);
return b0;

default:



return NULL;
}

}





#define BSD_AFNUM_INET6_BSD 24
#define BSD_AFNUM_INET6_FREEBSD 28
#define BSD_AFNUM_INET6_DARWIN 30








static struct block *
gen_linktype(compiler_state_t *cstate, int proto)
{
struct block *b0, *b1, *b2;
const char *description;


if (cstate->label_stack_depth > 0) {
switch (proto) {
case ETHERTYPE_IP:
case PPP_IP:

return gen_mpls_linktype(cstate, Q_IP);

case ETHERTYPE_IPV6:
case PPP_IPV6:

return gen_mpls_linktype(cstate, Q_IPV6);

default:
bpf_error(cstate, "unsupported protocol over mpls");

}
}

switch (cstate->linktype) {

case DLT_EN10MB:
case DLT_NETANALYZER:
case DLT_NETANALYZER_TRANSPARENT:


if (!cstate->is_geneve)
b0 = gen_prevlinkhdr_check(cstate);
else
b0 = NULL;

b1 = gen_ether_linktype(cstate, proto);
if (b0 != NULL)
gen_and(b0, b1);
return b1;


case DLT_C_HDLC:
switch (proto) {

case LLCSAP_ISONS:
proto = (proto << 8 | LLCSAP_ISONS);


default:
return gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, (bpf_int32)proto);

}

case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
case DLT_PPI:



b0 = gen_check_802_11_data_frame(cstate);




b1 = gen_llc_linktype(cstate, proto);
gen_and(b0, b1);
return b1;


case DLT_FDDI:



return gen_llc_linktype(cstate, proto);


case DLT_IEEE802:



return gen_llc_linktype(cstate, proto);


case DLT_ATM_RFC1483:
case DLT_ATM_CLIP:
case DLT_IP_OVER_FC:
return gen_llc_linktype(cstate, proto);


case DLT_SUNATM:







b0 = gen_atmfield_code_internal(cstate, A_PROTOTYPE, PT_LLC, BPF_JEQ, 0);
b1 = gen_llc_linktype(cstate, proto);
gen_and(b0, b1);
return b1;


case DLT_LINUX_SLL:
return gen_linux_sll_linktype(cstate, proto);


case DLT_SLIP:
case DLT_SLIP_BSDOS:
case DLT_RAW:







switch (proto) {

case ETHERTYPE_IP:

return gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, 0x40, 0xF0);

case ETHERTYPE_IPV6:

return gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, 0x60, 0xF0);

default:
return gen_false(cstate);
}


case DLT_IPV4:



if (proto == ETHERTYPE_IP)
return gen_true(cstate);


return gen_false(cstate);


case DLT_IPV6:



if (proto == ETHERTYPE_IPV6)
return gen_true(cstate);


return gen_false(cstate);


case DLT_PPP:
case DLT_PPP_PPPD:
case DLT_PPP_SERIAL:
case DLT_PPP_ETHER:




proto = ethertype_to_ppptype(proto);
return gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, (bpf_int32)proto);


case DLT_PPP_BSDOS:




switch (proto) {

case ETHERTYPE_IP:




b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, PPP_IP);
b1 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, PPP_VJC);
gen_or(b0, b1);
b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, PPP_VJNC);
gen_or(b1, b0);
return b0;

default:
proto = ethertype_to_ppptype(proto);
return gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H,
(bpf_int32)proto);
}


case DLT_NULL:
case DLT_LOOP:
case DLT_ENC:
switch (proto) {

case ETHERTYPE_IP:
return (gen_loopback_linktype(cstate, AF_INET));

case ETHERTYPE_IPV6:






















if (cstate->bpf_pcap->rfile != NULL) {




b0 = gen_loopback_linktype(cstate, BSD_AFNUM_INET6_BSD);
b1 = gen_loopback_linktype(cstate, BSD_AFNUM_INET6_FREEBSD);
gen_or(b0, b1);
b0 = gen_loopback_linktype(cstate, BSD_AFNUM_INET6_DARWIN);
gen_or(b0, b1);
return (b1);
} else {





#if defined(_WIN32)






return (gen_loopback_linktype(cstate, 24));
#else
#if defined(AF_INET6)
return (gen_loopback_linktype(cstate, AF_INET6));
#else




return gen_false(cstate);
#endif
#endif
}

default:





return gen_false(cstate);
}

#if defined(HAVE_NET_PFVAR_H)
case DLT_PFLOG:




if (proto == ETHERTYPE_IP)
return (gen_cmp(cstate, OR_LINKHDR, offsetof(struct pfloghdr, af),
BPF_B, (bpf_int32)AF_INET));
else if (proto == ETHERTYPE_IPV6)
return (gen_cmp(cstate, OR_LINKHDR, offsetof(struct pfloghdr, af),
BPF_B, (bpf_int32)AF_INET6));
else
return gen_false(cstate);

#endif

case DLT_ARCNET:
case DLT_ARCNET_LINUX:




switch (proto) {

default:
return gen_false(cstate);

case ETHERTYPE_IPV6:
return (gen_cmp(cstate, OR_LINKTYPE, 0, BPF_B,
(bpf_int32)ARCTYPE_INET6));

case ETHERTYPE_IP:
b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_B,
(bpf_int32)ARCTYPE_IP);
b1 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_B,
(bpf_int32)ARCTYPE_IP_OLD);
gen_or(b0, b1);
return (b1);

case ETHERTYPE_ARP:
b0 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_B,
(bpf_int32)ARCTYPE_ARP);
b1 = gen_cmp(cstate, OR_LINKTYPE, 0, BPF_B,
(bpf_int32)ARCTYPE_ARP_OLD);
gen_or(b0, b1);
return (b1);

case ETHERTYPE_REVARP:
return (gen_cmp(cstate, OR_LINKTYPE, 0, BPF_B,
(bpf_int32)ARCTYPE_REVARP));

case ETHERTYPE_ATALK:
return (gen_cmp(cstate, OR_LINKTYPE, 0, BPF_B,
(bpf_int32)ARCTYPE_ATALK));
}


case DLT_LTALK:
switch (proto) {
case ETHERTYPE_ATALK:
return gen_true(cstate);
default:
return gen_false(cstate);
}


case DLT_FRELAY:




switch (proto) {

case ETHERTYPE_IP:



return gen_cmp(cstate, OR_LINKHDR, 2, BPF_H, (0x03<<8) | 0xcc);

case ETHERTYPE_IPV6:



return gen_cmp(cstate, OR_LINKHDR, 2, BPF_H, (0x03<<8) | 0x8e);

case LLCSAP_ISONS:











b0 = gen_cmp(cstate, OR_LINKHDR, 2, BPF_H, (0x03<<8) | ISO8473_CLNP);
b1 = gen_cmp(cstate, OR_LINKHDR, 2, BPF_H, (0x03<<8) | ISO9542_ESIS);
b2 = gen_cmp(cstate, OR_LINKHDR, 2, BPF_H, (0x03<<8) | ISO10589_ISIS);
gen_or(b1, b2);
gen_or(b0, b2);
return b2;

default:
return gen_false(cstate);
}


case DLT_MFR:
bpf_error(cstate, "Multi-link Frame Relay link-layer type filtering not implemented");

case DLT_JUNIPER_MFR:
case DLT_JUNIPER_MLFR:
case DLT_JUNIPER_MLPPP:
case DLT_JUNIPER_ATM1:
case DLT_JUNIPER_ATM2:
case DLT_JUNIPER_PPPOE:
case DLT_JUNIPER_PPPOE_ATM:
case DLT_JUNIPER_GGSN:
case DLT_JUNIPER_ES:
case DLT_JUNIPER_MONITOR:
case DLT_JUNIPER_SERVICES:
case DLT_JUNIPER_ETHER:
case DLT_JUNIPER_PPP:
case DLT_JUNIPER_FRELAY:
case DLT_JUNIPER_CHDLC:
case DLT_JUNIPER_VP:
case DLT_JUNIPER_ST:
case DLT_JUNIPER_ISM:
case DLT_JUNIPER_VS:
case DLT_JUNIPER_SRX_E2E:
case DLT_JUNIPER_FIBRECHANNEL:
case DLT_JUNIPER_ATM_CEMIC:








return gen_mcmp(cstate, OR_LINKHDR, 0, BPF_W, 0x4d474300, 0xffffff00);

case DLT_BACNET_MS_TP:
return gen_mcmp(cstate, OR_LINKHDR, 0, BPF_W, 0x55FF0000, 0xffff0000);

case DLT_IPNET:
return gen_ipnet_linktype(cstate, proto);

case DLT_LINUX_IRDA:
bpf_error(cstate, "IrDA link-layer type filtering not implemented");

case DLT_DOCSIS:
bpf_error(cstate, "DOCSIS link-layer type filtering not implemented");

case DLT_MTP2:
case DLT_MTP2_WITH_PHDR:
bpf_error(cstate, "MTP2 link-layer type filtering not implemented");

case DLT_ERF:
bpf_error(cstate, "ERF link-layer type filtering not implemented");

case DLT_PFSYNC:
bpf_error(cstate, "PFSYNC link-layer type filtering not implemented");

case DLT_LINUX_LAPD:
bpf_error(cstate, "LAPD link-layer type filtering not implemented");

case DLT_USB_FREEBSD:
case DLT_USB_LINUX:
case DLT_USB_LINUX_MMAPPED:
case DLT_USBPCAP:
bpf_error(cstate, "USB link-layer type filtering not implemented");

case DLT_BLUETOOTH_HCI_H4:
case DLT_BLUETOOTH_HCI_H4_WITH_PHDR:
bpf_error(cstate, "Bluetooth link-layer type filtering not implemented");

case DLT_CAN20B:
case DLT_CAN_SOCKETCAN:
bpf_error(cstate, "CAN link-layer type filtering not implemented");

case DLT_IEEE802_15_4:
case DLT_IEEE802_15_4_LINUX:
case DLT_IEEE802_15_4_NONASK_PHY:
case DLT_IEEE802_15_4_NOFCS:
bpf_error(cstate, "IEEE 802.15.4 link-layer type filtering not implemented");

case DLT_IEEE802_16_MAC_CPS_RADIO:
bpf_error(cstate, "IEEE 802.16 link-layer type filtering not implemented");

case DLT_SITA:
bpf_error(cstate, "SITA link-layer type filtering not implemented");

case DLT_RAIF1:
bpf_error(cstate, "RAIF1 link-layer type filtering not implemented");

case DLT_IPMB_KONTRON:
case DLT_IPMB_LINUX:
bpf_error(cstate, "IPMB link-layer type filtering not implemented");

case DLT_AX25_KISS:
bpf_error(cstate, "AX.25 link-layer type filtering not implemented");

case DLT_NFLOG:




bpf_error(cstate, "NFLOG link-layer type filtering not implemented");

default:






if (cstate->off_linktype.constant_part != OFFSET_NOT_SET) {





return gen_cmp(cstate, OR_LINKTYPE, 0, BPF_H, (bpf_int32)proto);

} else {



description = pcap_datalink_val_to_description_or_dlt(cstate->linktype);
bpf_error(cstate, "%s link-layer type filtering not implemented",
description);

}
}
}








static struct block *
gen_snap(compiler_state_t *cstate, bpf_u_int32 orgcode, bpf_u_int32 ptype)
{
u_char snapblock[8];

snapblock[0] = LLCSAP_SNAP;
snapblock[1] = LLCSAP_SNAP;
snapblock[2] = 0x03;
snapblock[3] = (u_char)(orgcode >> 16);
snapblock[4] = (u_char)(orgcode >> 8);
snapblock[5] = (u_char)(orgcode >> 0);
snapblock[6] = (u_char)(ptype >> 8);
snapblock[7] = (u_char)(ptype >> 0);
return gen_bcmp(cstate, OR_LLC, 0, 8, snapblock);
}




static struct block *
gen_llc_internal(compiler_state_t *cstate)
{
struct block *b0, *b1;

switch (cstate->linktype) {

case DLT_EN10MB:




b0 = gen_cmp_gt(cstate, OR_LINKTYPE, 0, BPF_H, ETHERMTU);
gen_not(b0);





b1 = gen_cmp(cstate, OR_LLC, 0, BPF_H, (bpf_int32)0xFFFF);
gen_not(b1);
gen_and(b0, b1);
return b1;

case DLT_SUNATM:



b0 = gen_atmtype_llc(cstate);
return b0;

case DLT_IEEE802:



return gen_true(cstate);

case DLT_FDDI:



return gen_true(cstate);

case DLT_ATM_RFC1483:








return gen_true(cstate);

case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_PPI:



b0 = gen_check_802_11_data_frame(cstate);
return b0;

default:
bpf_error(cstate, "'llc' not supported for %s",
pcap_datalink_val_to_description_or_dlt(cstate->linktype));

}
}

struct block *
gen_llc(compiler_state_t *cstate)
{




if (setjmp(cstate->top_ctx))
return (NULL);

return gen_llc_internal(cstate);
}

struct block *
gen_llc_i(compiler_state_t *cstate)
{
struct block *b0, *b1;
struct slist *s;





if (setjmp(cstate->top_ctx))
return (NULL);




b0 = gen_llc_internal(cstate);





s = gen_load_a(cstate, OR_LLC, 2, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x01;
b1->stmts = s;
gen_not(b1);
gen_and(b0, b1);
return b1;
}

struct block *
gen_llc_s(compiler_state_t *cstate)
{
struct block *b0, *b1;





if (setjmp(cstate->top_ctx))
return (NULL);




b0 = gen_llc_internal(cstate);





b1 = gen_mcmp(cstate, OR_LLC, 2, BPF_B, LLC_S_FMT, 0x03);
gen_and(b0, b1);
return b1;
}

struct block *
gen_llc_u(compiler_state_t *cstate)
{
struct block *b0, *b1;





if (setjmp(cstate->top_ctx))
return (NULL);




b0 = gen_llc_internal(cstate);





b1 = gen_mcmp(cstate, OR_LLC, 2, BPF_B, LLC_U_FMT, 0x03);
gen_and(b0, b1);
return b1;
}

struct block *
gen_llc_s_subtype(compiler_state_t *cstate, bpf_u_int32 subtype)
{
struct block *b0, *b1;





if (setjmp(cstate->top_ctx))
return (NULL);




b0 = gen_llc_internal(cstate);




b1 = gen_mcmp(cstate, OR_LLC, 2, BPF_B, subtype, LLC_S_CMD_MASK);
gen_and(b0, b1);
return b1;
}

struct block *
gen_llc_u_subtype(compiler_state_t *cstate, bpf_u_int32 subtype)
{
struct block *b0, *b1;





if (setjmp(cstate->top_ctx))
return (NULL);




b0 = gen_llc_internal(cstate);




b1 = gen_mcmp(cstate, OR_LLC, 2, BPF_B, subtype, LLC_U_CMD_MASK);
gen_and(b0, b1);
return b1;
}













static struct block *
gen_llc_linktype(compiler_state_t *cstate, int proto)
{



switch (proto) {

case LLCSAP_IP:
case LLCSAP_ISONS:
case LLCSAP_NETBEUI:





return gen_cmp(cstate, OR_LLC, 0, BPF_H, (bpf_u_int32)
((proto << 8) | proto));

case LLCSAP_IPX:




return gen_cmp(cstate, OR_LLC, 0, BPF_B,
(bpf_int32)LLCSAP_IPX);

case ETHERTYPE_ATALK:









return gen_snap(cstate, 0x080007, ETHERTYPE_ATALK);

default:




if (proto <= ETHERMTU) {




return gen_cmp(cstate, OR_LLC, 0, BPF_B, (bpf_int32)proto);
} else {



















return gen_cmp(cstate, OR_LLC, 6, BPF_H, (bpf_int32)proto);
}
}
}

static struct block *
gen_hostop(compiler_state_t *cstate, bpf_u_int32 addr, bpf_u_int32 mask,
int dir, int proto, u_int src_off, u_int dst_off)
{
struct block *b0, *b1;
u_int offset;

switch (dir) {

case Q_SRC:
offset = src_off;
break;

case Q_DST:
offset = dst_off;
break;

case Q_AND:
b0 = gen_hostop(cstate, addr, mask, Q_SRC, proto, src_off, dst_off);
b1 = gen_hostop(cstate, addr, mask, Q_DST, proto, src_off, dst_off);
gen_and(b0, b1);
return b1;

case Q_DEFAULT:
case Q_OR:
b0 = gen_hostop(cstate, addr, mask, Q_SRC, proto, src_off, dst_off);
b1 = gen_hostop(cstate, addr, mask, Q_DST, proto, src_off, dst_off);
gen_or(b0, b1);
return b1;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_RA:
bpf_error(cstate, "'ra' is not a valid qualifier for addresses other than 802.11 MAC addresses");


case Q_TA:
bpf_error(cstate, "'ta' is not a valid qualifier for addresses other than 802.11 MAC addresses");


default:
abort();

}
b0 = gen_linktype(cstate, proto);
b1 = gen_mcmp(cstate, OR_LINKPL, offset, BPF_W, (bpf_int32)addr, mask);
gen_and(b0, b1);
return b1;
}

#if defined(INET6)
static struct block *
gen_hostop6(compiler_state_t *cstate, struct in6_addr *addr,
struct in6_addr *mask, int dir, int proto, u_int src_off, u_int dst_off)
{
struct block *b0, *b1;
u_int offset;
uint32_t *a, *m;

switch (dir) {

case Q_SRC:
offset = src_off;
break;

case Q_DST:
offset = dst_off;
break;

case Q_AND:
b0 = gen_hostop6(cstate, addr, mask, Q_SRC, proto, src_off, dst_off);
b1 = gen_hostop6(cstate, addr, mask, Q_DST, proto, src_off, dst_off);
gen_and(b0, b1);
return b1;

case Q_DEFAULT:
case Q_OR:
b0 = gen_hostop6(cstate, addr, mask, Q_SRC, proto, src_off, dst_off);
b1 = gen_hostop6(cstate, addr, mask, Q_DST, proto, src_off, dst_off);
gen_or(b0, b1);
return b1;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_RA:
bpf_error(cstate, "'ra' is not a valid qualifier for addresses other than 802.11 MAC addresses");


case Q_TA:
bpf_error(cstate, "'ta' is not a valid qualifier for addresses other than 802.11 MAC addresses");


default:
abort();

}

a = (uint32_t *)addr;
m = (uint32_t *)mask;
b1 = gen_mcmp(cstate, OR_LINKPL, offset + 12, BPF_W, ntohl(a[3]), ntohl(m[3]));
b0 = gen_mcmp(cstate, OR_LINKPL, offset + 8, BPF_W, ntohl(a[2]), ntohl(m[2]));
gen_and(b0, b1);
b0 = gen_mcmp(cstate, OR_LINKPL, offset + 4, BPF_W, ntohl(a[1]), ntohl(m[1]));
gen_and(b0, b1);
b0 = gen_mcmp(cstate, OR_LINKPL, offset + 0, BPF_W, ntohl(a[0]), ntohl(m[0]));
gen_and(b0, b1);
b0 = gen_linktype(cstate, proto);
gen_and(b0, b1);
return b1;
}
#endif

static struct block *
gen_ehostop(compiler_state_t *cstate, const u_char *eaddr, int dir)
{
register struct block *b0, *b1;

switch (dir) {
case Q_SRC:
return gen_bcmp(cstate, OR_LINKHDR, 6, 6, eaddr);

case Q_DST:
return gen_bcmp(cstate, OR_LINKHDR, 0, 6, eaddr);

case Q_AND:
b0 = gen_ehostop(cstate, eaddr, Q_SRC);
b1 = gen_ehostop(cstate, eaddr, Q_DST);
gen_and(b0, b1);
return b1;

case Q_DEFAULT:
case Q_OR:
b0 = gen_ehostop(cstate, eaddr, Q_SRC);
b1 = gen_ehostop(cstate, eaddr, Q_DST);
gen_or(b0, b1);
return b1;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are only supported on 802.11 with 802.11 headers");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are only supported on 802.11 with 802.11 headers");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are only supported on 802.11 with 802.11 headers");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are only supported on 802.11 with 802.11 headers");


case Q_RA:
bpf_error(cstate, "'ra' is only supported on 802.11 with 802.11 headers");


case Q_TA:
bpf_error(cstate, "'ta' is only supported on 802.11 with 802.11 headers");

}
abort();

}




static struct block *
gen_fhostop(compiler_state_t *cstate, const u_char *eaddr, int dir)
{
struct block *b0, *b1;

switch (dir) {
case Q_SRC:
return gen_bcmp(cstate, OR_LINKHDR, 6 + 1 + cstate->pcap_fddipad, 6, eaddr);

case Q_DST:
return gen_bcmp(cstate, OR_LINKHDR, 0 + 1 + cstate->pcap_fddipad, 6, eaddr);

case Q_AND:
b0 = gen_fhostop(cstate, eaddr, Q_SRC);
b1 = gen_fhostop(cstate, eaddr, Q_DST);
gen_and(b0, b1);
return b1;

case Q_DEFAULT:
case Q_OR:
b0 = gen_fhostop(cstate, eaddr, Q_SRC);
b1 = gen_fhostop(cstate, eaddr, Q_DST);
gen_or(b0, b1);
return b1;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are only supported on 802.11");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are only supported on 802.11");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are only supported on 802.11");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are only supported on 802.11");


case Q_RA:
bpf_error(cstate, "'ra' is only supported on 802.11");


case Q_TA:
bpf_error(cstate, "'ta' is only supported on 802.11");

}
abort();

}




static struct block *
gen_thostop(compiler_state_t *cstate, const u_char *eaddr, int dir)
{
register struct block *b0, *b1;

switch (dir) {
case Q_SRC:
return gen_bcmp(cstate, OR_LINKHDR, 8, 6, eaddr);

case Q_DST:
return gen_bcmp(cstate, OR_LINKHDR, 2, 6, eaddr);

case Q_AND:
b0 = gen_thostop(cstate, eaddr, Q_SRC);
b1 = gen_thostop(cstate, eaddr, Q_DST);
gen_and(b0, b1);
return b1;

case Q_DEFAULT:
case Q_OR:
b0 = gen_thostop(cstate, eaddr, Q_SRC);
b1 = gen_thostop(cstate, eaddr, Q_DST);
gen_or(b0, b1);
return b1;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are only supported on 802.11");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are only supported on 802.11");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are only supported on 802.11");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are only supported on 802.11");


case Q_RA:
bpf_error(cstate, "'ra' is only supported on 802.11");


case Q_TA:
bpf_error(cstate, "'ta' is only supported on 802.11");

}
abort();

}





static struct block *
gen_wlanhostop(compiler_state_t *cstate, const u_char *eaddr, int dir)
{
register struct block *b0, *b1, *b2;
register struct slist *s;

#if defined(ENABLE_WLAN_FILTERING_PATCH)






cstate->no_optimize = 1;
#endif

switch (dir) {
case Q_SRC:

























s = gen_load_a(cstate, OR_LINKHDR, 1, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x01;
b1->stmts = s;




b0 = gen_bcmp(cstate, OR_LINKHDR, 24, 6, eaddr);
gen_and(b1, b0);





s = gen_load_a(cstate, OR_LINKHDR, 1, BPF_B);
b2 = new_block(cstate, JMP(BPF_JSET));
b2->s.k = 0x01;
b2->stmts = s;
gen_not(b2);




b1 = gen_bcmp(cstate, OR_LINKHDR, 16, 6, eaddr);
gen_and(b2, b1);






gen_or(b1, b0);





s = gen_load_a(cstate, OR_LINKHDR, 1, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x02;
b1->stmts = s;
gen_and(b1, b0);




s = gen_load_a(cstate, OR_LINKHDR, 1, BPF_B);
b2 = new_block(cstate, JMP(BPF_JSET));
b2->s.k = 0x02;
b2->stmts = s;
gen_not(b2);




b1 = gen_bcmp(cstate, OR_LINKHDR, 10, 6, eaddr);
gen_and(b2, b1);






gen_or(b1, b0);





s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x08;
b1->stmts = s;




gen_and(b1, b0);






s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b2 = new_block(cstate, JMP(BPF_JSET));
b2->s.k = 0x08;
b2->stmts = s;
gen_not(b2);




b1 = gen_bcmp(cstate, OR_LINKHDR, 10, 6, eaddr);
gen_and(b2, b1);






gen_or(b1, b0);









s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x04;
b1->stmts = s;
gen_not(b1);





gen_and(b1, b0);
return b0;

case Q_DST:





















s = gen_load_a(cstate, OR_LINKHDR, 1, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x01;
b1->stmts = s;




b0 = gen_bcmp(cstate, OR_LINKHDR, 16, 6, eaddr);
gen_and(b1, b0);





s = gen_load_a(cstate, OR_LINKHDR, 1, BPF_B);
b2 = new_block(cstate, JMP(BPF_JSET));
b2->s.k = 0x01;
b2->stmts = s;
gen_not(b2);




b1 = gen_bcmp(cstate, OR_LINKHDR, 4, 6, eaddr);
gen_and(b2, b1);





gen_or(b1, b0);





s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x08;
b1->stmts = s;




gen_and(b1, b0);






s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b2 = new_block(cstate, JMP(BPF_JSET));
b2->s.k = 0x08;
b2->stmts = s;
gen_not(b2);




b1 = gen_bcmp(cstate, OR_LINKHDR, 4, 6, eaddr);
gen_and(b2, b1);






gen_or(b1, b0);









s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x04;
b1->stmts = s;
gen_not(b1);





gen_and(b1, b0);
return b0;

case Q_AND:
b0 = gen_wlanhostop(cstate, eaddr, Q_SRC);
b1 = gen_wlanhostop(cstate, eaddr, Q_DST);
gen_and(b0, b1);
return b1;

case Q_DEFAULT:
case Q_OR:
b0 = gen_wlanhostop(cstate, eaddr, Q_SRC);
b1 = gen_wlanhostop(cstate, eaddr, Q_DST);
gen_or(b0, b1);
return b1;




case Q_ADDR1:
return (gen_bcmp(cstate, OR_LINKHDR, 4, 6, eaddr));

case Q_ADDR2:



b0 = gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, IEEE80211_FC0_TYPE_CTL,
IEEE80211_FC0_TYPE_MASK);
gen_not(b0);
b1 = gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, IEEE80211_FC0_SUBTYPE_CTS,
IEEE80211_FC0_SUBTYPE_MASK);
gen_not(b1);
b2 = gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, IEEE80211_FC0_SUBTYPE_ACK,
IEEE80211_FC0_SUBTYPE_MASK);
gen_not(b2);
gen_and(b1, b2);
gen_or(b0, b2);
b1 = gen_bcmp(cstate, OR_LINKHDR, 10, 6, eaddr);
gen_and(b2, b1);
return b1;

case Q_ADDR3:



b0 = gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, IEEE80211_FC0_TYPE_CTL,
IEEE80211_FC0_TYPE_MASK);
gen_not(b0);
b1 = gen_bcmp(cstate, OR_LINKHDR, 16, 6, eaddr);
gen_and(b0, b1);
return b1;

case Q_ADDR4:






b0 = gen_mcmp(cstate, OR_LINKHDR, 1, BPF_B,
IEEE80211_FC1_DIR_DSTODS, IEEE80211_FC1_DIR_MASK);
b1 = gen_bcmp(cstate, OR_LINKHDR, 24, 6, eaddr);
gen_and(b0, b1);
return b1;

case Q_RA:










s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x08;
b1->stmts = s;




b0 = gen_bcmp(cstate, OR_LINKHDR, 4, 6, eaddr);




gen_and(b1, b0);
return (b0);

case Q_TA:








b0 = gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, IEEE80211_FC0_TYPE_CTL,
IEEE80211_FC0_TYPE_MASK);
gen_not(b0);
b1 = gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, IEEE80211_FC0_SUBTYPE_CTS,
IEEE80211_FC0_SUBTYPE_MASK);
gen_not(b1);
b2 = gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, IEEE80211_FC0_SUBTYPE_ACK,
IEEE80211_FC0_SUBTYPE_MASK);
gen_not(b2);
gen_and(b1, b2);
gen_or(b0, b2);






s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x08;
b1->stmts = s;





gen_and(b1, b2);




b1 = gen_bcmp(cstate, OR_LINKHDR, 10, 6, eaddr);
gen_and(b2, b1);
return b1;
}
abort();

}






static struct block *
gen_ipfchostop(compiler_state_t *cstate, const u_char *eaddr, int dir)
{
register struct block *b0, *b1;

switch (dir) {
case Q_SRC:
return gen_bcmp(cstate, OR_LINKHDR, 10, 6, eaddr);

case Q_DST:
return gen_bcmp(cstate, OR_LINKHDR, 2, 6, eaddr);

case Q_AND:
b0 = gen_ipfchostop(cstate, eaddr, Q_SRC);
b1 = gen_ipfchostop(cstate, eaddr, Q_DST);
gen_and(b0, b1);
return b1;

case Q_DEFAULT:
case Q_OR:
b0 = gen_ipfchostop(cstate, eaddr, Q_SRC);
b1 = gen_ipfchostop(cstate, eaddr, Q_DST);
gen_or(b0, b1);
return b1;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are only supported on 802.11");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are only supported on 802.11");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are only supported on 802.11");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are only supported on 802.11");


case Q_RA:
bpf_error(cstate, "'ra' is only supported on 802.11");


case Q_TA:
bpf_error(cstate, "'ta' is only supported on 802.11");

}
abort();

}



















static struct block *
gen_dnhostop(compiler_state_t *cstate, bpf_u_int32 addr, int dir)
{
struct block *b0, *b1, *b2, *tmp;
u_int offset_lh;
u_int offset_sh;

switch (dir) {

case Q_DST:
offset_sh = 1;
offset_lh = 7;
break;

case Q_SRC:
offset_sh = 3;
offset_lh = 15;
break;

case Q_AND:

b0 = gen_dnhostop(cstate, addr, Q_SRC);
b1 = gen_dnhostop(cstate, addr, Q_DST);
gen_and(b0, b1);
return b1;

case Q_DEFAULT:
case Q_OR:

b0 = gen_dnhostop(cstate, addr, Q_SRC);
b1 = gen_dnhostop(cstate, addr, Q_DST);
gen_or(b0, b1);
return b1;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are not valid qualifiers for addresses other than 802.11 MAC addresses");


case Q_RA:
bpf_error(cstate, "'ra' is not a valid qualifier for addresses other than 802.11 MAC addresses");


case Q_TA:
bpf_error(cstate, "'ta' is not a valid qualifier for addresses other than 802.11 MAC addresses");


default:
abort();

}
b0 = gen_linktype(cstate, ETHERTYPE_DN);

tmp = gen_mcmp(cstate, OR_LINKPL, 2, BPF_H,
(bpf_int32)ntohs(0x0681), (bpf_int32)ntohs(0x07FF));
b1 = gen_cmp(cstate, OR_LINKPL, 2 + 1 + offset_lh,
BPF_H, (bpf_int32)ntohs((u_short)addr));
gen_and(tmp, b1);

tmp = gen_mcmp(cstate, OR_LINKPL, 2, BPF_B, (bpf_int32)0x06, (bpf_int32)0x7);
b2 = gen_cmp(cstate, OR_LINKPL, 2 + offset_lh, BPF_H, (bpf_int32)ntohs((u_short)addr));
gen_and(tmp, b2);
gen_or(b2, b1);

tmp = gen_mcmp(cstate, OR_LINKPL, 2, BPF_H,
(bpf_int32)ntohs(0x0281), (bpf_int32)ntohs(0x07FF));
b2 = gen_cmp(cstate, OR_LINKPL, 2 + 1 + offset_sh, BPF_H, (bpf_int32)ntohs((u_short)addr));
gen_and(tmp, b2);
gen_or(b2, b1);

tmp = gen_mcmp(cstate, OR_LINKPL, 2, BPF_B, (bpf_int32)0x02, (bpf_int32)0x7);
b2 = gen_cmp(cstate, OR_LINKPL, 2 + offset_sh, BPF_H, (bpf_int32)ntohs((u_short)addr));
gen_and(tmp, b2);
gen_or(b2, b1);


gen_and(b0, b1);
return b1;
}






static struct block *
gen_mpls_linktype(compiler_state_t *cstate, int proto)
{
struct block *b0, *b1;

switch (proto) {

case Q_IP:

b0 = gen_mcmp(cstate, OR_LINKPL, (u_int)-2, BPF_B, 0x01, 0x01);

b1 = gen_mcmp(cstate, OR_LINKPL, 0, BPF_B, 0x40, 0xf0);
gen_and(b0, b1);
return b1;

case Q_IPV6:

b0 = gen_mcmp(cstate, OR_LINKPL, (u_int)-2, BPF_B, 0x01, 0x01);

b1 = gen_mcmp(cstate, OR_LINKPL, 0, BPF_B, 0x60, 0xf0);
gen_and(b0, b1);
return b1;

default:
abort();
}
}

static struct block *
gen_host(compiler_state_t *cstate, bpf_u_int32 addr, bpf_u_int32 mask,
int proto, int dir, int type)
{
struct block *b0, *b1;
const char *typestr;

if (type == Q_NET)
typestr = "net";
else
typestr = "host";

switch (proto) {

case Q_DEFAULT:
b0 = gen_host(cstate, addr, mask, Q_IP, dir, type);




if (cstate->label_stack_depth == 0) {
b1 = gen_host(cstate, addr, mask, Q_ARP, dir, type);
gen_or(b0, b1);
b0 = gen_host(cstate, addr, mask, Q_RARP, dir, type);
gen_or(b1, b0);
}
return b0;

case Q_LINK:
bpf_error(cstate, "link-layer modifier applied to %s", typestr);

case Q_IP:
return gen_hostop(cstate, addr, mask, dir, ETHERTYPE_IP, 12, 16);

case Q_RARP:
return gen_hostop(cstate, addr, mask, dir, ETHERTYPE_REVARP, 14, 24);

case Q_ARP:
return gen_hostop(cstate, addr, mask, dir, ETHERTYPE_ARP, 14, 24);

case Q_SCTP:
bpf_error(cstate, "'sctp' modifier applied to %s", typestr);

case Q_TCP:
bpf_error(cstate, "'tcp' modifier applied to %s", typestr);

case Q_UDP:
bpf_error(cstate, "'udp' modifier applied to %s", typestr);

case Q_ICMP:
bpf_error(cstate, "'icmp' modifier applied to %s", typestr);

case Q_IGMP:
bpf_error(cstate, "'igmp' modifier applied to %s", typestr);

case Q_IGRP:
bpf_error(cstate, "'igrp' modifier applied to %s", typestr);

case Q_ATALK:
bpf_error(cstate, "AppleTalk host filtering not implemented");

case Q_DECNET:
return gen_dnhostop(cstate, addr, dir);

case Q_LAT:
bpf_error(cstate, "LAT host filtering not implemented");

case Q_SCA:
bpf_error(cstate, "SCA host filtering not implemented");

case Q_MOPRC:
bpf_error(cstate, "MOPRC host filtering not implemented");

case Q_MOPDL:
bpf_error(cstate, "MOPDL host filtering not implemented");

case Q_IPV6:
bpf_error(cstate, "'ip6' modifier applied to ip host");

case Q_ICMPV6:
bpf_error(cstate, "'icmp6' modifier applied to %s", typestr);

case Q_AH:
bpf_error(cstate, "'ah' modifier applied to %s", typestr);

case Q_ESP:
bpf_error(cstate, "'esp' modifier applied to %s", typestr);

case Q_PIM:
bpf_error(cstate, "'pim' modifier applied to %s", typestr);

case Q_VRRP:
bpf_error(cstate, "'vrrp' modifier applied to %s", typestr);

case Q_AARP:
bpf_error(cstate, "AARP host filtering not implemented");

case Q_ISO:
bpf_error(cstate, "ISO host filtering not implemented");

case Q_ESIS:
bpf_error(cstate, "'esis' modifier applied to %s", typestr);

case Q_ISIS:
bpf_error(cstate, "'isis' modifier applied to %s", typestr);

case Q_CLNP:
bpf_error(cstate, "'clnp' modifier applied to %s", typestr);

case Q_STP:
bpf_error(cstate, "'stp' modifier applied to %s", typestr);

case Q_IPX:
bpf_error(cstate, "IPX host filtering not implemented");

case Q_NETBEUI:
bpf_error(cstate, "'netbeui' modifier applied to %s", typestr);

case Q_ISIS_L1:
bpf_error(cstate, "'l1' modifier applied to %s", typestr);

case Q_ISIS_L2:
bpf_error(cstate, "'l2' modifier applied to %s", typestr);

case Q_ISIS_IIH:
bpf_error(cstate, "'iih' modifier applied to %s", typestr);

case Q_ISIS_SNP:
bpf_error(cstate, "'snp' modifier applied to %s", typestr);

case Q_ISIS_CSNP:
bpf_error(cstate, "'csnp' modifier applied to %s", typestr);

case Q_ISIS_PSNP:
bpf_error(cstate, "'psnp' modifier applied to %s", typestr);

case Q_ISIS_LSP:
bpf_error(cstate, "'lsp' modifier applied to %s", typestr);

case Q_RADIO:
bpf_error(cstate, "'radio' modifier applied to %s", typestr);

case Q_CARP:
bpf_error(cstate, "'carp' modifier applied to %s", typestr);

default:
abort();
}

}

#if defined(INET6)
static struct block *
gen_host6(compiler_state_t *cstate, struct in6_addr *addr,
struct in6_addr *mask, int proto, int dir, int type)
{
const char *typestr;

if (type == Q_NET)
typestr = "net";
else
typestr = "host";

switch (proto) {

case Q_DEFAULT:
return gen_host6(cstate, addr, mask, Q_IPV6, dir, type);

case Q_LINK:
bpf_error(cstate, "link-layer modifier applied to ip6 %s", typestr);

case Q_IP:
bpf_error(cstate, "'ip' modifier applied to ip6 %s", typestr);

case Q_RARP:
bpf_error(cstate, "'rarp' modifier applied to ip6 %s", typestr);

case Q_ARP:
bpf_error(cstate, "'arp' modifier applied to ip6 %s", typestr);

case Q_SCTP:
bpf_error(cstate, "'sctp' modifier applied to ip6 %s", typestr);

case Q_TCP:
bpf_error(cstate, "'tcp' modifier applied to ip6 %s", typestr);

case Q_UDP:
bpf_error(cstate, "'udp' modifier applied to ip6 %s", typestr);

case Q_ICMP:
bpf_error(cstate, "'icmp' modifier applied to ip6 %s", typestr);

case Q_IGMP:
bpf_error(cstate, "'igmp' modifier applied to ip6 %s", typestr);

case Q_IGRP:
bpf_error(cstate, "'igrp' modifier applied to ip6 %s", typestr);

case Q_ATALK:
bpf_error(cstate, "AppleTalk modifier applied to ip6 %s", typestr);

case Q_DECNET:
bpf_error(cstate, "'decnet' modifier applied to ip6 %s", typestr);

case Q_LAT:
bpf_error(cstate, "'lat' modifier applied to ip6 %s", typestr);

case Q_SCA:
bpf_error(cstate, "'sca' modifier applied to ip6 %s", typestr);

case Q_MOPRC:
bpf_error(cstate, "'moprc' modifier applied to ip6 %s", typestr);

case Q_MOPDL:
bpf_error(cstate, "'mopdl' modifier applied to ip6 %s", typestr);

case Q_IPV6:
return gen_hostop6(cstate, addr, mask, dir, ETHERTYPE_IPV6, 8, 24);

case Q_ICMPV6:
bpf_error(cstate, "'icmp6' modifier applied to ip6 %s", typestr);

case Q_AH:
bpf_error(cstate, "'ah' modifier applied to ip6 %s", typestr);

case Q_ESP:
bpf_error(cstate, "'esp' modifier applied to ip6 %s", typestr);

case Q_PIM:
bpf_error(cstate, "'pim' modifier applied to ip6 %s", typestr);

case Q_VRRP:
bpf_error(cstate, "'vrrp' modifier applied to ip6 %s", typestr);

case Q_AARP:
bpf_error(cstate, "'aarp' modifier applied to ip6 %s", typestr);

case Q_ISO:
bpf_error(cstate, "'iso' modifier applied to ip6 %s", typestr);

case Q_ESIS:
bpf_error(cstate, "'esis' modifier applied to ip6 %s", typestr);

case Q_ISIS:
bpf_error(cstate, "'isis' modifier applied to ip6 %s", typestr);

case Q_CLNP:
bpf_error(cstate, "'clnp' modifier applied to ip6 %s", typestr);

case Q_STP:
bpf_error(cstate, "'stp' modifier applied to ip6 %s", typestr);

case Q_IPX:
bpf_error(cstate, "'ipx' modifier applied to ip6 %s", typestr);

case Q_NETBEUI:
bpf_error(cstate, "'netbeui' modifier applied to ip6 %s", typestr);

case Q_ISIS_L1:
bpf_error(cstate, "'l1' modifier applied to ip6 %s", typestr);

case Q_ISIS_L2:
bpf_error(cstate, "'l2' modifier applied to ip6 %s", typestr);

case Q_ISIS_IIH:
bpf_error(cstate, "'iih' modifier applied to ip6 %s", typestr);

case Q_ISIS_SNP:
bpf_error(cstate, "'snp' modifier applied to ip6 %s", typestr);

case Q_ISIS_CSNP:
bpf_error(cstate, "'csnp' modifier applied to ip6 %s", typestr);

case Q_ISIS_PSNP:
bpf_error(cstate, "'psnp' modifier applied to ip6 %s", typestr);

case Q_ISIS_LSP:
bpf_error(cstate, "'lsp' modifier applied to ip6 %s", typestr);

case Q_RADIO:
bpf_error(cstate, "'radio' modifier applied to ip6 %s", typestr);

case Q_CARP:
bpf_error(cstate, "'carp' modifier applied to ip6 %s", typestr);

default:
abort();
}

}
#endif

#if !defined(INET6)
static struct block *
gen_gateway(compiler_state_t *cstate, const u_char *eaddr,
struct addrinfo *alist, int proto, int dir)
{
struct block *b0, *b1, *tmp;
struct addrinfo *ai;
struct sockaddr_in *sin;

if (dir != 0)
bpf_error(cstate, "direction applied to 'gateway'");

switch (proto) {
case Q_DEFAULT:
case Q_IP:
case Q_ARP:
case Q_RARP:
switch (cstate->linktype) {
case DLT_EN10MB:
case DLT_NETANALYZER:
case DLT_NETANALYZER_TRANSPARENT:
b1 = gen_prevlinkhdr_check(cstate);
b0 = gen_ehostop(cstate, eaddr, Q_OR);
if (b1 != NULL)
gen_and(b1, b0);
break;
case DLT_FDDI:
b0 = gen_fhostop(cstate, eaddr, Q_OR);
break;
case DLT_IEEE802:
b0 = gen_thostop(cstate, eaddr, Q_OR);
break;
case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
case DLT_PPI:
b0 = gen_wlanhostop(cstate, eaddr, Q_OR);
break;
case DLT_SUNATM:





bpf_error(cstate,
"'gateway' supported only on ethernet/FDDI/token ring/802.11/ATM LANE/Fibre Channel");
break;
case DLT_IP_OVER_FC:
b0 = gen_ipfchostop(cstate, eaddr, Q_OR);
break;
default:
bpf_error(cstate,
"'gateway' supported only on ethernet/FDDI/token ring/802.11/ATM LANE/Fibre Channel");
}
b1 = NULL;
for (ai = alist; ai != NULL; ai = ai->ai_next) {



if (ai->ai_addr != NULL) {



if (ai->ai_addr->sa_family == AF_INET) {



sin = (struct sockaddr_in *)ai->ai_addr;
tmp = gen_host(cstate,
ntohl(sin->sin_addr.s_addr),
0xffffffff, proto, Q_OR, Q_HOST);



if (b1 == NULL) {



b1 = tmp;
} else {





gen_or(b1, tmp);
b1 = tmp;
}
}
}
}
if (b1 == NULL) {



return (NULL);
}
gen_not(b1);
gen_and(b0, b1);
return b1;
}
bpf_error(cstate, "illegal modifier of 'gateway'");

}
#endif

static struct block *
gen_proto_abbrev_internal(compiler_state_t *cstate, int proto)
{
struct block *b0;
struct block *b1;

switch (proto) {

case Q_SCTP:
b1 = gen_proto(cstate, IPPROTO_SCTP, Q_IP, Q_DEFAULT);
b0 = gen_proto(cstate, IPPROTO_SCTP, Q_IPV6, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_TCP:
b1 = gen_proto(cstate, IPPROTO_TCP, Q_IP, Q_DEFAULT);
b0 = gen_proto(cstate, IPPROTO_TCP, Q_IPV6, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_UDP:
b1 = gen_proto(cstate, IPPROTO_UDP, Q_IP, Q_DEFAULT);
b0 = gen_proto(cstate, IPPROTO_UDP, Q_IPV6, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_ICMP:
b1 = gen_proto(cstate, IPPROTO_ICMP, Q_IP, Q_DEFAULT);
break;

#if !defined(IPPROTO_IGMP)
#define IPPROTO_IGMP 2
#endif

case Q_IGMP:
b1 = gen_proto(cstate, IPPROTO_IGMP, Q_IP, Q_DEFAULT);
break;

#if !defined(IPPROTO_IGRP)
#define IPPROTO_IGRP 9
#endif
case Q_IGRP:
b1 = gen_proto(cstate, IPPROTO_IGRP, Q_IP, Q_DEFAULT);
break;

#if !defined(IPPROTO_PIM)
#define IPPROTO_PIM 103
#endif

case Q_PIM:
b1 = gen_proto(cstate, IPPROTO_PIM, Q_IP, Q_DEFAULT);
b0 = gen_proto(cstate, IPPROTO_PIM, Q_IPV6, Q_DEFAULT);
gen_or(b0, b1);
break;

#if !defined(IPPROTO_VRRP)
#define IPPROTO_VRRP 112
#endif

case Q_VRRP:
b1 = gen_proto(cstate, IPPROTO_VRRP, Q_IP, Q_DEFAULT);
break;

#if !defined(IPPROTO_CARP)
#define IPPROTO_CARP 112
#endif

case Q_CARP:
b1 = gen_proto(cstate, IPPROTO_CARP, Q_IP, Q_DEFAULT);
break;

case Q_IP:
b1 = gen_linktype(cstate, ETHERTYPE_IP);
break;

case Q_ARP:
b1 = gen_linktype(cstate, ETHERTYPE_ARP);
break;

case Q_RARP:
b1 = gen_linktype(cstate, ETHERTYPE_REVARP);
break;

case Q_LINK:
bpf_error(cstate, "link layer applied in wrong context");

case Q_ATALK:
b1 = gen_linktype(cstate, ETHERTYPE_ATALK);
break;

case Q_AARP:
b1 = gen_linktype(cstate, ETHERTYPE_AARP);
break;

case Q_DECNET:
b1 = gen_linktype(cstate, ETHERTYPE_DN);
break;

case Q_SCA:
b1 = gen_linktype(cstate, ETHERTYPE_SCA);
break;

case Q_LAT:
b1 = gen_linktype(cstate, ETHERTYPE_LAT);
break;

case Q_MOPDL:
b1 = gen_linktype(cstate, ETHERTYPE_MOPDL);
break;

case Q_MOPRC:
b1 = gen_linktype(cstate, ETHERTYPE_MOPRC);
break;

case Q_IPV6:
b1 = gen_linktype(cstate, ETHERTYPE_IPV6);
break;

#if !defined(IPPROTO_ICMPV6)
#define IPPROTO_ICMPV6 58
#endif
case Q_ICMPV6:
b1 = gen_proto(cstate, IPPROTO_ICMPV6, Q_IPV6, Q_DEFAULT);
break;

#if !defined(IPPROTO_AH)
#define IPPROTO_AH 51
#endif
case Q_AH:
b1 = gen_proto(cstate, IPPROTO_AH, Q_IP, Q_DEFAULT);
b0 = gen_proto(cstate, IPPROTO_AH, Q_IPV6, Q_DEFAULT);
gen_or(b0, b1);
break;

#if !defined(IPPROTO_ESP)
#define IPPROTO_ESP 50
#endif
case Q_ESP:
b1 = gen_proto(cstate, IPPROTO_ESP, Q_IP, Q_DEFAULT);
b0 = gen_proto(cstate, IPPROTO_ESP, Q_IPV6, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_ISO:
b1 = gen_linktype(cstate, LLCSAP_ISONS);
break;

case Q_ESIS:
b1 = gen_proto(cstate, ISO9542_ESIS, Q_ISO, Q_DEFAULT);
break;

case Q_ISIS:
b1 = gen_proto(cstate, ISO10589_ISIS, Q_ISO, Q_DEFAULT);
break;

case Q_ISIS_L1:
b0 = gen_proto(cstate, ISIS_L1_LAN_IIH, Q_ISIS, Q_DEFAULT);
b1 = gen_proto(cstate, ISIS_PTP_IIH, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
b0 = gen_proto(cstate, ISIS_L1_LSP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
b0 = gen_proto(cstate, ISIS_L1_CSNP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
b0 = gen_proto(cstate, ISIS_L1_PSNP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_ISIS_L2:
b0 = gen_proto(cstate, ISIS_L2_LAN_IIH, Q_ISIS, Q_DEFAULT);
b1 = gen_proto(cstate, ISIS_PTP_IIH, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
b0 = gen_proto(cstate, ISIS_L2_LSP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
b0 = gen_proto(cstate, ISIS_L2_CSNP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
b0 = gen_proto(cstate, ISIS_L2_PSNP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_ISIS_IIH:
b0 = gen_proto(cstate, ISIS_L1_LAN_IIH, Q_ISIS, Q_DEFAULT);
b1 = gen_proto(cstate, ISIS_L2_LAN_IIH, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
b0 = gen_proto(cstate, ISIS_PTP_IIH, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_ISIS_LSP:
b0 = gen_proto(cstate, ISIS_L1_LSP, Q_ISIS, Q_DEFAULT);
b1 = gen_proto(cstate, ISIS_L2_LSP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_ISIS_SNP:
b0 = gen_proto(cstate, ISIS_L1_CSNP, Q_ISIS, Q_DEFAULT);
b1 = gen_proto(cstate, ISIS_L2_CSNP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
b0 = gen_proto(cstate, ISIS_L1_PSNP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
b0 = gen_proto(cstate, ISIS_L2_PSNP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_ISIS_CSNP:
b0 = gen_proto(cstate, ISIS_L1_CSNP, Q_ISIS, Q_DEFAULT);
b1 = gen_proto(cstate, ISIS_L2_CSNP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_ISIS_PSNP:
b0 = gen_proto(cstate, ISIS_L1_PSNP, Q_ISIS, Q_DEFAULT);
b1 = gen_proto(cstate, ISIS_L2_PSNP, Q_ISIS, Q_DEFAULT);
gen_or(b0, b1);
break;

case Q_CLNP:
b1 = gen_proto(cstate, ISO8473_CLNP, Q_ISO, Q_DEFAULT);
break;

case Q_STP:
b1 = gen_linktype(cstate, LLCSAP_8021D);
break;

case Q_IPX:
b1 = gen_linktype(cstate, LLCSAP_IPX);
break;

case Q_NETBEUI:
b1 = gen_linktype(cstate, LLCSAP_NETBEUI);
break;

case Q_RADIO:
bpf_error(cstate, "'radio' is not a valid protocol type");

default:
abort();
}
return b1;
}

struct block *
gen_proto_abbrev(compiler_state_t *cstate, int proto)
{




if (setjmp(cstate->top_ctx))
return (NULL);

return gen_proto_abbrev_internal(cstate, proto);
}

static struct block *
gen_ipfrag(compiler_state_t *cstate)
{
struct slist *s;
struct block *b;


s = gen_load_a(cstate, OR_LINKPL, 6, BPF_H);
b = new_block(cstate, JMP(BPF_JSET));
b->s.k = 0x1fff;
b->stmts = s;
gen_not(b);

return b;
}










static struct block *
gen_portatom(compiler_state_t *cstate, int off, bpf_int32 v)
{
return gen_cmp(cstate, OR_TRAN_IPV4, off, BPF_H, v);
}

static struct block *
gen_portatom6(compiler_state_t *cstate, int off, bpf_int32 v)
{
return gen_cmp(cstate, OR_TRAN_IPV6, off, BPF_H, v);
}

struct block *
gen_portop(compiler_state_t *cstate, int port, int proto, int dir)
{
struct block *b0, *b1, *tmp;


tmp = gen_cmp(cstate, OR_LINKPL, 9, BPF_B, (bpf_int32)proto);
b0 = gen_ipfrag(cstate);
gen_and(tmp, b0);

switch (dir) {
case Q_SRC:
b1 = gen_portatom(cstate, 0, (bpf_int32)port);
break;

case Q_DST:
b1 = gen_portatom(cstate, 2, (bpf_int32)port);
break;

case Q_AND:
tmp = gen_portatom(cstate, 0, (bpf_int32)port);
b1 = gen_portatom(cstate, 2, (bpf_int32)port);
gen_and(tmp, b1);
break;

case Q_DEFAULT:
case Q_OR:
tmp = gen_portatom(cstate, 0, (bpf_int32)port);
b1 = gen_portatom(cstate, 2, (bpf_int32)port);
gen_or(tmp, b1);
break;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are not valid qualifiers for ports");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are not valid qualifiers for ports");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are not valid qualifiers for ports");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are not valid qualifiers for ports");


case Q_RA:
bpf_error(cstate, "'ra' is not a valid qualifier for ports");


case Q_TA:
bpf_error(cstate, "'ta' is not a valid qualifier for ports");


default:
abort();

}
gen_and(b0, b1);

return b1;
}

static struct block *
gen_port(compiler_state_t *cstate, int port, int ip_proto, int dir)
{
struct block *b0, *b1, *tmp;


















b0 = gen_linktype(cstate, ETHERTYPE_IP);

switch (ip_proto) {
case IPPROTO_UDP:
case IPPROTO_TCP:
case IPPROTO_SCTP:
b1 = gen_portop(cstate, port, ip_proto, dir);
break;

case PROTO_UNDEF:
tmp = gen_portop(cstate, port, IPPROTO_TCP, dir);
b1 = gen_portop(cstate, port, IPPROTO_UDP, dir);
gen_or(tmp, b1);
tmp = gen_portop(cstate, port, IPPROTO_SCTP, dir);
gen_or(tmp, b1);
break;

default:
abort();
}
gen_and(b0, b1);
return b1;
}

struct block *
gen_portop6(compiler_state_t *cstate, int port, int proto, int dir)
{
struct block *b0, *b1, *tmp;



b0 = gen_cmp(cstate, OR_LINKPL, 6, BPF_B, (bpf_int32)proto);

switch (dir) {
case Q_SRC:
b1 = gen_portatom6(cstate, 0, (bpf_int32)port);
break;

case Q_DST:
b1 = gen_portatom6(cstate, 2, (bpf_int32)port);
break;

case Q_AND:
tmp = gen_portatom6(cstate, 0, (bpf_int32)port);
b1 = gen_portatom6(cstate, 2, (bpf_int32)port);
gen_and(tmp, b1);
break;

case Q_DEFAULT:
case Q_OR:
tmp = gen_portatom6(cstate, 0, (bpf_int32)port);
b1 = gen_portatom6(cstate, 2, (bpf_int32)port);
gen_or(tmp, b1);
break;

default:
abort();
}
gen_and(b0, b1);

return b1;
}

static struct block *
gen_port6(compiler_state_t *cstate, int port, int ip_proto, int dir)
{
struct block *b0, *b1, *tmp;


b0 = gen_linktype(cstate, ETHERTYPE_IPV6);

switch (ip_proto) {
case IPPROTO_UDP:
case IPPROTO_TCP:
case IPPROTO_SCTP:
b1 = gen_portop6(cstate, port, ip_proto, dir);
break;

case PROTO_UNDEF:
tmp = gen_portop6(cstate, port, IPPROTO_TCP, dir);
b1 = gen_portop6(cstate, port, IPPROTO_UDP, dir);
gen_or(tmp, b1);
tmp = gen_portop6(cstate, port, IPPROTO_SCTP, dir);
gen_or(tmp, b1);
break;

default:
abort();
}
gen_and(b0, b1);
return b1;
}


static struct block *
gen_portrangeatom(compiler_state_t *cstate, int off, bpf_int32 v1,
bpf_int32 v2)
{
struct block *b1, *b2;

if (v1 > v2) {



bpf_int32 vtemp;

vtemp = v1;
v1 = v2;
v2 = vtemp;
}

b1 = gen_cmp_ge(cstate, OR_TRAN_IPV4, off, BPF_H, v1);
b2 = gen_cmp_le(cstate, OR_TRAN_IPV4, off, BPF_H, v2);

gen_and(b1, b2);

return b2;
}

struct block *
gen_portrangeop(compiler_state_t *cstate, int port1, int port2, int proto,
int dir)
{
struct block *b0, *b1, *tmp;


tmp = gen_cmp(cstate, OR_LINKPL, 9, BPF_B, (bpf_int32)proto);
b0 = gen_ipfrag(cstate);
gen_and(tmp, b0);

switch (dir) {
case Q_SRC:
b1 = gen_portrangeatom(cstate, 0, (bpf_int32)port1, (bpf_int32)port2);
break;

case Q_DST:
b1 = gen_portrangeatom(cstate, 2, (bpf_int32)port1, (bpf_int32)port2);
break;

case Q_AND:
tmp = gen_portrangeatom(cstate, 0, (bpf_int32)port1, (bpf_int32)port2);
b1 = gen_portrangeatom(cstate, 2, (bpf_int32)port1, (bpf_int32)port2);
gen_and(tmp, b1);
break;

case Q_DEFAULT:
case Q_OR:
tmp = gen_portrangeatom(cstate, 0, (bpf_int32)port1, (bpf_int32)port2);
b1 = gen_portrangeatom(cstate, 2, (bpf_int32)port1, (bpf_int32)port2);
gen_or(tmp, b1);
break;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are not valid qualifiers for port ranges");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are not valid qualifiers for port ranges");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are not valid qualifiers for port ranges");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are not valid qualifiers for port ranges");


case Q_RA:
bpf_error(cstate, "'ra' is not a valid qualifier for port ranges");


case Q_TA:
bpf_error(cstate, "'ta' is not a valid qualifier for port ranges");


default:
abort();

}
gen_and(b0, b1);

return b1;
}

static struct block *
gen_portrange(compiler_state_t *cstate, int port1, int port2, int ip_proto,
int dir)
{
struct block *b0, *b1, *tmp;


b0 = gen_linktype(cstate, ETHERTYPE_IP);

switch (ip_proto) {
case IPPROTO_UDP:
case IPPROTO_TCP:
case IPPROTO_SCTP:
b1 = gen_portrangeop(cstate, port1, port2, ip_proto, dir);
break;

case PROTO_UNDEF:
tmp = gen_portrangeop(cstate, port1, port2, IPPROTO_TCP, dir);
b1 = gen_portrangeop(cstate, port1, port2, IPPROTO_UDP, dir);
gen_or(tmp, b1);
tmp = gen_portrangeop(cstate, port1, port2, IPPROTO_SCTP, dir);
gen_or(tmp, b1);
break;

default:
abort();
}
gen_and(b0, b1);
return b1;
}

static struct block *
gen_portrangeatom6(compiler_state_t *cstate, int off, bpf_int32 v1,
bpf_int32 v2)
{
struct block *b1, *b2;

if (v1 > v2) {



bpf_int32 vtemp;

vtemp = v1;
v1 = v2;
v2 = vtemp;
}

b1 = gen_cmp_ge(cstate, OR_TRAN_IPV6, off, BPF_H, v1);
b2 = gen_cmp_le(cstate, OR_TRAN_IPV6, off, BPF_H, v2);

gen_and(b1, b2);

return b2;
}

struct block *
gen_portrangeop6(compiler_state_t *cstate, int port1, int port2, int proto,
int dir)
{
struct block *b0, *b1, *tmp;



b0 = gen_cmp(cstate, OR_LINKPL, 6, BPF_B, (bpf_int32)proto);

switch (dir) {
case Q_SRC:
b1 = gen_portrangeatom6(cstate, 0, (bpf_int32)port1, (bpf_int32)port2);
break;

case Q_DST:
b1 = gen_portrangeatom6(cstate, 2, (bpf_int32)port1, (bpf_int32)port2);
break;

case Q_AND:
tmp = gen_portrangeatom6(cstate, 0, (bpf_int32)port1, (bpf_int32)port2);
b1 = gen_portrangeatom6(cstate, 2, (bpf_int32)port1, (bpf_int32)port2);
gen_and(tmp, b1);
break;

case Q_DEFAULT:
case Q_OR:
tmp = gen_portrangeatom6(cstate, 0, (bpf_int32)port1, (bpf_int32)port2);
b1 = gen_portrangeatom6(cstate, 2, (bpf_int32)port1, (bpf_int32)port2);
gen_or(tmp, b1);
break;

default:
abort();
}
gen_and(b0, b1);

return b1;
}

static struct block *
gen_portrange6(compiler_state_t *cstate, int port1, int port2, int ip_proto,
int dir)
{
struct block *b0, *b1, *tmp;


b0 = gen_linktype(cstate, ETHERTYPE_IPV6);

switch (ip_proto) {
case IPPROTO_UDP:
case IPPROTO_TCP:
case IPPROTO_SCTP:
b1 = gen_portrangeop6(cstate, port1, port2, ip_proto, dir);
break;

case PROTO_UNDEF:
tmp = gen_portrangeop6(cstate, port1, port2, IPPROTO_TCP, dir);
b1 = gen_portrangeop6(cstate, port1, port2, IPPROTO_UDP, dir);
gen_or(tmp, b1);
tmp = gen_portrangeop6(cstate, port1, port2, IPPROTO_SCTP, dir);
gen_or(tmp, b1);
break;

default:
abort();
}
gen_and(b0, b1);
return b1;
}

static int
lookup_proto(compiler_state_t *cstate, const char *name, int proto)
{
register int v;

switch (proto) {

case Q_DEFAULT:
case Q_IP:
case Q_IPV6:
v = pcap_nametoproto(name);
if (v == PROTO_UNDEF)
bpf_error(cstate, "unknown ip proto '%s'", name);
break;

case Q_LINK:

v = pcap_nametoeproto(name);
if (v == PROTO_UNDEF) {
v = pcap_nametollc(name);
if (v == PROTO_UNDEF)
bpf_error(cstate, "unknown ether proto '%s'", name);
}
break;

case Q_ISO:
if (strcmp(name, "esis") == 0)
v = ISO9542_ESIS;
else if (strcmp(name, "isis") == 0)
v = ISO10589_ISIS;
else if (strcmp(name, "clnp") == 0)
v = ISO8473_CLNP;
else
bpf_error(cstate, "unknown osi proto '%s'", name);
break;

default:
v = PROTO_UNDEF;
break;
}
return v;
}

#if 0
struct stmt *
gen_joinsp(struct stmt **s, int n)
{
return NULL;
}
#endif

static struct block *
gen_protochain(compiler_state_t *cstate, int v, int proto, int dir)
{
#if defined(NO_PROTOCHAIN)
return gen_proto(cstate, v, proto, dir);
#else
struct block *b0, *b;
struct slist *s[100];
int fix2, fix3, fix4, fix5;
int ahcheck, again, end;
int i, max;
int reg2 = alloc_reg(cstate);

memset(s, 0, sizeof(s));
fix3 = fix4 = fix5 = 0;

switch (proto) {
case Q_IP:
case Q_IPV6:
break;
case Q_DEFAULT:
b0 = gen_protochain(cstate, v, Q_IP, dir);
b = gen_protochain(cstate, v, Q_IPV6, dir);
gen_or(b0, b);
return b;
default:
bpf_error(cstate, "bad protocol applied for 'protochain'");

}












if (cstate->off_linkpl.is_variable)
bpf_error(cstate, "'protochain' not supported with variable length headers");

cstate->no_optimize = 1;






i = 0;
s[i] = new_stmt(cstate, 0);
i++;

switch (proto) {
case Q_IP:
b0 = gen_linktype(cstate, ETHERTYPE_IP);


s[i] = new_stmt(cstate, BPF_LD|BPF_ABS|BPF_B);
s[i]->s.k = cstate->off_linkpl.constant_part + cstate->off_nl + 9;
i++;

s[i] = new_stmt(cstate, BPF_LDX|BPF_MSH|BPF_B);
s[i]->s.k = cstate->off_linkpl.constant_part + cstate->off_nl;
i++;
break;

case Q_IPV6:
b0 = gen_linktype(cstate, ETHERTYPE_IPV6);


s[i] = new_stmt(cstate, BPF_LD|BPF_ABS|BPF_B);
s[i]->s.k = cstate->off_linkpl.constant_part + cstate->off_nl + 6;
i++;

s[i] = new_stmt(cstate, BPF_LDX|BPF_IMM);
s[i]->s.k = 40;
i++;
break;

default:
bpf_error(cstate, "unsupported proto to gen_protochain");

}


again = i;
s[i] = new_stmt(cstate, BPF_JMP|BPF_JEQ|BPF_K);
s[i]->s.k = v;
s[i]->s.jt = NULL;
s[i]->s.jf = NULL;
fix5 = i;
i++;

#if !defined(IPPROTO_NONE)
#define IPPROTO_NONE 59
#endif

s[i] = new_stmt(cstate, BPF_JMP|BPF_JEQ|BPF_K);
s[i]->s.jt = NULL;
s[i]->s.jf = NULL;
s[i]->s.k = IPPROTO_NONE;
s[fix5]->s.jf = s[i];
fix2 = i;
i++;

if (proto == Q_IPV6) {
int v6start, v6end, v6advance, j;

v6start = i;

s[i] = new_stmt(cstate, BPF_JMP|BPF_JEQ|BPF_K);
s[i]->s.jt = NULL;
s[i]->s.jf = NULL;
s[i]->s.k = IPPROTO_HOPOPTS;
s[fix2]->s.jf = s[i];
i++;

s[i - 1]->s.jf = s[i] = new_stmt(cstate, BPF_JMP|BPF_JEQ|BPF_K);
s[i]->s.jt = NULL;
s[i]->s.jf = NULL;
s[i]->s.k = IPPROTO_DSTOPTS;
i++;

s[i - 1]->s.jf = s[i] = new_stmt(cstate, BPF_JMP|BPF_JEQ|BPF_K);
s[i]->s.jt = NULL;
s[i]->s.jf = NULL;
s[i]->s.k = IPPROTO_ROUTING;
i++;

s[i - 1]->s.jf = s[i] = new_stmt(cstate, BPF_JMP|BPF_JEQ|BPF_K);
s[i]->s.jt = NULL;
s[i]->s.jf = NULL;
s[i]->s.k = IPPROTO_FRAGMENT;
fix3 = i;
v6end = i;
i++;


v6advance = i;







s[i] = new_stmt(cstate, BPF_LD|BPF_IND|BPF_B);
s[i]->s.k = cstate->off_linkpl.constant_part + cstate->off_nl;
i++;

s[i] = new_stmt(cstate, BPF_ST);
s[i]->s.k = reg2;
i++;

s[i] = new_stmt(cstate, BPF_LD|BPF_IND|BPF_B);
s[i]->s.k = cstate->off_linkpl.constant_part + cstate->off_nl + 1;
i++;

s[i] = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s[i]->s.k = 1;
i++;

s[i] = new_stmt(cstate, BPF_ALU|BPF_MUL|BPF_K);
s[i]->s.k = 8;
i++;

s[i] = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_X);
s[i]->s.k = 0;
i++;

s[i] = new_stmt(cstate, BPF_MISC|BPF_TAX);
i++;

s[i] = new_stmt(cstate, BPF_LD|BPF_MEM);
s[i]->s.k = reg2;
i++;


s[i] = new_stmt(cstate, BPF_JMP|BPF_JA);
s[i]->s.k = again - i - 1;
s[i - 1]->s.jf = s[i];
i++;


for (j = v6start; j <= v6end; j++)
s[j]->s.jt = s[v6advance];
} else {

s[i] = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s[i]->s.k = 0;
s[fix2]->s.jf = s[i];
i++;
}


ahcheck = i;

s[i] = new_stmt(cstate, BPF_JMP|BPF_JEQ|BPF_K);
s[i]->s.jt = NULL;
s[i]->s.jf = NULL;
s[i]->s.k = IPPROTO_AH;
if (fix3)
s[fix3]->s.jf = s[ahcheck];
fix4 = i;
i++;







s[i - 1]->s.jt = s[i] = new_stmt(cstate, BPF_MISC|BPF_TXA);
i++;

s[i] = new_stmt(cstate, BPF_LD|BPF_IND|BPF_B);
s[i]->s.k = cstate->off_linkpl.constant_part + cstate->off_nl;
i++;

s[i] = new_stmt(cstate, BPF_ST);
s[i]->s.k = reg2;
i++;

s[i - 1]->s.jt = s[i] = new_stmt(cstate, BPF_MISC|BPF_TXA);
i++;

s[i] = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s[i]->s.k = 1;
i++;

s[i] = new_stmt(cstate, BPF_MISC|BPF_TAX);
i++;

s[i] = new_stmt(cstate, BPF_LD|BPF_IND|BPF_B);
s[i]->s.k = cstate->off_linkpl.constant_part + cstate->off_nl;
i++;

s[i] = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s[i]->s.k = 2;
i++;

s[i] = new_stmt(cstate, BPF_ALU|BPF_MUL|BPF_K);
s[i]->s.k = 4;
i++;

s[i] = new_stmt(cstate, BPF_MISC|BPF_TAX);
i++;

s[i] = new_stmt(cstate, BPF_LD|BPF_MEM);
s[i]->s.k = reg2;
i++;


s[i] = new_stmt(cstate, BPF_JMP|BPF_JA);
s[i]->s.k = again - i - 1;
i++;


end = i;
s[i] = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s[i]->s.k = 0;
s[fix2]->s.jt = s[end];
s[fix4]->s.jf = s[end];
s[fix5]->s.jt = s[end];
i++;




max = i;
for (i = 0; i < max - 1; i++)
s[i]->next = s[i + 1];
s[max - 1]->next = NULL;




b = new_block(cstate, JMP(BPF_JEQ));
b->stmts = s[1];
b->s.k = v;

free_reg(cstate, reg2);

gen_and(b0, b);
return b;
#endif
}

static struct block *
gen_check_802_11_data_frame(compiler_state_t *cstate)
{
struct slist *s;
struct block *b0, *b1;





s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b0 = new_block(cstate, JMP(BPF_JSET));
b0->s.k = 0x08;
b0->stmts = s;

s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x04;
b1->stmts = s;
gen_not(b1);

gen_and(b1, b0);

return b0;
}










static struct block *
gen_proto(compiler_state_t *cstate, int v, int proto, int dir)
{
struct block *b0, *b1;
#if !defined(CHASE_CHAIN)
struct block *b2;
#endif

if (dir != Q_DEFAULT)
bpf_error(cstate, "direction applied to 'proto'");

switch (proto) {
case Q_DEFAULT:
b0 = gen_proto(cstate, v, Q_IP, dir);
b1 = gen_proto(cstate, v, Q_IPV6, dir);
gen_or(b0, b1);
return b1;

case Q_LINK:
return gen_linktype(cstate, v);

case Q_IP:















b0 = gen_linktype(cstate, ETHERTYPE_IP);
#if !defined(CHASE_CHAIN)
b1 = gen_cmp(cstate, OR_LINKPL, 9, BPF_B, (bpf_int32)v);
#else
b1 = gen_protochain(cstate, v, Q_IP);
#endif
gen_and(b0, b1);
return b1;

case Q_ARP:
bpf_error(cstate, "arp does not encapsulate another protocol");


case Q_RARP:
bpf_error(cstate, "rarp does not encapsulate another protocol");


case Q_SCTP:
bpf_error(cstate, "'sctp proto' is bogus");


case Q_TCP:
bpf_error(cstate, "'tcp proto' is bogus");


case Q_UDP:
bpf_error(cstate, "'udp proto' is bogus");


case Q_ICMP:
bpf_error(cstate, "'icmp proto' is bogus");


case Q_IGMP:
bpf_error(cstate, "'igmp proto' is bogus");


case Q_IGRP:
bpf_error(cstate, "'igrp proto' is bogus");


case Q_ATALK:
bpf_error(cstate, "AppleTalk encapsulation is not specifiable");


case Q_DECNET:
bpf_error(cstate, "DECNET encapsulation is not specifiable");


case Q_LAT:
bpf_error(cstate, "LAT does not encapsulate another protocol");


case Q_SCA:
bpf_error(cstate, "SCA does not encapsulate another protocol");


case Q_MOPRC:
bpf_error(cstate, "MOPRC does not encapsulate another protocol");


case Q_MOPDL:
bpf_error(cstate, "MOPDL does not encapsulate another protocol");


case Q_IPV6:
b0 = gen_linktype(cstate, ETHERTYPE_IPV6);
#if !defined(CHASE_CHAIN)




b2 = gen_cmp(cstate, OR_LINKPL, 6, BPF_B, IPPROTO_FRAGMENT);
b1 = gen_cmp(cstate, OR_LINKPL, 40, BPF_B, (bpf_int32)v);
gen_and(b2, b1);
b2 = gen_cmp(cstate, OR_LINKPL, 6, BPF_B, (bpf_int32)v);
gen_or(b2, b1);
#else
b1 = gen_protochain(cstate, v, Q_IPV6);
#endif
gen_and(b0, b1);
return b1;

case Q_ICMPV6:
bpf_error(cstate, "'icmp6 proto' is bogus");


case Q_AH:
bpf_error(cstate, "'ah proto' is bogus");


case Q_ESP:
bpf_error(cstate, "'ah proto' is bogus");


case Q_PIM:
bpf_error(cstate, "'pim proto' is bogus");


case Q_VRRP:
bpf_error(cstate, "'vrrp proto' is bogus");


case Q_AARP:
bpf_error(cstate, "'aarp proto' is bogus");


case Q_ISO:
switch (cstate->linktype) {

case DLT_FRELAY:


















return gen_cmp(cstate, OR_LINKHDR, 2, BPF_H, (0x03<<8) | v);


case DLT_C_HDLC:




b0 = gen_linktype(cstate, LLCSAP_ISONS<<8 | LLCSAP_ISONS);

b1 = gen_cmp(cstate, OR_LINKPL_NOSNAP, 1, BPF_B, (long)v);
gen_and(b0, b1);
return b1;

default:
b0 = gen_linktype(cstate, LLCSAP_ISONS);
b1 = gen_cmp(cstate, OR_LINKPL_NOSNAP, 0, BPF_B, (long)v);
gen_and(b0, b1);
return b1;
}

case Q_ESIS:
bpf_error(cstate, "'esis proto' is bogus");


case Q_ISIS:
b0 = gen_proto(cstate, ISO10589_ISIS, Q_ISO, Q_DEFAULT);




b1 = gen_cmp(cstate, OR_LINKPL_NOSNAP, 4, BPF_B, (long)v);
gen_and(b0, b1);
return b1;

case Q_CLNP:
bpf_error(cstate, "'clnp proto' is not supported");


case Q_STP:
bpf_error(cstate, "'stp proto' is bogus");


case Q_IPX:
bpf_error(cstate, "'ipx proto' is bogus");


case Q_NETBEUI:
bpf_error(cstate, "'netbeui proto' is bogus");


case Q_ISIS_L1:
bpf_error(cstate, "'l1 proto' is bogus");


case Q_ISIS_L2:
bpf_error(cstate, "'l2 proto' is bogus");


case Q_ISIS_IIH:
bpf_error(cstate, "'iih proto' is bogus");


case Q_ISIS_SNP:
bpf_error(cstate, "'snp proto' is bogus");


case Q_ISIS_CSNP:
bpf_error(cstate, "'csnp proto' is bogus");


case Q_ISIS_PSNP:
bpf_error(cstate, "'psnp proto' is bogus");


case Q_ISIS_LSP:
bpf_error(cstate, "'lsp proto' is bogus");


case Q_RADIO:
bpf_error(cstate, "'radio proto' is bogus");


case Q_CARP:
bpf_error(cstate, "'carp proto' is bogus");


default:
abort();

}

}

struct block *
gen_scode(compiler_state_t *cstate, const char *name, struct qual q)
{
int proto = q.proto;
int dir = q.dir;
int tproto;
u_char *eaddr;
bpf_u_int32 mask, addr;
struct addrinfo *res, *res0;
struct sockaddr_in *sin4;
#if defined(INET6)
int tproto6;
struct sockaddr_in6 *sin6;
struct in6_addr mask128;
#endif
struct block *b, *tmp;
int port, real_proto;
int port1, port2;





if (setjmp(cstate->top_ctx))
return (NULL);

switch (q.addr) {

case Q_NET:
addr = pcap_nametonetaddr(name);
if (addr == 0)
bpf_error(cstate, "unknown network '%s'", name);

mask = 0xffffffff;
while (addr && (addr & 0xff000000) == 0) {
addr <<= 8;
mask <<= 8;
}
return gen_host(cstate, addr, mask, proto, dir, q.addr);

case Q_DEFAULT:
case Q_HOST:
if (proto == Q_LINK) {
switch (cstate->linktype) {

case DLT_EN10MB:
case DLT_NETANALYZER:
case DLT_NETANALYZER_TRANSPARENT:
eaddr = pcap_ether_hostton(name);
if (eaddr == NULL)
bpf_error(cstate,
"unknown ether host '%s'", name);
tmp = gen_prevlinkhdr_check(cstate);
b = gen_ehostop(cstate, eaddr, dir);
if (tmp != NULL)
gen_and(tmp, b);
free(eaddr);
return b;

case DLT_FDDI:
eaddr = pcap_ether_hostton(name);
if (eaddr == NULL)
bpf_error(cstate,
"unknown FDDI host '%s'", name);
b = gen_fhostop(cstate, eaddr, dir);
free(eaddr);
return b;

case DLT_IEEE802:
eaddr = pcap_ether_hostton(name);
if (eaddr == NULL)
bpf_error(cstate,
"unknown token ring host '%s'", name);
b = gen_thostop(cstate, eaddr, dir);
free(eaddr);
return b;

case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
case DLT_PPI:
eaddr = pcap_ether_hostton(name);
if (eaddr == NULL)
bpf_error(cstate,
"unknown 802.11 host '%s'", name);
b = gen_wlanhostop(cstate, eaddr, dir);
free(eaddr);
return b;

case DLT_IP_OVER_FC:
eaddr = pcap_ether_hostton(name);
if (eaddr == NULL)
bpf_error(cstate,
"unknown Fibre Channel host '%s'", name);
b = gen_ipfchostop(cstate, eaddr, dir);
free(eaddr);
return b;
}

bpf_error(cstate, "only ethernet/FDDI/token ring/802.11/ATM LANE/Fibre Channel supports link-level host name");
} else if (proto == Q_DECNET) {
unsigned short dn_addr;

if (!__pcap_nametodnaddr(name, &dn_addr)) {
#if defined(DECNETLIB)
bpf_error(cstate, "unknown decnet host name '%s'\n", name);
#else
bpf_error(cstate, "decnet name support not included, '%s' cannot be translated\n",
name);
#endif
}




return (gen_host(cstate, dn_addr, 0, proto, dir, q.addr));
} else {
#if defined(INET6)
memset(&mask128, 0xff, sizeof(mask128));
#endif
res0 = res = pcap_nametoaddrinfo(name);
if (res == NULL)
bpf_error(cstate, "unknown host '%s'", name);
cstate->ai = res;
b = tmp = NULL;
tproto = proto;
#if defined(INET6)
tproto6 = proto;
#endif
if (cstate->off_linktype.constant_part == OFFSET_NOT_SET &&
tproto == Q_DEFAULT) {
tproto = Q_IP;
#if defined(INET6)
tproto6 = Q_IPV6;
#endif
}
for (res = res0; res; res = res->ai_next) {
switch (res->ai_family) {
case AF_INET:
#if defined(INET6)
if (tproto == Q_IPV6)
continue;
#endif

sin4 = (struct sockaddr_in *)
res->ai_addr;
tmp = gen_host(cstate, ntohl(sin4->sin_addr.s_addr),
0xffffffff, tproto, dir, q.addr);
break;
#if defined(INET6)
case AF_INET6:
if (tproto6 == Q_IP)
continue;

sin6 = (struct sockaddr_in6 *)
res->ai_addr;
tmp = gen_host6(cstate, &sin6->sin6_addr,
&mask128, tproto6, dir, q.addr);
break;
#endif
default:
continue;
}
if (b)
gen_or(b, tmp);
b = tmp;
}
cstate->ai = NULL;
freeaddrinfo(res0);
if (b == NULL) {
bpf_error(cstate, "unknown host '%s'%s", name,
(proto == Q_DEFAULT)
? ""
: " for specified address family");
}
return b;
}

case Q_PORT:
if (proto != Q_DEFAULT &&
proto != Q_UDP && proto != Q_TCP && proto != Q_SCTP)
bpf_error(cstate, "illegal qualifier of 'port'");
if (pcap_nametoport(name, &port, &real_proto) == 0)
bpf_error(cstate, "unknown port '%s'", name);
if (proto == Q_UDP) {
if (real_proto == IPPROTO_TCP)
bpf_error(cstate, "port '%s' is tcp", name);
else if (real_proto == IPPROTO_SCTP)
bpf_error(cstate, "port '%s' is sctp", name);
else

real_proto = IPPROTO_UDP;
}
if (proto == Q_TCP) {
if (real_proto == IPPROTO_UDP)
bpf_error(cstate, "port '%s' is udp", name);

else if (real_proto == IPPROTO_SCTP)
bpf_error(cstate, "port '%s' is sctp", name);
else

real_proto = IPPROTO_TCP;
}
if (proto == Q_SCTP) {
if (real_proto == IPPROTO_UDP)
bpf_error(cstate, "port '%s' is udp", name);

else if (real_proto == IPPROTO_TCP)
bpf_error(cstate, "port '%s' is tcp", name);
else

real_proto = IPPROTO_SCTP;
}
if (port < 0)
bpf_error(cstate, "illegal port number %d < 0", port);
if (port > 65535)
bpf_error(cstate, "illegal port number %d > 65535", port);
b = gen_port(cstate, port, real_proto, dir);
gen_or(gen_port6(cstate, port, real_proto, dir), b);
return b;

case Q_PORTRANGE:
if (proto != Q_DEFAULT &&
proto != Q_UDP && proto != Q_TCP && proto != Q_SCTP)
bpf_error(cstate, "illegal qualifier of 'portrange'");
if (pcap_nametoportrange(name, &port1, &port2, &real_proto) == 0)
bpf_error(cstate, "unknown port in range '%s'", name);
if (proto == Q_UDP) {
if (real_proto == IPPROTO_TCP)
bpf_error(cstate, "port in range '%s' is tcp", name);
else if (real_proto == IPPROTO_SCTP)
bpf_error(cstate, "port in range '%s' is sctp", name);
else

real_proto = IPPROTO_UDP;
}
if (proto == Q_TCP) {
if (real_proto == IPPROTO_UDP)
bpf_error(cstate, "port in range '%s' is udp", name);
else if (real_proto == IPPROTO_SCTP)
bpf_error(cstate, "port in range '%s' is sctp", name);
else

real_proto = IPPROTO_TCP;
}
if (proto == Q_SCTP) {
if (real_proto == IPPROTO_UDP)
bpf_error(cstate, "port in range '%s' is udp", name);
else if (real_proto == IPPROTO_TCP)
bpf_error(cstate, "port in range '%s' is tcp", name);
else

real_proto = IPPROTO_SCTP;
}
if (port1 < 0)
bpf_error(cstate, "illegal port number %d < 0", port1);
if (port1 > 65535)
bpf_error(cstate, "illegal port number %d > 65535", port1);
if (port2 < 0)
bpf_error(cstate, "illegal port number %d < 0", port2);
if (port2 > 65535)
bpf_error(cstate, "illegal port number %d > 65535", port2);

b = gen_portrange(cstate, port1, port2, real_proto, dir);
gen_or(gen_portrange6(cstate, port1, port2, real_proto, dir), b);
return b;

case Q_GATEWAY:
#if !defined(INET6)
eaddr = pcap_ether_hostton(name);
if (eaddr == NULL)
bpf_error(cstate, "unknown ether host: %s", name);

res = pcap_nametoaddrinfo(name);
cstate->ai = res;
if (res == NULL)
bpf_error(cstate, "unknown host '%s'", name);
b = gen_gateway(cstate, eaddr, res, proto, dir);
cstate->ai = NULL;
freeaddrinfo(res);
if (b == NULL)
bpf_error(cstate, "unknown host '%s'", name);
return b;
#else
bpf_error(cstate, "'gateway' not supported in this configuration");
#endif

case Q_PROTO:
real_proto = lookup_proto(cstate, name, proto);
if (real_proto >= 0)
return gen_proto(cstate, real_proto, proto, dir);
else
bpf_error(cstate, "unknown protocol: %s", name);

case Q_PROTOCHAIN:
real_proto = lookup_proto(cstate, name, proto);
if (real_proto >= 0)
return gen_protochain(cstate, real_proto, proto, dir);
else
bpf_error(cstate, "unknown protocol: %s", name);

case Q_UNDEF:
syntax(cstate);

}
abort();

}

struct block *
gen_mcode(compiler_state_t *cstate, const char *s1, const char *s2,
unsigned int masklen, struct qual q)
{
register int nlen, mlen;
bpf_u_int32 n, m;





if (setjmp(cstate->top_ctx))
return (NULL);

nlen = __pcap_atoin(s1, &n);
if (nlen < 0)
bpf_error(cstate, "invalid IPv4 address '%s'", s1);

n <<= 32 - nlen;

if (s2 != NULL) {
mlen = __pcap_atoin(s2, &m);
if (mlen < 0)
bpf_error(cstate, "invalid IPv4 address '%s'", s2);

m <<= 32 - mlen;
if ((n & ~m) != 0)
bpf_error(cstate, "non-network bits set in \"%s mask %s\"",
s1, s2);
} else {

if (masklen > 32)
bpf_error(cstate, "mask length must be <= 32");
if (masklen == 0) {




m = 0;
} else
m = 0xffffffff << (32 - masklen);
if ((n & ~m) != 0)
bpf_error(cstate, "non-network bits set in \"%s/%d\"",
s1, masklen);
}

switch (q.addr) {

case Q_NET:
return gen_host(cstate, n, m, q.proto, q.dir, q.addr);

default:
bpf_error(cstate, "Mask syntax for networks only");

}

}

struct block *
gen_ncode(compiler_state_t *cstate, const char *s, bpf_u_int32 v, struct qual q)
{
bpf_u_int32 mask;
int proto;
int dir;
register int vlen;





if (setjmp(cstate->top_ctx))
return (NULL);

proto = q.proto;
dir = q.dir;
if (s == NULL)
vlen = 32;
else if (q.proto == Q_DECNET) {
vlen = __pcap_atodn(s, &v);
if (vlen == 0)
bpf_error(cstate, "malformed decnet address '%s'", s);
} else {
vlen = __pcap_atoin(s, &v);
if (vlen < 0)
bpf_error(cstate, "invalid IPv4 address '%s'", s);
}

switch (q.addr) {

case Q_DEFAULT:
case Q_HOST:
case Q_NET:
if (proto == Q_DECNET)
return gen_host(cstate, v, 0, proto, dir, q.addr);
else if (proto == Q_LINK) {
bpf_error(cstate, "illegal link layer address");
} else {
mask = 0xffffffff;
if (s == NULL && q.addr == Q_NET) {

while (v && (v & 0xff000000) == 0) {
v <<= 8;
mask <<= 8;
}
} else {

v <<= 32 - vlen;
mask <<= 32 - vlen ;
}
return gen_host(cstate, v, mask, proto, dir, q.addr);
}

case Q_PORT:
if (proto == Q_UDP)
proto = IPPROTO_UDP;
else if (proto == Q_TCP)
proto = IPPROTO_TCP;
else if (proto == Q_SCTP)
proto = IPPROTO_SCTP;
else if (proto == Q_DEFAULT)
proto = PROTO_UNDEF;
else
bpf_error(cstate, "illegal qualifier of 'port'");

if (v > 65535)
bpf_error(cstate, "illegal port number %u > 65535", v);

{
struct block *b;
b = gen_port(cstate, (int)v, proto, dir);
gen_or(gen_port6(cstate, (int)v, proto, dir), b);
return b;
}

case Q_PORTRANGE:
if (proto == Q_UDP)
proto = IPPROTO_UDP;
else if (proto == Q_TCP)
proto = IPPROTO_TCP;
else if (proto == Q_SCTP)
proto = IPPROTO_SCTP;
else if (proto == Q_DEFAULT)
proto = PROTO_UNDEF;
else
bpf_error(cstate, "illegal qualifier of 'portrange'");

if (v > 65535)
bpf_error(cstate, "illegal port number %u > 65535", v);

{
struct block *b;
b = gen_portrange(cstate, (int)v, (int)v, proto, dir);
gen_or(gen_portrange6(cstate, (int)v, (int)v, proto, dir), b);
return b;
}

case Q_GATEWAY:
bpf_error(cstate, "'gateway' requires a name");


case Q_PROTO:
return gen_proto(cstate, (int)v, proto, dir);

case Q_PROTOCHAIN:
return gen_protochain(cstate, (int)v, proto, dir);

case Q_UNDEF:
syntax(cstate);


default:
abort();

}

}

#if defined(INET6)
struct block *
gen_mcode6(compiler_state_t *cstate, const char *s1, const char *s2,
unsigned int masklen, struct qual q)
{
struct addrinfo *res;
struct in6_addr *addr;
struct in6_addr mask;
struct block *b;
uint32_t *a, *m;





if (setjmp(cstate->top_ctx))
return (NULL);

if (s2)
bpf_error(cstate, "no mask %s supported", s2);

res = pcap_nametoaddrinfo(s1);
if (!res)
bpf_error(cstate, "invalid ip6 address %s", s1);
cstate->ai = res;
if (res->ai_next)
bpf_error(cstate, "%s resolved to multiple address", s1);
addr = &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;

if (sizeof(mask) * 8 < masklen)
bpf_error(cstate, "mask length must be <= %u", (unsigned int)(sizeof(mask) * 8));
memset(&mask, 0, sizeof(mask));
memset(&mask, 0xff, masklen / 8);
if (masklen % 8) {
mask.s6_addr[masklen / 8] =
(0xff << (8 - masklen % 8)) & 0xff;
}

a = (uint32_t *)addr;
m = (uint32_t *)&mask;
if ((a[0] & ~m[0]) || (a[1] & ~m[1])
|| (a[2] & ~m[2]) || (a[3] & ~m[3])) {
bpf_error(cstate, "non-network bits set in \"%s/%d\"", s1, masklen);
}

switch (q.addr) {

case Q_DEFAULT:
case Q_HOST:
if (masklen != 128)
bpf_error(cstate, "Mask syntax for networks only");


case Q_NET:
b = gen_host6(cstate, addr, &mask, q.proto, q.dir, q.addr);
cstate->ai = NULL;
freeaddrinfo(res);
return b;

default:
bpf_error(cstate, "invalid qualifier against IPv6 address");

}
}
#endif

struct block *
gen_ecode(compiler_state_t *cstate, const char *s, struct qual q)
{
struct block *b, *tmp;





if (setjmp(cstate->top_ctx))
return (NULL);

if ((q.addr == Q_HOST || q.addr == Q_DEFAULT) && q.proto == Q_LINK) {
cstate->e = pcap_ether_aton(s);
if (cstate->e == NULL)
bpf_error(cstate, "malloc");
switch (cstate->linktype) {
case DLT_EN10MB:
case DLT_NETANALYZER:
case DLT_NETANALYZER_TRANSPARENT:
tmp = gen_prevlinkhdr_check(cstate);
b = gen_ehostop(cstate, cstate->e, (int)q.dir);
if (tmp != NULL)
gen_and(tmp, b);
break;
case DLT_FDDI:
b = gen_fhostop(cstate, cstate->e, (int)q.dir);
break;
case DLT_IEEE802:
b = gen_thostop(cstate, cstate->e, (int)q.dir);
break;
case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
case DLT_PPI:
b = gen_wlanhostop(cstate, cstate->e, (int)q.dir);
break;
case DLT_IP_OVER_FC:
b = gen_ipfchostop(cstate, cstate->e, (int)q.dir);
break;
default:
free(cstate->e);
cstate->e = NULL;
bpf_error(cstate, "ethernet addresses supported only on ethernet/FDDI/token ring/802.11/ATM LANE/Fibre Channel");

}
free(cstate->e);
cstate->e = NULL;
return (b);
}
bpf_error(cstate, "ethernet address used in non-ether expression");

}

void
sappend(struct slist *s0, struct slist *s1)
{




while (s0->next)
s0 = s0->next;
s0->next = s1;
}

static struct slist *
xfer_to_x(compiler_state_t *cstate, struct arth *a)
{
struct slist *s;

s = new_stmt(cstate, BPF_LDX|BPF_MEM);
s->s.k = a->regno;
return s;
}

static struct slist *
xfer_to_a(compiler_state_t *cstate, struct arth *a)
{
struct slist *s;

s = new_stmt(cstate, BPF_LD|BPF_MEM);
s->s.k = a->regno;
return s;
}








static struct arth *
gen_load_internal(compiler_state_t *cstate, int proto, struct arth *inst, int size)
{
struct slist *s, *tmp;
struct block *b;
int regno = alloc_reg(cstate);

free_reg(cstate, inst->regno);
switch (size) {

default:
bpf_error(cstate, "data size must be 1, 2, or 4");

case 1:
size = BPF_B;
break;

case 2:
size = BPF_H;
break;

case 4:
size = BPF_W;
break;
}
switch (proto) {
default:
bpf_error(cstate, "unsupported index operation");

case Q_RADIO:





if (cstate->linktype != DLT_IEEE802_11_RADIO_AVS &&
cstate->linktype != DLT_IEEE802_11_RADIO &&
cstate->linktype != DLT_PRISM_HEADER)
bpf_error(cstate, "radio information not present in capture");





s = xfer_to_x(cstate, inst);




tmp = new_stmt(cstate, BPF_LD|BPF_IND|size);
sappend(s, tmp);
sappend(inst->s, s);
break;

case Q_LINK:











s = gen_abs_offset_varpart(cstate, &cstate->off_linkhdr);










if (s != NULL) {
sappend(s, xfer_to_a(cstate, inst));
sappend(s, new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_X));
sappend(s, new_stmt(cstate, BPF_MISC|BPF_TAX));
} else
s = xfer_to_x(cstate, inst);








tmp = new_stmt(cstate, BPF_LD|BPF_IND|size);
tmp->s.k = cstate->off_linkhdr.constant_part;
sappend(s, tmp);
sappend(inst->s, s);
break;

case Q_IP:
case Q_ARP:
case Q_RARP:
case Q_ATALK:
case Q_DECNET:
case Q_SCA:
case Q_LAT:
case Q_MOPRC:
case Q_MOPDL:
case Q_IPV6:






s = gen_abs_offset_varpart(cstate, &cstate->off_linkpl);










if (s != NULL) {
sappend(s, xfer_to_a(cstate, inst));
sappend(s, new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_X));
sappend(s, new_stmt(cstate, BPF_MISC|BPF_TAX));
} else
s = xfer_to_x(cstate, inst);








tmp = new_stmt(cstate, BPF_LD|BPF_IND|size);
tmp->s.k = cstate->off_linkpl.constant_part + cstate->off_nl;
sappend(s, tmp);
sappend(inst->s, s);





b = gen_proto_abbrev_internal(cstate, proto);
if (inst->b)
gen_and(inst->b, b);
inst->b = b;
break;

case Q_SCTP:
case Q_TCP:
case Q_UDP:
case Q_ICMP:
case Q_IGMP:
case Q_IGRP:
case Q_PIM:
case Q_VRRP:
case Q_CARP:














s = gen_loadx_iphdrlen(cstate);















sappend(s, xfer_to_a(cstate, inst));
sappend(s, new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_X));
sappend(s, new_stmt(cstate, BPF_MISC|BPF_TAX));
sappend(s, tmp = new_stmt(cstate, BPF_LD|BPF_IND|size));
tmp->s.k = cstate->off_linkpl.constant_part + cstate->off_nl;
sappend(inst->s, s);







gen_and(gen_proto_abbrev_internal(cstate, proto), b = gen_ipfrag(cstate));
if (inst->b)
gen_and(inst->b, b);
gen_and(gen_proto_abbrev_internal(cstate, Q_IP), b);
inst->b = b;
break;
case Q_ICMPV6:




b = gen_proto_abbrev_internal(cstate, Q_IPV6);
if (inst->b) {
gen_and(inst->b, b);
}
inst->b = b;




b = gen_cmp(cstate, OR_LINKPL, 6, BPF_B, 58);
if (inst->b) {
gen_and(inst->b, b);
}
inst->b = b;


s = gen_abs_offset_varpart(cstate, &cstate->off_linkpl);









if (s != NULL) {
sappend(s, xfer_to_a(cstate, inst));
sappend(s, new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_X));
sappend(s, new_stmt(cstate, BPF_MISC|BPF_TAX));
} else {
s = xfer_to_x(cstate, inst);
}








tmp = new_stmt(cstate, BPF_LD|BPF_IND|size);
tmp->s.k = cstate->off_linkpl.constant_part + cstate->off_nl + 40;

sappend(s, tmp);
sappend(inst->s, s);

break;
}
inst->regno = regno;
s = new_stmt(cstate, BPF_ST);
s->s.k = regno;
sappend(inst->s, s);

return inst;
}

struct arth *
gen_load(compiler_state_t *cstate, int proto, struct arth *inst, int size)
{




if (setjmp(cstate->top_ctx))
return (NULL);

return gen_load_internal(cstate, proto, inst, size);
}

static struct block *
gen_relation_internal(compiler_state_t *cstate, int code, struct arth *a0,
struct arth *a1, int reversed)
{
struct slist *s0, *s1, *s2;
struct block *b, *tmp;

s0 = xfer_to_x(cstate, a1);
s1 = xfer_to_a(cstate, a0);
if (code == BPF_JEQ) {
s2 = new_stmt(cstate, BPF_ALU|BPF_SUB|BPF_X);
b = new_block(cstate, JMP(code));
sappend(s1, s2);
}
else
b = new_block(cstate, BPF_JMP|code|BPF_X);
if (reversed)
gen_not(b);

sappend(s0, s1);
sappend(a1->s, s0);
sappend(a0->s, a1->s);

b->stmts = a0->s;

free_reg(cstate, a0->regno);
free_reg(cstate, a1->regno);


if (a0->b) {
if (a1->b) {
gen_and(a0->b, tmp = a1->b);
}
else
tmp = a0->b;
} else
tmp = a1->b;

if (tmp)
gen_and(tmp, b);

return b;
}

struct block *
gen_relation(compiler_state_t *cstate, int code, struct arth *a0,
struct arth *a1, int reversed)
{




if (setjmp(cstate->top_ctx))
return (NULL);

return gen_relation_internal(cstate, code, a0, a1, reversed);
}

struct arth *
gen_loadlen(compiler_state_t *cstate)
{
int regno;
struct arth *a;
struct slist *s;





if (setjmp(cstate->top_ctx))
return (NULL);

regno = alloc_reg(cstate);
a = (struct arth *)newchunk(cstate, sizeof(*a));
s = new_stmt(cstate, BPF_LD|BPF_LEN);
s->next = new_stmt(cstate, BPF_ST);
s->next->s.k = regno;
a->s = s;
a->regno = regno;

return a;
}

static struct arth *
gen_loadi_internal(compiler_state_t *cstate, int val)
{
struct arth *a;
struct slist *s;
int reg;

a = (struct arth *)newchunk(cstate, sizeof(*a));

reg = alloc_reg(cstate);

s = new_stmt(cstate, BPF_LD|BPF_IMM);
s->s.k = val;
s->next = new_stmt(cstate, BPF_ST);
s->next->s.k = reg;
a->s = s;
a->regno = reg;

return a;
}

struct arth *
gen_loadi(compiler_state_t *cstate, int val)
{




if (setjmp(cstate->top_ctx))
return (NULL);

return gen_loadi_internal(cstate, val);
}






struct arth *
gen_neg(compiler_state_t *cstate, struct arth *a_arg)
{
struct arth *a = a_arg;
struct slist *s;





if (setjmp(cstate->top_ctx))
return (NULL);

s = xfer_to_a(cstate, a);
sappend(a->s, s);
s = new_stmt(cstate, BPF_ALU|BPF_NEG);
s->s.k = 0;
sappend(a->s, s);
s = new_stmt(cstate, BPF_ST);
s->s.k = a->regno;
sappend(a->s, s);

return a;
}






struct arth *
gen_arth(compiler_state_t *cstate, int code, struct arth *a0_arg,
struct arth *a1)
{
struct arth *a0 = a0_arg;
struct slist *s0, *s1, *s2;





if (setjmp(cstate->top_ctx))
return (NULL);








if (code == BPF_DIV) {
if (a1->s->s.code == (BPF_LD|BPF_IMM) && a1->s->s.k == 0)
bpf_error(cstate, "division by zero");
} else if (code == BPF_MOD) {
if (a1->s->s.code == (BPF_LD|BPF_IMM) && a1->s->s.k == 0)
bpf_error(cstate, "modulus by zero");
} else if (code == BPF_LSH || code == BPF_RSH) {





if (a1->s->s.code == (BPF_LD|BPF_IMM) &&
(a1->s->s.k < 0 || a1->s->s.k > 31))
bpf_error(cstate, "shift by more than 31 bits");
}
s0 = xfer_to_x(cstate, a1);
s1 = xfer_to_a(cstate, a0);
s2 = new_stmt(cstate, BPF_ALU|BPF_X|code);

sappend(s1, s2);
sappend(s0, s1);
sappend(a1->s, s0);
sappend(a0->s, a1->s);

free_reg(cstate, a0->regno);
free_reg(cstate, a1->regno);

s0 = new_stmt(cstate, BPF_ST);
a0->regno = s0->s.k = alloc_reg(cstate);
sappend(a0->s, s0);

return a0;
}




static void
init_regs(compiler_state_t *cstate)
{
cstate->curreg = 0;
memset(cstate->regused, 0, sizeof cstate->regused);
}




static int
alloc_reg(compiler_state_t *cstate)
{
int n = BPF_MEMWORDS;

while (--n >= 0) {
if (cstate->regused[cstate->curreg])
cstate->curreg = (cstate->curreg + 1) % BPF_MEMWORDS;
else {
cstate->regused[cstate->curreg] = 1;
return cstate->curreg;
}
}
bpf_error(cstate, "too many registers needed to evaluate expression");

}





static void
free_reg(compiler_state_t *cstate, int n)
{
cstate->regused[n] = 0;
}

static struct block *
gen_len(compiler_state_t *cstate, int jmp, int n)
{
struct slist *s;
struct block *b;

s = new_stmt(cstate, BPF_LD|BPF_LEN);
b = new_block(cstate, JMP(jmp));
b->stmts = s;
b->s.k = n;

return b;
}

struct block *
gen_greater(compiler_state_t *cstate, int n)
{




if (setjmp(cstate->top_ctx))
return (NULL);

return gen_len(cstate, BPF_JGE, n);
}




struct block *
gen_less(compiler_state_t *cstate, int n)
{
struct block *b;





if (setjmp(cstate->top_ctx))
return (NULL);

b = gen_len(cstate, BPF_JGT, n);
gen_not(b);

return b;
}











struct block *
gen_byteop(compiler_state_t *cstate, int op, int idx, int val)
{
struct block *b;
struct slist *s;





if (setjmp(cstate->top_ctx))
return (NULL);

switch (op) {
default:
abort();

case '=':
return gen_cmp(cstate, OR_LINKHDR, (u_int)idx, BPF_B, (bpf_int32)val);

case '<':
b = gen_cmp_lt(cstate, OR_LINKHDR, (u_int)idx, BPF_B, (bpf_int32)val);
return b;

case '>':
b = gen_cmp_gt(cstate, OR_LINKHDR, (u_int)idx, BPF_B, (bpf_int32)val);
return b;

case '|':
s = new_stmt(cstate, BPF_ALU|BPF_OR|BPF_K);
break;

case '&':
s = new_stmt(cstate, BPF_ALU|BPF_AND|BPF_K);
break;
}
s->s.k = val;
b = new_block(cstate, JMP(BPF_JEQ));
b->stmts = s;
gen_not(b);

return b;
}

static const u_char abroadcast[] = { 0x0 };

struct block *
gen_broadcast(compiler_state_t *cstate, int proto)
{
bpf_u_int32 hostmask;
struct block *b0, *b1, *b2;
static const u_char ebroadcast[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };





if (setjmp(cstate->top_ctx))
return (NULL);

switch (proto) {

case Q_DEFAULT:
case Q_LINK:
switch (cstate->linktype) {
case DLT_ARCNET:
case DLT_ARCNET_LINUX:
return gen_ahostop(cstate, abroadcast, Q_DST);
case DLT_EN10MB:
case DLT_NETANALYZER:
case DLT_NETANALYZER_TRANSPARENT:
b1 = gen_prevlinkhdr_check(cstate);
b0 = gen_ehostop(cstate, ebroadcast, Q_DST);
if (b1 != NULL)
gen_and(b1, b0);
return b0;
case DLT_FDDI:
return gen_fhostop(cstate, ebroadcast, Q_DST);
case DLT_IEEE802:
return gen_thostop(cstate, ebroadcast, Q_DST);
case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
case DLT_PPI:
return gen_wlanhostop(cstate, ebroadcast, Q_DST);
case DLT_IP_OVER_FC:
return gen_ipfchostop(cstate, ebroadcast, Q_DST);
default:
bpf_error(cstate, "not a broadcast link");
}


case Q_IP:





if (cstate->netmask == PCAP_NETMASK_UNKNOWN)
bpf_error(cstate, "netmask not known, so 'ip broadcast' not supported");
b0 = gen_linktype(cstate, ETHERTYPE_IP);
hostmask = ~cstate->netmask;
b1 = gen_mcmp(cstate, OR_LINKPL, 16, BPF_W, (bpf_int32)0, hostmask);
b2 = gen_mcmp(cstate, OR_LINKPL, 16, BPF_W,
(bpf_int32)(~0 & hostmask), hostmask);
gen_or(b1, b2);
gen_and(b0, b2);
return b2;
}
bpf_error(cstate, "only link-layer/IP broadcast filters supported");

}





static struct block *
gen_mac_multicast(compiler_state_t *cstate, int offset)
{
register struct block *b0;
register struct slist *s;


s = gen_load_a(cstate, OR_LINKHDR, offset, BPF_B);
b0 = new_block(cstate, JMP(BPF_JSET));
b0->s.k = 1;
b0->stmts = s;
return b0;
}

struct block *
gen_multicast(compiler_state_t *cstate, int proto)
{
register struct block *b0, *b1, *b2;
register struct slist *s;





if (setjmp(cstate->top_ctx))
return (NULL);

switch (proto) {

case Q_DEFAULT:
case Q_LINK:
switch (cstate->linktype) {
case DLT_ARCNET:
case DLT_ARCNET_LINUX:

return gen_ahostop(cstate, abroadcast, Q_DST);
case DLT_EN10MB:
case DLT_NETANALYZER:
case DLT_NETANALYZER_TRANSPARENT:
b1 = gen_prevlinkhdr_check(cstate);

b0 = gen_mac_multicast(cstate, 0);
if (b1 != NULL)
gen_and(b1, b0);
return b0;
case DLT_FDDI:






return gen_mac_multicast(cstate, 1);
case DLT_IEEE802:

return gen_mac_multicast(cstate, 2);
case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
case DLT_PPI:





















s = gen_load_a(cstate, OR_LINKHDR, 1, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x01;
b1->stmts = s;




b0 = gen_mac_multicast(cstate, 16);
gen_and(b1, b0);





s = gen_load_a(cstate, OR_LINKHDR, 1, BPF_B);
b2 = new_block(cstate, JMP(BPF_JSET));
b2->s.k = 0x01;
b2->stmts = s;
gen_not(b2);




b1 = gen_mac_multicast(cstate, 4);
gen_and(b2, b1);





gen_or(b1, b0);





s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x08;
b1->stmts = s;




gen_and(b1, b0);






s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b2 = new_block(cstate, JMP(BPF_JSET));
b2->s.k = 0x08;
b2->stmts = s;
gen_not(b2);




b1 = gen_mac_multicast(cstate, 4);
gen_and(b2, b1);






gen_or(b1, b0);









s = gen_load_a(cstate, OR_LINKHDR, 0, BPF_B);
b1 = new_block(cstate, JMP(BPF_JSET));
b1->s.k = 0x04;
b1->stmts = s;
gen_not(b1);





gen_and(b1, b0);
return b0;
case DLT_IP_OVER_FC:
b0 = gen_mac_multicast(cstate, 2);
return b0;
default:
break;
}

break;

case Q_IP:
b0 = gen_linktype(cstate, ETHERTYPE_IP);
b1 = gen_cmp_ge(cstate, OR_LINKPL, 16, BPF_B, (bpf_int32)224);
gen_and(b0, b1);
return b1;

case Q_IPV6:
b0 = gen_linktype(cstate, ETHERTYPE_IPV6);
b1 = gen_cmp(cstate, OR_LINKPL, 24, BPF_B, (bpf_int32)255);
gen_and(b0, b1);
return b1;
}
bpf_error(cstate, "link-layer multicast filters supported only on ethernet/FDDI/token ring/ARCNET/802.11/ATM LANE/Fibre Channel");

}










struct block *
gen_inbound(compiler_state_t *cstate, int dir)
{
register struct block *b0;





if (setjmp(cstate->top_ctx))
return (NULL);




switch (cstate->linktype) {
case DLT_SLIP:
b0 = gen_relation_internal(cstate, BPF_JEQ,
gen_load_internal(cstate, Q_LINK, gen_loadi_internal(cstate, 0), 1),
gen_loadi_internal(cstate, 0),
dir);
break;

case DLT_IPNET:
if (dir) {

b0 = gen_cmp(cstate, OR_LINKHDR, 2, BPF_H, IPNET_OUTBOUND);
} else {

b0 = gen_cmp(cstate, OR_LINKHDR, 2, BPF_H, IPNET_INBOUND);
}
break;

case DLT_LINUX_SLL:

b0 = gen_cmp(cstate, OR_LINKHDR, 0, BPF_H, LINUX_SLL_OUTGOING);
if (!dir) {

gen_not(b0);
}
break;

case DLT_LINUX_SLL2:

b0 = gen_cmp(cstate, OR_LINKHDR, 10, BPF_B, LINUX_SLL_OUTGOING);
if (!dir) {

gen_not(b0);
}
break;

#if defined(HAVE_NET_PFVAR_H)
case DLT_PFLOG:
b0 = gen_cmp(cstate, OR_LINKHDR, offsetof(struct pfloghdr, dir), BPF_B,
(bpf_int32)((dir == 0) ? PF_IN : PF_OUT));
break;
#endif

case DLT_PPP_PPPD:
if (dir) {

b0 = gen_cmp(cstate, OR_LINKHDR, 0, BPF_B, PPP_PPPD_OUT);
} else {

b0 = gen_cmp(cstate, OR_LINKHDR, 0, BPF_B, PPP_PPPD_IN);
}
break;

case DLT_JUNIPER_MFR:
case DLT_JUNIPER_MLFR:
case DLT_JUNIPER_MLPPP:
case DLT_JUNIPER_ATM1:
case DLT_JUNIPER_ATM2:
case DLT_JUNIPER_PPPOE:
case DLT_JUNIPER_PPPOE_ATM:
case DLT_JUNIPER_GGSN:
case DLT_JUNIPER_ES:
case DLT_JUNIPER_MONITOR:
case DLT_JUNIPER_SERVICES:
case DLT_JUNIPER_ETHER:
case DLT_JUNIPER_PPP:
case DLT_JUNIPER_FRELAY:
case DLT_JUNIPER_CHDLC:
case DLT_JUNIPER_VP:
case DLT_JUNIPER_ST:
case DLT_JUNIPER_ISM:
case DLT_JUNIPER_VS:
case DLT_JUNIPER_SRX_E2E:
case DLT_JUNIPER_FIBRECHANNEL:
case DLT_JUNIPER_ATM_CEMIC:



if (dir) {

b0 = gen_mcmp(cstate, OR_LINKHDR, 3, BPF_B, 0, 0x01);
} else {

b0 = gen_mcmp(cstate, OR_LINKHDR, 3, BPF_B, 1, 0x01);
}
break;

default:















#if defined(linux) && defined(PF_PACKET) && defined(SO_ATTACH_FILTER)






if (cstate->bpf_pcap->rfile != NULL) {

bpf_error(cstate, "inbound/outbound not supported on %s when reading savefiles",
pcap_datalink_val_to_description_or_dlt(cstate->linktype));
b0 = NULL;

}

b0 = gen_cmp(cstate, OR_LINKHDR, SKF_AD_OFF + SKF_AD_PKTTYPE, BPF_H,
PACKET_OUTGOING);
if (!dir) {

gen_not(b0);
}
#else
bpf_error(cstate, "inbound/outbound not supported on %s",
pcap_datalink_val_to_description_or_dlt(cstate->linktype));

#endif
}
return (b0);
}

#if defined(HAVE_NET_PFVAR_H)

struct block *
gen_pf_ifname(compiler_state_t *cstate, const char *ifname)
{
struct block *b0;
u_int len, off;





if (setjmp(cstate->top_ctx))
return (NULL);

if (cstate->linktype != DLT_PFLOG) {
bpf_error(cstate, "ifname supported only on PF linktype");

}
len = sizeof(((struct pfloghdr *)0)->ifname);
off = offsetof(struct pfloghdr, ifname);
if (strlen(ifname) >= len) {
bpf_error(cstate, "ifname interface names can only be %d characters",
len-1);

}
b0 = gen_bcmp(cstate, OR_LINKHDR, off, (u_int)strlen(ifname),
(const u_char *)ifname);
return (b0);
}


struct block *
gen_pf_ruleset(compiler_state_t *cstate, char *ruleset)
{
struct block *b0;





if (setjmp(cstate->top_ctx))
return (NULL);

if (cstate->linktype != DLT_PFLOG) {
bpf_error(cstate, "ruleset supported only on PF linktype");

}

if (strlen(ruleset) >= sizeof(((struct pfloghdr *)0)->ruleset)) {
bpf_error(cstate, "ruleset names can only be %ld characters",
(long)(sizeof(((struct pfloghdr *)0)->ruleset) - 1));

}

b0 = gen_bcmp(cstate, OR_LINKHDR, offsetof(struct pfloghdr, ruleset),
(u_int)strlen(ruleset), (const u_char *)ruleset);
return (b0);
}


struct block *
gen_pf_rnr(compiler_state_t *cstate, int rnr)
{
struct block *b0;





if (setjmp(cstate->top_ctx))
return (NULL);

if (cstate->linktype != DLT_PFLOG) {
bpf_error(cstate, "rnr supported only on PF linktype");

}

b0 = gen_cmp(cstate, OR_LINKHDR, offsetof(struct pfloghdr, rulenr), BPF_W,
(bpf_int32)rnr);
return (b0);
}


struct block *
gen_pf_srnr(compiler_state_t *cstate, int srnr)
{
struct block *b0;





if (setjmp(cstate->top_ctx))
return (NULL);

if (cstate->linktype != DLT_PFLOG) {
bpf_error(cstate, "srnr supported only on PF linktype");

}

b0 = gen_cmp(cstate, OR_LINKHDR, offsetof(struct pfloghdr, subrulenr), BPF_W,
(bpf_int32)srnr);
return (b0);
}


struct block *
gen_pf_reason(compiler_state_t *cstate, int reason)
{
struct block *b0;





if (setjmp(cstate->top_ctx))
return (NULL);

if (cstate->linktype != DLT_PFLOG) {
bpf_error(cstate, "reason supported only on PF linktype");

}

b0 = gen_cmp(cstate, OR_LINKHDR, offsetof(struct pfloghdr, reason), BPF_B,
(bpf_int32)reason);
return (b0);
}


struct block *
gen_pf_action(compiler_state_t *cstate, int action)
{
struct block *b0;





if (setjmp(cstate->top_ctx))
return (NULL);

if (cstate->linktype != DLT_PFLOG) {
bpf_error(cstate, "action supported only on PF linktype");

}

b0 = gen_cmp(cstate, OR_LINKHDR, offsetof(struct pfloghdr, action), BPF_B,
(bpf_int32)action);
return (b0);
}
#else
struct block *
gen_pf_ifname(compiler_state_t *cstate, const char *ifname _U_)
{




if (setjmp(cstate->top_ctx))
return (NULL);

bpf_error(cstate, "libpcap was compiled without pf support");

}

struct block *
gen_pf_ruleset(compiler_state_t *cstate, char *ruleset _U_)
{




if (setjmp(cstate->top_ctx))
return (NULL);

bpf_error(cstate, "libpcap was compiled on a machine without pf support");

}

struct block *
gen_pf_rnr(compiler_state_t *cstate, int rnr _U_)
{




if (setjmp(cstate->top_ctx))
return (NULL);

bpf_error(cstate, "libpcap was compiled on a machine without pf support");

}

struct block *
gen_pf_srnr(compiler_state_t *cstate, int srnr _U_)
{




if (setjmp(cstate->top_ctx))
return (NULL);

bpf_error(cstate, "libpcap was compiled on a machine without pf support");

}

struct block *
gen_pf_reason(compiler_state_t *cstate, int reason _U_)
{




if (setjmp(cstate->top_ctx))
return (NULL);

bpf_error(cstate, "libpcap was compiled on a machine without pf support");

}

struct block *
gen_pf_action(compiler_state_t *cstate, int action _U_)
{




if (setjmp(cstate->top_ctx))
return (NULL);

bpf_error(cstate, "libpcap was compiled on a machine without pf support");

}
#endif


struct block *
gen_p80211_type(compiler_state_t *cstate, int type, int mask)
{
struct block *b0;





if (setjmp(cstate->top_ctx))
return (NULL);

switch (cstate->linktype) {

case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
b0 = gen_mcmp(cstate, OR_LINKHDR, 0, BPF_B, (bpf_int32)type,
(bpf_int32)mask);
break;

default:
bpf_error(cstate, "802.11 link-layer types supported only on 802.11");

}

return (b0);
}

struct block *
gen_p80211_fcdir(compiler_state_t *cstate, int fcdir)
{
struct block *b0;





if (setjmp(cstate->top_ctx))
return (NULL);

switch (cstate->linktype) {

case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
break;

default:
bpf_error(cstate, "frame direction supported only with 802.11 headers");

}

b0 = gen_mcmp(cstate, OR_LINKHDR, 1, BPF_B, (bpf_int32)fcdir,
(bpf_u_int32)IEEE80211_FC1_DIR_MASK);

return (b0);
}

struct block *
gen_acode(compiler_state_t *cstate, const char *s, struct qual q)
{
struct block *b;





if (setjmp(cstate->top_ctx))
return (NULL);

switch (cstate->linktype) {

case DLT_ARCNET:
case DLT_ARCNET_LINUX:
if ((q.addr == Q_HOST || q.addr == Q_DEFAULT) &&
q.proto == Q_LINK) {
cstate->e = pcap_ether_aton(s);
if (cstate->e == NULL)
bpf_error(cstate, "malloc");
b = gen_ahostop(cstate, cstate->e, (int)q.dir);
free(cstate->e);
cstate->e = NULL;
return (b);
} else
bpf_error(cstate, "ARCnet address used in non-arc expression");


default:
bpf_error(cstate, "aid supported only on ARCnet");

}
}

static struct block *
gen_ahostop(compiler_state_t *cstate, const u_char *eaddr, int dir)
{
register struct block *b0, *b1;

switch (dir) {

case Q_SRC:
return gen_bcmp(cstate, OR_LINKHDR, 0, 1, eaddr);

case Q_DST:
return gen_bcmp(cstate, OR_LINKHDR, 1, 1, eaddr);

case Q_AND:
b0 = gen_ahostop(cstate, eaddr, Q_SRC);
b1 = gen_ahostop(cstate, eaddr, Q_DST);
gen_and(b0, b1);
return b1;

case Q_DEFAULT:
case Q_OR:
b0 = gen_ahostop(cstate, eaddr, Q_SRC);
b1 = gen_ahostop(cstate, eaddr, Q_DST);
gen_or(b0, b1);
return b1;

case Q_ADDR1:
bpf_error(cstate, "'addr1' and 'address1' are only supported on 802.11");


case Q_ADDR2:
bpf_error(cstate, "'addr2' and 'address2' are only supported on 802.11");


case Q_ADDR3:
bpf_error(cstate, "'addr3' and 'address3' are only supported on 802.11");


case Q_ADDR4:
bpf_error(cstate, "'addr4' and 'address4' are only supported on 802.11");


case Q_RA:
bpf_error(cstate, "'ra' is only supported on 802.11");


case Q_TA:
bpf_error(cstate, "'ta' is only supported on 802.11");

}
abort();

}

static struct block *
gen_vlan_tpid_test(compiler_state_t *cstate)
{
struct block *b0, *b1;


b0 = gen_linktype(cstate, ETHERTYPE_8021Q);
b1 = gen_linktype(cstate, ETHERTYPE_8021AD);
gen_or(b0,b1);
b0 = b1;
b1 = gen_linktype(cstate, ETHERTYPE_8021QINQ);
gen_or(b0,b1);

return b1;
}

static struct block *
gen_vlan_vid_test(compiler_state_t *cstate, bpf_u_int32 vlan_num)
{
if (vlan_num > 0x0fff) {
bpf_error(cstate, "VLAN tag %u greater than maximum %u",
vlan_num, 0x0fff);
}
return gen_mcmp(cstate, OR_LINKPL, 0, BPF_H, (bpf_int32)vlan_num, 0x0fff);
}

static struct block *
gen_vlan_no_bpf_extensions(compiler_state_t *cstate, bpf_u_int32 vlan_num,
int has_vlan_tag)
{
struct block *b0, *b1;

b0 = gen_vlan_tpid_test(cstate);

if (has_vlan_tag) {
b1 = gen_vlan_vid_test(cstate, vlan_num);
gen_and(b0, b1);
b0 = b1;
}





cstate->off_linkpl.constant_part += 4;
cstate->off_linktype.constant_part += 4;

return b0;
}

#if defined(SKF_AD_VLAN_TAG_PRESENT)

static void
gen_vlan_vloffset_add(compiler_state_t *cstate, bpf_abs_offset *off, int v, struct slist *s)
{
struct slist *s2;

if (!off->is_variable)
off->is_variable = 1;
if (off->reg == -1)
off->reg = alloc_reg(cstate);

s2 = new_stmt(cstate, BPF_LD|BPF_MEM);
s2->s.k = off->reg;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_IMM);
s2->s.k = v;
sappend(s, s2);
s2 = new_stmt(cstate, BPF_ST);
s2->s.k = off->reg;
sappend(s, s2);
}





static void
gen_vlan_patch_tpid_test(compiler_state_t *cstate, struct block *b_tpid)
{
struct slist s;


s.next = NULL;
cstate->is_vlan_vloffset = 1;
gen_vlan_vloffset_add(cstate, &cstate->off_linkpl, 4, &s);
gen_vlan_vloffset_add(cstate, &cstate->off_linktype, 4, &s);


sappend(s.next, b_tpid->head->stmts);
b_tpid->head->stmts = s.next;
}





static void
gen_vlan_patch_vid_test(compiler_state_t *cstate, struct block *b_vid)
{
struct slist *s, *s2, *sjeq;
unsigned cnt;

s = new_stmt(cstate, BPF_LD|BPF_B|BPF_ABS);
s->s.k = SKF_AD_OFF + SKF_AD_VLAN_TAG_PRESENT;


sjeq = new_stmt(cstate, JMP(BPF_JEQ));
sjeq->s.k = 1;
sjeq->s.jf = b_vid->stmts;
sappend(s, sjeq);

s2 = new_stmt(cstate, BPF_LD|BPF_B|BPF_ABS);
s2->s.k = SKF_AD_OFF + SKF_AD_VLAN_TAG;
sappend(s, s2);
sjeq->s.jt = s2;





cnt = 0;
for (s2 = b_vid->stmts; s2; s2 = s2->next)
cnt++;
s2 = new_stmt(cstate, JMP(BPF_JA));
s2->s.k = cnt - 1;
sappend(s, s2);


sappend(s, b_vid->stmts);
b_vid->stmts = s;
}









static struct block *
gen_vlan_bpf_extensions(compiler_state_t *cstate, bpf_u_int32 vlan_num,
int has_vlan_tag)
{
struct block *b0, *b_tpid, *b_vid = NULL;
struct slist *s;



s = new_stmt(cstate, BPF_LD|BPF_B|BPF_ABS);
s->s.k = SKF_AD_OFF + SKF_AD_VLAN_TAG_PRESENT;

b0 = new_block(cstate, JMP(BPF_JEQ));
b0->stmts = s;
b0->s.k = 1;











b_tpid = gen_vlan_tpid_test(cstate);
if (has_vlan_tag)
b_vid = gen_vlan_vid_test(cstate, vlan_num);

gen_vlan_patch_tpid_test(cstate, b_tpid);
gen_or(b0, b_tpid);
b0 = b_tpid;

if (has_vlan_tag) {
gen_vlan_patch_vid_test(cstate, b_vid);
gen_and(b0, b_vid);
b0 = b_vid;
}

return b0;
}
#endif




struct block *
gen_vlan(compiler_state_t *cstate, bpf_u_int32 vlan_num, int has_vlan_tag)
{
struct block *b0;





if (setjmp(cstate->top_ctx))
return (NULL);


if (cstate->label_stack_depth > 0)
bpf_error(cstate, "no VLAN match after MPLS");
































switch (cstate->linktype) {

case DLT_EN10MB:
case DLT_NETANALYZER:
case DLT_NETANALYZER_TRANSPARENT:
#if defined(SKF_AD_VLAN_TAG_PRESENT)


if (cstate->vlan_stack_depth == 0 && !cstate->off_linkhdr.is_variable &&
cstate->off_linkhdr.constant_part ==
cstate->off_outermostlinkhdr.constant_part) {



if (cstate->bpf_pcap->bpf_codegen_flags & BPF_SPECIAL_VLAN_HANDLING)
b0 = gen_vlan_bpf_extensions(cstate, vlan_num,
has_vlan_tag);
else
b0 = gen_vlan_no_bpf_extensions(cstate,
vlan_num, has_vlan_tag);
} else
#endif
b0 = gen_vlan_no_bpf_extensions(cstate, vlan_num,
has_vlan_tag);
break;

case DLT_IEEE802_11:
case DLT_PRISM_HEADER:
case DLT_IEEE802_11_RADIO_AVS:
case DLT_IEEE802_11_RADIO:
b0 = gen_vlan_no_bpf_extensions(cstate, vlan_num, has_vlan_tag);
break;

default:
bpf_error(cstate, "no VLAN support for %s",
pcap_datalink_val_to_description_or_dlt(cstate->linktype));

}

cstate->vlan_stack_depth++;

return (b0);
}








struct block *
gen_mpls(compiler_state_t *cstate, bpf_u_int32 label_num_arg,
int has_label_num)
{
volatile bpf_u_int32 label_num = label_num_arg;
struct block *b0, *b1;





if (setjmp(cstate->top_ctx))
return (NULL);

if (cstate->label_stack_depth > 0) {

b0 = gen_mcmp(cstate, OR_PREVMPLSHDR, 2, BPF_B, 0, 0x01);
} else {




switch (cstate->linktype) {

case DLT_C_HDLC:
case DLT_EN10MB:
case DLT_NETANALYZER:
case DLT_NETANALYZER_TRANSPARENT:
b0 = gen_linktype(cstate, ETHERTYPE_MPLS);
break;

case DLT_PPP:
b0 = gen_linktype(cstate, PPP_MPLS_UCAST);
break;





default:
bpf_error(cstate, "no MPLS support for %s",
pcap_datalink_val_to_description_or_dlt(cstate->linktype));

}
}


if (has_label_num) {
if (label_num > 0xFFFFF) {
bpf_error(cstate, "MPLS label %u greater than maximum %u",
label_num, 0xFFFFF);
}
label_num = label_num << 12;
b1 = gen_mcmp(cstate, OR_LINKPL, 0, BPF_W, (bpf_int32)label_num,
0xfffff000);
gen_and(b0, b1);
b0 = b1;
}















cstate->off_nl_nosnap += 4;
cstate->off_nl += 4;
cstate->label_stack_depth++;
return (b0);
}




struct block *
gen_pppoed(compiler_state_t *cstate)
{




if (setjmp(cstate->top_ctx))
return (NULL);


return gen_linktype(cstate, (bpf_int32)ETHERTYPE_PPPOED);
}

struct block *
gen_pppoes(compiler_state_t *cstate, bpf_u_int32 sess_num, int has_sess_num)
{
struct block *b0, *b1;





if (setjmp(cstate->top_ctx))
return (NULL);




b0 = gen_linktype(cstate, (bpf_int32)ETHERTYPE_PPPOES);


if (has_sess_num) {
if (sess_num > 0x0000ffff) {
bpf_error(cstate, "PPPoE session number %u greater than maximum %u",
sess_num, 0x0000ffff);
}
b1 = gen_mcmp(cstate, OR_LINKPL, 0, BPF_W,
(bpf_int32)sess_num, 0x0000ffff);
gen_and(b0, b1);
b0 = b1;
}



















PUSH_LINKHDR(cstate, DLT_PPP, cstate->off_linkpl.is_variable,
cstate->off_linkpl.constant_part + cstate->off_nl + 6,
cstate->off_linkpl.reg);

cstate->off_linktype = cstate->off_linkhdr;
cstate->off_linkpl.constant_part = cstate->off_linkhdr.constant_part + 2;

cstate->off_nl = 0;
cstate->off_nl_nosnap = 0;

return b0;
}



static struct block *
gen_geneve_check(compiler_state_t *cstate,
struct block *(*gen_portfn)(compiler_state_t *, int, int, int),
enum e_offrel offrel, bpf_u_int32 vni, int has_vni)
{
struct block *b0, *b1;

b0 = gen_portfn(cstate, GENEVE_PORT, IPPROTO_UDP, Q_DST);




b1 = gen_mcmp(cstate, offrel, 8, BPF_B, (bpf_int32)0, 0xc0);
gen_and(b0, b1);
b0 = b1;

if (has_vni) {
if (vni > 0xffffff) {
bpf_error(cstate, "Geneve VNI %u greater than maximum %u",
vni, 0xffffff);
}
vni <<= 8;
b1 = gen_mcmp(cstate, offrel, 12, BPF_W, (bpf_int32)vni,
0xffffff00);
gen_and(b0, b1);
b0 = b1;
}

return b0;
}






static struct block *
gen_geneve4(compiler_state_t *cstate, bpf_u_int32 vni, int has_vni)
{
struct block *b0, *b1;
struct slist *s, *s1;

b0 = gen_geneve_check(cstate, gen_port, OR_TRAN_IPV4, vni, has_vni);


s = gen_loadx_iphdrlen(cstate);

s1 = new_stmt(cstate, BPF_MISC|BPF_TXA);
sappend(s, s1);




b1 = new_block(cstate, BPF_JMP|BPF_JEQ|BPF_X);
b1->stmts = s;
b1->s.k = 0;

gen_and(b0, b1);

return b1;
}

static struct block *
gen_geneve6(compiler_state_t *cstate, bpf_u_int32 vni, int has_vni)
{
struct block *b0, *b1;
struct slist *s, *s1;

b0 = gen_geneve_check(cstate, gen_port6, OR_TRAN_IPV6, vni, has_vni);



s = gen_abs_offset_varpart(cstate, &cstate->off_linkpl);
if (s) {
s1 = new_stmt(cstate, BPF_LD|BPF_IMM);
s1->s.k = 40;
sappend(s, s1);

s1 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_X);
s1->s.k = 0;
sappend(s, s1);
} else {
s = new_stmt(cstate, BPF_LD|BPF_IMM);
s->s.k = 40;
}




s1 = new_stmt(cstate, BPF_MISC|BPF_TAX);
sappend(s, s1);

b1 = new_block(cstate, BPF_JMP|BPF_JEQ|BPF_X);
b1->stmts = s;
b1->s.k = 0;

gen_and(b0, b1);

return b1;
}





static struct slist *
gen_geneve_offsets(compiler_state_t *cstate)
{
struct slist *s, *s1, *s_proto;






s = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s->s.k = cstate->off_linkpl.constant_part + cstate->off_nl + 8;


s1 = new_stmt(cstate, BPF_MISC|BPF_TAX);
sappend(s, s1);



s1 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s1->s.k = 2;
sappend(s, s1);

cstate->off_linktype.reg = alloc_reg(cstate);
cstate->off_linktype.is_variable = 1;
cstate->off_linktype.constant_part = 0;

s1 = new_stmt(cstate, BPF_ST);
s1->s.k = cstate->off_linktype.reg;
sappend(s, s1);




s1 = new_stmt(cstate, BPF_LD|BPF_IND|BPF_B);
s1->s.k = 0;
sappend(s, s1);

s1 = new_stmt(cstate, BPF_ALU|BPF_AND|BPF_K);
s1->s.k = 0x3f;
sappend(s, s1);

s1 = new_stmt(cstate, BPF_ALU|BPF_MUL|BPF_K);
s1->s.k = 4;
sappend(s, s1);


s1 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s1->s.k = 8;
sappend(s, s1);


s1 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_X);
s1->s.k = 0;
sappend(s, s1);









PUSH_LINKHDR(cstate, DLT_EN10MB, 1, 0, alloc_reg(cstate));

s1 = new_stmt(cstate, BPF_ST);
s1->s.k = cstate->off_linkhdr.reg;
sappend(s, s1);







cstate->no_optimize = 1;


s1 = new_stmt(cstate, BPF_LD|BPF_IND|BPF_H);
s1->s.k = 2;
sappend(s, s1);


s1 = new_stmt(cstate, BPF_LDX|BPF_MEM);
s1->s.k = cstate->off_linkhdr.reg;
sappend(s, s1);




s_proto = new_stmt(cstate, JMP(BPF_JEQ));
s_proto->s.k = ETHERTYPE_TEB;
sappend(s, s_proto);

s1 = new_stmt(cstate, BPF_MISC|BPF_TXA);
sappend(s, s1);
s_proto->s.jt = s1;



s1 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s1->s.k = 12;
sappend(s, s1);

s1 = new_stmt(cstate, BPF_ST);
s1->s.k = cstate->off_linktype.reg;
sappend(s, s1);



s1 = new_stmt(cstate, BPF_ALU|BPF_ADD|BPF_K);
s1->s.k = 2;
sappend(s, s1);


s1 = new_stmt(cstate, BPF_MISC|BPF_TAX);
sappend(s, s1);


cstate->off_linkpl.reg = alloc_reg(cstate);
cstate->off_linkpl.is_variable = 1;
cstate->off_linkpl.constant_part = 0;

s1 = new_stmt(cstate, BPF_STX);
s1->s.k = cstate->off_linkpl.reg;
sappend(s, s1);
s_proto->s.jf = s1;

cstate->off_nl = 0;

return s;
}


struct block *
gen_geneve(compiler_state_t *cstate, bpf_u_int32 vni, int has_vni)
{
struct block *b0, *b1;
struct slist *s;





if (setjmp(cstate->top_ctx))
return (NULL);

b0 = gen_geneve4(cstate, vni, has_vni);
b1 = gen_geneve6(cstate, vni, has_vni);

gen_or(b0, b1);
b0 = b1;




s = gen_geneve_offsets(cstate);

b1 = gen_true(cstate);
sappend(s, b1->stmts);
b1->stmts = s;

gen_and(b0, b1);

cstate->is_geneve = 1;

return b1;
}



static struct block *
gen_geneve_ll_check(compiler_state_t *cstate)
{
struct block *b0;
struct slist *s, *s1;







s = new_stmt(cstate, BPF_LD|BPF_MEM);
s->s.k = cstate->off_linkhdr.reg;

s1 = new_stmt(cstate, BPF_LDX|BPF_MEM);
s1->s.k = cstate->off_linkpl.reg;
sappend(s, s1);

b0 = new_block(cstate, BPF_JMP|BPF_JEQ|BPF_X);
b0->stmts = s;
b0->s.k = 0;
gen_not(b0);

return b0;
}

static struct block *
gen_atmfield_code_internal(compiler_state_t *cstate, int atmfield,
bpf_int32 jvalue, bpf_u_int32 jtype, int reverse)
{
struct block *b0;

switch (atmfield) {

case A_VPI:
if (!cstate->is_atm)
bpf_error(cstate, "'vpi' supported only on raw ATM");
if (cstate->off_vpi == OFFSET_NOT_SET)
abort();
b0 = gen_ncmp(cstate, OR_LINKHDR, cstate->off_vpi, BPF_B, 0xffffffff, jtype,
reverse, jvalue);
break;

case A_VCI:
if (!cstate->is_atm)
bpf_error(cstate, "'vci' supported only on raw ATM");
if (cstate->off_vci == OFFSET_NOT_SET)
abort();
b0 = gen_ncmp(cstate, OR_LINKHDR, cstate->off_vci, BPF_H, 0xffffffff, jtype,
reverse, jvalue);
break;

case A_PROTOTYPE:
if (cstate->off_proto == OFFSET_NOT_SET)
abort();
b0 = gen_ncmp(cstate, OR_LINKHDR, cstate->off_proto, BPF_B, 0x0f, jtype,
reverse, jvalue);
break;

case A_MSGTYPE:
if (cstate->off_payload == OFFSET_NOT_SET)
abort();
b0 = gen_ncmp(cstate, OR_LINKHDR, cstate->off_payload + MSG_TYPE_POS, BPF_B,
0xffffffff, jtype, reverse, jvalue);
break;

case A_CALLREFTYPE:
if (!cstate->is_atm)
bpf_error(cstate, "'callref' supported only on raw ATM");
if (cstate->off_proto == OFFSET_NOT_SET)
abort();
b0 = gen_ncmp(cstate, OR_LINKHDR, cstate->off_proto, BPF_B, 0xffffffff,
jtype, reverse, jvalue);
break;

default:
abort();
}
return b0;
}

static struct block *
gen_atmtype_metac(compiler_state_t *cstate)
{
struct block *b0, *b1;

b0 = gen_atmfield_code_internal(cstate, A_VPI, 0, BPF_JEQ, 0);
b1 = gen_atmfield_code_internal(cstate, A_VCI, 1, BPF_JEQ, 0);
gen_and(b0, b1);
return b1;
}

static struct block *
gen_atmtype_sc(compiler_state_t *cstate)
{
struct block *b0, *b1;

b0 = gen_atmfield_code_internal(cstate, A_VPI, 0, BPF_JEQ, 0);
b1 = gen_atmfield_code_internal(cstate, A_VCI, 5, BPF_JEQ, 0);
gen_and(b0, b1);
return b1;
}

static struct block *
gen_atmtype_llc(compiler_state_t *cstate)
{
struct block *b0;

b0 = gen_atmfield_code_internal(cstate, A_PROTOTYPE, PT_LLC, BPF_JEQ, 0);
cstate->linktype = cstate->prevlinktype;
return b0;
}

struct block *
gen_atmfield_code(compiler_state_t *cstate, int atmfield,
bpf_int32 jvalue, bpf_u_int32 jtype, int reverse)
{




if (setjmp(cstate->top_ctx))
return (NULL);

return gen_atmfield_code_internal(cstate, atmfield, jvalue, jtype,
reverse);
}

struct block *
gen_atmtype_abbrev(compiler_state_t *cstate, int type)
{
struct block *b0, *b1;





if (setjmp(cstate->top_ctx))
return (NULL);

switch (type) {

case A_METAC:

if (!cstate->is_atm)
bpf_error(cstate, "'metac' supported only on raw ATM");
b1 = gen_atmtype_metac(cstate);
break;

case A_BCC:

if (!cstate->is_atm)
bpf_error(cstate, "'bcc' supported only on raw ATM");
b0 = gen_atmfield_code_internal(cstate, A_VPI, 0, BPF_JEQ, 0);
b1 = gen_atmfield_code_internal(cstate, A_VCI, 2, BPF_JEQ, 0);
gen_and(b0, b1);
break;

case A_OAMF4SC:

if (!cstate->is_atm)
bpf_error(cstate, "'oam4sc' supported only on raw ATM");
b0 = gen_atmfield_code_internal(cstate, A_VPI, 0, BPF_JEQ, 0);
b1 = gen_atmfield_code_internal(cstate, A_VCI, 3, BPF_JEQ, 0);
gen_and(b0, b1);
break;

case A_OAMF4EC:

if (!cstate->is_atm)
bpf_error(cstate, "'oam4ec' supported only on raw ATM");
b0 = gen_atmfield_code_internal(cstate, A_VPI, 0, BPF_JEQ, 0);
b1 = gen_atmfield_code_internal(cstate, A_VCI, 4, BPF_JEQ, 0);
gen_and(b0, b1);
break;

case A_SC:

if (!cstate->is_atm)
bpf_error(cstate, "'sc' supported only on raw ATM");
b1 = gen_atmtype_sc(cstate);
break;

case A_ILMIC:

if (!cstate->is_atm)
bpf_error(cstate, "'ilmic' supported only on raw ATM");
b0 = gen_atmfield_code_internal(cstate, A_VPI, 0, BPF_JEQ, 0);
b1 = gen_atmfield_code_internal(cstate, A_VCI, 16, BPF_JEQ, 0);
gen_and(b0, b1);
break;

case A_LANE:

if (!cstate->is_atm)
bpf_error(cstate, "'lane' supported only on raw ATM");
b1 = gen_atmfield_code_internal(cstate, A_PROTOTYPE, PT_LANE, BPF_JEQ, 0);









PUSH_LINKHDR(cstate, DLT_EN10MB, 0,
cstate->off_payload + 2,
-1);
cstate->off_linktype.constant_part = cstate->off_linkhdr.constant_part + 12;
cstate->off_linkpl.constant_part = cstate->off_linkhdr.constant_part + 14;
cstate->off_nl = 0;
cstate->off_nl_nosnap = 3;
break;

case A_LLC:

if (!cstate->is_atm)
bpf_error(cstate, "'llc' supported only on raw ATM");
b1 = gen_atmtype_llc(cstate);
break;

default:
abort();
}
return b1;
}








struct block *
gen_mtp2type_abbrev(compiler_state_t *cstate, int type)
{
struct block *b0, *b1;





if (setjmp(cstate->top_ctx))
return (NULL);

switch (type) {

case M_FISU:
if ( (cstate->linktype != DLT_MTP2) &&
(cstate->linktype != DLT_ERF) &&
(cstate->linktype != DLT_MTP2_WITH_PHDR) )
bpf_error(cstate, "'fisu' supported only on MTP2");

b0 = gen_ncmp(cstate, OR_PACKET, cstate->off_li, BPF_B, 0x3f, BPF_JEQ, 0, 0);
break;

case M_LSSU:
if ( (cstate->linktype != DLT_MTP2) &&
(cstate->linktype != DLT_ERF) &&
(cstate->linktype != DLT_MTP2_WITH_PHDR) )
bpf_error(cstate, "'lssu' supported only on MTP2");
b0 = gen_ncmp(cstate, OR_PACKET, cstate->off_li, BPF_B, 0x3f, BPF_JGT, 1, 2);
b1 = gen_ncmp(cstate, OR_PACKET, cstate->off_li, BPF_B, 0x3f, BPF_JGT, 0, 0);
gen_and(b1, b0);
break;

case M_MSU:
if ( (cstate->linktype != DLT_MTP2) &&
(cstate->linktype != DLT_ERF) &&
(cstate->linktype != DLT_MTP2_WITH_PHDR) )
bpf_error(cstate, "'msu' supported only on MTP2");
b0 = gen_ncmp(cstate, OR_PACKET, cstate->off_li, BPF_B, 0x3f, BPF_JGT, 0, 2);
break;

case MH_FISU:
if ( (cstate->linktype != DLT_MTP2) &&
(cstate->linktype != DLT_ERF) &&
(cstate->linktype != DLT_MTP2_WITH_PHDR) )
bpf_error(cstate, "'hfisu' supported only on MTP2_HSL");

b0 = gen_ncmp(cstate, OR_PACKET, cstate->off_li_hsl, BPF_H, 0xff80, BPF_JEQ, 0, 0);
break;

case MH_LSSU:
if ( (cstate->linktype != DLT_MTP2) &&
(cstate->linktype != DLT_ERF) &&
(cstate->linktype != DLT_MTP2_WITH_PHDR) )
bpf_error(cstate, "'hlssu' supported only on MTP2_HSL");
b0 = gen_ncmp(cstate, OR_PACKET, cstate->off_li_hsl, BPF_H, 0xff80, BPF_JGT, 1, 0x0100);
b1 = gen_ncmp(cstate, OR_PACKET, cstate->off_li_hsl, BPF_H, 0xff80, BPF_JGT, 0, 0);
gen_and(b1, b0);
break;

case MH_MSU:
if ( (cstate->linktype != DLT_MTP2) &&
(cstate->linktype != DLT_ERF) &&
(cstate->linktype != DLT_MTP2_WITH_PHDR) )
bpf_error(cstate, "'hmsu' supported only on MTP2_HSL");
b0 = gen_ncmp(cstate, OR_PACKET, cstate->off_li_hsl, BPF_H, 0xff80, BPF_JGT, 0, 0x0100);
break;

default:
abort();
}
return b0;
}






struct block *
gen_mtp3field_code(compiler_state_t *cstate, int mtp3field,
bpf_u_int32 jvalue_arg, bpf_u_int32 jtype, int reverse)
{
volatile bpf_u_int32 jvalue = jvalue_arg;
struct block *b0;
bpf_u_int32 val1 , val2 , val3;
u_int newoff_sio;
u_int newoff_opc;
u_int newoff_dpc;
u_int newoff_sls;





if (setjmp(cstate->top_ctx))
return (NULL);

newoff_sio = cstate->off_sio;
newoff_opc = cstate->off_opc;
newoff_dpc = cstate->off_dpc;
newoff_sls = cstate->off_sls;
switch (mtp3field) {

case MH_SIO:
newoff_sio += 3;


case M_SIO:
if (cstate->off_sio == OFFSET_NOT_SET)
bpf_error(cstate, "'sio' supported only on SS7");

if(jvalue > 255)
bpf_error(cstate, "sio value %u too big; max value = 255",
jvalue);
b0 = gen_ncmp(cstate, OR_PACKET, newoff_sio, BPF_B, 0xffffffff,
(u_int)jtype, reverse, (u_int)jvalue);
break;

case MH_OPC:
newoff_opc += 3;


case M_OPC:
if (cstate->off_opc == OFFSET_NOT_SET)
bpf_error(cstate, "'opc' supported only on SS7");

if (jvalue > 16383)
bpf_error(cstate, "opc value %u too big; max value = 16383",
jvalue);


val1 = jvalue & 0x00003c00;
val1 = val1 >>10;
val2 = jvalue & 0x000003fc;
val2 = val2 <<6;
val3 = jvalue & 0x00000003;
val3 = val3 <<22;
jvalue = val1 + val2 + val3;
b0 = gen_ncmp(cstate, OR_PACKET, newoff_opc, BPF_W, 0x00c0ff0f,
(u_int)jtype, reverse, (u_int)jvalue);
break;

case MH_DPC:
newoff_dpc += 3;


case M_DPC:
if (cstate->off_dpc == OFFSET_NOT_SET)
bpf_error(cstate, "'dpc' supported only on SS7");

if (jvalue > 16383)
bpf_error(cstate, "dpc value %u too big; max value = 16383",
jvalue);


val1 = jvalue & 0x000000ff;
val1 = val1 << 24;
val2 = jvalue & 0x00003f00;
val2 = val2 << 8;
jvalue = val1 + val2;
b0 = gen_ncmp(cstate, OR_PACKET, newoff_dpc, BPF_W, 0xff3f0000,
(u_int)jtype, reverse, (u_int)jvalue);
break;

case MH_SLS:
newoff_sls += 3;


case M_SLS:
if (cstate->off_sls == OFFSET_NOT_SET)
bpf_error(cstate, "'sls' supported only on SS7");

if (jvalue > 15)
bpf_error(cstate, "sls value %u too big; max value = 15",
jvalue);


jvalue = jvalue << 4;
b0 = gen_ncmp(cstate, OR_PACKET, newoff_sls, BPF_B, 0xf0,
(u_int)jtype,reverse, (u_int)jvalue);
break;

default:
abort();
}
return b0;
}

static struct block *
gen_msg_abbrev(compiler_state_t *cstate, int type)
{
struct block *b1;





switch (type) {

case A_SETUP:
b1 = gen_atmfield_code_internal(cstate, A_MSGTYPE, SETUP, BPF_JEQ, 0);
break;

case A_CALLPROCEED:
b1 = gen_atmfield_code_internal(cstate, A_MSGTYPE, CALL_PROCEED, BPF_JEQ, 0);
break;

case A_CONNECT:
b1 = gen_atmfield_code_internal(cstate, A_MSGTYPE, CONNECT, BPF_JEQ, 0);
break;

case A_CONNECTACK:
b1 = gen_atmfield_code_internal(cstate, A_MSGTYPE, CONNECT_ACK, BPF_JEQ, 0);
break;

case A_RELEASE:
b1 = gen_atmfield_code_internal(cstate, A_MSGTYPE, RELEASE, BPF_JEQ, 0);
break;

case A_RELEASE_DONE:
b1 = gen_atmfield_code_internal(cstate, A_MSGTYPE, RELEASE_DONE, BPF_JEQ, 0);
break;

default:
abort();
}
return b1;
}

struct block *
gen_atmmulti_abbrev(compiler_state_t *cstate, int type)
{
struct block *b0, *b1;





if (setjmp(cstate->top_ctx))
return (NULL);

switch (type) {

case A_OAM:
if (!cstate->is_atm)
bpf_error(cstate, "'oam' supported only on raw ATM");

b0 = gen_atmfield_code_internal(cstate, A_VCI, 3, BPF_JEQ, 0);
b1 = gen_atmfield_code_internal(cstate, A_VCI, 4, BPF_JEQ, 0);
gen_or(b0, b1);
b0 = gen_atmfield_code_internal(cstate, A_VPI, 0, BPF_JEQ, 0);
gen_and(b0, b1);
break;

case A_OAMF4:
if (!cstate->is_atm)
bpf_error(cstate, "'oamf4' supported only on raw ATM");

b0 = gen_atmfield_code_internal(cstate, A_VCI, 3, BPF_JEQ, 0);
b1 = gen_atmfield_code_internal(cstate, A_VCI, 4, BPF_JEQ, 0);
gen_or(b0, b1);
b0 = gen_atmfield_code_internal(cstate, A_VPI, 0, BPF_JEQ, 0);
gen_and(b0, b1);
break;

case A_CONNECTMSG:




if (!cstate->is_atm)
bpf_error(cstate, "'connectmsg' supported only on raw ATM");
b0 = gen_msg_abbrev(cstate, A_SETUP);
b1 = gen_msg_abbrev(cstate, A_CALLPROCEED);
gen_or(b0, b1);
b0 = gen_msg_abbrev(cstate, A_CONNECT);
gen_or(b0, b1);
b0 = gen_msg_abbrev(cstate, A_CONNECTACK);
gen_or(b0, b1);
b0 = gen_msg_abbrev(cstate, A_RELEASE);
gen_or(b0, b1);
b0 = gen_msg_abbrev(cstate, A_RELEASE_DONE);
gen_or(b0, b1);
b0 = gen_atmtype_sc(cstate);
gen_and(b0, b1);
break;

case A_METACONNECT:
if (!cstate->is_atm)
bpf_error(cstate, "'metaconnect' supported only on raw ATM");
b0 = gen_msg_abbrev(cstate, A_SETUP);
b1 = gen_msg_abbrev(cstate, A_CALLPROCEED);
gen_or(b0, b1);
b0 = gen_msg_abbrev(cstate, A_CONNECT);
gen_or(b0, b1);
b0 = gen_msg_abbrev(cstate, A_RELEASE);
gen_or(b0, b1);
b0 = gen_msg_abbrev(cstate, A_RELEASE_DONE);
gen_or(b0, b1);
b0 = gen_atmtype_metac(cstate);
gen_and(b0, b1);
break;

default:
abort();
}
return b1;
}
