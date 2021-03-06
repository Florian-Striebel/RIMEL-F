




































#include "mibII.h"
#include "mibII_oid.h"

#define ARPREFRESH 30

struct mibarp *
mib_find_arp(const struct mibif *ifp, struct in_addr in)
{
struct mibarp *at;
uint32_t a = ntohl(in.s_addr);

if (get_ticks() >= mibarpticks + ARPREFRESH)
mib_arp_update();

TAILQ_FOREACH(at, &mibarp_list, link)
if (at->index.subs[0] == ifp->index &&
(at->index.subs[1] == ((a >> 24) & 0xff)) &&
(at->index.subs[2] == ((a >> 16) & 0xff)) &&
(at->index.subs[3] == ((a >> 8) & 0xff)) &&
(at->index.subs[4] == ((a >> 0) & 0xff)))
return (at);
return (NULL);
}

struct mibarp *
mib_arp_create(const struct mibif *ifp, struct in_addr in, const u_char *phys,
size_t physlen)
{
struct mibarp *at;
uint32_t a = ntohl(in.s_addr);

if ((at = malloc(sizeof(*at))) == NULL)
return (NULL);
at->flags = 0;

at->index.len = 5;
at->index.subs[0] = ifp->index;
at->index.subs[1] = (a >> 24) & 0xff;
at->index.subs[2] = (a >> 16) & 0xff;
at->index.subs[3] = (a >> 8) & 0xff;
at->index.subs[4] = (a >> 0) & 0xff;
if ((at->physlen = physlen) > sizeof(at->phys))
at->physlen = sizeof(at->phys);
memcpy(at->phys, phys, at->physlen);

INSERT_OBJECT_OID(at, &mibarp_list);

return (at);
}

void
mib_arp_delete(struct mibarp *at)
{
TAILQ_REMOVE(&mibarp_list, at, link);
free(at);
}

int
op_nettomedia(struct snmp_context *ctx __unused, struct snmp_value *value,
u_int sub, u_int iidx __unused, enum snmp_op op)
{
struct mibarp *at;

at = NULL;

if (get_ticks() >= mibarpticks + ARPREFRESH)
mib_arp_update();

switch (op) {

case SNMP_OP_GETNEXT:
if ((at = NEXT_OBJECT_OID(&mibarp_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
index_append(&value->var, sub, &at->index);
break;

case SNMP_OP_GET:
if ((at = FIND_OBJECT_OID(&mibarp_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
break;

case SNMP_OP_SET:
if ((at = FIND_OBJECT_OID(&mibarp_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NO_CREATION);
return (SNMP_ERR_NOT_WRITEABLE);

case SNMP_OP_ROLLBACK:
case SNMP_OP_COMMIT:
abort();
}

switch (value->var.subs[sub - 1]) {

case LEAF_ipNetToMediaIfIndex:
value->v.integer = at->index.subs[0];
break;

case LEAF_ipNetToMediaPhysAddress:
return (string_get(value, at->phys, at->physlen));

case LEAF_ipNetToMediaNetAddress:
value->v.ipaddress[0] = at->index.subs[1];
value->v.ipaddress[1] = at->index.subs[2];
value->v.ipaddress[2] = at->index.subs[3];
value->v.ipaddress[3] = at->index.subs[4];
break;

case LEAF_ipNetToMediaType:
value->v.integer = (at->flags & MIBARP_PERM) ? 4 : 3;
break;
}
return (SNMP_ERR_NOERROR);
}
