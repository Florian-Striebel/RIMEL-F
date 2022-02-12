#include "server.h"
#include "cluster.h"
#include "bio.h"
#include "functions.h"
#include <memory.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
void replicationDiscardCachedMaster(void);
void replicationResurrectCachedMaster(connection *conn);
void replicationSendAck(void);
void replicaPutOnline(client *slave);
void replicaStartCommandStream(client *slave);
int cancelReplicationHandshake(int reconnect);
int RDBGeneratedByReplication = 0;
char *replicationGetSlaveName(client *c) {
static char buf[NET_HOST_PORT_STR_LEN];
char ip[NET_IP_STR_LEN];
ip[0] = '\0';
buf[0] = '\0';
if (c->slave_addr ||
connPeerToString(c->conn,ip,sizeof(ip),NULL) != -1)
{
char *addr = c->slave_addr ? c->slave_addr : ip;
if (c->slave_listening_port)
anetFormatAddr(buf,sizeof(buf),addr,c->slave_listening_port);
else
snprintf(buf,sizeof(buf),"%s:<unknown-replica-port>",addr);
} else {
snprintf(buf,sizeof(buf),"client id #%llu",
(unsigned long long) c->id);
}
return buf;
}
int bg_unlink(const char *filename) {
int fd = open(filename,O_RDONLY|O_NONBLOCK);
if (fd == -1) {
return unlink(filename);
} else {
int retval = unlink(filename);
if (retval == -1) {
int old_errno = errno;
close(fd);
errno = old_errno;
return -1;
}
bioCreateCloseJob(fd);
return 0;
}
}
void createReplicationBacklog(void) {
serverAssert(server.repl_backlog == NULL);
server.repl_backlog = zmalloc(sizeof(replBacklog));
server.repl_backlog->ref_repl_buf_node = NULL;
server.repl_backlog->unindexed_count = 0;
server.repl_backlog->blocks_index = raxNew();
server.repl_backlog->histlen = 0;
server.repl_backlog->offset = server.master_repl_offset+1;
}
void resizeReplicationBacklog(void) {
if (server.repl_backlog_size < CONFIG_REPL_BACKLOG_MIN_SIZE)
server.repl_backlog_size = CONFIG_REPL_BACKLOG_MIN_SIZE;
if (server.repl_backlog)
incrementalTrimReplicationBacklog(REPL_BACKLOG_TRIM_BLOCKS_PER_CALL);
}
void freeReplicationBacklog(void) {
serverAssert(listLength(server.slaves) == 0);
if (server.repl_backlog == NULL) return;
if (server.repl_backlog->ref_repl_buf_node) {
replBufBlock *o = listNodeValue(
server.repl_backlog->ref_repl_buf_node);
serverAssert(o->refcount == 1);
o->refcount--;
}
freeReplicationBacklogRefMemAsync(server.repl_buffer_blocks,
server.repl_backlog->blocks_index);
resetReplicationBuffer();
zfree(server.repl_backlog);
server.repl_backlog = NULL;
}
void createReplicationBacklogIndex(listNode *ln) {
server.repl_backlog->unindexed_count++;
if (server.repl_backlog->unindexed_count >= REPL_BACKLOG_INDEX_PER_BLOCKS) {
replBufBlock *o = listNodeValue(ln);
uint64_t encoded_offset = htonu64(o->repl_offset);
raxInsert(server.repl_backlog->blocks_index,
(unsigned char*)&encoded_offset, sizeof(uint64_t),
ln, NULL);
server.repl_backlog->unindexed_count = 0;
}
}
void rebaseReplicationBuffer(long long base_repl_offset) {
raxFree(server.repl_backlog->blocks_index);
server.repl_backlog->blocks_index = raxNew();
server.repl_backlog->unindexed_count = 0;
listIter li;
listNode *ln;
listRewind(server.repl_buffer_blocks, &li);
while ((ln = listNext(&li))) {
replBufBlock *o = listNodeValue(ln);
o->repl_offset += base_repl_offset;
createReplicationBacklogIndex(ln);
}
}
void resetReplicationBuffer(void) {
server.repl_buffer_mem = 0;
server.repl_buffer_blocks = listCreate();
listSetFreeMethod(server.repl_buffer_blocks, (void (*)(void*))zfree);
}
int canFeedReplicaReplBuffer(client *replica) {
if (replica->flags & CLIENT_REPL_RDBONLY) return 0;
if (replica->replstate == SLAVE_STATE_WAIT_BGSAVE_START) return 0;
return 1;
}
int prepareReplicasToWrite(void) {
listIter li;
listNode *ln;
int prepared = 0;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
if (!canFeedReplicaReplBuffer(slave)) continue;
if (prepareClientToWrite(slave) == C_ERR) continue;
prepared++;
}
return prepared;
}
void feedReplicationBufferWithObject(robj *o) {
char llstr[LONG_STR_SIZE];
void *p;
size_t len;
if (o->encoding == OBJ_ENCODING_INT) {
len = ll2string(llstr,sizeof(llstr),(long)o->ptr);
p = llstr;
} else {
len = sdslen(o->ptr);
p = o->ptr;
}
feedReplicationBuffer(p,len);
}
void incrementalTrimReplicationBacklog(size_t max_blocks) {
serverAssert(server.repl_backlog != NULL);
size_t trimmed_blocks = 0;
while (server.repl_backlog->histlen > server.repl_backlog_size &&
trimmed_blocks < max_blocks)
{
if (listLength(server.repl_buffer_blocks) <= 1) break;
listNode *first = listFirst(server.repl_buffer_blocks);
serverAssert(first == server.repl_backlog->ref_repl_buf_node);
replBufBlock *fo = listNodeValue(first);
if (fo->refcount != 1) break;
if (server.repl_backlog->histlen - (long long)fo->size <=
server.repl_backlog_size) break;
fo->refcount--;
trimmed_blocks++;
server.repl_backlog->histlen -= fo->size;
listNode *next = listNextNode(first);
server.repl_backlog->ref_repl_buf_node = next;
serverAssert(server.repl_backlog->ref_repl_buf_node != NULL);
((replBufBlock *)listNodeValue(next))->refcount++;
uint64_t encoded_offset = htonu64(fo->repl_offset);
raxRemove(server.repl_backlog->blocks_index,
(unsigned char*)&encoded_offset, sizeof(uint64_t), NULL);
serverAssert(fo->refcount == 0 && fo->used == fo->size);
server.repl_buffer_mem -= (fo->size +
sizeof(listNode) + sizeof(replBufBlock));
listDelNode(server.repl_buffer_blocks, first);
}
server.repl_backlog->offset = server.master_repl_offset -
server.repl_backlog->histlen + 1;
}
void freeReplicaReferencedReplBuffer(client *replica) {
if (replica->ref_repl_buf_node != NULL) {
replBufBlock *o = listNodeValue(replica->ref_repl_buf_node);
serverAssert(o->refcount > 0);
o->refcount--;
incrementalTrimReplicationBacklog(REPL_BACKLOG_TRIM_BLOCKS_PER_CALL);
}
replica->ref_repl_buf_node = NULL;
replica->ref_block_pos = 0;
}
void feedReplicationBuffer(char *s, size_t len) {
static long long repl_block_id = 0;
if (server.repl_backlog == NULL) return;
server.master_repl_offset += len;
server.repl_backlog->histlen += len;
prepareReplicasToWrite();
size_t start_pos = 0;
listNode *start_node = NULL;
int add_new_block = 0;
listNode *ln = listLast(server.repl_buffer_blocks);
replBufBlock *tail = ln ? listNodeValue(ln) : NULL;
if (tail && tail->size > tail->used) {
start_node = listLast(server.repl_buffer_blocks);
start_pos = tail->used;
size_t avail = tail->size - tail->used;
size_t copy = (avail >= len) ? len : avail;
memcpy(tail->buf + tail->used, s, copy);
tail->used += copy;
s += copy;
len -= copy;
}
if (len) {
size_t usable_size;
size_t size = (len < PROTO_REPLY_CHUNK_BYTES) ? PROTO_REPLY_CHUNK_BYTES : len;
tail = zmalloc_usable(size + sizeof(replBufBlock), &usable_size);
tail->size = usable_size - sizeof(replBufBlock);
tail->used = len;
tail->refcount = 0;
tail->repl_offset = server.master_repl_offset - tail->used + 1;
tail->id = repl_block_id++;
memcpy(tail->buf, s, len);
listAddNodeTail(server.repl_buffer_blocks, tail);
server.repl_buffer_mem += (usable_size + sizeof(listNode));
add_new_block = 1;
if (start_node == NULL) {
start_node = listLast(server.repl_buffer_blocks);
start_pos = 0;
}
}
listIter li;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
if (!canFeedReplicaReplBuffer(slave)) continue;
if (slave->ref_repl_buf_node == NULL) {
slave->ref_repl_buf_node = start_node;
slave->ref_block_pos = start_pos;
((replBufBlock *)listNodeValue(start_node))->refcount++;
}
if (add_new_block) closeClientOnOutputBufferLimitReached(slave, 1);
}
if (server.repl_backlog->ref_repl_buf_node == NULL) {
server.repl_backlog->ref_repl_buf_node = start_node;
((replBufBlock *)listNodeValue(start_node))->refcount++;
serverAssert(add_new_block == 1 && start_pos == 0);
}
if (add_new_block) {
createReplicationBacklogIndex(listLast(server.repl_buffer_blocks));
}
incrementalTrimReplicationBacklog(REPL_BACKLOG_TRIM_BLOCKS_PER_CALL);
}
void replicationFeedSlaves(list *slaves, int dictid, robj **argv, int argc) {
int j, len;
char llstr[LONG_STR_SIZE];
serverAssert(dictid == -1 || (dictid >= 0 && dictid < server.dbnum));
if (server.masterhost != NULL) return;
if (server.repl_backlog == NULL && listLength(slaves) == 0) return;
serverAssert(!(listLength(slaves) != 0 && server.repl_backlog == NULL));
if (server.slaveseldb != dictid) {
robj *selectcmd;
if (dictid >= 0 && dictid < PROTO_SHARED_SELECT_CMDS) {
selectcmd = shared.select[dictid];
} else {
int dictid_len;
dictid_len = ll2string(llstr,sizeof(llstr),dictid);
selectcmd = createObject(OBJ_STRING,
sdscatprintf(sdsempty(),
"*2\r\n$6\r\nSELECT\r\n$%d\r\n%s\r\n",
dictid_len, llstr));
}
feedReplicationBufferWithObject(selectcmd);
if (dictid < 0 || dictid >= PROTO_SHARED_SELECT_CMDS)
decrRefCount(selectcmd);
server.slaveseldb = dictid;
}
char aux[LONG_STR_SIZE+3];
aux[0] = '*';
len = ll2string(aux+1,sizeof(aux)-1,argc);
aux[len+1] = '\r';
aux[len+2] = '\n';
feedReplicationBuffer(aux,len+3);
for (j = 0; j < argc; j++) {
long objlen = stringObjectLen(argv[j]);
aux[0] = '$';
len = ll2string(aux+1,sizeof(aux)-1,objlen);
aux[len+1] = '\r';
aux[len+2] = '\n';
feedReplicationBuffer(aux,len+3);
feedReplicationBufferWithObject(argv[j]);
feedReplicationBuffer(aux+len+1,2);
}
}
void showLatestBacklog(void) {
if (server.repl_backlog == NULL) return;
if (listLength(server.repl_buffer_blocks) == 0) return;
size_t dumplen = 256;
if (server.repl_backlog->histlen < (long long)dumplen)
dumplen = server.repl_backlog->histlen;
sds dump = sdsempty();
listNode *node = listLast(server.repl_buffer_blocks);
while(dumplen) {
if (node == NULL) break;
replBufBlock *o = listNodeValue(node);
size_t thislen = o->used >= dumplen ? dumplen : o->used;
sds head = sdscatrepr(sdsempty(), o->buf+o->used-thislen, thislen);
sds tmp = sdscatsds(head, dump);
sdsfree(dump);
dump = tmp;
dumplen -= thislen;
node = listPrevNode(node);
}
serverLog(LL_WARNING,"Latest backlog is: '%s'", dump);
sdsfree(dump);
}
#include <ctype.h>
void replicationFeedStreamFromMasterStream(char *buf, size_t buflen) {
if (0) {
printf("%zu:",buflen);
for (size_t j = 0; j < buflen; j++) {
printf("%c", isprint(buf[j]) ? buf[j] : '.');
}
printf("\n");
}
if (listLength(server.slaves)) serverAssert(server.repl_backlog != NULL);
if (server.repl_backlog) feedReplicationBuffer(buf,buflen);
}
void replicationFeedMonitors(client *c, list *monitors, int dictid, robj **argv, int argc) {
if (!(listLength(server.monitors) && !server.loading)) return;
listNode *ln;
listIter li;
int j;
sds cmdrepr = sdsnew("+");
robj *cmdobj;
struct timeval tv;
gettimeofday(&tv,NULL);
cmdrepr = sdscatprintf(cmdrepr,"%ld.%06ld ",(long)tv.tv_sec,(long)tv.tv_usec);
if (c->flags & CLIENT_SCRIPT) {
cmdrepr = sdscatprintf(cmdrepr,"[%d lua] ",dictid);
} else if (c->flags & CLIENT_UNIX_SOCKET) {
cmdrepr = sdscatprintf(cmdrepr,"[%d unix:%s] ",dictid,server.unixsocket);
} else {
cmdrepr = sdscatprintf(cmdrepr,"[%d %s] ",dictid,getClientPeerId(c));
}
for (j = 0; j < argc; j++) {
if (argv[j]->encoding == OBJ_ENCODING_INT) {
cmdrepr = sdscatprintf(cmdrepr, "\"%ld\"", (long)argv[j]->ptr);
} else {
cmdrepr = sdscatrepr(cmdrepr,(char*)argv[j]->ptr,
sdslen(argv[j]->ptr));
}
if (j != argc-1)
cmdrepr = sdscatlen(cmdrepr," ",1);
}
cmdrepr = sdscatlen(cmdrepr,"\r\n",2);
cmdobj = createObject(OBJ_STRING,cmdrepr);
listRewind(monitors,&li);
while((ln = listNext(&li))) {
client *monitor = ln->value;
addReply(monitor,cmdobj);
updateClientMemUsage(c);
}
decrRefCount(cmdobj);
}
long long addReplyReplicationBacklog(client *c, long long offset) {
long long skip;
serverLog(LL_DEBUG, "[PSYNC] Replica request offset: %lld", offset);
if (server.repl_backlog->histlen == 0) {
serverLog(LL_DEBUG, "[PSYNC] Backlog history len is zero");
return 0;
}
serverLog(LL_DEBUG, "[PSYNC] Backlog size: %lld",
server.repl_backlog_size);
serverLog(LL_DEBUG, "[PSYNC] First byte: %lld",
server.repl_backlog->offset);
serverLog(LL_DEBUG, "[PSYNC] History len: %lld",
server.repl_backlog->histlen);
skip = offset - server.repl_backlog->offset;
serverLog(LL_DEBUG, "[PSYNC] Skipping: %lld", skip);
listNode *node = NULL;
if (raxSize(server.repl_backlog->blocks_index) > 0) {
uint64_t encoded_offset = htonu64(offset);
raxIterator ri;
raxStart(&ri, server.repl_backlog->blocks_index);
raxSeek(&ri, ">", (unsigned char*)&encoded_offset, sizeof(uint64_t));
if (raxEOF(&ri)) {
raxSeek(&ri, "$", NULL, 0);
raxPrev(&ri);
node = (listNode *)ri.data;
} else {
raxPrev(&ri);
if (raxPrev(&ri))
node = (listNode *)ri.data;
else
node = server.repl_backlog->ref_repl_buf_node;
}
raxStop(&ri);
} else {
node = server.repl_backlog->ref_repl_buf_node;
}
while (node != NULL) {
replBufBlock *o = listNodeValue(node);
if (o->repl_offset + (long long)o->used >= offset) break;
node = listNextNode(node);
}
serverAssert(node != NULL);
prepareClientToWrite(c);
replBufBlock *o = listNodeValue(node);
o->refcount++;
c->ref_repl_buf_node = node;
c->ref_block_pos = offset - o->repl_offset;
return server.repl_backlog->histlen - skip;
}
long long getPsyncInitialOffset(void) {
return server.master_repl_offset;
}
int replicationSetupSlaveForFullResync(client *slave, long long offset) {
char buf[128];
int buflen;
slave->psync_initial_offset = offset;
slave->replstate = SLAVE_STATE_WAIT_BGSAVE_END;
server.slaveseldb = -1;
if (!(slave->flags & CLIENT_PRE_PSYNC)) {
buflen = snprintf(buf,sizeof(buf),"+FULLRESYNC %s %lld\r\n",
server.replid,offset);
if (connWrite(slave->conn,buf,buflen) != buflen) {
freeClientAsync(slave);
return C_ERR;
}
}
return C_OK;
}
int masterTryPartialResynchronization(client *c) {
long long psync_offset, psync_len;
char *master_replid = c->argv[1]->ptr;
char buf[128];
int buflen;
if (getLongLongFromObjectOrReply(c,c->argv[2],&psync_offset,NULL) !=
C_OK) goto need_full_resync;
if (strcasecmp(master_replid, server.replid) &&
(strcasecmp(master_replid, server.replid2) ||
psync_offset > server.second_replid_offset))
{
if (master_replid[0] != '?') {
if (strcasecmp(master_replid, server.replid) &&
strcasecmp(master_replid, server.replid2))
{
serverLog(LL_NOTICE,"Partial resynchronization not accepted: "
"Replication ID mismatch (Replica asked for '%s', my "
"replication IDs are '%s' and '%s')",
master_replid, server.replid, server.replid2);
} else {
serverLog(LL_NOTICE,"Partial resynchronization not accepted: "
"Requested offset for second ID was %lld, but I can reply "
"up to %lld", psync_offset, server.second_replid_offset);
}
} else {
serverLog(LL_NOTICE,"Full resync requested by replica %s",
replicationGetSlaveName(c));
}
goto need_full_resync;
}
if (!server.repl_backlog ||
psync_offset < server.repl_backlog->offset ||
psync_offset > (server.repl_backlog->offset + server.repl_backlog->histlen))
{
serverLog(LL_NOTICE,
"Unable to partial resync with replica %s for lack of backlog (Replica request was: %lld).", replicationGetSlaveName(c), psync_offset);
if (psync_offset > server.master_repl_offset) {
serverLog(LL_WARNING,
"Warning: replica %s tried to PSYNC with an offset that is greater than the master replication offset.", replicationGetSlaveName(c));
}
goto need_full_resync;
}
c->flags |= CLIENT_SLAVE;
c->replstate = SLAVE_STATE_ONLINE;
c->repl_ack_time = server.unixtime;
c->repl_start_cmd_stream_on_ack = 0;
listAddNodeTail(server.slaves,c);
if (c->slave_capa & SLAVE_CAPA_PSYNC2) {
buflen = snprintf(buf,sizeof(buf),"+CONTINUE %s\r\n", server.replid);
} else {
buflen = snprintf(buf,sizeof(buf),"+CONTINUE\r\n");
}
if (connWrite(c->conn,buf,buflen) != buflen) {
freeClientAsync(c);
return C_OK;
}
psync_len = addReplyReplicationBacklog(c,psync_offset);
serverLog(LL_NOTICE,
"Partial resynchronization request from %s accepted. Sending %lld bytes of backlog starting from offset %lld.",
replicationGetSlaveName(c),
psync_len, psync_offset);
refreshGoodSlavesCount();
moduleFireServerEvent(REDISMODULE_EVENT_REPLICA_CHANGE,
REDISMODULE_SUBEVENT_REPLICA_CHANGE_ONLINE,
NULL);
return C_OK;
need_full_resync:
return C_ERR;
}
int startBgsaveForReplication(int mincapa, int req) {
int retval;
int socket_target = 0;
listIter li;
listNode *ln;
socket_target = (server.repl_diskless_sync || req & SLAVE_REQ_RDB_MASK) && (mincapa & SLAVE_CAPA_EOF);
serverAssert(socket_target || !(req & SLAVE_REQ_RDB_MASK));
serverLog(LL_NOTICE,"Starting BGSAVE for SYNC with target: %s",
socket_target ? "replicas sockets" : "disk");
rdbSaveInfo rsi, *rsiptr;
rsiptr = rdbPopulateSaveInfo(&rsi);
if (rsiptr) {
if (socket_target)
retval = rdbSaveToSlavesSockets(req,rsiptr);
else
retval = rdbSaveBackground(req,server.rdb_filename,rsiptr);
} else {
serverLog(LL_WARNING,"BGSAVE for replication: replication information not available, can't generate the RDB file right now. Try later.");
retval = C_ERR;
}
if (retval == C_OK && !socket_target && server.rdb_del_sync_files)
RDBGeneratedByReplication = 1;
if (retval == C_ERR) {
serverLog(LL_WARNING,"BGSAVE for replication failed");
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
if (slave->replstate == SLAVE_STATE_WAIT_BGSAVE_START) {
slave->replstate = REPL_STATE_NONE;
slave->flags &= ~CLIENT_SLAVE;
listDelNode(server.slaves,ln);
addReplyError(slave,
"BGSAVE failed, replication can't continue");
slave->flags |= CLIENT_CLOSE_AFTER_REPLY;
}
}
return retval;
}
if (!socket_target) {
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
if (slave->replstate == SLAVE_STATE_WAIT_BGSAVE_START) {
if (slave->slave_req != req)
continue;
replicationSetupSlaveForFullResync(slave, getPsyncInitialOffset());
}
}
}
return retval;
}
void syncCommand(client *c) {
if (c->flags & CLIENT_SLAVE) return;
if (c->argc > 3 && !strcasecmp(c->argv[0]->ptr,"psync") &&
!strcasecmp(c->argv[3]->ptr,"failover"))
{
serverLog(LL_WARNING, "Failover request received for replid %s.",
(unsigned char *)c->argv[1]->ptr);
if (!server.masterhost) {
addReplyError(c, "PSYNC FAILOVER can't be sent to a master.");
return;
}
if (!strcasecmp(c->argv[1]->ptr,server.replid)) {
replicationUnsetMaster();
sds client = catClientInfoString(sdsempty(),c);
serverLog(LL_NOTICE,
"MASTER MODE enabled (failover request from '%s')",client);
sdsfree(client);
} else {
addReplyError(c, "PSYNC FAILOVER replid must match my replid.");
return;
}
}
if (server.failover_state != NO_FAILOVER) {
addReplyError(c,"-NOMASTERLINK Can't SYNC while failing over");
return;
}
if (server.masterhost && server.repl_state != REPL_STATE_CONNECTED) {
addReplyError(c,"-NOMASTERLINK Can't SYNC while not connected with my master");
return;
}
if (clientHasPendingReplies(c)) {
addReplyError(c,"SYNC and PSYNC are invalid with pending output");
return;
}
if (c->slave_req & SLAVE_REQ_RDB_MASK && !(c->slave_capa & SLAVE_CAPA_EOF)) {
addReplyError(c,"Filtered replica requires EOF capability");
return;
}
serverLog(LL_NOTICE,"Replica %s asks for synchronization",
replicationGetSlaveName(c));
if (!strcasecmp(c->argv[0]->ptr,"psync")) {
if (masterTryPartialResynchronization(c) == C_OK) {
server.stat_sync_partial_ok++;
return;
} else {
char *master_replid = c->argv[1]->ptr;
if (master_replid[0] != '?') server.stat_sync_partial_err++;
}
} else {
c->flags |= CLIENT_PRE_PSYNC;
}
server.stat_sync_full++;
c->replstate = SLAVE_STATE_WAIT_BGSAVE_START;
if (server.repl_disable_tcp_nodelay)
connDisableTcpNoDelay(c->conn);
c->repldbfd = -1;
c->flags |= CLIENT_SLAVE;
listAddNodeTail(server.slaves,c);
if (listLength(server.slaves) == 1 && server.repl_backlog == NULL) {
changeReplicationId();
clearReplicationId2();
createReplicationBacklog();
serverLog(LL_NOTICE,"Replication backlog created, my new "
"replication IDs are '%s' and '%s'",
server.replid, server.replid2);
}
if (server.child_type == CHILD_TYPE_RDB &&
server.rdb_child_type == RDB_CHILD_TYPE_DISK)
{
client *slave;
listNode *ln;
listIter li;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
slave = ln->value;
if (slave->replstate == SLAVE_STATE_WAIT_BGSAVE_END &&
(!(slave->flags & CLIENT_REPL_RDBONLY) ||
(c->flags & CLIENT_REPL_RDBONLY)))
break;
}
if (ln && ((c->slave_capa & slave->slave_capa) == slave->slave_capa) &&
c->slave_req == slave->slave_req) {
if (!(c->flags & CLIENT_REPL_RDBONLY))
copyReplicaOutputBuffer(c,slave);
replicationSetupSlaveForFullResync(c,slave->psync_initial_offset);
serverLog(LL_NOTICE,"Waiting for end of BGSAVE for SYNC");
} else {
serverLog(LL_NOTICE,"Can't attach the replica to the current BGSAVE. Waiting for next BGSAVE for SYNC");
}
} else if (server.child_type == CHILD_TYPE_RDB &&
server.rdb_child_type == RDB_CHILD_TYPE_SOCKET)
{
serverLog(LL_NOTICE,"Current BGSAVE has socket target. Waiting for next BGSAVE for SYNC");
} else {
if (server.repl_diskless_sync && (c->slave_capa & SLAVE_CAPA_EOF) &&
server.repl_diskless_sync_delay)
{
serverLog(LL_NOTICE,"Delay next BGSAVE for diskless SYNC");
} else {
if (!hasActiveChildProcess()) {
startBgsaveForReplication(c->slave_capa, c->slave_req);
} else {
serverLog(LL_NOTICE,
"No BGSAVE in progress, but another BG operation is active. "
"BGSAVE for replication delayed");
}
}
}
return;
}
void replconfCommand(client *c) {
int j;
if ((c->argc % 2) == 0) {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
for (j = 1; j < c->argc; j+=2) {
if (!strcasecmp(c->argv[j]->ptr,"listening-port")) {
long port;
if ((getLongFromObjectOrReply(c,c->argv[j+1],
&port,NULL) != C_OK))
return;
c->slave_listening_port = port;
} else if (!strcasecmp(c->argv[j]->ptr,"ip-address")) {
sds addr = c->argv[j+1]->ptr;
if (sdslen(addr) < NET_HOST_STR_LEN) {
if (c->slave_addr) sdsfree(c->slave_addr);
c->slave_addr = sdsdup(addr);
} else {
addReplyErrorFormat(c,"REPLCONF ip-address provided by "
"replica instance is too long: %zd bytes", sdslen(addr));
return;
}
} else if (!strcasecmp(c->argv[j]->ptr,"capa")) {
if (!strcasecmp(c->argv[j+1]->ptr,"eof"))
c->slave_capa |= SLAVE_CAPA_EOF;
else if (!strcasecmp(c->argv[j+1]->ptr,"psync2"))
c->slave_capa |= SLAVE_CAPA_PSYNC2;
} else if (!strcasecmp(c->argv[j]->ptr,"ack")) {
long long offset;
if (!(c->flags & CLIENT_SLAVE)) return;
if ((getLongLongFromObject(c->argv[j+1], &offset) != C_OK))
return;
if (offset > c->repl_ack_off)
c->repl_ack_off = offset;
c->repl_ack_time = server.unixtime;
if (server.child_type == CHILD_TYPE_RDB && c->replstate == SLAVE_STATE_WAIT_BGSAVE_END)
checkChildrenDone();
if (c->repl_start_cmd_stream_on_ack && c->replstate == SLAVE_STATE_ONLINE)
replicaStartCommandStream(c);
return;
} else if (!strcasecmp(c->argv[j]->ptr,"getack")) {
if (server.masterhost && server.master) replicationSendAck();
return;
} else if (!strcasecmp(c->argv[j]->ptr,"rdb-only")) {
long rdb_only = 0;
if (getRangeLongFromObjectOrReply(c,c->argv[j+1],
0,1,&rdb_only,NULL) != C_OK)
return;
if (rdb_only == 1) c->flags |= CLIENT_REPL_RDBONLY;
else c->flags &= ~CLIENT_REPL_RDBONLY;
} else if (!strcasecmp(c->argv[j]->ptr,"rdb-filter-only")) {
int filter_count, i;
sds *filters;
if (!(filters = sdssplitargs(c->argv[j+1]->ptr, &filter_count))) {
addReplyErrorFormat(c, "Missing rdb-filter-only values");
return;
}
c->slave_req |= SLAVE_REQ_RDB_EXCLUDE_DATA;
c->slave_req |= SLAVE_REQ_RDB_EXCLUDE_FUNCTIONS;
for (i = 0; i < filter_count; i++) {
if (!strcasecmp(filters[i], "functions"))
c->slave_req &= ~SLAVE_REQ_RDB_EXCLUDE_FUNCTIONS;
else {
addReplyErrorFormat(c, "Unsupported rdb-filter-only option: %s", (char*)filters[i]);
sdsfreesplitres(filters, filter_count);
return;
}
}
sdsfreesplitres(filters, filter_count);
} else {
addReplyErrorFormat(c,"Unrecognized REPLCONF option: %s",
(char*)c->argv[j]->ptr);
return;
}
}
addReply(c,shared.ok);
}
void replicaPutOnline(client *slave) {
if (slave->flags & CLIENT_REPL_RDBONLY) {
return;
}
slave->replstate = SLAVE_STATE_ONLINE;
slave->repl_ack_time = server.unixtime;
refreshGoodSlavesCount();
moduleFireServerEvent(REDISMODULE_EVENT_REPLICA_CHANGE,
REDISMODULE_SUBEVENT_REPLICA_CHANGE_ONLINE,
NULL);
serverLog(LL_NOTICE,"Synchronization with replica %s succeeded",
replicationGetSlaveName(slave));
}
void replicaStartCommandStream(client *slave) {
slave->repl_start_cmd_stream_on_ack = 0;
if (slave->flags & CLIENT_REPL_RDBONLY) {
serverLog(LL_NOTICE,
"Close the connection with replica %s as RDB transfer is complete",
replicationGetSlaveName(slave));
freeClientAsync(slave);
return;
}
clientInstallWriteHandler(slave);
}
void removeRDBUsedToSyncReplicas(void) {
if (!server.rdb_del_sync_files) {
RDBGeneratedByReplication = 0;
return;
}
if (allPersistenceDisabled() && RDBGeneratedByReplication) {
client *slave;
listNode *ln;
listIter li;
int delrdb = 1;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
slave = ln->value;
if (slave->replstate == SLAVE_STATE_WAIT_BGSAVE_START ||
slave->replstate == SLAVE_STATE_WAIT_BGSAVE_END ||
slave->replstate == SLAVE_STATE_SEND_BULK)
{
delrdb = 0;
break;
}
}
if (delrdb) {
struct stat sb;
if (lstat(server.rdb_filename,&sb) != -1) {
RDBGeneratedByReplication = 0;
serverLog(LL_NOTICE,
"Removing the RDB file used to feed replicas "
"in a persistence-less instance");
bg_unlink(server.rdb_filename);
}
}
}
}
void sendBulkToSlave(connection *conn) {
client *slave = connGetPrivateData(conn);
char buf[PROTO_IOBUF_LEN];
ssize_t nwritten, buflen;
if (slave->replpreamble) {
nwritten = connWrite(conn,slave->replpreamble,sdslen(slave->replpreamble));
if (nwritten == -1) {
serverLog(LL_WARNING,
"Write error sending RDB preamble to replica: %s",
connGetLastError(conn));
freeClient(slave);
return;
}
atomicIncr(server.stat_net_output_bytes, nwritten);
sdsrange(slave->replpreamble,nwritten,-1);
if (sdslen(slave->replpreamble) == 0) {
sdsfree(slave->replpreamble);
slave->replpreamble = NULL;
} else {
return;
}
}
lseek(slave->repldbfd,slave->repldboff,SEEK_SET);
buflen = read(slave->repldbfd,buf,PROTO_IOBUF_LEN);
if (buflen <= 0) {
serverLog(LL_WARNING,"Read error sending DB to replica: %s",
(buflen == 0) ? "premature EOF" : strerror(errno));
freeClient(slave);
return;
}
if ((nwritten = connWrite(conn,buf,buflen)) == -1) {
if (connGetState(conn) != CONN_STATE_CONNECTED) {
serverLog(LL_WARNING,"Write error sending DB to replica: %s",
connGetLastError(conn));
freeClient(slave);
}
return;
}
slave->repldboff += nwritten;
atomicIncr(server.stat_net_output_bytes, nwritten);
if (slave->repldboff == slave->repldbsize) {
close(slave->repldbfd);
slave->repldbfd = -1;
connSetWriteHandler(slave->conn,NULL);
replicaPutOnline(slave);
replicaStartCommandStream(slave);
}
}
void rdbPipeWriteHandlerConnRemoved(struct connection *conn) {
if (!connHasWriteHandler(conn))
return;
connSetWriteHandler(conn, NULL);
client *slave = connGetPrivateData(conn);
slave->repl_last_partial_write = 0;
server.rdb_pipe_numconns_writing--;
if (server.rdb_pipe_numconns_writing == 0) {
if (aeCreateFileEvent(server.el, server.rdb_pipe_read, AE_READABLE, rdbPipeReadHandler,NULL) == AE_ERR) {
serverPanic("Unrecoverable error creating server.rdb_pipe_read file event.");
}
}
}
void rdbPipeWriteHandler(struct connection *conn) {
serverAssert(server.rdb_pipe_bufflen>0);
client *slave = connGetPrivateData(conn);
int nwritten;
if ((nwritten = connWrite(conn, server.rdb_pipe_buff + slave->repldboff,
server.rdb_pipe_bufflen - slave->repldboff)) == -1)
{
if (connGetState(conn) == CONN_STATE_CONNECTED)
return;
serverLog(LL_WARNING,"Write error sending DB to replica: %s",
connGetLastError(conn));
freeClient(slave);
return;
} else {
slave->repldboff += nwritten;
atomicIncr(server.stat_net_output_bytes, nwritten);
if (slave->repldboff < server.rdb_pipe_bufflen) {
slave->repl_last_partial_write = server.unixtime;
return;
}
}
rdbPipeWriteHandlerConnRemoved(conn);
}
void rdbPipeReadHandler(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask) {
UNUSED(mask);
UNUSED(clientData);
UNUSED(eventLoop);
int i;
if (!server.rdb_pipe_buff)
server.rdb_pipe_buff = zmalloc(PROTO_IOBUF_LEN);
serverAssert(server.rdb_pipe_numconns_writing==0);
while (1) {
server.rdb_pipe_bufflen = read(fd, server.rdb_pipe_buff, PROTO_IOBUF_LEN);
if (server.rdb_pipe_bufflen < 0) {
if (errno == EAGAIN || errno == EWOULDBLOCK)
return;
serverLog(LL_WARNING,"Diskless rdb transfer, read error sending DB to replicas: %s", strerror(errno));
for (i=0; i < server.rdb_pipe_numconns; i++) {
connection *conn = server.rdb_pipe_conns[i];
if (!conn)
continue;
client *slave = connGetPrivateData(conn);
freeClient(slave);
server.rdb_pipe_conns[i] = NULL;
}
killRDBChild();
return;
}
if (server.rdb_pipe_bufflen == 0) {
int stillUp = 0;
aeDeleteFileEvent(server.el, server.rdb_pipe_read, AE_READABLE);
for (i=0; i < server.rdb_pipe_numconns; i++)
{
connection *conn = server.rdb_pipe_conns[i];
if (!conn)
continue;
stillUp++;
}
serverLog(LL_WARNING,"Diskless rdb transfer, done reading from pipe, %d replicas still up.", stillUp);
close(server.rdb_child_exit_pipe);
server.rdb_child_exit_pipe = -1;
return;
}
int stillAlive = 0;
for (i=0; i < server.rdb_pipe_numconns; i++)
{
int nwritten;
connection *conn = server.rdb_pipe_conns[i];
if (!conn)
continue;
client *slave = connGetPrivateData(conn);
if ((nwritten = connWrite(conn, server.rdb_pipe_buff, server.rdb_pipe_bufflen)) == -1) {
if (connGetState(conn) != CONN_STATE_CONNECTED) {
serverLog(LL_WARNING,"Diskless rdb transfer, write error sending DB to replica: %s",
connGetLastError(conn));
freeClient(slave);
server.rdb_pipe_conns[i] = NULL;
continue;
}
slave->repldboff = 0;
} else {
slave->repldboff = nwritten;
atomicIncr(server.stat_net_output_bytes, nwritten);
}
if (nwritten != server.rdb_pipe_bufflen) {
slave->repl_last_partial_write = server.unixtime;
server.rdb_pipe_numconns_writing++;
connSetWriteHandler(conn, rdbPipeWriteHandler);
}
stillAlive++;
}
if (stillAlive == 0) {
serverLog(LL_WARNING,"Diskless rdb transfer, last replica dropped, killing fork child.");
killRDBChild();
}
if (server.rdb_pipe_numconns_writing || stillAlive == 0) {
aeDeleteFileEvent(server.el, server.rdb_pipe_read, AE_READABLE);
break;
}
}
}
void updateSlavesWaitingBgsave(int bgsaveerr, int type) {
listNode *ln;
listIter li;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
if (slave->replstate == SLAVE_STATE_WAIT_BGSAVE_END) {
struct redis_stat buf;
if (bgsaveerr != C_OK) {
freeClient(slave);
serverLog(LL_WARNING,"SYNC failed. BGSAVE child returned an error");
continue;
}
if (type == RDB_CHILD_TYPE_SOCKET) {
serverLog(LL_NOTICE,
"Streamed RDB transfer with replica %s succeeded (socket). Waiting for REPLCONF ACK from slave to enable streaming",
replicationGetSlaveName(slave));
replicaPutOnline(slave);
slave->repl_start_cmd_stream_on_ack = 1;
} else {
if ((slave->repldbfd = open(server.rdb_filename,O_RDONLY)) == -1 ||
redis_fstat(slave->repldbfd,&buf) == -1) {
freeClient(slave);
serverLog(LL_WARNING,"SYNC failed. Can't open/stat DB after BGSAVE: %s", strerror(errno));
continue;
}
slave->repldboff = 0;
slave->repldbsize = buf.st_size;
slave->replstate = SLAVE_STATE_SEND_BULK;
slave->replpreamble = sdscatprintf(sdsempty(),"$%lld\r\n",
(unsigned long long) slave->repldbsize);
connSetWriteHandler(slave->conn,NULL);
if (connSetWriteHandler(slave->conn,sendBulkToSlave) == C_ERR) {
freeClient(slave);
continue;
}
}
}
}
}
void changeReplicationId(void) {
getRandomHexChars(server.replid,CONFIG_RUN_ID_SIZE);
server.replid[CONFIG_RUN_ID_SIZE] = '\0';
}
void clearReplicationId2(void) {
memset(server.replid2,'0',sizeof(server.replid));
server.replid2[CONFIG_RUN_ID_SIZE] = '\0';
server.second_replid_offset = -1;
}
void shiftReplicationId(void) {
memcpy(server.replid2,server.replid,sizeof(server.replid));
server.second_replid_offset = server.master_repl_offset+1;
changeReplicationId();
serverLog(LL_WARNING,"Setting secondary replication ID to %s, valid up to offset: %lld. New replication ID is %s", server.replid2, server.second_replid_offset, server.replid);
}
int slaveIsInHandshakeState(void) {
return server.repl_state >= REPL_STATE_RECEIVE_PING_REPLY &&
server.repl_state <= REPL_STATE_RECEIVE_PSYNC_REPLY;
}
void replicationSendNewlineToMaster(void) {
static time_t newline_sent;
if (time(NULL) != newline_sent) {
newline_sent = time(NULL);
if (server.repl_transfer_s) connWrite(server.repl_transfer_s, "\n", 1);
}
}
void replicationEmptyDbCallback(dict *d) {
UNUSED(d);
if (server.repl_state == REPL_STATE_TRANSFER)
replicationSendNewlineToMaster();
}
void replicationCreateMasterClient(connection *conn, int dbid) {
server.master = createClient(conn);
if (conn)
connSetReadHandler(server.master->conn, readQueryFromClient);
server.master->flags |= CLIENT_MASTER;
server.master->authenticated = 1;
server.master->reploff = server.master_initial_offset;
server.master->read_reploff = server.master->reploff;
server.master->user = NULL;
memcpy(server.master->replid, server.master_replid,
sizeof(server.master_replid));
if (server.master->reploff == -1)
server.master->flags |= CLIENT_PRE_PSYNC;
if (dbid != -1) selectDb(server.master,dbid);
}
void restartAOFAfterSYNC() {
unsigned int tries, max_tries = 10;
for (tries = 0; tries < max_tries; ++tries) {
if (startAppendOnly() == C_OK) break;
serverLog(LL_WARNING,
"Failed enabling the AOF after successful master synchronization! "
"Trying it again in one second.");
sleep(1);
}
if (tries == max_tries) {
serverLog(LL_WARNING,
"FATAL: this replica instance finished the synchronization with "
"its master, but the AOF can't be turned on. Exiting now.");
exit(1);
}
}
static int useDisklessLoad() {
int enabled = server.repl_diskless_load == REPL_DISKLESS_LOAD_SWAPDB ||
(server.repl_diskless_load == REPL_DISKLESS_LOAD_WHEN_DB_EMPTY && dbTotalServerKeyCount()==0);
if (enabled) {
if (!moduleAllDatatypesHandleErrors()) {
serverLog(LL_WARNING,
"Skipping diskless-load because there are modules that don't handle read errors.");
enabled = 0;
}
else if (server.repl_diskless_load == REPL_DISKLESS_LOAD_SWAPDB && !moduleAllModulesHandleReplAsyncLoad()) {
serverLog(LL_WARNING,
"Skipping diskless-load because there are modules that are not aware of async replication.");
enabled = 0;
}
}
return enabled;
}
redisDb *disklessLoadInitTempDb(void) {
return initTempDb();
}
void disklessLoadDiscardTempDb(redisDb *tempDb) {
discardTempDb(tempDb, replicationEmptyDbCallback);
}
void replicationAttachToNewMaster() {
serverAssert(server.master == NULL);
replicationDiscardCachedMaster();
disconnectSlaves();
freeReplicationBacklog();
}
#define REPL_MAX_WRITTEN_BEFORE_FSYNC (1024*1024*8)
void readSyncBulkPayload(connection *conn) {
char buf[PROTO_IOBUF_LEN];
ssize_t nread, readlen, nwritten;
int use_diskless_load = useDisklessLoad();
redisDb *diskless_load_tempDb = NULL;
functionsLibCtx* temp_functions_lib_ctx = NULL;
int empty_db_flags = server.repl_slave_lazy_flush ? EMPTYDB_ASYNC :
EMPTYDB_NO_FLAGS;
off_t left;
static char eofmark[CONFIG_RUN_ID_SIZE];
static char lastbytes[CONFIG_RUN_ID_SIZE];
static int usemark = 0;
if (server.repl_transfer_size == -1) {
if (connSyncReadLine(conn,buf,1024,server.repl_syncio_timeout*1000) == -1) {
serverLog(LL_WARNING,
"I/O error reading bulk count from MASTER: %s",
strerror(errno));
goto error;
}
if (buf[0] == '-') {
serverLog(LL_WARNING,
"MASTER aborted replication with an error: %s",
buf+1);
goto error;
} else if (buf[0] == '\0') {
server.repl_transfer_lastio = server.unixtime;
return;
} else if (buf[0] != '$') {
serverLog(LL_WARNING,"Bad protocol from MASTER, the first byte is not '$' (we received '%s'), are you sure the host and port are right?", buf);
goto error;
}
if (strncmp(buf+1,"EOF:",4) == 0 && strlen(buf+5) >= CONFIG_RUN_ID_SIZE) {
usemark = 1;
memcpy(eofmark,buf+5,CONFIG_RUN_ID_SIZE);
memset(lastbytes,0,CONFIG_RUN_ID_SIZE);
server.repl_transfer_size = 0;
serverLog(LL_NOTICE,
"MASTER <-> REPLICA sync: receiving streamed RDB from master with EOF %s",
use_diskless_load? "to parser":"to disk");
} else {
usemark = 0;
server.repl_transfer_size = strtol(buf+1,NULL,10);
serverLog(LL_NOTICE,
"MASTER <-> REPLICA sync: receiving %lld bytes from master %s",
(long long) server.repl_transfer_size,
use_diskless_load? "to parser":"to disk");
}
return;
}
if (!use_diskless_load) {
if (usemark) {
readlen = sizeof(buf);
} else {
left = server.repl_transfer_size - server.repl_transfer_read;
readlen = (left < (signed)sizeof(buf)) ? left : (signed)sizeof(buf);
}
nread = connRead(conn,buf,readlen);
if (nread <= 0) {
if (connGetState(conn) == CONN_STATE_CONNECTED) {
return;
}
serverLog(LL_WARNING,"I/O error trying to sync with MASTER: %s",
(nread == -1) ? strerror(errno) : "connection lost");
cancelReplicationHandshake(1);
return;
}
atomicIncr(server.stat_net_input_bytes, nread);
int eof_reached = 0;
if (usemark) {
if (nread >= CONFIG_RUN_ID_SIZE) {
memcpy(lastbytes,buf+nread-CONFIG_RUN_ID_SIZE,
CONFIG_RUN_ID_SIZE);
} else {
int rem = CONFIG_RUN_ID_SIZE-nread;
memmove(lastbytes,lastbytes+nread,rem);
memcpy(lastbytes+rem,buf,nread);
}
if (memcmp(lastbytes,eofmark,CONFIG_RUN_ID_SIZE) == 0)
eof_reached = 1;
}
server.repl_transfer_lastio = server.unixtime;
if ((nwritten = write(server.repl_transfer_fd,buf,nread)) != nread) {
serverLog(LL_WARNING,
"Write error or short write writing to the DB dump file "
"needed for MASTER <-> REPLICA synchronization: %s",
(nwritten == -1) ? strerror(errno) : "short write");
goto error;
}
server.repl_transfer_read += nread;
if (usemark && eof_reached) {
if (ftruncate(server.repl_transfer_fd,
server.repl_transfer_read - CONFIG_RUN_ID_SIZE) == -1)
{
serverLog(LL_WARNING,
"Error truncating the RDB file received from the master "
"for SYNC: %s", strerror(errno));
goto error;
}
}
if (server.repl_transfer_read >=
server.repl_transfer_last_fsync_off + REPL_MAX_WRITTEN_BEFORE_FSYNC)
{
off_t sync_size = server.repl_transfer_read -
server.repl_transfer_last_fsync_off;
rdb_fsync_range(server.repl_transfer_fd,
server.repl_transfer_last_fsync_off, sync_size);
server.repl_transfer_last_fsync_off += sync_size;
}
if (!usemark) {
if (server.repl_transfer_read == server.repl_transfer_size)
eof_reached = 1;
}
if (!eof_reached) return;
}
if (server.aof_state != AOF_OFF) stopAppendOnly();
if (use_diskless_load && server.repl_diskless_load == REPL_DISKLESS_LOAD_SWAPDB) {
diskless_load_tempDb = disklessLoadInitTempDb();
temp_functions_lib_ctx = functionsLibCtxCreate();
moduleFireServerEvent(REDISMODULE_EVENT_REPL_ASYNC_LOAD,
REDISMODULE_SUBEVENT_REPL_ASYNC_LOAD_STARTED,
NULL);
} else {
replicationAttachToNewMaster();
serverLog(LL_NOTICE, "MASTER <-> REPLICA sync: Flushing old data");
emptyData(-1,empty_db_flags,replicationEmptyDbCallback);
}
connSetReadHandler(conn, NULL);
serverLog(LL_NOTICE, "MASTER <-> REPLICA sync: Loading DB in memory");
rdbSaveInfo rsi = RDB_SAVE_INFO_INIT;
if (use_diskless_load) {
rio rdb;
redisDb *dbarray;
functionsLibCtx* functions_lib_ctx;
int asyncLoading = 0;
if (server.repl_diskless_load == REPL_DISKLESS_LOAD_SWAPDB) {
if (memcmp(server.replid, server.master_replid, CONFIG_RUN_ID_SIZE) == 0) {
asyncLoading = 1;
}
dbarray = diskless_load_tempDb;
functions_lib_ctx = temp_functions_lib_ctx;
} else {
dbarray = server.db;
functions_lib_ctx = functionsLibCtxGetCurrent();
functionsLibCtxClear(functions_lib_ctx);
}
rioInitWithConn(&rdb,conn,server.repl_transfer_size);
connBlock(conn);
connRecvTimeout(conn, server.repl_timeout*1000);
startLoading(server.repl_transfer_size, RDBFLAGS_REPLICATION, asyncLoading);
int loadingFailed = 0;
rdbLoadingCtx loadingCtx = { .dbarray = dbarray, .functions_lib_ctx = functions_lib_ctx };
if (rdbLoadRioWithLoadingCtx(&rdb,RDBFLAGS_REPLICATION,&rsi,&loadingCtx) != C_OK) {
serverLog(LL_WARNING,
"Failed trying to load the MASTER synchronization DB "
"from socket: %s", strerror(errno));
loadingFailed = 1;
} else if (usemark) {
if (!rioRead(&rdb, buf, CONFIG_RUN_ID_SIZE) ||
memcmp(buf, eofmark, CONFIG_RUN_ID_SIZE) != 0)
{
serverLog(LL_WARNING, "Replication stream EOF marker is broken");
loadingFailed = 1;
}
}
if (loadingFailed) {
stopLoading(0);
cancelReplicationHandshake(1);
rioFreeConn(&rdb, NULL);
if (server.repl_diskless_load == REPL_DISKLESS_LOAD_SWAPDB) {
moduleFireServerEvent(REDISMODULE_EVENT_REPL_ASYNC_LOAD,
REDISMODULE_SUBEVENT_REPL_ASYNC_LOAD_ABORTED,
NULL);
disklessLoadDiscardTempDb(diskless_load_tempDb);
functionsLibCtxFree(temp_functions_lib_ctx);
serverLog(LL_NOTICE, "MASTER <-> REPLICA sync: Discarding temporary DB in background");
} else {
emptyData(-1,empty_db_flags,replicationEmptyDbCallback);
}
return;
}
if (server.repl_diskless_load == REPL_DISKLESS_LOAD_SWAPDB) {
replicationAttachToNewMaster();
serverLog(LL_NOTICE, "MASTER <-> REPLICA sync: Swapping active DB with loaded DB");
swapMainDbWithTempDb(diskless_load_tempDb);
functionsLibCtxSwapWithCurrent(temp_functions_lib_ctx);
moduleFireServerEvent(REDISMODULE_EVENT_REPL_ASYNC_LOAD,
REDISMODULE_SUBEVENT_REPL_ASYNC_LOAD_COMPLETED,
NULL);
disklessLoadDiscardTempDb(diskless_load_tempDb);
serverLog(LL_NOTICE, "MASTER <-> REPLICA sync: Discarding old DB in background");
}
server.dirty++;
stopLoading(1);
rioFreeConn(&rdb, NULL);
connNonBlock(conn);
connRecvTimeout(conn,0);
} else {
if (server.child_type == CHILD_TYPE_RDB) {
serverLog(LL_NOTICE,
"Replica is about to load the RDB file received from the "
"master, but there is a pending RDB child running. "
"Killing process %ld and removing its temp file to avoid "
"any race",
(long) server.child_pid);
killRDBChild();
}
if (fsync(server.repl_transfer_fd) == -1) {
serverLog(LL_WARNING,
"Failed trying to sync the temp DB to disk in "
"MASTER <-> REPLICA synchronization: %s",
strerror(errno));
cancelReplicationHandshake(1);
return;
}
int old_rdb_fd = open(server.rdb_filename,O_RDONLY|O_NONBLOCK);
if (rename(server.repl_transfer_tmpfile,server.rdb_filename) == -1) {
serverLog(LL_WARNING,
"Failed trying to rename the temp DB into %s in "
"MASTER <-> REPLICA synchronization: %s",
server.rdb_filename, strerror(errno));
cancelReplicationHandshake(1);
if (old_rdb_fd != -1) close(old_rdb_fd);
return;
}
if (old_rdb_fd != -1) bioCreateCloseJob(old_rdb_fd);
if (rdbLoad(server.rdb_filename,&rsi,RDBFLAGS_REPLICATION) != C_OK) {
serverLog(LL_WARNING,
"Failed trying to load the MASTER synchronization "
"DB from disk: %s", strerror(errno));
cancelReplicationHandshake(1);
if (server.rdb_del_sync_files && allPersistenceDisabled()) {
serverLog(LL_NOTICE,"Removing the RDB file obtained from "
"the master. This replica has persistence "
"disabled");
bg_unlink(server.rdb_filename);
}
return;
}
if (server.rdb_del_sync_files && allPersistenceDisabled()) {
serverLog(LL_NOTICE,"Removing the RDB file obtained from "
"the master. This replica has persistence "
"disabled");
bg_unlink(server.rdb_filename);
}
zfree(server.repl_transfer_tmpfile);
close(server.repl_transfer_fd);
server.repl_transfer_fd = -1;
server.repl_transfer_tmpfile = NULL;
}
replicationCreateMasterClient(server.repl_transfer_s,rsi.repl_stream_db);
server.repl_state = REPL_STATE_CONNECTED;
server.repl_down_since = 0;
moduleFireServerEvent(REDISMODULE_EVENT_MASTER_LINK_CHANGE,
REDISMODULE_SUBEVENT_MASTER_LINK_UP,
NULL);
memcpy(server.replid,server.master->replid,sizeof(server.replid));
server.master_repl_offset = server.master->reploff;
clearReplicationId2();
if (server.repl_backlog == NULL) createReplicationBacklog();
serverLog(LL_NOTICE, "MASTER <-> REPLICA sync: Finished with success");
if (server.supervised_mode == SUPERVISED_SYSTEMD) {
redisCommunicateSystemd("STATUS=MASTER <-> REPLICA sync: Finished with success. Ready to accept connections in read-write mode.\n");
}
if (usemark) replicationSendAck();
if (server.aof_enabled) restartAOFAfterSYNC();
return;
error:
cancelReplicationHandshake(1);
return;
}
char *receiveSynchronousResponse(connection *conn) {
char buf[256];
if (connSyncReadLine(conn,buf,sizeof(buf),server.repl_syncio_timeout*1000) == -1)
{
return sdscatprintf(sdsempty(),"-Reading from master: %s",
strerror(errno));
}
server.repl_transfer_lastio = server.unixtime;
return sdsnew(buf);
}
char* sendCommandRaw(connection *conn, sds cmd) {
if (connSyncWrite(conn,cmd,sdslen(cmd),server.repl_syncio_timeout*1000) == -1) {
return sdscatprintf(sdsempty(),"-Writing to master: %s",
connGetLastError(conn));
}
return NULL;
}
char *sendCommand(connection *conn, ...) {
va_list ap;
sds cmd = sdsempty();
sds cmdargs = sdsempty();
size_t argslen = 0;
char *arg;
va_start(ap,conn);
while(1) {
arg = va_arg(ap, char*);
if (arg == NULL) break;
cmdargs = sdscatprintf(cmdargs,"$%zu\r\n%s\r\n",strlen(arg),arg);
argslen++;
}
cmd = sdscatprintf(cmd,"*%zu\r\n",argslen);
cmd = sdscatsds(cmd,cmdargs);
sdsfree(cmdargs);
va_end(ap);
char* err = sendCommandRaw(conn, cmd);
sdsfree(cmd);
if(err)
return err;
return NULL;
}
char *sendCommandArgv(connection *conn, int argc, char **argv, size_t *argv_lens) {
sds cmd = sdsempty();
char *arg;
int i;
cmd = sdscatfmt(cmd,"*%i\r\n",argc);
for (i=0; i<argc; i++) {
int len;
arg = argv[i];
len = argv_lens ? argv_lens[i] : strlen(arg);
cmd = sdscatfmt(cmd,"$%i\r\n",len);
cmd = sdscatlen(cmd,arg,len);
cmd = sdscatlen(cmd,"\r\n",2);
}
char* err = sendCommandRaw(conn, cmd);
sdsfree(cmd);
if (err)
return err;
return NULL;
}
#define PSYNC_WRITE_ERROR 0
#define PSYNC_WAIT_REPLY 1
#define PSYNC_CONTINUE 2
#define PSYNC_FULLRESYNC 3
#define PSYNC_NOT_SUPPORTED 4
#define PSYNC_TRY_LATER 5
int slaveTryPartialResynchronization(connection *conn, int read_reply) {
char *psync_replid;
char psync_offset[32];
sds reply;
if (!read_reply) {
server.master_initial_offset = -1;
if (server.cached_master) {
psync_replid = server.cached_master->replid;
snprintf(psync_offset,sizeof(psync_offset),"%lld", server.cached_master->reploff+1);
serverLog(LL_NOTICE,"Trying a partial resynchronization (request %s:%s).", psync_replid, psync_offset);
} else {
serverLog(LL_NOTICE,"Partial resynchronization not possible (no cached master)");
psync_replid = "?";
memcpy(psync_offset,"-1",3);
}
if (server.failover_state == FAILOVER_IN_PROGRESS) {
reply = sendCommand(conn,"PSYNC",psync_replid,psync_offset,"FAILOVER",NULL);
} else {
reply = sendCommand(conn,"PSYNC",psync_replid,psync_offset,NULL);
}
if (reply != NULL) {
serverLog(LL_WARNING,"Unable to send PSYNC to master: %s",reply);
sdsfree(reply);
connSetReadHandler(conn, NULL);
return PSYNC_WRITE_ERROR;
}
return PSYNC_WAIT_REPLY;
}
reply = receiveSynchronousResponse(conn);
if (sdslen(reply) == 0) {
sdsfree(reply);
return PSYNC_WAIT_REPLY;
}
connSetReadHandler(conn, NULL);
if (!strncmp(reply,"+FULLRESYNC",11)) {
char *replid = NULL, *offset = NULL;
replid = strchr(reply,' ');
if (replid) {
replid++;
offset = strchr(replid,' ');
if (offset) offset++;
}
if (!replid || !offset || (offset-replid-1) != CONFIG_RUN_ID_SIZE) {
serverLog(LL_WARNING,
"Master replied with wrong +FULLRESYNC syntax.");
memset(server.master_replid,0,CONFIG_RUN_ID_SIZE+1);
} else {
memcpy(server.master_replid, replid, offset-replid-1);
server.master_replid[CONFIG_RUN_ID_SIZE] = '\0';
server.master_initial_offset = strtoll(offset,NULL,10);
serverLog(LL_NOTICE,"Full resync from master: %s:%lld",
server.master_replid,
server.master_initial_offset);
}
sdsfree(reply);
return PSYNC_FULLRESYNC;
}
if (!strncmp(reply,"+CONTINUE",9)) {
serverLog(LL_NOTICE,
"Successful partial resynchronization with master.");
char *start = reply+10;
char *end = reply+9;
while(end[0] != '\r' && end[0] != '\n' && end[0] != '\0') end++;
if (end-start == CONFIG_RUN_ID_SIZE) {
char new[CONFIG_RUN_ID_SIZE+1];
memcpy(new,start,CONFIG_RUN_ID_SIZE);
new[CONFIG_RUN_ID_SIZE] = '\0';
if (strcmp(new,server.cached_master->replid)) {
serverLog(LL_WARNING,"Master replication ID changed to %s",new);
memcpy(server.replid2,server.cached_master->replid,
sizeof(server.replid2));
server.second_replid_offset = server.master_repl_offset+1;
memcpy(server.replid,new,sizeof(server.replid));
memcpy(server.cached_master->replid,new,sizeof(server.replid));
disconnectSlaves();
}
}
sdsfree(reply);
replicationResurrectCachedMaster(conn);
if (server.repl_backlog == NULL) createReplicationBacklog();
return PSYNC_CONTINUE;
}
if (!strncmp(reply,"-NOMASTERLINK",13) ||
!strncmp(reply,"-LOADING",8))
{
serverLog(LL_NOTICE,
"Master is currently unable to PSYNC "
"but should be in the future: %s", reply);
sdsfree(reply);
return PSYNC_TRY_LATER;
}
if (strncmp(reply,"-ERR",4)) {
serverLog(LL_WARNING,
"Unexpected reply to PSYNC from master: %s", reply);
} else {
serverLog(LL_NOTICE,
"Master does not support PSYNC or is in "
"error state (reply: %s)", reply);
}
sdsfree(reply);
return PSYNC_NOT_SUPPORTED;
}
void syncWithMaster(connection *conn) {
char tmpfile[256], *err = NULL;
int dfd = -1, maxtries = 5;
int psync_result;
if (server.repl_state == REPL_STATE_NONE) {
connClose(conn);
return;
}
if (connGetState(conn) != CONN_STATE_CONNECTED) {
serverLog(LL_WARNING,"Error condition on socket for SYNC: %s",
connGetLastError(conn));
goto error;
}
if (server.repl_state == REPL_STATE_CONNECTING) {
serverLog(LL_NOTICE,"Non blocking connect for SYNC fired the event.");
connSetReadHandler(conn, syncWithMaster);
connSetWriteHandler(conn, NULL);
server.repl_state = REPL_STATE_RECEIVE_PING_REPLY;
err = sendCommand(conn,"PING",NULL);
if (err) goto write_error;
return;
}
if (server.repl_state == REPL_STATE_RECEIVE_PING_REPLY) {
err = receiveSynchronousResponse(conn);
if (err[0] != '+' &&
strncmp(err,"-NOAUTH",7) != 0 &&
strncmp(err,"-NOPERM",7) != 0 &&
strncmp(err,"-ERR operation not permitted",28) != 0)
{
serverLog(LL_WARNING,"Error reply to PING from master: '%s'",err);
sdsfree(err);
goto error;
} else {
serverLog(LL_NOTICE,
"Master replied to PING, replication can continue...");
}
sdsfree(err);
err = NULL;
server.repl_state = REPL_STATE_SEND_HANDSHAKE;
}
if (server.repl_state == REPL_STATE_SEND_HANDSHAKE) {
if (server.masterauth) {
char *args[3] = {"AUTH",NULL,NULL};
size_t lens[3] = {4,0,0};
int argc = 1;
if (server.masteruser) {
args[argc] = server.masteruser;
lens[argc] = strlen(server.masteruser);
argc++;
}
args[argc] = server.masterauth;
lens[argc] = sdslen(server.masterauth);
argc++;
err = sendCommandArgv(conn, argc, args, lens);
if (err) goto write_error;
}
{
int port;
if (server.slave_announce_port)
port = server.slave_announce_port;
else if (server.tls_replication && server.tls_port)
port = server.tls_port;
else
port = server.port;
sds portstr = sdsfromlonglong(port);
err = sendCommand(conn,"REPLCONF",
"listening-port",portstr, NULL);
sdsfree(portstr);
if (err) goto write_error;
}
if (server.slave_announce_ip) {
err = sendCommand(conn,"REPLCONF",
"ip-address",server.slave_announce_ip, NULL);
if (err) goto write_error;
}
err = sendCommand(conn,"REPLCONF",
"capa","eof","capa","psync2",NULL);
if (err) goto write_error;
server.repl_state = REPL_STATE_RECEIVE_AUTH_REPLY;
return;
}
if (server.repl_state == REPL_STATE_RECEIVE_AUTH_REPLY && !server.masterauth)
server.repl_state = REPL_STATE_RECEIVE_PORT_REPLY;
if (server.repl_state == REPL_STATE_RECEIVE_AUTH_REPLY) {
err = receiveSynchronousResponse(conn);
if (err[0] == '-') {
serverLog(LL_WARNING,"Unable to AUTH to MASTER: %s",err);
sdsfree(err);
goto error;
}
sdsfree(err);
err = NULL;
server.repl_state = REPL_STATE_RECEIVE_PORT_REPLY;
return;
}
if (server.repl_state == REPL_STATE_RECEIVE_PORT_REPLY) {
err = receiveSynchronousResponse(conn);
if (err[0] == '-') {
serverLog(LL_NOTICE,"(Non critical) Master does not understand "
"REPLCONF listening-port: %s", err);
}
sdsfree(err);
server.repl_state = REPL_STATE_RECEIVE_IP_REPLY;
return;
}
if (server.repl_state == REPL_STATE_RECEIVE_IP_REPLY && !server.slave_announce_ip)
server.repl_state = REPL_STATE_RECEIVE_CAPA_REPLY;
if (server.repl_state == REPL_STATE_RECEIVE_IP_REPLY) {
err = receiveSynchronousResponse(conn);
if (err[0] == '-') {
serverLog(LL_NOTICE,"(Non critical) Master does not understand "
"REPLCONF ip-address: %s", err);
}
sdsfree(err);
server.repl_state = REPL_STATE_RECEIVE_CAPA_REPLY;
return;
}
if (server.repl_state == REPL_STATE_RECEIVE_CAPA_REPLY) {
err = receiveSynchronousResponse(conn);
if (err[0] == '-') {
serverLog(LL_NOTICE,"(Non critical) Master does not understand "
"REPLCONF capa: %s", err);
}
sdsfree(err);
err = NULL;
server.repl_state = REPL_STATE_SEND_PSYNC;
}
if (server.repl_state == REPL_STATE_SEND_PSYNC) {
if (slaveTryPartialResynchronization(conn,0) == PSYNC_WRITE_ERROR) {
err = sdsnew("Write error sending the PSYNC command.");
abortFailover("Write error to failover target");
goto write_error;
}
server.repl_state = REPL_STATE_RECEIVE_PSYNC_REPLY;
return;
}
if (server.repl_state != REPL_STATE_RECEIVE_PSYNC_REPLY) {
serverLog(LL_WARNING,"syncWithMaster(): state machine error, "
"state should be RECEIVE_PSYNC but is %d",
server.repl_state);
goto error;
}
psync_result = slaveTryPartialResynchronization(conn,1);
if (psync_result == PSYNC_WAIT_REPLY) return;
if (server.failover_state == FAILOVER_IN_PROGRESS) {
if (psync_result == PSYNC_CONTINUE || psync_result == PSYNC_FULLRESYNC) {
clearFailoverState();
} else {
abortFailover("Failover target rejected psync request");
return;
}
}
if (psync_result == PSYNC_TRY_LATER) goto error;
if (psync_result == PSYNC_CONTINUE) {
serverLog(LL_NOTICE, "MASTER <-> REPLICA sync: Master accepted a Partial Resynchronization.");
if (server.supervised_mode == SUPERVISED_SYSTEMD) {
redisCommunicateSystemd("STATUS=MASTER <-> REPLICA sync: Partial Resynchronization accepted. Ready to accept connections in read-write mode.\n");
}
return;
}
if (psync_result == PSYNC_NOT_SUPPORTED) {
serverLog(LL_NOTICE,"Retrying with SYNC...");
if (connSyncWrite(conn,"SYNC\r\n",6,server.repl_syncio_timeout*1000) == -1) {
serverLog(LL_WARNING,"I/O error writing to MASTER: %s",
strerror(errno));
goto error;
}
}
if (!useDisklessLoad()) {
while(maxtries--) {
snprintf(tmpfile,256,
"temp-%d.%ld.rdb",(int)server.unixtime,(long int)getpid());
dfd = open(tmpfile,O_CREAT|O_WRONLY|O_EXCL,0644);
if (dfd != -1) break;
sleep(1);
}
if (dfd == -1) {
serverLog(LL_WARNING,"Opening the temp file needed for MASTER <-> REPLICA synchronization: %s",strerror(errno));
goto error;
}
server.repl_transfer_tmpfile = zstrdup(tmpfile);
server.repl_transfer_fd = dfd;
}
if (connSetReadHandler(conn, readSyncBulkPayload)
== C_ERR)
{
char conninfo[CONN_INFO_LEN];
serverLog(LL_WARNING,
"Can't create readable event for SYNC: %s (%s)",
strerror(errno), connGetInfo(conn, conninfo, sizeof(conninfo)));
goto error;
}
server.repl_state = REPL_STATE_TRANSFER;
server.repl_transfer_size = -1;
server.repl_transfer_read = 0;
server.repl_transfer_last_fsync_off = 0;
server.repl_transfer_lastio = server.unixtime;
return;
error:
if (dfd != -1) close(dfd);
connClose(conn);
server.repl_transfer_s = NULL;
if (server.repl_transfer_fd != -1)
close(server.repl_transfer_fd);
if (server.repl_transfer_tmpfile)
zfree(server.repl_transfer_tmpfile);
server.repl_transfer_tmpfile = NULL;
server.repl_transfer_fd = -1;
server.repl_state = REPL_STATE_CONNECT;
return;
write_error:
serverLog(LL_WARNING,"Sending command to master in replication handshake: %s", err);
sdsfree(err);
goto error;
}
int connectWithMaster(void) {
server.repl_transfer_s = server.tls_replication ? connCreateTLS() : connCreateSocket();
if (connConnect(server.repl_transfer_s, server.masterhost, server.masterport,
server.bind_source_addr, syncWithMaster) == C_ERR) {
serverLog(LL_WARNING,"Unable to connect to MASTER: %s",
connGetLastError(server.repl_transfer_s));
connClose(server.repl_transfer_s);
server.repl_transfer_s = NULL;
return C_ERR;
}
server.repl_transfer_lastio = server.unixtime;
server.repl_state = REPL_STATE_CONNECTING;
serverLog(LL_NOTICE,"MASTER <-> REPLICA sync started");
return C_OK;
}
void undoConnectWithMaster(void) {
connClose(server.repl_transfer_s);
server.repl_transfer_s = NULL;
}
void replicationAbortSyncTransfer(void) {
serverAssert(server.repl_state == REPL_STATE_TRANSFER);
undoConnectWithMaster();
if (server.repl_transfer_fd!=-1) {
close(server.repl_transfer_fd);
bg_unlink(server.repl_transfer_tmpfile);
zfree(server.repl_transfer_tmpfile);
server.repl_transfer_tmpfile = NULL;
server.repl_transfer_fd = -1;
}
}
int cancelReplicationHandshake(int reconnect) {
if (server.repl_state == REPL_STATE_TRANSFER) {
replicationAbortSyncTransfer();
server.repl_state = REPL_STATE_CONNECT;
} else if (server.repl_state == REPL_STATE_CONNECTING ||
slaveIsInHandshakeState())
{
undoConnectWithMaster();
server.repl_state = REPL_STATE_CONNECT;
} else {
return 0;
}
if (!reconnect)
return 1;
serverLog(LL_NOTICE,"Reconnecting to MASTER %s:%d after failure",
server.masterhost, server.masterport);
connectWithMaster();
return 1;
}
void replicationSetMaster(char *ip, int port) {
int was_master = server.masterhost == NULL;
sdsfree(server.masterhost);
server.masterhost = NULL;
if (server.master) {
freeClient(server.master);
}
disconnectAllBlockedClients();
server.masterhost = sdsnew(ip);
server.masterport = port;
setOOMScoreAdj(-1);
cancelReplicationHandshake(0);
if (was_master) {
replicationDiscardCachedMaster();
replicationCacheMasterUsingMyself();
}
moduleFireServerEvent(REDISMODULE_EVENT_REPLICATION_ROLE_CHANGED,
REDISMODULE_EVENT_REPLROLECHANGED_NOW_REPLICA,
NULL);
if (server.repl_state == REPL_STATE_CONNECTED)
moduleFireServerEvent(REDISMODULE_EVENT_MASTER_LINK_CHANGE,
REDISMODULE_SUBEVENT_MASTER_LINK_DOWN,
NULL);
server.repl_state = REPL_STATE_CONNECT;
serverLog(LL_NOTICE,"Connecting to MASTER %s:%d",
server.masterhost, server.masterport);
connectWithMaster();
}
void replicationUnsetMaster(void) {
if (server.masterhost == NULL) return;
if (server.repl_state == REPL_STATE_CONNECTED)
moduleFireServerEvent(REDISMODULE_EVENT_MASTER_LINK_CHANGE,
REDISMODULE_SUBEVENT_MASTER_LINK_DOWN,
NULL);
sdsfree(server.masterhost);
server.masterhost = NULL;
if (server.master) freeClient(server.master);
replicationDiscardCachedMaster();
cancelReplicationHandshake(0);
shiftReplicationId();
disconnectSlaves();
server.repl_state = REPL_STATE_NONE;
server.slaveseldb = -1;
setOOMScoreAdj(-1);
server.repl_no_slaves_since = server.unixtime;
server.repl_down_since = 0;
moduleFireServerEvent(REDISMODULE_EVENT_REPLICATION_ROLE_CHANGED,
REDISMODULE_EVENT_REPLROLECHANGED_NOW_MASTER,
NULL);
if (server.aof_enabled && server.aof_state == AOF_OFF) restartAOFAfterSYNC();
}
void replicationHandleMasterDisconnection(void) {
if (server.repl_state == REPL_STATE_CONNECTED)
moduleFireServerEvent(REDISMODULE_EVENT_MASTER_LINK_CHANGE,
REDISMODULE_SUBEVENT_MASTER_LINK_DOWN,
NULL);
server.master = NULL;
server.repl_state = REPL_STATE_CONNECT;
server.repl_down_since = server.unixtime;
if (server.masterhost) {
serverLog(LL_NOTICE,"Reconnecting to MASTER %s:%d",
server.masterhost, server.masterport);
connectWithMaster();
}
}
void replicaofCommand(client *c) {
if (server.cluster_enabled) {
addReplyError(c,"REPLICAOF not allowed in cluster mode.");
return;
}
if (server.failover_state != NO_FAILOVER) {
addReplyError(c,"REPLICAOF not allowed while failing over.");
return;
}
if (!strcasecmp(c->argv[1]->ptr,"no") &&
!strcasecmp(c->argv[2]->ptr,"one")) {
if (server.masterhost) {
replicationUnsetMaster();
sds client = catClientInfoString(sdsempty(),c);
serverLog(LL_NOTICE,"MASTER MODE enabled (user request from '%s')",
client);
sdsfree(client);
}
} else {
long port;
if (c->flags & CLIENT_SLAVE)
{
addReplyError(c, "Command is not valid when client is a replica.");
return;
}
if (getRangeLongFromObjectOrReply(c, c->argv[2], 0, 65535, &port,
"Invalid master port") != C_OK)
return;
if (server.masterhost && !strcasecmp(server.masterhost,c->argv[1]->ptr)
&& server.masterport == port) {
serverLog(LL_NOTICE,"REPLICAOF would result into synchronization "
"with the master we are already connected "
"with. No operation performed.");
addReplySds(c,sdsnew("+OK Already connected to specified "
"master\r\n"));
return;
}
replicationSetMaster(c->argv[1]->ptr, port);
sds client = catClientInfoString(sdsempty(),c);
serverLog(LL_NOTICE,"REPLICAOF %s:%d enabled (user request from '%s')",
server.masterhost, server.masterport, client);
sdsfree(client);
}
addReply(c,shared.ok);
}
void roleCommand(client *c) {
if (server.sentinel_mode) {
sentinelRoleCommand(c);
return;
}
if (server.masterhost == NULL) {
listIter li;
listNode *ln;
void *mbcount;
int slaves = 0;
addReplyArrayLen(c,3);
addReplyBulkCBuffer(c,"master",6);
addReplyLongLong(c,server.master_repl_offset);
mbcount = addReplyDeferredLen(c);
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
char ip[NET_IP_STR_LEN], *slaveaddr = slave->slave_addr;
if (!slaveaddr) {
if (connPeerToString(slave->conn,ip,sizeof(ip),NULL) == -1)
continue;
slaveaddr = ip;
}
if (slave->replstate != SLAVE_STATE_ONLINE) continue;
addReplyArrayLen(c,3);
addReplyBulkCString(c,slaveaddr);
addReplyBulkLongLong(c,slave->slave_listening_port);
addReplyBulkLongLong(c,slave->repl_ack_off);
slaves++;
}
setDeferredArrayLen(c,mbcount,slaves);
} else {
char *slavestate = NULL;
addReplyArrayLen(c,5);
addReplyBulkCBuffer(c,"slave",5);
addReplyBulkCString(c,server.masterhost);
addReplyLongLong(c,server.masterport);
if (slaveIsInHandshakeState()) {
slavestate = "handshake";
} else {
switch(server.repl_state) {
case REPL_STATE_NONE: slavestate = "none"; break;
case REPL_STATE_CONNECT: slavestate = "connect"; break;
case REPL_STATE_CONNECTING: slavestate = "connecting"; break;
case REPL_STATE_TRANSFER: slavestate = "sync"; break;
case REPL_STATE_CONNECTED: slavestate = "connected"; break;
default: slavestate = "unknown"; break;
}
}
addReplyBulkCString(c,slavestate);
addReplyLongLong(c,server.master ? server.master->reploff : -1);
}
}
void replicationSendAck(void) {
client *c = server.master;
if (c != NULL) {
c->flags |= CLIENT_MASTER_FORCE_REPLY;
addReplyArrayLen(c,3);
addReplyBulkCString(c,"REPLCONF");
addReplyBulkCString(c,"ACK");
addReplyBulkLongLong(c,c->reploff);
c->flags &= ~CLIENT_MASTER_FORCE_REPLY;
}
}
void replicationCacheMaster(client *c) {
serverAssert(server.master != NULL && server.cached_master == NULL);
serverLog(LL_NOTICE,"Caching the disconnected master state.");
unlinkClient(c);
sdsclear(server.master->querybuf);
sdsclear(server.master->pending_querybuf);
server.master->read_reploff = server.master->reploff;
if (c->flags & CLIENT_MULTI) discardTransaction(c);
listEmpty(c->reply);
c->sentlen = 0;
c->reply_bytes = 0;
c->bufpos = 0;
resetClient(c);
server.cached_master = server.master;
if (c->peerid) {
sdsfree(c->peerid);
c->peerid = NULL;
}
if (c->sockname) {
sdsfree(c->sockname);
c->sockname = NULL;
}
replicationHandleMasterDisconnection();
}
void replicationCacheMasterUsingMyself(void) {
serverLog(LL_NOTICE,
"Before turning into a replica, using my own master parameters "
"to synthesize a cached master: I may be able to synchronize with "
"the new master with just a partial transfer.");
server.master_initial_offset = server.master_repl_offset;
replicationCreateMasterClient(NULL,-1);
memcpy(server.master->replid, server.replid, sizeof(server.replid));
unlinkClient(server.master);
server.cached_master = server.master;
server.master = NULL;
}
void replicationDiscardCachedMaster(void) {
if (server.cached_master == NULL) return;
serverLog(LL_NOTICE,"Discarding previously cached master state.");
server.cached_master->flags &= ~CLIENT_MASTER;
freeClient(server.cached_master);
server.cached_master = NULL;
}
void replicationResurrectCachedMaster(connection *conn) {
server.master = server.cached_master;
server.cached_master = NULL;
server.master->conn = conn;
connSetPrivateData(server.master->conn, server.master);
server.master->flags &= ~(CLIENT_CLOSE_AFTER_REPLY|CLIENT_CLOSE_ASAP);
server.master->authenticated = 1;
server.master->lastinteraction = server.unixtime;
server.repl_state = REPL_STATE_CONNECTED;
server.repl_down_since = 0;
moduleFireServerEvent(REDISMODULE_EVENT_MASTER_LINK_CHANGE,
REDISMODULE_SUBEVENT_MASTER_LINK_UP,
NULL);
linkClient(server.master);
if (connSetReadHandler(server.master->conn, readQueryFromClient)) {
serverLog(LL_WARNING,"Error resurrecting the cached master, impossible to add the readable handler: %s", strerror(errno));
freeClientAsync(server.master);
}
if (clientHasPendingReplies(server.master)) {
if (connSetWriteHandler(server.master->conn, sendReplyToClient)) {
serverLog(LL_WARNING,"Error resurrecting the cached master, impossible to add the writable handler: %s", strerror(errno));
freeClientAsync(server.master);
}
}
}
void refreshGoodSlavesCount(void) {
listIter li;
listNode *ln;
int good = 0;
if (!server.repl_min_slaves_to_write ||
!server.repl_min_slaves_max_lag) return;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
time_t lag = server.unixtime - slave->repl_ack_time;
if (slave->replstate == SLAVE_STATE_ONLINE &&
lag <= server.repl_min_slaves_max_lag) good++;
}
server.repl_good_slaves_count = good;
}
void replicationRequestAckFromSlaves(void) {
server.get_ack_from_slaves = 1;
}
int replicationCountAcksByOffset(long long offset) {
listIter li;
listNode *ln;
int count = 0;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
if (slave->replstate != SLAVE_STATE_ONLINE) continue;
if (slave->repl_ack_off >= offset) count++;
}
return count;
}
void waitCommand(client *c) {
mstime_t timeout;
long numreplicas, ackreplicas;
long long offset = c->woff;
if (server.masterhost) {
addReplyError(c,"WAIT cannot be used with replica instances. Please also note that since Redis 4.0 if a replica is configured to be writable (which is not the default) writes to replicas are just local and are not propagated.");
return;
}
if (getLongFromObjectOrReply(c,c->argv[1],&numreplicas,NULL) != C_OK)
return;
if (getTimeoutFromObjectOrReply(c,c->argv[2],&timeout,UNIT_MILLISECONDS)
!= C_OK) return;
ackreplicas = replicationCountAcksByOffset(c->woff);
if (ackreplicas >= numreplicas || c->flags & CLIENT_MULTI) {
addReplyLongLong(c,ackreplicas);
return;
}
c->bpop.timeout = timeout;
c->bpop.reploffset = offset;
c->bpop.numreplicas = numreplicas;
listAddNodeHead(server.clients_waiting_acks,c);
blockClient(c,BLOCKED_WAIT);
replicationRequestAckFromSlaves();
}
void unblockClientWaitingReplicas(client *c) {
listNode *ln = listSearchKey(server.clients_waiting_acks,c);
serverAssert(ln != NULL);
listDelNode(server.clients_waiting_acks,ln);
}
void processClientsWaitingReplicas(void) {
long long last_offset = 0;
int last_numreplicas = 0;
listIter li;
listNode *ln;
listRewind(server.clients_waiting_acks,&li);
while((ln = listNext(&li))) {
client *c = ln->value;
if (last_offset && last_offset >= c->bpop.reploffset &&
last_numreplicas >= c->bpop.numreplicas)
{
unblockClient(c);
addReplyLongLong(c,last_numreplicas);
} else {
int numreplicas = replicationCountAcksByOffset(c->bpop.reploffset);
if (numreplicas >= c->bpop.numreplicas) {
last_offset = c->bpop.reploffset;
last_numreplicas = numreplicas;
unblockClient(c);
addReplyLongLong(c,numreplicas);
}
}
}
}
long long replicationGetSlaveOffset(void) {
long long offset = 0;
if (server.masterhost != NULL) {
if (server.master) {
offset = server.master->reploff;
} else if (server.cached_master) {
offset = server.cached_master->reploff;
}
}
if (offset < 0) offset = 0;
return offset;
}
void replicationCron(void) {
static long long replication_cron_loops = 0;
updateFailoverStatus();
if (server.masterhost &&
(server.repl_state == REPL_STATE_CONNECTING ||
slaveIsInHandshakeState()) &&
(time(NULL)-server.repl_transfer_lastio) > server.repl_timeout)
{
serverLog(LL_WARNING,"Timeout connecting to the MASTER...");
cancelReplicationHandshake(1);
}
if (server.masterhost && server.repl_state == REPL_STATE_TRANSFER &&
(time(NULL)-server.repl_transfer_lastio) > server.repl_timeout)
{
serverLog(LL_WARNING,"Timeout receiving bulk data from MASTER... If the problem persists try to set the 'repl-timeout' parameter in redis.conf to a larger value.");
cancelReplicationHandshake(1);
}
if (server.masterhost && server.repl_state == REPL_STATE_CONNECTED &&
(time(NULL)-server.master->lastinteraction) > server.repl_timeout)
{
serverLog(LL_WARNING,"MASTER timeout: no data nor PING received...");
freeClient(server.master);
}
if (server.repl_state == REPL_STATE_CONNECT) {
serverLog(LL_NOTICE,"Connecting to MASTER %s:%d",
server.masterhost, server.masterport);
connectWithMaster();
}
if (server.masterhost && server.master &&
!(server.master->flags & CLIENT_PRE_PSYNC))
replicationSendAck();
listIter li;
listNode *ln;
robj *ping_argv[1];
if ((replication_cron_loops % server.repl_ping_slave_period) == 0 &&
listLength(server.slaves))
{
int manual_failover_in_progress =
((server.cluster_enabled &&
server.cluster->mf_end) ||
server.failover_end_time) &&
checkClientPauseTimeoutAndReturnIfPaused();
if (!manual_failover_in_progress) {
ping_argv[0] = shared.ping;
replicationFeedSlaves(server.slaves, server.slaveseldb,
ping_argv, 1);
}
}
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
int is_presync =
(slave->replstate == SLAVE_STATE_WAIT_BGSAVE_START ||
(slave->replstate == SLAVE_STATE_WAIT_BGSAVE_END &&
server.rdb_child_type != RDB_CHILD_TYPE_SOCKET));
if (is_presync) {
connWrite(slave->conn, "\n", 1);
}
}
if (listLength(server.slaves)) {
listIter li;
listNode *ln;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
if (slave->replstate == SLAVE_STATE_ONLINE) {
if (slave->flags & CLIENT_PRE_PSYNC)
continue;
if ((server.unixtime - slave->repl_ack_time) > server.repl_timeout) {
serverLog(LL_WARNING, "Disconnecting timedout replica (streaming sync): %s",
replicationGetSlaveName(slave));
freeClient(slave);
continue;
}
}
if (slave->replstate == SLAVE_STATE_WAIT_BGSAVE_END && server.rdb_child_type == RDB_CHILD_TYPE_SOCKET) {
if (slave->repl_last_partial_write != 0 &&
(server.unixtime - slave->repl_last_partial_write) > server.repl_timeout)
{
serverLog(LL_WARNING, "Disconnecting timedout replica (full sync): %s",
replicationGetSlaveName(slave));
freeClient(slave);
continue;
}
}
}
}
if (listLength(server.slaves) == 0 && server.repl_backlog_time_limit &&
server.repl_backlog && server.masterhost == NULL)
{
time_t idle = server.unixtime - server.repl_no_slaves_since;
if (idle > server.repl_backlog_time_limit) {
changeReplicationId();
clearReplicationId2();
freeReplicationBacklog();
serverLog(LL_NOTICE,
"Replication backlog freed after %d seconds "
"without connected replicas.",
(int) server.repl_backlog_time_limit);
}
}
replicationStartPendingFork();
removeRDBUsedToSyncReplicas();
if (listLength(server.repl_buffer_blocks) > 0) {
replBufBlock *o = listNodeValue(listFirst(server.repl_buffer_blocks));
serverAssert(o->refcount > 0 &&
o->refcount <= (int)listLength(server.slaves)+1);
}
refreshGoodSlavesCount();
replication_cron_loops++;
}
int shouldStartChildReplication(int *mincapa_out, int *req_out) {
if (!hasActiveChildProcess()) {
time_t idle, max_idle = 0;
int slaves_waiting = 0;
int mincapa;
int req;
int first = 1;
listNode *ln;
listIter li;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = ln->value;
if (slave->replstate == SLAVE_STATE_WAIT_BGSAVE_START) {
if (first) {
req = slave->slave_req;
} else if (req != slave->slave_req) {
continue;
}
idle = server.unixtime - slave->lastinteraction;
if (idle > max_idle) max_idle = idle;
slaves_waiting++;
mincapa = first ? slave->slave_capa : (mincapa & slave->slave_capa);
first = 0;
}
}
if (slaves_waiting &&
(!server.repl_diskless_sync ||
(server.repl_diskless_sync_max_replicas > 0 &&
slaves_waiting >= server.repl_diskless_sync_max_replicas) ||
max_idle >= server.repl_diskless_sync_delay))
{
if (mincapa_out)
*mincapa_out = mincapa;
if (req_out)
*req_out = req;
return 1;
}
}
return 0;
}
void replicationStartPendingFork(void) {
int mincapa = -1;
int req = -1;
if (shouldStartChildReplication(&mincapa, &req)) {
startBgsaveForReplication(mincapa, req);
}
}
static client *findReplica(char *host, int port) {
listIter li;
listNode *ln;
client *replica;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
replica = ln->value;
char ip[NET_IP_STR_LEN], *replicaip = replica->slave_addr;
if (!replicaip) {
if (connPeerToString(replica->conn, ip, sizeof(ip), NULL) == -1)
continue;
replicaip = ip;
}
if (!strcasecmp(host, replicaip) &&
(port == replica->slave_listening_port))
return replica;
}
return NULL;
}
const char *getFailoverStateString() {
switch(server.failover_state) {
case NO_FAILOVER: return "no-failover";
case FAILOVER_IN_PROGRESS: return "failover-in-progress";
case FAILOVER_WAIT_FOR_SYNC: return "waiting-for-sync";
default: return "unknown";
}
}
void clearFailoverState() {
server.failover_end_time = 0;
server.force_failover = 0;
zfree(server.target_replica_host);
server.target_replica_host = NULL;
server.target_replica_port = 0;
server.failover_state = NO_FAILOVER;
unpauseClients(PAUSE_DURING_FAILOVER);
}
void abortFailover(const char *err) {
if (server.failover_state == NO_FAILOVER) return;
if (server.target_replica_host) {
serverLog(LL_NOTICE,"FAILOVER to %s:%d aborted: %s",
server.target_replica_host,server.target_replica_port,err);
} else {
serverLog(LL_NOTICE,"FAILOVER to any replica aborted: %s",err);
}
if (server.failover_state == FAILOVER_IN_PROGRESS) {
replicationUnsetMaster();
}
clearFailoverState();
}
void failoverCommand(client *c) {
if (server.cluster_enabled) {
addReplyError(c,"FAILOVER not allowed in cluster mode. "
"Use CLUSTER FAILOVER command instead.");
return;
}
if ((c->argc == 2) && !strcasecmp(c->argv[1]->ptr,"abort")) {
if (server.failover_state == NO_FAILOVER) {
addReplyError(c, "No failover in progress.");
return;
}
abortFailover("Failover manually aborted");
addReply(c,shared.ok);
return;
}
long timeout_in_ms = 0;
int force_flag = 0;
long port = 0;
char *host = NULL;
for (int j = 1; j < c->argc; j++) {
if (!strcasecmp(c->argv[j]->ptr,"timeout") && (j + 1 < c->argc) &&
timeout_in_ms == 0)
{
if (getLongFromObjectOrReply(c,c->argv[j + 1],
&timeout_in_ms,NULL) != C_OK) return;
if (timeout_in_ms <= 0) {
addReplyError(c,"FAILOVER timeout must be greater than 0");
return;
}
j++;
} else if (!strcasecmp(c->argv[j]->ptr,"to") && (j + 2 < c->argc) &&
!host)
{
if (getLongFromObjectOrReply(c,c->argv[j + 2],&port,NULL) != C_OK)
return;
host = c->argv[j + 1]->ptr;
j += 2;
} else if (!strcasecmp(c->argv[j]->ptr,"force") && !force_flag) {
force_flag = 1;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
}
if (server.failover_state != NO_FAILOVER) {
addReplyError(c,"FAILOVER already in progress.");
return;
}
if (server.masterhost) {
addReplyError(c,"FAILOVER is not valid when server is a replica.");
return;
}
if (listLength(server.slaves) == 0) {
addReplyError(c,"FAILOVER requires connected replicas.");
return;
}
if (force_flag && (!timeout_in_ms || !host)) {
addReplyError(c,"FAILOVER with force option requires both a timeout "
"and target HOST and IP.");
return;
}
if (host) {
client *replica = findReplica(host, port);
if (replica == NULL) {
addReplyError(c,"FAILOVER target HOST and PORT is not "
"a replica.");
return;
}
if (replica->replstate != SLAVE_STATE_ONLINE) {
addReplyError(c,"FAILOVER target replica is not online.");
return;
}
server.target_replica_host = zstrdup(host);
server.target_replica_port = port;
serverLog(LL_NOTICE,"FAILOVER requested to %s:%ld.",host,port);
} else {
serverLog(LL_NOTICE,"FAILOVER requested to any replica.");
}
mstime_t now = mstime();
if (timeout_in_ms) {
server.failover_end_time = now + timeout_in_ms;
}
server.force_failover = force_flag;
server.failover_state = FAILOVER_WAIT_FOR_SYNC;
pauseClients(PAUSE_DURING_FAILOVER, LLONG_MAX, CLIENT_PAUSE_WRITE);
addReply(c,shared.ok);
}
void updateFailoverStatus(void) {
if (server.failover_state != FAILOVER_WAIT_FOR_SYNC) return;
mstime_t now = server.mstime;
if (server.failover_end_time && server.failover_end_time <= now) {
if (server.force_failover) {
serverLog(LL_NOTICE,
"FAILOVER to %s:%d time out exceeded, failing over.",
server.target_replica_host, server.target_replica_port);
server.failover_state = FAILOVER_IN_PROGRESS;
replicationSetMaster(server.target_replica_host,
server.target_replica_port);
return;
} else {
abortFailover("Replica never caught up before timeout");
return;
}
}
client *replica = NULL;
if (server.target_replica_host) {
replica = findReplica(server.target_replica_host,
server.target_replica_port);
} else {
listIter li;
listNode *ln;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
replica = ln->value;
if (replica->repl_ack_off == server.master_repl_offset) {
char ip[NET_IP_STR_LEN], *replicaaddr = replica->slave_addr;
if (!replicaaddr) {
if (connPeerToString(replica->conn,ip,sizeof(ip),NULL) == -1)
continue;
replicaaddr = ip;
}
server.target_replica_host = zstrdup(replicaaddr);
server.target_replica_port = replica->slave_listening_port;
break;
}
}
}
if (replica && (replica->repl_ack_off == server.master_repl_offset)) {
server.failover_state = FAILOVER_IN_PROGRESS;
serverLog(LL_NOTICE,
"Failover target %s:%d is synced, failing over.",
server.target_replica_host, server.target_replica_port);
replicationSetMaster(server.target_replica_host,
server.target_replica_port);
}
}
