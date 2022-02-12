































#include "mibII.h"
#include "mibII_oid.h"




struct mibrcvaddr *
mib_find_rcvaddr(u_int ifindex, const u_char *addr, size_t addrlen)
{
struct mibrcvaddr *rcv;

TAILQ_FOREACH(rcv, &mibrcvaddr_list, link)
if (rcv->ifindex == ifindex &&
rcv->addrlen == addrlen &&
memcmp(rcv->addr, addr, addrlen) == 0)
return (rcv);
return (NULL);
}




struct mibrcvaddr *
mib_rcvaddr_create(struct mibif *ifp, const u_char *addr, size_t addrlen)
{
struct mibrcvaddr *rcv;
u_int i;

if (addrlen + OIDLEN_ifRcvAddressEntry + 1 > ASN_MAXOIDLEN)
return (NULL);

if ((rcv = malloc(sizeof(*rcv))) == NULL)
return (NULL);
rcv->ifindex = ifp->index;
rcv->addrlen = addrlen;
memcpy(rcv->addr, addr, addrlen);
rcv->flags = 0;

rcv->index.len = addrlen + 2;
rcv->index.subs[0] = ifp->index;
rcv->index.subs[1] = addrlen;
for (i = 0; i < addrlen; i++)
rcv->index.subs[i + 2] = addr[i];

INSERT_OBJECT_OID(rcv, &mibrcvaddr_list);

return (rcv);
}




void
mib_rcvaddr_delete(struct mibrcvaddr *rcv)
{
TAILQ_REMOVE(&mibrcvaddr_list, rcv, link);
free(rcv);
}

int
op_rcvaddr(struct snmp_context *ctx __unused, struct snmp_value *value,
u_int sub, u_int iidx __unused, enum snmp_op op)
{
struct mibrcvaddr *rcv;

rcv = NULL;

switch (op) {

case SNMP_OP_GETNEXT:
if ((rcv = NEXT_OBJECT_OID(&mibrcvaddr_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
index_append(&value->var, sub, &rcv->index);
break;

case SNMP_OP_GET:
if ((rcv = FIND_OBJECT_OID(&mibrcvaddr_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
break;

case SNMP_OP_SET:
if ((rcv = FIND_OBJECT_OID(&mibrcvaddr_list, &value->var, sub)) == NULL)
return (SNMP_ERR_NO_CREATION);
return (SNMP_ERR_NOT_WRITEABLE);

case SNMP_OP_ROLLBACK:
case SNMP_OP_COMMIT:
abort();
}

switch (value->var.subs[sub - 1]) {

case LEAF_ifRcvAddressStatus:
value->v.integer = 1;
break;

case LEAF_ifRcvAddressType:
value->v.integer = (rcv->flags & MIBRCVADDR_VOLATILE) ? 2 : 3;
break;
}
return (SNMP_ERR_NOERROR);
}
