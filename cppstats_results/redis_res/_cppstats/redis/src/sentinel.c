#include "server.h"
#include "hiredis.h"
#if defined(USE_OPENSSL)
#include "openssl/ssl.h"
#include "hiredis_ssl.h"
#endif
#include "async.h"
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
extern char **environ;
#if defined(USE_OPENSSL)
extern SSL_CTX *redis_tls_ctx;
extern SSL_CTX *redis_tls_client_ctx;
#endif
#define REDIS_SENTINEL_PORT 26379
typedef struct sentinelAddr {
char *hostname;
char *ip;
int port;
} sentinelAddr;
#define SRI_MASTER (1<<0)
#define SRI_SLAVE (1<<1)
#define SRI_SENTINEL (1<<2)
#define SRI_S_DOWN (1<<3)
#define SRI_O_DOWN (1<<4)
#define SRI_MASTER_DOWN (1<<5)
#define SRI_FAILOVER_IN_PROGRESS (1<<6)
#define SRI_PROMOTED (1<<7)
#define SRI_RECONF_SENT (1<<8)
#define SRI_RECONF_INPROG (1<<9)
#define SRI_RECONF_DONE (1<<10)
#define SRI_FORCE_FAILOVER (1<<11)
#define SRI_SCRIPT_KILL_SENT (1<<12)
#define SRI_MASTER_REBOOT (1<<13)
#define SENTINEL_PING_PERIOD 1000
static mstime_t sentinel_info_period = 10000;
static mstime_t sentinel_ping_period = SENTINEL_PING_PERIOD;
static mstime_t sentinel_ask_period = 1000;
static mstime_t sentinel_publish_period = 2000;
static mstime_t sentinel_default_down_after = 30000;
static mstime_t sentinel_tilt_trigger = 2000;
static mstime_t sentinel_tilt_period = SENTINEL_PING_PERIOD * 30;
static mstime_t sentinel_slave_reconf_timeout = 10000;
static mstime_t sentinel_min_link_reconnect_period = 15000;
static mstime_t sentinel_election_timeout = 10000;
static mstime_t sentinel_script_max_runtime = 60000;
static mstime_t sentinel_script_retry_delay = 30000;
static mstime_t sentinel_default_failover_timeout = 60*3*1000;
#define SENTINEL_HELLO_CHANNEL "__sentinel__:hello"
#define SENTINEL_DEFAULT_SLAVE_PRIORITY 100
#define SENTINEL_DEFAULT_PARALLEL_SYNCS 1
#define SENTINEL_MAX_PENDING_COMMANDS 100
#define SENTINEL_MAX_DESYNC 1000
#define SENTINEL_DEFAULT_DENY_SCRIPTS_RECONFIG 1
#define SENTINEL_DEFAULT_RESOLVE_HOSTNAMES 0
#define SENTINEL_DEFAULT_ANNOUNCE_HOSTNAMES 0
#define SENTINEL_FAILOVER_STATE_NONE 0
#define SENTINEL_FAILOVER_STATE_WAIT_START 1
#define SENTINEL_FAILOVER_STATE_SELECT_SLAVE 2
#define SENTINEL_FAILOVER_STATE_SEND_SLAVEOF_NOONE 3
#define SENTINEL_FAILOVER_STATE_WAIT_PROMOTION 4
#define SENTINEL_FAILOVER_STATE_RECONF_SLAVES 5
#define SENTINEL_FAILOVER_STATE_UPDATE_CONFIG 6
#define SENTINEL_MASTER_LINK_STATUS_UP 0
#define SENTINEL_MASTER_LINK_STATUS_DOWN 1
#define SENTINEL_NO_FLAGS 0
#define SENTINEL_GENERATE_EVENT (1<<16)
#define SENTINEL_LEADER (1<<17)
#define SENTINEL_OBSERVER (1<<18)
#define SENTINEL_SCRIPT_NONE 0
#define SENTINEL_SCRIPT_RUNNING 1
#define SENTINEL_SCRIPT_MAX_QUEUE 256
#define SENTINEL_SCRIPT_MAX_RUNNING 16
#define SENTINEL_SCRIPT_MAX_RETRY 10
#define SENTINEL_SIMFAILURE_NONE 0
#define SENTINEL_SIMFAILURE_CRASH_AFTER_ELECTION (1<<0)
#define SENTINEL_SIMFAILURE_CRASH_AFTER_PROMOTION (1<<1)
typedef struct instanceLink {
int refcount;
int disconnected;
int pending_commands;
redisAsyncContext *cc;
redisAsyncContext *pc;
mstime_t cc_conn_time;
mstime_t pc_conn_time;
mstime_t pc_last_activity;
mstime_t last_avail_time;
mstime_t act_ping_time;
mstime_t last_ping_time;
mstime_t last_pong_time;
mstime_t last_reconn_time;
} instanceLink;
typedef struct sentinelRedisInstance {
int flags;
char *name;
char *runid;
uint64_t config_epoch;
sentinelAddr *addr;
instanceLink *link;
mstime_t last_pub_time;
mstime_t last_hello_time;
mstime_t last_master_down_reply_time;
mstime_t s_down_since_time;
mstime_t o_down_since_time;
mstime_t down_after_period;
mstime_t master_reboot_down_after_period;
mstime_t master_reboot_since_time;
mstime_t info_refresh;
dict *renamed_commands;
int role_reported;
mstime_t role_reported_time;
mstime_t slave_conf_change_time;
dict *sentinels;
dict *slaves;
unsigned int quorum;
int parallel_syncs;
char *auth_pass;
char *auth_user;
mstime_t master_link_down_time;
int slave_priority;
int replica_announced;
mstime_t slave_reconf_sent_time;
struct sentinelRedisInstance *master;
char *slave_master_host;
int slave_master_port;
int slave_master_link_status;
unsigned long long slave_repl_offset;
char *leader;
uint64_t leader_epoch;
uint64_t failover_epoch;
int failover_state;
mstime_t failover_state_change_time;
mstime_t failover_start_time;
mstime_t failover_timeout;
mstime_t failover_delay_logged;
struct sentinelRedisInstance *promoted_slave;
char *notification_script;
char *client_reconfig_script;
sds info;
} sentinelRedisInstance;
struct sentinelState {
char myid[CONFIG_RUN_ID_SIZE+1];
uint64_t current_epoch;
dict *masters;
int tilt;
int running_scripts;
mstime_t tilt_start_time;
mstime_t previous_time;
list *scripts_queue;
char *announce_ip;
int announce_port;
unsigned long simfailure_flags;
int deny_scripts_reconfig;
char *sentinel_auth_pass;
char *sentinel_auth_user;
int resolve_hostnames;
int announce_hostnames;
} sentinel;
typedef struct sentinelScriptJob {
int flags;
int retry_num;
char **argv;
mstime_t start_time;
pid_t pid;
} sentinelScriptJob;
typedef struct redisAeEvents {
redisAsyncContext *context;
aeEventLoop *loop;
int fd;
int reading, writing;
} redisAeEvents;
static void redisAeReadEvent(aeEventLoop *el, int fd, void *privdata, int mask) {
((void)el); ((void)fd); ((void)mask);
redisAeEvents *e = (redisAeEvents*)privdata;
redisAsyncHandleRead(e->context);
}
static void redisAeWriteEvent(aeEventLoop *el, int fd, void *privdata, int mask) {
((void)el); ((void)fd); ((void)mask);
redisAeEvents *e = (redisAeEvents*)privdata;
redisAsyncHandleWrite(e->context);
}
static void redisAeAddRead(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
aeEventLoop *loop = e->loop;
if (!e->reading) {
e->reading = 1;
aeCreateFileEvent(loop,e->fd,AE_READABLE,redisAeReadEvent,e);
}
}
static void redisAeDelRead(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
aeEventLoop *loop = e->loop;
if (e->reading) {
e->reading = 0;
aeDeleteFileEvent(loop,e->fd,AE_READABLE);
}
}
static void redisAeAddWrite(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
aeEventLoop *loop = e->loop;
if (!e->writing) {
e->writing = 1;
aeCreateFileEvent(loop,e->fd,AE_WRITABLE,redisAeWriteEvent,e);
}
}
static void redisAeDelWrite(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
aeEventLoop *loop = e->loop;
if (e->writing) {
e->writing = 0;
aeDeleteFileEvent(loop,e->fd,AE_WRITABLE);
}
}
static void redisAeCleanup(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
redisAeDelRead(privdata);
redisAeDelWrite(privdata);
zfree(e);
}
static int redisAeAttach(aeEventLoop *loop, redisAsyncContext *ac) {
redisContext *c = &(ac->c);
redisAeEvents *e;
if (ac->ev.data != NULL)
return C_ERR;
e = (redisAeEvents*)zmalloc(sizeof(*e));
e->context = ac;
e->loop = loop;
e->fd = c->fd;
e->reading = e->writing = 0;
ac->ev.addRead = redisAeAddRead;
ac->ev.delRead = redisAeDelRead;
ac->ev.addWrite = redisAeAddWrite;
ac->ev.delWrite = redisAeDelWrite;
ac->ev.cleanup = redisAeCleanup;
ac->ev.data = e;
return C_OK;
}
void sentinelLinkEstablishedCallback(const redisAsyncContext *c, int status);
void sentinelDisconnectCallback(const redisAsyncContext *c, int status);
void sentinelReceiveHelloMessages(redisAsyncContext *c, void *reply, void *privdata);
sentinelRedisInstance *sentinelGetMasterByName(char *name);
char *sentinelGetSubjectiveLeader(sentinelRedisInstance *master);
char *sentinelGetObjectiveLeader(sentinelRedisInstance *master);
int yesnotoi(char *s);
void instanceLinkConnectionError(const redisAsyncContext *c);
const char *sentinelRedisInstanceTypeStr(sentinelRedisInstance *ri);
void sentinelAbortFailover(sentinelRedisInstance *ri);
void sentinelEvent(int level, char *type, sentinelRedisInstance *ri, const char *fmt, ...);
sentinelRedisInstance *sentinelSelectSlave(sentinelRedisInstance *master);
void sentinelScheduleScriptExecution(char *path, ...);
void sentinelStartFailover(sentinelRedisInstance *master);
void sentinelDiscardReplyCallback(redisAsyncContext *c, void *reply, void *privdata);
int sentinelSendSlaveOf(sentinelRedisInstance *ri, const sentinelAddr *addr);
char *sentinelVoteLeader(sentinelRedisInstance *master, uint64_t req_epoch, char *req_runid, uint64_t *leader_epoch);
int sentinelFlushConfig(void);
void sentinelGenerateInitialMonitorEvents(void);
int sentinelSendPing(sentinelRedisInstance *ri);
int sentinelForceHelloUpdateForMaster(sentinelRedisInstance *master);
sentinelRedisInstance *getSentinelRedisInstanceByAddrAndRunID(dict *instances, char *ip, int port, char *runid);
void sentinelSimFailureCrash(void);
void releaseSentinelRedisInstance(sentinelRedisInstance *ri);
void dictInstancesValDestructor (dict *d, void *obj) {
UNUSED(d);
releaseSentinelRedisInstance(obj);
}
dictType instancesDictType = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
NULL,
dictInstancesValDestructor,
NULL
};
dictType leaderVotesDictType = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
NULL,
NULL,
NULL
};
dictType renamedCommandsDictType = {
dictSdsCaseHash,
NULL,
NULL,
dictSdsKeyCaseCompare,
dictSdsDestructor,
dictSdsDestructor,
NULL
};
void sentinelSetCommand(client *c);
void sentinelConfigGetCommand(client *c);
void sentinelConfigSetCommand(client *c);
const char *preMonitorCfgName[] = {
"announce-ip",
"announce-port",
"deny-scripts-reconfig",
"sentinel-user",
"sentinel-pass",
"current-epoch",
"myid",
"resolve-hostnames",
"announce-hostnames"
};
void initSentinelConfig(void) {
server.port = REDIS_SENTINEL_PORT;
server.protected_mode = 0;
}
void freeSentinelLoadQueueEntry(void *item);
void initSentinel(void) {
sentinel.current_epoch = 0;
sentinel.masters = dictCreate(&instancesDictType);
sentinel.tilt = 0;
sentinel.tilt_start_time = 0;
sentinel.previous_time = mstime();
sentinel.running_scripts = 0;
sentinel.scripts_queue = listCreate();
sentinel.announce_ip = NULL;
sentinel.announce_port = 0;
sentinel.simfailure_flags = SENTINEL_SIMFAILURE_NONE;
sentinel.deny_scripts_reconfig = SENTINEL_DEFAULT_DENY_SCRIPTS_RECONFIG;
sentinel.sentinel_auth_pass = NULL;
sentinel.sentinel_auth_user = NULL;
sentinel.resolve_hostnames = SENTINEL_DEFAULT_RESOLVE_HOSTNAMES;
sentinel.announce_hostnames = SENTINEL_DEFAULT_ANNOUNCE_HOSTNAMES;
memset(sentinel.myid,0,sizeof(sentinel.myid));
server.sentinel_config = NULL;
}
void sentinelCheckConfigFile(void) {
if (server.configfile == NULL) {
serverLog(LL_WARNING,
"Sentinel needs config file on disk to save state. Exiting...");
exit(1);
} else if (access(server.configfile,W_OK) == -1) {
serverLog(LL_WARNING,
"Sentinel config file %s is not writable: %s. Exiting...",
server.configfile,strerror(errno));
exit(1);
}
}
void sentinelIsRunning(void) {
int j;
for (j = 0; j < CONFIG_RUN_ID_SIZE; j++)
if (sentinel.myid[j] != 0) break;
if (j == CONFIG_RUN_ID_SIZE) {
getRandomHexChars(sentinel.myid,CONFIG_RUN_ID_SIZE);
sentinelFlushConfig();
}
serverLog(LL_WARNING,"Sentinel ID is %s", sentinel.myid);
sentinelGenerateInitialMonitorEvents();
}
sentinelAddr *createSentinelAddr(char *hostname, int port) {
char ip[NET_IP_STR_LEN];
sentinelAddr *sa;
if (port < 0 || port > 65535) {
errno = EINVAL;
return NULL;
}
if (anetResolve(NULL,hostname,ip,sizeof(ip),
sentinel.resolve_hostnames ? ANET_NONE : ANET_IP_ONLY) == ANET_ERR) {
errno = ENOENT;
return NULL;
}
sa = zmalloc(sizeof(*sa));
sa->hostname = sdsnew(hostname);
sa->ip = sdsnew(ip);
sa->port = port;
return sa;
}
sentinelAddr *dupSentinelAddr(sentinelAddr *src) {
sentinelAddr *sa;
sa = zmalloc(sizeof(*sa));
sa->hostname = sdsnew(src->hostname);
sa->ip = sdsnew(src->ip);
sa->port = src->port;
return sa;
}
void releaseSentinelAddr(sentinelAddr *sa) {
sdsfree(sa->hostname);
sdsfree(sa->ip);
zfree(sa);
}
int sentinelAddrIsEqual(sentinelAddr *a, sentinelAddr *b) {
return a->port == b->port && !strcasecmp(a->ip,b->ip);
}
int sentinelAddrEqualsHostname(sentinelAddr *a, char *hostname) {
char ip[NET_IP_STR_LEN];
if (anetResolve(NULL, hostname, ip, sizeof(ip),
sentinel.resolve_hostnames ? ANET_NONE : ANET_IP_ONLY) == ANET_ERR)
return 0;
return !strcasecmp(a->ip, ip);
}
const char *announceSentinelAddr(const sentinelAddr *a) {
return sentinel.announce_hostnames ? a->hostname : a->ip;
}
sds announceSentinelAddrAndPort(const sentinelAddr *a) {
const char *addr = announceSentinelAddr(a);
if (strchr(addr, ':') != NULL)
return sdscatprintf(sdsempty(), "[%s]:%d", addr, a->port);
else
return sdscatprintf(sdsempty(), "%s:%d", addr, a->port);
}
void sentinelEvent(int level, char *type, sentinelRedisInstance *ri,
const char *fmt, ...) {
va_list ap;
char msg[LOG_MAX_LEN];
robj *channel, *payload;
if (fmt[0] == '%' && fmt[1] == '@') {
sentinelRedisInstance *master = (ri->flags & SRI_MASTER) ?
NULL : ri->master;
if (master) {
snprintf(msg, sizeof(msg), "%s %s %s %d @ %s %s %d",
sentinelRedisInstanceTypeStr(ri),
ri->name, announceSentinelAddr(ri->addr), ri->addr->port,
master->name, announceSentinelAddr(master->addr), master->addr->port);
} else {
snprintf(msg, sizeof(msg), "%s %s %s %d",
sentinelRedisInstanceTypeStr(ri),
ri->name, announceSentinelAddr(ri->addr), ri->addr->port);
}
fmt += 2;
} else {
msg[0] = '\0';
}
if (fmt[0] != '\0') {
va_start(ap, fmt);
vsnprintf(msg+strlen(msg), sizeof(msg)-strlen(msg), fmt, ap);
va_end(ap);
}
if (level >= server.verbosity)
serverLog(level,"%s %s",type,msg);
if (level != LL_DEBUG) {
channel = createStringObject(type,strlen(type));
payload = createStringObject(msg,strlen(msg));
pubsubPublishMessage(channel,payload);
decrRefCount(channel);
decrRefCount(payload);
}
if (level == LL_WARNING && ri != NULL) {
sentinelRedisInstance *master = (ri->flags & SRI_MASTER) ?
ri : ri->master;
if (master && master->notification_script) {
sentinelScheduleScriptExecution(master->notification_script,
type,msg,NULL);
}
}
}
void sentinelGenerateInitialMonitorEvents(void) {
dictIterator *di;
dictEntry *de;
di = dictGetIterator(sentinel.masters);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
sentinelEvent(LL_WARNING,"+monitor",ri,"%@ quorum %d",ri->quorum);
}
dictReleaseIterator(di);
}
void sentinelReleaseScriptJob(sentinelScriptJob *sj) {
int j = 0;
while(sj->argv[j]) sdsfree(sj->argv[j++]);
zfree(sj->argv);
zfree(sj);
}
#define SENTINEL_SCRIPT_MAX_ARGS 16
void sentinelScheduleScriptExecution(char *path, ...) {
va_list ap;
char *argv[SENTINEL_SCRIPT_MAX_ARGS+1];
int argc = 1;
sentinelScriptJob *sj;
va_start(ap, path);
while(argc < SENTINEL_SCRIPT_MAX_ARGS) {
argv[argc] = va_arg(ap,char*);
if (!argv[argc]) break;
argv[argc] = sdsnew(argv[argc]);
argc++;
}
va_end(ap);
argv[0] = sdsnew(path);
sj = zmalloc(sizeof(*sj));
sj->flags = SENTINEL_SCRIPT_NONE;
sj->retry_num = 0;
sj->argv = zmalloc(sizeof(char*)*(argc+1));
sj->start_time = 0;
sj->pid = 0;
memcpy(sj->argv,argv,sizeof(char*)*(argc+1));
listAddNodeTail(sentinel.scripts_queue,sj);
if (listLength(sentinel.scripts_queue) > SENTINEL_SCRIPT_MAX_QUEUE) {
listNode *ln;
listIter li;
listRewind(sentinel.scripts_queue,&li);
while ((ln = listNext(&li)) != NULL) {
sj = ln->value;
if (sj->flags & SENTINEL_SCRIPT_RUNNING) continue;
listDelNode(sentinel.scripts_queue,ln);
sentinelReleaseScriptJob(sj);
break;
}
serverAssert(listLength(sentinel.scripts_queue) <=
SENTINEL_SCRIPT_MAX_QUEUE);
}
}
listNode *sentinelGetScriptListNodeByPid(pid_t pid) {
listNode *ln;
listIter li;
listRewind(sentinel.scripts_queue,&li);
while ((ln = listNext(&li)) != NULL) {
sentinelScriptJob *sj = ln->value;
if ((sj->flags & SENTINEL_SCRIPT_RUNNING) && sj->pid == pid)
return ln;
}
return NULL;
}
void sentinelRunPendingScripts(void) {
listNode *ln;
listIter li;
mstime_t now = mstime();
listRewind(sentinel.scripts_queue,&li);
while (sentinel.running_scripts < SENTINEL_SCRIPT_MAX_RUNNING &&
(ln = listNext(&li)) != NULL)
{
sentinelScriptJob *sj = ln->value;
pid_t pid;
if (sj->flags & SENTINEL_SCRIPT_RUNNING) continue;
if (sj->start_time && sj->start_time > now) continue;
sj->flags |= SENTINEL_SCRIPT_RUNNING;
sj->start_time = mstime();
sj->retry_num++;
pid = fork();
if (pid == -1) {
sentinelEvent(LL_WARNING,"-script-error",NULL,
"%s %d %d", sj->argv[0], 99, 0);
sj->flags &= ~SENTINEL_SCRIPT_RUNNING;
sj->pid = 0;
} else if (pid == 0) {
tlsCleanup();
execve(sj->argv[0],sj->argv,environ);
_exit(2);
} else {
sentinel.running_scripts++;
sj->pid = pid;
sentinelEvent(LL_DEBUG,"+script-child",NULL,"%ld",(long)pid);
}
}
}
mstime_t sentinelScriptRetryDelay(int retry_num) {
mstime_t delay = sentinel_script_retry_delay;
while (retry_num-- > 1) delay *= 2;
return delay;
}
void sentinelCollectTerminatedScripts(void) {
int statloc;
pid_t pid;
while ((pid = waitpid(-1, &statloc, WNOHANG)) > 0) {
int exitcode = WEXITSTATUS(statloc);
int bysignal = 0;
listNode *ln;
sentinelScriptJob *sj;
if (WIFSIGNALED(statloc)) bysignal = WTERMSIG(statloc);
sentinelEvent(LL_DEBUG,"-script-child",NULL,"%ld %d %d",
(long)pid, exitcode, bysignal);
ln = sentinelGetScriptListNodeByPid(pid);
if (ln == NULL) {
serverLog(LL_WARNING,"waitpid() returned a pid (%ld) we can't find in our scripts execution queue!", (long)pid);
continue;
}
sj = ln->value;
if ((bysignal || exitcode == 1) &&
sj->retry_num != SENTINEL_SCRIPT_MAX_RETRY)
{
sj->flags &= ~SENTINEL_SCRIPT_RUNNING;
sj->pid = 0;
sj->start_time = mstime() +
sentinelScriptRetryDelay(sj->retry_num);
} else {
if (bysignal || exitcode != 0) {
sentinelEvent(LL_WARNING,"-script-error",NULL,
"%s %d %d", sj->argv[0], bysignal, exitcode);
}
listDelNode(sentinel.scripts_queue,ln);
sentinelReleaseScriptJob(sj);
}
sentinel.running_scripts--;
}
}
void sentinelKillTimedoutScripts(void) {
listNode *ln;
listIter li;
mstime_t now = mstime();
listRewind(sentinel.scripts_queue,&li);
while ((ln = listNext(&li)) != NULL) {
sentinelScriptJob *sj = ln->value;
if (sj->flags & SENTINEL_SCRIPT_RUNNING &&
(now - sj->start_time) > sentinel_script_max_runtime)
{
sentinelEvent(LL_WARNING,"-script-timeout",NULL,"%s %ld",
sj->argv[0], (long)sj->pid);
kill(sj->pid,SIGKILL);
}
}
}
void sentinelPendingScriptsCommand(client *c) {
listNode *ln;
listIter li;
addReplyArrayLen(c,listLength(sentinel.scripts_queue));
listRewind(sentinel.scripts_queue,&li);
while ((ln = listNext(&li)) != NULL) {
sentinelScriptJob *sj = ln->value;
int j = 0;
addReplyMapLen(c,5);
addReplyBulkCString(c,"argv");
while (sj->argv[j]) j++;
addReplyArrayLen(c,j);
j = 0;
while (sj->argv[j]) addReplyBulkCString(c,sj->argv[j++]);
addReplyBulkCString(c,"flags");
addReplyBulkCString(c,
(sj->flags & SENTINEL_SCRIPT_RUNNING) ? "running" : "scheduled");
addReplyBulkCString(c,"pid");
addReplyBulkLongLong(c,sj->pid);
if (sj->flags & SENTINEL_SCRIPT_RUNNING) {
addReplyBulkCString(c,"run-time");
addReplyBulkLongLong(c,mstime() - sj->start_time);
} else {
mstime_t delay = sj->start_time ? (sj->start_time-mstime()) : 0;
if (delay < 0) delay = 0;
addReplyBulkCString(c,"run-delay");
addReplyBulkLongLong(c,delay);
}
addReplyBulkCString(c,"retry-num");
addReplyBulkLongLong(c,sj->retry_num);
}
}
void sentinelCallClientReconfScript(sentinelRedisInstance *master, int role, char *state, sentinelAddr *from, sentinelAddr *to) {
char fromport[32], toport[32];
if (master->client_reconfig_script == NULL) return;
ll2string(fromport,sizeof(fromport),from->port);
ll2string(toport,sizeof(toport),to->port);
sentinelScheduleScriptExecution(master->client_reconfig_script,
master->name,
(role == SENTINEL_LEADER) ? "leader" : "observer",
state, announceSentinelAddr(from), fromport,
announceSentinelAddr(to), toport, NULL);
}
instanceLink *createInstanceLink(void) {
instanceLink *link = zmalloc(sizeof(*link));
link->refcount = 1;
link->disconnected = 1;
link->pending_commands = 0;
link->cc = NULL;
link->pc = NULL;
link->cc_conn_time = 0;
link->pc_conn_time = 0;
link->last_reconn_time = 0;
link->pc_last_activity = 0;
link->act_ping_time = mstime();
link->last_ping_time = 0;
link->last_avail_time = mstime();
link->last_pong_time = mstime();
return link;
}
void instanceLinkCloseConnection(instanceLink *link, redisAsyncContext *c) {
if (c == NULL) return;
if (link->cc == c) {
link->cc = NULL;
link->pending_commands = 0;
}
if (link->pc == c) link->pc = NULL;
c->data = NULL;
link->disconnected = 1;
redisAsyncFree(c);
}
instanceLink *releaseInstanceLink(instanceLink *link, sentinelRedisInstance *ri)
{
serverAssert(link->refcount > 0);
link->refcount--;
if (link->refcount != 0) {
if (ri && ri->link->cc) {
redisCallback *cb;
redisCallbackList *callbacks = &link->cc->replies;
cb = callbacks->head;
while(cb) {
if (cb->privdata == ri) {
cb->fn = sentinelDiscardReplyCallback;
cb->privdata = NULL;
}
cb = cb->next;
}
}
return link;
}
instanceLinkCloseConnection(link,link->cc);
instanceLinkCloseConnection(link,link->pc);
zfree(link);
return NULL;
}
int sentinelTryConnectionSharing(sentinelRedisInstance *ri) {
serverAssert(ri->flags & SRI_SENTINEL);
dictIterator *di;
dictEntry *de;
if (ri->runid == NULL) return C_ERR;
if (ri->link->refcount > 1) return C_ERR;
di = dictGetIterator(sentinel.masters);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *master = dictGetVal(de), *match;
if (master == ri->master) continue;
match = getSentinelRedisInstanceByAddrAndRunID(master->sentinels,
NULL,0,ri->runid);
if (match == NULL) continue;
if (match == ri) continue;
releaseInstanceLink(ri->link,NULL);
ri->link = match->link;
match->link->refcount++;
dictReleaseIterator(di);
return C_OK;
}
dictReleaseIterator(di);
return C_ERR;
}
int sentinelDropConnections(void) {
dictIterator *di;
dictEntry *de;
int dropped = 0;
di = dictGetIterator(sentinel.masters);
while ((de = dictNext(di)) != NULL) {
dictIterator *sdi;
dictEntry *sde;
sentinelRedisInstance *ri = dictGetVal(de);
sdi = dictGetIterator(ri->sentinels);
while ((sde = dictNext(sdi)) != NULL) {
sentinelRedisInstance *si = dictGetVal(sde);
if (!si->link->disconnected) {
instanceLinkCloseConnection(si->link, si->link->pc);
instanceLinkCloseConnection(si->link, si->link->cc);
dropped++;
}
}
dictReleaseIterator(sdi);
}
dictReleaseIterator(di);
return dropped;
}
int sentinelUpdateSentinelAddressInAllMasters(sentinelRedisInstance *ri) {
serverAssert(ri->flags & SRI_SENTINEL);
dictIterator *di;
dictEntry *de;
int reconfigured = 0;
di = dictGetIterator(sentinel.masters);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *master = dictGetVal(de), *match;
match = getSentinelRedisInstanceByAddrAndRunID(master->sentinels,
NULL,0,ri->runid);
if (match == NULL) continue;
if (match->link->cc != NULL)
instanceLinkCloseConnection(match->link,match->link->cc);
if (match->link->pc != NULL)
instanceLinkCloseConnection(match->link,match->link->pc);
if (match->addr->port == 0)
dictDelete(master->sentinels,match->name);
if (match == ri) continue;
releaseSentinelAddr(match->addr);
match->addr = dupSentinelAddr(ri->addr);
reconfigured++;
}
dictReleaseIterator(di);
if (reconfigured)
sentinelEvent(LL_NOTICE,"+sentinel-address-update", ri,
"%@ %d additional matching instances", reconfigured);
return reconfigured;
}
void instanceLinkConnectionError(const redisAsyncContext *c) {
instanceLink *link = c->data;
int pubsub;
if (!link) return;
pubsub = (link->pc == c);
if (pubsub)
link->pc = NULL;
else
link->cc = NULL;
link->disconnected = 1;
}
void sentinelLinkEstablishedCallback(const redisAsyncContext *c, int status) {
if (status != C_OK) instanceLinkConnectionError(c);
}
void sentinelDisconnectCallback(const redisAsyncContext *c, int status) {
UNUSED(status);
instanceLinkConnectionError(c);
}
sentinelRedisInstance *createSentinelRedisInstance(char *name, int flags, char *hostname, int port, int quorum, sentinelRedisInstance *master) {
sentinelRedisInstance *ri;
sentinelAddr *addr;
dict *table = NULL;
sds sdsname;
serverAssert(flags & (SRI_MASTER|SRI_SLAVE|SRI_SENTINEL));
serverAssert((flags & SRI_MASTER) || master != NULL);
addr = createSentinelAddr(hostname,port);
if (addr == NULL) return NULL;
if (flags & SRI_SLAVE)
sdsname = announceSentinelAddrAndPort(addr);
else
sdsname = sdsnew(name);
if (flags & SRI_MASTER) table = sentinel.masters;
else if (flags & SRI_SLAVE) table = master->slaves;
else if (flags & SRI_SENTINEL) table = master->sentinels;
if (dictFind(table,sdsname)) {
releaseSentinelAddr(addr);
sdsfree(sdsname);
errno = EBUSY;
return NULL;
}
ri = zmalloc(sizeof(*ri));
ri->flags = flags;
ri->name = sdsname;
ri->runid = NULL;
ri->config_epoch = 0;
ri->addr = addr;
ri->link = createInstanceLink();
ri->last_pub_time = mstime();
ri->last_hello_time = mstime();
ri->last_master_down_reply_time = mstime();
ri->s_down_since_time = 0;
ri->o_down_since_time = 0;
ri->down_after_period = master ? master->down_after_period : sentinel_default_down_after;
ri->master_reboot_down_after_period = 0;
ri->master_link_down_time = 0;
ri->auth_pass = NULL;
ri->auth_user = NULL;
ri->slave_priority = SENTINEL_DEFAULT_SLAVE_PRIORITY;
ri->replica_announced = 1;
ri->slave_reconf_sent_time = 0;
ri->slave_master_host = NULL;
ri->slave_master_port = 0;
ri->slave_master_link_status = SENTINEL_MASTER_LINK_STATUS_DOWN;
ri->slave_repl_offset = 0;
ri->sentinels = dictCreate(&instancesDictType);
ri->quorum = quorum;
ri->parallel_syncs = SENTINEL_DEFAULT_PARALLEL_SYNCS;
ri->master = master;
ri->slaves = dictCreate(&instancesDictType);
ri->info_refresh = 0;
ri->renamed_commands = dictCreate(&renamedCommandsDictType);
ri->leader = NULL;
ri->leader_epoch = 0;
ri->failover_epoch = 0;
ri->failover_state = SENTINEL_FAILOVER_STATE_NONE;
ri->failover_state_change_time = 0;
ri->failover_start_time = 0;
ri->failover_timeout = sentinel_default_failover_timeout;
ri->failover_delay_logged = 0;
ri->promoted_slave = NULL;
ri->notification_script = NULL;
ri->client_reconfig_script = NULL;
ri->info = NULL;
ri->role_reported = ri->flags & (SRI_MASTER|SRI_SLAVE);
ri->role_reported_time = mstime();
ri->slave_conf_change_time = mstime();
dictAdd(table, ri->name, ri);
return ri;
}
void releaseSentinelRedisInstance(sentinelRedisInstance *ri) {
dictRelease(ri->sentinels);
dictRelease(ri->slaves);
releaseInstanceLink(ri->link,ri);
sdsfree(ri->name);
sdsfree(ri->runid);
sdsfree(ri->notification_script);
sdsfree(ri->client_reconfig_script);
sdsfree(ri->slave_master_host);
sdsfree(ri->leader);
sdsfree(ri->auth_pass);
sdsfree(ri->auth_user);
sdsfree(ri->info);
releaseSentinelAddr(ri->addr);
dictRelease(ri->renamed_commands);
if ((ri->flags & SRI_SLAVE) && (ri->flags & SRI_PROMOTED) && ri->master)
ri->master->promoted_slave = NULL;
zfree(ri);
}
sentinelRedisInstance *sentinelRedisInstanceLookupSlave(
sentinelRedisInstance *ri, char *slave_addr, int port)
{
sds key;
sentinelRedisInstance *slave;
sentinelAddr *addr;
serverAssert(ri->flags & SRI_MASTER);
addr = createSentinelAddr(slave_addr, port);
if (!addr) return NULL;
key = announceSentinelAddrAndPort(addr);
releaseSentinelAddr(addr);
slave = dictFetchValue(ri->slaves,key);
sdsfree(key);
return slave;
}
const char *sentinelRedisInstanceTypeStr(sentinelRedisInstance *ri) {
if (ri->flags & SRI_MASTER) return "master";
else if (ri->flags & SRI_SLAVE) return "slave";
else if (ri->flags & SRI_SENTINEL) return "sentinel";
else return "unknown";
}
int removeMatchingSentinelFromMaster(sentinelRedisInstance *master, char *runid) {
dictIterator *di;
dictEntry *de;
int removed = 0;
if (runid == NULL) return 0;
di = dictGetSafeIterator(master->sentinels);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
if (ri->runid && strcmp(ri->runid,runid) == 0) {
dictDelete(master->sentinels,ri->name);
removed++;
}
}
dictReleaseIterator(di);
return removed;
}
sentinelRedisInstance *getSentinelRedisInstanceByAddrAndRunID(dict *instances, char *addr, int port, char *runid) {
dictIterator *di;
dictEntry *de;
sentinelRedisInstance *instance = NULL;
sentinelAddr *ri_addr = NULL;
serverAssert(addr || runid);
if (addr != NULL) {
ri_addr = createSentinelAddr(addr, port);
if (!ri_addr) return NULL;
}
di = dictGetIterator(instances);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
if (runid && !ri->runid) continue;
if ((runid == NULL || strcmp(ri->runid, runid) == 0) &&
(addr == NULL || (strcmp(ri->addr->ip, ri_addr->ip) == 0 &&
ri->addr->port == port)))
{
instance = ri;
break;
}
}
dictReleaseIterator(di);
if (ri_addr != NULL)
releaseSentinelAddr(ri_addr);
return instance;
}
sentinelRedisInstance *sentinelGetMasterByName(char *name) {
sentinelRedisInstance *ri;
sds sdsname = sdsnew(name);
ri = dictFetchValue(sentinel.masters,sdsname);
sdsfree(sdsname);
return ri;
}
#define SENTINEL_RESET_NO_SENTINELS (1<<0)
void sentinelResetMaster(sentinelRedisInstance *ri, int flags) {
serverAssert(ri->flags & SRI_MASTER);
dictRelease(ri->slaves);
ri->slaves = dictCreate(&instancesDictType);
if (!(flags & SENTINEL_RESET_NO_SENTINELS)) {
dictRelease(ri->sentinels);
ri->sentinels = dictCreate(&instancesDictType);
}
instanceLinkCloseConnection(ri->link,ri->link->cc);
instanceLinkCloseConnection(ri->link,ri->link->pc);
ri->flags &= SRI_MASTER;
if (ri->leader) {
sdsfree(ri->leader);
ri->leader = NULL;
}
ri->failover_state = SENTINEL_FAILOVER_STATE_NONE;
ri->failover_state_change_time = 0;
ri->failover_start_time = 0;
ri->promoted_slave = NULL;
sdsfree(ri->runid);
sdsfree(ri->slave_master_host);
ri->runid = NULL;
ri->slave_master_host = NULL;
ri->link->act_ping_time = mstime();
ri->link->last_ping_time = 0;
ri->link->last_avail_time = mstime();
ri->link->last_pong_time = mstime();
ri->role_reported_time = mstime();
ri->role_reported = SRI_MASTER;
if (flags & SENTINEL_GENERATE_EVENT)
sentinelEvent(LL_WARNING,"+reset-master",ri,"%@");
}
int sentinelResetMastersByPattern(char *pattern, int flags) {
dictIterator *di;
dictEntry *de;
int reset = 0;
di = dictGetIterator(sentinel.masters);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
if (ri->name) {
if (stringmatch(pattern,ri->name,0)) {
sentinelResetMaster(ri,flags);
reset++;
}
}
}
dictReleaseIterator(di);
return reset;
}
int sentinelResetMasterAndChangeAddress(sentinelRedisInstance *master, char *hostname, int port) {
sentinelAddr *oldaddr, *newaddr;
sentinelAddr **slaves = NULL;
int numslaves = 0, j;
dictIterator *di;
dictEntry *de;
newaddr = createSentinelAddr(hostname,port);
if (newaddr == NULL) return C_ERR;
slaves = zmalloc(sizeof(sentinelAddr*)*(dictSize(master->slaves) + 1));
di = dictGetIterator(master->slaves);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *slave = dictGetVal(de);
if (sentinelAddrIsEqual(slave->addr,newaddr)) continue;
slaves[numslaves++] = dupSentinelAddr(slave->addr);
}
dictReleaseIterator(di);
if (!sentinelAddrIsEqual(newaddr,master->addr)) {
slaves[numslaves++] = dupSentinelAddr(master->addr);
}
sentinelResetMaster(master,SENTINEL_RESET_NO_SENTINELS);
oldaddr = master->addr;
master->addr = newaddr;
master->o_down_since_time = 0;
master->s_down_since_time = 0;
for (j = 0; j < numslaves; j++) {
sentinelRedisInstance *slave;
slave = createSentinelRedisInstance(NULL,SRI_SLAVE,slaves[j]->hostname,
slaves[j]->port, master->quorum, master);
releaseSentinelAddr(slaves[j]);
if (slave) sentinelEvent(LL_NOTICE,"+slave",slave,"%@");
}
zfree(slaves);
releaseSentinelAddr(oldaddr);
sentinelFlushConfig();
return C_OK;
}
int sentinelRedisInstanceNoDownFor(sentinelRedisInstance *ri, mstime_t ms) {
mstime_t most_recent;
most_recent = ri->s_down_since_time;
if (ri->o_down_since_time > most_recent)
most_recent = ri->o_down_since_time;
return most_recent == 0 || (mstime() - most_recent) > ms;
}
sentinelAddr *sentinelGetCurrentMasterAddress(sentinelRedisInstance *master) {
if ((master->flags & SRI_FAILOVER_IN_PROGRESS) &&
master->promoted_slave &&
master->failover_state >= SENTINEL_FAILOVER_STATE_RECONF_SLAVES)
{
return master->promoted_slave->addr;
} else {
return master->addr;
}
}
void sentinelPropagateDownAfterPeriod(sentinelRedisInstance *master) {
dictIterator *di;
dictEntry *de;
int j;
dict *d[] = {master->slaves, master->sentinels, NULL};
for (j = 0; d[j]; j++) {
di = dictGetIterator(d[j]);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
ri->down_after_period = master->down_after_period;
}
dictReleaseIterator(di);
}
}
char *sentinelInstanceMapCommand(sentinelRedisInstance *ri, char *command) {
sds sc = sdsnew(command);
if (ri->master) ri = ri->master;
char *retval = dictFetchValue(ri->renamed_commands, sc);
sdsfree(sc);
return retval ? retval : command;
}
const char *sentinelCheckCreateInstanceErrors(int role) {
switch(errno) {
case EBUSY:
switch (role) {
case SRI_MASTER:
return "Duplicate master name.";
case SRI_SLAVE:
return "Duplicate hostname and port for replica.";
case SRI_SENTINEL:
return "Duplicate runid for sentinel.";
default:
serverAssert(0);
break;
}
break;
case ENOENT:
return "Can't resolve instance hostname.";
case EINVAL:
return "Invalid port number.";
default:
return "Unknown Error for creating instances.";
}
}
void initializeSentinelConfig() {
server.sentinel_config = zmalloc(sizeof(struct sentinelConfig));
server.sentinel_config->monitor_cfg = listCreate();
server.sentinel_config->pre_monitor_cfg = listCreate();
server.sentinel_config->post_monitor_cfg = listCreate();
listSetFreeMethod(server.sentinel_config->monitor_cfg,freeSentinelLoadQueueEntry);
listSetFreeMethod(server.sentinel_config->pre_monitor_cfg,freeSentinelLoadQueueEntry);
listSetFreeMethod(server.sentinel_config->post_monitor_cfg,freeSentinelLoadQueueEntry);
}
void freeSentinelConfig() {
listRelease(server.sentinel_config->pre_monitor_cfg);
listRelease(server.sentinel_config->monitor_cfg);
listRelease(server.sentinel_config->post_monitor_cfg);
zfree(server.sentinel_config);
server.sentinel_config = NULL;
}
int searchPreMonitorCfgName(const char *name) {
for (unsigned int i = 0; i < sizeof(preMonitorCfgName)/sizeof(preMonitorCfgName[0]); i++) {
if (!strcasecmp(preMonitorCfgName[i],name)) return 1;
}
return 0;
}
void freeSentinelLoadQueueEntry(void *item) {
struct sentinelLoadQueueEntry *entry = item;
sdsfreesplitres(entry->argv,entry->argc);
sdsfree(entry->line);
zfree(entry);
}
void queueSentinelConfig(sds *argv, int argc, int linenum, sds line) {
int i;
struct sentinelLoadQueueEntry *entry;
if (server.sentinel_config == NULL) initializeSentinelConfig();
entry = zmalloc(sizeof(struct sentinelLoadQueueEntry));
entry->argv = zmalloc(sizeof(char*)*argc);
entry->argc = argc;
entry->linenum = linenum;
entry->line = sdsdup(line);
for (i = 0; i < argc; i++) {
entry->argv[i] = sdsdup(argv[i]);
}
if (!strcasecmp(argv[0],"monitor")) {
listAddNodeTail(server.sentinel_config->monitor_cfg,entry);
} else if (searchPreMonitorCfgName(argv[0])) {
listAddNodeTail(server.sentinel_config->pre_monitor_cfg,entry);
} else{
listAddNodeTail(server.sentinel_config->post_monitor_cfg,entry);
}
}
void loadSentinelConfigFromQueue(void) {
const char *err = NULL;
listIter li;
listNode *ln;
int linenum = 0;
sds line = NULL;
unsigned int j;
if (server.sentinel_config == NULL) return;
list *sentinel_configs[3] = {
server.sentinel_config->pre_monitor_cfg,
server.sentinel_config->monitor_cfg,
server.sentinel_config->post_monitor_cfg
};
for (j = 0; j < sizeof(sentinel_configs) / sizeof(sentinel_configs[0]); j++) {
listRewind(sentinel_configs[j],&li);
while((ln = listNext(&li))) {
struct sentinelLoadQueueEntry *entry = ln->value;
err = sentinelHandleConfiguration(entry->argv,entry->argc);
if (err) {
linenum = entry->linenum;
line = entry->line;
goto loaderr;
}
}
}
freeSentinelConfig();
return;
loaderr:
fprintf(stderr, "\n*** FATAL CONFIG FILE ERROR (Redis %s) ***\n",
REDIS_VERSION);
fprintf(stderr, "Reading the configuration file, at line %d\n", linenum);
fprintf(stderr, ">>> '%s'\n", line);
fprintf(stderr, "%s\n", err);
exit(1);
}
const char *sentinelHandleConfiguration(char **argv, int argc) {
sentinelRedisInstance *ri;
if (!strcasecmp(argv[0],"monitor") && argc == 5) {
int quorum = atoi(argv[4]);
if (quorum <= 0) return "Quorum must be 1 or greater.";
if (createSentinelRedisInstance(argv[1],SRI_MASTER,argv[2],
atoi(argv[3]),quorum,NULL) == NULL)
{
return sentinelCheckCreateInstanceErrors(SRI_MASTER);
}
} else if (!strcasecmp(argv[0],"down-after-milliseconds") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
ri->down_after_period = atoi(argv[2]);
if (ri->down_after_period <= 0)
return "negative or zero time parameter.";
sentinelPropagateDownAfterPeriod(ri);
} else if (!strcasecmp(argv[0],"failover-timeout") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
ri->failover_timeout = atoi(argv[2]);
if (ri->failover_timeout <= 0)
return "negative or zero time parameter.";
} else if (!strcasecmp(argv[0],"parallel-syncs") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
ri->parallel_syncs = atoi(argv[2]);
} else if (!strcasecmp(argv[0],"notification-script") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
if (access(argv[2],X_OK) == -1)
return "Notification script seems non existing or non executable.";
ri->notification_script = sdsnew(argv[2]);
} else if (!strcasecmp(argv[0],"client-reconfig-script") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
if (access(argv[2],X_OK) == -1)
return "Client reconfiguration script seems non existing or "
"non executable.";
ri->client_reconfig_script = sdsnew(argv[2]);
} else if (!strcasecmp(argv[0],"auth-pass") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
ri->auth_pass = sdsnew(argv[2]);
} else if (!strcasecmp(argv[0],"auth-user") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
ri->auth_user = sdsnew(argv[2]);
} else if (!strcasecmp(argv[0],"current-epoch") && argc == 2) {
unsigned long long current_epoch = strtoull(argv[1],NULL,10);
if (current_epoch > sentinel.current_epoch)
sentinel.current_epoch = current_epoch;
} else if (!strcasecmp(argv[0],"myid") && argc == 2) {
if (strlen(argv[1]) != CONFIG_RUN_ID_SIZE)
return "Malformed Sentinel id in myid option.";
memcpy(sentinel.myid,argv[1],CONFIG_RUN_ID_SIZE);
} else if (!strcasecmp(argv[0],"config-epoch") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
ri->config_epoch = strtoull(argv[2],NULL,10);
if (ri->config_epoch > sentinel.current_epoch)
sentinel.current_epoch = ri->config_epoch;
} else if (!strcasecmp(argv[0],"leader-epoch") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
ri->leader_epoch = strtoull(argv[2],NULL,10);
} else if ((!strcasecmp(argv[0],"known-slave") ||
!strcasecmp(argv[0],"known-replica")) && argc == 4)
{
sentinelRedisInstance *slave;
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
if ((slave = createSentinelRedisInstance(NULL,SRI_SLAVE,argv[2],
atoi(argv[3]), ri->quorum, ri)) == NULL)
{
return sentinelCheckCreateInstanceErrors(SRI_SLAVE);
}
} else if (!strcasecmp(argv[0],"known-sentinel") &&
(argc == 4 || argc == 5)) {
sentinelRedisInstance *si;
if (argc == 5) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
if ((si = createSentinelRedisInstance(argv[4],SRI_SENTINEL,argv[2],
atoi(argv[3]), ri->quorum, ri)) == NULL)
{
return sentinelCheckCreateInstanceErrors(SRI_SENTINEL);
}
si->runid = sdsnew(argv[4]);
sentinelTryConnectionSharing(si);
}
} else if (!strcasecmp(argv[0],"rename-command") && argc == 4) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
sds oldcmd = sdsnew(argv[2]);
sds newcmd = sdsnew(argv[3]);
if (dictAdd(ri->renamed_commands,oldcmd,newcmd) != DICT_OK) {
sdsfree(oldcmd);
sdsfree(newcmd);
return "Same command renamed multiple times with rename-command.";
}
} else if (!strcasecmp(argv[0],"announce-ip") && argc == 2) {
if (strlen(argv[1]))
sentinel.announce_ip = sdsnew(argv[1]);
} else if (!strcasecmp(argv[0],"announce-port") && argc == 2) {
sentinel.announce_port = atoi(argv[1]);
} else if (!strcasecmp(argv[0],"deny-scripts-reconfig") && argc == 2) {
if ((sentinel.deny_scripts_reconfig = yesnotoi(argv[1])) == -1) {
return "Please specify yes or no for the "
"deny-scripts-reconfig options.";
}
} else if (!strcasecmp(argv[0],"sentinel-user") && argc == 2) {
if (strlen(argv[1]))
sentinel.sentinel_auth_user = sdsnew(argv[1]);
} else if (!strcasecmp(argv[0],"sentinel-pass") && argc == 2) {
if (strlen(argv[1]))
sentinel.sentinel_auth_pass = sdsnew(argv[1]);
} else if (!strcasecmp(argv[0],"resolve-hostnames") && argc == 2) {
if ((sentinel.resolve_hostnames = yesnotoi(argv[1])) == -1) {
return "Please specify yes or no for the resolve-hostnames option.";
}
} else if (!strcasecmp(argv[0],"announce-hostnames") && argc == 2) {
if ((sentinel.announce_hostnames = yesnotoi(argv[1])) == -1) {
return "Please specify yes or no for the announce-hostnames option.";
}
} else if (!strcasecmp(argv[0],"master-reboot-down-after-period") && argc == 3) {
ri = sentinelGetMasterByName(argv[1]);
if (!ri) return "No such master with specified name.";
ri->master_reboot_down_after_period = atoi(argv[2]);
if (ri->master_reboot_down_after_period < 0)
return "negative time parameter.";
} else {
return "Unrecognized sentinel configuration statement.";
}
return NULL;
}
void rewriteConfigSentinelOption(struct rewriteConfigState *state) {
dictIterator *di, *di2;
dictEntry *de;
sds line;
line = sdscatprintf(sdsempty(), "sentinel myid %s", sentinel.myid);
rewriteConfigRewriteLine(state,"sentinel myid",line,1);
line = sdscatprintf(sdsempty(), "sentinel deny-scripts-reconfig %s",
sentinel.deny_scripts_reconfig ? "yes" : "no");
rewriteConfigRewriteLine(state,"sentinel deny-scripts-reconfig",line,
sentinel.deny_scripts_reconfig != SENTINEL_DEFAULT_DENY_SCRIPTS_RECONFIG);
line = sdscatprintf(sdsempty(), "sentinel resolve-hostnames %s",
sentinel.resolve_hostnames ? "yes" : "no");
rewriteConfigRewriteLine(state,"sentinel resolve-hostnames",line,
sentinel.resolve_hostnames != SENTINEL_DEFAULT_RESOLVE_HOSTNAMES);
line = sdscatprintf(sdsempty(), "sentinel announce-hostnames %s",
sentinel.announce_hostnames ? "yes" : "no");
rewriteConfigRewriteLine(state,"sentinel announce-hostnames",line,
sentinel.announce_hostnames != SENTINEL_DEFAULT_ANNOUNCE_HOSTNAMES);
di = dictGetIterator(sentinel.masters);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *master, *ri;
sentinelAddr *master_addr;
master = dictGetVal(de);
master_addr = sentinelGetCurrentMasterAddress(master);
line = sdscatprintf(sdsempty(),"sentinel monitor %s %s %d %d",
master->name, announceSentinelAddr(master_addr), master_addr->port,
master->quorum);
rewriteConfigRewriteLine(state,"sentinel monitor",line,1);
if (master->down_after_period != sentinel_default_down_after) {
line = sdscatprintf(sdsempty(),
"sentinel down-after-milliseconds %s %ld",
master->name, (long) master->down_after_period);
rewriteConfigRewriteLine(state,"sentinel down-after-milliseconds",line,1);
}
if (master->failover_timeout != sentinel_default_failover_timeout) {
line = sdscatprintf(sdsempty(),
"sentinel failover-timeout %s %ld",
master->name, (long) master->failover_timeout);
rewriteConfigRewriteLine(state,"sentinel failover-timeout",line,1);
}
if (master->parallel_syncs != SENTINEL_DEFAULT_PARALLEL_SYNCS) {
line = sdscatprintf(sdsempty(),
"sentinel parallel-syncs %s %d",
master->name, master->parallel_syncs);
rewriteConfigRewriteLine(state,"sentinel parallel-syncs",line,1);
}
if (master->notification_script) {
line = sdscatprintf(sdsempty(),
"sentinel notification-script %s %s",
master->name, master->notification_script);
rewriteConfigRewriteLine(state,"sentinel notification-script",line,1);
}
if (master->client_reconfig_script) {
line = sdscatprintf(sdsempty(),
"sentinel client-reconfig-script %s %s",
master->name, master->client_reconfig_script);
rewriteConfigRewriteLine(state,"sentinel client-reconfig-script",line,1);
}
if (master->auth_pass) {
line = sdscatprintf(sdsempty(),
"sentinel auth-pass %s %s",
master->name, master->auth_pass);
rewriteConfigRewriteLine(state,"sentinel auth-pass",line,1);
}
if (master->auth_user) {
line = sdscatprintf(sdsempty(),
"sentinel auth-user %s %s",
master->name, master->auth_user);
rewriteConfigRewriteLine(state,"sentinel auth-user",line,1);
}
if (master->master_reboot_down_after_period != 0) {
line = sdscatprintf(sdsempty(),
"sentinel master-reboot-down-after-period %s %ld",
master->name, (long) master->master_reboot_down_after_period);
rewriteConfigRewriteLine(state,"sentinel master-reboot-down-after-period",line,1);
}
line = sdscatprintf(sdsempty(),
"sentinel config-epoch %s %llu",
master->name, (unsigned long long) master->config_epoch);
rewriteConfigRewriteLine(state,"sentinel config-epoch",line,1);
line = sdscatprintf(sdsempty(),
"sentinel leader-epoch %s %llu",
master->name, (unsigned long long) master->leader_epoch);
rewriteConfigRewriteLine(state,"sentinel leader-epoch",line,1);
di2 = dictGetIterator(master->slaves);
while((de = dictNext(di2)) != NULL) {
sentinelAddr *slave_addr;
ri = dictGetVal(de);
slave_addr = ri->addr;
if (sentinelAddrIsEqual(slave_addr,master_addr))
slave_addr = master->addr;
line = sdscatprintf(sdsempty(),
"sentinel known-replica %s %s %d",
master->name, announceSentinelAddr(slave_addr), slave_addr->port);
rewriteConfigRewriteLine(state,"sentinel known-replica",line,1);
}
dictReleaseIterator(di2);
di2 = dictGetIterator(master->sentinels);
while((de = dictNext(di2)) != NULL) {
ri = dictGetVal(de);
if (ri->runid == NULL) continue;
line = sdscatprintf(sdsempty(),
"sentinel known-sentinel %s %s %d %s",
master->name, announceSentinelAddr(ri->addr), ri->addr->port, ri->runid);
rewriteConfigRewriteLine(state,"sentinel known-sentinel",line,1);
}
dictReleaseIterator(di2);
di2 = dictGetIterator(master->renamed_commands);
while((de = dictNext(di2)) != NULL) {
sds oldname = dictGetKey(de);
sds newname = dictGetVal(de);
line = sdscatprintf(sdsempty(),
"sentinel rename-command %s %s %s",
master->name, oldname, newname);
rewriteConfigRewriteLine(state,"sentinel rename-command",line,1);
}
dictReleaseIterator(di2);
}
line = sdscatprintf(sdsempty(),
"sentinel current-epoch %llu", (unsigned long long) sentinel.current_epoch);
rewriteConfigRewriteLine(state,"sentinel current-epoch",line,1);
if (sentinel.announce_ip) {
line = sdsnew("sentinel announce-ip ");
line = sdscatrepr(line, sentinel.announce_ip, sdslen(sentinel.announce_ip));
rewriteConfigRewriteLine(state,"sentinel announce-ip",line,1);
} else {
rewriteConfigMarkAsProcessed(state,"sentinel announce-ip");
}
if (sentinel.announce_port) {
line = sdscatprintf(sdsempty(),"sentinel announce-port %d",
sentinel.announce_port);
rewriteConfigRewriteLine(state,"sentinel announce-port",line,1);
} else {
rewriteConfigMarkAsProcessed(state,"sentinel announce-port");
}
if (sentinel.sentinel_auth_user) {
line = sdscatprintf(sdsempty(), "sentinel sentinel-user %s", sentinel.sentinel_auth_user);
rewriteConfigRewriteLine(state,"sentinel sentinel-user",line,1);
} else {
rewriteConfigMarkAsProcessed(state,"sentinel sentinel-user");
}
if (sentinel.sentinel_auth_pass) {
line = sdscatprintf(sdsempty(), "sentinel sentinel-pass %s", sentinel.sentinel_auth_pass);
rewriteConfigRewriteLine(state,"sentinel sentinel-pass",line,1);
} else {
rewriteConfigMarkAsProcessed(state,"sentinel sentinel-pass");
}
dictReleaseIterator(di);
rewriteConfigMarkAsProcessed(state,"sentinel monitor");
rewriteConfigMarkAsProcessed(state,"sentinel down-after-milliseconds");
rewriteConfigMarkAsProcessed(state,"sentinel failover-timeout");
rewriteConfigMarkAsProcessed(state,"sentinel parallel-syncs");
rewriteConfigMarkAsProcessed(state,"sentinel notification-script");
rewriteConfigMarkAsProcessed(state,"sentinel client-reconfig-script");
rewriteConfigMarkAsProcessed(state,"sentinel auth-pass");
rewriteConfigMarkAsProcessed(state,"sentinel auth-user");
rewriteConfigMarkAsProcessed(state,"sentinel config-epoch");
rewriteConfigMarkAsProcessed(state,"sentinel leader-epoch");
rewriteConfigMarkAsProcessed(state,"sentinel known-replica");
rewriteConfigMarkAsProcessed(state,"sentinel known-sentinel");
rewriteConfigMarkAsProcessed(state,"sentinel rename-command");
rewriteConfigMarkAsProcessed(state,"sentinel master-reboot-down-after-period");
}
int sentinelFlushConfig(void) {
int fd = -1;
int saved_hz = server.hz;
int rewrite_status;
server.hz = CONFIG_DEFAULT_HZ;
rewrite_status = rewriteConfig(server.configfile, 0);
server.hz = saved_hz;
if (rewrite_status == -1) goto werr;
if ((fd = open(server.configfile,O_RDONLY)) == -1) goto werr;
if (fsync(fd) == -1) goto werr;
if (close(fd) == EOF) goto werr;
serverLog(LL_NOTICE,"Sentinel new configuration saved on disk");
return C_OK;
werr:
serverLog(LL_WARNING,"WARNING: Sentinel was not able to save the new configuration on disk!!!: %s", strerror(errno));
if (fd != -1) close(fd);
return C_ERR;
}
void sentinelSendAuthIfNeeded(sentinelRedisInstance *ri, redisAsyncContext *c) {
char *auth_pass = NULL;
char *auth_user = NULL;
if (ri->flags & SRI_MASTER) {
auth_pass = ri->auth_pass;
auth_user = ri->auth_user;
} else if (ri->flags & SRI_SLAVE) {
auth_pass = ri->master->auth_pass;
auth_user = ri->master->auth_user;
} else if (ri->flags & SRI_SENTINEL) {
if (sentinel.sentinel_auth_pass) {
auth_pass = sentinel.sentinel_auth_pass;
auth_user = sentinel.sentinel_auth_user;
} else {
auth_pass = server.requirepass;
auth_user = NULL;
}
}
if (auth_pass && auth_user == NULL) {
if (redisAsyncCommand(c, sentinelDiscardReplyCallback, ri, "%s %s",
sentinelInstanceMapCommand(ri,"AUTH"),
auth_pass) == C_OK) ri->link->pending_commands++;
} else if (auth_pass && auth_user) {
if (redisAsyncCommand(c, sentinelDiscardReplyCallback, ri, "%s %s %s",
sentinelInstanceMapCommand(ri,"AUTH"),
auth_user, auth_pass) == C_OK) ri->link->pending_commands++;
}
}
void sentinelSetClientName(sentinelRedisInstance *ri, redisAsyncContext *c, char *type) {
char name[64];
snprintf(name,sizeof(name),"sentinel-%.8s-%s",sentinel.myid,type);
if (redisAsyncCommand(c, sentinelDiscardReplyCallback, ri,
"%s SETNAME %s",
sentinelInstanceMapCommand(ri,"CLIENT"),
name) == C_OK)
{
ri->link->pending_commands++;
}
}
static int instanceLinkNegotiateTLS(redisAsyncContext *context) {
#if !defined(USE_OPENSSL)
(void) context;
#else
if (!redis_tls_ctx) return C_ERR;
SSL *ssl = SSL_new(redis_tls_client_ctx ? redis_tls_client_ctx : redis_tls_ctx);
if (!ssl) return C_ERR;
if (redisInitiateSSL(&context->c, ssl) == REDIS_ERR) {
SSL_free(ssl);
return C_ERR;
}
#endif
return C_OK;
}
void sentinelReconnectInstance(sentinelRedisInstance *ri) {
if (ri->link->disconnected == 0) return;
if (ri->addr->port == 0) return;
instanceLink *link = ri->link;
mstime_t now = mstime();
if (now - ri->link->last_reconn_time < sentinel_ping_period) return;
ri->link->last_reconn_time = now;
if (link->cc == NULL) {
link->cc = redisAsyncConnectBind(ri->addr->ip,ri->addr->port,server.bind_source_addr);
if (link->cc && !link->cc->err) anetCloexec(link->cc->c.fd);
if (!link->cc) {
sentinelEvent(LL_DEBUG,"-cmd-link-reconnection",ri,"%@ #Failed to establish connection");
} else if (!link->cc->err && server.tls_replication &&
(instanceLinkNegotiateTLS(link->cc) == C_ERR)) {
sentinelEvent(LL_DEBUG,"-cmd-link-reconnection",ri,"%@ #Failed to initialize TLS");
instanceLinkCloseConnection(link,link->cc);
} else if (link->cc->err) {
sentinelEvent(LL_DEBUG,"-cmd-link-reconnection",ri,"%@ #%s",
link->cc->errstr);
instanceLinkCloseConnection(link,link->cc);
} else {
link->pending_commands = 0;
link->cc_conn_time = mstime();
link->cc->data = link;
redisAeAttach(server.el,link->cc);
redisAsyncSetConnectCallback(link->cc,
sentinelLinkEstablishedCallback);
redisAsyncSetDisconnectCallback(link->cc,
sentinelDisconnectCallback);
sentinelSendAuthIfNeeded(ri,link->cc);
sentinelSetClientName(ri,link->cc,"cmd");
sentinelSendPing(ri);
}
}
if ((ri->flags & (SRI_MASTER|SRI_SLAVE)) && link->pc == NULL) {
link->pc = redisAsyncConnectBind(ri->addr->ip,ri->addr->port,server.bind_source_addr);
if (link->pc && !link->pc->err) anetCloexec(link->pc->c.fd);
if (!link->pc) {
sentinelEvent(LL_DEBUG,"-pubsub-link-reconnection",ri,"%@ #Failed to establish connection");
} else if (!link->pc->err && server.tls_replication &&
(instanceLinkNegotiateTLS(link->pc) == C_ERR)) {
sentinelEvent(LL_DEBUG,"-pubsub-link-reconnection",ri,"%@ #Failed to initialize TLS");
} else if (link->pc->err) {
sentinelEvent(LL_DEBUG,"-pubsub-link-reconnection",ri,"%@ #%s",
link->pc->errstr);
instanceLinkCloseConnection(link,link->pc);
} else {
int retval;
link->pc_conn_time = mstime();
link->pc->data = link;
redisAeAttach(server.el,link->pc);
redisAsyncSetConnectCallback(link->pc,
sentinelLinkEstablishedCallback);
redisAsyncSetDisconnectCallback(link->pc,
sentinelDisconnectCallback);
sentinelSendAuthIfNeeded(ri,link->pc);
sentinelSetClientName(ri,link->pc,"pubsub");
retval = redisAsyncCommand(link->pc,
sentinelReceiveHelloMessages, ri, "%s %s",
sentinelInstanceMapCommand(ri,"SUBSCRIBE"),
SENTINEL_HELLO_CHANNEL);
if (retval != C_OK) {
instanceLinkCloseConnection(link,link->pc);
return;
}
}
}
if (link->cc && (ri->flags & SRI_SENTINEL || link->pc))
link->disconnected = 0;
}
int sentinelMasterLooksSane(sentinelRedisInstance *master) {
return
master->flags & SRI_MASTER &&
master->role_reported == SRI_MASTER &&
(master->flags & (SRI_S_DOWN|SRI_O_DOWN)) == 0 &&
(mstime() - master->info_refresh) < sentinel_info_period*2;
}
void sentinelRefreshInstanceInfo(sentinelRedisInstance *ri, const char *info) {
sds *lines;
int numlines, j;
int role = 0;
sdsfree(ri->info);
ri->info = sdsnew(info);
ri->master_link_down_time = 0;
lines = sdssplitlen(info,strlen(info),"\r\n",2,&numlines);
for (j = 0; j < numlines; j++) {
sentinelRedisInstance *slave;
sds l = lines[j];
if (sdslen(l) >= 47 && !memcmp(l,"run_id:",7)) {
if (ri->runid == NULL) {
ri->runid = sdsnewlen(l+7,40);
} else {
if (strncmp(ri->runid,l+7,40) != 0) {
sentinelEvent(LL_NOTICE,"+reboot",ri,"%@");
if (ri->flags & SRI_MASTER && ri->master_reboot_down_after_period != 0) {
ri->flags |= SRI_MASTER_REBOOT;
ri->master_reboot_since_time = mstime();
}
sdsfree(ri->runid);
ri->runid = sdsnewlen(l+7,40);
}
}
}
if ((ri->flags & SRI_MASTER) &&
sdslen(l) >= 7 &&
!memcmp(l,"slave",5) && isdigit(l[5]))
{
char *ip, *port, *end;
if (strstr(l,"ip=") == NULL) {
ip = strchr(l,':'); if (!ip) continue;
ip++;
port = strchr(ip,','); if (!port) continue;
*port = '\0';
port++;
end = strchr(port,','); if (!end) continue;
*end = '\0';
} else {
ip = strstr(l,"ip="); if (!ip) continue;
ip += 3;
port = strstr(l,"port="); if (!port) continue;
port += 5;
end = strchr(ip,','); if (end) *end = '\0';
end = strchr(port,','); if (end) *end = '\0';
}
if (sentinelRedisInstanceLookupSlave(ri,ip,atoi(port)) == NULL) {
if ((slave = createSentinelRedisInstance(NULL,SRI_SLAVE,ip,
atoi(port), ri->quorum, ri)) != NULL)
{
sentinelEvent(LL_NOTICE,"+slave",slave,"%@");
sentinelFlushConfig();
}
}
}
if (sdslen(l) >= 32 &&
!memcmp(l,"master_link_down_since_seconds",30))
{
ri->master_link_down_time = strtoll(l+31,NULL,10)*1000;
}
if (sdslen(l) >= 11 && !memcmp(l,"role:master",11)) role = SRI_MASTER;
else if (sdslen(l) >= 10 && !memcmp(l,"role:slave",10)) role = SRI_SLAVE;
if (role == SRI_SLAVE) {
if (sdslen(l) >= 12 && !memcmp(l,"master_host:",12)) {
if (ri->slave_master_host == NULL ||
strcasecmp(l+12,ri->slave_master_host))
{
sdsfree(ri->slave_master_host);
ri->slave_master_host = sdsnew(l+12);
ri->slave_conf_change_time = mstime();
}
}
if (sdslen(l) >= 12 && !memcmp(l,"master_port:",12)) {
int slave_master_port = atoi(l+12);
if (ri->slave_master_port != slave_master_port) {
ri->slave_master_port = slave_master_port;
ri->slave_conf_change_time = mstime();
}
}
if (sdslen(l) >= 19 && !memcmp(l,"master_link_status:",19)) {
ri->slave_master_link_status =
(strcasecmp(l+19,"up") == 0) ?
SENTINEL_MASTER_LINK_STATUS_UP :
SENTINEL_MASTER_LINK_STATUS_DOWN;
}
if (sdslen(l) >= 15 && !memcmp(l,"slave_priority:",15))
ri->slave_priority = atoi(l+15);
if (sdslen(l) >= 18 && !memcmp(l,"slave_repl_offset:",18))
ri->slave_repl_offset = strtoull(l+18,NULL,10);
if (sdslen(l) >= 18 && !memcmp(l,"replica_announced:",18))
ri->replica_announced = atoi(l+18);
}
}
ri->info_refresh = mstime();
sdsfreesplitres(lines,numlines);
if (role != ri->role_reported) {
ri->role_reported_time = mstime();
ri->role_reported = role;
if (role == SRI_SLAVE) ri->slave_conf_change_time = mstime();
sentinelEvent(LL_VERBOSE,
((ri->flags & (SRI_MASTER|SRI_SLAVE)) == role) ?
"+role-change" : "-role-change",
ri, "%@ new reported role is %s",
role == SRI_MASTER ? "master" : "slave");
}
if (sentinel.tilt) return;
if ((ri->flags & SRI_MASTER) && role == SRI_SLAVE) {
}
if ((ri->flags & SRI_SLAVE) && role == SRI_MASTER) {
if ((ri->flags & SRI_PROMOTED) &&
(ri->master->flags & SRI_FAILOVER_IN_PROGRESS) &&
(ri->master->failover_state ==
SENTINEL_FAILOVER_STATE_WAIT_PROMOTION))
{
ri->master->config_epoch = ri->master->failover_epoch;
ri->master->failover_state = SENTINEL_FAILOVER_STATE_RECONF_SLAVES;
ri->master->failover_state_change_time = mstime();
sentinelFlushConfig();
sentinelEvent(LL_WARNING,"+promoted-slave",ri,"%@");
if (sentinel.simfailure_flags &
SENTINEL_SIMFAILURE_CRASH_AFTER_PROMOTION)
sentinelSimFailureCrash();
sentinelEvent(LL_WARNING,"+failover-state-reconf-slaves",
ri->master,"%@");
sentinelCallClientReconfScript(ri->master,SENTINEL_LEADER,
"start",ri->master->addr,ri->addr);
sentinelForceHelloUpdateForMaster(ri->master);
} else {
mstime_t wait_time = sentinel_publish_period*4;
if (!(ri->flags & SRI_PROMOTED) &&
sentinelMasterLooksSane(ri->master) &&
sentinelRedisInstanceNoDownFor(ri,wait_time) &&
mstime() - ri->role_reported_time > wait_time)
{
int retval = sentinelSendSlaveOf(ri,ri->master->addr);
if (retval == C_OK)
sentinelEvent(LL_NOTICE,"+convert-to-slave",ri,"%@");
}
}
}
if ((ri->flags & SRI_SLAVE) &&
role == SRI_SLAVE &&
(ri->slave_master_port != ri->master->addr->port ||
!sentinelAddrEqualsHostname(ri->master->addr, ri->slave_master_host)))
{
mstime_t wait_time = ri->master->failover_timeout;
if (sentinelMasterLooksSane(ri->master) &&
sentinelRedisInstanceNoDownFor(ri,wait_time) &&
mstime() - ri->slave_conf_change_time > wait_time)
{
int retval = sentinelSendSlaveOf(ri,ri->master->addr);
if (retval == C_OK)
sentinelEvent(LL_NOTICE,"+fix-slave-config",ri,"%@");
}
}
if ((ri->flags & SRI_SLAVE) && role == SRI_SLAVE &&
(ri->flags & (SRI_RECONF_SENT|SRI_RECONF_INPROG)))
{
if ((ri->flags & SRI_RECONF_SENT) &&
ri->slave_master_host &&
sentinelAddrEqualsHostname(ri->master->promoted_slave->addr,
ri->slave_master_host) &&
ri->slave_master_port == ri->master->promoted_slave->addr->port)
{
ri->flags &= ~SRI_RECONF_SENT;
ri->flags |= SRI_RECONF_INPROG;
sentinelEvent(LL_NOTICE,"+slave-reconf-inprog",ri,"%@");
}
if ((ri->flags & SRI_RECONF_INPROG) &&
ri->slave_master_link_status == SENTINEL_MASTER_LINK_STATUS_UP)
{
ri->flags &= ~SRI_RECONF_INPROG;
ri->flags |= SRI_RECONF_DONE;
sentinelEvent(LL_NOTICE,"+slave-reconf-done",ri,"%@");
}
}
}
void sentinelInfoReplyCallback(redisAsyncContext *c, void *reply, void *privdata) {
sentinelRedisInstance *ri = privdata;
instanceLink *link = c->data;
redisReply *r;
if (!reply || !link) return;
link->pending_commands--;
r = reply;
if (r->type == REDIS_REPLY_STRING)
sentinelRefreshInstanceInfo(ri,r->str);
}
void sentinelDiscardReplyCallback(redisAsyncContext *c, void *reply, void *privdata) {
instanceLink *link = c->data;
UNUSED(reply);
UNUSED(privdata);
if (link) link->pending_commands--;
}
void sentinelPingReplyCallback(redisAsyncContext *c, void *reply, void *privdata) {
sentinelRedisInstance *ri = privdata;
instanceLink *link = c->data;
redisReply *r;
if (!reply || !link) return;
link->pending_commands--;
r = reply;
if (r->type == REDIS_REPLY_STATUS ||
r->type == REDIS_REPLY_ERROR) {
if (strncmp(r->str,"PONG",4) == 0 ||
strncmp(r->str,"LOADING",7) == 0 ||
strncmp(r->str,"MASTERDOWN",10) == 0)
{
link->last_avail_time = mstime();
link->act_ping_time = 0;
if (ri->flags & SRI_MASTER_REBOOT && strncmp(r->str,"PONG",4) == 0)
ri->flags &= ~SRI_MASTER_REBOOT;
} else {
if (strncmp(r->str,"BUSY",4) == 0 &&
(ri->flags & SRI_S_DOWN) &&
!(ri->flags & SRI_SCRIPT_KILL_SENT))
{
if (redisAsyncCommand(ri->link->cc,
sentinelDiscardReplyCallback, ri,
"%s KILL",
sentinelInstanceMapCommand(ri,"SCRIPT")) == C_OK)
{
ri->link->pending_commands++;
}
ri->flags |= SRI_SCRIPT_KILL_SENT;
}
}
}
link->last_pong_time = mstime();
}
void sentinelPublishReplyCallback(redisAsyncContext *c, void *reply, void *privdata) {
sentinelRedisInstance *ri = privdata;
instanceLink *link = c->data;
redisReply *r;
if (!reply || !link) return;
link->pending_commands--;
r = reply;
if (r->type != REDIS_REPLY_ERROR)
ri->last_pub_time = mstime();
}
void sentinelProcessHelloMessage(char *hello, int hello_len) {
int numtokens, port, removed, master_port;
uint64_t current_epoch, master_config_epoch;
char **token = sdssplitlen(hello, hello_len, ",", 1, &numtokens);
sentinelRedisInstance *si, *master;
if (numtokens == 8) {
master = sentinelGetMasterByName(token[4]);
if (!master) goto cleanup;
port = atoi(token[1]);
master_port = atoi(token[6]);
si = getSentinelRedisInstanceByAddrAndRunID(
master->sentinels,token[0],port,token[2]);
current_epoch = strtoull(token[3],NULL,10);
master_config_epoch = strtoull(token[7],NULL,10);
if (!si) {
removed = removeMatchingSentinelFromMaster(master,token[2]);
if (removed) {
sentinelEvent(LL_NOTICE,"+sentinel-address-switch",master,
"%@ ip %s port %d for %s", token[0],port,token[2]);
} else {
sentinelRedisInstance *other =
getSentinelRedisInstanceByAddrAndRunID(
master->sentinels, token[0],port,NULL);
if (other) {
sentinelEvent(LL_NOTICE,"+sentinel-invalid-addr",other,"%@");
other->addr->port = 0;
sentinelUpdateSentinelAddressInAllMasters(other);
}
}
si = createSentinelRedisInstance(token[2],SRI_SENTINEL,
token[0],port,master->quorum,master);
if (si) {
if (!removed) sentinelEvent(LL_NOTICE,"+sentinel",si,"%@");
si->runid = sdsnew(token[2]);
sentinelTryConnectionSharing(si);
if (removed) sentinelUpdateSentinelAddressInAllMasters(si);
sentinelFlushConfig();
}
}
if (current_epoch > sentinel.current_epoch) {
sentinel.current_epoch = current_epoch;
sentinelFlushConfig();
sentinelEvent(LL_WARNING,"+new-epoch",master,"%llu",
(unsigned long long) sentinel.current_epoch);
}
if (si && master->config_epoch < master_config_epoch) {
master->config_epoch = master_config_epoch;
if (master_port != master->addr->port ||
!sentinelAddrEqualsHostname(master->addr, token[5]))
{
sentinelAddr *old_addr;
sentinelEvent(LL_WARNING,"+config-update-from",si,"%@");
sentinelEvent(LL_WARNING,"+switch-master",
master,"%s %s %d %s %d",
master->name,
announceSentinelAddr(master->addr), master->addr->port,
token[5], master_port);
old_addr = dupSentinelAddr(master->addr);
sentinelResetMasterAndChangeAddress(master, token[5], master_port);
sentinelCallClientReconfScript(master,
SENTINEL_OBSERVER,"start",
old_addr,master->addr);
releaseSentinelAddr(old_addr);
}
}
if (si) si->last_hello_time = mstime();
}
cleanup:
sdsfreesplitres(token,numtokens);
}
void sentinelReceiveHelloMessages(redisAsyncContext *c, void *reply, void *privdata) {
sentinelRedisInstance *ri = privdata;
redisReply *r;
UNUSED(c);
if (!reply || !ri) return;
r = reply;
ri->link->pc_last_activity = mstime();
if (r->type != REDIS_REPLY_ARRAY ||
r->elements != 3 ||
r->element[0]->type != REDIS_REPLY_STRING ||
r->element[1]->type != REDIS_REPLY_STRING ||
r->element[2]->type != REDIS_REPLY_STRING ||
strcmp(r->element[0]->str,"message") != 0) return;
if (strstr(r->element[2]->str,sentinel.myid) != NULL) return;
sentinelProcessHelloMessage(r->element[2]->str, r->element[2]->len);
}
int sentinelSendHello(sentinelRedisInstance *ri) {
char ip[NET_IP_STR_LEN];
char payload[NET_IP_STR_LEN+1024];
int retval;
char *announce_ip;
int announce_port;
sentinelRedisInstance *master = (ri->flags & SRI_MASTER) ? ri : ri->master;
sentinelAddr *master_addr = sentinelGetCurrentMasterAddress(master);
if (ri->link->disconnected) return C_ERR;
if (sentinel.announce_ip) {
announce_ip = sentinel.announce_ip;
} else {
if (anetFdToString(ri->link->cc->c.fd,ip,sizeof(ip),NULL,FD_TO_SOCK_NAME) == -1)
return C_ERR;
announce_ip = ip;
}
if (sentinel.announce_port) announce_port = sentinel.announce_port;
else if (server.tls_replication && server.tls_port) announce_port = server.tls_port;
else announce_port = server.port;
snprintf(payload,sizeof(payload),
"%s,%d,%s,%llu,"
"%s,%s,%d,%llu",
announce_ip, announce_port, sentinel.myid,
(unsigned long long) sentinel.current_epoch,
master->name,announceSentinelAddr(master_addr),master_addr->port,
(unsigned long long) master->config_epoch);
retval = redisAsyncCommand(ri->link->cc,
sentinelPublishReplyCallback, ri, "%s %s %s",
sentinelInstanceMapCommand(ri,"PUBLISH"),
SENTINEL_HELLO_CHANNEL,payload);
if (retval != C_OK) return C_ERR;
ri->link->pending_commands++;
return C_OK;
}
void sentinelForceHelloUpdateDictOfRedisInstances(dict *instances) {
dictIterator *di;
dictEntry *de;
di = dictGetSafeIterator(instances);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
if (ri->last_pub_time >= (sentinel_publish_period+1))
ri->last_pub_time -= (sentinel_publish_period+1);
}
dictReleaseIterator(di);
}
int sentinelForceHelloUpdateForMaster(sentinelRedisInstance *master) {
if (!(master->flags & SRI_MASTER)) return C_ERR;
if (master->last_pub_time >= (sentinel_publish_period+1))
master->last_pub_time -= (sentinel_publish_period+1);
sentinelForceHelloUpdateDictOfRedisInstances(master->sentinels);
sentinelForceHelloUpdateDictOfRedisInstances(master->slaves);
return C_OK;
}
int sentinelSendPing(sentinelRedisInstance *ri) {
int retval = redisAsyncCommand(ri->link->cc,
sentinelPingReplyCallback, ri, "%s",
sentinelInstanceMapCommand(ri,"PING"));
if (retval == C_OK) {
ri->link->pending_commands++;
ri->link->last_ping_time = mstime();
if (ri->link->act_ping_time == 0)
ri->link->act_ping_time = ri->link->last_ping_time;
return 1;
} else {
return 0;
}
}
void sentinelSendPeriodicCommands(sentinelRedisInstance *ri) {
mstime_t now = mstime();
mstime_t info_period, ping_period;
int retval;
if (ri->link->disconnected) return;
if (ri->link->pending_commands >=
SENTINEL_MAX_PENDING_COMMANDS * ri->link->refcount) return;
if ((ri->flags & SRI_SLAVE) &&
((ri->master->flags & (SRI_O_DOWN|SRI_FAILOVER_IN_PROGRESS)) ||
(ri->master_link_down_time != 0)))
{
info_period = 1000;
} else {
info_period = sentinel_info_period;
}
ping_period = ri->down_after_period;
if (ping_period > sentinel_ping_period) ping_period = sentinel_ping_period;
if ((ri->flags & SRI_SENTINEL) == 0 &&
(ri->info_refresh == 0 ||
(now - ri->info_refresh) > info_period))
{
retval = redisAsyncCommand(ri->link->cc,
sentinelInfoReplyCallback, ri, "%s",
sentinelInstanceMapCommand(ri,"INFO"));
if (retval == C_OK) ri->link->pending_commands++;
}
if ((now - ri->link->last_pong_time) > ping_period &&
(now - ri->link->last_ping_time) > ping_period/2) {
sentinelSendPing(ri);
}
if ((now - ri->last_pub_time) > sentinel_publish_period) {
sentinelSendHello(ri);
}
}
void sentinelConfigSetCommand(client *c) {
robj *o = c->argv[3];
robj *val = c->argv[4];
long long numval;
int drop_conns = 0;
if (!strcasecmp(o->ptr, "resolve-hostnames")) {
if ((numval = yesnotoi(val->ptr)) == -1) goto badfmt;
sentinel.resolve_hostnames = numval;
} else if (!strcasecmp(o->ptr, "announce-hostnames")) {
if ((numval = yesnotoi(val->ptr)) == -1) goto badfmt;
sentinel.announce_hostnames = numval;
} else if (!strcasecmp(o->ptr, "announce-ip")) {
if (sentinel.announce_ip) sdsfree(sentinel.announce_ip);
sentinel.announce_ip = sdsnew(val->ptr);
} else if (!strcasecmp(o->ptr, "announce-port")) {
if (getLongLongFromObject(val, &numval) == C_ERR ||
numval < 0 || numval > 65535)
goto badfmt;
sentinel.announce_port = numval;
} else if (!strcasecmp(o->ptr, "sentinel-user")) {
sdsfree(sentinel.sentinel_auth_user);
sentinel.sentinel_auth_user = sdslen(val->ptr) == 0 ?
NULL : sdsdup(val->ptr);
drop_conns = 1;
} else if (!strcasecmp(o->ptr, "sentinel-pass")) {
sdsfree(sentinel.sentinel_auth_pass);
sentinel.sentinel_auth_pass = sdslen(val->ptr) == 0 ?
NULL : sdsdup(val->ptr);
drop_conns = 1;
} else {
addReplyErrorFormat(c, "Invalid argument '%s' to SENTINEL CONFIG SET",
(char *) o->ptr);
return;
}
sentinelFlushConfig();
addReply(c, shared.ok);
if (drop_conns)
sentinelDropConnections();
return;
badfmt:
addReplyErrorFormat(c, "Invalid value '%s' to SENTINEL CONFIG SET '%s'",
(char *) val->ptr, (char *) o->ptr);
}
void sentinelConfigGetCommand(client *c) {
robj *o = c->argv[3];
const char *pattern = o->ptr;
void *replylen = addReplyDeferredLen(c);
int matches = 0;
if (stringmatch(pattern,"resolve-hostnames",1)) {
addReplyBulkCString(c,"resolve-hostnames");
addReplyBulkCString(c,sentinel.resolve_hostnames ? "yes" : "no");
matches++;
}
if (stringmatch(pattern, "announce-hostnames", 1)) {
addReplyBulkCString(c,"announce-hostnames");
addReplyBulkCString(c,sentinel.announce_hostnames ? "yes" : "no");
matches++;
}
if (stringmatch(pattern, "announce-ip", 1)) {
addReplyBulkCString(c,"announce-ip");
addReplyBulkCString(c,sentinel.announce_ip ? sentinel.announce_ip : "");
matches++;
}
if (stringmatch(pattern, "announce-port", 1)) {
addReplyBulkCString(c, "announce-port");
addReplyBulkLongLong(c, sentinel.announce_port);
matches++;
}
if (stringmatch(pattern, "sentinel-user", 1)) {
addReplyBulkCString(c, "sentinel-user");
addReplyBulkCString(c, sentinel.sentinel_auth_user ? sentinel.sentinel_auth_user : "");
matches++;
}
if (stringmatch(pattern, "sentinel-pass", 1)) {
addReplyBulkCString(c, "sentinel-pass");
addReplyBulkCString(c, sentinel.sentinel_auth_pass ? sentinel.sentinel_auth_pass : "");
matches++;
}
setDeferredMapLen(c, replylen, matches);
}
const char *sentinelFailoverStateStr(int state) {
switch(state) {
case SENTINEL_FAILOVER_STATE_NONE: return "none";
case SENTINEL_FAILOVER_STATE_WAIT_START: return "wait_start";
case SENTINEL_FAILOVER_STATE_SELECT_SLAVE: return "select_slave";
case SENTINEL_FAILOVER_STATE_SEND_SLAVEOF_NOONE: return "send_slaveof_noone";
case SENTINEL_FAILOVER_STATE_WAIT_PROMOTION: return "wait_promotion";
case SENTINEL_FAILOVER_STATE_RECONF_SLAVES: return "reconf_slaves";
case SENTINEL_FAILOVER_STATE_UPDATE_CONFIG: return "update_config";
default: return "unknown";
}
}
void addReplySentinelRedisInstance(client *c, sentinelRedisInstance *ri) {
char *flags = sdsempty();
void *mbl;
int fields = 0;
mbl = addReplyDeferredLen(c);
addReplyBulkCString(c,"name");
addReplyBulkCString(c,ri->name);
fields++;
addReplyBulkCString(c,"ip");
addReplyBulkCString(c,announceSentinelAddr(ri->addr));
fields++;
addReplyBulkCString(c,"port");
addReplyBulkLongLong(c,ri->addr->port);
fields++;
addReplyBulkCString(c,"runid");
addReplyBulkCString(c,ri->runid ? ri->runid : "");
fields++;
addReplyBulkCString(c,"flags");
if (ri->flags & SRI_S_DOWN) flags = sdscat(flags,"s_down,");
if (ri->flags & SRI_O_DOWN) flags = sdscat(flags,"o_down,");
if (ri->flags & SRI_MASTER) flags = sdscat(flags,"master,");
if (ri->flags & SRI_SLAVE) flags = sdscat(flags,"slave,");
if (ri->flags & SRI_SENTINEL) flags = sdscat(flags,"sentinel,");
if (ri->link->disconnected) flags = sdscat(flags,"disconnected,");
if (ri->flags & SRI_MASTER_DOWN) flags = sdscat(flags,"master_down,");
if (ri->flags & SRI_FAILOVER_IN_PROGRESS)
flags = sdscat(flags,"failover_in_progress,");
if (ri->flags & SRI_PROMOTED) flags = sdscat(flags,"promoted,");
if (ri->flags & SRI_RECONF_SENT) flags = sdscat(flags,"reconf_sent,");
if (ri->flags & SRI_RECONF_INPROG) flags = sdscat(flags,"reconf_inprog,");
if (ri->flags & SRI_RECONF_DONE) flags = sdscat(flags,"reconf_done,");
if (ri->flags & SRI_FORCE_FAILOVER) flags = sdscat(flags,"force_failover,");
if (ri->flags & SRI_SCRIPT_KILL_SENT) flags = sdscat(flags,"script_kill_sent,");
if (sdslen(flags) != 0) sdsrange(flags,0,-2);
addReplyBulkCString(c,flags);
sdsfree(flags);
fields++;
addReplyBulkCString(c,"link-pending-commands");
addReplyBulkLongLong(c,ri->link->pending_commands);
fields++;
addReplyBulkCString(c,"link-refcount");
addReplyBulkLongLong(c,ri->link->refcount);
fields++;
if (ri->flags & SRI_FAILOVER_IN_PROGRESS) {
addReplyBulkCString(c,"failover-state");
addReplyBulkCString(c,(char*)sentinelFailoverStateStr(ri->failover_state));
fields++;
}
addReplyBulkCString(c,"last-ping-sent");
addReplyBulkLongLong(c,
ri->link->act_ping_time ? (mstime() - ri->link->act_ping_time) : 0);
fields++;
addReplyBulkCString(c,"last-ok-ping-reply");
addReplyBulkLongLong(c,mstime() - ri->link->last_avail_time);
fields++;
addReplyBulkCString(c,"last-ping-reply");
addReplyBulkLongLong(c,mstime() - ri->link->last_pong_time);
fields++;
if (ri->flags & SRI_S_DOWN) {
addReplyBulkCString(c,"s-down-time");
addReplyBulkLongLong(c,mstime()-ri->s_down_since_time);
fields++;
}
if (ri->flags & SRI_O_DOWN) {
addReplyBulkCString(c,"o-down-time");
addReplyBulkLongLong(c,mstime()-ri->o_down_since_time);
fields++;
}
addReplyBulkCString(c,"down-after-milliseconds");
addReplyBulkLongLong(c,ri->down_after_period);
fields++;
if (ri->flags & (SRI_MASTER|SRI_SLAVE)) {
addReplyBulkCString(c,"info-refresh");
addReplyBulkLongLong(c,
ri->info_refresh ? (mstime() - ri->info_refresh) : 0);
fields++;
addReplyBulkCString(c,"role-reported");
addReplyBulkCString(c, (ri->role_reported == SRI_MASTER) ? "master" :
"slave");
fields++;
addReplyBulkCString(c,"role-reported-time");
addReplyBulkLongLong(c,mstime() - ri->role_reported_time);
fields++;
}
if (ri->flags & SRI_MASTER) {
addReplyBulkCString(c,"config-epoch");
addReplyBulkLongLong(c,ri->config_epoch);
fields++;
addReplyBulkCString(c,"num-slaves");
addReplyBulkLongLong(c,dictSize(ri->slaves));
fields++;
addReplyBulkCString(c,"num-other-sentinels");
addReplyBulkLongLong(c,dictSize(ri->sentinels));
fields++;
addReplyBulkCString(c,"quorum");
addReplyBulkLongLong(c,ri->quorum);
fields++;
addReplyBulkCString(c,"failover-timeout");
addReplyBulkLongLong(c,ri->failover_timeout);
fields++;
addReplyBulkCString(c,"parallel-syncs");
addReplyBulkLongLong(c,ri->parallel_syncs);
fields++;
if (ri->notification_script) {
addReplyBulkCString(c,"notification-script");
addReplyBulkCString(c,ri->notification_script);
fields++;
}
if (ri->client_reconfig_script) {
addReplyBulkCString(c,"client-reconfig-script");
addReplyBulkCString(c,ri->client_reconfig_script);
fields++;
}
}
if (ri->flags & SRI_SLAVE) {
addReplyBulkCString(c,"master-link-down-time");
addReplyBulkLongLong(c,ri->master_link_down_time);
fields++;
addReplyBulkCString(c,"master-link-status");
addReplyBulkCString(c,
(ri->slave_master_link_status == SENTINEL_MASTER_LINK_STATUS_UP) ?
"ok" : "err");
fields++;
addReplyBulkCString(c,"master-host");
addReplyBulkCString(c,
ri->slave_master_host ? ri->slave_master_host : "?");
fields++;
addReplyBulkCString(c,"master-port");
addReplyBulkLongLong(c,ri->slave_master_port);
fields++;
addReplyBulkCString(c,"slave-priority");
addReplyBulkLongLong(c,ri->slave_priority);
fields++;
addReplyBulkCString(c,"slave-repl-offset");
addReplyBulkLongLong(c,ri->slave_repl_offset);
fields++;
addReplyBulkCString(c,"replica-announced");
addReplyBulkLongLong(c,ri->replica_announced);
fields++;
}
if (ri->flags & SRI_SENTINEL) {
addReplyBulkCString(c,"last-hello-message");
addReplyBulkLongLong(c,mstime() - ri->last_hello_time);
fields++;
addReplyBulkCString(c,"voted-leader");
addReplyBulkCString(c,ri->leader ? ri->leader : "?");
fields++;
addReplyBulkCString(c,"voted-leader-epoch");
addReplyBulkLongLong(c,ri->leader_epoch);
fields++;
}
setDeferredMapLen(c,mbl,fields);
}
void sentinelSetDebugConfigParameters(client *c){
int j;
int badarg = 0;
char *option;
for (j = 2; j < c->argc; j++) {
int moreargs = (c->argc-1) - j;
option = c->argv[j]->ptr;
long long ll;
if (!strcasecmp(option,"info-period") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_info_period = ll;
} else if (!strcasecmp(option,"ping-period") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_ping_period = ll;
} else if (!strcasecmp(option,"ask-period") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_ask_period = ll;
} else if (!strcasecmp(option,"publish-period") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_publish_period = ll;
}else if (!strcasecmp(option,"default-down-after") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_default_down_after = ll;
} else if (!strcasecmp(option,"tilt-trigger") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_tilt_trigger = ll;
} else if (!strcasecmp(option,"tilt-period") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_tilt_period = ll;
} else if (!strcasecmp(option,"slave-reconf-timeout") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_slave_reconf_timeout = ll;
} else if (!strcasecmp(option,"min-link-reconnect-period") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_min_link_reconnect_period = ll;
} else if (!strcasecmp(option,"default-failover-timeout") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_default_failover_timeout = ll;
} else if (!strcasecmp(option,"election-timeout") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_election_timeout = ll;
} else if (!strcasecmp(option,"script-max-runtime") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_script_max_runtime = ll;
} else if (!strcasecmp(option,"script-retry-delay") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
sentinel_script_retry_delay = ll;
} else {
addReplyErrorFormat(c,"Unknown option or number of arguments for "
"SENTINEL SET '%s'", option);
}
}
addReply(c,shared.ok);
return;
badfmt:
addReplyErrorFormat(c,"Invalid argument '%s' for SENTINEL SET '%s'",
(char*)c->argv[badarg]->ptr,option);
return;
}
void addReplySentinelDebugInfo(client *c) {
void *mbl;
int fields = 0;
mbl = addReplyDeferredLen(c);
addReplyBulkCString(c,"INFO-PERIOD");
addReplyBulkLongLong(c,sentinel_info_period);
fields++;
addReplyBulkCString(c,"PING-PERIOD");
addReplyBulkLongLong(c,sentinel_ping_period);
fields++;
addReplyBulkCString(c,"ASK-PERIOD");
addReplyBulkLongLong(c,sentinel_ask_period);
fields++;
addReplyBulkCString(c,"PUBLISH-PERIOD");
addReplyBulkLongLong(c,sentinel_publish_period);
fields++;
addReplyBulkCString(c,"DEFAULT-DOWN-AFTER");
addReplyBulkLongLong(c,sentinel_default_down_after);
fields++;
addReplyBulkCString(c,"DEFAULT-FAILOVER-TIMEOUT");
addReplyBulkLongLong(c,sentinel_default_failover_timeout);
fields++;
addReplyBulkCString(c,"TILT-TRIGGER");
addReplyBulkLongLong(c,sentinel_tilt_trigger);
fields++;
addReplyBulkCString(c,"TILT-PERIOD");
addReplyBulkLongLong(c,sentinel_tilt_period);
fields++;
addReplyBulkCString(c,"SLAVE-RECONF-TIMEOUT");
addReplyBulkLongLong(c,sentinel_slave_reconf_timeout);
fields++;
addReplyBulkCString(c,"MIN-LINK-RECONNECT-PERIOD");
addReplyBulkLongLong(c,sentinel_min_link_reconnect_period);
fields++;
addReplyBulkCString(c,"ELECTION-TIMEOUT");
addReplyBulkLongLong(c,sentinel_election_timeout);
fields++;
addReplyBulkCString(c,"SCRIPT-MAX-RUNTIME");
addReplyBulkLongLong(c,sentinel_script_max_runtime);
fields++;
addReplyBulkCString(c,"SCRIPT-RETRY-DELAY");
addReplyBulkLongLong(c,sentinel_script_retry_delay);
fields++;
setDeferredMapLen(c,mbl,fields);
}
void addReplyDictOfRedisInstances(client *c, dict *instances) {
dictIterator *di;
dictEntry *de;
long slaves = 0;
void *replylen = addReplyDeferredLen(c);
di = dictGetIterator(instances);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
if (ri->flags & SRI_SLAVE && !ri->replica_announced) continue;
addReplySentinelRedisInstance(c,ri);
slaves++;
}
dictReleaseIterator(di);
setDeferredArrayLen(c, replylen, slaves);
}
sentinelRedisInstance *sentinelGetMasterByNameOrReplyError(client *c,
robj *name)
{
sentinelRedisInstance *ri;
ri = dictFetchValue(sentinel.masters,name->ptr);
if (!ri) {
addReplyError(c,"No such master with that name");
return NULL;
}
return ri;
}
#define SENTINEL_ISQR_OK 0
#define SENTINEL_ISQR_NOQUORUM (1<<0)
#define SENTINEL_ISQR_NOAUTH (1<<1)
int sentinelIsQuorumReachable(sentinelRedisInstance *master, int *usableptr) {
dictIterator *di;
dictEntry *de;
int usable = 1;
int result = SENTINEL_ISQR_OK;
int voters = dictSize(master->sentinels)+1;
di = dictGetIterator(master->sentinels);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
if (ri->flags & (SRI_S_DOWN|SRI_O_DOWN)) continue;
usable++;
}
dictReleaseIterator(di);
if (usable < (int)master->quorum) result |= SENTINEL_ISQR_NOQUORUM;
if (usable < voters/2+1) result |= SENTINEL_ISQR_NOAUTH;
if (usableptr) *usableptr = usable;
return result;
}
void sentinelCommand(client *c) {
if (c->argc == 2 && !strcasecmp(c->argv[1]->ptr,"help")) {
const char *help[] = {
"CKQUORUM <master-name>",
" Check if the current Sentinel configuration is able to reach the quorum",
" needed to failover a master and the majority needed to authorize the",
" failover.",
"CONFIG SET <param> <value>",
" Set a global Sentinel configuration parameter.",
"CONFIG GET <param>",
" Get global Sentinel configuration parameter.",
"DEBUG",
" Show a list of configurable time parameters and their values (milliseconds).",
"GET-MASTER-ADDR-BY-NAME <master-name>",
" Return the ip and port number of the master with that name.",
"FAILOVER <master-name>",
" Manually failover a master node without asking for agreement from other",
" Sentinels",
"FLUSHCONFIG",
" Force Sentinel to rewrite its configuration on disk, including the current",
" Sentinel state.",
"INFO-CACHE <master-name>",
" Return last cached INFO output from masters and all its replicas.",
"IS-MASTER-DOWN-BY-ADDR <ip> <port> <current-epoch> <runid>",
" Check if the master specified by ip:port is down from current Sentinel's",
" point of view.",
"MASTER <master-name>",
" Show the state and info of the specified master.",
"MASTERS",
" Show a list of monitored masters and their state.",
"MONITOR <name> <ip> <port> <quorum>",
" Start monitoring a new master with the specified name, ip, port and quorum.",
"MYID",
" Return the ID of the Sentinel instance.",
"PENDING-SCRIPTS",
" Get pending scripts information.",
"REMOVE <master-name>",
" Remove master from Sentinel's monitor list.",
"REPLICAS <master-name>",
" Show a list of replicas for this master and their state.",
"RESET <pattern>",
" Reset masters for specific master name matching this pattern.",
"SENTINELS <master-name>",
" Show a list of Sentinel instances for this master and their state.",
"SET <master-name> <option> <value> [<option> <value> ...]",
" Set configuration parameters for certain masters.",
"SIMULATE-FAILURE [CRASH-AFTER-ELECTION] [CRASH-AFTER-PROMOTION] [HELP]",
" Simulate a Sentinel crash.",
NULL
};
addReplyHelp(c, help);
} else if (!strcasecmp(c->argv[1]->ptr,"masters")) {
if (c->argc != 2) goto numargserr;
addReplyDictOfRedisInstances(c,sentinel.masters);
} else if (!strcasecmp(c->argv[1]->ptr,"master")) {
sentinelRedisInstance *ri;
if (c->argc != 3) goto numargserr;
if ((ri = sentinelGetMasterByNameOrReplyError(c,c->argv[2]))
== NULL) return;
addReplySentinelRedisInstance(c,ri);
} else if (!strcasecmp(c->argv[1]->ptr,"slaves") ||
!strcasecmp(c->argv[1]->ptr,"replicas"))
{
sentinelRedisInstance *ri;
if (c->argc != 3) goto numargserr;
if ((ri = sentinelGetMasterByNameOrReplyError(c,c->argv[2])) == NULL)
return;
addReplyDictOfRedisInstances(c,ri->slaves);
} else if (!strcasecmp(c->argv[1]->ptr,"sentinels")) {
sentinelRedisInstance *ri;
if (c->argc != 3) goto numargserr;
if ((ri = sentinelGetMasterByNameOrReplyError(c,c->argv[2])) == NULL)
return;
addReplyDictOfRedisInstances(c,ri->sentinels);
} else if (!strcasecmp(c->argv[1]->ptr,"myid") && c->argc == 2) {
addReplyBulkCBuffer(c,sentinel.myid,CONFIG_RUN_ID_SIZE);
} else if (!strcasecmp(c->argv[1]->ptr,"is-master-down-by-addr")) {
sentinelRedisInstance *ri;
long long req_epoch;
uint64_t leader_epoch = 0;
char *leader = NULL;
long port;
int isdown = 0;
if (c->argc != 6) goto numargserr;
if (getLongFromObjectOrReply(c,c->argv[3],&port,NULL) != C_OK ||
getLongLongFromObjectOrReply(c,c->argv[4],&req_epoch,NULL)
!= C_OK)
return;
ri = getSentinelRedisInstanceByAddrAndRunID(sentinel.masters,
c->argv[2]->ptr,port,NULL);
if (!sentinel.tilt && ri && (ri->flags & SRI_S_DOWN) &&
(ri->flags & SRI_MASTER))
isdown = 1;
if (ri && ri->flags & SRI_MASTER && strcasecmp(c->argv[5]->ptr,"*")) {
leader = sentinelVoteLeader(ri,(uint64_t)req_epoch,
c->argv[5]->ptr,
&leader_epoch);
}
addReplyArrayLen(c,3);
addReply(c, isdown ? shared.cone : shared.czero);
addReplyBulkCString(c, leader ? leader : "*");
addReplyLongLong(c, (long long)leader_epoch);
if (leader) sdsfree(leader);
} else if (!strcasecmp(c->argv[1]->ptr,"reset")) {
if (c->argc != 3) goto numargserr;
addReplyLongLong(c,sentinelResetMastersByPattern(c->argv[2]->ptr,SENTINEL_GENERATE_EVENT));
} else if (!strcasecmp(c->argv[1]->ptr,"get-master-addr-by-name")) {
sentinelRedisInstance *ri;
if (c->argc != 3) goto numargserr;
ri = sentinelGetMasterByName(c->argv[2]->ptr);
if (ri == NULL) {
addReplyNullArray(c);
} else {
sentinelAddr *addr = sentinelGetCurrentMasterAddress(ri);
addReplyArrayLen(c,2);
addReplyBulkCString(c,announceSentinelAddr(addr));
addReplyBulkLongLong(c,addr->port);
}
} else if (!strcasecmp(c->argv[1]->ptr,"failover")) {
sentinelRedisInstance *ri;
if (c->argc != 3) goto numargserr;
if ((ri = sentinelGetMasterByNameOrReplyError(c,c->argv[2])) == NULL)
return;
if (ri->flags & SRI_FAILOVER_IN_PROGRESS) {
addReplyError(c,"-INPROG Failover already in progress");
return;
}
if (sentinelSelectSlave(ri) == NULL) {
addReplyError(c,"-NOGOODSLAVE No suitable replica to promote");
return;
}
serverLog(LL_WARNING,"Executing user requested FAILOVER of '%s'",
ri->name);
sentinelStartFailover(ri);
ri->flags |= SRI_FORCE_FAILOVER;
addReply(c,shared.ok);
} else if (!strcasecmp(c->argv[1]->ptr,"pending-scripts")) {
if (c->argc != 2) goto numargserr;
sentinelPendingScriptsCommand(c);
} else if (!strcasecmp(c->argv[1]->ptr,"monitor")) {
sentinelRedisInstance *ri;
long quorum, port;
char ip[NET_IP_STR_LEN];
if (c->argc != 6) goto numargserr;
if (getLongFromObjectOrReply(c,c->argv[5],&quorum,"Invalid quorum")
!= C_OK) return;
if (getLongFromObjectOrReply(c,c->argv[4],&port,"Invalid port")
!= C_OK) return;
if (quorum <= 0) {
addReplyError(c, "Quorum must be 1 or greater.");
return;
}
if (anetResolve(NULL,c->argv[3]->ptr,ip,sizeof(ip),
sentinel.resolve_hostnames ? ANET_NONE : ANET_IP_ONLY) == ANET_ERR) {
addReplyError(c, "Invalid IP address or hostname specified");
return;
}
ri = createSentinelRedisInstance(c->argv[2]->ptr,SRI_MASTER,
c->argv[3]->ptr,port,quorum,NULL);
if (ri == NULL) {
addReplyError(c,sentinelCheckCreateInstanceErrors(SRI_MASTER));
} else {
sentinelFlushConfig();
sentinelEvent(LL_WARNING,"+monitor",ri,"%@ quorum %d",ri->quorum);
addReply(c,shared.ok);
}
} else if (!strcasecmp(c->argv[1]->ptr,"flushconfig")) {
if (c->argc != 2) goto numargserr;
sentinelFlushConfig();
addReply(c,shared.ok);
return;
} else if (!strcasecmp(c->argv[1]->ptr,"remove")) {
sentinelRedisInstance *ri;
if (c->argc != 3) goto numargserr;
if ((ri = sentinelGetMasterByNameOrReplyError(c,c->argv[2]))
== NULL) return;
sentinelEvent(LL_WARNING,"-monitor",ri,"%@");
dictDelete(sentinel.masters,c->argv[2]->ptr);
sentinelFlushConfig();
addReply(c,shared.ok);
} else if (!strcasecmp(c->argv[1]->ptr,"ckquorum")) {
sentinelRedisInstance *ri;
int usable;
if (c->argc != 3) goto numargserr;
if ((ri = sentinelGetMasterByNameOrReplyError(c,c->argv[2]))
== NULL) return;
int result = sentinelIsQuorumReachable(ri,&usable);
if (result == SENTINEL_ISQR_OK) {
addReplySds(c, sdscatfmt(sdsempty(),
"+OK %i usable Sentinels. Quorum and failover authorization "
"can be reached\r\n",usable));
} else {
sds e = sdscatfmt(sdsempty(),
"-NOQUORUM %i usable Sentinels. ",usable);
if (result & SENTINEL_ISQR_NOQUORUM)
e = sdscat(e,"Not enough available Sentinels to reach the"
" specified quorum for this master");
if (result & SENTINEL_ISQR_NOAUTH) {
if (result & SENTINEL_ISQR_NOQUORUM) e = sdscat(e,". ");
e = sdscat(e, "Not enough available Sentinels to reach the"
" majority and authorize a failover");
}
addReplyErrorSds(c,e);
}
} else if (!strcasecmp(c->argv[1]->ptr,"set")) {
sentinelSetCommand(c);
} else if (!strcasecmp(c->argv[1]->ptr,"config")) {
if (c->argc < 3) goto numargserr;
if (!strcasecmp(c->argv[2]->ptr,"set") && c->argc == 5)
sentinelConfigSetCommand(c);
else if (!strcasecmp(c->argv[2]->ptr,"get") && c->argc == 4)
sentinelConfigGetCommand(c);
else
addReplyError(c, "Only SENTINEL CONFIG GET <option> / SET <option> <value> are supported.");
} else if (!strcasecmp(c->argv[1]->ptr,"info-cache")) {
if (c->argc < 2) goto numargserr;
mstime_t now = mstime();
dictType copy_keeper = instancesDictType;
copy_keeper.valDestructor = NULL;
dict *masters_local = sentinel.masters;
if (c->argc > 2) {
masters_local = dictCreate(&copy_keeper);
for (int i = 2; i < c->argc; i++) {
sentinelRedisInstance *ri;
ri = sentinelGetMasterByName(c->argv[i]->ptr);
if (!ri) continue;
dictAdd(masters_local, ri->name, ri);
}
}
addReplyArrayLen(c,dictSize(masters_local) * 2);
dictIterator *di;
dictEntry *de;
di = dictGetIterator(masters_local);
while ((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
addReplyBulkCBuffer(c,ri->name,strlen(ri->name));
addReplyArrayLen(c,dictSize(ri->slaves) + 1);
addReplyArrayLen(c,2);
addReplyLongLong(c,
ri->info_refresh ? (now - ri->info_refresh) : 0);
if (ri->info)
addReplyBulkCBuffer(c,ri->info,sdslen(ri->info));
else
addReplyNull(c);
dictIterator *sdi;
dictEntry *sde;
sdi = dictGetIterator(ri->slaves);
while ((sde = dictNext(sdi)) != NULL) {
sentinelRedisInstance *sri = dictGetVal(sde);
addReplyArrayLen(c,2);
addReplyLongLong(c,
ri->info_refresh ? (now - sri->info_refresh) : 0);
if (sri->info)
addReplyBulkCBuffer(c,sri->info,sdslen(sri->info));
else
addReplyNull(c);
}
dictReleaseIterator(sdi);
}
dictReleaseIterator(di);
if (masters_local != sentinel.masters) dictRelease(masters_local);
} else if (!strcasecmp(c->argv[1]->ptr,"simulate-failure")) {
int j;
sentinel.simfailure_flags = SENTINEL_SIMFAILURE_NONE;
for (j = 2; j < c->argc; j++) {
if (!strcasecmp(c->argv[j]->ptr,"crash-after-election")) {
sentinel.simfailure_flags |=
SENTINEL_SIMFAILURE_CRASH_AFTER_ELECTION;
serverLog(LL_WARNING,"Failure simulation: this Sentinel "
"will crash after being successfully elected as failover "
"leader");
} else if (!strcasecmp(c->argv[j]->ptr,"crash-after-promotion")) {
sentinel.simfailure_flags |=
SENTINEL_SIMFAILURE_CRASH_AFTER_PROMOTION;
serverLog(LL_WARNING,"Failure simulation: this Sentinel "
"will crash after promoting the selected replica to master");
} else if (!strcasecmp(c->argv[j]->ptr,"help")) {
addReplyArrayLen(c,2);
addReplyBulkCString(c,"crash-after-election");
addReplyBulkCString(c,"crash-after-promotion");
return;
} else {
addReplyError(c,"Unknown failure simulation specified");
return;
}
}
addReply(c,shared.ok);
} else if (!strcasecmp(c->argv[1]->ptr,"debug")) {
if(c->argc == 2)
addReplySentinelDebugInfo(c);
else
sentinelSetDebugConfigParameters(c);
}
else {
addReplySubcommandSyntaxError(c);
}
return;
numargserr:
addReplyErrorArity(c);
}
#define info_section_from_redis(section_name) do { if (defsections || allsections || !strcasecmp(section,section_name)) { sds redissection; if (sections++) info = sdscat(info,"\r\n"); redissection = genRedisInfoString(section_name); info = sdscatlen(info,redissection,sdslen(redissection)); sdsfree(redissection); } } while(0)
void sentinelInfoCommand(client *c) {
if (c->argc > 2) {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
int defsections = 0, allsections = 0;
char *section = c->argc == 2 ? c->argv[1]->ptr : NULL;
if (section) {
allsections = !strcasecmp(section,"all");
defsections = !strcasecmp(section,"default");
} else {
defsections = 1;
}
int sections = 0;
sds info = sdsempty();
info_section_from_redis("server");
info_section_from_redis("clients");
info_section_from_redis("cpu");
info_section_from_redis("stats");
if (defsections || allsections || !strcasecmp(section,"sentinel")) {
dictIterator *di;
dictEntry *de;
int master_id = 0;
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info,
"#Sentinel\r\n"
"sentinel_masters:%lu\r\n"
"sentinel_tilt:%d\r\n"
"sentinel_tilt_since_seconds:%jd\r\n"
"sentinel_running_scripts:%d\r\n"
"sentinel_scripts_queue_length:%ld\r\n"
"sentinel_simulate_failure_flags:%lu\r\n",
dictSize(sentinel.masters),
sentinel.tilt,
sentinel.tilt ? (intmax_t)((mstime()-sentinel.tilt_start_time)/1000) : -1,
sentinel.running_scripts,
listLength(sentinel.scripts_queue),
sentinel.simfailure_flags);
di = dictGetIterator(sentinel.masters);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
char *status = "ok";
if (ri->flags & SRI_O_DOWN) status = "odown";
else if (ri->flags & SRI_S_DOWN) status = "sdown";
info = sdscatprintf(info,
"master%d:name=%s,status=%s,address=%s:%d,"
"slaves=%lu,sentinels=%lu\r\n",
master_id++, ri->name, status,
announceSentinelAddr(ri->addr), ri->addr->port,
dictSize(ri->slaves),
dictSize(ri->sentinels)+1);
}
dictReleaseIterator(di);
}
addReplyBulkSds(c, info);
}
void sentinelRoleCommand(client *c) {
dictIterator *di;
dictEntry *de;
addReplyArrayLen(c,2);
addReplyBulkCBuffer(c,"sentinel",8);
addReplyArrayLen(c,dictSize(sentinel.masters));
di = dictGetIterator(sentinel.masters);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
addReplyBulkCString(c,ri->name);
}
dictReleaseIterator(di);
}
void sentinelSetCommand(client *c) {
sentinelRedisInstance *ri;
int j, changes = 0;
int badarg = 0;
char *option;
int redacted;
if ((ri = sentinelGetMasterByNameOrReplyError(c,c->argv[2]))
== NULL) return;
for (j = 3; j < c->argc; j++) {
int moreargs = (c->argc-1) - j;
option = c->argv[j]->ptr;
long long ll;
int old_j = j;
redacted = 0;
if (!strcasecmp(option,"down-after-milliseconds") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
ri->down_after_period = ll;
sentinelPropagateDownAfterPeriod(ri);
changes++;
} else if (!strcasecmp(option,"failover-timeout") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
ri->failover_timeout = ll;
changes++;
} else if (!strcasecmp(option,"parallel-syncs") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
ri->parallel_syncs = ll;
changes++;
} else if (!strcasecmp(option,"notification-script") && moreargs > 0) {
char *value = c->argv[++j]->ptr;
if (sentinel.deny_scripts_reconfig) {
addReplyError(c,
"Reconfiguration of scripts path is denied for "
"security reasons. Check the deny-scripts-reconfig "
"configuration directive in your Sentinel configuration");
goto seterr;
}
if (strlen(value) && access(value,X_OK) == -1) {
addReplyError(c,
"Notification script seems non existing or non executable");
goto seterr;
}
sdsfree(ri->notification_script);
ri->notification_script = strlen(value) ? sdsnew(value) : NULL;
changes++;
} else if (!strcasecmp(option,"client-reconfig-script") && moreargs > 0) {
char *value = c->argv[++j]->ptr;
if (sentinel.deny_scripts_reconfig) {
addReplyError(c,
"Reconfiguration of scripts path is denied for "
"security reasons. Check the deny-scripts-reconfig "
"configuration directive in your Sentinel configuration");
goto seterr;
}
if (strlen(value) && access(value,X_OK) == -1) {
addReplyError(c,
"Client reconfiguration script seems non existing or "
"non executable");
goto seterr;
}
sdsfree(ri->client_reconfig_script);
ri->client_reconfig_script = strlen(value) ? sdsnew(value) : NULL;
changes++;
} else if (!strcasecmp(option,"auth-pass") && moreargs > 0) {
char *value = c->argv[++j]->ptr;
sdsfree(ri->auth_pass);
ri->auth_pass = strlen(value) ? sdsnew(value) : NULL;
changes++;
redacted = 1;
} else if (!strcasecmp(option,"auth-user") && moreargs > 0) {
char *value = c->argv[++j]->ptr;
sdsfree(ri->auth_user);
ri->auth_user = strlen(value) ? sdsnew(value) : NULL;
changes++;
} else if (!strcasecmp(option,"quorum") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll <= 0) {
badarg = j;
goto badfmt;
}
ri->quorum = ll;
changes++;
} else if (!strcasecmp(option,"rename-command") && moreargs > 1) {
sds oldname = c->argv[++j]->ptr;
sds newname = c->argv[++j]->ptr;
if ((sdslen(oldname) == 0) || (sdslen(newname) == 0)) {
badarg = sdslen(newname) ? j-1 : j;
goto badfmt;
}
dictDelete(ri->renamed_commands,oldname);
if (!dictSdsKeyCaseCompare(ri->renamed_commands,oldname,newname)) {
oldname = sdsdup(oldname);
newname = sdsdup(newname);
dictAdd(ri->renamed_commands,oldname,newname);
}
changes++;
} else if (!strcasecmp(option,"master-reboot-down-after-period") && moreargs > 0) {
robj *o = c->argv[++j];
if (getLongLongFromObject(o,&ll) == C_ERR || ll < 0) {
badarg = j;
goto badfmt;
}
ri->master_reboot_down_after_period = ll;
changes++;
} else {
addReplyErrorFormat(c,"Unknown option or number of arguments for "
"SENTINEL SET '%s'", option);
goto seterr;
}
int numargs = j-old_j+1;
switch(numargs) {
case 2:
sentinelEvent(LL_WARNING,"+set",ri,"%@ %s %s",(char*)c->argv[old_j]->ptr,
redacted ? "******" : (char*)c->argv[old_j+1]->ptr);
break;
case 3:
sentinelEvent(LL_WARNING,"+set",ri,"%@ %s %s %s",(char*)c->argv[old_j]->ptr,
(char*)c->argv[old_j+1]->ptr,
(char*)c->argv[old_j+2]->ptr);
break;
default:
sentinelEvent(LL_WARNING,"+set",ri,"%@ %s",(char*)c->argv[old_j]->ptr);
break;
}
}
if (changes && sentinelFlushConfig() == C_ERR) {
addReplyErrorFormat(c,"Failed to save Sentinel new configuration on disk");
return;
}
addReply(c,shared.ok);
return;
badfmt:
addReplyErrorFormat(c,"Invalid argument '%s' for SENTINEL SET '%s'",
(char*)c->argv[badarg]->ptr,option);
seterr:
if (changes && sentinelFlushConfig() == C_ERR) {
addReplyErrorFormat(c,"Failed to save Sentinel new configuration on disk");
return;
}
return;
}
void sentinelPublishCommand(client *c) {
if (strcmp(c->argv[1]->ptr,SENTINEL_HELLO_CHANNEL)) {
addReplyError(c, "Only HELLO messages are accepted by Sentinel instances.");
return;
}
sentinelProcessHelloMessage(c->argv[2]->ptr,sdslen(c->argv[2]->ptr));
addReplyLongLong(c,1);
}
void sentinelCheckSubjectivelyDown(sentinelRedisInstance *ri) {
mstime_t elapsed = 0;
if (ri->link->act_ping_time)
elapsed = mstime() - ri->link->act_ping_time;
else if (ri->link->disconnected)
elapsed = mstime() - ri->link->last_avail_time;
if (ri->link->cc &&
(mstime() - ri->link->cc_conn_time) >
sentinel_min_link_reconnect_period &&
ri->link->act_ping_time != 0 &&
(mstime() - ri->link->act_ping_time) > (ri->down_after_period/2) &&
(mstime() - ri->link->last_pong_time) > (ri->down_after_period/2))
{
instanceLinkCloseConnection(ri->link,ri->link->cc);
}
if (ri->link->pc &&
(mstime() - ri->link->pc_conn_time) >
sentinel_min_link_reconnect_period &&
(mstime() - ri->link->pc_last_activity) > (sentinel_publish_period*3))
{
instanceLinkCloseConnection(ri->link,ri->link->pc);
}
if (elapsed > ri->down_after_period ||
(ri->flags & SRI_MASTER &&
ri->role_reported == SRI_SLAVE &&
mstime() - ri->role_reported_time >
(ri->down_after_period+sentinel_info_period*2)) ||
(ri->flags & SRI_MASTER_REBOOT &&
mstime()-ri->master_reboot_since_time > ri->master_reboot_down_after_period))
{
if ((ri->flags & SRI_S_DOWN) == 0) {
sentinelEvent(LL_WARNING,"+sdown",ri,"%@");
ri->s_down_since_time = mstime();
ri->flags |= SRI_S_DOWN;
}
} else {
if (ri->flags & SRI_S_DOWN) {
sentinelEvent(LL_WARNING,"-sdown",ri,"%@");
ri->flags &= ~(SRI_S_DOWN|SRI_SCRIPT_KILL_SENT);
}
}
}
void sentinelCheckObjectivelyDown(sentinelRedisInstance *master) {
dictIterator *di;
dictEntry *de;
unsigned int quorum = 0, odown = 0;
if (master->flags & SRI_S_DOWN) {
quorum = 1;
di = dictGetIterator(master->sentinels);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
if (ri->flags & SRI_MASTER_DOWN) quorum++;
}
dictReleaseIterator(di);
if (quorum >= master->quorum) odown = 1;
}
if (odown) {
if ((master->flags & SRI_O_DOWN) == 0) {
sentinelEvent(LL_WARNING,"+odown",master,"%@ #quorum %d/%d",
quorum, master->quorum);
master->flags |= SRI_O_DOWN;
master->o_down_since_time = mstime();
}
} else {
if (master->flags & SRI_O_DOWN) {
sentinelEvent(LL_WARNING,"-odown",master,"%@");
master->flags &= ~SRI_O_DOWN;
}
}
}
void sentinelReceiveIsMasterDownReply(redisAsyncContext *c, void *reply, void *privdata) {
sentinelRedisInstance *ri = privdata;
instanceLink *link = c->data;
redisReply *r;
if (!reply || !link) return;
link->pending_commands--;
r = reply;
if (r->type == REDIS_REPLY_ARRAY && r->elements == 3 &&
r->element[0]->type == REDIS_REPLY_INTEGER &&
r->element[1]->type == REDIS_REPLY_STRING &&
r->element[2]->type == REDIS_REPLY_INTEGER)
{
ri->last_master_down_reply_time = mstime();
if (r->element[0]->integer == 1) {
ri->flags |= SRI_MASTER_DOWN;
} else {
ri->flags &= ~SRI_MASTER_DOWN;
}
if (strcmp(r->element[1]->str,"*")) {
sdsfree(ri->leader);
if ((long long)ri->leader_epoch != r->element[2]->integer)
serverLog(LL_WARNING,
"%s voted for %s %llu", ri->name,
r->element[1]->str,
(unsigned long long) r->element[2]->integer);
ri->leader = sdsnew(r->element[1]->str);
ri->leader_epoch = r->element[2]->integer;
}
}
}
#define SENTINEL_ASK_FORCED (1<<0)
void sentinelAskMasterStateToOtherSentinels(sentinelRedisInstance *master, int flags) {
dictIterator *di;
dictEntry *de;
di = dictGetIterator(master->sentinels);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
mstime_t elapsed = mstime() - ri->last_master_down_reply_time;
char port[32];
int retval;
if (elapsed > sentinel_ask_period*5) {
ri->flags &= ~SRI_MASTER_DOWN;
sdsfree(ri->leader);
ri->leader = NULL;
}
if ((master->flags & SRI_S_DOWN) == 0) continue;
if (ri->link->disconnected) continue;
if (!(flags & SENTINEL_ASK_FORCED) &&
mstime() - ri->last_master_down_reply_time < sentinel_ask_period)
continue;
ll2string(port,sizeof(port),master->addr->port);
retval = redisAsyncCommand(ri->link->cc,
sentinelReceiveIsMasterDownReply, ri,
"%s is-master-down-by-addr %s %s %llu %s",
sentinelInstanceMapCommand(ri,"SENTINEL"),
announceSentinelAddr(master->addr), port,
sentinel.current_epoch,
(master->failover_state > SENTINEL_FAILOVER_STATE_NONE) ?
sentinel.myid : "*");
if (retval == C_OK) ri->link->pending_commands++;
}
dictReleaseIterator(di);
}
void sentinelSimFailureCrash(void) {
serverLog(LL_WARNING,
"Sentinel CRASH because of SENTINEL simulate-failure");
exit(99);
}
char *sentinelVoteLeader(sentinelRedisInstance *master, uint64_t req_epoch, char *req_runid, uint64_t *leader_epoch) {
if (req_epoch > sentinel.current_epoch) {
sentinel.current_epoch = req_epoch;
sentinelFlushConfig();
sentinelEvent(LL_WARNING,"+new-epoch",master,"%llu",
(unsigned long long) sentinel.current_epoch);
}
if (master->leader_epoch < req_epoch && sentinel.current_epoch <= req_epoch)
{
sdsfree(master->leader);
master->leader = sdsnew(req_runid);
master->leader_epoch = sentinel.current_epoch;
sentinelFlushConfig();
sentinelEvent(LL_WARNING,"+vote-for-leader",master,"%s %llu",
master->leader, (unsigned long long) master->leader_epoch);
if (strcasecmp(master->leader,sentinel.myid))
master->failover_start_time = mstime()+rand()%SENTINEL_MAX_DESYNC;
}
*leader_epoch = master->leader_epoch;
return master->leader ? sdsnew(master->leader) : NULL;
}
struct sentinelLeader {
char *runid;
unsigned long votes;
};
int sentinelLeaderIncr(dict *counters, char *runid) {
dictEntry *existing, *de;
uint64_t oldval;
de = dictAddRaw(counters,runid,&existing);
if (existing) {
oldval = dictGetUnsignedIntegerVal(existing);
dictSetUnsignedIntegerVal(existing,oldval+1);
return oldval+1;
} else {
serverAssert(de != NULL);
dictSetUnsignedIntegerVal(de,1);
return 1;
}
}
char *sentinelGetLeader(sentinelRedisInstance *master, uint64_t epoch) {
dict *counters;
dictIterator *di;
dictEntry *de;
unsigned int voters = 0, voters_quorum;
char *myvote;
char *winner = NULL;
uint64_t leader_epoch;
uint64_t max_votes = 0;
serverAssert(master->flags & (SRI_O_DOWN|SRI_FAILOVER_IN_PROGRESS));
counters = dictCreate(&leaderVotesDictType);
voters = dictSize(master->sentinels)+1;
di = dictGetIterator(master->sentinels);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
if (ri->leader != NULL && ri->leader_epoch == sentinel.current_epoch)
sentinelLeaderIncr(counters,ri->leader);
}
dictReleaseIterator(di);
di = dictGetIterator(counters);
while((de = dictNext(di)) != NULL) {
uint64_t votes = dictGetUnsignedIntegerVal(de);
if (votes > max_votes) {
max_votes = votes;
winner = dictGetKey(de);
}
}
dictReleaseIterator(di);
if (winner)
myvote = sentinelVoteLeader(master,epoch,winner,&leader_epoch);
else
myvote = sentinelVoteLeader(master,epoch,sentinel.myid,&leader_epoch);
if (myvote && leader_epoch == epoch) {
uint64_t votes = sentinelLeaderIncr(counters,myvote);
if (votes > max_votes) {
max_votes = votes;
winner = myvote;
}
}
voters_quorum = voters/2+1;
if (winner && (max_votes < voters_quorum || max_votes < master->quorum))
winner = NULL;
winner = winner ? sdsnew(winner) : NULL;
sdsfree(myvote);
dictRelease(counters);
return winner;
}
int sentinelSendSlaveOf(sentinelRedisInstance *ri, const sentinelAddr *addr) {
char portstr[32];
const char *host;
int retval;
if (!addr) {
host = "NO";
memcpy(portstr,"ONE",4);
} else {
host = announceSentinelAddr(addr);
ll2string(portstr,sizeof(portstr),addr->port);
}
retval = redisAsyncCommand(ri->link->cc,
sentinelDiscardReplyCallback, ri, "%s",
sentinelInstanceMapCommand(ri,"MULTI"));
if (retval == C_ERR) return retval;
ri->link->pending_commands++;
retval = redisAsyncCommand(ri->link->cc,
sentinelDiscardReplyCallback, ri, "%s %s %s",
sentinelInstanceMapCommand(ri,"SLAVEOF"),
host, portstr);
if (retval == C_ERR) return retval;
ri->link->pending_commands++;
retval = redisAsyncCommand(ri->link->cc,
sentinelDiscardReplyCallback, ri, "%s REWRITE",
sentinelInstanceMapCommand(ri,"CONFIG"));
if (retval == C_ERR) return retval;
ri->link->pending_commands++;
for (int type = 0; type < 2; type++) {
retval = redisAsyncCommand(ri->link->cc,
sentinelDiscardReplyCallback, ri, "%s KILL TYPE %s",
sentinelInstanceMapCommand(ri,"CLIENT"),
type == 0 ? "normal" : "pubsub");
if (retval == C_ERR) return retval;
ri->link->pending_commands++;
}
retval = redisAsyncCommand(ri->link->cc,
sentinelDiscardReplyCallback, ri, "%s",
sentinelInstanceMapCommand(ri,"EXEC"));
if (retval == C_ERR) return retval;
ri->link->pending_commands++;
return C_OK;
}
void sentinelStartFailover(sentinelRedisInstance *master) {
serverAssert(master->flags & SRI_MASTER);
master->failover_state = SENTINEL_FAILOVER_STATE_WAIT_START;
master->flags |= SRI_FAILOVER_IN_PROGRESS;
master->failover_epoch = ++sentinel.current_epoch;
sentinelEvent(LL_WARNING,"+new-epoch",master,"%llu",
(unsigned long long) sentinel.current_epoch);
sentinelEvent(LL_WARNING,"+try-failover",master,"%@");
master->failover_start_time = mstime()+rand()%SENTINEL_MAX_DESYNC;
master->failover_state_change_time = mstime();
}
int sentinelStartFailoverIfNeeded(sentinelRedisInstance *master) {
if (!(master->flags & SRI_O_DOWN)) return 0;
if (master->flags & SRI_FAILOVER_IN_PROGRESS) return 0;
if (mstime() - master->failover_start_time <
master->failover_timeout*2)
{
if (master->failover_delay_logged != master->failover_start_time) {
time_t clock = (master->failover_start_time +
master->failover_timeout*2) / 1000;
char ctimebuf[26];
ctime_r(&clock,ctimebuf);
ctimebuf[24] = '\0';
master->failover_delay_logged = master->failover_start_time;
serverLog(LL_WARNING,
"Next failover delay: I will not start a failover before %s",
ctimebuf);
}
return 0;
}
sentinelStartFailover(master);
return 1;
}
int compareSlavesForPromotion(const void *a, const void *b) {
sentinelRedisInstance **sa = (sentinelRedisInstance **)a,
**sb = (sentinelRedisInstance **)b;
char *sa_runid, *sb_runid;
if ((*sa)->slave_priority != (*sb)->slave_priority)
return (*sa)->slave_priority - (*sb)->slave_priority;
if ((*sa)->slave_repl_offset > (*sb)->slave_repl_offset) {
return -1;
} else if ((*sa)->slave_repl_offset < (*sb)->slave_repl_offset) {
return 1;
}
sa_runid = (*sa)->runid;
sb_runid = (*sb)->runid;
if (sa_runid == NULL && sb_runid == NULL) return 0;
else if (sa_runid == NULL) return 1;
else if (sb_runid == NULL) return -1;
return strcasecmp(sa_runid, sb_runid);
}
sentinelRedisInstance *sentinelSelectSlave(sentinelRedisInstance *master) {
sentinelRedisInstance **instance =
zmalloc(sizeof(instance[0])*dictSize(master->slaves));
sentinelRedisInstance *selected = NULL;
int instances = 0;
dictIterator *di;
dictEntry *de;
mstime_t max_master_down_time = 0;
if (master->flags & SRI_S_DOWN)
max_master_down_time += mstime() - master->s_down_since_time;
max_master_down_time += master->down_after_period * 10;
di = dictGetIterator(master->slaves);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *slave = dictGetVal(de);
mstime_t info_validity_time;
if (slave->flags & (SRI_S_DOWN|SRI_O_DOWN)) continue;
if (slave->link->disconnected) continue;
if (mstime() - slave->link->last_avail_time > sentinel_ping_period*5) continue;
if (slave->slave_priority == 0) continue;
if (master->flags & SRI_S_DOWN)
info_validity_time = sentinel_ping_period*5;
else
info_validity_time = sentinel_info_period*3;
if (mstime() - slave->info_refresh > info_validity_time) continue;
if (slave->master_link_down_time > max_master_down_time) continue;
instance[instances++] = slave;
}
dictReleaseIterator(di);
if (instances) {
qsort(instance,instances,sizeof(sentinelRedisInstance*),
compareSlavesForPromotion);
selected = instance[0];
}
zfree(instance);
return selected;
}
void sentinelFailoverWaitStart(sentinelRedisInstance *ri) {
char *leader;
int isleader;
leader = sentinelGetLeader(ri, ri->failover_epoch);
isleader = leader && strcasecmp(leader,sentinel.myid) == 0;
sdsfree(leader);
if (!isleader && !(ri->flags & SRI_FORCE_FAILOVER)) {
mstime_t election_timeout = sentinel_election_timeout;
if (election_timeout > ri->failover_timeout)
election_timeout = ri->failover_timeout;
if (mstime() - ri->failover_start_time > election_timeout) {
sentinelEvent(LL_WARNING,"-failover-abort-not-elected",ri,"%@");
sentinelAbortFailover(ri);
}
return;
}
sentinelEvent(LL_WARNING,"+elected-leader",ri,"%@");
if (sentinel.simfailure_flags & SENTINEL_SIMFAILURE_CRASH_AFTER_ELECTION)
sentinelSimFailureCrash();
ri->failover_state = SENTINEL_FAILOVER_STATE_SELECT_SLAVE;
ri->failover_state_change_time = mstime();
sentinelEvent(LL_WARNING,"+failover-state-select-slave",ri,"%@");
}
void sentinelFailoverSelectSlave(sentinelRedisInstance *ri) {
sentinelRedisInstance *slave = sentinelSelectSlave(ri);
if (slave == NULL) {
sentinelEvent(LL_WARNING,"-failover-abort-no-good-slave",ri,"%@");
sentinelAbortFailover(ri);
} else {
sentinelEvent(LL_WARNING,"+selected-slave",slave,"%@");
slave->flags |= SRI_PROMOTED;
ri->promoted_slave = slave;
ri->failover_state = SENTINEL_FAILOVER_STATE_SEND_SLAVEOF_NOONE;
ri->failover_state_change_time = mstime();
sentinelEvent(LL_NOTICE,"+failover-state-send-slaveof-noone",
slave, "%@");
}
}
void sentinelFailoverSendSlaveOfNoOne(sentinelRedisInstance *ri) {
int retval;
if (ri->promoted_slave->link->disconnected) {
if (mstime() - ri->failover_state_change_time > ri->failover_timeout) {
sentinelEvent(LL_WARNING,"-failover-abort-slave-timeout",ri,"%@");
sentinelAbortFailover(ri);
}
return;
}
retval = sentinelSendSlaveOf(ri->promoted_slave,NULL);
if (retval != C_OK) return;
sentinelEvent(LL_NOTICE, "+failover-state-wait-promotion",
ri->promoted_slave,"%@");
ri->failover_state = SENTINEL_FAILOVER_STATE_WAIT_PROMOTION;
ri->failover_state_change_time = mstime();
}
void sentinelFailoverWaitPromotion(sentinelRedisInstance *ri) {
if (mstime() - ri->failover_state_change_time > ri->failover_timeout) {
sentinelEvent(LL_WARNING,"-failover-abort-slave-timeout",ri,"%@");
sentinelAbortFailover(ri);
}
}
void sentinelFailoverDetectEnd(sentinelRedisInstance *master) {
int not_reconfigured = 0, timeout = 0;
dictIterator *di;
dictEntry *de;
mstime_t elapsed = mstime() - master->failover_state_change_time;
if (master->promoted_slave == NULL ||
master->promoted_slave->flags & SRI_S_DOWN) return;
di = dictGetIterator(master->slaves);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *slave = dictGetVal(de);
if (slave->flags & (SRI_PROMOTED|SRI_RECONF_DONE)) continue;
if (slave->flags & SRI_S_DOWN) continue;
not_reconfigured++;
}
dictReleaseIterator(di);
if (elapsed > master->failover_timeout) {
not_reconfigured = 0;
timeout = 1;
sentinelEvent(LL_WARNING,"+failover-end-for-timeout",master,"%@");
}
if (not_reconfigured == 0) {
sentinelEvent(LL_WARNING,"+failover-end",master,"%@");
master->failover_state = SENTINEL_FAILOVER_STATE_UPDATE_CONFIG;
master->failover_state_change_time = mstime();
}
if (timeout) {
dictIterator *di;
dictEntry *de;
di = dictGetIterator(master->slaves);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *slave = dictGetVal(de);
int retval;
if (slave->flags & (SRI_PROMOTED|SRI_RECONF_DONE|SRI_RECONF_SENT)) continue;
if (slave->link->disconnected) continue;
retval = sentinelSendSlaveOf(slave,master->promoted_slave->addr);
if (retval == C_OK) {
sentinelEvent(LL_NOTICE,"+slave-reconf-sent-be",slave,"%@");
slave->flags |= SRI_RECONF_SENT;
}
}
dictReleaseIterator(di);
}
}
void sentinelFailoverReconfNextSlave(sentinelRedisInstance *master) {
dictIterator *di;
dictEntry *de;
int in_progress = 0;
di = dictGetIterator(master->slaves);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *slave = dictGetVal(de);
if (slave->flags & (SRI_RECONF_SENT|SRI_RECONF_INPROG))
in_progress++;
}
dictReleaseIterator(di);
di = dictGetIterator(master->slaves);
while(in_progress < master->parallel_syncs &&
(de = dictNext(di)) != NULL)
{
sentinelRedisInstance *slave = dictGetVal(de);
int retval;
if (slave->flags & (SRI_PROMOTED|SRI_RECONF_DONE)) continue;
if ((slave->flags & SRI_RECONF_SENT) &&
(mstime() - slave->slave_reconf_sent_time) >
sentinel_slave_reconf_timeout)
{
sentinelEvent(LL_NOTICE,"-slave-reconf-sent-timeout",slave,"%@");
slave->flags &= ~SRI_RECONF_SENT;
slave->flags |= SRI_RECONF_DONE;
}
if (slave->flags & (SRI_RECONF_SENT|SRI_RECONF_INPROG)) continue;
if (slave->link->disconnected) continue;
retval = sentinelSendSlaveOf(slave,master->promoted_slave->addr);
if (retval == C_OK) {
slave->flags |= SRI_RECONF_SENT;
slave->slave_reconf_sent_time = mstime();
sentinelEvent(LL_NOTICE,"+slave-reconf-sent",slave,"%@");
in_progress++;
}
}
dictReleaseIterator(di);
sentinelFailoverDetectEnd(master);
}
void sentinelFailoverSwitchToPromotedSlave(sentinelRedisInstance *master) {
sentinelRedisInstance *ref = master->promoted_slave ?
master->promoted_slave : master;
sentinelEvent(LL_WARNING,"+switch-master",master,"%s %s %d %s %d",
master->name, announceSentinelAddr(master->addr), master->addr->port,
announceSentinelAddr(ref->addr), ref->addr->port);
sentinelResetMasterAndChangeAddress(master,ref->addr->hostname,ref->addr->port);
}
void sentinelFailoverStateMachine(sentinelRedisInstance *ri) {
serverAssert(ri->flags & SRI_MASTER);
if (!(ri->flags & SRI_FAILOVER_IN_PROGRESS)) return;
switch(ri->failover_state) {
case SENTINEL_FAILOVER_STATE_WAIT_START:
sentinelFailoverWaitStart(ri);
break;
case SENTINEL_FAILOVER_STATE_SELECT_SLAVE:
sentinelFailoverSelectSlave(ri);
break;
case SENTINEL_FAILOVER_STATE_SEND_SLAVEOF_NOONE:
sentinelFailoverSendSlaveOfNoOne(ri);
break;
case SENTINEL_FAILOVER_STATE_WAIT_PROMOTION:
sentinelFailoverWaitPromotion(ri);
break;
case SENTINEL_FAILOVER_STATE_RECONF_SLAVES:
sentinelFailoverReconfNextSlave(ri);
break;
}
}
void sentinelAbortFailover(sentinelRedisInstance *ri) {
serverAssert(ri->flags & SRI_FAILOVER_IN_PROGRESS);
serverAssert(ri->failover_state <= SENTINEL_FAILOVER_STATE_WAIT_PROMOTION);
ri->flags &= ~(SRI_FAILOVER_IN_PROGRESS|SRI_FORCE_FAILOVER);
ri->failover_state = SENTINEL_FAILOVER_STATE_NONE;
ri->failover_state_change_time = mstime();
if (ri->promoted_slave) {
ri->promoted_slave->flags &= ~SRI_PROMOTED;
ri->promoted_slave = NULL;
}
}
void sentinelHandleRedisInstance(sentinelRedisInstance *ri) {
sentinelReconnectInstance(ri);
sentinelSendPeriodicCommands(ri);
if (sentinel.tilt) {
if (mstime()-sentinel.tilt_start_time < sentinel_tilt_period) return;
sentinel.tilt = 0;
sentinelEvent(LL_WARNING,"-tilt",NULL,"#tilt mode exited");
}
sentinelCheckSubjectivelyDown(ri);
if (ri->flags & (SRI_MASTER|SRI_SLAVE)) {
}
if (ri->flags & SRI_MASTER) {
sentinelCheckObjectivelyDown(ri);
if (sentinelStartFailoverIfNeeded(ri))
sentinelAskMasterStateToOtherSentinels(ri,SENTINEL_ASK_FORCED);
sentinelFailoverStateMachine(ri);
sentinelAskMasterStateToOtherSentinels(ri,SENTINEL_NO_FLAGS);
}
}
void sentinelHandleDictOfRedisInstances(dict *instances) {
dictIterator *di;
dictEntry *de;
sentinelRedisInstance *switch_to_promoted = NULL;
di = dictGetIterator(instances);
while((de = dictNext(di)) != NULL) {
sentinelRedisInstance *ri = dictGetVal(de);
sentinelHandleRedisInstance(ri);
if (ri->flags & SRI_MASTER) {
sentinelHandleDictOfRedisInstances(ri->slaves);
sentinelHandleDictOfRedisInstances(ri->sentinels);
if (ri->failover_state == SENTINEL_FAILOVER_STATE_UPDATE_CONFIG) {
switch_to_promoted = ri;
}
}
}
if (switch_to_promoted)
sentinelFailoverSwitchToPromotedSlave(switch_to_promoted);
dictReleaseIterator(di);
}
void sentinelCheckTiltCondition(void) {
mstime_t now = mstime();
mstime_t delta = now - sentinel.previous_time;
if (delta < 0 || delta > sentinel_tilt_trigger) {
sentinel.tilt = 1;
sentinel.tilt_start_time = mstime();
sentinelEvent(LL_WARNING,"+tilt",NULL,"#tilt mode entered");
}
sentinel.previous_time = mstime();
}
void sentinelTimer(void) {
sentinelCheckTiltCondition();
sentinelHandleDictOfRedisInstances(sentinel.masters);
sentinelRunPendingScripts();
sentinelCollectTerminatedScripts();
sentinelKillTimedoutScripts();
server.hz = CONFIG_DEFAULT_HZ + rand() % CONFIG_DEFAULT_HZ;
}
