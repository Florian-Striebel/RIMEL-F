#include "server.h"
#include "monotonic.h"
#include "cluster.h"
#include "slowlog.h"
#include "bio.h"
#include "latency.h"
#include "atomicvar.h"
#include "mt19937-64.h"
#include "functions.h"
#include "hdr_alloc.h"
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <locale.h>
#include <sys/socket.h>
#include <sys/resource.h>
#if defined(__linux__)
#include <sys/mman.h>
#endif
#if defined(HAVE_SYSCTL_KIPC_SOMAXCONN) || defined(HAVE_SYSCTL_KERN_SOMAXCONN)
#include <sys/sysctl.h>
#endif
struct sharedObjectsStruct shared;
double R_Zero, R_PosInf, R_NegInf, R_Nan;
struct redisServer server;
static inline int isShutdownInitiated(void);
int isReadyToShutdown(void);
int finishShutdown(void);
const char *replstateToString(int replstate);
void nolocks_localtime(struct tm *tmp, time_t t, time_t tz, int dst);
void serverLogRaw(int level, const char *msg) {
const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
const char *c = ".-*#";
FILE *fp;
char buf[64];
int rawmode = (level & LL_RAW);
int log_to_stdout = server.logfile[0] == '\0';
level &= 0xff;
if (level < server.verbosity) return;
fp = log_to_stdout ? stdout : fopen(server.logfile,"a");
if (!fp) return;
if (rawmode) {
fprintf(fp,"%s",msg);
} else {
int off;
struct timeval tv;
int role_char;
pid_t pid = getpid();
gettimeofday(&tv,NULL);
struct tm tm;
nolocks_localtime(&tm,tv.tv_sec,server.timezone,server.daylight_active);
off = strftime(buf,sizeof(buf),"%d %b %Y %H:%M:%S.",&tm);
snprintf(buf+off,sizeof(buf)-off,"%03d",(int)tv.tv_usec/1000);
if (server.sentinel_mode) {
role_char = 'X';
} else if (pid != server.pid) {
role_char = 'C';
} else {
role_char = (server.masterhost ? 'S':'M');
}
fprintf(fp,"%d:%c %s %c %s\n",
(int)getpid(),role_char, buf,c[level],msg);
}
fflush(fp);
if (!log_to_stdout) fclose(fp);
if (server.syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}
void _serverLog(int level, const char *fmt, ...) {
va_list ap;
char msg[LOG_MAX_LEN];
va_start(ap, fmt);
vsnprintf(msg, sizeof(msg), fmt, ap);
va_end(ap);
serverLogRaw(level,msg);
}
void serverLogFromHandler(int level, const char *msg) {
int fd;
int log_to_stdout = server.logfile[0] == '\0';
char buf[64];
if ((level&0xff) < server.verbosity || (log_to_stdout && server.daemonize))
return;
fd = log_to_stdout ? STDOUT_FILENO :
open(server.logfile, O_APPEND|O_CREAT|O_WRONLY, 0644);
if (fd == -1) return;
ll2string(buf,sizeof(buf),getpid());
if (write(fd,buf,strlen(buf)) == -1) goto err;
if (write(fd,":signal-handler (",17) == -1) goto err;
ll2string(buf,sizeof(buf),time(NULL));
if (write(fd,buf,strlen(buf)) == -1) goto err;
if (write(fd,") ",2) == -1) goto err;
if (write(fd,msg,strlen(msg)) == -1) goto err;
if (write(fd,"\n",1) == -1) goto err;
err:
if (!log_to_stdout) close(fd);
}
long long ustime(void) {
struct timeval tv;
long long ust;
gettimeofday(&tv, NULL);
ust = ((long long)tv.tv_sec)*1000000;
ust += tv.tv_usec;
return ust;
}
mstime_t mstime(void) {
return ustime()/1000;
}
void exitFromChild(int retcode) {
#if defined(COVERAGE_TEST)
exit(retcode);
#else
_exit(retcode);
#endif
}
void dictVanillaFree(dict *d, void *val)
{
UNUSED(d);
zfree(val);
}
void dictListDestructor(dict *d, void *val)
{
UNUSED(d);
listRelease((list*)val);
}
int dictSdsKeyCompare(dict *d, const void *key1,
const void *key2)
{
int l1,l2;
UNUSED(d);
l1 = sdslen((sds)key1);
l2 = sdslen((sds)key2);
if (l1 != l2) return 0;
return memcmp(key1, key2, l1) == 0;
}
int dictSdsKeyCaseCompare(dict *d, const void *key1,
const void *key2)
{
UNUSED(d);
return strcasecmp(key1, key2) == 0;
}
void dictObjectDestructor(dict *d, void *val)
{
UNUSED(d);
if (val == NULL) return;
decrRefCount(val);
}
void dictSdsDestructor(dict *d, void *val)
{
UNUSED(d);
sdsfree(val);
}
void *dictSdsDup(dict *d, const void *key) {
UNUSED(d);
return sdsdup((const sds) key);
}
int dictObjKeyCompare(dict *d, const void *key1,
const void *key2)
{
const robj *o1 = key1, *o2 = key2;
return dictSdsKeyCompare(d, o1->ptr,o2->ptr);
}
uint64_t dictObjHash(const void *key) {
const robj *o = key;
return dictGenHashFunction(o->ptr, sdslen((sds)o->ptr));
}
uint64_t dictSdsHash(const void *key) {
return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
}
uint64_t dictSdsCaseHash(const void *key) {
return dictGenCaseHashFunction((unsigned char*)key, sdslen((char*)key));
}
int dictEncObjKeyCompare(dict *d, const void *key1, const void *key2)
{
robj *o1 = (robj*) key1, *o2 = (robj*) key2;
int cmp;
if (o1->encoding == OBJ_ENCODING_INT &&
o2->encoding == OBJ_ENCODING_INT)
return o1->ptr == o2->ptr;
if (o1->refcount != OBJ_STATIC_REFCOUNT) o1 = getDecodedObject(o1);
if (o2->refcount != OBJ_STATIC_REFCOUNT) o2 = getDecodedObject(o2);
cmp = dictSdsKeyCompare(d,o1->ptr,o2->ptr);
if (o1->refcount != OBJ_STATIC_REFCOUNT) decrRefCount(o1);
if (o2->refcount != OBJ_STATIC_REFCOUNT) decrRefCount(o2);
return cmp;
}
uint64_t dictEncObjHash(const void *key) {
robj *o = (robj*) key;
if (sdsEncodedObject(o)) {
return dictGenHashFunction(o->ptr, sdslen((sds)o->ptr));
} else if (o->encoding == OBJ_ENCODING_INT) {
char buf[32];
int len;
len = ll2string(buf,32,(long)o->ptr);
return dictGenHashFunction((unsigned char*)buf, len);
} else {
serverPanic("Unknown string encoding");
}
}
int dictExpandAllowed(size_t moreMem, double usedRatio) {
if (usedRatio <= HASHTABLE_MAX_LOAD_FACTOR) {
return !overMaxmemoryAfterAlloc(moreMem);
} else {
return 1;
}
}
size_t dictEntryMetadataSize(dict *d) {
UNUSED(d);
return server.cluster_enabled ? sizeof(clusterDictEntryMetadata) : 0;
}
dictType objectKeyPointerValueDictType = {
dictEncObjHash,
NULL,
NULL,
dictEncObjKeyCompare,
dictObjectDestructor,
NULL,
NULL
};
dictType objectKeyHeapPointerValueDictType = {
dictEncObjHash,
NULL,
NULL,
dictEncObjKeyCompare,
dictObjectDestructor,
dictVanillaFree,
NULL
};
dictType setDictType = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
dictSdsDestructor,
NULL
};
dictType zsetDictType = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
NULL,
NULL,
NULL
};
dictType dbDictType = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
dictSdsDestructor,
dictObjectDestructor,
dictExpandAllowed,
dictEntryMetadataSize
};
dictType dbExpiresDictType = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
NULL,
NULL,
dictExpandAllowed
};
dictType commandTableDictType = {
dictSdsCaseHash,
NULL,
NULL,
dictSdsKeyCaseCompare,
dictSdsDestructor,
NULL,
NULL
};
dictType hashDictType = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
dictSdsDestructor,
dictSdsDestructor,
NULL
};
dictType sdsReplyDictType = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
NULL,
NULL,
NULL
};
dictType keylistDictType = {
dictObjHash,
NULL,
NULL,
dictObjKeyCompare,
dictObjectDestructor,
dictListDestructor,
NULL
};
dictType modulesDictType = {
dictSdsCaseHash,
NULL,
NULL,
dictSdsKeyCaseCompare,
dictSdsDestructor,
NULL,
NULL
};
dictType migrateCacheDictType = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
dictSdsDestructor,
NULL,
NULL
};
dictType replScriptCacheDictType = {
dictSdsCaseHash,
NULL,
NULL,
dictSdsKeyCaseCompare,
dictSdsDestructor,
NULL,
NULL
};
int htNeedsResize(dict *dict) {
long long size, used;
size = dictSlots(dict);
used = dictSize(dict);
return (size > DICT_HT_INITIAL_SIZE &&
(used*100/size < HASHTABLE_MIN_FILL));
}
void tryResizeHashTables(int dbid) {
if (htNeedsResize(server.db[dbid].dict))
dictResize(server.db[dbid].dict);
if (htNeedsResize(server.db[dbid].expires))
dictResize(server.db[dbid].expires);
}
int incrementallyRehash(int dbid) {
if (dictIsRehashing(server.db[dbid].dict)) {
dictRehashMilliseconds(server.db[dbid].dict,1);
return 1;
}
if (dictIsRehashing(server.db[dbid].expires)) {
dictRehashMilliseconds(server.db[dbid].expires,1);
return 1;
}
return 0;
}
void updateDictResizePolicy(void) {
if (!hasActiveChildProcess())
dictEnableResize();
else
dictDisableResize();
}
const char *strChildType(int type) {
switch(type) {
case CHILD_TYPE_RDB: return "RDB";
case CHILD_TYPE_AOF: return "AOF";
case CHILD_TYPE_LDB: return "LDB";
case CHILD_TYPE_MODULE: return "MODULE";
default: return "Unknown";
}
}
int hasActiveChildProcess() {
return server.child_pid != -1;
}
void resetChildState() {
server.child_type = CHILD_TYPE_NONE;
server.child_pid = -1;
server.stat_current_cow_peak = 0;
server.stat_current_cow_bytes = 0;
server.stat_current_cow_updated = 0;
server.stat_current_save_keys_processed = 0;
server.stat_module_progress = 0;
server.stat_current_save_keys_total = 0;
updateDictResizePolicy();
closeChildInfoPipe();
moduleFireServerEvent(REDISMODULE_EVENT_FORK_CHILD,
REDISMODULE_SUBEVENT_FORK_CHILD_DIED,
NULL);
}
int isMutuallyExclusiveChildType(int type) {
return type == CHILD_TYPE_RDB || type == CHILD_TYPE_AOF || type == CHILD_TYPE_MODULE;
}
int allPersistenceDisabled(void) {
return server.saveparamslen == 0 && server.aof_state == AOF_OFF;
}
void trackInstantaneousMetric(int metric, long long current_reading) {
long long now = mstime();
long long t = now - server.inst_metric[metric].last_sample_time;
long long ops = current_reading -
server.inst_metric[metric].last_sample_count;
long long ops_sec;
ops_sec = t > 0 ? (ops*1000/t) : 0;
server.inst_metric[metric].samples[server.inst_metric[metric].idx] =
ops_sec;
server.inst_metric[metric].idx++;
server.inst_metric[metric].idx %= STATS_METRIC_SAMPLES;
server.inst_metric[metric].last_sample_time = now;
server.inst_metric[metric].last_sample_count = current_reading;
}
long long getInstantaneousMetric(int metric) {
int j;
long long sum = 0;
for (j = 0; j < STATS_METRIC_SAMPLES; j++)
sum += server.inst_metric[metric].samples[j];
return sum / STATS_METRIC_SAMPLES;
}
int clientsCronResizeQueryBuffer(client *c) {
size_t querybuf_size = sdsalloc(c->querybuf);
time_t idletime = server.unixtime - c->lastinteraction;
if (sdsavail(c->querybuf) > 1024*4) {
if (idletime > 2) {
c->querybuf = sdsRemoveFreeSpace(c->querybuf);
} else if (querybuf_size > PROTO_RESIZE_THRESHOLD && querybuf_size/2 > c->querybuf_peak) {
size_t resize = sdslen(c->querybuf);
if (resize < c->querybuf_peak) resize = c->querybuf_peak;
if (c->bulklen != -1 && resize < (size_t)c->bulklen) resize = c->bulklen;
c->querybuf = sdsResize(c->querybuf, resize);
}
}
c->querybuf_peak = sdslen(c->querybuf);
if (c->bulklen != -1 && (size_t)c->bulklen > c->querybuf_peak)
c->querybuf_peak = c->bulklen;
if (c->flags & CLIENT_MASTER) {
size_t pending_querybuf_size = sdsAllocSize(c->pending_querybuf);
if(pending_querybuf_size > LIMIT_PENDING_QUERYBUF &&
sdslen(c->pending_querybuf) < (pending_querybuf_size/2))
{
c->pending_querybuf = sdsRemoveFreeSpace(c->pending_querybuf);
}
}
return 0;
}
#define CLIENTS_PEAK_MEM_USAGE_SLOTS 8
size_t ClientsPeakMemInput[CLIENTS_PEAK_MEM_USAGE_SLOTS] = {0};
size_t ClientsPeakMemOutput[CLIENTS_PEAK_MEM_USAGE_SLOTS] = {0};
int clientsCronTrackExpansiveClients(client *c, int time_idx) {
size_t in_usage = sdsZmallocSize(c->querybuf) + c->argv_len_sum +
(c->argv ? zmalloc_size(c->argv) : 0);
size_t out_usage = getClientOutputBufferMemoryUsage(c);
if (in_usage > ClientsPeakMemInput[time_idx]) ClientsPeakMemInput[time_idx] = in_usage;
if (out_usage > ClientsPeakMemOutput[time_idx]) ClientsPeakMemOutput[time_idx] = out_usage;
return 0;
}
clientMemUsageBucket *getMemUsageBucket(size_t mem) {
int size_in_bits = 8*(int)sizeof(mem);
int clz = mem > 0 ? __builtin_clzl(mem) : size_in_bits;
int bucket_idx = size_in_bits - clz;
if (bucket_idx > CLIENT_MEM_USAGE_BUCKET_MAX_LOG)
bucket_idx = CLIENT_MEM_USAGE_BUCKET_MAX_LOG;
else if (bucket_idx < CLIENT_MEM_USAGE_BUCKET_MIN_LOG)
bucket_idx = CLIENT_MEM_USAGE_BUCKET_MIN_LOG;
bucket_idx -= CLIENT_MEM_USAGE_BUCKET_MIN_LOG;
return &server.client_mem_usage_buckets[bucket_idx];
}
int updateClientMemUsage(client *c) {
size_t mem = getClientMemoryUsage(c, NULL);
int type = getClientType(c);
atomicDecr(server.stat_clients_type_memory[c->last_memory_type], c->last_memory_usage);
atomicIncr(server.stat_clients_type_memory[type], mem);
c->last_memory_usage = mem;
c->last_memory_type = type;
if (io_threads_op == IO_THREADS_OP_IDLE)
updateClientMemUsageBucket(c);
return 0;
}
void updateClientMemUsageBucket(client *c) {
serverAssert(io_threads_op == IO_THREADS_OP_IDLE);
int allow_eviction =
(c->last_memory_type == CLIENT_TYPE_NORMAL || c->last_memory_type == CLIENT_TYPE_PUBSUB) &&
!(c->flags & CLIENT_NO_EVICT);
if (c->mem_usage_bucket) {
c->mem_usage_bucket->mem_usage_sum -= c->last_memory_usage_on_bucket_update;
if (!allow_eviction) {
listDelNode(c->mem_usage_bucket->clients, c->mem_usage_bucket_node);
c->mem_usage_bucket = NULL;
c->mem_usage_bucket_node = NULL;
}
}
if (allow_eviction) {
clientMemUsageBucket *bucket = getMemUsageBucket(c->last_memory_usage);
bucket->mem_usage_sum += c->last_memory_usage;
if (bucket != c->mem_usage_bucket) {
if (c->mem_usage_bucket)
listDelNode(c->mem_usage_bucket->clients,
c->mem_usage_bucket_node);
c->mem_usage_bucket = bucket;
listAddNodeTail(bucket->clients, c);
c->mem_usage_bucket_node = listLast(bucket->clients);
}
}
c->last_memory_usage_on_bucket_update = c->last_memory_usage;
}
void getExpansiveClientsInfo(size_t *in_usage, size_t *out_usage) {
size_t i = 0, o = 0;
for (int j = 0; j < CLIENTS_PEAK_MEM_USAGE_SLOTS; j++) {
if (ClientsPeakMemInput[j] > i) i = ClientsPeakMemInput[j];
if (ClientsPeakMemOutput[j] > o) o = ClientsPeakMemOutput[j];
}
*in_usage = i;
*out_usage = o;
}
#define CLIENTS_CRON_MIN_ITERATIONS 5
void clientsCron(void) {
int numclients = listLength(server.clients);
int iterations = numclients/server.hz;
mstime_t now = mstime();
if (iterations < CLIENTS_CRON_MIN_ITERATIONS)
iterations = (numclients < CLIENTS_CRON_MIN_ITERATIONS) ?
numclients : CLIENTS_CRON_MIN_ITERATIONS;
int curr_peak_mem_usage_slot = server.unixtime % CLIENTS_PEAK_MEM_USAGE_SLOTS;
int zeroidx = (curr_peak_mem_usage_slot+1) % CLIENTS_PEAK_MEM_USAGE_SLOTS;
ClientsPeakMemInput[zeroidx] = 0;
ClientsPeakMemOutput[zeroidx] = 0;
while(listLength(server.clients) && iterations--) {
client *c;
listNode *head;
listRotateTailToHead(server.clients);
head = listFirst(server.clients);
c = listNodeValue(head);
if (clientsCronHandleTimeout(c,now)) continue;
if (clientsCronResizeQueryBuffer(c)) continue;
if (clientsCronTrackExpansiveClients(c, curr_peak_mem_usage_slot)) continue;
if (updateClientMemUsage(c)) continue;
if (closeClientOnOutputBufferLimitReached(c, 0)) continue;
}
}
void databasesCron(void) {
if (server.active_expire_enabled) {
if (iAmMaster()) {
activeExpireCycle(ACTIVE_EXPIRE_CYCLE_SLOW);
} else {
expireSlaveKeys();
}
}
activeDefragCycle();
if (!hasActiveChildProcess()) {
static unsigned int resize_db = 0;
static unsigned int rehash_db = 0;
int dbs_per_call = CRON_DBS_PER_CALL;
int j;
if (dbs_per_call > server.dbnum) dbs_per_call = server.dbnum;
for (j = 0; j < dbs_per_call; j++) {
tryResizeHashTables(resize_db % server.dbnum);
resize_db++;
}
if (server.activerehashing) {
for (j = 0; j < dbs_per_call; j++) {
int work_done = incrementallyRehash(rehash_db);
if (work_done) {
break;
} else {
rehash_db++;
rehash_db %= server.dbnum;
}
}
}
}
}
void updateCachedTime(int update_daylight_info) {
server.ustime = ustime();
server.mstime = server.ustime / 1000;
time_t unixtime = server.mstime / 1000;
atomicSet(server.unixtime, unixtime);
if (update_daylight_info) {
struct tm tm;
time_t ut = server.unixtime;
localtime_r(&ut,&tm);
server.daylight_active = tm.tm_isdst;
}
}
void checkChildrenDone(void) {
int statloc = 0;
pid_t pid;
if ((pid = waitpid(-1, &statloc, WNOHANG)) != 0) {
int exitcode = WIFEXITED(statloc) ? WEXITSTATUS(statloc) : -1;
int bysignal = 0;
if (WIFSIGNALED(statloc)) bysignal = WTERMSIG(statloc);
if (exitcode == SERVER_CHILD_NOERROR_RETVAL) {
bysignal = SIGUSR1;
exitcode = 1;
}
if (pid == -1) {
serverLog(LL_WARNING,"waitpid() returned an error: %s. "
"child_type: %s, child_pid = %d",
strerror(errno),
strChildType(server.child_type),
(int) server.child_pid);
} else if (pid == server.child_pid) {
if (server.child_type == CHILD_TYPE_RDB) {
backgroundSaveDoneHandler(exitcode, bysignal);
} else if (server.child_type == CHILD_TYPE_AOF) {
backgroundRewriteDoneHandler(exitcode, bysignal);
} else if (server.child_type == CHILD_TYPE_MODULE) {
ModuleForkDoneHandler(exitcode, bysignal);
} else {
serverPanic("Unknown child type %d for child pid %d", server.child_type, server.child_pid);
exit(1);
}
if (!bysignal && exitcode == 0) receiveChildInfo();
resetChildState();
} else {
if (!ldbRemoveChild(pid)) {
serverLog(LL_WARNING,
"Warning, detected child with unmatched pid: %ld",
(long) pid);
}
}
replicationStartPendingFork();
}
}
void cronUpdateMemoryStats() {
if (zmalloc_used_memory() > server.stat_peak_memory)
server.stat_peak_memory = zmalloc_used_memory();
run_with_period(100) {
server.cron_malloc_stats.process_rss = zmalloc_get_rss();
server.cron_malloc_stats.zmalloc_used = zmalloc_used_memory();
zmalloc_get_allocator_info(&server.cron_malloc_stats.allocator_allocated,
&server.cron_malloc_stats.allocator_active,
&server.cron_malloc_stats.allocator_resident);
if (!server.cron_malloc_stats.allocator_resident) {
size_t lua_memory = evalMemory();
server.cron_malloc_stats.allocator_resident = server.cron_malloc_stats.process_rss - lua_memory;
}
if (!server.cron_malloc_stats.allocator_active)
server.cron_malloc_stats.allocator_active = server.cron_malloc_stats.allocator_resident;
if (!server.cron_malloc_stats.allocator_allocated)
server.cron_malloc_stats.allocator_allocated = server.cron_malloc_stats.zmalloc_used;
}
}
int serverCron(struct aeEventLoop *eventLoop, long long id, void *clientData) {
int j;
UNUSED(eventLoop);
UNUSED(id);
UNUSED(clientData);
if (server.watchdog_period) watchdogScheduleSignal(server.watchdog_period);
updateCachedTime(1);
server.hz = server.config_hz;
if (server.dynamic_hz) {
while (listLength(server.clients) / server.hz >
MAX_CLIENTS_PER_CLOCK_TICK)
{
server.hz *= 2;
if (server.hz > CONFIG_MAX_HZ) {
server.hz = CONFIG_MAX_HZ;
break;
}
}
}
if (server.pause_cron) return 1000/server.hz;
run_with_period(100) {
long long stat_net_input_bytes, stat_net_output_bytes;
atomicGet(server.stat_net_input_bytes, stat_net_input_bytes);
atomicGet(server.stat_net_output_bytes, stat_net_output_bytes);
trackInstantaneousMetric(STATS_METRIC_COMMAND,server.stat_numcommands);
trackInstantaneousMetric(STATS_METRIC_NET_INPUT,
stat_net_input_bytes);
trackInstantaneousMetric(STATS_METRIC_NET_OUTPUT,
stat_net_output_bytes);
}
unsigned int lruclock = getLRUClock();
atomicSet(server.lruclock,lruclock);
cronUpdateMemoryStats();
if (server.shutdown_asap && !isShutdownInitiated()) {
if (prepareForShutdown(SHUTDOWN_NOFLAGS) == C_OK) exit(0);
} else if (isShutdownInitiated()) {
if (server.mstime >= server.shutdown_mstime || isReadyToShutdown()) {
if (finishShutdown() == C_OK) exit(0);
}
}
if (server.verbosity <= LL_VERBOSE) {
run_with_period(5000) {
for (j = 0; j < server.dbnum; j++) {
long long size, used, vkeys;
size = dictSlots(server.db[j].dict);
used = dictSize(server.db[j].dict);
vkeys = dictSize(server.db[j].expires);
if (used || vkeys) {
serverLog(LL_VERBOSE,"DB %d: %lld keys (%lld volatile) in %lld slots HT.",j,used,vkeys,size);
}
}
}
}
if (!server.sentinel_mode) {
run_with_period(5000) {
serverLog(LL_DEBUG,
"%lu clients connected (%lu replicas), %zu bytes in use",
listLength(server.clients)-listLength(server.slaves),
listLength(server.slaves),
zmalloc_used_memory());
}
}
clientsCron();
databasesCron();
if (!hasActiveChildProcess() &&
server.aof_rewrite_scheduled &&
!aofRewriteLimited())
{
rewriteAppendOnlyFileBackground();
}
if (hasActiveChildProcess() || ldbPendingChildren())
{
run_with_period(1000) receiveChildInfo();
checkChildrenDone();
} else {
for (j = 0; j < server.saveparamslen; j++) {
struct saveparam *sp = server.saveparams+j;
if (server.dirty >= sp->changes &&
server.unixtime-server.lastsave > sp->seconds &&
(server.unixtime-server.lastbgsave_try >
CONFIG_BGSAVE_RETRY_DELAY ||
server.lastbgsave_status == C_OK))
{
serverLog(LL_NOTICE,"%d changes in %d seconds. Saving...",
sp->changes, (int)sp->seconds);
rdbSaveInfo rsi, *rsiptr;
rsiptr = rdbPopulateSaveInfo(&rsi);
rdbSaveBackground(SLAVE_REQ_NONE,server.rdb_filename,rsiptr);
break;
}
}
if (server.aof_state == AOF_ON &&
!hasActiveChildProcess() &&
server.aof_rewrite_perc &&
server.aof_current_size > server.aof_rewrite_min_size &&
!aofRewriteLimited())
{
long long base = server.aof_rewrite_base_size ?
server.aof_rewrite_base_size : 1;
long long growth = (server.aof_current_size*100/base) - 100;
if (growth >= server.aof_rewrite_perc) {
serverLog(LL_NOTICE,"Starting automatic rewriting of AOF on %lld%% growth",growth);
rewriteAppendOnlyFileBackground();
}
}
}
updateDictResizePolicy();
if ((server.aof_state == AOF_ON || server.aof_state == AOF_WAIT_REWRITE) &&
server.aof_flush_postponed_start)
{
flushAppendOnlyFile(0);
}
run_with_period(1000) {
if (server.aof_state == AOF_ON && server.aof_last_write_status == C_ERR)
flushAppendOnlyFile(0);
}
checkClientPauseTimeoutAndReturnIfPaused();
if (server.failover_state != NO_FAILOVER) {
run_with_period(100) replicationCron();
} else {
run_with_period(1000) replicationCron();
}
run_with_period(100) {
if (server.cluster_enabled) clusterCron();
}
if (server.sentinel_mode) sentinelTimer();
run_with_period(1000) {
migrateCloseTimedoutSockets();
}
stopThreadedIOIfNeeded();
if (server.tracking_clients) trackingLimitUsedSlots();
if (!hasActiveChildProcess() &&
server.rdb_bgsave_scheduled &&
(server.unixtime-server.lastbgsave_try > CONFIG_BGSAVE_RETRY_DELAY ||
server.lastbgsave_status == C_OK))
{
rdbSaveInfo rsi, *rsiptr;
rsiptr = rdbPopulateSaveInfo(&rsi);
if (rdbSaveBackground(SLAVE_REQ_NONE,server.rdb_filename,rsiptr) == C_OK)
server.rdb_bgsave_scheduled = 0;
}
run_with_period(100) {
if (moduleCount()) modulesCron();
}
RedisModuleCronLoopV1 ei = {REDISMODULE_CRON_LOOP_VERSION,server.hz};
moduleFireServerEvent(REDISMODULE_EVENT_CRON_LOOP,
0,
&ei);
server.cronloops++;
return 1000/server.hz;
}
void blockingOperationStarts() {
if(!server.blocking_op_nesting++){
updateCachedTime(0);
server.blocked_last_cron = server.mstime;
}
}
void blockingOperationEnds() {
if(!(--server.blocking_op_nesting)){
server.blocked_last_cron = 0;
}
}
void whileBlockedCron() {
serverAssert(server.blocked_last_cron);
if (server.blocked_last_cron >= server.mstime)
return;
mstime_t latency;
latencyStartMonitor(latency);
long hz_ms = 1000/server.hz;
while (server.blocked_last_cron < server.mstime) {
activeDefragCycle();
server.blocked_last_cron += hz_ms;
server.cronloops++;
}
if (server.loading) cronUpdateMemoryStats();
latencyEndMonitor(latency);
latencyAddSampleIfNeeded("while-blocked-cron",latency);
if (server.shutdown_asap && server.loading) {
if (prepareForShutdown(SHUTDOWN_NOSAVE) == C_OK) exit(0);
serverLog(LL_WARNING,"SIGTERM received but errors trying to shut down the server, check the logs for more information");
server.shutdown_asap = 0;
}
}
static void sendGetackToReplicas(void) {
robj *argv[3];
argv[0] = shared.replconf;
argv[1] = shared.getack;
argv[2] = shared.special_asterick;
replicationFeedSlaves(server.slaves, server.slaveseldb, argv, 3);
}
extern int ProcessingEventsWhileBlocked;
void beforeSleep(struct aeEventLoop *eventLoop) {
UNUSED(eventLoop);
size_t zmalloc_used = zmalloc_used_memory();
if (zmalloc_used > server.stat_peak_memory)
server.stat_peak_memory = zmalloc_used;
if (ProcessingEventsWhileBlocked) {
uint64_t processed = 0;
processed += handleClientsWithPendingReadsUsingThreads();
processed += tlsProcessPendingData();
processed += handleClientsWithPendingWrites();
processed += freeClientsInAsyncFreeQueue();
server.events_processed_while_blocked += processed;
return;
}
handleBlockedClientsTimeout();
handleClientsWithPendingReadsUsingThreads();
tlsProcessPendingData();
aeSetDontWait(server.el, tlsHasPendingData());
if (server.cluster_enabled) clusterBeforeSleep();
if (server.active_expire_enabled && server.masterhost == NULL)
activeExpireCycle(ACTIVE_EXPIRE_CYCLE_FAST);
if (listLength(server.clients_waiting_acks))
processClientsWaitingReplicas();
if (moduleCount()) {
moduleFireServerEvent(REDISMODULE_EVENT_EVENTLOOP,
REDISMODULE_SUBEVENT_EVENTLOOP_BEFORE_SLEEP,
NULL);
moduleHandleBlockedClients();
}
if (listLength(server.unblocked_clients))
processUnblockedClients();
if (server.get_ack_from_slaves && !checkClientPauseTimeoutAndReturnIfPaused()) {
sendGetackToReplicas();
server.get_ack_from_slaves = 0;
}
updateFailoverStatus();
serverAssert(listLength(server.tracking_pending_keys) == 0);
trackingBroadcastInvalidationMessages();
if (server.aof_state == AOF_ON || server.aof_state == AOF_WAIT_REWRITE)
flushAppendOnlyFile(0);
handleClientsBlockedOnKeys();
handleClientsWithPendingWritesUsingThreads();
freeClientsInAsyncFreeQueue();
if (server.repl_backlog)
incrementalTrimReplicationBacklog(10*REPL_BACKLOG_TRIM_BLOCKS_PER_CALL);
evictClients();
if (moduleCount()) moduleReleaseGIL();
}
void afterSleep(struct aeEventLoop *eventLoop) {
UNUSED(eventLoop);
if (!ProcessingEventsWhileBlocked) {
if (moduleCount()) {
mstime_t latency;
latencyStartMonitor(latency);
moduleAcquireGIL();
moduleFireServerEvent(REDISMODULE_EVENT_EVENTLOOP,
REDISMODULE_SUBEVENT_EVENTLOOP_AFTER_SLEEP,
NULL);
latencyEndMonitor(latency);
latencyAddSampleIfNeeded("module-acquire-GIL",latency);
}
}
}
void createSharedObjects(void) {
int j;
shared.crlf = createObject(OBJ_STRING,sdsnew("\r\n"));
shared.ok = createObject(OBJ_STRING,sdsnew("+OK\r\n"));
shared.emptybulk = createObject(OBJ_STRING,sdsnew("$0\r\n\r\n"));
shared.czero = createObject(OBJ_STRING,sdsnew(":0\r\n"));
shared.cone = createObject(OBJ_STRING,sdsnew(":1\r\n"));
shared.emptyarray = createObject(OBJ_STRING,sdsnew("*0\r\n"));
shared.pong = createObject(OBJ_STRING,sdsnew("+PONG\r\n"));
shared.queued = createObject(OBJ_STRING,sdsnew("+QUEUED\r\n"));
shared.emptyscan = createObject(OBJ_STRING,sdsnew("*2\r\n$1\r\n0\r\n*0\r\n"));
shared.space = createObject(OBJ_STRING,sdsnew(" "));
shared.plus = createObject(OBJ_STRING,sdsnew("+"));
shared.wrongtypeerr = createObject(OBJ_STRING,sdsnew(
"-WRONGTYPE Operation against a key holding the wrong kind of value\r\n"));
shared.err = createObject(OBJ_STRING,sdsnew("-ERR\r\n"));
shared.nokeyerr = createObject(OBJ_STRING,sdsnew(
"-ERR no such key\r\n"));
shared.syntaxerr = createObject(OBJ_STRING,sdsnew(
"-ERR syntax error\r\n"));
shared.sameobjecterr = createObject(OBJ_STRING,sdsnew(
"-ERR source and destination objects are the same\r\n"));
shared.outofrangeerr = createObject(OBJ_STRING,sdsnew(
"-ERR index out of range\r\n"));
shared.noscripterr = createObject(OBJ_STRING,sdsnew(
"-NOSCRIPT No matching script. Please use EVAL.\r\n"));
shared.loadingerr = createObject(OBJ_STRING,sdsnew(
"-LOADING Redis is loading the dataset in memory\r\n"));
shared.slowevalerr = createObject(OBJ_STRING,sdsnew(
"-BUSY Redis is busy running a script. You can only call SCRIPT KILL or SHUTDOWN NOSAVE.\r\n"));
shared.slowscripterr = createObject(OBJ_STRING,sdsnew(
"-BUSY Redis is busy running a script. You can only call FUNCTION KILL or SHUTDOWN NOSAVE.\r\n"));
shared.slowmoduleerr = createObject(OBJ_STRING,sdsnew(
"-BUSY Redis is busy running a module command.\r\n"));
shared.masterdownerr = createObject(OBJ_STRING,sdsnew(
"-MASTERDOWN Link with MASTER is down and replica-serve-stale-data is set to 'no'.\r\n"));
shared.bgsaveerr = createObject(OBJ_STRING,sdsnew(
"-MISCONF Redis is configured to save RDB snapshots, but it's currently unable to persist to disk. Commands that may modify the data set are disabled, because this instance is configured to report errors during writes if RDB snapshotting fails (stop-writes-on-bgsave-error option). Please check the Redis logs for details about the RDB error.\r\n"));
shared.roslaveerr = createObject(OBJ_STRING,sdsnew(
"-READONLY You can't write against a read only replica.\r\n"));
shared.noautherr = createObject(OBJ_STRING,sdsnew(
"-NOAUTH Authentication required.\r\n"));
shared.oomerr = createObject(OBJ_STRING,sdsnew(
"-OOM command not allowed when used memory > 'maxmemory'.\r\n"));
shared.execaborterr = createObject(OBJ_STRING,sdsnew(
"-EXECABORT Transaction discarded because of previous errors.\r\n"));
shared.noreplicaserr = createObject(OBJ_STRING,sdsnew(
"-NOREPLICAS Not enough good replicas to write.\r\n"));
shared.busykeyerr = createObject(OBJ_STRING,sdsnew(
"-BUSYKEY Target key name already exists.\r\n"));
shared.null[0] = NULL;
shared.null[1] = NULL;
shared.null[2] = createObject(OBJ_STRING,sdsnew("$-1\r\n"));
shared.null[3] = createObject(OBJ_STRING,sdsnew("_\r\n"));
shared.nullarray[0] = NULL;
shared.nullarray[1] = NULL;
shared.nullarray[2] = createObject(OBJ_STRING,sdsnew("*-1\r\n"));
shared.nullarray[3] = createObject(OBJ_STRING,sdsnew("_\r\n"));
shared.emptymap[0] = NULL;
shared.emptymap[1] = NULL;
shared.emptymap[2] = createObject(OBJ_STRING,sdsnew("*0\r\n"));
shared.emptymap[3] = createObject(OBJ_STRING,sdsnew("%0\r\n"));
shared.emptyset[0] = NULL;
shared.emptyset[1] = NULL;
shared.emptyset[2] = createObject(OBJ_STRING,sdsnew("*0\r\n"));
shared.emptyset[3] = createObject(OBJ_STRING,sdsnew("~0\r\n"));
for (j = 0; j < PROTO_SHARED_SELECT_CMDS; j++) {
char dictid_str[64];
int dictid_len;
dictid_len = ll2string(dictid_str,sizeof(dictid_str),j);
shared.select[j] = createObject(OBJ_STRING,
sdscatprintf(sdsempty(),
"*2\r\n$6\r\nSELECT\r\n$%d\r\n%s\r\n",
dictid_len, dictid_str));
}
shared.messagebulk = createStringObject("$7\r\nmessage\r\n",13);
shared.pmessagebulk = createStringObject("$8\r\npmessage\r\n",14);
shared.subscribebulk = createStringObject("$9\r\nsubscribe\r\n",15);
shared.unsubscribebulk = createStringObject("$11\r\nunsubscribe\r\n",18);
shared.ssubscribebulk = createStringObject("$10\r\nssubscribe\r\n", 17);
shared.sunsubscribebulk = createStringObject("$12\r\nsunsubscribe\r\n", 19);
shared.psubscribebulk = createStringObject("$10\r\npsubscribe\r\n",17);
shared.punsubscribebulk = createStringObject("$12\r\npunsubscribe\r\n",19);
shared.del = createStringObject("DEL",3);
shared.unlink = createStringObject("UNLINK",6);
shared.rpop = createStringObject("RPOP",4);
shared.lpop = createStringObject("LPOP",4);
shared.lpush = createStringObject("LPUSH",5);
shared.rpoplpush = createStringObject("RPOPLPUSH",9);
shared.lmove = createStringObject("LMOVE",5);
shared.blmove = createStringObject("BLMOVE",6);
shared.zpopmin = createStringObject("ZPOPMIN",7);
shared.zpopmax = createStringObject("ZPOPMAX",7);
shared.multi = createStringObject("MULTI",5);
shared.exec = createStringObject("EXEC",4);
shared.hset = createStringObject("HSET",4);
shared.srem = createStringObject("SREM",4);
shared.xgroup = createStringObject("XGROUP",6);
shared.xclaim = createStringObject("XCLAIM",6);
shared.script = createStringObject("SCRIPT",6);
shared.replconf = createStringObject("REPLCONF",8);
shared.pexpireat = createStringObject("PEXPIREAT",9);
shared.pexpire = createStringObject("PEXPIRE",7);
shared.persist = createStringObject("PERSIST",7);
shared.set = createStringObject("SET",3);
shared.eval = createStringObject("EVAL",4);
shared.left = createStringObject("left",4);
shared.right = createStringObject("right",5);
shared.pxat = createStringObject("PXAT", 4);
shared.time = createStringObject("TIME",4);
shared.retrycount = createStringObject("RETRYCOUNT",10);
shared.force = createStringObject("FORCE",5);
shared.justid = createStringObject("JUSTID",6);
shared.lastid = createStringObject("LASTID",6);
shared.default_username = createStringObject("default",7);
shared.ping = createStringObject("ping",4);
shared.setid = createStringObject("SETID",5);
shared.keepttl = createStringObject("KEEPTTL",7);
shared.absttl = createStringObject("ABSTTL",6);
shared.load = createStringObject("LOAD",4);
shared.createconsumer = createStringObject("CREATECONSUMER",14);
shared.getack = createStringObject("GETACK",6);
shared.special_asterick = createStringObject("*",1);
shared.special_equals = createStringObject("=",1);
shared.redacted = makeObjectShared(createStringObject("(redacted)",10));
for (j = 0; j < OBJ_SHARED_INTEGERS; j++) {
shared.integers[j] =
makeObjectShared(createObject(OBJ_STRING,(void*)(long)j));
shared.integers[j]->encoding = OBJ_ENCODING_INT;
}
for (j = 0; j < OBJ_SHARED_BULKHDR_LEN; j++) {
shared.mbulkhdr[j] = createObject(OBJ_STRING,
sdscatprintf(sdsempty(),"*%d\r\n",j));
shared.bulkhdr[j] = createObject(OBJ_STRING,
sdscatprintf(sdsempty(),"$%d\r\n",j));
}
shared.minstring = sdsnew("minstring");
shared.maxstring = sdsnew("maxstring");
}
void initServerConfig(void) {
int j;
char *default_bindaddr[CONFIG_DEFAULT_BINDADDR_COUNT] = CONFIG_DEFAULT_BINDADDR;
initConfigValues();
updateCachedTime(1);
getRandomHexChars(server.runid,CONFIG_RUN_ID_SIZE);
server.runid[CONFIG_RUN_ID_SIZE] = '\0';
changeReplicationId();
clearReplicationId2();
server.hz = CONFIG_DEFAULT_HZ;
server.timezone = getTimeZone();
server.configfile = NULL;
server.executable = NULL;
server.arch_bits = (sizeof(long) == 8) ? 64 : 32;
server.bindaddr_count = CONFIG_DEFAULT_BINDADDR_COUNT;
for (j = 0; j < CONFIG_DEFAULT_BINDADDR_COUNT; j++)
server.bindaddr[j] = zstrdup(default_bindaddr[j]);
server.ipfd.count = 0;
server.tlsfd.count = 0;
server.sofd = -1;
server.active_expire_enabled = 1;
server.skip_checksum_validation = 0;
server.loading = 0;
server.async_loading = 0;
server.loading_rdb_used_mem = 0;
server.aof_state = AOF_OFF;
server.aof_rewrite_base_size = 0;
server.aof_rewrite_scheduled = 0;
server.aof_flush_sleep = 0;
server.aof_last_fsync = time(NULL);
server.aof_cur_timestamp = 0;
atomicSet(server.aof_bio_fsync_status,C_OK);
server.aof_rewrite_time_last = -1;
server.aof_rewrite_time_start = -1;
server.aof_lastbgrewrite_status = C_OK;
server.aof_delayed_fsync = 0;
server.aof_fd = -1;
server.aof_selected_db = -1;
server.aof_flush_postponed_start = 0;
server.aof_last_incr_size = 0;
server.active_defrag_running = 0;
server.notify_keyspace_events = 0;
server.blocked_clients = 0;
memset(server.blocked_clients_by_type,0,
sizeof(server.blocked_clients_by_type));
server.shutdown_asap = 0;
server.shutdown_flags = 0;
server.shutdown_mstime = 0;
server.cluster_module_flags = CLUSTER_MODULE_FLAG_NONE;
server.migrate_cached_sockets = dictCreate(&migrateCacheDictType);
server.next_client_id = 1;
server.page_size = sysconf(_SC_PAGESIZE);
server.pause_cron = 0;
server.latency_tracking_info_percentiles_len = 3;
server.latency_tracking_info_percentiles = zmalloc(sizeof(double)*(server.latency_tracking_info_percentiles_len));
server.latency_tracking_info_percentiles[0] = 50.0;
server.latency_tracking_info_percentiles[1] = 99.0;
server.latency_tracking_info_percentiles[2] = 99.9;
unsigned int lruclock = getLRUClock();
atomicSet(server.lruclock,lruclock);
resetServerSaveParams();
appendServerSaveParams(60*60,1);
appendServerSaveParams(300,100);
appendServerSaveParams(60,10000);
hdrAllocFuncs hdrallocfn = {
.mallocFn = zmalloc,
.callocFn = zcalloc_num,
.reallocFn = zrealloc,
.freeFn = zfree,
};
hdrSetAllocators(&hdrallocfn);
server.masterhost = NULL;
server.masterport = 6379;
server.master = NULL;
server.cached_master = NULL;
server.master_initial_offset = -1;
server.repl_state = REPL_STATE_NONE;
server.repl_transfer_tmpfile = NULL;
server.repl_transfer_fd = -1;
server.repl_transfer_s = NULL;
server.repl_syncio_timeout = CONFIG_REPL_SYNCIO_TIMEOUT;
server.repl_down_since = 0;
server.master_repl_offset = 0;
server.repl_backlog = NULL;
server.repl_no_slaves_since = time(NULL);
server.failover_end_time = 0;
server.force_failover = 0;
server.target_replica_host = NULL;
server.target_replica_port = 0;
server.failover_state = NO_FAILOVER;
for (j = 0; j < CLIENT_TYPE_OBUF_COUNT; j++)
server.client_obuf_limits[j] = clientBufferLimitsDefaults[j];
for (j = 0; j < CONFIG_OOM_COUNT; j++)
server.oom_score_adj_values[j] = configOOMScoreAdjValuesDefaults[j];
R_Zero = 0.0;
R_PosInf = 1.0/R_Zero;
R_NegInf = -1.0/R_Zero;
R_Nan = R_Zero/R_Zero;
server.commands = dictCreate(&commandTableDictType);
server.orig_commands = dictCreate(&commandTableDictType);
populateCommandTable();
server.watchdog_period = 0;
}
extern char **environ;
int restartServer(int flags, mstime_t delay) {
int j;
if (access(server.executable,X_OK) == -1) {
serverLog(LL_WARNING,"Can't restart: this process has no "
"permissions to execute %s", server.executable);
return C_ERR;
}
if (flags & RESTART_SERVER_CONFIG_REWRITE &&
server.configfile &&
rewriteConfig(server.configfile, 0) == -1)
{
serverLog(LL_WARNING,"Can't restart: configuration rewrite process "
"failed");
return C_ERR;
}
if (flags & RESTART_SERVER_GRACEFULLY &&
prepareForShutdown(SHUTDOWN_NOW) != C_OK)
{
serverLog(LL_WARNING,"Can't restart: error preparing for shutdown");
return C_ERR;
}
for (j = 3; j < (int)server.maxclients + 1024; j++) {
if (fcntl(j,F_GETFD) != -1) close(j);
}
if (delay) usleep(delay*1000);
zfree(server.exec_argv[0]);
server.exec_argv[0] = zstrdup(server.executable);
execve(server.executable,server.exec_argv,environ);
_exit(1);
return C_ERR;
}
int setOOMScoreAdj(int process_class) {
if (process_class == -1)
process_class = (server.masterhost ? CONFIG_OOM_REPLICA : CONFIG_OOM_MASTER);
serverAssert(process_class >= 0 && process_class < CONFIG_OOM_COUNT);
#if defined(HAVE_PROC_OOM_SCORE_ADJ)
static int oom_score_adjusted_by_redis = 0;
static int oom_score_adj_base = 0;
int fd;
int val;
char buf[64];
if (server.oom_score_adj != OOM_SCORE_ADJ_NO) {
if (!oom_score_adjusted_by_redis) {
oom_score_adjusted_by_redis = 1;
fd = open("/proc/self/oom_score_adj", O_RDONLY);
if (fd < 0 || read(fd, buf, sizeof(buf)) < 0) {
serverLog(LL_WARNING, "Unable to read oom_score_adj: %s", strerror(errno));
if (fd != -1) close(fd);
return C_ERR;
}
oom_score_adj_base = atoi(buf);
close(fd);
}
val = server.oom_score_adj_values[process_class];
if (server.oom_score_adj == OOM_SCORE_RELATIVE)
val += oom_score_adj_base;
if (val > 1000) val = 1000;
if (val < -1000) val = -1000;
} else if (oom_score_adjusted_by_redis) {
oom_score_adjusted_by_redis = 0;
val = oom_score_adj_base;
}
else {
return C_OK;
}
snprintf(buf, sizeof(buf) - 1, "%d\n", val);
fd = open("/proc/self/oom_score_adj", O_WRONLY);
if (fd < 0 || write(fd, buf, strlen(buf)) < 0) {
serverLog(LL_WARNING, "Unable to write oom_score_adj: %s", strerror(errno));
if (fd != -1) close(fd);
return C_ERR;
}
close(fd);
return C_OK;
#else
return C_ERR;
#endif
}
void adjustOpenFilesLimit(void) {
rlim_t maxfiles = server.maxclients+CONFIG_MIN_RESERVED_FDS;
struct rlimit limit;
if (getrlimit(RLIMIT_NOFILE,&limit) == -1) {
serverLog(LL_WARNING,"Unable to obtain the current NOFILE limit (%s), assuming 1024 and setting the max clients configuration accordingly.",
strerror(errno));
server.maxclients = 1024-CONFIG_MIN_RESERVED_FDS;
} else {
rlim_t oldlimit = limit.rlim_cur;
if (oldlimit < maxfiles) {
rlim_t bestlimit;
int setrlimit_error = 0;
bestlimit = maxfiles;
while(bestlimit > oldlimit) {
rlim_t decr_step = 16;
limit.rlim_cur = bestlimit;
limit.rlim_max = bestlimit;
if (setrlimit(RLIMIT_NOFILE,&limit) != -1) break;
setrlimit_error = errno;
if (bestlimit < decr_step) {
bestlimit = oldlimit;
break;
}
bestlimit -= decr_step;
}
if (bestlimit < oldlimit) bestlimit = oldlimit;
if (bestlimit < maxfiles) {
unsigned int old_maxclients = server.maxclients;
server.maxclients = bestlimit-CONFIG_MIN_RESERVED_FDS;
if (bestlimit <= CONFIG_MIN_RESERVED_FDS) {
serverLog(LL_WARNING,"Your current 'ulimit -n' "
"of %llu is not enough for the server to start. "
"Please increase your open file limit to at least "
"%llu. Exiting.",
(unsigned long long) oldlimit,
(unsigned long long) maxfiles);
exit(1);
}
serverLog(LL_WARNING,"You requested maxclients of %d "
"requiring at least %llu max file descriptors.",
old_maxclients,
(unsigned long long) maxfiles);
serverLog(LL_WARNING,"Server can't set maximum open files "
"to %llu because of OS error: %s.",
(unsigned long long) maxfiles, strerror(setrlimit_error));
serverLog(LL_WARNING,"Current maximum open files is %llu. "
"maxclients has been reduced to %d to compensate for "
"low ulimit. "
"If you need higher maxclients increase 'ulimit -n'.",
(unsigned long long) bestlimit, server.maxclients);
} else {
serverLog(LL_NOTICE,"Increased maximum number of open files "
"to %llu (it was originally set to %llu).",
(unsigned long long) maxfiles,
(unsigned long long) oldlimit);
}
}
}
}
void checkTcpBacklogSettings(void) {
#if defined(HAVE_PROC_SOMAXCONN)
FILE *fp = fopen("/proc/sys/net/core/somaxconn","r");
char buf[1024];
if (!fp) return;
if (fgets(buf,sizeof(buf),fp) != NULL) {
int somaxconn = atoi(buf);
if (somaxconn > 0 && somaxconn < server.tcp_backlog) {
serverLog(LL_WARNING,"WARNING: The TCP backlog setting of %d cannot be enforced because /proc/sys/net/core/somaxconn is set to the lower value of %d.", server.tcp_backlog, somaxconn);
}
}
fclose(fp);
#elif defined(HAVE_SYSCTL_KIPC_SOMAXCONN)
int somaxconn, mib[3];
size_t len = sizeof(int);
mib[0] = CTL_KERN;
mib[1] = KERN_IPC;
mib[2] = KIPC_SOMAXCONN;
if (sysctl(mib, 3, &somaxconn, &len, NULL, 0) == 0) {
if (somaxconn > 0 && somaxconn < server.tcp_backlog) {
serverLog(LL_WARNING,"WARNING: The TCP backlog setting of %d cannot be enforced because kern.ipc.somaxconn is set to the lower value of %d.", server.tcp_backlog, somaxconn);
}
}
#elif defined(HAVE_SYSCTL_KERN_SOMAXCONN)
int somaxconn, mib[2];
size_t len = sizeof(int);
mib[0] = CTL_KERN;
mib[1] = KERN_SOMAXCONN;
if (sysctl(mib, 2, &somaxconn, &len, NULL, 0) == 0) {
if (somaxconn > 0 && somaxconn < server.tcp_backlog) {
serverLog(LL_WARNING,"WARNING: The TCP backlog setting of %d cannot be enforced because kern.somaxconn is set to the lower value of %d.", server.tcp_backlog, somaxconn);
}
}
#elif defined(SOMAXCONN)
if (SOMAXCONN < server.tcp_backlog) {
serverLog(LL_WARNING,"WARNING: The TCP backlog setting of %d cannot be enforced because SOMAXCONN is set to the lower value of %d.", server.tcp_backlog, SOMAXCONN);
}
#endif
}
void closeSocketListeners(socketFds *sfd) {
int j;
for (j = 0; j < sfd->count; j++) {
if (sfd->fd[j] == -1) continue;
aeDeleteFileEvent(server.el, sfd->fd[j], AE_READABLE);
close(sfd->fd[j]);
}
sfd->count = 0;
}
int createSocketAcceptHandler(socketFds *sfd, aeFileProc *accept_handler) {
int j;
for (j = 0; j < sfd->count; j++) {
if (aeCreateFileEvent(server.el, sfd->fd[j], AE_READABLE, accept_handler,NULL) == AE_ERR) {
for (j = j-1; j >= 0; j--) aeDeleteFileEvent(server.el, sfd->fd[j], AE_READABLE);
return C_ERR;
}
}
return C_OK;
}
int listenToPort(int port, socketFds *sfd) {
int j;
char **bindaddr = server.bindaddr;
if (server.bindaddr_count == 0) return C_OK;
for (j = 0; j < server.bindaddr_count; j++) {
char* addr = bindaddr[j];
int optional = *addr == '-';
if (optional) addr++;
if (strchr(addr,':')) {
sfd->fd[sfd->count] = anetTcp6Server(server.neterr,port,addr,server.tcp_backlog);
} else {
sfd->fd[sfd->count] = anetTcpServer(server.neterr,port,addr,server.tcp_backlog);
}
if (sfd->fd[sfd->count] == ANET_ERR) {
int net_errno = errno;
serverLog(LL_WARNING,
"Warning: Could not create server TCP listening socket %s:%d: %s",
addr, port, server.neterr);
if (net_errno == EADDRNOTAVAIL && optional)
continue;
if (net_errno == ENOPROTOOPT || net_errno == EPROTONOSUPPORT ||
net_errno == ESOCKTNOSUPPORT || net_errno == EPFNOSUPPORT ||
net_errno == EAFNOSUPPORT)
continue;
closeSocketListeners(sfd);
return C_ERR;
}
anetNonBlock(NULL,sfd->fd[sfd->count]);
anetCloexec(sfd->fd[sfd->count]);
sfd->count++;
}
return C_OK;
}
void resetServerStats(void) {
int j;
server.stat_numcommands = 0;
server.stat_numconnections = 0;
server.stat_expiredkeys = 0;
server.stat_expired_stale_perc = 0;
server.stat_expired_time_cap_reached_count = 0;
server.stat_expire_cycle_time_used = 0;
server.stat_evictedkeys = 0;
server.stat_evictedclients = 0;
server.stat_total_eviction_exceeded_time = 0;
server.stat_last_eviction_exceeded_time = 0;
server.stat_keyspace_misses = 0;
server.stat_keyspace_hits = 0;
server.stat_active_defrag_hits = 0;
server.stat_active_defrag_misses = 0;
server.stat_active_defrag_key_hits = 0;
server.stat_active_defrag_key_misses = 0;
server.stat_active_defrag_scanned = 0;
server.stat_total_active_defrag_time = 0;
server.stat_last_active_defrag_time = 0;
server.stat_fork_time = 0;
server.stat_fork_rate = 0;
server.stat_total_forks = 0;
server.stat_rejected_conn = 0;
server.stat_sync_full = 0;
server.stat_sync_partial_ok = 0;
server.stat_sync_partial_err = 0;
server.stat_io_reads_processed = 0;
atomicSet(server.stat_total_reads_processed, 0);
server.stat_io_writes_processed = 0;
atomicSet(server.stat_total_writes_processed, 0);
for (j = 0; j < STATS_METRIC_COUNT; j++) {
server.inst_metric[j].idx = 0;
server.inst_metric[j].last_sample_time = mstime();
server.inst_metric[j].last_sample_count = 0;
memset(server.inst_metric[j].samples,0,
sizeof(server.inst_metric[j].samples));
}
atomicSet(server.stat_net_input_bytes, 0);
atomicSet(server.stat_net_output_bytes, 0);
server.stat_unexpected_error_replies = 0;
server.stat_total_error_replies = 0;
server.stat_dump_payload_sanitizations = 0;
server.aof_delayed_fsync = 0;
lazyfreeResetStats();
}
void makeThreadKillable(void) {
pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
}
void initServer(void) {
int j;
signal(SIGHUP, SIG_IGN);
signal(SIGPIPE, SIG_IGN);
setupSignalHandlers();
makeThreadKillable();
if (server.syslog_enabled) {
openlog(server.syslog_ident, LOG_PID | LOG_NDELAY | LOG_NOWAIT,
server.syslog_facility);
}
server.aof_state = server.aof_enabled ? AOF_ON : AOF_OFF;
server.hz = server.config_hz;
server.pid = getpid();
server.in_fork_child = CHILD_TYPE_NONE;
server.main_thread_id = pthread_self();
server.current_client = NULL;
server.errors = raxNew();
server.fixed_time_expire = 0;
server.in_nested_call = 0;
server.clients = listCreate();
server.clients_index = raxNew();
server.clients_to_close = listCreate();
server.slaves = listCreate();
server.monitors = listCreate();
server.clients_pending_write = listCreate();
server.clients_pending_read = listCreate();
server.clients_timeout_table = raxNew();
server.replication_allowed = 1;
server.slaveseldb = -1;
server.unblocked_clients = listCreate();
server.ready_keys = listCreate();
server.tracking_pending_keys = listCreate();
server.clients_waiting_acks = listCreate();
server.get_ack_from_slaves = 0;
server.client_pause_type = CLIENT_PAUSE_OFF;
server.client_pause_end_time = 0;
memset(server.client_pause_per_purpose, 0,
sizeof(server.client_pause_per_purpose));
server.postponed_clients = listCreate();
server.events_processed_while_blocked = 0;
server.system_memory_size = zmalloc_get_memory_size();
server.blocked_last_cron = 0;
server.blocking_op_nesting = 0;
server.thp_enabled = 0;
server.cluster_drop_packet_filter = -1;
resetReplicationBuffer();
if ((server.tls_port || server.tls_replication || server.tls_cluster)
&& tlsConfigure(&server.tls_ctx_config) == C_ERR) {
serverLog(LL_WARNING, "Failed to configure TLS. Check logs for more info.");
exit(1);
}
for (j = 0; j < CLIENT_MEM_USAGE_BUCKETS; j++) {
server.client_mem_usage_buckets[j].mem_usage_sum = 0;
server.client_mem_usage_buckets[j].clients = listCreate();
}
createSharedObjects();
adjustOpenFilesLimit();
const char *clk_msg = monotonicInit();
serverLog(LL_NOTICE, "monotonic clock: %s", clk_msg);
server.el = aeCreateEventLoop(server.maxclients+CONFIG_FDSET_INCR);
if (server.el == NULL) {
serverLog(LL_WARNING,
"Failed creating the event loop. Error message: '%s'",
strerror(errno));
exit(1);
}
server.db = zmalloc(sizeof(redisDb)*server.dbnum);
if (server.port != 0 &&
listenToPort(server.port,&server.ipfd) == C_ERR) {
serverLog(LL_WARNING, "Failed listening on port %u (TCP), aborting.", server.port);
exit(1);
}
if (server.tls_port != 0 &&
listenToPort(server.tls_port,&server.tlsfd) == C_ERR) {
serverLog(LL_WARNING, "Failed listening on port %u (TLS), aborting.", server.tls_port);
exit(1);
}
if (server.unixsocket != NULL) {
unlink(server.unixsocket);
server.sofd = anetUnixServer(server.neterr,server.unixsocket,
(mode_t)server.unixsocketperm, server.tcp_backlog);
if (server.sofd == ANET_ERR) {
serverLog(LL_WARNING, "Failed opening Unix socket: %s", server.neterr);
exit(1);
}
anetNonBlock(NULL,server.sofd);
anetCloexec(server.sofd);
}
if (server.ipfd.count == 0 && server.tlsfd.count == 0 && server.sofd < 0) {
serverLog(LL_WARNING, "Configured to not listen anywhere, exiting.");
exit(1);
}
for (j = 0; j < server.dbnum; j++) {
server.db[j].dict = dictCreate(&dbDictType);
server.db[j].expires = dictCreate(&dbExpiresDictType);
server.db[j].expires_cursor = 0;
server.db[j].blocking_keys = dictCreate(&keylistDictType);
server.db[j].ready_keys = dictCreate(&objectKeyPointerValueDictType);
server.db[j].watched_keys = dictCreate(&keylistDictType);
server.db[j].id = j;
server.db[j].avg_ttl = 0;
server.db[j].defrag_later = listCreate();
server.db[j].slots_to_keys = NULL;
listSetFreeMethod(server.db[j].defrag_later,(void (*)(void*))sdsfree);
}
evictionPoolAlloc();
server.pubsub_channels = dictCreate(&keylistDictType);
server.pubsub_patterns = dictCreate(&keylistDictType);
server.pubsubshard_channels = dictCreate(&keylistDictType);
server.cronloops = 0;
server.in_script = 0;
server.in_exec = 0;
server.busy_module_yield_flags = BUSY_MODULE_YIELD_NONE;
server.busy_module_yield_reply = NULL;
server.core_propagates = 0;
server.propagate_no_multi = 0;
server.module_ctx_nesting = 0;
server.client_pause_in_transaction = 0;
server.child_pid = -1;
server.child_type = CHILD_TYPE_NONE;
server.rdb_child_type = RDB_CHILD_TYPE_NONE;
server.rdb_pipe_conns = NULL;
server.rdb_pipe_numconns = 0;
server.rdb_pipe_numconns_writing = 0;
server.rdb_pipe_buff = NULL;
server.rdb_pipe_bufflen = 0;
server.rdb_bgsave_scheduled = 0;
server.child_info_pipe[0] = -1;
server.child_info_pipe[1] = -1;
server.child_info_nread = 0;
server.aof_buf = sdsempty();
server.lastsave = time(NULL);
server.lastbgsave_try = 0;
server.rdb_save_time_last = -1;
server.rdb_save_time_start = -1;
server.rdb_last_load_keys_expired = 0;
server.rdb_last_load_keys_loaded = 0;
server.dirty = 0;
resetServerStats();
server.stat_starttime = time(NULL);
server.stat_peak_memory = 0;
server.stat_current_cow_peak = 0;
server.stat_current_cow_bytes = 0;
server.stat_current_cow_updated = 0;
server.stat_current_save_keys_processed = 0;
server.stat_current_save_keys_total = 0;
server.stat_rdb_cow_bytes = 0;
server.stat_aof_cow_bytes = 0;
server.stat_module_cow_bytes = 0;
server.stat_module_progress = 0;
for (int j = 0; j < CLIENT_TYPE_COUNT; j++)
server.stat_clients_type_memory[j] = 0;
server.stat_cluster_links_memory = 0;
server.cron_malloc_stats.zmalloc_used = 0;
server.cron_malloc_stats.process_rss = 0;
server.cron_malloc_stats.allocator_allocated = 0;
server.cron_malloc_stats.allocator_active = 0;
server.cron_malloc_stats.allocator_resident = 0;
server.lastbgsave_status = C_OK;
server.aof_last_write_status = C_OK;
server.aof_last_write_errno = 0;
server.repl_good_slaves_count = 0;
if (aeCreateTimeEvent(server.el, 1, serverCron, NULL, NULL) == AE_ERR) {
serverPanic("Can't create event loop timers.");
exit(1);
}
if (createSocketAcceptHandler(&server.ipfd, acceptTcpHandler) != C_OK) {
serverPanic("Unrecoverable error creating TCP socket accept handler.");
}
if (createSocketAcceptHandler(&server.tlsfd, acceptTLSHandler) != C_OK) {
serverPanic("Unrecoverable error creating TLS socket accept handler.");
}
if (server.sofd > 0 && aeCreateFileEvent(server.el,server.sofd,AE_READABLE,
acceptUnixHandler,NULL) == AE_ERR) serverPanic("Unrecoverable error creating server.sofd file event.");
if (aeCreateFileEvent(server.el, server.module_pipe[0], AE_READABLE,
modulePipeReadable,NULL) == AE_ERR) {
serverPanic(
"Error registering the readable event for the module pipe.");
}
aeSetBeforeSleepProc(server.el,beforeSleep);
aeSetAfterSleepProc(server.el,afterSleep);
if (server.arch_bits == 32 && server.maxmemory == 0) {
serverLog(LL_WARNING,"Warning: 32 bit instance detected but no memory limit set. Setting 3 GB maxmemory limit with 'noeviction' policy now.");
server.maxmemory = 3072LL*(1024*1024);
server.maxmemory_policy = MAXMEMORY_NO_EVICTION;
}
if (server.cluster_enabled) clusterInit();
scriptingInit(1);
functionsInit();
slowlogInit();
latencyMonitorInit();
ACLUpdateDefaultUserPassword(server.requirepass);
applyWatchdogPeriod();
}
void InitServerLast() {
bioInit();
initThreadedIO();
set_jemalloc_bg_thread(server.jemalloc_bg_thread);
server.initial_memory_usage = zmalloc_used_memory();
}
void populateCommandLegacyRangeSpec(struct redisCommand *c) {
memset(&c->legacy_range_key_spec, 0, sizeof(c->legacy_range_key_spec));
if (c->key_specs_num == 0)
return;
if (c->key_specs_num == 1 &&
c->key_specs[0].begin_search_type == KSPEC_BS_INDEX &&
c->key_specs[0].find_keys_type == KSPEC_FK_RANGE)
{
c->legacy_range_key_spec = c->key_specs[0];
return;
}
int firstkey = INT_MAX, lastkey = 0;
int prev_lastkey = 0;
for (int i = 0; i < c->key_specs_num; i++) {
if (c->key_specs[i].begin_search_type != KSPEC_BS_INDEX ||
c->key_specs[i].find_keys_type != KSPEC_FK_RANGE)
continue;
if (c->key_specs[i].fk.range.keystep != 1)
return;
if (prev_lastkey && prev_lastkey != c->key_specs[i].bs.index.pos-1)
return;
firstkey = min(firstkey, c->key_specs[i].bs.index.pos);
int lastkey_abs_index = c->key_specs[i].fk.range.lastkey;
if (lastkey_abs_index >= 0)
lastkey_abs_index += c->key_specs[i].bs.index.pos;
lastkey = max((unsigned)lastkey, (unsigned)lastkey_abs_index);
}
if (firstkey == INT_MAX)
return;
serverAssert(firstkey != 0);
serverAssert(lastkey != 0);
c->legacy_range_key_spec.begin_search_type = KSPEC_BS_INDEX;
c->legacy_range_key_spec.bs.index.pos = firstkey;
c->legacy_range_key_spec.find_keys_type = KSPEC_FK_RANGE;
c->legacy_range_key_spec.fk.range.lastkey = lastkey < 0 ? lastkey : (lastkey-firstkey);
c->legacy_range_key_spec.fk.range.keystep = 1;
c->legacy_range_key_spec.fk.range.limit = 0;
}
sds catSubCommandFullname(const char *parent_name, const char *sub_name) {
return sdscatfmt(sdsempty(), "%s|%s", parent_name, sub_name);
}
void commandAddSubcommand(struct redisCommand *parent, struct redisCommand *subcommand, const char *declared_name) {
if (!parent->subcommands_dict)
parent->subcommands_dict = dictCreate(&commandTableDictType);
subcommand->parent = parent;
subcommand->id = ACLGetCommandID(subcommand->fullname);
serverAssert(dictAdd(parent->subcommands_dict, sdsnew(declared_name), subcommand) == DICT_OK);
}
void setImplicitACLCategories(struct redisCommand *c) {
if (c->flags & CMD_WRITE)
c->acl_categories |= ACL_CATEGORY_WRITE;
if (c->flags & CMD_READONLY)
c->acl_categories |= ACL_CATEGORY_READ;
if (c->flags & CMD_ADMIN)
c->acl_categories |= ACL_CATEGORY_ADMIN|ACL_CATEGORY_DANGEROUS;
if (c->flags & CMD_PUBSUB)
c->acl_categories |= ACL_CATEGORY_PUBSUB;
if (c->flags & CMD_FAST)
c->acl_categories |= ACL_CATEGORY_FAST;
if (c->flags & CMD_BLOCKING)
c->acl_categories |= ACL_CATEGORY_BLOCKING;
if (!(c->acl_categories & ACL_CATEGORY_FAST))
c->acl_categories |= ACL_CATEGORY_SLOW;
}
int populateArgsStructure(struct redisCommandArg *args) {
if (!args)
return 0;
int count = 0;
while (args->name) {
args->num_args = populateArgsStructure(args->subargs);
count++;
args++;
}
return count;
}
void populateCommandStructure(struct redisCommand *c) {
c->key_specs = c->key_specs_static;
c->key_specs_max = STATIC_KEY_SPECS_NUM;
c->latency_histogram = NULL;
for (int i = 0; i < STATIC_KEY_SPECS_NUM; i++) {
if (c->key_specs[i].begin_search_type == KSPEC_BS_INVALID)
break;
c->key_specs_num++;
}
while (c->history && c->history[c->num_history].since)
c->num_history++;
while (c->tips && c->tips[c->num_tips])
c->num_tips++;
c->num_args = populateArgsStructure(c->args);
populateCommandLegacyRangeSpec(c);
populateCommandMovableKeys(c);
c->id = ACLGetCommandID(c->fullname);
if (c->subcommands) {
for (int j = 0; c->subcommands[j].declared_name; j++) {
struct redisCommand *sub = c->subcommands+j;
setImplicitACLCategories(sub);
sub->fullname = catSubCommandFullname(c->declared_name, sub->declared_name);
populateCommandStructure(sub);
commandAddSubcommand(c, sub, sub->declared_name);
}
}
}
extern struct redisCommand redisCommandTable[];
void populateCommandTable(void) {
int j;
struct redisCommand *c;
for (j = 0;; j++) {
c = redisCommandTable + j;
if (c->declared_name == NULL)
break;
int retval1, retval2;
setImplicitACLCategories(c);
if (!(c->flags & CMD_SENTINEL) && server.sentinel_mode)
continue;
if (c->flags & CMD_ONLY_SENTINEL && !server.sentinel_mode)
continue;
c->fullname = sdsnew(c->declared_name);
populateCommandStructure(c);
retval1 = dictAdd(server.commands, sdsdup(c->fullname), c);
retval2 = dictAdd(server.orig_commands, sdsdup(c->fullname), c);
serverAssert(retval1 == DICT_OK && retval2 == DICT_OK);
}
}
void resetCommandTableStats(dict* commands) {
struct redisCommand *c;
dictEntry *de;
dictIterator *di;
di = dictGetSafeIterator(commands);
while((de = dictNext(di)) != NULL) {
c = (struct redisCommand *) dictGetVal(de);
c->microseconds = 0;
c->calls = 0;
c->rejected_calls = 0;
c->failed_calls = 0;
if(c->latency_histogram) {
hdr_close(c->latency_histogram);
c->latency_histogram = NULL;
}
if (c->subcommands_dict)
resetCommandTableStats(c->subcommands_dict);
}
dictReleaseIterator(di);
}
void resetErrorTableStats(void) {
raxFreeWithCallback(server.errors, zfree);
server.errors = raxNew();
}
void redisOpArrayInit(redisOpArray *oa) {
oa->ops = NULL;
oa->numops = 0;
oa->capacity = 0;
}
int redisOpArrayAppend(redisOpArray *oa, int dbid, robj **argv, int argc, int target) {
redisOp *op;
int prev_capacity = oa->capacity;
if (oa->numops == 0) {
oa->capacity = 16;
} else if (oa->numops >= oa->capacity) {
oa->capacity *= 2;
}
if (prev_capacity != oa->capacity)
oa->ops = zrealloc(oa->ops,sizeof(redisOp)*oa->capacity);
op = oa->ops+oa->numops;
op->dbid = dbid;
op->argv = argv;
op->argc = argc;
op->target = target;
oa->numops++;
return oa->numops;
}
void redisOpArrayFree(redisOpArray *oa) {
while(oa->numops) {
int j;
redisOp *op;
oa->numops--;
op = oa->ops+oa->numops;
for (j = 0; j < op->argc; j++)
decrRefCount(op->argv[j]);
zfree(op->argv);
}
zfree(oa->ops);
redisOpArrayInit(oa);
}
int isContainerCommandBySds(sds s) {
struct redisCommand *base_cmd = dictFetchValue(server.commands, s);
int has_subcommands = base_cmd && base_cmd->subcommands_dict;
return has_subcommands;
}
struct redisCommand *lookupSubcommand(struct redisCommand *container, sds sub_name) {
return dictFetchValue(container->subcommands_dict, sub_name);
}
struct redisCommand *lookupCommandLogic(dict *commands, robj **argv, int argc, int strict) {
struct redisCommand *base_cmd = dictFetchValue(commands, argv[0]->ptr);
int has_subcommands = base_cmd && base_cmd->subcommands_dict;
if (argc == 1 || !has_subcommands) {
if (strict && argc != 1)
return NULL;
return base_cmd;
} else {
if (strict && argc != 2)
return NULL;
return lookupSubcommand(base_cmd, argv[1]->ptr);
}
}
struct redisCommand *lookupCommand(robj **argv, int argc) {
return lookupCommandLogic(server.commands,argv,argc,0);
}
struct redisCommand *lookupCommandBySdsLogic(dict *commands, sds s) {
int argc, j;
sds *strings = sdssplitlen(s,sdslen(s),"|",1,&argc);
if (strings == NULL)
return NULL;
if (argc > 2) {
sdsfreesplitres(strings,argc);
return NULL;
}
robj objects[argc];
robj *argv[argc];
for (j = 0; j < argc; j++) {
initStaticStringObject(objects[j],strings[j]);
argv[j] = &objects[j];
}
struct redisCommand *cmd = lookupCommandLogic(commands,argv,argc,1);
sdsfreesplitres(strings,argc);
return cmd;
}
struct redisCommand *lookupCommandBySds(sds s) {
return lookupCommandBySdsLogic(server.commands,s);
}
struct redisCommand *lookupCommandByCStringLogic(dict *commands, const char *s) {
struct redisCommand *cmd;
sds name = sdsnew(s);
cmd = lookupCommandBySdsLogic(commands,name);
sdsfree(name);
return cmd;
}
struct redisCommand *lookupCommandByCString(const char *s) {
return lookupCommandByCStringLogic(server.commands,s);
}
struct redisCommand *lookupCommandOrOriginal(robj **argv ,int argc) {
struct redisCommand *cmd = lookupCommandLogic(server.commands, argv, argc, 0);
if (!cmd) cmd = lookupCommandLogic(server.orig_commands, argv, argc, 0);
return cmd;
}
static int shouldPropagate(int target) {
if (!server.replication_allowed || target == PROPAGATE_NONE || server.loading)
return 0;
if (target & PROPAGATE_AOF) {
if (server.aof_state != AOF_OFF)
return 1;
}
if (target & PROPAGATE_REPL) {
if (server.masterhost == NULL && (server.repl_backlog || listLength(server.slaves) != 0))
return 1;
}
return 0;
}
static void propagateNow(int dbid, robj **argv, int argc, int target) {
if (!shouldPropagate(target))
return;
serverAssert(!(areClientsPaused() && !server.client_pause_in_transaction));
if (server.aof_state != AOF_OFF && target & PROPAGATE_AOF)
feedAppendOnlyFile(dbid,argv,argc);
if (target & PROPAGATE_REPL)
replicationFeedSlaves(server.slaves,dbid,argv,argc);
}
void alsoPropagate(int dbid, robj **argv, int argc, int target) {
robj **argvcopy;
int j;
if (!shouldPropagate(target))
return;
argvcopy = zmalloc(sizeof(robj*)*argc);
for (j = 0; j < argc; j++) {
argvcopy[j] = argv[j];
incrRefCount(argv[j]);
}
redisOpArrayAppend(&server.also_propagate,dbid,argvcopy,argc,target);
}
void forceCommandPropagation(client *c, int flags) {
serverAssert(c->cmd->flags & (CMD_WRITE | CMD_MAY_REPLICATE));
if (flags & PROPAGATE_REPL) c->flags |= CLIENT_FORCE_REPL;
if (flags & PROPAGATE_AOF) c->flags |= CLIENT_FORCE_AOF;
}
void preventCommandPropagation(client *c) {
c->flags |= CLIENT_PREVENT_PROP;
}
void preventCommandAOF(client *c) {
c->flags |= CLIENT_PREVENT_AOF_PROP;
}
void preventCommandReplication(client *c) {
c->flags |= CLIENT_PREVENT_REPL_PROP;
}
void slowlogPushCurrentCommand(client *c, struct redisCommand *cmd, ustime_t duration) {
if (cmd->flags & CMD_SKIP_SLOWLOG)
return;
robj **argv = c->original_argv ? c->original_argv : c->argv;
int argc = c->original_argv ? c->original_argc : c->argc;
slowlogPushEntryIfNeeded(c,argv,argc,duration);
}
void updateCommandLatencyHistogram(struct hdr_histogram **latency_histogram, int64_t duration_hist){
if (duration_hist < LATENCY_HISTOGRAM_MIN_VALUE)
duration_hist=LATENCY_HISTOGRAM_MIN_VALUE;
if (duration_hist>LATENCY_HISTOGRAM_MAX_VALUE)
duration_hist=LATENCY_HISTOGRAM_MAX_VALUE;
if (*latency_histogram==NULL)
hdr_init(LATENCY_HISTOGRAM_MIN_VALUE,LATENCY_HISTOGRAM_MAX_VALUE,LATENCY_HISTOGRAM_PRECISION,latency_histogram);
hdr_record_value(*latency_histogram,duration_hist);
}
void propagatePendingCommands() {
if (server.also_propagate.numops == 0)
return;
int j;
redisOp *rop;
int multi_emitted = 0;
if (server.also_propagate.numops > 1 && !server.propagate_no_multi) {
int multi_dbid = server.also_propagate.ops[0].dbid;
propagateNow(multi_dbid,&shared.multi,1,PROPAGATE_AOF|PROPAGATE_REPL);
multi_emitted = 1;
}
for (j = 0; j < server.also_propagate.numops; j++) {
rop = &server.also_propagate.ops[j];
serverAssert(rop->target);
propagateNow(rop->dbid,rop->argv,rop->argc,rop->target);
}
if (multi_emitted) {
int exec_dbid = server.also_propagate.ops[server.also_propagate.numops-1].dbid;
propagateNow(exec_dbid,&shared.exec,1,PROPAGATE_AOF|PROPAGATE_REPL);
}
redisOpArrayFree(&server.also_propagate);
}
void call(client *c, int flags) {
long long dirty;
monotime call_timer;
uint64_t client_old_flags = c->flags;
struct redisCommand *real_cmd = c->cmd;
static long long prev_err_count;
c->flags &= ~(CLIENT_FORCE_AOF|CLIENT_FORCE_REPL|CLIENT_PREVENT_PROP);
int prev_core_propagates = server.core_propagates;
if (!server.core_propagates && !(flags & CMD_CALL_FROM_MODULE))
server.core_propagates = 1;
dirty = server.dirty;
prev_err_count = server.stat_total_error_replies;
if (server.fixed_time_expire++ == 0) {
updateCachedTime(0);
}
server.in_nested_call++;
elapsedStart(&call_timer);
c->cmd->proc(c);
const long duration = elapsedUs(call_timer);
c->duration = duration;
dirty = server.dirty-dirty;
if (dirty < 0) dirty = 0;
server.in_nested_call--;
if ((server.stat_total_error_replies - prev_err_count) > 0) {
real_cmd->failed_calls++;
}
if (c->flags & CLIENT_CLOSE_AFTER_COMMAND) {
c->flags &= ~CLIENT_CLOSE_AFTER_COMMAND;
c->flags |= CLIENT_CLOSE_AFTER_REPLY;
}
if (server.loading && c->flags & CLIENT_SCRIPT)
flags &= ~(CMD_CALL_SLOWLOG | CMD_CALL_STATS);
if (c->flags & CLIENT_SCRIPT && server.script_caller) {
if (c->flags & CLIENT_FORCE_REPL)
server.script_caller->flags |= CLIENT_FORCE_REPL;
if (c->flags & CLIENT_FORCE_AOF)
server.script_caller->flags |= CLIENT_FORCE_AOF;
}
if (flags & CMD_CALL_SLOWLOG) {
char *latency_event = (real_cmd->flags & CMD_FAST) ?
"fast-command" : "command";
latencyAddSampleIfNeeded(latency_event,duration/1000);
}
if ((flags & CMD_CALL_SLOWLOG) && !(c->flags & CLIENT_BLOCKED))
slowlogPushCurrentCommand(c, real_cmd, duration);
if (!(c->cmd->flags & (CMD_SKIP_MONITOR|CMD_ADMIN))) {
robj **argv = c->original_argv ? c->original_argv : c->argv;
int argc = c->original_argv ? c->original_argc : c->argc;
replicationFeedMonitors(c,server.monitors,c->db->id,argv,argc);
}
if (!(c->flags & CLIENT_BLOCKED))
freeClientOriginalArgv(c);
if (flags & CMD_CALL_STATS) {
real_cmd->microseconds += duration;
real_cmd->calls++;
if (server.latency_tracking_enabled && !(c->flags & CLIENT_BLOCKED))
updateCommandLatencyHistogram(&(real_cmd->latency_histogram), duration*1000);
}
if (flags & CMD_CALL_PROPAGATE &&
(c->flags & CLIENT_PREVENT_PROP) != CLIENT_PREVENT_PROP &&
c->cmd->proc != execCommand &&
!(c->cmd->flags & CMD_MODULE))
{
int propagate_flags = PROPAGATE_NONE;
if (dirty) propagate_flags |= (PROPAGATE_AOF|PROPAGATE_REPL);
if (c->flags & CLIENT_FORCE_REPL) propagate_flags |= PROPAGATE_REPL;
if (c->flags & CLIENT_FORCE_AOF) propagate_flags |= PROPAGATE_AOF;
if (c->flags & CLIENT_PREVENT_REPL_PROP ||
!(flags & CMD_CALL_PROPAGATE_REPL))
propagate_flags &= ~PROPAGATE_REPL;
if (c->flags & CLIENT_PREVENT_AOF_PROP ||
!(flags & CMD_CALL_PROPAGATE_AOF))
propagate_flags &= ~PROPAGATE_AOF;
if (propagate_flags != PROPAGATE_NONE)
alsoPropagate(c->db->id,c->argv,c->argc,propagate_flags);
}
c->flags &= ~(CLIENT_FORCE_AOF|CLIENT_FORCE_REPL|CLIENT_PREVENT_PROP);
c->flags |= client_old_flags &
(CLIENT_FORCE_AOF|CLIENT_FORCE_REPL|CLIENT_PREVENT_PROP);
if (c->cmd->flags & CMD_READONLY) {
client *caller = (c->flags & CLIENT_SCRIPT && server.script_caller) ?
server.script_caller : c;
if (caller->flags & CLIENT_TRACKING &&
!(caller->flags & CLIENT_TRACKING_BCAST))
{
trackingRememberKeys(caller);
}
}
server.fixed_time_expire--;
server.stat_numcommands++;
prev_err_count = server.stat_total_error_replies;
size_t zmalloc_used = zmalloc_used_memory();
if (zmalloc_used > server.stat_peak_memory)
server.stat_peak_memory = zmalloc_used;
afterCommand(c);
if (!server.in_exec && server.client_pause_in_transaction) {
server.client_pause_in_transaction = 0;
}
server.core_propagates = prev_core_propagates;
}
void rejectCommand(client *c, robj *reply) {
flagTransaction(c);
if (c->cmd) c->cmd->rejected_calls++;
if (c->cmd && c->cmd->proc == execCommand) {
execCommandAbort(c, reply->ptr);
} else {
addReplyErrorObject(c, reply);
}
}
void rejectCommandFormat(client *c, const char *fmt, ...) {
if (c->cmd) c->cmd->rejected_calls++;
flagTransaction(c);
va_list ap;
va_start(ap,fmt);
sds s = sdscatvprintf(sdsempty(),fmt,ap);
va_end(ap);
sdsmapchars(s, "\r\n", " ", 2);
if (c->cmd && c->cmd->proc == execCommand) {
execCommandAbort(c, s);
sdsfree(s);
} else {
addReplyErrorSds(c, s);
}
}
void afterCommand(client *c) {
UNUSED(c);
if (!server.in_nested_call) {
if (server.core_propagates)
propagatePendingCommands();
trackingHandlePendingKeyInvalidations();
}
}
void populateCommandMovableKeys(struct redisCommand *cmd) {
int movablekeys = 0;
if (cmd->getkeys_proc && !(cmd->flags & CMD_MODULE)) {
movablekeys = 1;
} else if (cmd->flags & CMD_MODULE_GETKEYS) {
movablekeys = 1;
} else {
for (int i = 0; i < cmd->key_specs_num; i++) {
if (cmd->key_specs[i].begin_search_type != KSPEC_BS_INDEX ||
cmd->key_specs[i].find_keys_type != KSPEC_FK_RANGE)
{
movablekeys = 1;
break;
}
}
}
if (movablekeys)
cmd->flags |= CMD_MOVABLE_KEYS;
}
int processCommand(client *c) {
if (!scriptIsTimedout()) {
serverAssert(!server.in_exec);
serverAssert(!server.in_script);
}
moduleCallCommandFilters(c);
if (!strcasecmp(c->argv[0]->ptr,"host:") || !strcasecmp(c->argv[0]->ptr,"post")) {
securityWarningCommand(c);
return C_ERR;
}
if (server.busy_module_yield_flags != BUSY_MODULE_YIELD_NONE &&
!(server.busy_module_yield_flags & BUSY_MODULE_YIELD_CLIENTS))
{
c->bpop.timeout = 0;
blockClient(c,BLOCKED_POSTPONE);
return C_OK;
}
c->cmd = c->lastcmd = lookupCommand(c->argv,c->argc);
if (!c->cmd) {
if (isContainerCommandBySds(c->argv[0]->ptr)) {
sds cmd = sdsnew((char *)c->argv[0]->ptr);
sdstoupper(cmd);
rejectCommandFormat(c, "Unknown subcommand '%.128s'. Try %s HELP.",
(char *)c->argv[1]->ptr, cmd);
sdsfree(cmd);
return C_OK;
}
sds args = sdsempty();
int i;
for (i=1; i < c->argc && sdslen(args) < 128; i++)
args = sdscatprintf(args, "'%.*s' ", 128-(int)sdslen(args), (char*)c->argv[i]->ptr);
rejectCommandFormat(c,"unknown command '%s', with args beginning with: %s",
(char*)c->argv[0]->ptr, args);
sdsfree(args);
return C_OK;
} else if ((c->cmd->arity > 0 && c->cmd->arity != c->argc) ||
(c->argc < -c->cmd->arity))
{
rejectCommandFormat(c,"wrong number of arguments for '%s' command", c->cmd->fullname);
return C_OK;
}
if (c->cmd->flags & CMD_PROTECTED) {
if ((c->cmd->proc == debugCommand && !allowProtectedAction(server.enable_debug_cmd, c)) ||
(c->cmd->proc == moduleCommand && !allowProtectedAction(server.enable_module_cmd, c)))
{
rejectCommandFormat(c,"%s command not allowed. If the %s option is set to \"local\", "
"you can run it from a local connection, otherwise you need to set this option "
"in the configuration file, and then restart the server.",
c->cmd->proc == debugCommand ? "DEBUG" : "MODULE",
c->cmd->proc == debugCommand ? "enable-debug-command" : "enable-module-command");
return C_OK;
}
}
int is_read_command = (c->cmd->flags & CMD_READONLY) ||
(c->cmd->proc == execCommand && (c->mstate.cmd_flags & CMD_READONLY));
int is_write_command = (c->cmd->flags & CMD_WRITE) ||
(c->cmd->proc == execCommand && (c->mstate.cmd_flags & CMD_WRITE));
int is_denyoom_command = (c->cmd->flags & CMD_DENYOOM) ||
(c->cmd->proc == execCommand && (c->mstate.cmd_flags & CMD_DENYOOM));
int is_denystale_command = !(c->cmd->flags & CMD_STALE) ||
(c->cmd->proc == execCommand && (c->mstate.cmd_inv_flags & CMD_STALE));
int is_denyloading_command = !(c->cmd->flags & CMD_LOADING) ||
(c->cmd->proc == execCommand && (c->mstate.cmd_inv_flags & CMD_LOADING));
int is_may_replicate_command = (c->cmd->flags & (CMD_WRITE | CMD_MAY_REPLICATE)) ||
(c->cmd->proc == execCommand && (c->mstate.cmd_flags & (CMD_WRITE | CMD_MAY_REPLICATE)));
int is_deny_async_loading_command = (c->cmd->flags & CMD_NO_ASYNC_LOADING) ||
(c->cmd->proc == execCommand && (c->mstate.cmd_flags & CMD_NO_ASYNC_LOADING));
if (authRequired(c)) {
if (!(c->cmd->flags & CMD_NO_AUTH)) {
rejectCommand(c,shared.noautherr);
return C_OK;
}
}
if (c->flags & CLIENT_MULTI && c->cmd->flags & CMD_NO_MULTI) {
rejectCommandFormat(c,"Command not allowed inside a transaction");
return C_OK;
}
int acl_errpos;
int acl_retval = ACLCheckAllPerm(c,&acl_errpos);
if (acl_retval != ACL_OK) {
addACLLogEntry(c,acl_retval,(c->flags & CLIENT_MULTI) ? ACL_LOG_CTX_MULTI : ACL_LOG_CTX_TOPLEVEL,acl_errpos,NULL,NULL);
switch (acl_retval) {
case ACL_DENIED_CMD:
{
rejectCommandFormat(c,
"-NOPERM this user has no permissions to run "
"the '%s' command", c->cmd->fullname);
break;
}
case ACL_DENIED_KEY:
rejectCommandFormat(c,
"-NOPERM this user has no permissions to access "
"one of the keys used as arguments");
break;
case ACL_DENIED_CHANNEL:
rejectCommandFormat(c,
"-NOPERM this user has no permissions to access "
"one of the channels used as arguments");
break;
default:
rejectCommandFormat(c, "no permission");
break;
}
return C_OK;
}
if (server.cluster_enabled &&
!(c->flags & CLIENT_MASTER) &&
!(c->flags & CLIENT_SCRIPT &&
server.script_caller->flags & CLIENT_MASTER) &&
!(!(c->cmd->flags&CMD_MOVABLE_KEYS) && c->cmd->key_specs_num == 0 &&
c->cmd->proc != execCommand))
{
int hashslot;
int error_code;
clusterNode *n = getNodeByQuery(c,c->cmd,c->argv,c->argc,
&hashslot,&error_code);
if (n == NULL || n != server.cluster->myself) {
if (c->cmd->proc == execCommand) {
discardTransaction(c);
} else {
flagTransaction(c);
}
clusterRedirectClient(c,n,hashslot,error_code);
c->cmd->rejected_calls++;
return C_OK;
}
}
evictClients();
if (server.current_client == NULL) {
return C_ERR;
}
if (server.maxmemory && !scriptIsTimedout()) {
int out_of_memory = (performEvictions() == EVICT_FAIL);
trackingHandlePendingKeyInvalidations();
if (server.current_client == NULL) return C_ERR;
int reject_cmd_on_oom = is_denyoom_command;
if (c->flags & CLIENT_MULTI &&
c->cmd->proc != execCommand &&
c->cmd->proc != discardCommand &&
c->cmd->proc != quitCommand &&
c->cmd->proc != resetCommand) {
reject_cmd_on_oom = 1;
}
if (out_of_memory && reject_cmd_on_oom) {
rejectCommand(c, shared.oomerr);
return C_OK;
}
if (c->cmd->proc == evalCommand ||
c->cmd->proc == evalShaCommand ||
c->cmd->proc == fcallCommand ||
c->cmd->proc == fcallroCommand)
{
server.script_oom = out_of_memory;
}
}
if (server.tracking_clients) trackingLimitUsedSlots();
int deny_write_type = writeCommandsDeniedByDiskError();
if (deny_write_type != DISK_ERROR_TYPE_NONE &&
server.masterhost == NULL &&
(is_write_command ||c->cmd->proc == pingCommand))
{
if (deny_write_type == DISK_ERROR_TYPE_RDB)
rejectCommand(c, shared.bgsaveerr);
else
rejectCommandFormat(c,
"-MISCONF Errors writing to the AOF file: %s",
strerror(server.aof_last_write_errno));
return C_OK;
}
if (server.masterhost == NULL &&
server.repl_min_slaves_to_write &&
server.repl_min_slaves_max_lag &&
is_write_command &&
server.repl_good_slaves_count < server.repl_min_slaves_to_write)
{
rejectCommand(c, shared.noreplicaserr);
return C_OK;
}
if (server.masterhost && server.repl_slave_ro &&
!(c->flags & CLIENT_MASTER) &&
is_write_command)
{
rejectCommand(c, shared.roslaveerr);
return C_OK;
}
if ((c->flags & CLIENT_PUBSUB && c->resp == 2) &&
c->cmd->proc != pingCommand &&
c->cmd->proc != subscribeCommand &&
c->cmd->proc != ssubscribeCommand &&
c->cmd->proc != unsubscribeCommand &&
c->cmd->proc != sunsubscribeCommand &&
c->cmd->proc != psubscribeCommand &&
c->cmd->proc != punsubscribeCommand &&
c->cmd->proc != quitCommand &&
c->cmd->proc != resetCommand) {
rejectCommandFormat(c,
"Can't execute '%s': only (P|S)SUBSCRIBE / "
"(P|S)UNSUBSCRIBE / PING / QUIT / RESET are allowed in this context",
c->cmd->fullname);
return C_OK;
}
if (server.masterhost && server.repl_state != REPL_STATE_CONNECTED &&
server.repl_serve_stale_data == 0 &&
is_denystale_command)
{
rejectCommand(c, shared.masterdownerr);
return C_OK;
}
if (server.loading && !server.async_loading && is_denyloading_command) {
rejectCommand(c, shared.loadingerr);
return C_OK;
}
if (server.async_loading && is_deny_async_loading_command) {
rejectCommand(c,shared.loadingerr);
return C_OK;
}
if ((scriptIsTimedout() || server.busy_module_yield_flags) && !(c->cmd->flags & CMD_ALLOW_BUSY)) {
if (server.busy_module_yield_flags && server.busy_module_yield_reply) {
rejectCommandFormat(c, "-BUSY %s", server.busy_module_yield_reply);
} else if (server.busy_module_yield_flags) {
rejectCommand(c, shared.slowmoduleerr);
} else if (scriptIsEval()) {
rejectCommand(c, shared.slowevalerr);
} else {
rejectCommand(c, shared.slowscripterr);
}
return C_OK;
}
if ((c->flags & CLIENT_SLAVE) && (is_may_replicate_command || is_write_command || is_read_command)) {
rejectCommandFormat(c, "Replica can't interact with the keyspace");
return C_OK;
}
if (!(c->flags & CLIENT_SLAVE) &&
((server.client_pause_type == CLIENT_PAUSE_ALL) ||
(server.client_pause_type == CLIENT_PAUSE_WRITE && is_may_replicate_command)))
{
c->bpop.timeout = 0;
blockClient(c,BLOCKED_POSTPONE);
return C_OK;
}
if (c->flags & CLIENT_MULTI &&
c->cmd->proc != execCommand &&
c->cmd->proc != discardCommand &&
c->cmd->proc != multiCommand &&
c->cmd->proc != watchCommand &&
c->cmd->proc != quitCommand &&
c->cmd->proc != resetCommand)
{
queueMultiCommand(c);
addReply(c,shared.queued);
} else {
call(c,CMD_CALL_FULL);
c->woff = server.master_repl_offset;
if (listLength(server.ready_keys))
handleClientsBlockedOnKeys();
}
return C_OK;
}
void incrementErrorCount(const char *fullerr, size_t namelen) {
struct redisError *error = raxFind(server.errors,(unsigned char*)fullerr,namelen);
if (error == raxNotFound) {
error = zmalloc(sizeof(*error));
error->count = 0;
raxInsert(server.errors,(unsigned char*)fullerr,namelen,error,NULL);
}
error->count++;
}
void closeListeningSockets(int unlink_unix_socket) {
int j;
for (j = 0; j < server.ipfd.count; j++) close(server.ipfd.fd[j]);
for (j = 0; j < server.tlsfd.count; j++) close(server.tlsfd.fd[j]);
if (server.sofd != -1) close(server.sofd);
if (server.cluster_enabled)
for (j = 0; j < server.cfd.count; j++) close(server.cfd.fd[j]);
if (unlink_unix_socket && server.unixsocket) {
serverLog(LL_NOTICE,"Removing the unix socket file.");
if (unlink(server.unixsocket) != 0)
serverLog(LL_WARNING,"Error removing the unix socket file: %s",strerror(errno));
}
}
int prepareForShutdown(int flags) {
if (isShutdownInitiated()) return C_ERR;
if (server.loading || server.sentinel_mode)
flags = (flags & ~SHUTDOWN_SAVE) | SHUTDOWN_NOSAVE;
server.shutdown_flags = flags;
serverLog(LL_WARNING,"User requested shutdown...");
if (server.supervised_mode == SUPERVISED_SYSTEMD)
redisCommunicateSystemd("STOPPING=1\n");
if (!(flags & SHUTDOWN_NOW) &&
server.shutdown_timeout != 0 &&
!isReadyToShutdown())
{
server.shutdown_mstime = server.mstime + server.shutdown_timeout * 1000;
if (!areClientsPaused()) sendGetackToReplicas();
pauseClients(PAUSE_DURING_SHUTDOWN, LLONG_MAX, CLIENT_PAUSE_WRITE);
serverLog(LL_NOTICE, "Waiting for replicas before shutting down.");
return C_ERR;
}
return finishShutdown();
}
static inline int isShutdownInitiated(void) {
return server.shutdown_mstime != 0;
}
int isReadyToShutdown(void) {
if (listLength(server.slaves) == 0) return 1;
listIter li;
listNode *ln;
listRewind(server.slaves, &li);
while ((ln = listNext(&li)) != NULL) {
client *replica = listNodeValue(ln);
if (replica->repl_ack_off != server.master_repl_offset) return 0;
}
return 1;
}
static void cancelShutdown(void) {
server.shutdown_asap = 0;
server.shutdown_flags = 0;
server.shutdown_mstime = 0;
replyToClientsBlockedOnShutdown();
unpauseClients(PAUSE_DURING_SHUTDOWN);
}
int abortShutdown(void) {
if (isShutdownInitiated()) {
cancelShutdown();
} else if (server.shutdown_asap) {
server.shutdown_asap = 0;
} else {
return C_ERR;
}
serverLog(LL_NOTICE, "Shutdown manually aborted.");
return C_OK;
}
int finishShutdown(void) {
int save = server.shutdown_flags & SHUTDOWN_SAVE;
int nosave = server.shutdown_flags & SHUTDOWN_NOSAVE;
int force = server.shutdown_flags & SHUTDOWN_FORCE;
listIter replicas_iter;
listNode *replicas_list_node;
int num_replicas = 0, num_lagging_replicas = 0;
listRewind(server.slaves, &replicas_iter);
while ((replicas_list_node = listNext(&replicas_iter)) != NULL) {
client *replica = listNodeValue(replicas_list_node);
num_replicas++;
if (replica->repl_ack_off != server.master_repl_offset) {
num_lagging_replicas++;
long lag = replica->replstate == SLAVE_STATE_ONLINE ?
time(NULL) - replica->repl_ack_time : 0;
serverLog(LL_WARNING,
"Lagging replica %s reported offset %lld behind master, lag=%ld, state=%s.",
replicationGetSlaveName(replica),
server.master_repl_offset - replica->repl_ack_off,
lag,
replstateToString(replica->replstate));
}
}
if (num_replicas > 0) {
serverLog(LL_NOTICE,
"%d of %d replicas are in sync when shutting down.",
num_replicas - num_lagging_replicas,
num_replicas);
}
ldbKillForkedSessions();
if (server.child_type == CHILD_TYPE_RDB) {
serverLog(LL_WARNING,"There is a child saving an .rdb. Killing it!");
killRDBChild();
rdbRemoveTempFile(server.child_pid, 0);
}
if (server.child_type == CHILD_TYPE_MODULE) {
serverLog(LL_WARNING,"There is a module fork child. Killing it!");
TerminateModuleForkChild(server.child_pid,0);
}
if (server.child_type == CHILD_TYPE_AOF) {
if (server.aof_state == AOF_WAIT_REWRITE) {
if (force) {
serverLog(LL_WARNING, "Writing initial AOF. Exit anyway.");
} else {
serverLog(LL_WARNING, "Writing initial AOF, can't exit.");
goto error;
}
}
serverLog(LL_WARNING,
"There is a child rewriting the AOF. Killing it!");
killAppendOnlyChild();
}
if (server.aof_state != AOF_OFF) {
serverLog(LL_NOTICE,"Calling fsync() on the AOF file.");
flushAppendOnlyFile(1);
if (redis_fsync(server.aof_fd) == -1) {
serverLog(LL_WARNING,"Fail to fsync the AOF file: %s.",
strerror(errno));
}
}
if ((server.saveparamslen > 0 && !nosave) || save) {
serverLog(LL_NOTICE,"Saving the final RDB snapshot before exiting.");
if (server.supervised_mode == SUPERVISED_SYSTEMD)
redisCommunicateSystemd("STATUS=Saving the final RDB snapshot\n");
rdbSaveInfo rsi, *rsiptr;
rsiptr = rdbPopulateSaveInfo(&rsi);
if (rdbSave(SLAVE_REQ_NONE,server.rdb_filename,rsiptr) != C_OK) {
if (force) {
serverLog(LL_WARNING,"Error trying to save the DB. Exit anyway.");
} else {
serverLog(LL_WARNING,"Error trying to save the DB, can't exit.");
if (server.supervised_mode == SUPERVISED_SYSTEMD)
redisCommunicateSystemd("STATUS=Error trying to save the DB, can't exit.\n");
goto error;
}
}
}
if (server.aof_manifest) aofManifestFree(server.aof_manifest);
moduleFireServerEvent(REDISMODULE_EVENT_SHUTDOWN,0,NULL);
if (server.daemonize || server.pidfile) {
serverLog(LL_NOTICE,"Removing the pid file.");
unlink(server.pidfile);
}
flushSlavesOutputBuffers();
closeListeningSockets(1);
serverLog(LL_WARNING,"%s is now ready to exit, bye bye...",
server.sentinel_mode ? "Sentinel" : "Redis");
return C_OK;
error:
serverLog(LL_WARNING, "Errors trying to shut down the server. Check the logs for more information.");
cancelShutdown();
return C_ERR;
}
int writeCommandsDeniedByDiskError(void) {
if (server.stop_writes_on_bgsave_err &&
server.saveparamslen > 0 &&
server.lastbgsave_status == C_ERR)
{
return DISK_ERROR_TYPE_RDB;
} else if (server.aof_state != AOF_OFF) {
if (server.aof_last_write_status == C_ERR) {
return DISK_ERROR_TYPE_AOF;
}
int aof_bio_fsync_status;
atomicGet(server.aof_bio_fsync_status,aof_bio_fsync_status);
if (aof_bio_fsync_status == C_ERR) {
atomicGet(server.aof_bio_fsync_errno,server.aof_last_write_errno);
return DISK_ERROR_TYPE_AOF;
}
}
return DISK_ERROR_TYPE_NONE;
}
void pingCommand(client *c) {
if (c->argc > 2) {
addReplyErrorArity(c);
return;
}
if (c->flags & CLIENT_PUBSUB && c->resp == 2) {
addReply(c,shared.mbulkhdr[2]);
addReplyBulkCBuffer(c,"pong",4);
if (c->argc == 1)
addReplyBulkCBuffer(c,"",0);
else
addReplyBulk(c,c->argv[1]);
} else {
if (c->argc == 1)
addReply(c,shared.pong);
else
addReplyBulk(c,c->argv[1]);
}
}
void echoCommand(client *c) {
addReplyBulk(c,c->argv[1]);
}
void timeCommand(client *c) {
struct timeval tv;
gettimeofday(&tv,NULL);
addReplyArrayLen(c,2);
addReplyBulkLongLong(c,tv.tv_sec);
addReplyBulkLongLong(c,tv.tv_usec);
}
typedef struct replyFlagNames {
uint64_t flag;
const char *name;
} replyFlagNames;
void addReplyCommandFlags(client *c, uint64_t flags, replyFlagNames *replyFlags) {
int count = 0, j=0;
while (replyFlags[j].name) {
if (flags & replyFlags[j].flag)
count++;
j++;
}
addReplySetLen(c, count);
j = 0;
while (replyFlags[j].name) {
if (flags & replyFlags[j].flag)
addReplyStatus(c, replyFlags[j].name);
j++;
}
}
void addReplyFlagsForCommand(client *c, struct redisCommand *cmd) {
replyFlagNames flagNames[] = {
{CMD_WRITE, "write"},
{CMD_READONLY, "readonly"},
{CMD_DENYOOM, "denyoom"},
{CMD_MODULE, "module"},
{CMD_ADMIN, "admin"},
{CMD_PUBSUB, "pubsub"},
{CMD_NOSCRIPT, "noscript"},
{CMD_BLOCKING, "blocking"},
{CMD_LOADING, "loading"},
{CMD_STALE, "stale"},
{CMD_SKIP_MONITOR, "skip_monitor"},
{CMD_SKIP_SLOWLOG, "skip_slowlog"},
{CMD_ASKING, "asking"},
{CMD_FAST, "fast"},
{CMD_NO_AUTH, "no_auth"},
{CMD_MAY_REPLICATE, "may_replicate"},
{CMD_NO_MANDATORY_KEYS, "no_mandatory_keys"},
{CMD_NO_ASYNC_LOADING, "no_async_loading"},
{CMD_NO_MULTI, "no_multi"},
{CMD_MOVABLE_KEYS, "movablekeys"},
{CMD_ALLOW_BUSY, "allow_busy"},
{0,NULL}
};
addReplyCommandFlags(c, cmd->flags, flagNames);
}
void addReplyDocFlagsForCommand(client *c, struct redisCommand *cmd) {
replyFlagNames docFlagNames[] = {
{CMD_DOC_DEPRECATED, "deprecated"},
{CMD_DOC_SYSCMD, "syscmd"},
{0,NULL}
};
addReplyCommandFlags(c, cmd->doc_flags, docFlagNames);
}
void addReplyFlagsForKeyArgs(client *c, uint64_t flags) {
replyFlagNames docFlagNames[] = {
{CMD_KEY_RO, "RO"},
{CMD_KEY_RW, "RW"},
{CMD_KEY_OW, "OW"},
{CMD_KEY_RM, "RM"},
{CMD_KEY_ACCESS, "access"},
{CMD_KEY_UPDATE, "update"},
{CMD_KEY_INSERT, "insert"},
{CMD_KEY_DELETE, "delete"},
{CMD_KEY_CHANNEL, "channel"},
{CMD_KEY_INCOMPLETE, "incomplete"},
{0,NULL}
};
addReplyCommandFlags(c, flags, docFlagNames);
}
const char *ARG_TYPE_STR[] = {
"string",
"integer",
"double",
"key",
"pattern",
"unix-time",
"pure-token",
"oneof",
"block",
};
void addReplyFlagsForArg(client *c, uint64_t flags) {
replyFlagNames argFlagNames[] = {
{CMD_ARG_OPTIONAL, "optional"},
{CMD_ARG_MULTIPLE, "multiple"},
{CMD_ARG_MULTIPLE_TOKEN, "multiple_token"},
{0,NULL}
};
addReplyCommandFlags(c, flags, argFlagNames);
}
void addReplyCommandArgList(client *c, struct redisCommandArg *args, int num_args) {
addReplySetLen(c, num_args);
for (int j = 0; j<num_args; j++) {
long maplen = 2;
if (args[j].type == ARG_TYPE_KEY) maplen++;
if (args[j].token) maplen++;
if (args[j].summary) maplen++;
if (args[j].since) maplen++;
if (args[j].flags) maplen++;
if (args[j].type == ARG_TYPE_ONEOF || args[j].type == ARG_TYPE_BLOCK)
maplen++;
addReplyMapLen(c, maplen);
addReplyBulkCString(c, "name");
addReplyBulkCString(c, args[j].name);
addReplyBulkCString(c, "type");
addReplyBulkCString(c, ARG_TYPE_STR[args[j].type]);
if (args[j].type == ARG_TYPE_KEY) {
addReplyBulkCString(c, "key_spec_index");
addReplyLongLong(c, args[j].key_spec_index);
}
if (args[j].token) {
addReplyBulkCString(c, "token");
addReplyBulkCString(c, args[j].token);
}
if (args[j].summary) {
addReplyBulkCString(c, "summary");
addReplyBulkCString(c, args[j].summary);
}
if (args[j].since) {
addReplyBulkCString(c, "since");
addReplyBulkCString(c, args[j].since);
}
if (args[j].flags) {
addReplyBulkCString(c, "flags");
addReplyFlagsForArg(c, args[j].flags);
}
if (args[j].type == ARG_TYPE_ONEOF || args[j].type == ARG_TYPE_BLOCK) {
addReplyBulkCString(c, "arguments");
addReplyCommandArgList(c, args[j].subargs, args[j].num_args);
}
}
}
const char *RESP2_TYPE_STR[] = {
"simple-string",
"error",
"integer",
"bulk-string",
"null-bulk-string",
"array",
"null-array",
};
const char *RESP3_TYPE_STR[] = {
"simple-string",
"error",
"integer",
"double",
"bulk-string",
"array",
"map",
"set",
"bool",
"null",
};
void addReplyCommandHistory(client *c, struct redisCommand *cmd) {
addReplySetLen(c, cmd->num_history);
for (int j = 0; j<cmd->num_history; j++) {
addReplyArrayLen(c, 2);
addReplyBulkCString(c, cmd->history[j].since);
addReplyBulkCString(c, cmd->history[j].changes);
}
}
void addReplyCommandTips(client *c, struct redisCommand *cmd) {
addReplySetLen(c, cmd->num_tips);
for (int j = 0; j<cmd->num_tips; j++) {
addReplyBulkCString(c, cmd->tips[j]);
}
}
void addReplyCommandKeySpecs(client *c, struct redisCommand *cmd) {
addReplySetLen(c, cmd->key_specs_num);
for (int i = 0; i < cmd->key_specs_num; i++) {
addReplyMapLen(c, 3);
addReplyBulkCString(c, "flags");
addReplyFlagsForKeyArgs(c,cmd->key_specs[i].flags);
addReplyBulkCString(c, "begin_search");
switch (cmd->key_specs[i].begin_search_type) {
case KSPEC_BS_UNKNOWN:
addReplyMapLen(c, 2);
addReplyBulkCString(c, "type");
addReplyBulkCString(c, "unknown");
addReplyBulkCString(c, "spec");
addReplyMapLen(c, 0);
break;
case KSPEC_BS_INDEX:
addReplyMapLen(c, 2);
addReplyBulkCString(c, "type");
addReplyBulkCString(c, "index");
addReplyBulkCString(c, "spec");
addReplyMapLen(c, 1);
addReplyBulkCString(c, "index");
addReplyLongLong(c, cmd->key_specs[i].bs.index.pos);
break;
case KSPEC_BS_KEYWORD:
addReplyMapLen(c, 2);
addReplyBulkCString(c, "type");
addReplyBulkCString(c, "keyword");
addReplyBulkCString(c, "spec");
addReplyMapLen(c, 2);
addReplyBulkCString(c, "keyword");
addReplyBulkCString(c, cmd->key_specs[i].bs.keyword.keyword);
addReplyBulkCString(c, "startfrom");
addReplyLongLong(c, cmd->key_specs[i].bs.keyword.startfrom);
break;
default:
serverPanic("Invalid begin_search key spec type %d", cmd->key_specs[i].begin_search_type);
}
addReplyBulkCString(c, "find_keys");
switch (cmd->key_specs[i].find_keys_type) {
case KSPEC_FK_UNKNOWN:
addReplyMapLen(c, 2);
addReplyBulkCString(c, "type");
addReplyBulkCString(c, "unknown");
addReplyBulkCString(c, "spec");
addReplyMapLen(c, 0);
break;
case KSPEC_FK_RANGE:
addReplyMapLen(c, 2);
addReplyBulkCString(c, "type");
addReplyBulkCString(c, "range");
addReplyBulkCString(c, "spec");
addReplyMapLen(c, 3);
addReplyBulkCString(c, "lastkey");
addReplyLongLong(c, cmd->key_specs[i].fk.range.lastkey);
addReplyBulkCString(c, "keystep");
addReplyLongLong(c, cmd->key_specs[i].fk.range.keystep);
addReplyBulkCString(c, "limit");
addReplyLongLong(c, cmd->key_specs[i].fk.range.limit);
break;
case KSPEC_FK_KEYNUM:
addReplyMapLen(c, 2);
addReplyBulkCString(c, "type");
addReplyBulkCString(c, "keynum");
addReplyBulkCString(c, "spec");
addReplyMapLen(c, 3);
addReplyBulkCString(c, "keynumidx");
addReplyLongLong(c, cmd->key_specs[i].fk.keynum.keynumidx);
addReplyBulkCString(c, "firstkey");
addReplyLongLong(c, cmd->key_specs[i].fk.keynum.firstkey);
addReplyBulkCString(c, "keystep");
addReplyLongLong(c, cmd->key_specs[i].fk.keynum.keystep);
break;
default:
serverPanic("Invalid find_keys key spec type %d", cmd->key_specs[i].begin_search_type);
}
}
}
void addReplyCommandSubCommands(client *c, struct redisCommand *cmd, void (*reply_function)(client*, struct redisCommand*), int use_map) {
if (!cmd->subcommands_dict) {
addReplySetLen(c, 0);
return;
}
if (use_map)
addReplyMapLen(c, dictSize(cmd->subcommands_dict));
else
addReplyArrayLen(c, dictSize(cmd->subcommands_dict));
dictEntry *de;
dictIterator *di = dictGetSafeIterator(cmd->subcommands_dict);
while((de = dictNext(di)) != NULL) {
struct redisCommand *sub = (struct redisCommand *)dictGetVal(de);
if (use_map)
addReplyBulkCBuffer(c, sub->fullname, sdslen(sub->fullname));
reply_function(c, sub);
}
dictReleaseIterator(di);
}
const char *COMMAND_GROUP_STR[] = {
"generic",
"string",
"list",
"set",
"sorted-set",
"hash",
"pubsub",
"transactions",
"connection",
"server",
"scripting",
"hyperloglog",
"cluster",
"sentinel",
"geo",
"stream",
"bitmap",
"module"
};
void addReplyCommandInfo(client *c, struct redisCommand *cmd) {
if (!cmd) {
addReplyNull(c);
} else {
int firstkey = 0, lastkey = 0, keystep = 0;
if (cmd->legacy_range_key_spec.begin_search_type != KSPEC_BS_INVALID) {
firstkey = cmd->legacy_range_key_spec.bs.index.pos;
lastkey = cmd->legacy_range_key_spec.fk.range.lastkey;
if (lastkey >= 0)
lastkey += firstkey;
keystep = cmd->legacy_range_key_spec.fk.range.keystep;
}
addReplyArrayLen(c, 10);
addReplyBulkCBuffer(c, cmd->fullname, sdslen(cmd->fullname));
addReplyLongLong(c, cmd->arity);
addReplyFlagsForCommand(c, cmd);
addReplyLongLong(c, firstkey);
addReplyLongLong(c, lastkey);
addReplyLongLong(c, keystep);
addReplyCommandCategories(c, cmd);
addReplyCommandTips(c, cmd);
addReplyCommandKeySpecs(c, cmd);
addReplyCommandSubCommands(c, cmd, addReplyCommandInfo, 0);
}
}
void addReplyCommandDocs(client *c, struct redisCommand *cmd) {
long maplen = 3;
if (cmd->complexity) maplen++;
if (cmd->doc_flags) maplen++;
if (cmd->deprecated_since) maplen++;
if (cmd->replaced_by) maplen++;
if (cmd->history) maplen++;
if (cmd->args) maplen++;
if (cmd->subcommands_dict) maplen++;
addReplyMapLen(c, maplen);
addReplyBulkCString(c, "summary");
addReplyBulkCString(c, cmd->summary);
addReplyBulkCString(c, "since");
addReplyBulkCString(c, cmd->since);
addReplyBulkCString(c, "group");
addReplyBulkCString(c, COMMAND_GROUP_STR[cmd->group]);
if (cmd->complexity) {
addReplyBulkCString(c, "complexity");
addReplyBulkCString(c, cmd->complexity);
}
if (cmd->doc_flags) {
addReplyBulkCString(c, "doc_flags");
addReplyDocFlagsForCommand(c, cmd);
}
if (cmd->deprecated_since) {
addReplyBulkCString(c, "deprecated_since");
addReplyBulkCString(c, cmd->deprecated_since);
}
if (cmd->replaced_by) {
addReplyBulkCString(c, "replaced_by");
addReplyBulkCString(c, cmd->replaced_by);
}
if (cmd->history) {
addReplyBulkCString(c, "history");
addReplyCommandHistory(c, cmd);
}
if (cmd->args) {
addReplyBulkCString(c, "arguments");
addReplyCommandArgList(c, cmd->args, cmd->num_args);
}
if (cmd->subcommands_dict) {
addReplyBulkCString(c, "subcommands");
addReplyCommandSubCommands(c, cmd, addReplyCommandDocs, 1);
}
}
void getKeysSubcommand(client *c) {
struct redisCommand *cmd = lookupCommand(c->argv+2,c->argc-2);
getKeysResult result = GETKEYS_RESULT_INIT;
int j;
if (!cmd) {
addReplyError(c,"Invalid command specified");
return;
} else if (cmd->getkeys_proc == NULL && cmd->key_specs_num == 0) {
addReplyError(c,"The command has no key arguments");
return;
} else if ((cmd->arity > 0 && cmd->arity != c->argc-2) ||
((c->argc-2) < -cmd->arity))
{
addReplyError(c,"Invalid number of arguments specified for command");
return;
}
if (!getKeysFromCommand(cmd,c->argv+2,c->argc-2,&result)) {
if (cmd->flags & CMD_NO_MANDATORY_KEYS) {
addReplyArrayLen(c,0);
} else {
addReplyError(c,"Invalid arguments specified for command");
}
} else {
addReplyArrayLen(c,result.numkeys);
for (j = 0; j < result.numkeys; j++) addReplyBulk(c,c->argv[result.keys[j].pos+2]);
}
getKeysFreeResult(&result);
}
void commandCommand(client *c) {
dictIterator *di;
dictEntry *de;
addReplyArrayLen(c, dictSize(server.commands));
di = dictGetIterator(server.commands);
while ((de = dictNext(di)) != NULL) {
addReplyCommandInfo(c, dictGetVal(de));
}
dictReleaseIterator(di);
}
void commandCountCommand(client *c) {
addReplyLongLong(c, dictSize(server.commands));
}
typedef enum {
COMMAND_LIST_FILTER_MODULE,
COMMAND_LIST_FILTER_ACLCAT,
COMMAND_LIST_FILTER_PATTERN,
} commandListFilterType;
typedef struct {
commandListFilterType type;
sds arg;
struct {
int valid;
union {
uint64_t aclcat;
void *module_handle;
} u;
} cache;
} commandListFilter;
int shouldFilterFromCommandList(struct redisCommand *cmd, commandListFilter *filter) {
switch (filter->type) {
case (COMMAND_LIST_FILTER_MODULE):
if (!filter->cache.valid) {
filter->cache.u.module_handle = moduleGetHandleByName(filter->arg);
filter->cache.valid = 1;
}
return !moduleIsModuleCommand(filter->cache.u.module_handle, cmd);
case (COMMAND_LIST_FILTER_ACLCAT): {
if (!filter->cache.valid) {
filter->cache.u.aclcat = ACLGetCommandCategoryFlagByName(filter->arg);
filter->cache.valid = 1;
}
uint64_t cat = filter->cache.u.aclcat;
if (cat == 0)
return 1;
return (!(cmd->acl_categories & cat));
break;
}
case (COMMAND_LIST_FILTER_PATTERN):
return !stringmatchlen(filter->arg, sdslen(filter->arg), cmd->fullname, sdslen(cmd->fullname), 1);
default:
serverPanic("Invalid filter type %d", filter->type);
}
}
void commandListWithFilter(client *c, dict *commands, commandListFilter filter, int *numcmds) {
dictEntry *de;
dictIterator *di = dictGetIterator(commands);
while ((de = dictNext(di)) != NULL) {
struct redisCommand *cmd = dictGetVal(de);
if (!shouldFilterFromCommandList(cmd,&filter)) {
addReplyBulkCBuffer(c, cmd->fullname, sdslen(cmd->fullname));
(*numcmds)++;
}
if (cmd->subcommands_dict) {
commandListWithFilter(c, cmd->subcommands_dict, filter, numcmds);
}
}
dictReleaseIterator(di);
}
void commandListWithoutFilter(client *c, dict *commands, int *numcmds) {
dictEntry *de;
dictIterator *di = dictGetIterator(commands);
while ((de = dictNext(di)) != NULL) {
struct redisCommand *cmd = dictGetVal(de);
addReplyBulkCBuffer(c, cmd->fullname, sdslen(cmd->fullname));
(*numcmds)++;
if (cmd->subcommands_dict) {
commandListWithoutFilter(c, cmd->subcommands_dict, numcmds);
}
}
dictReleaseIterator(di);
}
void commandListCommand(client *c) {
int i = 2, got_filter = 0;
commandListFilter filter = {0};
for (; i < c->argc; i++) {
int moreargs = (c->argc-1) - i;
char *opt = c->argv[i]->ptr;
if (!strcasecmp(opt,"filterby") && moreargs == 2) {
char *filtertype = c->argv[i+1]->ptr;
if (!strcasecmp(filtertype,"module")) {
filter.type = COMMAND_LIST_FILTER_MODULE;
} else if (!strcasecmp(filtertype,"aclcat")) {
filter.type = COMMAND_LIST_FILTER_ACLCAT;
} else if (!strcasecmp(filtertype,"pattern")) {
filter.type = COMMAND_LIST_FILTER_PATTERN;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
got_filter = 1;
filter.arg = c->argv[i+2]->ptr;
i += 2;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
}
int numcmds = 0;
void *replylen = addReplyDeferredLen(c);
if (got_filter) {
commandListWithFilter(c, server.commands, filter, &numcmds);
} else {
commandListWithoutFilter(c, server.commands, &numcmds);
}
setDeferredArrayLen(c,replylen,numcmds);
}
void commandInfoCommand(client *c) {
int i;
if (c->argc == 2) {
dictIterator *di;
dictEntry *de;
addReplyArrayLen(c, dictSize(server.commands));
di = dictGetIterator(server.commands);
while ((de = dictNext(di)) != NULL) {
addReplyCommandInfo(c, dictGetVal(de));
}
dictReleaseIterator(di);
} else {
addReplyArrayLen(c, c->argc-2);
for (i = 2; i < c->argc; i++) {
addReplyCommandInfo(c, lookupCommandBySds(c->argv[i]->ptr));
}
}
}
void commandDocsCommand(client *c) {
int i;
if (c->argc == 2) {
dictIterator *di;
dictEntry *de;
addReplyMapLen(c, dictSize(server.commands));
di = dictGetIterator(server.commands);
while ((de = dictNext(di)) != NULL) {
struct redisCommand *cmd = dictGetVal(de);
addReplyBulkCBuffer(c, cmd->fullname, sdslen(cmd->fullname));
addReplyCommandDocs(c, cmd);
}
dictReleaseIterator(di);
} else {
int numcmds = 0;
void *replylen = addReplyDeferredLen(c);
for (i = 2; i < c->argc; i++) {
struct redisCommand *cmd = lookupCommandBySds(c->argv[i]->ptr);
if (!cmd)
continue;
addReplyBulkCBuffer(c, cmd->fullname, sdslen(cmd->fullname));
addReplyCommandDocs(c, cmd);
numcmds++;
}
setDeferredMapLen(c,replylen,numcmds);
}
}
void commandGetKeysCommand(client *c) {
getKeysSubcommand(c);
}
void commandHelpCommand(client *c) {
const char *help[] = {
"(no subcommand)",
" Return details about all Redis commands.",
"COUNT",
" Return the total number of commands in this Redis server.",
"LIST",
" Return a list of all commands in this Redis server.",
"INFO [<command-name> ...]",
" Return details about multiple Redis commands.",
" If no command names are given, documentation details for all",
" commands are returned.",
"DOCS [<command-name> ...]",
" Return documentation details about multiple Redis commands.",
" If no command names are given, documentation details for all",
" commands are returned.",
"GETKEYS <full-command>",
" Return the keys from a full Redis command.",
NULL
};
addReplyHelp(c, help);
}
void bytesToHuman(char *s, unsigned long long n) {
double d;
if (n < 1024) {
sprintf(s,"%lluB",n);
} else if (n < (1024*1024)) {
d = (double)n/(1024);
sprintf(s,"%.2fK",d);
} else if (n < (1024LL*1024*1024)) {
d = (double)n/(1024*1024);
sprintf(s,"%.2fM",d);
} else if (n < (1024LL*1024*1024*1024)) {
d = (double)n/(1024LL*1024*1024);
sprintf(s,"%.2fG",d);
} else if (n < (1024LL*1024*1024*1024*1024)) {
d = (double)n/(1024LL*1024*1024*1024);
sprintf(s,"%.2fT",d);
} else if (n < (1024LL*1024*1024*1024*1024*1024)) {
d = (double)n/(1024LL*1024*1024*1024*1024);
sprintf(s,"%.2fP",d);
} else {
sprintf(s,"%lluB",n);
}
}
sds fillPercentileDistributionLatencies(sds info, const char* histogram_name, struct hdr_histogram* histogram) {
info = sdscatfmt(info,"latency_percentiles_usec_%s:",histogram_name);
for (int j = 0; j < server.latency_tracking_info_percentiles_len; j++) {
char fbuf[128];
size_t len = sprintf(fbuf, "%f", server.latency_tracking_info_percentiles[j]);
len = trimDoubleString(fbuf, len);
info = sdscatprintf(info,"p%s=%.3f", fbuf,
((double)hdr_value_at_percentile(histogram,server.latency_tracking_info_percentiles[j]))/1000.0f);
if (j != server.latency_tracking_info_percentiles_len-1)
info = sdscatlen(info,",",1);
}
info = sdscatprintf(info,"\r\n");
return info;
}
const char *replstateToString(int replstate) {
switch (replstate) {
case SLAVE_STATE_WAIT_BGSAVE_START:
case SLAVE_STATE_WAIT_BGSAVE_END:
return "wait_bgsave";
case SLAVE_STATE_SEND_BULK:
return "send_bulk";
case SLAVE_STATE_ONLINE:
return "online";
default:
return "";
}
}
static char unsafe_info_chars[] = "#:\n\r";
static char unsafe_info_chars_substs[] = "____";
const char *getSafeInfoString(const char *s, size_t len, char **tmp) {
*tmp = NULL;
if (mempbrk(s, len, unsafe_info_chars,sizeof(unsafe_info_chars)-1)
== NULL) return s;
char *new = *tmp = zmalloc(len + 1);
memcpy(new, s, len);
new[len] = '\0';
return memmapchars(new, len, unsafe_info_chars, unsafe_info_chars_substs,
sizeof(unsafe_info_chars)-1);
}
sds genRedisInfoStringCommandStats(sds info, dict *commands) {
struct redisCommand *c;
dictEntry *de;
dictIterator *di;
di = dictGetSafeIterator(commands);
while((de = dictNext(di)) != NULL) {
char *tmpsafe;
c = (struct redisCommand *) dictGetVal(de);
if (c->calls || c->failed_calls || c->rejected_calls) {
info = sdscatprintf(info,
"cmdstat_%s:calls=%lld,usec=%lld,usec_per_call=%.2f"
",rejected_calls=%lld,failed_calls=%lld\r\n",
getSafeInfoString(c->fullname, sdslen(c->fullname), &tmpsafe), c->calls, c->microseconds,
(c->calls == 0) ? 0 : ((float)c->microseconds/c->calls),
c->rejected_calls, c->failed_calls);
if (tmpsafe != NULL) zfree(tmpsafe);
}
if (c->subcommands_dict) {
info = genRedisInfoStringCommandStats(info, c->subcommands_dict);
}
}
dictReleaseIterator(di);
return info;
}
sds genRedisInfoStringLatencyStats(sds info, dict *commands) {
struct redisCommand *c;
dictEntry *de;
dictIterator *di;
di = dictGetSafeIterator(commands);
while((de = dictNext(di)) != NULL) {
char *tmpsafe;
c = (struct redisCommand *) dictGetVal(de);
if (c->latency_histogram) {
info = fillPercentileDistributionLatencies(info,
getSafeInfoString(c->fullname, sdslen(c->fullname), &tmpsafe),
c->latency_histogram);
if (tmpsafe != NULL) zfree(tmpsafe);
}
if (c->subcommands_dict) {
info = genRedisInfoStringLatencyStats(info, c->subcommands_dict);
}
}
dictReleaseIterator(di);
return info;
}
sds genRedisInfoString(const char *section) {
sds info = sdsempty();
time_t uptime = server.unixtime-server.stat_starttime;
int j;
int allsections = 0, defsections = 0, everything = 0, modules = 0;
int sections = 0;
if (section == NULL) section = "default";
allsections = strcasecmp(section,"all") == 0;
defsections = strcasecmp(section,"default") == 0;
everything = strcasecmp(section,"everything") == 0;
modules = strcasecmp(section,"modules") == 0;
if (everything) allsections = 1;
if (allsections || defsections || !strcasecmp(section,"server")) {
static int call_uname = 1;
static struct utsname name;
char *mode;
char *supervised;
if (server.cluster_enabled) mode = "cluster";
else if (server.sentinel_mode) mode = "sentinel";
else mode = "standalone";
if (server.supervised) {
if (server.supervised_mode == SUPERVISED_UPSTART) supervised = "upstart";
else if (server.supervised_mode == SUPERVISED_SYSTEMD) supervised = "systemd";
else supervised = "unknown";
} else {
supervised = "no";
}
if (sections++) info = sdscat(info,"\r\n");
if (call_uname) {
uname(&name);
call_uname = 0;
}
unsigned int lruclock;
atomicGet(server.lruclock,lruclock);
info = sdscatfmt(info,
"#Server\r\n"
"redis_version:%s\r\n"
"redis_git_sha1:%s\r\n"
"redis_git_dirty:%i\r\n"
"redis_build_id:%s\r\n"
"redis_mode:%s\r\n"
"os:%s %s %s\r\n"
"arch_bits:%i\r\n"
"multiplexing_api:%s\r\n"
"atomicvar_api:%s\r\n"
"gcc_version:%i.%i.%i\r\n"
"process_id:%I\r\n"
"process_supervised:%s\r\n"
"run_id:%s\r\n"
"tcp_port:%i\r\n"
"server_time_usec:%I\r\n"
"uptime_in_seconds:%I\r\n"
"uptime_in_days:%I\r\n"
"hz:%i\r\n"
"configured_hz:%i\r\n"
"lru_clock:%u\r\n"
"executable:%s\r\n"
"config_file:%s\r\n"
"io_threads_active:%i\r\n",
REDIS_VERSION,
redisGitSHA1(),
strtol(redisGitDirty(),NULL,10) > 0,
redisBuildIdString(),
mode,
name.sysname, name.release, name.machine,
server.arch_bits,
aeGetApiName(),
REDIS_ATOMIC_API,
#if defined(__GNUC__)
__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__,
#else
0,0,0,
#endif
(int64_t) getpid(),
supervised,
server.runid,
server.port ? server.port : server.tls_port,
(int64_t)server.ustime,
(int64_t)uptime,
(int64_t)(uptime/(3600*24)),
server.hz,
server.config_hz,
lruclock,
server.executable ? server.executable : "",
server.configfile ? server.configfile : "",
server.io_threads_active);
if (isShutdownInitiated()) {
info = sdscatfmt(info,
"shutdown_in_milliseconds:%I\r\n",
(int64_t)(server.shutdown_mstime - server.mstime));
}
}
if (allsections || defsections || !strcasecmp(section,"clients")) {
size_t maxin, maxout;
getExpansiveClientsInfo(&maxin,&maxout);
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info,
"#Clients\r\n"
"connected_clients:%lu\r\n"
"cluster_connections:%lu\r\n"
"maxclients:%u\r\n"
"client_recent_max_input_buffer:%zu\r\n"
"client_recent_max_output_buffer:%zu\r\n"
"blocked_clients:%d\r\n"
"tracking_clients:%d\r\n"
"clients_in_timeout_table:%llu\r\n",
listLength(server.clients)-listLength(server.slaves),
getClusterConnectionsCount(),
server.maxclients,
maxin, maxout,
server.blocked_clients,
server.tracking_clients,
(unsigned long long) raxSize(server.clients_timeout_table));
}
if (allsections || defsections || !strcasecmp(section,"memory")) {
char hmem[64];
char peak_hmem[64];
char total_system_hmem[64];
char used_memory_lua_hmem[64];
char used_memory_vm_total_hmem[64];
char used_memory_scripts_hmem[64];
char used_memory_rss_hmem[64];
char maxmemory_hmem[64];
size_t zmalloc_used = zmalloc_used_memory();
size_t total_system_mem = server.system_memory_size;
const char *evict_policy = evictPolicyToString();
long long memory_lua = evalMemory();
long long memory_functions = functionsMemory();
struct redisMemOverhead *mh = getMemoryOverheadData();
if (zmalloc_used > server.stat_peak_memory)
server.stat_peak_memory = zmalloc_used;
bytesToHuman(hmem,zmalloc_used);
bytesToHuman(peak_hmem,server.stat_peak_memory);
bytesToHuman(total_system_hmem,total_system_mem);
bytesToHuman(used_memory_lua_hmem,memory_lua);
bytesToHuman(used_memory_vm_total_hmem,memory_functions + memory_lua);
bytesToHuman(used_memory_scripts_hmem,mh->lua_caches + mh->functions_caches);
bytesToHuman(used_memory_rss_hmem,server.cron_malloc_stats.process_rss);
bytesToHuman(maxmemory_hmem,server.maxmemory);
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info,
"#Memory\r\n"
"used_memory:%zu\r\n"
"used_memory_human:%s\r\n"
"used_memory_rss:%zu\r\n"
"used_memory_rss_human:%s\r\n"
"used_memory_peak:%zu\r\n"
"used_memory_peak_human:%s\r\n"
"used_memory_peak_perc:%.2f%%\r\n"
"used_memory_overhead:%zu\r\n"
"used_memory_startup:%zu\r\n"
"used_memory_dataset:%zu\r\n"
"used_memory_dataset_perc:%.2f%%\r\n"
"allocator_allocated:%zu\r\n"
"allocator_active:%zu\r\n"
"allocator_resident:%zu\r\n"
"total_system_memory:%lu\r\n"
"total_system_memory_human:%s\r\n"
"used_memory_lua:%lld\r\n"
"used_memory_vm_eval:%lld\r\n"
"used_memory_lua_human:%s\r\n"
"used_memory_scripts_eval:%lld\r\n"
"number_of_cached_scripts:%lu\r\n"
"number_of_functions:%lu\r\n"
"number_of_libraries:%lu\r\n"
"used_memory_vm_functions:%lld\r\n"
"used_memory_vm_total:%lld\r\n"
"used_memory_vm_total_human:%s\r\n"
"used_memory_functions:%lld\r\n"
"used_memory_scripts:%lld\r\n"
"used_memory_scripts_human:%s\r\n"
"maxmemory:%lld\r\n"
"maxmemory_human:%s\r\n"
"maxmemory_policy:%s\r\n"
"allocator_frag_ratio:%.2f\r\n"
"allocator_frag_bytes:%zu\r\n"
"allocator_rss_ratio:%.2f\r\n"
"allocator_rss_bytes:%zd\r\n"
"rss_overhead_ratio:%.2f\r\n"
"rss_overhead_bytes:%zd\r\n"
"mem_fragmentation_ratio:%.2f\r\n"
"mem_fragmentation_bytes:%zd\r\n"
"mem_not_counted_for_evict:%zu\r\n"
"mem_replication_backlog:%zu\r\n"
"mem_total_replication_buffers:%zu\r\n"
"mem_clients_slaves:%zu\r\n"
"mem_clients_normal:%zu\r\n"
"mem_cluster_links:%zu\r\n"
"mem_aof_buffer:%zu\r\n"
"mem_allocator:%s\r\n"
"active_defrag_running:%d\r\n"
"lazyfree_pending_objects:%zu\r\n"
"lazyfreed_objects:%zu\r\n",
zmalloc_used,
hmem,
server.cron_malloc_stats.process_rss,
used_memory_rss_hmem,
server.stat_peak_memory,
peak_hmem,
mh->peak_perc,
mh->overhead_total,
mh->startup_allocated,
mh->dataset,
mh->dataset_perc,
server.cron_malloc_stats.allocator_allocated,
server.cron_malloc_stats.allocator_active,
server.cron_malloc_stats.allocator_resident,
(unsigned long)total_system_mem,
total_system_hmem,
memory_lua,
memory_lua,
used_memory_lua_hmem,
(long long) mh->lua_caches,
dictSize(evalScriptsDict()),
functionsNum(),
functionsLibNum(),
memory_functions,
memory_functions + memory_lua,
used_memory_vm_total_hmem,
(long long) mh->functions_caches,
(long long) mh->lua_caches + (long long) mh->functions_caches,
used_memory_scripts_hmem,
server.maxmemory,
maxmemory_hmem,
evict_policy,
mh->allocator_frag,
mh->allocator_frag_bytes,
mh->allocator_rss,
mh->allocator_rss_bytes,
mh->rss_extra,
mh->rss_extra_bytes,
mh->total_frag,
mh->total_frag_bytes,
freeMemoryGetNotCountedMemory(),
mh->repl_backlog,
server.repl_buffer_mem,
mh->clients_slaves,
mh->clients_normal,
mh->cluster_links,
mh->aof_buffer,
ZMALLOC_LIB,
server.active_defrag_running,
lazyfreeGetPendingObjectsCount(),
lazyfreeGetFreedObjectsCount()
);
freeMemoryOverheadData(mh);
}
if (allsections || defsections || !strcasecmp(section,"persistence")) {
if (sections++) info = sdscat(info,"\r\n");
double fork_perc = 0;
if (server.stat_module_progress) {
fork_perc = server.stat_module_progress * 100;
} else if (server.stat_current_save_keys_total) {
fork_perc = ((double)server.stat_current_save_keys_processed / server.stat_current_save_keys_total) * 100;
}
int aof_bio_fsync_status;
atomicGet(server.aof_bio_fsync_status,aof_bio_fsync_status);
info = sdscatprintf(info,
"#Persistence\r\n"
"loading:%d\r\n"
"async_loading:%d\r\n"
"current_cow_peak:%zu\r\n"
"current_cow_size:%zu\r\n"
"current_cow_size_age:%lu\r\n"
"current_fork_perc:%.2f\r\n"
"current_save_keys_processed:%zu\r\n"
"current_save_keys_total:%zu\r\n"
"rdb_changes_since_last_save:%lld\r\n"
"rdb_bgsave_in_progress:%d\r\n"
"rdb_last_save_time:%jd\r\n"
"rdb_last_bgsave_status:%s\r\n"
"rdb_last_bgsave_time_sec:%jd\r\n"
"rdb_current_bgsave_time_sec:%jd\r\n"
"rdb_last_cow_size:%zu\r\n"
"rdb_last_load_keys_expired:%lld\r\n"
"rdb_last_load_keys_loaded:%lld\r\n"
"aof_enabled:%d\r\n"
"aof_rewrite_in_progress:%d\r\n"
"aof_rewrite_scheduled:%d\r\n"
"aof_last_rewrite_time_sec:%jd\r\n"
"aof_current_rewrite_time_sec:%jd\r\n"
"aof_last_bgrewrite_status:%s\r\n"
"aof_last_write_status:%s\r\n"
"aof_last_cow_size:%zu\r\n"
"module_fork_in_progress:%d\r\n"
"module_fork_last_cow_size:%zu\r\n",
(int)(server.loading && !server.async_loading),
(int)server.async_loading,
server.stat_current_cow_peak,
server.stat_current_cow_bytes,
server.stat_current_cow_updated ? (unsigned long) elapsedMs(server.stat_current_cow_updated) / 1000 : 0,
fork_perc,
server.stat_current_save_keys_processed,
server.stat_current_save_keys_total,
server.dirty,
server.child_type == CHILD_TYPE_RDB,
(intmax_t)server.lastsave,
(server.lastbgsave_status == C_OK) ? "ok" : "err",
(intmax_t)server.rdb_save_time_last,
(intmax_t)((server.child_type != CHILD_TYPE_RDB) ?
-1 : time(NULL)-server.rdb_save_time_start),
server.stat_rdb_cow_bytes,
server.rdb_last_load_keys_expired,
server.rdb_last_load_keys_loaded,
server.aof_state != AOF_OFF,
server.child_type == CHILD_TYPE_AOF,
server.aof_rewrite_scheduled,
(intmax_t)server.aof_rewrite_time_last,
(intmax_t)((server.child_type != CHILD_TYPE_AOF) ?
-1 : time(NULL)-server.aof_rewrite_time_start),
(server.aof_lastbgrewrite_status == C_OK) ? "ok" : "err",
(server.aof_last_write_status == C_OK &&
aof_bio_fsync_status == C_OK) ? "ok" : "err",
server.stat_aof_cow_bytes,
server.child_type == CHILD_TYPE_MODULE,
server.stat_module_cow_bytes);
if (server.aof_enabled) {
info = sdscatprintf(info,
"aof_current_size:%lld\r\n"
"aof_base_size:%lld\r\n"
"aof_pending_rewrite:%d\r\n"
"aof_buffer_length:%zu\r\n"
"aof_pending_bio_fsync:%llu\r\n"
"aof_delayed_fsync:%lu\r\n",
(long long) server.aof_current_size,
(long long) server.aof_rewrite_base_size,
server.aof_rewrite_scheduled,
sdslen(server.aof_buf),
bioPendingJobsOfType(BIO_AOF_FSYNC),
server.aof_delayed_fsync);
}
if (server.loading) {
double perc = 0;
time_t eta, elapsed;
off_t remaining_bytes = 1;
if (server.loading_total_bytes) {
perc = ((double)server.loading_loaded_bytes / server.loading_total_bytes) * 100;
remaining_bytes = server.loading_total_bytes - server.loading_loaded_bytes;
} else if(server.loading_rdb_used_mem) {
perc = ((double)server.loading_loaded_bytes / server.loading_rdb_used_mem) * 100;
remaining_bytes = server.loading_rdb_used_mem - server.loading_loaded_bytes;
if (perc > 99.99) perc = 99.99;
if (remaining_bytes < 1) remaining_bytes = 1;
}
elapsed = time(NULL)-server.loading_start_time;
if (elapsed == 0) {
eta = 1;
} else {
eta = (elapsed*remaining_bytes)/(server.loading_loaded_bytes+1);
}
info = sdscatprintf(info,
"loading_start_time:%jd\r\n"
"loading_total_bytes:%llu\r\n"
"loading_rdb_used_mem:%llu\r\n"
"loading_loaded_bytes:%llu\r\n"
"loading_loaded_perc:%.2f\r\n"
"loading_eta_seconds:%jd\r\n",
(intmax_t) server.loading_start_time,
(unsigned long long) server.loading_total_bytes,
(unsigned long long) server.loading_rdb_used_mem,
(unsigned long long) server.loading_loaded_bytes,
perc,
(intmax_t)eta
);
}
}
if (allsections || defsections || !strcasecmp(section,"stats")) {
long long stat_total_reads_processed, stat_total_writes_processed;
long long stat_net_input_bytes, stat_net_output_bytes;
long long current_eviction_exceeded_time = server.stat_last_eviction_exceeded_time ?
(long long) elapsedUs(server.stat_last_eviction_exceeded_time): 0;
long long current_active_defrag_time = server.stat_last_active_defrag_time ?
(long long) elapsedUs(server.stat_last_active_defrag_time): 0;
atomicGet(server.stat_total_reads_processed, stat_total_reads_processed);
atomicGet(server.stat_total_writes_processed, stat_total_writes_processed);
atomicGet(server.stat_net_input_bytes, stat_net_input_bytes);
atomicGet(server.stat_net_output_bytes, stat_net_output_bytes);
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info,
"#Stats\r\n"
"total_connections_received:%lld\r\n"
"total_commands_processed:%lld\r\n"
"instantaneous_ops_per_sec:%lld\r\n"
"total_net_input_bytes:%lld\r\n"
"total_net_output_bytes:%lld\r\n"
"instantaneous_input_kbps:%.2f\r\n"
"instantaneous_output_kbps:%.2f\r\n"
"rejected_connections:%lld\r\n"
"sync_full:%lld\r\n"
"sync_partial_ok:%lld\r\n"
"sync_partial_err:%lld\r\n"
"expired_keys:%lld\r\n"
"expired_stale_perc:%.2f\r\n"
"expired_time_cap_reached_count:%lld\r\n"
"expire_cycle_cpu_milliseconds:%lld\r\n"
"evicted_keys:%lld\r\n"
"evicted_clients:%lld\r\n"
"total_eviction_exceeded_time:%lld\r\n"
"current_eviction_exceeded_time:%lld\r\n"
"keyspace_hits:%lld\r\n"
"keyspace_misses:%lld\r\n"
"pubsub_channels:%ld\r\n"
"pubsub_patterns:%lu\r\n"
"latest_fork_usec:%lld\r\n"
"total_forks:%lld\r\n"
"migrate_cached_sockets:%ld\r\n"
"slave_expires_tracked_keys:%zu\r\n"
"active_defrag_hits:%lld\r\n"
"active_defrag_misses:%lld\r\n"
"active_defrag_key_hits:%lld\r\n"
"active_defrag_key_misses:%lld\r\n"
"total_active_defrag_time:%lld\r\n"
"current_active_defrag_time:%lld\r\n"
"tracking_total_keys:%lld\r\n"
"tracking_total_items:%lld\r\n"
"tracking_total_prefixes:%lld\r\n"
"unexpected_error_replies:%lld\r\n"
"total_error_replies:%lld\r\n"
"dump_payload_sanitizations:%lld\r\n"
"total_reads_processed:%lld\r\n"
"total_writes_processed:%lld\r\n"
"io_threaded_reads_processed:%lld\r\n"
"io_threaded_writes_processed:%lld\r\n",
server.stat_numconnections,
server.stat_numcommands,
getInstantaneousMetric(STATS_METRIC_COMMAND),
stat_net_input_bytes,
stat_net_output_bytes,
(float)getInstantaneousMetric(STATS_METRIC_NET_INPUT)/1024,
(float)getInstantaneousMetric(STATS_METRIC_NET_OUTPUT)/1024,
server.stat_rejected_conn,
server.stat_sync_full,
server.stat_sync_partial_ok,
server.stat_sync_partial_err,
server.stat_expiredkeys,
server.stat_expired_stale_perc*100,
server.stat_expired_time_cap_reached_count,
server.stat_expire_cycle_time_used/1000,
server.stat_evictedkeys,
server.stat_evictedclients,
(server.stat_total_eviction_exceeded_time + current_eviction_exceeded_time) / 1000,
current_eviction_exceeded_time / 1000,
server.stat_keyspace_hits,
server.stat_keyspace_misses,
dictSize(server.pubsub_channels),
dictSize(server.pubsub_patterns),
server.stat_fork_time,
server.stat_total_forks,
dictSize(server.migrate_cached_sockets),
getSlaveKeyWithExpireCount(),
server.stat_active_defrag_hits,
server.stat_active_defrag_misses,
server.stat_active_defrag_key_hits,
server.stat_active_defrag_key_misses,
(server.stat_total_active_defrag_time + current_active_defrag_time) / 1000,
current_active_defrag_time / 1000,
(unsigned long long) trackingGetTotalKeys(),
(unsigned long long) trackingGetTotalItems(),
(unsigned long long) trackingGetTotalPrefixes(),
server.stat_unexpected_error_replies,
server.stat_total_error_replies,
server.stat_dump_payload_sanitizations,
stat_total_reads_processed,
stat_total_writes_processed,
server.stat_io_reads_processed,
server.stat_io_writes_processed);
}
if (allsections || defsections || !strcasecmp(section,"replication")) {
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info,
"#Replication\r\n"
"role:%s\r\n",
server.masterhost == NULL ? "master" : "slave");
if (server.masterhost) {
long long slave_repl_offset = 1;
long long slave_read_repl_offset = 1;
if (server.master) {
slave_repl_offset = server.master->reploff;
slave_read_repl_offset = server.master->read_reploff;
} else if (server.cached_master) {
slave_repl_offset = server.cached_master->reploff;
slave_read_repl_offset = server.cached_master->read_reploff;
}
info = sdscatprintf(info,
"master_host:%s\r\n"
"master_port:%d\r\n"
"master_link_status:%s\r\n"
"master_last_io_seconds_ago:%d\r\n"
"master_sync_in_progress:%d\r\n"
"slave_read_repl_offset:%lld\r\n"
"slave_repl_offset:%lld\r\n"
,server.masterhost,
server.masterport,
(server.repl_state == REPL_STATE_CONNECTED) ?
"up" : "down",
server.master ?
((int)(server.unixtime-server.master->lastinteraction)) : -1,
server.repl_state == REPL_STATE_TRANSFER,
slave_read_repl_offset,
slave_repl_offset
);
if (server.repl_state == REPL_STATE_TRANSFER) {
double perc = 0;
if (server.repl_transfer_size) {
perc = ((double)server.repl_transfer_read / server.repl_transfer_size) * 100;
}
info = sdscatprintf(info,
"master_sync_total_bytes:%lld\r\n"
"master_sync_read_bytes:%lld\r\n"
"master_sync_left_bytes:%lld\r\n"
"master_sync_perc:%.2f\r\n"
"master_sync_last_io_seconds_ago:%d\r\n",
(long long) server.repl_transfer_size,
(long long) server.repl_transfer_read,
(long long) (server.repl_transfer_size - server.repl_transfer_read),
perc,
(int)(server.unixtime-server.repl_transfer_lastio)
);
}
if (server.repl_state != REPL_STATE_CONNECTED) {
info = sdscatprintf(info,
"master_link_down_since_seconds:%jd\r\n",
server.repl_down_since ?
(intmax_t)(server.unixtime-server.repl_down_since) : -1);
}
info = sdscatprintf(info,
"slave_priority:%d\r\n"
"slave_read_only:%d\r\n"
"replica_announced:%d\r\n",
server.slave_priority,
server.repl_slave_ro,
server.replica_announced);
}
info = sdscatprintf(info,
"connected_slaves:%lu\r\n",
listLength(server.slaves));
if (server.repl_min_slaves_to_write &&
server.repl_min_slaves_max_lag) {
info = sdscatprintf(info,
"min_slaves_good_slaves:%d\r\n",
server.repl_good_slaves_count);
}
if (listLength(server.slaves)) {
int slaveid = 0;
listNode *ln;
listIter li;
listRewind(server.slaves,&li);
while((ln = listNext(&li))) {
client *slave = listNodeValue(ln);
char ip[NET_IP_STR_LEN], *slaveip = slave->slave_addr;
int port;
long lag = 0;
if (!slaveip) {
if (connPeerToString(slave->conn,ip,sizeof(ip),&port) == -1)
continue;
slaveip = ip;
}
const char *state = replstateToString(slave->replstate);
if (state[0] == '\0') continue;
if (slave->replstate == SLAVE_STATE_ONLINE)
lag = time(NULL) - slave->repl_ack_time;
info = sdscatprintf(info,
"slave%d:ip=%s,port=%d,state=%s,"
"offset=%lld,lag=%ld\r\n",
slaveid,slaveip,slave->slave_listening_port,state,
slave->repl_ack_off, lag);
slaveid++;
}
}
info = sdscatprintf(info,
"master_failover_state:%s\r\n"
"master_replid:%s\r\n"
"master_replid2:%s\r\n"
"master_repl_offset:%lld\r\n"
"second_repl_offset:%lld\r\n"
"repl_backlog_active:%d\r\n"
"repl_backlog_size:%lld\r\n"
"repl_backlog_first_byte_offset:%lld\r\n"
"repl_backlog_histlen:%lld\r\n",
getFailoverStateString(),
server.replid,
server.replid2,
server.master_repl_offset,
server.second_replid_offset,
server.repl_backlog != NULL,
server.repl_backlog_size,
server.repl_backlog ? server.repl_backlog->offset : 0,
server.repl_backlog ? server.repl_backlog->histlen : 0);
}
if (allsections || defsections || !strcasecmp(section,"cpu")) {
if (sections++) info = sdscat(info,"\r\n");
struct rusage self_ru, c_ru;
getrusage(RUSAGE_SELF, &self_ru);
getrusage(RUSAGE_CHILDREN, &c_ru);
info = sdscatprintf(info,
"#CPU\r\n"
"used_cpu_sys:%ld.%06ld\r\n"
"used_cpu_user:%ld.%06ld\r\n"
"used_cpu_sys_children:%ld.%06ld\r\n"
"used_cpu_user_children:%ld.%06ld\r\n",
(long)self_ru.ru_stime.tv_sec, (long)self_ru.ru_stime.tv_usec,
(long)self_ru.ru_utime.tv_sec, (long)self_ru.ru_utime.tv_usec,
(long)c_ru.ru_stime.tv_sec, (long)c_ru.ru_stime.tv_usec,
(long)c_ru.ru_utime.tv_sec, (long)c_ru.ru_utime.tv_usec);
#if defined(RUSAGE_THREAD)
struct rusage m_ru;
getrusage(RUSAGE_THREAD, &m_ru);
info = sdscatprintf(info,
"used_cpu_sys_main_thread:%ld.%06ld\r\n"
"used_cpu_user_main_thread:%ld.%06ld\r\n",
(long)m_ru.ru_stime.tv_sec, (long)m_ru.ru_stime.tv_usec,
(long)m_ru.ru_utime.tv_sec, (long)m_ru.ru_utime.tv_usec);
#endif
}
if (allsections || defsections || !strcasecmp(section,"modules")) {
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info,"#Modules\r\n");
info = genModulesInfoString(info);
}
if (allsections || !strcasecmp(section,"commandstats")) {
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info, "#Commandstats\r\n");
info = genRedisInfoStringCommandStats(info, server.commands);
}
if (allsections || defsections || !strcasecmp(section,"errorstats")) {
if (sections++) info = sdscat(info,"\r\n");
info = sdscat(info, "#Errorstats\r\n");
raxIterator ri;
raxStart(&ri,server.errors);
raxSeek(&ri,"^",NULL,0);
struct redisError *e;
while(raxNext(&ri)) {
char *tmpsafe;
e = (struct redisError *) ri.data;
info = sdscatprintf(info,
"errorstat_%.*s:count=%lld\r\n",
(int)ri.key_len, getSafeInfoString((char *) ri.key, ri.key_len, &tmpsafe), e->count);
if (tmpsafe != NULL) zfree(tmpsafe);
}
raxStop(&ri);
}
if (allsections || !strcasecmp(section,"latencystats")) {
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info, "#Latencystats\r\n");
if (server.latency_tracking_enabled) {
info = genRedisInfoStringLatencyStats(info, server.commands);
}
}
if (allsections || defsections || !strcasecmp(section,"cluster")) {
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info,
"#Cluster\r\n"
"cluster_enabled:%d\r\n",
server.cluster_enabled);
}
if (allsections || defsections || !strcasecmp(section,"keyspace")) {
if (sections++) info = sdscat(info,"\r\n");
info = sdscatprintf(info, "#Keyspace\r\n");
for (j = 0; j < server.dbnum; j++) {
long long keys, vkeys;
keys = dictSize(server.db[j].dict);
vkeys = dictSize(server.db[j].expires);
if (keys || vkeys) {
info = sdscatprintf(info,
"db%d:keys=%lld,expires=%lld,avg_ttl=%lld\r\n",
j, keys, vkeys, server.db[j].avg_ttl);
}
}
}
if (everything || modules ||
(!allsections && !defsections && sections==0)) {
info = modulesCollectInfo(info,
everything || modules ? NULL: section,
0,
sections);
}
return info;
}
void infoCommand(client *c) {
if (server.sentinel_mode) {
sentinelInfoCommand(c);
return;
}
char *section = c->argc == 2 ? c->argv[1]->ptr : "default";
if (c->argc > 2) {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
sds info = genRedisInfoString(section);
addReplyVerbatim(c,info,sdslen(info),"txt");
sdsfree(info);
}
void monitorCommand(client *c) {
if (c->flags & CLIENT_DENY_BLOCKING) {
addReplyError(c, "MONITOR isn't allowed for DENY BLOCKING client");
return;
}
if (c->flags & CLIENT_SLAVE) return;
c->flags |= (CLIENT_SLAVE|CLIENT_MONITOR);
listAddNodeTail(server.monitors,c);
addReply(c,shared.ok);
}
int checkIgnoreWarning(const char *warning) {
int argc, j;
sds *argv = sdssplitargs(server.ignore_warnings, &argc);
if (argv == NULL)
return 0;
for (j = 0; j < argc; j++) {
char *flag = argv[j];
if (!strcasecmp(flag, warning))
break;
}
sdsfreesplitres(argv,argc);
return j < argc;
}
#if defined(__linux__)
int linuxOvercommitMemoryValue(void) {
FILE *fp = fopen("/proc/sys/vm/overcommit_memory","r");
char buf[64];
if (!fp) return -1;
if (fgets(buf,64,fp) == NULL) {
fclose(fp);
return -1;
}
fclose(fp);
return atoi(buf);
}
void linuxMemoryWarnings(void) {
if (linuxOvercommitMemoryValue() == 0) {
serverLog(LL_WARNING,"WARNING overcommit_memory is set to 0! Background save may fail under low memory condition. To fix this issue add 'vm.overcommit_memory = 1' to /etc/sysctl.conf and then reboot or run the command 'sysctl vm.overcommit_memory=1' for this to take effect.");
}
if (THPIsEnabled()) {
server.thp_enabled = 1;
if (THPDisable() == 0) {
server.thp_enabled = 0;
return;
}
serverLog(LL_WARNING,"WARNING you have Transparent Huge Pages (THP) support enabled in your kernel. This will create latency and memory usage issues with Redis. To fix this issue run the command 'echo madvise > /sys/kernel/mm/transparent_hugepage/enabled' as root, and add it to your /etc/rc.local in order to retain the setting after a reboot. Redis must be restarted after THP is disabled (set to 'madvise' or 'never').");
}
}
#if defined(__arm64__)
static int smapsGetSharedDirty(unsigned long addr) {
int ret, in_mapping = 0, val = -1;
unsigned long from, to;
char buf[64];
FILE *f;
f = fopen("/proc/self/smaps", "r");
if (!f) return -1;
while (1) {
if (!fgets(buf, sizeof(buf), f))
break;
ret = sscanf(buf, "%lx-%lx", &from, &to);
if (ret == 2)
in_mapping = from <= addr && addr < to;
if (in_mapping && !memcmp(buf, "Shared_Dirty:", 13)) {
sscanf(buf, "%*s %d", &val);
break;
}
}
fclose(f);
return val;
}
int linuxMadvFreeForkBugCheck(void) {
int ret, pipefd[2] = { -1, -1 };
pid_t pid;
char *p = NULL, *q;
int bug_found = 0;
long page_size = sysconf(_SC_PAGESIZE);
long map_size = 3 * page_size;
p = mmap(NULL, map_size, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
if (p == MAP_FAILED) {
serverLog(LL_WARNING, "Failed to mmap(): %s", strerror(errno));
return -1;
}
q = p + page_size;
ret = mprotect(q, page_size, PROT_READ | PROT_WRITE);
if (ret < 0) {
serverLog(LL_WARNING, "Failed to mprotect(): %s", strerror(errno));
bug_found = -1;
goto exit;
}
*(volatile char*)q = 0;
#if !defined(MADV_FREE)
#define MADV_FREE 8
#endif
ret = madvise(q, page_size, MADV_FREE);
if (ret < 0) {
if (errno == EINVAL) goto exit;
serverLog(LL_WARNING, "Failed to madvise(): %s", strerror(errno));
bug_found = -1;
goto exit;
}
*(volatile char*)q = 0;
ret = anetPipe(pipefd, 0, 0);
if (ret < 0) {
serverLog(LL_WARNING, "Failed to create pipe: %s", strerror(errno));
bug_found = -1;
goto exit;
}
pid = fork();
if (pid < 0) {
serverLog(LL_WARNING, "Failed to fork: %s", strerror(errno));
bug_found = -1;
goto exit;
} else if (!pid) {
ret = smapsGetSharedDirty((unsigned long) q);
if (!ret)
bug_found = 1;
else if (ret == -1)
bug_found = -1;
if (write(pipefd[1], &bug_found, sizeof(bug_found)) < 0)
serverLog(LL_WARNING, "Failed to write to parent: %s", strerror(errno));
exitFromChild(0);
} else {
ret = read(pipefd[0], &bug_found, sizeof(bug_found));
if (ret < 0) {
serverLog(LL_WARNING, "Failed to read from child: %s", strerror(errno));
bug_found = -1;
}
waitpid(pid, NULL, 0);
}
exit:
if (pipefd[0] != -1) close(pipefd[0]);
if (pipefd[1] != -1) close(pipefd[1]);
if (p != NULL) munmap(p, map_size);
return bug_found;
}
#endif
#endif
void createPidFile(void) {
if (!server.pidfile) server.pidfile = zstrdup(CONFIG_DEFAULT_PID_FILE);
FILE *fp = fopen(server.pidfile,"w");
if (fp) {
fprintf(fp,"%d\n",(int)getpid());
fclose(fp);
}
}
void daemonize(void) {
int fd;
if (fork() != 0) exit(0);
setsid();
if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
dup2(fd, STDIN_FILENO);
dup2(fd, STDOUT_FILENO);
dup2(fd, STDERR_FILENO);
if (fd > STDERR_FILENO) close(fd);
}
}
void version(void) {
printf("Redis server v=%s sha=%s:%d malloc=%s bits=%d build=%llx\n",
REDIS_VERSION,
redisGitSHA1(),
atoi(redisGitDirty()) > 0,
ZMALLOC_LIB,
sizeof(long) == 4 ? 32 : 64,
(unsigned long long) redisBuildId());
exit(0);
}
void usage(void) {
fprintf(stderr,"Usage: ./redis-server [/path/to/redis.conf] [options] [-]\n");
fprintf(stderr," ./redis-server - (read config from stdin)\n");
fprintf(stderr," ./redis-server -v or --version\n");
fprintf(stderr," ./redis-server -h or --help\n");
fprintf(stderr," ./redis-server --test-memory <megabytes>\n\n");
fprintf(stderr,"Examples:\n");
fprintf(stderr," ./redis-server (run the server with default conf)\n");
fprintf(stderr," ./redis-server /etc/redis/6379.conf\n");
fprintf(stderr," ./redis-server --port 7777\n");
fprintf(stderr," ./redis-server --port 7777 --replicaof 127.0.0.1 8888\n");
fprintf(stderr," ./redis-server /etc/myredis.conf --loglevel verbose -\n");
fprintf(stderr," ./redis-server /etc/myredis.conf --loglevel verbose\n\n");
fprintf(stderr,"Sentinel mode:\n");
fprintf(stderr," ./redis-server /etc/sentinel.conf --sentinel\n");
exit(1);
}
void redisAsciiArt(void) {
#include "asciilogo.h"
char *buf = zmalloc(1024*16);
char *mode;
if (server.cluster_enabled) mode = "cluster";
else if (server.sentinel_mode) mode = "sentinel";
else mode = "standalone";
int show_logo = ((!server.syslog_enabled &&
server.logfile[0] == '\0' &&
isatty(fileno(stdout))) ||
server.always_show_logo);
if (!show_logo) {
serverLog(LL_NOTICE,
"Running mode=%s, port=%d.",
mode, server.port ? server.port : server.tls_port
);
} else {
snprintf(buf,1024*16,ascii_logo,
REDIS_VERSION,
redisGitSHA1(),
strtol(redisGitDirty(),NULL,10) > 0,
(sizeof(long) == 8) ? "64" : "32",
mode, server.port ? server.port : server.tls_port,
(long) getpid()
);
serverLogRaw(LL_NOTICE|LL_RAW,buf);
}
zfree(buf);
}
int changeBindAddr(void) {
closeSocketListeners(&server.ipfd);
closeSocketListeners(&server.tlsfd);
if ((server.port != 0 && listenToPort(server.port, &server.ipfd) != C_OK) ||
(server.tls_port != 0 && listenToPort(server.tls_port, &server.tlsfd) != C_OK)) {
serverLog(LL_WARNING, "Failed to bind");
closeSocketListeners(&server.ipfd);
closeSocketListeners(&server.tlsfd);
return C_ERR;
}
if (createSocketAcceptHandler(&server.ipfd, acceptTcpHandler) != C_OK) {
serverPanic("Unrecoverable error creating TCP socket accept handler.");
}
if (createSocketAcceptHandler(&server.tlsfd, acceptTLSHandler) != C_OK) {
serverPanic("Unrecoverable error creating TLS socket accept handler.");
}
if (server.set_proc_title) redisSetProcTitle(NULL);
return C_OK;
}
int changeListenPort(int port, socketFds *sfd, aeFileProc *accept_handler) {
socketFds new_sfd = {{0}};
closeSocketListeners(sfd);
if (port == 0) {
if (server.set_proc_title) redisSetProcTitle(NULL);
return C_OK;
}
if (listenToPort(port, &new_sfd) != C_OK) {
return C_ERR;
}
if (createSocketAcceptHandler(&new_sfd, accept_handler) != C_OK) {
closeSocketListeners(&new_sfd);
return C_ERR;
}
sfd->count = new_sfd.count;
memcpy(sfd->fd, new_sfd.fd, sizeof(new_sfd.fd));
if (server.set_proc_title) redisSetProcTitle(NULL);
return C_OK;
}
static void sigShutdownHandler(int sig) {
char *msg;
switch (sig) {
case SIGINT:
msg = "Received SIGINT scheduling shutdown...";
break;
case SIGTERM:
msg = "Received SIGTERM scheduling shutdown...";
break;
default:
msg = "Received shutdown signal, scheduling shutdown...";
};
if (server.shutdown_asap && sig == SIGINT) {
serverLogFromHandler(LL_WARNING, "You insist... exiting now.");
rdbRemoveTempFile(getpid(), 1);
exit(1);
} else if (server.loading) {
msg = "Received shutdown signal during loading, scheduling shutdown.";
}
serverLogFromHandler(LL_WARNING, msg);
server.shutdown_asap = 1;
}
void setupSignalHandlers(void) {
struct sigaction act;
sigemptyset(&act.sa_mask);
act.sa_flags = 0;
act.sa_handler = sigShutdownHandler;
sigaction(SIGTERM, &act, NULL);
sigaction(SIGINT, &act, NULL);
sigemptyset(&act.sa_mask);
act.sa_flags = SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
act.sa_sigaction = sigsegvHandler;
if(server.crashlog_enabled) {
sigaction(SIGSEGV, &act, NULL);
sigaction(SIGBUS, &act, NULL);
sigaction(SIGFPE, &act, NULL);
sigaction(SIGILL, &act, NULL);
sigaction(SIGABRT, &act, NULL);
}
return;
}
void removeSignalHandlers(void) {
struct sigaction act;
sigemptyset(&act.sa_mask);
act.sa_flags = SA_NODEFER | SA_RESETHAND;
act.sa_handler = SIG_DFL;
sigaction(SIGSEGV, &act, NULL);
sigaction(SIGBUS, &act, NULL);
sigaction(SIGFPE, &act, NULL);
sigaction(SIGILL, &act, NULL);
sigaction(SIGABRT, &act, NULL);
}
static void sigKillChildHandler(int sig) {
UNUSED(sig);
int level = server.in_fork_child == CHILD_TYPE_MODULE? LL_VERBOSE: LL_WARNING;
serverLogFromHandler(level, "Received SIGUSR1 in child, exiting now.");
exitFromChild(SERVER_CHILD_NOERROR_RETVAL);
}
void setupChildSignalHandlers(void) {
struct sigaction act;
sigemptyset(&act.sa_mask);
act.sa_flags = 0;
act.sa_handler = sigKillChildHandler;
sigaction(SIGUSR1, &act, NULL);
}
void closeChildUnusedResourceAfterFork() {
closeListeningSockets(0);
if (server.cluster_enabled && server.cluster_config_file_lock_fd != -1)
close(server.cluster_config_file_lock_fd);
zfree(server.pidfile);
server.pidfile = NULL;
}
int redisFork(int purpose) {
if (isMutuallyExclusiveChildType(purpose)) {
if (hasActiveChildProcess()) {
errno = EEXIST;
return -1;
}
openChildInfoPipe();
}
int childpid;
long long start = ustime();
if ((childpid = fork()) == 0) {
server.in_fork_child = purpose;
setupChildSignalHandlers();
setOOMScoreAdj(CONFIG_OOM_BGCHILD);
dismissMemoryInChild();
closeChildUnusedResourceAfterFork();
} else {
if (childpid == -1) {
int fork_errno = errno;
if (isMutuallyExclusiveChildType(purpose)) closeChildInfoPipe();
errno = fork_errno;
return -1;
}
server.stat_total_forks++;
server.stat_fork_time = ustime()-start;
server.stat_fork_rate = (double) zmalloc_used_memory() * 1000000 / server.stat_fork_time / (1024*1024*1024);
latencyAddSampleIfNeeded("fork",server.stat_fork_time/1000);
if (isMutuallyExclusiveChildType(purpose)) {
server.child_pid = childpid;
server.child_type = purpose;
server.stat_current_cow_peak = 0;
server.stat_current_cow_bytes = 0;
server.stat_current_cow_updated = 0;
server.stat_current_save_keys_processed = 0;
server.stat_module_progress = 0;
server.stat_current_save_keys_total = dbTotalServerKeyCount();
}
updateDictResizePolicy();
moduleFireServerEvent(REDISMODULE_EVENT_FORK_CHILD,
REDISMODULE_SUBEVENT_FORK_CHILD_BORN,
NULL);
}
return childpid;
}
void sendChildCowInfo(childInfoType info_type, char *pname) {
sendChildInfoGeneric(info_type, 0, -1, pname);
}
void sendChildInfo(childInfoType info_type, size_t keys, char *pname) {
sendChildInfoGeneric(info_type, keys, -1, pname);
}
void dismissMemory(void* ptr, size_t size_hint) {
if (ptr == NULL) return;
if (size_hint && size_hint <= server.page_size/2) return;
zmadvise_dontneed(ptr);
}
void dismissClientMemory(client *c) {
dismissSds(c->querybuf);
dismissSds(c->pending_querybuf);
if (c->argc && c->argv_len_sum/c->argc >= server.page_size) {
for (int i = 0; i < c->argc; i++) {
dismissObject(c->argv[i], 0);
}
}
if (c->argc) dismissMemory(c->argv, c->argc*sizeof(robj*));
if (listLength(c->reply) &&
c->reply_bytes/listLength(c->reply) >= server.page_size)
{
listIter li;
listNode *ln;
listRewind(c->reply, &li);
while ((ln = listNext(&li))) {
clientReplyBlock *bulk = listNodeValue(ln);
if (bulk) dismissMemory(bulk, bulk->size);
}
}
dismissMemory(c, 0);
}
void dismissMemoryInChild(void) {
if (server.thp_enabled) return;
#if defined(USE_JEMALLOC) && defined(__linux__)
listIter li;
listNode *ln;
listRewind(server.repl_buffer_blocks, &li);
while((ln = listNext(&li))) {
replBufBlock *o = listNodeValue(ln);
dismissMemory(o, o->size);
}
listRewind(server.clients, &li);
while((ln = listNext(&li))) {
client *c = listNodeValue(ln);
dismissClientMemory(c);
}
#endif
}
void memtest(size_t megabytes, int passes);
int checkForSentinelMode(int argc, char **argv, char *exec_name) {
if (strstr(exec_name,"redis-sentinel") != NULL) return 1;
for (int j = 1; j < argc; j++)
if (!strcmp(argv[j],"--sentinel")) return 1;
return 0;
}
void loadDataFromDisk(void) {
long long start = ustime();
if (server.aof_state == AOF_ON) {
int ret = loadAppendOnlyFiles(server.aof_manifest);
if (ret == AOF_FAILED || ret == AOF_OPEN_ERR)
exit(1);
} else {
rdbSaveInfo rsi = RDB_SAVE_INFO_INIT;
errno = 0;
int rdb_flags = RDBFLAGS_NONE;
if (iAmMaster()) {
createReplicationBacklog();
rdb_flags |= RDBFLAGS_FEED_REPL;
}
if (rdbLoad(server.rdb_filename,&rsi,rdb_flags) == C_OK) {
serverLog(LL_NOTICE,"DB loaded from disk: %.3f seconds",
(float)(ustime()-start)/1000000);
if (rsi.repl_id_is_set &&
rsi.repl_offset != -1 &&
rsi.repl_stream_db != -1)
{
if (!iAmMaster()) {
memcpy(server.replid,rsi.repl_id,sizeof(server.replid));
server.master_repl_offset = rsi.repl_offset;
replicationCacheMasterUsingMyself();
selectDb(server.cached_master,rsi.repl_stream_db);
} else {
memcpy(server.replid2,rsi.repl_id,sizeof(server.replid));
server.second_replid_offset = rsi.repl_offset+1;
server.master_repl_offset += rsi.repl_offset;
serverAssert(server.repl_backlog);
server.repl_backlog->offset = server.master_repl_offset -
server.repl_backlog->histlen + 1;
rebaseReplicationBuffer(rsi.repl_offset);
server.repl_no_slaves_since = time(NULL);
}
}
} else if (errno != ENOENT) {
serverLog(LL_WARNING,"Fatal error loading the DB: %s. Exiting.",strerror(errno));
exit(1);
}
if (server.master_repl_offset == 0 && server.repl_backlog)
freeReplicationBacklog();
}
}
void redisOutOfMemoryHandler(size_t allocation_size) {
serverLog(LL_WARNING,"Out Of Memory allocating %zu bytes!",
allocation_size);
serverPanic("Redis aborting for OUT OF MEMORY. Allocating %zu bytes!",
allocation_size);
}
static sds redisProcTitleGetVariable(const sds varname, void *arg)
{
if (!strcmp(varname, "title")) {
return sdsnew(arg);
} else if (!strcmp(varname, "listen-addr")) {
if (server.port || server.tls_port)
return sdscatprintf(sdsempty(), "%s:%u",
server.bindaddr_count ? server.bindaddr[0] : "*",
server.port ? server.port : server.tls_port);
else
return sdscatprintf(sdsempty(), "unixsocket:%s", server.unixsocket);
} else if (!strcmp(varname, "server-mode")) {
if (server.cluster_enabled) return sdsnew("[cluster]");
else if (server.sentinel_mode) return sdsnew("[sentinel]");
else return sdsempty();
} else if (!strcmp(varname, "config-file")) {
return sdsnew(server.configfile ? server.configfile : "-");
} else if (!strcmp(varname, "port")) {
return sdscatprintf(sdsempty(), "%u", server.port);
} else if (!strcmp(varname, "tls-port")) {
return sdscatprintf(sdsempty(), "%u", server.tls_port);
} else if (!strcmp(varname, "unixsocket")) {
return sdsnew(server.unixsocket);
} else
return NULL;
}
static sds expandProcTitleTemplate(const char *template, const char *title) {
sds res = sdstemplate(template, redisProcTitleGetVariable, (void *) title);
if (!res)
return NULL;
return sdstrim(res, " ");
}
int validateProcTitleTemplate(const char *template) {
int ok = 1;
sds res = expandProcTitleTemplate(template, "");
if (!res)
return 0;
if (sdslen(res) == 0) ok = 0;
sdsfree(res);
return ok;
}
int redisSetProcTitle(char *title) {
#if defined(USE_SETPROCTITLE)
if (!title) title = server.exec_argv[0];
sds proc_title = expandProcTitleTemplate(server.proc_title_template, title);
if (!proc_title) return C_ERR;
setproctitle("%s", proc_title);
sdsfree(proc_title);
#else
UNUSED(title);
#endif
return C_OK;
}
void redisSetCpuAffinity(const char *cpulist) {
#if defined(USE_SETCPUAFFINITY)
setcpuaffinity(cpulist);
#else
UNUSED(cpulist);
#endif
}
int redisCommunicateSystemd(const char *sd_notify_msg) {
#if defined(HAVE_LIBSYSTEMD)
int ret = sd_notify(0, sd_notify_msg);
if (ret == 0)
serverLog(LL_WARNING, "systemd supervision error: NOTIFY_SOCKET not found!");
else if (ret < 0)
serverLog(LL_WARNING, "systemd supervision error: sd_notify: %d", ret);
return ret;
#else
UNUSED(sd_notify_msg);
return 0;
#endif
}
static int redisSupervisedUpstart(void) {
const char *upstart_job = getenv("UPSTART_JOB");
if (!upstart_job) {
serverLog(LL_WARNING,
"upstart supervision requested, but UPSTART_JOB not found!");
return 0;
}
serverLog(LL_NOTICE, "supervised by upstart, will stop to signal readiness.");
raise(SIGSTOP);
unsetenv("UPSTART_JOB");
return 1;
}
static int redisSupervisedSystemd(void) {
#if !defined(HAVE_LIBSYSTEMD)
serverLog(LL_WARNING,
"systemd supervision requested or auto-detected, but Redis is compiled without libsystemd support!");
return 0;
#else
if (redisCommunicateSystemd("STATUS=Redis is loading...\n") <= 0)
return 0;
serverLog(LL_NOTICE,
"Supervised by systemd. Please make sure you set appropriate values for TimeoutStartSec and TimeoutStopSec in your service unit.");
return 1;
#endif
}
int redisIsSupervised(int mode) {
int ret = 0;
if (mode == SUPERVISED_AUTODETECT) {
if (getenv("UPSTART_JOB")) {
serverLog(LL_VERBOSE, "Upstart supervision detected.");
mode = SUPERVISED_UPSTART;
} else if (getenv("NOTIFY_SOCKET")) {
serverLog(LL_VERBOSE, "Systemd supervision detected.");
mode = SUPERVISED_SYSTEMD;
}
}
switch (mode) {
case SUPERVISED_UPSTART:
ret = redisSupervisedUpstart();
break;
case SUPERVISED_SYSTEMD:
ret = redisSupervisedSystemd();
break;
default:
break;
}
if (ret)
server.supervised_mode = mode;
return ret;
}
int iAmMaster(void) {
return ((!server.cluster_enabled && server.masterhost == NULL) ||
(server.cluster_enabled && nodeIsMaster(server.cluster->myself)));
}
#if defined(REDIS_TEST)
#include "testhelp.h"
int __failed_tests = 0;
int __test_num = 0;
typedef int redisTestProc(int argc, char **argv, int flags);
struct redisTest {
char *name;
redisTestProc *proc;
int failed;
} redisTests[] = {
{"ziplist", ziplistTest},
{"quicklist", quicklistTest},
{"intset", intsetTest},
{"zipmap", zipmapTest},
{"sha1test", sha1Test},
{"util", utilTest},
{"endianconv", endianconvTest},
{"crc64", crc64Test},
{"zmalloc", zmalloc_test},
{"sds", sdsTest},
{"dict", dictTest},
{"listpack", listpackTest}
};
redisTestProc *getTestProcByName(const char *name) {
int numtests = sizeof(redisTests)/sizeof(struct redisTest);
for (int j = 0; j < numtests; j++) {
if (!strcasecmp(name,redisTests[j].name)) {
return redisTests[j].proc;
}
}
return NULL;
}
#endif
int main(int argc, char **argv) {
struct timeval tv;
int j;
char config_from_stdin = 0;
#if defined(REDIS_TEST)
if (argc >= 3 && !strcasecmp(argv[1], "test")) {
int flags = 0;
for (j = 3; j < argc; j++) {
char *arg = argv[j];
if (!strcasecmp(arg, "--accurate")) flags |= REDIS_TEST_ACCURATE;
else if (!strcasecmp(arg, "--large-memory")) flags |= REDIS_TEST_LARGE_MEMORY;
}
if (!strcasecmp(argv[2], "all")) {
int numtests = sizeof(redisTests)/sizeof(struct redisTest);
for (j = 0; j < numtests; j++) {
redisTests[j].failed = (redisTests[j].proc(argc,argv,flags) != 0);
}
int failed_num = 0;
for (j = 0; j < numtests; j++) {
if (redisTests[j].failed) {
failed_num++;
printf("[failed] Test - %s\n", redisTests[j].name);
} else {
printf("[ok] Test - %s\n", redisTests[j].name);
}
}
printf("%d tests, %d passed, %d failed\n", numtests,
numtests-failed_num, failed_num);
return failed_num == 0 ? 0 : 1;
} else {
redisTestProc *proc = getTestProcByName(argv[2]);
if (!proc) return -1;
return proc(argc,argv,flags);
}
return 0;
}
#endif
#if defined(INIT_SETPROCTITLE_REPLACEMENT)
spt_init(argc, argv);
#endif
setlocale(LC_COLLATE,"");
tzset();
zmalloc_set_oom_handler(redisOutOfMemoryHandler);
srand(time(NULL)^getpid());
srandom(time(NULL)^getpid());
gettimeofday(&tv,NULL);
init_genrand64(((long long) tv.tv_sec * 1000000 + tv.tv_usec) ^ getpid());
crc64_init();
umask(server.umask = umask(0777));
uint8_t hashseed[16];
getRandomBytes(hashseed,sizeof(hashseed));
dictSetHashFunctionSeed(hashseed);
char *exec_name = strrchr(argv[0], '/');
if (exec_name == NULL) exec_name = argv[0];
server.sentinel_mode = checkForSentinelMode(argc,argv, exec_name);
initServerConfig();
ACLInit();
moduleInitModulesSystem();
tlsInit();
server.executable = getAbsolutePath(argv[0]);
server.exec_argv = zmalloc(sizeof(char*)*(argc+1));
server.exec_argv[argc] = NULL;
for (j = 0; j < argc; j++) server.exec_argv[j] = zstrdup(argv[j]);
if (server.sentinel_mode) {
initSentinelConfig();
initSentinel();
}
if (strstr(exec_name,"redis-check-rdb") != NULL)
redis_check_rdb_main(argc,argv,NULL);
else if (strstr(exec_name,"redis-check-aof") != NULL)
redis_check_aof_main(argc,argv);
if (argc >= 2) {
j = 1;
sds options = sdsempty();
if (strcmp(argv[1], "-v") == 0 ||
strcmp(argv[1], "--version") == 0) version();
if (strcmp(argv[1], "--help") == 0 ||
strcmp(argv[1], "-h") == 0) usage();
if (strcmp(argv[1], "--test-memory") == 0) {
if (argc == 3) {
memtest(atoi(argv[2]),50);
exit(0);
} else {
fprintf(stderr,"Please specify the amount of memory to test in megabytes.\n");
fprintf(stderr,"Example: ./redis-server --test-memory 4096\n\n");
exit(1);
}
}
if (argv[1][0] != '-') {
server.configfile = getAbsolutePath(argv[1]);
zfree(server.exec_argv[1]);
server.exec_argv[1] = zstrdup(server.configfile);
j = 2;
}
while(j < argc) {
if (argv[j][0] == '-' && argv[j][1] == '\0' && (j == 1 || j == argc-1)) {
config_from_stdin = 1;
}
else if (argv[j][0] == '-' && argv[j][1] == '-') {
if (sdslen(options)) options = sdscat(options,"\n");
options = sdscat(options,argv[j]+2);
options = sdscat(options," ");
} else {
options = sdscatrepr(options,argv[j],strlen(argv[j]));
options = sdscat(options," ");
}
j++;
}
loadServerConfig(server.configfile, config_from_stdin, options);
if (server.sentinel_mode) loadSentinelConfigFromQueue();
sdsfree(options);
}
if (server.sentinel_mode) sentinelCheckConfigFile();
server.supervised = redisIsSupervised(server.supervised_mode);
int background = server.daemonize && !server.supervised;
if (background) daemonize();
serverLog(LL_WARNING, "oO0OoO0OoO0Oo Redis is starting oO0OoO0OoO0Oo");
serverLog(LL_WARNING,
"Redis version=%s, bits=%d, commit=%s, modified=%d, pid=%d, just started",
REDIS_VERSION,
(sizeof(long) == 8) ? 64 : 32,
redisGitSHA1(),
strtol(redisGitDirty(),NULL,10) > 0,
(int)getpid());
if (argc == 1) {
serverLog(LL_WARNING, "Warning: no config file specified, using the default config. In order to specify a config file use %s /path/to/redis.conf", argv[0]);
} else {
serverLog(LL_WARNING, "Configuration loaded");
}
initServer();
if (background || server.pidfile) createPidFile();
if (server.set_proc_title) redisSetProcTitle(NULL);
redisAsciiArt();
checkTcpBacklogSettings();
if (!server.sentinel_mode) {
serverLog(LL_WARNING,"Server initialized");
#if defined(__linux__)
linuxMemoryWarnings();
#if defined (__arm64__)
int ret;
if ((ret = linuxMadvFreeForkBugCheck())) {
if (ret == 1)
serverLog(LL_WARNING,"WARNING Your kernel has a bug that could lead to data corruption during background save. "
"Please upgrade to the latest stable kernel.");
else
serverLog(LL_WARNING, "Failed to test the kernel for a bug that could lead to data corruption during background save. "
"Your system could be affected, please report this error.");
if (!checkIgnoreWarning("ARM64-COW-BUG")) {
serverLog(LL_WARNING,"Redis will now exit to prevent data corruption. "
"Note that it is possible to suppress this warning by setting the following config: ignore-warnings ARM64-COW-BUG");
exit(1);
}
}
#endif
#endif
moduleInitModulesSystemLast();
moduleLoadFromQueue();
ACLLoadUsersAtStartup();
InitServerLast();
aofLoadManifestFromDisk();
loadDataFromDisk();
aofOpenIfNeededOnServerStart();
aofDelHistoryFiles();
if (server.cluster_enabled) {
if (verifyClusterConfigWithData() == C_ERR) {
serverLog(LL_WARNING,
"You can't have keys in a DB different than DB 0 when in "
"Cluster mode. Exiting.");
exit(1);
}
}
if (server.ipfd.count > 0 || server.tlsfd.count > 0)
serverLog(LL_NOTICE,"Ready to accept connections");
if (server.sofd > 0)
serverLog(LL_NOTICE,"The server is now ready to accept connections at %s", server.unixsocket);
if (server.supervised_mode == SUPERVISED_SYSTEMD) {
if (!server.masterhost) {
redisCommunicateSystemd("STATUS=Ready to accept connections\n");
} else {
redisCommunicateSystemd("STATUS=Ready to accept connections in read-only mode. Waiting for MASTER <-> REPLICA sync\n");
}
redisCommunicateSystemd("READY=1\n");
}
} else {
ACLLoadUsersAtStartup();
InitServerLast();
sentinelIsRunning();
if (server.supervised_mode == SUPERVISED_SYSTEMD) {
redisCommunicateSystemd("STATUS=Ready to accept connections\n");
redisCommunicateSystemd("READY=1\n");
}
}
if (server.maxmemory > 0 && server.maxmemory < 1024*1024) {
serverLog(LL_WARNING,"WARNING: You specified a maxmemory value that is less than 1MB (current value is %llu bytes). Are you sure this is what you really want?", server.maxmemory);
}
redisSetCpuAffinity(server.server_cpulist);
setOOMScoreAdj(-1);
aeMain(server.el);
aeDeleteEventLoop(server.el);
return 0;
}
