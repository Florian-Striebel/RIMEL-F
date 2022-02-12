#include "server.h"
void initClientMultiState(client *c) {
c->mstate.commands = NULL;
c->mstate.count = 0;
c->mstate.cmd_flags = 0;
c->mstate.cmd_inv_flags = 0;
c->mstate.argv_len_sums = 0;
}
void freeClientMultiState(client *c) {
int j;
for (j = 0; j < c->mstate.count; j++) {
int i;
multiCmd *mc = c->mstate.commands+j;
for (i = 0; i < mc->argc; i++)
decrRefCount(mc->argv[i]);
zfree(mc->argv);
}
zfree(c->mstate.commands);
}
void queueMultiCommand(client *c) {
multiCmd *mc;
if (c->flags & (CLIENT_DIRTY_CAS|CLIENT_DIRTY_EXEC))
return;
c->mstate.commands = zrealloc(c->mstate.commands,
sizeof(multiCmd)*(c->mstate.count+1));
mc = c->mstate.commands+c->mstate.count;
mc->cmd = c->cmd;
mc->argc = c->argc;
mc->argv = c->argv;
mc->argv_len = c->argv_len;
c->mstate.count++;
c->mstate.cmd_flags |= c->cmd->flags;
c->mstate.cmd_inv_flags |= ~c->cmd->flags;
c->mstate.argv_len_sums += c->argv_len_sum + sizeof(robj*)*c->argc;
c->argv = NULL;
c->argc = 0;
c->argv_len_sum = 0;
c->argv_len = 0;
}
void discardTransaction(client *c) {
freeClientMultiState(c);
initClientMultiState(c);
c->flags &= ~(CLIENT_MULTI|CLIENT_DIRTY_CAS|CLIENT_DIRTY_EXEC);
unwatchAllKeys(c);
}
void flagTransaction(client *c) {
if (c->flags & CLIENT_MULTI)
c->flags |= CLIENT_DIRTY_EXEC;
}
void multiCommand(client *c) {
if (c->flags & CLIENT_MULTI) {
addReplyError(c,"MULTI calls can not be nested");
return;
}
c->flags |= CLIENT_MULTI;
addReply(c,shared.ok);
}
void discardCommand(client *c) {
if (!(c->flags & CLIENT_MULTI)) {
addReplyError(c,"DISCARD without MULTI");
return;
}
discardTransaction(c);
addReply(c,shared.ok);
}
void execCommandAbort(client *c, sds error) {
discardTransaction(c);
if (error[0] == '-') error++;
addReplyErrorFormat(c, "-EXECABORT Transaction discarded because of: %s", error);
replicationFeedMonitors(c,server.monitors,c->db->id,c->argv,c->argc);
}
void execCommand(client *c) {
int j;
robj **orig_argv;
int orig_argc, orig_argv_len;
struct redisCommand *orig_cmd;
if (!(c->flags & CLIENT_MULTI)) {
addReplyError(c,"EXEC without MULTI");
return;
}
if (isWatchedKeyExpired(c)) {
c->flags |= (CLIENT_DIRTY_CAS);
}
if (c->flags & (CLIENT_DIRTY_CAS | CLIENT_DIRTY_EXEC)) {
if (c->flags & CLIENT_DIRTY_EXEC) {
addReplyErrorObject(c, shared.execaborterr);
} else {
addReply(c, shared.nullarray[c->resp]);
}
discardTransaction(c);
return;
}
uint64_t old_flags = c->flags;
c->flags |= CLIENT_DENY_BLOCKING;
unwatchAllKeys(c);
server.in_exec = 1;
orig_argv = c->argv;
orig_argv_len = c->argv_len;
orig_argc = c->argc;
orig_cmd = c->cmd;
addReplyArrayLen(c,c->mstate.count);
for (j = 0; j < c->mstate.count; j++) {
c->argc = c->mstate.commands[j].argc;
c->argv = c->mstate.commands[j].argv;
c->argv_len = c->mstate.commands[j].argv_len;
c->cmd = c->mstate.commands[j].cmd;
int acl_errpos;
int acl_retval = ACLCheckAllPerm(c,&acl_errpos);
if (acl_retval != ACL_OK) {
char *reason;
switch (acl_retval) {
case ACL_DENIED_CMD:
reason = "no permission to execute the command or subcommand";
break;
case ACL_DENIED_KEY:
reason = "no permission to touch the specified keys";
break;
case ACL_DENIED_CHANNEL:
reason = "no permission to access one of the channels used "
"as arguments";
break;
default:
reason = "no permission";
break;
}
addACLLogEntry(c,acl_retval,ACL_LOG_CTX_MULTI,acl_errpos,NULL,NULL);
addReplyErrorFormat(c,
"-NOPERM ACLs rules changed between the moment the "
"transaction was accumulated and the EXEC call. "
"This command is no longer allowed for the "
"following reason: %s", reason);
} else {
if (c->id == CLIENT_ID_AOF)
call(c,CMD_CALL_NONE);
else
call(c,CMD_CALL_FULL);
serverAssert((c->flags & CLIENT_BLOCKED) == 0);
}
c->mstate.commands[j].argc = c->argc;
c->mstate.commands[j].argv = c->argv;
c->mstate.commands[j].cmd = c->cmd;
}
if (!(old_flags & CLIENT_DENY_BLOCKING))
c->flags &= ~CLIENT_DENY_BLOCKING;
c->argv = orig_argv;
c->argv_len = orig_argv_len;
c->argc = orig_argc;
c->cmd = orig_cmd;
discardTransaction(c);
server.in_exec = 0;
}
typedef struct watchedKey {
robj *key;
redisDb *db;
} watchedKey;
void watchForKey(client *c, robj *key) {
list *clients = NULL;
listIter li;
listNode *ln;
watchedKey *wk;
listRewind(c->watched_keys,&li);
while((ln = listNext(&li))) {
wk = listNodeValue(ln);
if (wk->db == c->db && equalStringObjects(key,wk->key))
return;
}
clients = dictFetchValue(c->db->watched_keys,key);
if (!clients) {
clients = listCreate();
dictAdd(c->db->watched_keys,key,clients);
incrRefCount(key);
}
listAddNodeTail(clients,c);
wk = zmalloc(sizeof(*wk));
wk->key = key;
wk->db = c->db;
incrRefCount(key);
listAddNodeTail(c->watched_keys,wk);
}
void unwatchAllKeys(client *c) {
listIter li;
listNode *ln;
if (listLength(c->watched_keys) == 0) return;
listRewind(c->watched_keys,&li);
while((ln = listNext(&li))) {
list *clients;
watchedKey *wk;
wk = listNodeValue(ln);
clients = dictFetchValue(wk->db->watched_keys, wk->key);
serverAssertWithInfo(c,NULL,clients != NULL);
listDelNode(clients,listSearchKey(clients,c));
if (listLength(clients) == 0)
dictDelete(wk->db->watched_keys, wk->key);
listDelNode(c->watched_keys,ln);
decrRefCount(wk->key);
zfree(wk);
}
}
int isWatchedKeyExpired(client *c) {
listIter li;
listNode *ln;
watchedKey *wk;
if (listLength(c->watched_keys) == 0) return 0;
listRewind(c->watched_keys,&li);
while ((ln = listNext(&li))) {
wk = listNodeValue(ln);
if (keyIsExpired(wk->db, wk->key)) return 1;
}
return 0;
}
void touchWatchedKey(redisDb *db, robj *key) {
list *clients;
listIter li;
listNode *ln;
if (dictSize(db->watched_keys) == 0) return;
clients = dictFetchValue(db->watched_keys, key);
if (!clients) return;
listRewind(clients,&li);
while((ln = listNext(&li))) {
client *c = listNodeValue(ln);
c->flags |= CLIENT_DIRTY_CAS;
unwatchAllKeys(c);
}
}
void touchAllWatchedKeysInDb(redisDb *emptied, redisDb *replaced_with) {
listIter li;
listNode *ln;
dictEntry *de;
if (dictSize(emptied->watched_keys) == 0) return;
dictIterator *di = dictGetSafeIterator(emptied->watched_keys);
while((de = dictNext(di)) != NULL) {
robj *key = dictGetKey(de);
if (dictFind(emptied->dict, key->ptr) ||
(replaced_with && dictFind(replaced_with->dict, key->ptr)))
{
list *clients = dictGetVal(de);
if (!clients) continue;
listRewind(clients,&li);
while((ln = listNext(&li))) {
client *c = listNodeValue(ln);
c->flags |= CLIENT_DIRTY_CAS;
unwatchAllKeys(c);
}
}
}
dictReleaseIterator(di);
}
void watchCommand(client *c) {
int j;
if (c->flags & CLIENT_MULTI) {
addReplyError(c,"WATCH inside MULTI is not allowed");
return;
}
if (c->flags & CLIENT_DIRTY_CAS) {
addReply(c,shared.ok);
return;
}
for (j = 1; j < c->argc; j++)
watchForKey(c,c->argv[j]);
addReply(c,shared.ok);
}
void unwatchCommand(client *c) {
unwatchAllKeys(c);
c->flags &= (~CLIENT_DIRTY_CAS);
addReply(c,shared.ok);
}
size_t multiStateMemOverhead(client *c) {
size_t mem = c->mstate.argv_len_sums;
mem += listLength(c->watched_keys) * (sizeof(listNode) + sizeof(watchedKey));
return mem;
}
