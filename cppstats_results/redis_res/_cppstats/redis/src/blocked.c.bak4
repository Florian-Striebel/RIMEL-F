





























































#include "server.h"
#include "slowlog.h"
#include "latency.h"
#include "monotonic.h"

void serveClientBlockedOnList(client *receiver, robj *o, robj *key, robj *dstkey, redisDb *db, int wherefrom, int whereto, int *deleted);
int getListPositionFromObjectOrReply(client *c, robj *arg, int *position);











typedef struct bkinfo {
listNode *listnode;
streamID stream_id;
} bkinfo;




void blockClient(client *c, int btype) {

serverAssert(!(c->flags & CLIENT_MASTER &&
btype != BLOCKED_MODULE &&
btype != BLOCKED_POSTPONE));

c->flags |= CLIENT_BLOCKED;
c->btype = btype;
server.blocked_clients++;
server.blocked_clients_by_type[btype]++;
addClientToTimeoutTable(c);
if (btype == BLOCKED_POSTPONE) {
listAddNodeTail(server.postponed_clients, c);
c->postponed_list_node = listLast(server.postponed_clients);

c->flags |= CLIENT_PENDING_COMMAND;
}
}




void updateStatsOnUnblock(client *c, long blocked_us, long reply_us){
const ustime_t total_cmd_duration = c->duration + blocked_us + reply_us;
c->lastcmd->microseconds += total_cmd_duration;
if (server.latency_tracking_enabled)
updateCommandLatencyHistogram(&(c->lastcmd->latency_histogram), total_cmd_duration*1000);

slowlogPushCurrentCommand(c, c->lastcmd, total_cmd_duration);

latencyAddSampleIfNeeded("command-unblocking",reply_us/1000);
}




void processUnblockedClients(void) {
listNode *ln;
client *c;

while (listLength(server.unblocked_clients)) {
ln = listFirst(server.unblocked_clients);
serverAssert(ln != NULL);
c = ln->value;
listDelNode(server.unblocked_clients,ln);
c->flags &= ~CLIENT_UNBLOCKED;





if (!(c->flags & CLIENT_BLOCKED)) {

if (processPendingCommandsAndResetClient(c) == C_OK) {

if (c->querybuf && sdslen(c->querybuf) > 0) {
if (processInputBuffer(c) == C_ERR) c = NULL;
}
} else {
c = NULL;
}
}
beforeNextClient(c);
}
}

















void queueClientForReprocessing(client *c) {


if (!(c->flags & CLIENT_UNBLOCKED)) {
c->flags |= CLIENT_UNBLOCKED;
listAddNodeTail(server.unblocked_clients,c);
}
}



void unblockClient(client *c) {
if (c->btype == BLOCKED_LIST ||
c->btype == BLOCKED_ZSET ||
c->btype == BLOCKED_STREAM) {
unblockClientWaitingData(c);
} else if (c->btype == BLOCKED_WAIT) {
unblockClientWaitingReplicas(c);
} else if (c->btype == BLOCKED_MODULE) {
if (moduleClientIsBlockedOnKeys(c)) unblockClientWaitingData(c);
unblockClientFromModule(c);
} else if (c->btype == BLOCKED_POSTPONE) {
listDelNode(server.postponed_clients,c->postponed_list_node);
c->postponed_list_node = NULL;
} else if (c->btype == BLOCKED_SHUTDOWN) {

} else {
serverPanic("Unknown btype in unblockClient().");
}





if (c->btype != BLOCKED_POSTPONE) {
freeClientOriginalArgv(c);
resetClient(c);
}



server.blocked_clients--;
server.blocked_clients_by_type[c->btype]--;
c->flags &= ~CLIENT_BLOCKED;
c->btype = BLOCKED_NONE;
removeClientFromTimeoutTable(c);
queueClientForReprocessing(c);
}




void replyToBlockedClientTimedOut(client *c) {
if (c->btype == BLOCKED_LIST ||
c->btype == BLOCKED_ZSET ||
c->btype == BLOCKED_STREAM) {
addReplyNullArray(c);
} else if (c->btype == BLOCKED_WAIT) {
addReplyLongLong(c,replicationCountAcksByOffset(c->bpop.reploffset));
} else if (c->btype == BLOCKED_MODULE) {
moduleBlockedClientTimedOut(c);
} else {
serverPanic("Unknown btype in replyToBlockedClientTimedOut().");
}
}



void replyToClientsBlockedOnShutdown(void) {
if (server.blocked_clients_by_type[BLOCKED_SHUTDOWN] == 0) return;
listNode *ln;
listIter li;
listRewind(server.clients, &li);
while((ln = listNext(&li))) {
client *c = listNodeValue(ln);
if (c->flags & CLIENT_BLOCKED && c->btype == BLOCKED_SHUTDOWN) {
addReplyError(c, "Errors trying to SHUTDOWN. Check logs.");
unblockClient(c);
}
}
}








void disconnectAllBlockedClients(void) {
listNode *ln;
listIter li;

listRewind(server.clients,&li);
while((ln = listNext(&li))) {
client *c = listNodeValue(ln);

if (c->flags & CLIENT_BLOCKED) {




if (c->btype == BLOCKED_POSTPONE)
continue;

addReplyError(c,
"-UNBLOCKED force unblock from blocking operation, "
"instance state changed (master -> replica?)");
unblockClient(c);
c->flags |= CLIENT_CLOSE_AFTER_REPLY;
}
}
}




void serveClientsBlockedOnListKey(robj *o, readyList *rl) {


dictEntry *de = dictFind(rl->db->blocking_keys,rl->key);
if (de) {
list *clients = dictGetVal(de);
int numclients = listLength(clients);
int deleted = 0;

while(numclients--) {
listNode *clientnode = listFirst(clients);
client *receiver = clientnode->value;

if (receiver->btype != BLOCKED_LIST) {


listRotateHeadToTail(clients);
continue;
}

robj *dstkey = receiver->bpop.target;
int wherefrom = receiver->bpop.blockpos.wherefrom;
int whereto = receiver->bpop.blockpos.whereto;




if (dstkey) incrRefCount(dstkey);

client *old_client = server.current_client;
server.current_client = receiver;
monotime replyTimer;
elapsedStart(&replyTimer);
serveClientBlockedOnList(receiver, o,
rl->key, dstkey, rl->db,
wherefrom, whereto,
&deleted);
updateStatsOnUnblock(receiver, 0, elapsedUs(replyTimer));
unblockClient(receiver);
afterCommand(receiver);
server.current_client = old_client;

if (dstkey) decrRefCount(dstkey);


if (deleted) break;
}
}
}




void serveClientsBlockedOnSortedSetKey(robj *o, readyList *rl) {


dictEntry *de = dictFind(rl->db->blocking_keys,rl->key);
if (de) {
list *clients = dictGetVal(de);
int numclients = listLength(clients);
int deleted = 0;

while (numclients--) {
listNode *clientnode = listFirst(clients);
client *receiver = clientnode->value;

if (receiver->btype != BLOCKED_ZSET) {


listRotateHeadToTail(clients);
continue;
}

long llen = zsetLength(o);
long count = receiver->bpop.count;
int where = receiver->bpop.blockpos.wherefrom;
int use_nested_array = (receiver->lastcmd &&
receiver->lastcmd->proc == bzmpopCommand)
? 1 : 0;
int reply_nil_when_empty = use_nested_array;

client *old_client = server.current_client;
server.current_client = receiver;
monotime replyTimer;
elapsedStart(&replyTimer);
genericZpopCommand(receiver, &rl->key, 1, where, 1, count, use_nested_array, reply_nil_when_empty, &deleted);


int argc = 2;
robj *argv[3];
argv[0] = where == ZSET_MIN ? shared.zpopmin : shared.zpopmax;
argv[1] = rl->key;
incrRefCount(rl->key);
if (count != -1) {

robj *count_obj = createStringObjectFromLongLong((count > llen) ? llen : count);
argv[2] = count_obj;
argc++;
}
alsoPropagate(receiver->db->id, argv, argc, PROPAGATE_AOF|PROPAGATE_REPL);
decrRefCount(argv[1]);
if (count != -1) decrRefCount(argv[2]);

updateStatsOnUnblock(receiver, 0, elapsedUs(replyTimer));
unblockClient(receiver);
afterCommand(receiver);
server.current_client = old_client;


if (deleted) break;
}
}
}




void serveClientsBlockedOnStreamKey(robj *o, readyList *rl) {
dictEntry *de = dictFind(rl->db->blocking_keys,rl->key);
stream *s = o->ptr;




if (de) {
list *clients = dictGetVal(de);
listNode *ln;
listIter li;
listRewind(clients,&li);

while((ln = listNext(&li))) {
client *receiver = listNodeValue(ln);
if (receiver->btype != BLOCKED_STREAM) continue;
bkinfo *bki = dictFetchValue(receiver->bpop.keys,rl->key);
streamID *gt = &bki->stream_id;










streamCG *group = NULL;
if (receiver->bpop.xread_group) {
group = streamLookupCG(s,
receiver->bpop.xread_group->ptr);


if (!group) {
addReplyError(receiver,
"-NOGROUP the consumer group this client "
"was blocked on no longer exists");
unblockClient(receiver);
continue;
} else {
*gt = group->last_id;
}
}

if (streamCompareID(&s->last_id, gt) > 0) {
streamID start = *gt;
streamIncrID(&start);


streamConsumer *consumer = NULL;
int noack = 0;

if (group) {
noack = receiver->bpop.xread_group_noack;
sds name = receiver->bpop.xread_consumer->ptr;
consumer = streamLookupConsumer(group,name,SLC_DEFAULT);
if (consumer == NULL) {
consumer = streamCreateConsumer(group,name,rl->key,
rl->db->id,SCC_DEFAULT);
if (noack) {
streamPropagateConsumerCreation(receiver,rl->key,
receiver->bpop.xread_group,
consumer->name);
}
}
}

client *old_client = server.current_client;
server.current_client = receiver;
monotime replyTimer;
elapsedStart(&replyTimer);




if (receiver->resp == 2) {
addReplyArrayLen(receiver,1);
addReplyArrayLen(receiver,2);
} else {
addReplyMapLen(receiver,1);
}
addReplyBulk(receiver,rl->key);

streamPropInfo pi = {
rl->key,
receiver->bpop.xread_group
};
streamReplyWithRange(receiver,s,&start,NULL,
receiver->bpop.xread_count,
0, group, consumer, noack, &pi);
updateStatsOnUnblock(receiver, 0, elapsedUs(replyTimer));




unblockClient(receiver);
afterCommand(receiver);
server.current_client = old_client;
}
}
}
}







void serveClientsBlockedOnKeyByModule(readyList *rl) {
dictEntry *de;



if (!server.blocked_clients_by_type[BLOCKED_MODULE]) return;



de = dictFind(rl->db->blocking_keys,rl->key);
if (de) {
list *clients = dictGetVal(de);
int numclients = listLength(clients);

while(numclients--) {
listNode *clientnode = listFirst(clients);
client *receiver = clientnode->value;






listRotateHeadToTail(clients);

if (receiver->btype != BLOCKED_MODULE) continue;







client *old_client = server.current_client;
server.current_client = receiver;
monotime replyTimer;
elapsedStart(&replyTimer);
if (!moduleTryServeClientBlockedOnKey(receiver, rl->key)) continue;
updateStatsOnUnblock(receiver, 0, elapsedUs(replyTimer));
moduleUnblockClient(receiver);
afterCommand(receiver);
server.current_client = old_client;
}
}
}






















void handleClientsBlockedOnKeys(void) {


serverAssert(server.also_propagate.numops == 0);
server.core_propagates = 1;

while(listLength(server.ready_keys) != 0) {
list *l;





l = server.ready_keys;
server.ready_keys = listCreate();

while(listLength(l) != 0) {
listNode *ln = listFirst(l);
readyList *rl = ln->value;



dictDelete(rl->db->ready_keys,rl->key);








server.fixed_time_expire++;
updateCachedTime(0);


robj *o = lookupKeyReadWithFlags(rl->db, rl->key, LOOKUP_NONOTIFY | LOOKUP_NOSTATS);
if (o != NULL) {
if (o->type == OBJ_LIST)
serveClientsBlockedOnListKey(o,rl);
else if (o->type == OBJ_ZSET)
serveClientsBlockedOnSortedSetKey(o,rl);
else if (o->type == OBJ_STREAM)
serveClientsBlockedOnStreamKey(o,rl);



serveClientsBlockedOnKeyByModule(rl);
} else {


if (server.also_propagate.numops > 0)
propagatePendingCommands();
}
server.fixed_time_expire--;


decrRefCount(rl->key);
zfree(rl);
listDelNode(l,ln);
}
listRelease(l);
}

serverAssert(server.core_propagates);

server.core_propagates = 0;
}































void blockForKeys(client *c, int btype, robj **keys, int numkeys, long count, mstime_t timeout, robj *target, struct blockPos *blockpos, streamID *ids) {
dictEntry *de;
list *l;
int j;

c->bpop.count = count;
c->bpop.timeout = timeout;
c->bpop.target = target;

if (blockpos != NULL) c->bpop.blockpos = *blockpos;

if (target != NULL) incrRefCount(target);

for (j = 0; j < numkeys; j++) {


bkinfo *bki = zmalloc(sizeof(*bki));
if (btype == BLOCKED_STREAM)
bki->stream_id = ids[j];


if (dictAdd(c->bpop.keys,keys[j],bki) != DICT_OK) {
zfree(bki);
continue;
}
incrRefCount(keys[j]);


de = dictFind(c->db->blocking_keys,keys[j]);
if (de == NULL) {
int retval;


l = listCreate();
retval = dictAdd(c->db->blocking_keys,keys[j],l);
incrRefCount(keys[j]);
serverAssertWithInfo(c,keys[j],retval == DICT_OK);
} else {
l = dictGetVal(de);
}
listAddNodeTail(l,c);
bki->listnode = listLast(l);
}
blockClient(c,btype);
}



void unblockClientWaitingData(client *c) {
dictEntry *de;
dictIterator *di;
list *l;

serverAssertWithInfo(c,NULL,dictSize(c->bpop.keys) != 0);
di = dictGetIterator(c->bpop.keys);

while((de = dictNext(di)) != NULL) {
robj *key = dictGetKey(de);
bkinfo *bki = dictGetVal(de);


l = dictFetchValue(c->db->blocking_keys,key);
serverAssertWithInfo(c,key,l != NULL);
listDelNode(l,bki->listnode);

if (listLength(l) == 0)
dictDelete(c->db->blocking_keys,key);
}
dictReleaseIterator(di);


dictEmpty(c->bpop.keys,NULL);
if (c->bpop.target) {
decrRefCount(c->bpop.target);
c->bpop.target = NULL;
}
if (c->bpop.xread_group) {
decrRefCount(c->bpop.xread_group);
decrRefCount(c->bpop.xread_consumer);
c->bpop.xread_group = NULL;
c->bpop.xread_consumer = NULL;
}
}

static int getBlockedTypeByType(int type) {
switch (type) {
case OBJ_LIST: return BLOCKED_LIST;
case OBJ_ZSET: return BLOCKED_ZSET;
case OBJ_MODULE: return BLOCKED_MODULE;
case OBJ_STREAM: return BLOCKED_STREAM;
default: return BLOCKED_NONE;
}
}








void signalKeyAsReady(redisDb *db, robj *key, int type) {
readyList *rl;


int btype = getBlockedTypeByType(type);
if (btype == BLOCKED_NONE) {

return;
}
if (!server.blocked_clients_by_type[btype] &&
!server.blocked_clients_by_type[BLOCKED_MODULE]) {




return;
}


if (dictFind(db->blocking_keys,key) == NULL) return;


if (dictFind(db->ready_keys,key) != NULL) return;


rl = zmalloc(sizeof(*rl));
rl->key = key;
rl->db = db;
incrRefCount(key);
listAddNodeTail(server.ready_keys,rl);




incrRefCount(key);
serverAssert(dictAdd(db->ready_keys,key,NULL) == DICT_OK);
}

