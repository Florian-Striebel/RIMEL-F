































#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/syslog.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <err.h>
#include <ctype.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_mib.h>
#include <net/route.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "asn1.h"
#include "snmp.h"
#include "snmpmod.h"
#include "snmp_mibII.h"
#include "mibII_tree.h"


#define MIBIF_ALIAS_SIZE (64 + 1)
#define MIBIF_ALIAS_SIZE_MAX 1024




TAILQ_HEAD(mibif_list, mibif);
enum {
MIBIF_FOUND = 0x0001,
MIBIF_HIGHSPEED = 0x0002,
MIBIF_VERYHIGHSPEED = 0x0004,
};




struct mibif_private {
uint64_t hc_inoctets;
uint64_t hc_outoctets;
uint64_t hc_omcasts;
uint64_t hc_opackets;
uint64_t hc_imcasts;
uint64_t hc_ipackets;
};
#define MIBIF_PRIV(IFP) ((struct mibif_private *)((IFP)->private))




TAILQ_HEAD(mibifa_list, mibifa);
enum {
MIBIFA_FOUND = 0x0001,
MIBIFA_DESTROYED = 0x0002,
};




TAILQ_HEAD(mibrcvaddr_list, mibrcvaddr);
enum {
MIBRCVADDR_FOUND = 0x00010000,
};












struct mibdynif {
SLIST_ENTRY(mibdynif) link;
char name[IFNAMSIZ];
};
SLIST_HEAD(mibdynif_list, mibdynif);

struct mibindexmap {
STAILQ_ENTRY(mibindexmap) link;
u_short sysindex;
u_int ifindex;
struct mibif *mibif;
char name[IFNAMSIZ];
};
STAILQ_HEAD(mibindexmap_list, mibindexmap);







struct mibifstack {
TAILQ_ENTRY(mibifstack) link;
struct asn_oid index;
};
TAILQ_HEAD(mibifstack_list, mibifstack);




struct mibarp {
TAILQ_ENTRY(mibarp) link;
struct asn_oid index;
u_char phys[128];
u_int physlen;
u_int flags;
};
TAILQ_HEAD(mibarp_list, mibarp);
enum {
MIBARP_FOUND = 0x00010000,
MIBARP_PERM = 0x00000001,
};




struct newifreg {
TAILQ_ENTRY(newifreg) link;
const struct lmodule *mod;
int (*func)(struct mibif *);
};
TAILQ_HEAD(newifreg_list, newifreg);


extern struct mibifa_list mibifa_list;


extern struct mibif_list mibif_list;


extern struct mibdynif_list mibdynif_list;


extern struct mibindexmap_list mibindexmap_list;


extern struct mibifstack_list mibifstack_list;


extern struct mibrcvaddr_list mibrcvaddr_list;


extern struct mibarp_list mibarp_list;


extern int32_t mib_if_number;


extern uint64_t mib_iftable_last_change;


extern uint64_t mib_ifstack_last_change;


extern int mib_iflist_bad;


extern uint64_t mibarpticks;


extern struct clockinfo clockinfo;


extern uint64_t mibif_maxspeed;


extern u_int mibif_force_hc_update_interval;


extern u_int mibif_hc_update_interval;


void mibif_reset_hc_timer(void);


extern u_int mibII_poll_ticks;


void mibif_restart_mibII_poll_timer(void);

#define MIBII_POLL_TICKS 100


void mib_fetch_interfaces(void);


int mib_if_is_dyn(const char *name);


int mib_destroy_ifa(struct mibifa *);


void mib_undestroy_ifa(struct mibifa *);


int mib_modify_ifa(struct mibifa *);


void mib_unmodify_ifa(struct mibifa *);


struct mibifa * mib_create_ifa(u_int ifindex, struct in_addr addr, struct in_addr mask, struct in_addr bcast);


void mib_uncreate_ifa(struct mibifa *);


struct mibarp *mib_arp_create(const struct mibif *, struct in_addr, const u_char *, size_t);
void mib_arp_delete(struct mibarp *);


struct mibarp *mib_find_arp(const struct mibif *, struct in_addr);


void mib_arp_update(void);


u_char *mib_fetch_rtab(int af, int info, int arg, size_t *lenp);


void mib_sroute_process(struct rt_msghdr *, struct sockaddr *,
struct sockaddr *, struct sockaddr *);


void mib_send_rtmsg(struct rt_msghdr *, struct sockaddr *,
struct sockaddr *, struct sockaddr *);


void mib_extract_addrs(int, u_char *, struct sockaddr **);


int mib_fetch_route(void);
