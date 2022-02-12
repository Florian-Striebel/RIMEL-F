
































#if defined(USE_LIBBEGEMOT)
#include <rpoll.h>
#else
#include <isc/eventlib.h>
#endif

#define PATH_SYSCONFIG "/etc:/usr/etc:/usr/local/etc"

#if defined(USE_LIBBEGEMOT)
#define evTimerID int
#define evFileID int
#endif





struct community {
struct lmodule *owner;
u_int private;
u_int value;
u_char * string;
const u_char * descr;
TAILQ_ENTRY(community) link;

struct asn_oid index;
};

extern TAILQ_HEAD(community_list, community) community_list;





struct idrange {
u_int type;
int32_t base;
int32_t size;
int32_t next;
struct lmodule *owner;
TAILQ_ENTRY(idrange) link;
};


extern TAILQ_HEAD(idrange_list, idrange) idrange_list;


extern u_int next_idrange;


extern u_int trap_reqid;





struct timer {
void (*func)(void *);
void *udata;
evTimerID id;
struct lmodule *owner;
LIST_ENTRY(timer) link;
};


extern LIST_HEAD(timer_list, timer) timer_list;






struct fdesc {
int fd;
void (*func)(int, void *);
void *udata;
evFileID id;
struct lmodule *owner;
LIST_ENTRY(fdesc) link;
};


extern LIST_HEAD(fdesc_list, fdesc) fdesc_list;





#define LM_SECTION_MAX 14
struct lmodule {
char section[LM_SECTION_MAX + 1];
char *path;
u_int flags;
void *handle;
const struct snmp_module *config;

TAILQ_ENTRY(lmodule) link;
TAILQ_ENTRY(lmodule) start;

struct asn_oid index;
};
#define LM_STARTED 0x0001
#define LM_ONSTARTLIST 0x0002

extern TAILQ_HEAD(lmodules, lmodule) lmodules;

struct lmodule *lm_load(const char *, const char *);
void lm_unload(struct lmodule *);
void lm_start(struct lmodule *);








struct port_input {
int fd;
void *id;

int stream : 1;
int cred : 1;

struct sockaddr *peer;
socklen_t peerlen;
int priv : 1;

u_char *buf;
size_t buflen;
size_t length;
size_t consumed;
};

struct tport {
struct asn_oid index;
TAILQ_ENTRY(tport) link;
struct transport *transport;
};
TAILQ_HEAD(tport_list, tport);

int snmpd_input(struct port_input *, struct tport *);
void snmpd_input_close(struct port_input *);
void snmpd_input_init(struct port_input *);




#define TRANS_NAMELEN 64

struct transport_def {
const char *name;
struct asn_oid id;

int (*start)(void);
int (*stop)(int);

void (*close_port)(struct tport *);
int (*init_port)(struct tport *);

ssize_t (*send)(struct tport *, const u_char *, size_t,
const struct sockaddr *, size_t);
ssize_t (*recv)(struct tport *, struct port_input *);


ssize_t (*send2)(struct tport *, const u_char *, size_t,
struct port_input *);
};
struct transport {
struct asn_oid index;
TAILQ_ENTRY(transport) link;
u_int or_index;

struct tport_list table;

const struct transport_def *vtab;
};

TAILQ_HEAD(transport_list, transport);
extern struct transport_list transport_list;

void trans_insert_port(struct transport *, struct tport *);
void trans_remove_port(struct tport *);
struct tport *trans_find_port(struct transport *,
const struct asn_oid *, u_int);
struct tport *trans_next_port(struct transport *,
const struct asn_oid *, u_int);
struct tport *trans_first_port(struct transport *);
struct tport *trans_iter_port(struct transport *,
int (*)(struct tport *, intptr_t), intptr_t);

int trans_register(const struct transport_def *, struct transport **);
int trans_unregister(struct transport *);





struct snmpd {

u_int32_t txbuf;


u_int32_t rxbuf;


int comm_dis;


int auth_traps;


u_char trap1addr[4];


uint32_t version_enable;
};
extern struct snmpd snmpd;

#define VERS_ENABLE_V1 0x00000001
#define VERS_ENABLE_V2C 0x00000002
#define VERS_ENABLE_V3 0x00000004
#define VERS_ENABLE_ALL (VERS_ENABLE_V1 | VERS_ENABLE_V2C | VERS_ENABLE_V3)




struct debug {
u_int dump_pdus;
u_int logpri;
u_int evdebug;
};
extern struct debug debug;





struct snmpd_stats {
u_int32_t inPkts;
u_int32_t inBadVersions;
u_int32_t inASNParseErrs;
u_int32_t inBadCommunityNames;
u_int32_t inBadCommunityUses;
u_int32_t proxyDrops;
u_int32_t silentDrops;

u_int32_t inBadPduTypes;
u_int32_t inTooLong;
u_int32_t noTxbuf;
u_int32_t noRxbuf;
};
extern struct snmpd_stats snmpd_stats;




extern struct snmp_engine snmpd_engine;




struct objres {
TAILQ_ENTRY(objres) link;
u_int index;
struct asn_oid oid;
char descr[256];
u_int32_t uptime;
struct lmodule *module;
};
TAILQ_HEAD(objres_list, objres);
extern struct objres_list objres_list;




struct trapsink {
TAILQ_ENTRY(trapsink) link;
struct asn_oid index;
u_int status;
int socket;
u_char comm[SNMP_COMMUNITY_MAXLEN + 1];
int version;
};
enum {
TRAPSINK_ACTIVE = 1,
TRAPSINK_NOT_IN_SERVICE = 2,
TRAPSINK_NOT_READY = 3,
TRAPSINK_DESTROY = 6,

TRAPSINK_V1 = 1,
TRAPSINK_V2 = 2,
};
TAILQ_HEAD(trapsink_list, trapsink);
extern struct trapsink_list trapsink_list;

extern const char *syspath;


extern int32_t snmp_serial_no;

int init_actvals(void);

extern char engine_file[];
int init_snmpd_engine(void);
int set_snmpd_engine(void);
void update_snmpd_engine_time(void);

int read_config(const char *, struct lmodule *);
int define_macro(const char *name, const char *value);

#define LOG_ASN1_ERRORS 0x10000000
#define LOG_SNMP_ERRORS 0x20000000
