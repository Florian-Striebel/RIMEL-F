#include "server.h"
rax *TrackingTable = NULL;
rax *PrefixTable = NULL;
uint64_t TrackingTableTotalItems = 0;
robj *TrackingChannelName;
typedef struct bcastState {
rax *keys;
rax *clients;
} bcastState;
void disableTracking(client *c) {
if (c->flags & CLIENT_TRACKING_BCAST) {
raxIterator ri;
raxStart(&ri,c->client_tracking_prefixes);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
bcastState *bs = raxFind(PrefixTable,ri.key,ri.key_len);
serverAssert(bs != raxNotFound);
raxRemove(bs->clients,(unsigned char*)&c,sizeof(c),NULL);
if (raxSize(bs->clients) == 0) {
raxFree(bs->clients);
raxFree(bs->keys);
zfree(bs);
raxRemove(PrefixTable,ri.key,ri.key_len,NULL);
}
}
raxStop(&ri);
raxFree(c->client_tracking_prefixes);
c->client_tracking_prefixes = NULL;
}
if (c->flags & CLIENT_TRACKING) {
server.tracking_clients--;
c->flags &= ~(CLIENT_TRACKING|CLIENT_TRACKING_BROKEN_REDIR|
CLIENT_TRACKING_BCAST|CLIENT_TRACKING_OPTIN|
CLIENT_TRACKING_OPTOUT|CLIENT_TRACKING_CACHING|
CLIENT_TRACKING_NOLOOP);
}
}
static int stringCheckPrefix(unsigned char *s1, size_t s1_len, unsigned char *s2, size_t s2_len) {
size_t min_length = s1_len < s2_len ? s1_len : s2_len;
return memcmp(s1,s2,min_length) == 0;
}
int checkPrefixCollisionsOrReply(client *c, robj **prefixes, size_t numprefix) {
for (size_t i = 0; i < numprefix; i++) {
if (c->client_tracking_prefixes) {
raxIterator ri;
raxStart(&ri,c->client_tracking_prefixes);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
if (stringCheckPrefix(ri.key,ri.key_len,
prefixes[i]->ptr,sdslen(prefixes[i]->ptr)))
{
sds collision = sdsnewlen(ri.key,ri.key_len);
addReplyErrorFormat(c,
"Prefix '%s' overlaps with an existing prefix '%s'. "
"Prefixes for a single client must not overlap.",
(unsigned char *)prefixes[i]->ptr,
(unsigned char *)collision);
sdsfree(collision);
raxStop(&ri);
return 0;
}
}
raxStop(&ri);
}
for (size_t j = i + 1; j < numprefix; j++) {
if (stringCheckPrefix(prefixes[i]->ptr,sdslen(prefixes[i]->ptr),
prefixes[j]->ptr,sdslen(prefixes[j]->ptr)))
{
addReplyErrorFormat(c,
"Prefix '%s' overlaps with another provided prefix '%s'. "
"Prefixes for a single client must not overlap.",
(unsigned char *)prefixes[i]->ptr,
(unsigned char *)prefixes[j]->ptr);
return i;
}
}
}
return 1;
}
void enableBcastTrackingForPrefix(client *c, char *prefix, size_t plen) {
bcastState *bs = raxFind(PrefixTable,(unsigned char*)prefix,plen);
if (bs == raxNotFound) {
bs = zmalloc(sizeof(*bs));
bs->keys = raxNew();
bs->clients = raxNew();
raxInsert(PrefixTable,(unsigned char*)prefix,plen,bs,NULL);
}
if (raxTryInsert(bs->clients,(unsigned char*)&c,sizeof(c),NULL,NULL)) {
if (c->client_tracking_prefixes == NULL)
c->client_tracking_prefixes = raxNew();
raxInsert(c->client_tracking_prefixes,
(unsigned char*)prefix,plen,NULL,NULL);
}
}
void enableTracking(client *c, uint64_t redirect_to, uint64_t options, robj **prefix, size_t numprefix) {
if (!(c->flags & CLIENT_TRACKING)) server.tracking_clients++;
c->flags |= CLIENT_TRACKING;
c->flags &= ~(CLIENT_TRACKING_BROKEN_REDIR|CLIENT_TRACKING_BCAST|
CLIENT_TRACKING_OPTIN|CLIENT_TRACKING_OPTOUT|
CLIENT_TRACKING_NOLOOP);
c->client_tracking_redirection = redirect_to;
if (TrackingTable == NULL) {
TrackingTable = raxNew();
PrefixTable = raxNew();
TrackingChannelName = createStringObject("__redis__:invalidate",20);
}
if (options & CLIENT_TRACKING_BCAST) {
c->flags |= CLIENT_TRACKING_BCAST;
if (numprefix == 0) enableBcastTrackingForPrefix(c,"",0);
for (size_t j = 0; j < numprefix; j++) {
sds sdsprefix = prefix[j]->ptr;
enableBcastTrackingForPrefix(c,sdsprefix,sdslen(sdsprefix));
}
}
c->flags |= options & (CLIENT_TRACKING_OPTIN|CLIENT_TRACKING_OPTOUT|
CLIENT_TRACKING_NOLOOP);
}
void trackingRememberKeys(client *c) {
uint64_t optin = c->flags & CLIENT_TRACKING_OPTIN;
uint64_t optout = c->flags & CLIENT_TRACKING_OPTOUT;
uint64_t caching_given = c->flags & CLIENT_TRACKING_CACHING;
if ((optin && !caching_given) || (optout && caching_given)) return;
getKeysResult result = GETKEYS_RESULT_INIT;
int numkeys = getKeysFromCommand(c->cmd,c->argv,c->argc,&result);
if (!numkeys) {
getKeysFreeResult(&result);
return;
}
if (c->cmd->flags & CMD_PUBSUB) {
return;
}
keyReference *keys = result.keys;
for(int j = 0; j < numkeys; j++) {
int idx = keys[j].pos;
sds sdskey = c->argv[idx]->ptr;
rax *ids = raxFind(TrackingTable,(unsigned char*)sdskey,sdslen(sdskey));
if (ids == raxNotFound) {
ids = raxNew();
int inserted = raxTryInsert(TrackingTable,(unsigned char*)sdskey,
sdslen(sdskey),ids, NULL);
serverAssert(inserted == 1);
}
if (raxTryInsert(ids,(unsigned char*)&c->id,sizeof(c->id),NULL,NULL))
TrackingTableTotalItems++;
}
getKeysFreeResult(&result);
}
void sendTrackingMessage(client *c, char *keyname, size_t keylen, int proto) {
int using_redirection = 0;
if (c->client_tracking_redirection) {
client *redir = lookupClientByID(c->client_tracking_redirection);
if (!redir) {
c->flags |= CLIENT_TRACKING_BROKEN_REDIR;
if (c->resp > 2) {
addReplyPushLen(c,2);
addReplyBulkCBuffer(c,"tracking-redir-broken",21);
addReplyLongLong(c,c->client_tracking_redirection);
}
return;
}
c = redir;
using_redirection = 1;
}
if (c->resp > 2) {
addReplyPushLen(c,2);
addReplyBulkCBuffer(c,"invalidate",10);
} else if (using_redirection && c->flags & CLIENT_PUBSUB) {
addReplyPubsubMessage(c,TrackingChannelName,NULL);
} else {
return;
}
if (proto) {
addReplyProto(c,keyname,keylen);
} else {
addReplyArrayLen(c,1);
addReplyBulkCBuffer(c,keyname,keylen);
}
updateClientMemUsage(c);
}
void trackingRememberKeyToBroadcast(client *c, char *keyname, size_t keylen) {
raxIterator ri;
raxStart(&ri,PrefixTable);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
if (ri.key_len > keylen) continue;
if (ri.key_len != 0 && memcmp(ri.key,keyname,ri.key_len) != 0)
continue;
bcastState *bs = ri.data;
raxInsert(bs->keys,(unsigned char*)keyname,keylen,c,NULL);
}
raxStop(&ri);
}
void trackingInvalidateKey(client *c, robj *keyobj, int bcast) {
if (TrackingTable == NULL) return;
unsigned char *key = (unsigned char*)keyobj->ptr;
size_t keylen = sdslen(keyobj->ptr);
if (bcast && raxSize(PrefixTable) > 0)
trackingRememberKeyToBroadcast(c,(char *)key,keylen);
rax *ids = raxFind(TrackingTable,key,keylen);
if (ids == raxNotFound) return;
raxIterator ri;
raxStart(&ri,ids);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
uint64_t id;
memcpy(&id,ri.key,sizeof(id));
client *target = lookupClientByID(id);
if (target == NULL ||
!(target->flags & CLIENT_TRACKING)||
target->flags & CLIENT_TRACKING_BCAST)
{
continue;
}
if (target->flags & CLIENT_TRACKING_NOLOOP &&
target == c)
{
continue;
}
if (target == server.current_client){
incrRefCount(keyobj);
listAddNodeTail(server.tracking_pending_keys, keyobj);
} else {
sendTrackingMessage(target,(char *)keyobj->ptr,sdslen(keyobj->ptr),0);
}
}
raxStop(&ri);
TrackingTableTotalItems -= raxSize(ids);
raxFree(ids);
raxRemove(TrackingTable,(unsigned char*)key,keylen,NULL);
}
void trackingHandlePendingKeyInvalidations() {
if (!listLength(server.tracking_pending_keys)) return;
listNode *ln;
listIter li;
listRewind(server.tracking_pending_keys,&li);
while ((ln = listNext(&li)) != NULL) {
robj *key = listNodeValue(ln);
if (server.current_client != NULL)
sendTrackingMessage(server.current_client,(char *)key->ptr,sdslen(key->ptr),0);
decrRefCount(key);
}
listEmpty(server.tracking_pending_keys);
}
void freeTrackingRadixTreeCallback(void *rt) {
raxFree(rt);
}
void freeTrackingRadixTree(rax *rt) {
raxFreeWithCallback(rt,freeTrackingRadixTreeCallback);
}
void trackingInvalidateKeysOnFlush(int async) {
if (server.tracking_clients) {
listNode *ln;
listIter li;
listRewind(server.clients,&li);
while ((ln = listNext(&li)) != NULL) {
client *c = listNodeValue(ln);
if (c->flags & CLIENT_TRACKING) {
sendTrackingMessage(c,shared.null[c->resp]->ptr,sdslen(shared.null[c->resp]->ptr),1);
}
}
}
if (TrackingTable) {
if (async) {
freeTrackingRadixTreeAsync(TrackingTable);
} else {
freeTrackingRadixTree(TrackingTable);
}
TrackingTable = raxNew();
TrackingTableTotalItems = 0;
}
}
void trackingLimitUsedSlots(void) {
static unsigned int timeout_counter = 0;
if (TrackingTable == NULL) return;
if (server.tracking_table_max_keys == 0) return;
size_t max_keys = server.tracking_table_max_keys;
if (raxSize(TrackingTable) <= max_keys) {
timeout_counter = 0;
return;
}
int effort = 100 * (timeout_counter+1);
raxIterator ri;
raxStart(&ri,TrackingTable);
while(effort > 0) {
effort--;
raxSeek(&ri,"^",NULL,0);
raxRandomWalk(&ri,0);
if (raxEOF(&ri)) break;
robj *keyobj = createStringObject((char*)ri.key,ri.key_len);
trackingInvalidateKey(NULL,keyobj,0);
decrRefCount(keyobj);
if (raxSize(TrackingTable) <= max_keys) {
timeout_counter = 0;
raxStop(&ri);
return;
}
}
raxStop(&ri);
timeout_counter++;
}
sds trackingBuildBroadcastReply(client *c, rax *keys) {
raxIterator ri;
uint64_t count;
if (c == NULL) {
count = raxSize(keys);
} else {
count = 0;
raxStart(&ri,keys);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
if (ri.data != c) count++;
}
raxStop(&ri);
if (count == 0) return NULL;
}
char buf[32];
size_t len = ll2string(buf,sizeof(buf),count);
sds proto = sdsempty();
proto = sdsMakeRoomFor(proto,count*15);
proto = sdscatlen(proto,"*",1);
proto = sdscatlen(proto,buf,len);
proto = sdscatlen(proto,"\r\n",2);
raxStart(&ri,keys);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
if (c && ri.data == c) continue;
len = ll2string(buf,sizeof(buf),ri.key_len);
proto = sdscatlen(proto,"$",1);
proto = sdscatlen(proto,buf,len);
proto = sdscatlen(proto,"\r\n",2);
proto = sdscatlen(proto,ri.key,ri.key_len);
proto = sdscatlen(proto,"\r\n",2);
}
raxStop(&ri);
return proto;
}
void trackingBroadcastInvalidationMessages(void) {
raxIterator ri, ri2;
if (TrackingTable == NULL || !server.tracking_clients) return;
raxStart(&ri,PrefixTable);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
bcastState *bs = ri.data;
if (raxSize(bs->keys)) {
sds proto = trackingBuildBroadcastReply(NULL,bs->keys);
raxStart(&ri2,bs->clients);
raxSeek(&ri2,"^",NULL,0);
while(raxNext(&ri2)) {
client *c;
memcpy(&c,ri2.key,sizeof(c));
if (c->flags & CLIENT_TRACKING_NOLOOP) {
sds adhoc = trackingBuildBroadcastReply(c,bs->keys);
if (adhoc) {
sendTrackingMessage(c,adhoc,sdslen(adhoc),1);
sdsfree(adhoc);
}
} else {
sendTrackingMessage(c,proto,sdslen(proto),1);
}
}
raxStop(&ri2);
sdsfree(proto);
}
raxFree(bs->keys);
bs->keys = raxNew();
}
raxStop(&ri);
}
uint64_t trackingGetTotalItems(void) {
return TrackingTableTotalItems;
}
uint64_t trackingGetTotalKeys(void) {
if (TrackingTable == NULL) return 0;
return raxSize(TrackingTable);
}
uint64_t trackingGetTotalPrefixes(void) {
if (PrefixTable == NULL) return 0;
return raxSize(PrefixTable);
}
