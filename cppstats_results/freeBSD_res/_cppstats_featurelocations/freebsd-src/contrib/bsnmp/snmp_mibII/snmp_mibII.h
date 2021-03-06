































#if !defined(snmp_mibII_h_)
#define snmp_mibII_h_


struct mibif;

enum mibif_notify {
MIBIF_NOTIFY_DESTROY
};

typedef void (*mibif_notify_f)(struct mibif *, enum mibif_notify, void *);









struct mibif {
TAILQ_ENTRY(mibif) link;
u_int flags;
u_int index;
u_int sysindex;
char name[IFNAMSIZ];
char descr[256];
struct ifmibdata mib;
uint64_t mibtick;
void *specmib;
size_t specmiblen;
u_char *physaddr;
u_int physaddrlen;
int has_connector;
int trap_enable;
uint64_t counter_disc;







mibif_notify_f xnotify;
void *xnotify_data;
const struct lmodule *xnotify_mod;


struct asn_oid spec_oid;

char *alias;
size_t alias_size;


void *private;
};




struct mibifa {
TAILQ_ENTRY(mibifa) link;
struct in_addr inaddr;
struct in_addr inmask;
struct in_addr inbcast;
struct asn_oid index;
u_int ifindex;
u_int flags;
};





struct mibrcvaddr {
TAILQ_ENTRY(mibrcvaddr) link;
struct asn_oid index;
u_int ifindex;
u_char addr[ASN_MAXOIDLEN];
size_t addrlen;
u_int flags;
};
enum {
MIBRCVADDR_VOLATILE = 0x00000001,
MIBRCVADDR_BCAST = 0x00000002,
MIBRCVADDR_HW = 0x00000004,
};


extern int mib_netsock;


void mib_if_set_dyn(const char *);


void mib_refresh_iflist(void);


struct mibif *mib_find_if(u_int);
struct mibif *mib_find_if_sys(u_int);
struct mibif *mib_find_if_name(const char *);


struct mibif *mib_first_if(void);
struct mibif *mib_next_if(const struct mibif *);


int mib_register_newif(int (*)(struct mibif *), const struct lmodule *);
void mib_unregister_newif(const struct lmodule *);


int mib_fetch_ifmib(struct mibif *);


int mib_if_admin(struct mibif *, int up);


struct mibifa *mib_find_ifa(struct in_addr);


struct mibifa *mib_first_ififa(const struct mibif *);
struct mibifa *mib_next_ififa(struct mibifa *);


int mib_ifstack_create(const struct mibif *lower, const struct mibif *upper);
void mib_ifstack_delete(const struct mibif *lower, const struct mibif *upper);


struct mibrcvaddr *mib_find_rcvaddr(u_int, const u_char *, size_t);


struct mibrcvaddr *mib_rcvaddr_create(struct mibif *, const u_char *, size_t);
void mib_rcvaddr_delete(struct mibrcvaddr *);


void *mibif_notify(struct mibif *, const struct lmodule *, mibif_notify_f,
void *);
void mibif_unnotify(void *);

#endif
