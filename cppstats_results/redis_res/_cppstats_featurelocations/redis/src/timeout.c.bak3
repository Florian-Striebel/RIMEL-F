



























#include "server.h"
#include "cluster.h"






int checkBlockedClientTimeout(client *c, mstime_t now) {
if (c->flags & CLIENT_BLOCKED &&
c->bpop.timeout != 0
&& c->bpop.timeout < now)
{

replyToBlockedClientTimedOut(c);
unblockClient(c);
return 1;
} else {
return 0;
}
}





int clientsCronHandleTimeout(client *c, mstime_t now_ms) {
time_t now = now_ms/1000;

if (server.maxidletime &&

!(c->flags & CLIENT_SLAVE) &&
!(c->flags & CLIENT_MASTER) &&
!(c->flags & CLIENT_BLOCKED) &&
!(c->flags & CLIENT_PUBSUB) &&
(now - c->lastinteraction > server.maxidletime))
{
serverLog(LL_VERBOSE,"Closing idle client");
freeClient(c);
return 1;
} else if (c->flags & CLIENT_BLOCKED) {


if (server.cluster_enabled) {
if (clusterRedirectBlockedClientIfNeeded(c))
unblockClient(c);
}
}
return 0;
}














#define CLIENT_ST_KEYLEN 16


void encodeTimeoutKey(unsigned char *buf, uint64_t timeout, client *c) {
timeout = htonu64(timeout);
memcpy(buf,&timeout,sizeof(timeout));
memcpy(buf+8,&c,sizeof(c));
if (sizeof(c) == 4) memset(buf+12,0,4);
}



void decodeTimeoutKey(unsigned char *buf, uint64_t *toptr, client **cptr) {
memcpy(toptr,buf,sizeof(*toptr));
*toptr = ntohu64(*toptr);
memcpy(cptr,buf+8,sizeof(*cptr));
}




void addClientToTimeoutTable(client *c) {
if (c->bpop.timeout == 0) return;
uint64_t timeout = c->bpop.timeout;
unsigned char buf[CLIENT_ST_KEYLEN];
encodeTimeoutKey(buf,timeout,c);
if (raxTryInsert(server.clients_timeout_table,buf,sizeof(buf),NULL,NULL))
c->flags |= CLIENT_IN_TO_TABLE;
}



void removeClientFromTimeoutTable(client *c) {
if (!(c->flags & CLIENT_IN_TO_TABLE)) return;
c->flags &= ~CLIENT_IN_TO_TABLE;
uint64_t timeout = c->bpop.timeout;
unsigned char buf[CLIENT_ST_KEYLEN];
encodeTimeoutKey(buf,timeout,c);
raxRemove(server.clients_timeout_table,buf,sizeof(buf),NULL);
}



void handleBlockedClientsTimeout(void) {
if (raxSize(server.clients_timeout_table) == 0) return;
uint64_t now = mstime();
raxIterator ri;
raxStart(&ri,server.clients_timeout_table);
raxSeek(&ri,"^",NULL,0);

while(raxNext(&ri)) {
uint64_t timeout;
client *c;
decodeTimeoutKey(ri.key,&timeout,&c);
if (timeout >= now) break;
c->flags &= ~CLIENT_IN_TO_TABLE;
checkBlockedClientTimeout(c,now);
raxRemove(server.clients_timeout_table,ri.key,ri.key_len,NULL);
raxSeek(&ri,"^",NULL,0);
}
raxStop(&ri);
}









int getTimeoutFromObjectOrReply(client *c, robj *object, mstime_t *timeout, int unit) {
long long tval;
long double ftval;

if (unit == UNIT_SECONDS) {
if (getLongDoubleFromObjectOrReply(c,object,&ftval,
"timeout is not a float or out of range") != C_OK)
return C_ERR;
tval = (long long) (ftval * 1000.0);
} else {
if (getLongLongFromObjectOrReply(c,object,&tval,
"timeout is not an integer or out of range") != C_OK)
return C_ERR;
}

if (tval < 0) {
addReplyError(c,"timeout is negative");
return C_ERR;
}

if (tval > 0) {
tval += mstime();
}
*timeout = tval;

return C_OK;
}
