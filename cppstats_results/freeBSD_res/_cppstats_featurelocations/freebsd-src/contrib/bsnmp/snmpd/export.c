































#include <sys/types.h>
#include <sys/queue.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>

#include "snmpmod.h"
#include "snmpd.h"
#include "tree.h"











int
string_save(struct snmp_value *value, struct snmp_context *ctx,
ssize_t req_size, u_char **valp)
{
if (req_size != -1 && value->v.octetstring.len != (u_long)req_size)
return (SNMP_ERR_BADVALUE);

ctx->scratch->ptr1 = *valp;

if ((*valp = malloc(value->v.octetstring.len + 1)) == NULL) {
*valp = ctx->scratch->ptr1;
return (SNMP_ERR_RES_UNAVAIL);
}

memcpy(*valp, value->v.octetstring.octets, value->v.octetstring.len);
(*valp)[value->v.octetstring.len] = '\0';

return (0);
}




void
string_commit(struct snmp_context *ctx)
{
free(ctx->scratch->ptr1);
}




void
string_rollback(struct snmp_context *ctx, u_char **valp)
{
free(*valp);
*valp = ctx->scratch->ptr1;
}




void
string_free(struct snmp_context *ctx)
{
free(ctx->scratch->ptr1);
}




int
string_get(struct snmp_value *value, const u_char *ptr, ssize_t len)
{
if (ptr == NULL) {
value->v.octetstring.len = 0;
value->v.octetstring.octets = NULL;
return (SNMP_ERR_NOERROR);
}
if (len == -1)
len = strlen(ptr);
if ((value->v.octetstring.octets = malloc((size_t)len)) == NULL) {
value->v.octetstring.len = 0;
return (SNMP_ERR_RES_UNAVAIL);
}
value->v.octetstring.len = (u_long)len;
memcpy(value->v.octetstring.octets, ptr, (size_t)len);
return (SNMP_ERR_NOERROR);
}




int
string_get_max(struct snmp_value *value, const u_char *ptr, ssize_t len,
size_t maxlen)
{

if (ptr == NULL) {
value->v.octetstring.len = 0;
value->v.octetstring.octets = NULL;
return (SNMP_ERR_NOERROR);
}
if (len == -1)
len = strlen(ptr);
if ((size_t)len > maxlen)
len = maxlen;
if ((value->v.octetstring.octets = malloc((size_t)len)) == NULL) {
value->v.octetstring.len = 0;
return (SNMP_ERR_RES_UNAVAIL);
}
value->v.octetstring.len = (u_long)len;
memcpy(value->v.octetstring.octets, ptr, (size_t)len);
return (SNMP_ERR_NOERROR);
}






int
ip_save(struct snmp_value *value, struct snmp_context *ctx, u_char *valp)
{
ctx->scratch->int1 = (valp[0] << 24) | (valp[1] << 16) | (valp[2] << 8)
| valp[3];

valp[0] = value->v.ipaddress[0];
valp[1] = value->v.ipaddress[1];
valp[2] = value->v.ipaddress[2];
valp[3] = value->v.ipaddress[3];

return (0);
}




void
ip_rollback(struct snmp_context *ctx, u_char *valp)
{
valp[0] = ctx->scratch->int1 >> 24;
valp[1] = ctx->scratch->int1 >> 16;
valp[2] = ctx->scratch->int1 >> 8;
valp[3] = ctx->scratch->int1;
}




void
ip_commit(struct snmp_context *ctx __unused)
{
}




int
ip_get(struct snmp_value *value, u_char *valp)
{
value->v.ipaddress[0] = valp[0];
value->v.ipaddress[1] = valp[1];
value->v.ipaddress[2] = valp[2];
value->v.ipaddress[3] = valp[3];

return (SNMP_ERR_NOERROR);
}






int
oid_save(struct snmp_value *value, struct snmp_context *ctx,
struct asn_oid *oid)
{
if ((ctx->scratch->ptr1 = malloc(sizeof(struct asn_oid))) == NULL)
return (SNMP_ERR_RES_UNAVAIL);
*(struct asn_oid *)ctx->scratch->ptr1 = *oid;
*oid = value->v.oid;

return (0);
}

void
oid_rollback(struct snmp_context *ctx, struct asn_oid *oid)
{
*oid = *(struct asn_oid *)ctx->scratch->ptr1;
free(ctx->scratch->ptr1);
}

void
oid_commit(struct snmp_context *ctx)
{
free(ctx->scratch->ptr1);
}

int
oid_get(struct snmp_value *value, const struct asn_oid *oid)
{
value->v.oid = *oid;
return (SNMP_ERR_NOERROR);
}




int
index_decode(const struct asn_oid *oid, u_int sub, u_int code, ...)
{
va_list ap;
u_int index_count;
void *octs[10];
u_int nocts;
u_int idx;

va_start(ap, code);
index_count = SNMP_INDEX_COUNT(code);
nocts = 0;

for (idx = 0; idx < index_count; idx++) {
switch (SNMP_INDEX(code, idx)) {

case SNMP_SYNTAX_NULL:
break;

case SNMP_SYNTAX_INTEGER:
if (sub == oid->len)
goto err;
*va_arg(ap, int32_t *) = oid->subs[sub++];
break;

case SNMP_SYNTAX_COUNTER64:
if (sub == oid->len)
goto err;
*va_arg(ap, u_int64_t *) = oid->subs[sub++];
break;

case SNMP_SYNTAX_OCTETSTRING:
{
u_char **cval;
size_t *sval;
u_int i;


if (sub == oid->len)
goto err;
cval = va_arg(ap, u_char **);
sval = va_arg(ap, size_t *);
*sval = oid->subs[sub++];
if (sub + *sval > oid->len)
goto err;
if ((*cval = malloc(*sval)) == NULL) {
syslog(LOG_ERR, "%s: %m", __func__);
goto err;
}
octs[nocts++] = *cval;
for (i = 0; i < *sval; i++) {
if (oid->subs[sub] > 0xff)
goto err;
(*cval)[i] = oid->subs[sub++];
}
break;
}

case SNMP_SYNTAX_OID:
{
struct asn_oid *aval;
u_int i;

if (sub == oid->len)
goto err;
aval = va_arg(ap, struct asn_oid *);
aval->len = oid->subs[sub++];
if (aval->len > ASN_MAXOIDLEN)
goto err;
for (i = 0; i < aval->len; i++)
aval->subs[i] = oid->subs[sub++];
break;
}

case SNMP_SYNTAX_IPADDRESS:
{
u_int8_t *pval;
u_int i;

if (sub + 4 > oid->len)
goto err;
pval = va_arg(ap, u_int8_t *);
for (i = 0; i < 4; i++) {
if (oid->subs[sub] > 0xff)
goto err;
pval[i] = oid->subs[sub++];
}
break;
}

case SNMP_SYNTAX_COUNTER:
case SNMP_SYNTAX_GAUGE:
case SNMP_SYNTAX_TIMETICKS:
if (sub == oid->len)
goto err;
if (oid->subs[sub] > 0xffffffff)
goto err;
*va_arg(ap, u_int32_t *) = oid->subs[sub++];
break;
}
}

va_end(ap);
return (0);

err:
va_end(ap);
while(nocts > 0)
free(octs[--nocts]);
return (-1);
}




int
index_compare_off(const struct asn_oid *oid, u_int sub,
const struct asn_oid *idx, u_int off)
{
u_int i;

for (i = off; i < idx->len && i < oid->len - sub; i++) {
if (oid->subs[sub + i] < idx->subs[i])
return (-1);
if (oid->subs[sub + i] > idx->subs[i])
return (+1);
}
if (oid->len - sub < idx->len)
return (-1);
if (oid->len - sub > idx->len)
return (+1);

return (0);
}

int
index_compare(const struct asn_oid *oid, u_int sub, const struct asn_oid *idx)
{
return (index_compare_off(oid, sub, idx, 0));
}




void
index_append_off(struct asn_oid *var, u_int sub, const struct asn_oid *idx,
u_int off)
{
u_int i;

var->len = sub + idx->len;
for (i = off; i < idx->len; i++)
var->subs[sub + i] = idx->subs[i];
}
void
index_append(struct asn_oid *var, u_int sub, const struct asn_oid *idx)
{
index_append_off(var, sub, idx, 0);
}

