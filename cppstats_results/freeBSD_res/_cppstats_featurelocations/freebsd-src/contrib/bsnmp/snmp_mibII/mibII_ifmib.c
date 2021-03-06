































#include "mibII.h"
#include "mibII_oid.h"




int
op_ifmib(struct snmp_context *ctx __unused, struct snmp_value *value,
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

case LEAF_ifTableLastChange:
if (mib_iftable_last_change > start_tick)
value->v.uint32 = mib_iftable_last_change - start_tick;
else
value->v.uint32 = 0;
break;

case LEAF_ifStackLastChange:
if (mib_ifstack_last_change > start_tick)
value->v.uint32 = mib_ifstack_last_change - start_tick;
else
value->v.uint32 = 0;
break;
}
return (SNMP_ERR_NOERROR);
}
