































#include "mibII.h"
#include "mibII_oid.h"




struct ifchange {
struct snmp_dependency dep;

u_int ifindex;

uint32_t set;
int promisc;
int admin;
int traps;

uint32_t rb;
int rb_flags;
int rb_traps;
};
#define IFC_PROMISC 0x0001
#define IFC_ADMIN 0x0002
#define IFC_TRAPS 0x0004
#define IFRB_FLAGS 0x0001
#define IFRB_TRAPS 0x0002

static const struct asn_oid
oid_ifTable = OIDX_ifTable;





static int
ifchange_func(struct snmp_context *ctx __unused, struct snmp_dependency *dep,
enum snmp_depop op)
{
struct ifchange *ifc = (struct ifchange *)dep;
struct mibif *ifp;
struct ifreq ifr, ifr1;

if ((ifp = mib_find_if(ifc->ifindex)) == NULL)
return (SNMP_ERR_NO_CREATION);

switch (op) {

case SNMP_DEPOP_COMMIT:
strlcpy(ifr.ifr_name, ifp->name, sizeof(ifr.ifr_name));
if (ioctl(mib_netsock, SIOCGIFFLAGS, &ifr) == -1) {
syslog(LOG_ERR, "GIFFLAGS(%s): %m", ifp->name);
return (SNMP_ERR_GENERR);
}
if (ifc->set & IFC_PROMISC) {
ifr.ifr_flags &= ~IFF_PROMISC;
if (ifc->promisc)
ifr.ifr_flags |= IFF_PROMISC;
ifc->rb |= IFRB_FLAGS;
}
if (ifc->set & IFC_ADMIN) {
ifr.ifr_flags &= ~IFF_UP;
if (ifc->admin)
ifr.ifr_flags |= IFF_UP;
ifc->rb |= IFRB_FLAGS;
}
if (ifc->rb & IFRB_FLAGS) {
strlcpy(ifr1.ifr_name, ifp->name, sizeof(ifr1.ifr_name));
if (ioctl(mib_netsock, SIOCGIFFLAGS, &ifr1) == -1) {
syslog(LOG_ERR, "GIFFLAGS(%s): %m", ifp->name);
return (SNMP_ERR_GENERR);
}
ifc->rb_flags = ifr1.ifr_flags;
if (ioctl(mib_netsock, SIOCSIFFLAGS, &ifr) == -1) {
syslog(LOG_ERR, "SIFFLAGS(%s): %m", ifp->name);
return (SNMP_ERR_GENERR);
}
(void)mib_fetch_ifmib(ifp);
}
if (ifc->set & IFC_TRAPS) {
ifc->rb |= IFRB_TRAPS;
ifc->rb_traps = ifp->trap_enable;
ifp->trap_enable = ifc->traps;
}
return (SNMP_ERR_NOERROR);

case SNMP_DEPOP_ROLLBACK:
if (ifc->rb & IFRB_FLAGS) {
strlcpy(ifr.ifr_name, ifp->name, sizeof(ifr.ifr_name));
ifr.ifr_flags = ifc->rb_flags;
if (ioctl(mib_netsock, SIOCSIFFLAGS, &ifr) == -1) {
syslog(LOG_ERR, "SIFFLAGS(%s): %m", ifp->name);
return (SNMP_ERR_UNDO_FAILED);
}
(void)mib_fetch_ifmib(ifp);
}
if (ifc->rb & IFRB_TRAPS)
ifp->trap_enable = ifc->rb_traps;
return (SNMP_ERR_NOERROR);

case SNMP_DEPOP_FINISH:
return (SNMP_ERR_NOERROR);

}
abort();
}





static uint32_t
ticks_get_timeval(struct timeval *tv)
{
uint64_t v;

if (tv->tv_sec != 0 || tv->tv_usec != 0) {
v = 100ULL * tv->tv_sec + tv->tv_usec / 10000ULL;
if (v > start_tick)
return (v - start_tick);
}
return (0);
}




int
op_interfaces(struct snmp_context *ctx __unused, struct snmp_value *value,
u_int sub, u_int idx __unused, enum snmp_op op)
{
switch (op) {

case SNMP_OP_GETNEXT:
abort();

case SNMP_OP_GET:
break;

case SNMP_OP_SET:
return (SNMP_ERR_NOT_WRITEABLE);

case SNMP_OP_ROLLBACK:
case SNMP_OP_COMMIT:
abort();
}

switch (value->var.subs[sub - 1]) {

case LEAF_ifNumber:
value->v.integer = mib_if_number;
break;
}
return (SNMP_ERR_NOERROR);
}




int
op_ifentry(struct snmp_context *ctx, struct snmp_value *value,
u_int sub, u_int iidx __unused, enum snmp_op op)
{
struct mibif *ifp = NULL;
int ret;
struct ifchange *ifc;
struct asn_oid idx;

switch (op) {

case SNMP_OP_GETNEXT:
if ((ifp = NEXT_OBJECT_INT(&mibif_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
value->var.len = sub + 1;
value->var.subs[sub] = ifp->index;
break;

case SNMP_OP_GET:
if (value->var.len - sub != 1)
return (SNMP_ERR_NOSUCHNAME);
if ((ifp = mib_find_if(value->var.subs[sub])) == NULL)
return (SNMP_ERR_NOSUCHNAME);
break;

case SNMP_OP_SET:
if (value->var.len - sub != 1)
return (SNMP_ERR_NO_CREATION);
if ((ifp = mib_find_if(value->var.subs[sub])) == NULL)
return (SNMP_ERR_NO_CREATION);
if (value->var.subs[sub - 1] != LEAF_ifAdminStatus)
return (SNMP_ERR_NOT_WRITEABLE);

idx.len = 1;
idx.subs[0] = ifp->index;

if (value->v.integer != 1 && value->v.integer != 2)
return (SNMP_ERR_WRONG_VALUE);

if ((ifc = (struct ifchange *)snmp_dep_lookup(ctx,
&oid_ifTable, &idx, sizeof(*ifc), ifchange_func)) == NULL)
return (SNMP_ERR_RES_UNAVAIL);
ifc->ifindex = ifp->index;

if (ifc->set & IFC_ADMIN)
return (SNMP_ERR_INCONS_VALUE);
ifc->set |= IFC_ADMIN;
ifc->admin = (value->v.integer == 1) ? 1 : 0;

return (SNMP_ERR_NOERROR);

case SNMP_OP_ROLLBACK:
case SNMP_OP_COMMIT:
return (SNMP_ERR_NOERROR);
}

if (ifp->mibtick < this_tick)
(void)mib_fetch_ifmib(ifp);

ret = SNMP_ERR_NOERROR;
switch (value->var.subs[sub - 1]) {

case LEAF_ifIndex:
value->v.integer = ifp->index;
break;

case LEAF_ifDescr:
ret = string_get(value, ifp->descr, -1);
break;

case LEAF_ifType:
value->v.integer = ifp->mib.ifmd_data.ifi_type;
break;

case LEAF_ifMtu:
value->v.integer = ifp->mib.ifmd_data.ifi_mtu;
break;

case LEAF_ifSpeed:
value->v.integer = ifp->mib.ifmd_data.ifi_baudrate;
break;

case LEAF_ifPhysAddress:
ret = string_get(value, ifp->physaddr,
ifp->physaddrlen);
break;

case LEAF_ifAdminStatus:
value->v.integer =
(ifp->mib.ifmd_flags & IFF_UP) ? 1 : 2;
break;

case LEAF_ifOperStatus:








if (ifp->mib.ifmd_flags & IFF_RUNNING) {
if (ifp->mib.ifmd_data.ifi_link_state != LINK_STATE_UP)
value->v.integer = 5;
else
value->v.integer = 1;
} else
value->v.integer = 2;
break;

case LEAF_ifLastChange:
value->v.uint32 =
ticks_get_timeval(&ifp->mib.ifmd_data.ifi_lastchange);
break;

case LEAF_ifInOctets:
value->v.uint32 = ifp->mib.ifmd_data.ifi_ibytes;
break;

case LEAF_ifInUcastPkts:
value->v.uint32 = ifp->mib.ifmd_data.ifi_ipackets -
ifp->mib.ifmd_data.ifi_imcasts;
break;

case LEAF_ifInNUcastPkts:
value->v.uint32 = ifp->mib.ifmd_data.ifi_imcasts;
break;

case LEAF_ifInDiscards:
value->v.uint32 = ifp->mib.ifmd_data.ifi_iqdrops;
break;

case LEAF_ifInErrors:
value->v.uint32 = ifp->mib.ifmd_data.ifi_ierrors;
break;

case LEAF_ifInUnknownProtos:
value->v.uint32 = ifp->mib.ifmd_data.ifi_noproto;
break;

case LEAF_ifOutOctets:
value->v.uint32 = ifp->mib.ifmd_data.ifi_obytes;
break;

case LEAF_ifOutUcastPkts:
value->v.uint32 = ifp->mib.ifmd_data.ifi_opackets -
ifp->mib.ifmd_data.ifi_omcasts;
break;

case LEAF_ifOutNUcastPkts:
value->v.uint32 = ifp->mib.ifmd_data.ifi_omcasts;
break;

case LEAF_ifOutDiscards:
value->v.uint32 = ifp->mib.ifmd_snd_drops;
break;

case LEAF_ifOutErrors:
value->v.uint32 = ifp->mib.ifmd_data.ifi_oerrors;
break;

case LEAF_ifOutQLen:
value->v.uint32 = ifp->mib.ifmd_snd_len;
break;

case LEAF_ifSpecific:
value->v.oid = ifp->spec_oid;
break;
}
return (ret);
}




int
op_ifxtable(struct snmp_context *ctx, struct snmp_value *value,
u_int sub, u_int iidx __unused, enum snmp_op op)
{
struct mibif *ifp = NULL;
int ret;
struct ifchange *ifc;
struct asn_oid idx;

switch (op) {

case SNMP_OP_GETNEXT:
if ((ifp = NEXT_OBJECT_INT(&mibif_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
value->var.len = sub + 1;
value->var.subs[sub] = ifp->index;
break;

case SNMP_OP_GET:
if (value->var.len - sub != 1)
return (SNMP_ERR_NOSUCHNAME);
if ((ifp = mib_find_if(value->var.subs[sub])) == NULL)
return (SNMP_ERR_NOSUCHNAME);
break;

case SNMP_OP_SET:
if (value->var.len - sub != 1)
return (SNMP_ERR_NO_CREATION);
if ((ifp = mib_find_if(value->var.subs[sub])) == NULL)
return (SNMP_ERR_NO_CREATION);

idx.len = 1;
idx.subs[0] = ifp->index;

if ((ifc = (struct ifchange *)snmp_dep_lookup(ctx,
&oid_ifTable, &idx, sizeof(*ifc), ifchange_func)) == NULL)
return (SNMP_ERR_RES_UNAVAIL);
ifc->ifindex = ifp->index;

switch (value->var.subs[sub - 1]) {

case LEAF_ifLinkUpDownTrapEnable:
if (value->v.integer != 1 && value->v.integer != 2)
return (SNMP_ERR_WRONG_VALUE);
if (ifc->set & IFC_TRAPS)
return (SNMP_ERR_INCONS_VALUE);
ifc->set |= IFC_TRAPS;
ifc->traps = (value->v.integer == 1) ? 1 : 0;
return (SNMP_ERR_NOERROR);

case LEAF_ifPromiscuousMode:
if (value->v.integer != 1 && value->v.integer != 2)
return (SNMP_ERR_WRONG_VALUE);
if (ifc->set & IFC_PROMISC)
return (SNMP_ERR_INCONS_VALUE);
ifc->set |= IFC_PROMISC;
ifc->promisc = (value->v.integer == 1) ? 1 : 0;
return (SNMP_ERR_NOERROR);
}
return (SNMP_ERR_NOT_WRITEABLE);

case SNMP_OP_ROLLBACK:
case SNMP_OP_COMMIT:
return (SNMP_ERR_NOERROR);
}

if (ifp->mibtick < this_tick)
(void)mib_fetch_ifmib(ifp);

ret = SNMP_ERR_NOERROR;
switch (value->var.subs[sub - 1]) {

case LEAF_ifName:
ret = string_get(value, ifp->name, -1);
break;

case LEAF_ifInMulticastPkts:
value->v.uint32 = ifp->mib.ifmd_data.ifi_imcasts;
break;

case LEAF_ifInBroadcastPkts:
value->v.uint32 = 0;
break;

case LEAF_ifOutMulticastPkts:
value->v.uint32 = ifp->mib.ifmd_data.ifi_omcasts;
break;

case LEAF_ifOutBroadcastPkts:
value->v.uint32 = 0;
break;

case LEAF_ifHCInOctets:
value->v.counter64 = MIBIF_PRIV(ifp)->hc_inoctets;
break;

case LEAF_ifHCInUcastPkts:
value->v.counter64 = MIBIF_PRIV(ifp)->hc_ipackets -
MIBIF_PRIV(ifp)->hc_imcasts;
break;

case LEAF_ifHCInMulticastPkts:
value->v.counter64 = MIBIF_PRIV(ifp)->hc_imcasts;
break;

case LEAF_ifHCInBroadcastPkts:
value->v.counter64 = 0;
break;

case LEAF_ifHCOutOctets:
value->v.counter64 = MIBIF_PRIV(ifp)->hc_outoctets;
break;

case LEAF_ifHCOutUcastPkts:
value->v.counter64 = MIBIF_PRIV(ifp)->hc_opackets -
MIBIF_PRIV(ifp)->hc_omcasts;
break;

case LEAF_ifHCOutMulticastPkts:
value->v.counter64 = MIBIF_PRIV(ifp)->hc_omcasts;
break;

case LEAF_ifHCOutBroadcastPkts:
value->v.counter64 = 0;
break;

case LEAF_ifLinkUpDownTrapEnable:
value->v.integer = ifp->trap_enable ? 1 : 2;
break;

case LEAF_ifHighSpeed:
value->v.integer =
(ifp->mib.ifmd_data.ifi_baudrate + 499999) / 1000000;
break;

case LEAF_ifPromiscuousMode:
value->v.integer =
(ifp->mib.ifmd_flags & IFF_PROMISC) ? 1 : 2;
break;

case LEAF_ifConnectorPresent:
value->v.integer = ifp->has_connector ? 1 : 2;
break;

case LEAF_ifAlias:
ret = string_get(value, ifp->alias, ifp->alias_size - 1);
break;

case LEAF_ifCounterDiscontinuityTime:
if (ifp->counter_disc > start_tick)
value->v.uint32 = ifp->counter_disc - start_tick;
else
value->v.uint32 = 0;
break;
}
return (ret);
}
