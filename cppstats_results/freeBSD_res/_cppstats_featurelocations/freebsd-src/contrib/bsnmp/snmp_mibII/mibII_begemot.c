































#include "mibII.h"
#include "mibII_oid.h"




int
op_begemot_mibII(struct snmp_context *ctx, struct snmp_value *value,
u_int sub, u_int idx __unused, enum snmp_op op)
{
switch (op) {

case SNMP_OP_GETNEXT:
abort();

case SNMP_OP_GET:
goto get;

case SNMP_OP_SET:
switch (value->var.subs[sub - 1]) {

case LEAF_begemotIfMaxspeed:
case LEAF_begemotIfPoll:
return (SNMP_ERR_NOT_WRITEABLE);

case LEAF_begemotIfForcePoll:
ctx->scratch->int1 = mibif_force_hc_update_interval;
mibif_force_hc_update_interval = value->v.uint32;
return (SNMP_ERR_NOERROR);

case LEAF_begemotIfDataPoll:
ctx->scratch->int1 = mibII_poll_ticks;
mibII_poll_ticks = value->v.uint32;
return (SNMP_ERR_NOERROR);
}
abort();

case SNMP_OP_ROLLBACK:
switch (value->var.subs[sub - 1]) {

case LEAF_begemotIfForcePoll:
mibif_force_hc_update_interval = ctx->scratch->int1;
return (SNMP_ERR_NOERROR);

case LEAF_begemotIfDataPoll:
mibII_poll_ticks = ctx->scratch->int1;
return (SNMP_ERR_NOERROR);
}
abort();

case SNMP_OP_COMMIT:
switch (value->var.subs[sub - 1]) {

case LEAF_begemotIfForcePoll:
mibif_force_hc_update_interval = ctx->scratch->int1;
mibif_reset_hc_timer();
return (SNMP_ERR_NOERROR);

case LEAF_begemotIfDataPoll:
mibif_restart_mibII_poll_timer();
return (SNMP_ERR_NOERROR);
}
abort();
}
abort();

get:

switch (value->var.subs[sub - 1]) {

case LEAF_begemotIfMaxspeed:
value->v.counter64 = mibif_maxspeed;
return (SNMP_ERR_NOERROR);

case LEAF_begemotIfPoll:
value->v.uint32 = mibif_hc_update_interval;
return (SNMP_ERR_NOERROR);

case LEAF_begemotIfForcePoll:
value->v.uint32 = mibif_force_hc_update_interval;
return (SNMP_ERR_NOERROR);

case LEAF_begemotIfDataPoll:
value->v.uint32 = mibII_poll_ticks;
return (SNMP_ERR_NOERROR);
}
abort();
}
