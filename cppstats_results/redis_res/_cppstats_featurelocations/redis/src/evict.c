































#include "server.h"
#include "bio.h"
#include "atomicvar.h"
#include "script.h"
#include <math.h>
















#define EVPOOL_SIZE 16
#define EVPOOL_CACHED_SDS_SIZE 255
struct evictionPoolEntry {
unsigned long long idle;
sds key;
sds cached;
int dbid;
};

static struct evictionPoolEntry *EvictionPoolLRU;








unsigned int getLRUClock(void) {
return (mstime()/LRU_CLOCK_RESOLUTION) & LRU_CLOCK_MAX;
}





unsigned int LRU_CLOCK(void) {
unsigned int lruclock;
if (1000/server.hz <= LRU_CLOCK_RESOLUTION) {
atomicGet(server.lruclock,lruclock);
} else {
lruclock = getLRUClock();
}
return lruclock;
}



unsigned long long estimateObjectIdleTime(robj *o) {
unsigned long long lruclock = LRU_CLOCK();
if (lruclock >= o->lru) {
return (lruclock - o->lru) * LRU_CLOCK_RESOLUTION;
} else {
return (lruclock + (LRU_CLOCK_MAX - o->lru)) *
LRU_CLOCK_RESOLUTION;
}
}






















void evictionPoolAlloc(void) {
struct evictionPoolEntry *ep;
int j;

ep = zmalloc(sizeof(*ep)*EVPOOL_SIZE);
for (j = 0; j < EVPOOL_SIZE; j++) {
ep[j].idle = 0;
ep[j].key = NULL;
ep[j].cached = sdsnewlen(NULL,EVPOOL_CACHED_SDS_SIZE);
ep[j].dbid = 0;
}
EvictionPoolLRU = ep;
}










void evictionPoolPopulate(int dbid, dict *sampledict, dict *keydict, struct evictionPoolEntry *pool) {
int j, k, count;
dictEntry *samples[server.maxmemory_samples];

count = dictGetSomeKeys(sampledict,samples,server.maxmemory_samples);
for (j = 0; j < count; j++) {
unsigned long long idle;
sds key;
robj *o;
dictEntry *de;

de = samples[j];
key = dictGetKey(de);




if (server.maxmemory_policy != MAXMEMORY_VOLATILE_TTL) {
if (sampledict != keydict) de = dictFind(keydict, key);
o = dictGetVal(de);
}




if (server.maxmemory_policy & MAXMEMORY_FLAG_LRU) {
idle = estimateObjectIdleTime(o);
} else if (server.maxmemory_policy & MAXMEMORY_FLAG_LFU) {







idle = 255-LFUDecrAndReturn(o);
} else if (server.maxmemory_policy == MAXMEMORY_VOLATILE_TTL) {

idle = ULLONG_MAX - (long)dictGetVal(de);
} else {
serverPanic("Unknown eviction policy in evictionPoolPopulate()");
}




k = 0;
while (k < EVPOOL_SIZE &&
pool[k].key &&
pool[k].idle < idle) k++;
if (k == 0 && pool[EVPOOL_SIZE-1].key != NULL) {


continue;
} else if (k < EVPOOL_SIZE && pool[k].key == NULL) {

} else {


if (pool[EVPOOL_SIZE-1].key == NULL) {




sds cached = pool[EVPOOL_SIZE-1].cached;
memmove(pool+k+1,pool+k,
sizeof(pool[0])*(EVPOOL_SIZE-k-1));
pool[k].cached = cached;
} else {

k--;


sds cached = pool[0].cached;
if (pool[0].key != pool[0].cached) sdsfree(pool[0].key);
memmove(pool,pool+1,sizeof(pool[0])*k);
pool[k].cached = cached;
}
}





int klen = sdslen(key);
if (klen > EVPOOL_CACHED_SDS_SIZE) {
pool[k].key = sdsdup(key);
} else {
memcpy(pool[k].cached,key,klen+1);
sdssetlen(pool[k].cached,klen);
pool[k].key = pool[k].cached;
}
pool[k].idle = idle;
pool[k].dbid = dbid;
}
}









































unsigned long LFUGetTimeInMinutes(void) {
return (server.unixtime/60) & 65535;
}





unsigned long LFUTimeElapsed(unsigned long ldt) {
unsigned long now = LFUGetTimeInMinutes();
if (now >= ldt) return now-ldt;
return 65535-ldt+now;
}



uint8_t LFULogIncr(uint8_t counter) {
if (counter == 255) return 255;
double r = (double)rand()/RAND_MAX;
double baseval = counter - LFU_INIT_VAL;
if (baseval < 0) baseval = 0;
double p = 1.0/(baseval*server.lfu_log_factor+1);
if (r < p) counter++;
return counter;
}











unsigned long LFUDecrAndReturn(robj *o) {
unsigned long ldt = o->lru >> 8;
unsigned long counter = o->lru & 255;
unsigned long num_periods = server.lfu_decay_time ? LFUTimeElapsed(ldt) / server.lfu_decay_time : 0;
if (num_periods)
counter = (num_periods > counter) ? 0 : counter - num_periods;
return counter;
}









size_t freeMemoryGetNotCountedMemory(void) {
size_t overhead = 0;
















if ((long long)server.repl_buffer_mem > server.repl_backlog_size) {



size_t extra_approx_size =
(server.repl_backlog_size/PROTO_REPLY_CHUNK_BYTES + 1) *
(sizeof(replBufBlock)+sizeof(listNode));
size_t counted_mem = server.repl_backlog_size + extra_approx_size;
if (server.repl_buffer_mem > counted_mem) {
overhead += (server.repl_buffer_mem - counted_mem);
}
}

if (server.aof_state != AOF_OFF) {
overhead += sdsAllocSize(server.aof_buf);
}
return overhead;
}

























int getMaxmemoryState(size_t *total, size_t *logical, size_t *tofree, float *level) {
size_t mem_reported, mem_used, mem_tofree;



mem_reported = zmalloc_used_memory();
if (total) *total = mem_reported;


if (!server.maxmemory) {
if (level) *level = 0;
return C_OK;
}
if (mem_reported <= server.maxmemory && !level) return C_OK;



mem_used = mem_reported;
size_t overhead = freeMemoryGetNotCountedMemory();
mem_used = (mem_used > overhead) ? mem_used-overhead : 0;


if (level) *level = (float)mem_used / (float)server.maxmemory;

if (mem_reported <= server.maxmemory) return C_OK;


if (mem_used <= server.maxmemory) return C_OK;


mem_tofree = mem_used - server.maxmemory;

if (logical) *logical = mem_used;
if (tofree) *tofree = mem_tofree;

return C_ERR;
}




int overMaxmemoryAfterAlloc(size_t moremem) {
if (!server.maxmemory) return 0;


size_t mem_used = zmalloc_used_memory();
if (mem_used + moremem <= server.maxmemory) return 0;

size_t overhead = freeMemoryGetNotCountedMemory();
mem_used = (mem_used > overhead) ? mem_used - overhead : 0;
return mem_used + moremem > server.maxmemory;
}





static int isEvictionProcRunning = 0;
static int evictionTimeProc(
struct aeEventLoop *eventLoop, long long id, void *clientData) {
UNUSED(eventLoop);
UNUSED(id);
UNUSED(clientData);

if (performEvictions() == EVICT_RUNNING) return 0;



isEvictionProcRunning = 0;
return AE_NOMORE;
}

void startEvictionTimeProc(void) {
if (!isEvictionProcRunning) {
isEvictionProcRunning = 1;
aeCreateTimeEvent(server.el, 0,
evictionTimeProc, NULL, NULL);
}
}





static int isSafeToPerformEvictions(void) {


if (scriptIsTimedout() || server.loading) return 0;



if (server.masterhost && server.repl_slave_ignore_maxmemory) return 0;




if (checkClientPauseTimeoutAndReturnIfPaused()) return 0;



if (server.also_propagate.numops != 0) return 0;

return 1;
}


static unsigned long evictionTimeLimitUs() {
serverAssert(server.maxmemory_eviction_tenacity >= 0);
serverAssert(server.maxmemory_eviction_tenacity <= 100);

if (server.maxmemory_eviction_tenacity <= 10) {

return 50uL * server.maxmemory_eviction_tenacity;
}

if (server.maxmemory_eviction_tenacity < 100) {

return (unsigned long)(500.0 * pow(1.15, server.maxmemory_eviction_tenacity - 10.0));
}

return ULONG_MAX;
}

























int performEvictions(void) {


if (!isSafeToPerformEvictions()) return EVICT_OK;

int keys_freed = 0;
size_t mem_reported, mem_tofree;
long long mem_freed;
mstime_t latency, eviction_latency;
long long delta;
int slaves = listLength(server.slaves);
int result = EVICT_FAIL;

if (getMaxmemoryState(&mem_reported,NULL,&mem_tofree,NULL) == C_OK) {
result = EVICT_OK;
goto update_metrics;
}

if (server.maxmemory_policy == MAXMEMORY_NO_EVICTION) {
result = EVICT_FAIL;
goto update_metrics;
}

unsigned long eviction_time_limit_us = evictionTimeLimitUs();

mem_freed = 0;

latencyStartMonitor(latency);

monotime evictionTimer;
elapsedStart(&evictionTimer);



int prev_core_propagates = server.core_propagates;
serverAssert(server.also_propagate.numops == 0);
server.core_propagates = 1;
server.propagate_no_multi = 1;

while (mem_freed < (long long)mem_tofree) {
int j, k, i;
static unsigned int next_db = 0;
sds bestkey = NULL;
int bestdbid;
redisDb *db;
dict *dict;
dictEntry *de;

if (server.maxmemory_policy & (MAXMEMORY_FLAG_LRU|MAXMEMORY_FLAG_LFU) ||
server.maxmemory_policy == MAXMEMORY_VOLATILE_TTL)
{
struct evictionPoolEntry *pool = EvictionPoolLRU;

while(bestkey == NULL) {
unsigned long total_keys = 0, keys;




for (i = 0; i < server.dbnum; i++) {
db = server.db+i;
dict = (server.maxmemory_policy & MAXMEMORY_FLAG_ALLKEYS) ?
db->dict : db->expires;
if ((keys = dictSize(dict)) != 0) {
evictionPoolPopulate(i, dict, db->dict, pool);
total_keys += keys;
}
}
if (!total_keys) break;


for (k = EVPOOL_SIZE-1; k >= 0; k--) {
if (pool[k].key == NULL) continue;
bestdbid = pool[k].dbid;

if (server.maxmemory_policy & MAXMEMORY_FLAG_ALLKEYS) {
de = dictFind(server.db[bestdbid].dict,
pool[k].key);
} else {
de = dictFind(server.db[bestdbid].expires,
pool[k].key);
}


if (pool[k].key != pool[k].cached)
sdsfree(pool[k].key);
pool[k].key = NULL;
pool[k].idle = 0;



if (de) {
bestkey = dictGetKey(de);
break;
} else {

}
}
}
}


else if (server.maxmemory_policy == MAXMEMORY_ALLKEYS_RANDOM ||
server.maxmemory_policy == MAXMEMORY_VOLATILE_RANDOM)
{



for (i = 0; i < server.dbnum; i++) {
j = (++next_db) % server.dbnum;
db = server.db+j;
dict = (server.maxmemory_policy == MAXMEMORY_ALLKEYS_RANDOM) ?
db->dict : db->expires;
if (dictSize(dict) != 0) {
de = dictGetRandomKey(dict);
bestkey = dictGetKey(de);
bestdbid = j;
break;
}
}
}


if (bestkey) {
db = server.db+bestdbid;
robj *keyobj = createStringObject(bestkey,sdslen(bestkey));










delta = (long long) zmalloc_used_memory();
latencyStartMonitor(eviction_latency);
if (server.lazyfree_lazy_eviction)
dbAsyncDelete(db,keyobj);
else
dbSyncDelete(db,keyobj);
latencyEndMonitor(eviction_latency);
latencyAddSampleIfNeeded("eviction-del",eviction_latency);
delta -= (long long) zmalloc_used_memory();
mem_freed += delta;
server.stat_evictedkeys++;
signalModifiedKey(NULL,db,keyobj);
notifyKeyspaceEvent(NOTIFY_EVICTED, "evicted",
keyobj, db->id);
propagateDeletion(db,keyobj,server.lazyfree_lazy_eviction);
decrRefCount(keyobj);
keys_freed++;

if (keys_freed % 16 == 0) {




if (slaves) flushSlavesOutputBuffers();








if (server.lazyfree_lazy_eviction) {
if (getMaxmemoryState(NULL,NULL,NULL,NULL) == C_OK) {
break;
}
}




if (elapsedUs(evictionTimer) > eviction_time_limit_us) {

startEvictionTimeProc();
break;
}
}
} else {
goto cant_free;
}
}

result = (isEvictionProcRunning) ? EVICT_RUNNING : EVICT_OK;

cant_free:
if (result == EVICT_FAIL) {



if (bioPendingJobsOfType(BIO_LAZY_FREE)) {
usleep(eviction_time_limit_us);
if (getMaxmemoryState(NULL,NULL,NULL,NULL) == C_OK) {
result = EVICT_OK;
}
}
}

serverAssert(server.core_propagates);


propagatePendingCommands();

server.core_propagates = prev_core_propagates;
server.propagate_no_multi = 0;

latencyEndMonitor(latency);
latencyAddSampleIfNeeded("eviction-cycle",latency);

update_metrics:
if (result == EVICT_RUNNING || result == EVICT_FAIL) {
if (server.stat_last_eviction_exceeded_time == 0)
elapsedStart(&server.stat_last_eviction_exceeded_time);
} else if (result == EVICT_OK) {
if (server.stat_last_eviction_exceeded_time != 0) {
server.stat_total_eviction_exceeded_time += elapsedUs(server.stat_last_eviction_exceeded_time);
server.stat_last_eviction_exceeded_time = 0;
}
}
return result;
}
