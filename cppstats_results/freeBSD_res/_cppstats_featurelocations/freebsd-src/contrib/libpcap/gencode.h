




















#include "pcap/funcattrs.h"





































#define Q_HOST 1
#define Q_NET 2
#define Q_PORT 3
#define Q_GATEWAY 4
#define Q_PROTO 5
#define Q_PROTOCHAIN 6
#define Q_PORTRANGE 7



#define Q_LINK 1
#define Q_IP 2
#define Q_ARP 3
#define Q_RARP 4
#define Q_SCTP 5
#define Q_TCP 6
#define Q_UDP 7
#define Q_ICMP 8
#define Q_IGMP 9
#define Q_IGRP 10


#define Q_ATALK 11
#define Q_DECNET 12
#define Q_LAT 13
#define Q_SCA 14
#define Q_MOPRC 15
#define Q_MOPDL 16


#define Q_IPV6 17
#define Q_ICMPV6 18
#define Q_AH 19
#define Q_ESP 20

#define Q_PIM 21
#define Q_VRRP 22

#define Q_AARP 23

#define Q_ISO 24
#define Q_ESIS 25
#define Q_ISIS 26
#define Q_CLNP 27

#define Q_STP 28

#define Q_IPX 29

#define Q_NETBEUI 30


#define Q_ISIS_L1 31
#define Q_ISIS_L2 32

#define Q_ISIS_IIH 33
#define Q_ISIS_SNP 34
#define Q_ISIS_CSNP 35
#define Q_ISIS_PSNP 36
#define Q_ISIS_LSP 37

#define Q_RADIO 38

#define Q_CARP 39



#define Q_SRC 1
#define Q_DST 2
#define Q_OR 3
#define Q_AND 4
#define Q_ADDR1 5
#define Q_ADDR2 6
#define Q_ADDR3 7
#define Q_ADDR4 8
#define Q_RA 9
#define Q_TA 10

#define Q_DEFAULT 0
#define Q_UNDEF 255


#define A_METAC 22
#define A_BCC 23
#define A_OAMF4SC 24
#define A_OAMF4EC 25
#define A_SC 26
#define A_ILMIC 27
#define A_OAM 28
#define A_OAMF4 29
#define A_LANE 30
#define A_LLC 31


#define A_SETUP 41
#define A_CALLPROCEED 42
#define A_CONNECT 43
#define A_CONNECTACK 44
#define A_RELEASE 45
#define A_RELEASE_DONE 46


#define A_VPI 51
#define A_VCI 52
#define A_PROTOTYPE 53
#define A_MSGTYPE 54
#define A_CALLREFTYPE 55

#define A_CONNECTMSG 70


#define A_METACONNECT 71







#define M_FISU 22
#define M_LSSU 23
#define M_MSU 24


#define MH_FISU 25
#define MH_LSSU 26
#define MH_MSU 27


#define M_SIO 1
#define M_OPC 2
#define M_DPC 3
#define M_SLS 4


#define MH_SIO 5
#define MH_OPC 6
#define MH_DPC 7
#define MH_SLS 8


struct slist;

struct stmt {
int code;
struct slist *jt;
struct slist *jf;
bpf_int32 k;
};

struct slist {
struct stmt s;
struct slist *next;
};





typedef bpf_u_int32 atomset;
#define ATOMMASK(n) (1 << (n))
#define ATOMELEM(d, n) (d & ATOMMASK(n))




typedef bpf_u_int32 *uset;





#define N_ATOMS (BPF_MEMWORDS+2)

struct edge {
int id;
int code;
uset edom;
struct block *succ;
struct block *pred;
struct edge *next;
};

struct block {
int id;
struct slist *stmts;
struct stmt s;
int mark;
u_int longjt;
u_int longjf;
int level;
int offset;
int sense;
struct edge et;
struct edge ef;
struct block *head;
struct block *link;
uset dom;
uset closure;
struct edge *in_edges;
atomset def, kill;
atomset in_use;
atomset out_use;
int oval;
int val[N_ATOMS];
};




#define VAL_UNKNOWN 0

struct arth {
struct block *b;
struct slist *s;
int regno;
};

struct qual {
unsigned char addr;
unsigned char proto;
unsigned char dir;
unsigned char pad;
};

struct _compiler_state;

typedef struct _compiler_state compiler_state_t;

struct arth *gen_loadi(compiler_state_t *, int);
struct arth *gen_load(compiler_state_t *, int, struct arth *, int);
struct arth *gen_loadlen(compiler_state_t *);
struct arth *gen_neg(compiler_state_t *, struct arth *);
struct arth *gen_arth(compiler_state_t *, int, struct arth *, struct arth *);

void gen_and(struct block *, struct block *);
void gen_or(struct block *, struct block *);
void gen_not(struct block *);

struct block *gen_scode(compiler_state_t *, const char *, struct qual);
struct block *gen_ecode(compiler_state_t *, const char *, struct qual);
struct block *gen_acode(compiler_state_t *, const char *, struct qual);
struct block *gen_mcode(compiler_state_t *, const char *, const char *,
unsigned int, struct qual);
#if defined(INET6)
struct block *gen_mcode6(compiler_state_t *, const char *, const char *,
unsigned int, struct qual);
#endif
struct block *gen_ncode(compiler_state_t *, const char *, bpf_u_int32,
struct qual);
struct block *gen_proto_abbrev(compiler_state_t *, int);
struct block *gen_relation(compiler_state_t *, int, struct arth *,
struct arth *, int);
struct block *gen_less(compiler_state_t *, int);
struct block *gen_greater(compiler_state_t *, int);
struct block *gen_byteop(compiler_state_t *, int, int, int);
struct block *gen_broadcast(compiler_state_t *, int);
struct block *gen_multicast(compiler_state_t *, int);
struct block *gen_inbound(compiler_state_t *, int);

struct block *gen_llc(compiler_state_t *);
struct block *gen_llc_i(compiler_state_t *);
struct block *gen_llc_s(compiler_state_t *);
struct block *gen_llc_u(compiler_state_t *);
struct block *gen_llc_s_subtype(compiler_state_t *, bpf_u_int32);
struct block *gen_llc_u_subtype(compiler_state_t *, bpf_u_int32);

struct block *gen_vlan(compiler_state_t *, bpf_u_int32, int);
struct block *gen_mpls(compiler_state_t *, bpf_u_int32, int);

struct block *gen_pppoed(compiler_state_t *);
struct block *gen_pppoes(compiler_state_t *, bpf_u_int32, int);

struct block *gen_geneve(compiler_state_t *, bpf_u_int32, int);

struct block *gen_atmfield_code(compiler_state_t *, int, bpf_int32,
bpf_u_int32, int);
struct block *gen_atmtype_abbrev(compiler_state_t *, int type);
struct block *gen_atmmulti_abbrev(compiler_state_t *, int type);

struct block *gen_mtp2type_abbrev(compiler_state_t *, int type);
struct block *gen_mtp3field_code(compiler_state_t *, int, bpf_u_int32,
bpf_u_int32, int);

struct block *gen_pf_ifname(compiler_state_t *, const char *);
struct block *gen_pf_rnr(compiler_state_t *, int);
struct block *gen_pf_srnr(compiler_state_t *, int);
struct block *gen_pf_ruleset(compiler_state_t *, char *);
struct block *gen_pf_reason(compiler_state_t *, int);
struct block *gen_pf_action(compiler_state_t *, int);

struct block *gen_p80211_type(compiler_state_t *, int, int);
struct block *gen_p80211_fcdir(compiler_state_t *, int);







#define isMarked(icp, p) ((p)->mark == (icp)->cur_mark)
#define unMarkAll(icp) (icp)->cur_mark += 1
#define Mark(icp, p) ((p)->mark = (icp)->cur_mark)

struct icode {
struct block *root;
int cur_mark;
};

int bpf_optimize(struct icode *, char *);
void bpf_set_error(compiler_state_t *, const char *, ...)
PCAP_PRINTFLIKE(2, 3);

int finish_parse(compiler_state_t *, struct block *);
char *sdup(compiler_state_t *, const char *);

struct bpf_insn *icode_to_fcode(struct icode *, struct block *, u_int *,
char *);
void sappend(struct slist *, struct slist *);





int pcap_parse(void *, compiler_state_t *);


#define JT(b) ((b)->et.succ)
#define JF(b) ((b)->ef.succ)
