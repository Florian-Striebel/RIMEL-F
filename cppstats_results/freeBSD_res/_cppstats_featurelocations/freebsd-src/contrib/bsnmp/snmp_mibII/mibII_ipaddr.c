


































#include "mibII.h"
#include "mibII_oid.h"

static const struct asn_oid
oid_ipAddrTable = OIDX_ipAddrTable;





struct update {
struct snmp_dependency dep;

uint32_t set;
struct in_addr addr;
struct in_addr mask;
int bcast;
u_int ifindex;

uint32_t rb;
struct in_addr rb_mask;
struct in_addr rb_bcast;
};
#define UPD_IFINDEX 0x0001
#define UPD_MASK 0x0002
#define UPD_BCAST 0x0004
#define RB_CREATE 0x0001
#define RB_DESTROY 0x0002
#define RB_MODIFY 0x0004




static int
create(struct update *upd)
{
struct in_addr bcast;
struct mibifa *ifa;

if (!(upd->set & UPD_MASK)) {
if (IN_CLASSA(ntohl(upd->addr.s_addr)))
upd->mask.s_addr = htonl(IN_CLASSA_NET);
else if (IN_CLASSB(ntohl(upd->addr.s_addr)))
upd->mask.s_addr = htonl(IN_CLASSB_NET);
else if (IN_CLASSC(ntohl(upd->addr.s_addr)))
upd->mask.s_addr = htonl(IN_CLASSC_NET);
else
upd->mask.s_addr = 0xffffffff;
}

bcast.s_addr = upd->addr.s_addr & upd->mask.s_addr;
if (!(upd->set & UPD_BCAST) || upd->bcast) {
uint32_t tmp = ~ntohl(upd->mask.s_addr);
bcast.s_addr |= htonl(0xffffffff & ~tmp);
}

if ((ifa = mib_create_ifa(upd->ifindex, upd->addr, upd->mask, bcast))
== NULL)
return (SNMP_ERR_GENERR);

upd->rb |= RB_CREATE;
return (SNMP_ERR_NOERROR);
}





static int
modify(struct update *upd, struct mibifa *ifa)
{
struct mibif *ifp;

if ((ifp = mib_find_if(ifa->ifindex)) == NULL)
return (SNMP_ERR_WRONG_VALUE);
if ((upd->set & UPD_IFINDEX) && upd->ifindex != ifa->ifindex)
return (SNMP_ERR_INCONS_VALUE);

upd->rb_mask = ifa->inmask;
upd->rb_bcast = ifa->inbcast;
if (((upd->set & UPD_MASK) && upd->mask.s_addr != ifa->inmask.s_addr) ||
(upd->set & UPD_BCAST)) {
if (upd->set & UPD_MASK)
ifa->inmask = upd->mask;
if (upd->set & UPD_BCAST) {
ifa->inbcast.s_addr = ifa->inaddr.s_addr
& ifa->inmask.s_addr;
if (upd->bcast)
ifa->inbcast.s_addr |= 0xffffffff
& ~ifa->inmask.s_addr;
}
if (mib_modify_ifa(ifa)) {
syslog(LOG_ERR, "set netmask/bcast: %m");
ifa->inmask = upd->rb_mask;
ifa->inbcast = upd->rb_bcast;
mib_unmodify_ifa(ifa);
return (SNMP_ERR_GENERR);
}
upd->rb |= RB_MODIFY;
}
return (SNMP_ERR_NOERROR);
}






static int
destroy(struct snmp_context *ctx __unused, struct update *upd,
struct mibifa *ifa)
{
if (mib_destroy_ifa(ifa))
return (SNMP_ERR_GENERR);
upd->rb |= RB_DESTROY;
return (SNMP_ERR_NOERROR);
}




static int
update_func(struct snmp_context *ctx, struct snmp_dependency *dep,
enum snmp_depop op)
{
struct update *upd = (struct update *)dep;
struct mibifa *ifa;

switch (op) {

case SNMP_DEPOP_COMMIT:
if ((ifa = mib_find_ifa(upd->addr)) == NULL) {

if (!(upd->set & UPD_IFINDEX))
return (SNMP_ERR_INCONS_NAME);
return (create(upd));
}

if ((upd->set & UPD_IFINDEX) && upd->ifindex == 0) {

return (destroy(ctx, upd, ifa));
}

return (modify(upd, ifa));

case SNMP_DEPOP_ROLLBACK:
if ((ifa = mib_find_ifa(upd->addr)) == NULL) {

mib_iflist_bad = 1;
return (SNMP_ERR_NOERROR);
}
if (upd->rb & RB_CREATE) {
mib_uncreate_ifa(ifa);
return (SNMP_ERR_NOERROR);
}
if (upd->rb & RB_DESTROY) {
mib_undestroy_ifa(ifa);
return (SNMP_ERR_NOERROR);
}
if (upd->rb & RB_MODIFY) {
ifa->inmask = upd->rb_mask;
ifa->inbcast = upd->rb_bcast;
mib_unmodify_ifa(ifa);
return (SNMP_ERR_NOERROR);
}
return (SNMP_ERR_NOERROR);

case SNMP_DEPOP_FINISH:
if ((upd->rb & RB_DESTROY) &&
(ifa = mib_find_ifa(upd->addr)) != NULL &&
(ifa->flags & MIBIFA_DESTROYED)) {
TAILQ_REMOVE(&mibifa_list, ifa, link);
free(ifa);
}
return (SNMP_ERR_NOERROR);
}
abort();
}





int
op_ipaddr(struct snmp_context *ctx, struct snmp_value *value,
u_int sub, u_int iidx, enum snmp_op op)
{
asn_subid_t which;
struct mibifa *ifa;
struct update *upd;
struct asn_oid idx;
u_char ipaddr[4];

which = value->var.subs[sub - 1];

ifa = NULL;

switch (op) {

case SNMP_OP_GETNEXT:
if ((ifa = NEXT_OBJECT_OID(&mibifa_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
index_append(&value->var, sub, &ifa->index);
break;

case SNMP_OP_GET:
if ((ifa = FIND_OBJECT_OID(&mibifa_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
break;

case SNMP_OP_SET:
if (index_decode(&value->var, sub, iidx, ipaddr))
return (SNMP_ERR_NO_CREATION);
ifa = FIND_OBJECT_OID(&mibifa_list, &value->var, sub);
idx.len = 4;
idx.subs[0] = ipaddr[0];
idx.subs[1] = ipaddr[1];
idx.subs[2] = ipaddr[2];
idx.subs[3] = ipaddr[3];

if ((upd = (struct update *)snmp_dep_lookup(ctx,
&oid_ipAddrTable, &idx, sizeof(*upd), update_func)) == NULL)
return (SNMP_ERR_RES_UNAVAIL);

upd->addr.s_addr = htonl((ipaddr[0] << 24) | (ipaddr[1] << 16) |
(ipaddr[2] << 8) | (ipaddr[3] << 0));

switch (which) {

case LEAF_ipAdEntIfIndex:
if (upd->set & UPD_IFINDEX)
return (SNMP_ERR_INCONS_VALUE);
if (value->v.integer < 0 ||
value->v.integer > 0x07fffffff)
return (SNMP_ERR_WRONG_VALUE);
if (ifa != NULL) {
if (ifa->ifindex != (u_int)value->v.integer &&
value->v.integer != 0)
return (SNMP_ERR_INCONS_VALUE);
}
upd->set |= UPD_IFINDEX;
upd->ifindex = (u_int)value->v.integer;
break;

case LEAF_ipAdEntNetMask:
if (upd->set & UPD_MASK)
return (SNMP_ERR_INCONS_VALUE);
upd->mask.s_addr = htonl((value->v.ipaddress[0] << 24)
| (value->v.ipaddress[1] << 16)
| (value->v.ipaddress[2] << 8)
| (value->v.ipaddress[3] << 0));
upd->set |= UPD_MASK;
break;

case LEAF_ipAdEntBcastAddr:
if (upd->set & UPD_BCAST)
return (SNMP_ERR_INCONS_VALUE);
if (value->v.integer != 0 && value->v.integer != 1)
return (SNMP_ERR_WRONG_VALUE);
upd->bcast = value->v.integer;
upd->set |= UPD_BCAST;
break;

}
return (SNMP_ERR_NOERROR);

case SNMP_OP_ROLLBACK:
case SNMP_OP_COMMIT:
return (SNMP_ERR_NOERROR);
}

switch (which) {

case LEAF_ipAdEntAddr:
value->v.ipaddress[0] = ifa->index.subs[0];
value->v.ipaddress[1] = ifa->index.subs[1];
value->v.ipaddress[2] = ifa->index.subs[2];
value->v.ipaddress[3] = ifa->index.subs[3];
break;

case LEAF_ipAdEntIfIndex:
if (ifa->flags & MIBIFA_DESTROYED)
value->v.integer = 0;
else
value->v.integer = ifa->ifindex;
break;

case LEAF_ipAdEntNetMask:
value->v.ipaddress[0] = (ntohl(ifa->inmask.s_addr) >> 24) & 0xff;
value->v.ipaddress[1] = (ntohl(ifa->inmask.s_addr) >> 16) & 0xff;
value->v.ipaddress[2] = (ntohl(ifa->inmask.s_addr) >> 8) & 0xff;
value->v.ipaddress[3] = (ntohl(ifa->inmask.s_addr) >> 0) & 0xff;
break;

case LEAF_ipAdEntBcastAddr:
value->v.integer = ntohl(ifa->inbcast.s_addr) & 1;
break;


case LEAF_ipAdEntReasmMaxSize:
value->v.integer = 65535;
break;
}
return (SNMP_ERR_NOERROR);
}
