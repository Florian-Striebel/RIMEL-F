































#include "server.h"




















int activeExpireCycleTryExpire(redisDb *db, dictEntry *de, long long now) {
long long t = dictGetSignedIntegerVal(de);
if (now > t) {
sds key = dictGetKey(de);
robj *keyobj = createStringObject(key,sdslen(key));
deleteExpiredKeyAndPropagate(db,keyobj);
decrRefCount(keyobj);
return 1;
} else {
return 0;
}
}









































#define ACTIVE_EXPIRE_CYCLE_KEYS_PER_LOOP 20
#define ACTIVE_EXPIRE_CYCLE_FAST_DURATION 1000
#define ACTIVE_EXPIRE_CYCLE_SLOW_TIME_PERC 25
#define ACTIVE_EXPIRE_CYCLE_ACCEPTABLE_STALE 10


void activeExpireCycle(int type) {



unsigned long
effort = server.active_expire_effort-1,
config_keys_per_loop = ACTIVE_EXPIRE_CYCLE_KEYS_PER_LOOP +
ACTIVE_EXPIRE_CYCLE_KEYS_PER_LOOP/4*effort,
config_cycle_fast_duration = ACTIVE_EXPIRE_CYCLE_FAST_DURATION +
ACTIVE_EXPIRE_CYCLE_FAST_DURATION/4*effort,
config_cycle_slow_time_perc = ACTIVE_EXPIRE_CYCLE_SLOW_TIME_PERC +
2*effort,
config_cycle_acceptable_stale = ACTIVE_EXPIRE_CYCLE_ACCEPTABLE_STALE-
effort;



static unsigned int current_db = 0;
static int timelimit_exit = 0;
static long long last_fast_cycle = 0;

int j, iteration = 0;
int dbs_per_call = CRON_DBS_PER_CALL;
long long start = ustime(), timelimit, elapsed;




if (checkClientPauseTimeoutAndReturnIfPaused()) return;

if (type == ACTIVE_EXPIRE_CYCLE_FAST) {




if (!timelimit_exit &&
server.stat_expired_stale_perc < config_cycle_acceptable_stale)
return;

if (start < last_fast_cycle + (long long)config_cycle_fast_duration*2)
return;

last_fast_cycle = start;
}








if (dbs_per_call > server.dbnum || timelimit_exit)
dbs_per_call = server.dbnum;





timelimit = config_cycle_slow_time_perc*1000000/server.hz/100;
timelimit_exit = 0;
if (timelimit <= 0) timelimit = 1;

if (type == ACTIVE_EXPIRE_CYCLE_FAST)
timelimit = config_cycle_fast_duration;




long total_sampled = 0;
long total_expired = 0;



serverAssert(server.also_propagate.numops == 0);
server.core_propagates = 1;
server.propagate_no_multi = 1;

for (j = 0; j < dbs_per_call && timelimit_exit == 0; j++) {

unsigned long expired, sampled;

redisDb *db = server.db+(current_db % server.dbnum);




current_db++;





do {
unsigned long num, slots;
long long now, ttl_sum;
int ttl_samples;
iteration++;


if ((num = dictSize(db->expires)) == 0) {
db->avg_ttl = 0;
break;
}
slots = dictSlots(db->expires);
now = mstime();




if (slots > DICT_HT_INITIAL_SIZE &&
(num*100/slots < 1)) break;



expired = 0;
sampled = 0;
ttl_sum = 0;
ttl_samples = 0;

if (num > config_keys_per_loop)
num = config_keys_per_loop;











long max_buckets = num*20;
long checked_buckets = 0;

while (sampled < num && checked_buckets < max_buckets) {
for (int table = 0; table < 2; table++) {
if (table == 1 && !dictIsRehashing(db->expires)) break;

unsigned long idx = db->expires_cursor;
idx &= DICTHT_SIZE_MASK(db->expires->ht_size_exp[table]);
dictEntry *de = db->expires->ht_table[table][idx];
long long ttl;


checked_buckets++;
while(de) {


dictEntry *e = de;
de = de->next;

ttl = dictGetSignedIntegerVal(e)-now;
if (activeExpireCycleTryExpire(db,e,now)) expired++;
if (ttl > 0) {


ttl_sum += ttl;
ttl_samples++;
}
sampled++;
}
}
db->expires_cursor++;
}
total_expired += expired;
total_sampled += sampled;


if (ttl_samples) {
long long avg_ttl = ttl_sum/ttl_samples;




if (db->avg_ttl == 0) db->avg_ttl = avg_ttl;
db->avg_ttl = (db->avg_ttl/50)*49 + (avg_ttl/50);
}




if ((iteration & 0xf) == 0) {
elapsed = ustime()-start;
if (elapsed > timelimit) {
timelimit_exit = 1;
server.stat_expired_time_cap_reached_count++;
break;
}
}



} while (sampled == 0 ||
(expired*100/sampled) > config_cycle_acceptable_stale);
}

serverAssert(server.core_propagates);


propagatePendingCommands();

server.core_propagates = 0;
server.propagate_no_multi = 0;

elapsed = ustime()-start;
server.stat_expire_cycle_time_used += elapsed;
latencyAddSampleIfNeeded("expire-cycle",elapsed/1000);



double current_perc;
if (total_sampled) {
current_perc = (double)total_expired/total_sampled;
} else
current_perc = 0;
server.stat_expired_stale_perc = (current_perc*0.05)+
(server.stat_expired_stale_perc*0.95);
}




































dict *slaveKeysWithExpire = NULL;



void expireSlaveKeys(void) {
if (slaveKeysWithExpire == NULL ||
dictSize(slaveKeysWithExpire) == 0) return;

int cycles = 0, noexpire = 0;
mstime_t start = mstime();
while(1) {
dictEntry *de = dictGetRandomKey(slaveKeysWithExpire);
sds keyname = dictGetKey(de);
uint64_t dbids = dictGetUnsignedIntegerVal(de);
uint64_t new_dbids = 0;



int dbid = 0;
while(dbids && dbid < server.dbnum) {
if ((dbids & 1) != 0) {
redisDb *db = server.db+dbid;
dictEntry *expire = dictFind(db->expires,keyname);
int expired = 0;

if (expire &&
activeExpireCycleTryExpire(server.db+dbid,expire,start))
{
expired = 1;
}





if (expire && !expired) {
noexpire++;
new_dbids |= (uint64_t)1 << dbid;
}
}
dbid++;
dbids >>= 1;
}




if (new_dbids)
dictSetUnsignedIntegerVal(de,new_dbids);
else
dictDelete(slaveKeysWithExpire,keyname);



cycles++;
if (noexpire > 3) break;
if ((cycles % 64) == 0 && mstime()-start > 1) break;
if (dictSize(slaveKeysWithExpire) == 0) break;
}
}



void rememberSlaveKeyWithExpire(redisDb *db, robj *key) {
if (slaveKeysWithExpire == NULL) {
static dictType dt = {
dictSdsHash,
NULL,
NULL,
dictSdsKeyCompare,
dictSdsDestructor,
NULL,
NULL
};
slaveKeysWithExpire = dictCreate(&dt);
}
if (db->id > 63) return;

dictEntry *de = dictAddOrFind(slaveKeysWithExpire,key->ptr);




if (de->key == key->ptr) {
de->key = sdsdup(key->ptr);
dictSetUnsignedIntegerVal(de,0);
}

uint64_t dbids = dictGetUnsignedIntegerVal(de);
dbids |= (uint64_t)1 << db->id;
dictSetUnsignedIntegerVal(de,dbids);
}


size_t getSlaveKeyWithExpireCount(void) {
if (slaveKeysWithExpire == NULL) return 0;
return dictSize(slaveKeysWithExpire);
}









void flushSlaveKeysWithExpireList(void) {
if (slaveKeysWithExpire) {
dictRelease(slaveKeysWithExpire);
slaveKeysWithExpire = NULL;
}
}

int checkAlreadyExpired(long long when) {






return (when <= mstime() && !server.loading && !server.masterhost);
}

#define EXPIRE_NX (1<<0)
#define EXPIRE_XX (1<<1)
#define EXPIRE_GT (1<<2)
#define EXPIRE_LT (1<<3)








int parseExtendedExpireArgumentsOrReply(client *c, int *flags) {
int nx = 0, xx = 0, gt = 0, lt = 0;

int j = 3;
while (j < c->argc) {
char *opt = c->argv[j]->ptr;
if (!strcasecmp(opt,"nx")) {
*flags |= EXPIRE_NX;
nx = 1;
} else if (!strcasecmp(opt,"xx")) {
*flags |= EXPIRE_XX;
xx = 1;
} else if (!strcasecmp(opt,"gt")) {
*flags |= EXPIRE_GT;
gt = 1;
} else if (!strcasecmp(opt,"lt")) {
*flags |= EXPIRE_LT;
lt = 1;
} else {
addReplyErrorFormat(c, "Unsupported option %s", opt);
return C_ERR;
}
j++;
}

if ((nx && xx) || (nx && gt) || (nx && lt)) {
addReplyError(c, "NX and XX, GT or LT options at the same time are not compatible");
return C_ERR;
}

if (gt && lt) {
addReplyError(c, "GT and LT options at the same time are not compatible");
return C_ERR;
}

return C_OK;
}














void expireGenericCommand(client *c, long long basetime, int unit) {
robj *key = c->argv[1], *param = c->argv[2];
long long when;
long long current_expire = -1;
int flag = 0;


if (parseExtendedExpireArgumentsOrReply(c, &flag) != C_OK) {
return;
}

if (getLongLongFromObjectOrReply(c, param, &when, NULL) != C_OK)
return;



if (unit == UNIT_SECONDS) {
if (when > LLONG_MAX / 1000 || when < LLONG_MIN / 1000) {
addReplyErrorExpireTime(c);
return;
}
when *= 1000;
}

if (when > LLONG_MAX - basetime) {
addReplyErrorExpireTime(c);
return;
}
when += basetime;


if (lookupKeyWrite(c->db,key) == NULL) {
addReply(c,shared.czero);
return;
}

if (flag) {
current_expire = getExpire(c->db, key);


if (flag & EXPIRE_NX) {
if (current_expire != -1) {
addReply(c,shared.czero);
return;
}
}


if (flag & EXPIRE_XX) {
if (current_expire == -1) {

addReply(c,shared.czero);
return;
}
}


if (flag & EXPIRE_GT) {


if (when <= current_expire || current_expire == -1) {

addReply(c,shared.czero);
return;
}
}


if (flag & EXPIRE_LT) {



if (current_expire != -1 && when >= current_expire) {

addReply(c,shared.czero);
return;
}
}
}

if (checkAlreadyExpired(when)) {
robj *aux;

int deleted = server.lazyfree_lazy_expire ? dbAsyncDelete(c->db,key) :
dbSyncDelete(c->db,key);
serverAssertWithInfo(c,key,deleted);
server.dirty++;


aux = server.lazyfree_lazy_expire ? shared.unlink : shared.del;
rewriteClientCommandVector(c,2,aux,key);
signalModifiedKey(c,c->db,key);
notifyKeyspaceEvent(NOTIFY_GENERIC,"del",key,c->db->id);
addReply(c, shared.cone);
return;
} else {
setExpire(c,c->db,key,when);
addReply(c,shared.cone);

robj *when_obj = createStringObjectFromLongLong(when);
rewriteClientCommandVector(c, 3, shared.pexpireat, key, when_obj);
decrRefCount(when_obj);
signalModifiedKey(c,c->db,key);
notifyKeyspaceEvent(NOTIFY_GENERIC,"expire",key,c->db->id);
server.dirty++;
return;
}
}


void expireCommand(client *c) {
expireGenericCommand(c,mstime(),UNIT_SECONDS);
}


void expireatCommand(client *c) {
expireGenericCommand(c,0,UNIT_SECONDS);
}


void pexpireCommand(client *c) {
expireGenericCommand(c,mstime(),UNIT_MILLISECONDS);
}


void pexpireatCommand(client *c) {
expireGenericCommand(c,0,UNIT_MILLISECONDS);
}


void ttlGenericCommand(client *c, int output_ms, int output_abs) {
long long expire, ttl = -1;


if (lookupKeyReadWithFlags(c->db,c->argv[1],LOOKUP_NOTOUCH) == NULL) {
addReplyLongLong(c,-2);
return;
}



expire = getExpire(c->db,c->argv[1]);
if (expire != -1) {
ttl = output_abs ? expire : expire-mstime();
if (ttl < 0) ttl = 0;
}
if (ttl == -1) {
addReplyLongLong(c,-1);
} else {
addReplyLongLong(c,output_ms ? ttl : ((ttl+500)/1000));
}
}


void ttlCommand(client *c) {
ttlGenericCommand(c, 0, 0);
}


void pttlCommand(client *c) {
ttlGenericCommand(c, 1, 0);
}


void expiretimeCommand(client *c) {
ttlGenericCommand(c, 0, 1);
}


void pexpiretimeCommand(client *c) {
ttlGenericCommand(c, 1, 1);
}


void persistCommand(client *c) {
if (lookupKeyWrite(c->db,c->argv[1])) {
if (removeExpire(c->db,c->argv[1])) {
signalModifiedKey(c,c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_GENERIC,"persist",c->argv[1],c->db->id);
addReply(c,shared.cone);
server.dirty++;
} else {
addReply(c,shared.czero);
}
} else {
addReply(c,shared.czero);
}
}


void touchCommand(client *c) {
int touched = 0;
for (int j = 1; j < c->argc; j++)
if (lookupKeyRead(c->db,c->argv[j]) != NULL) touched++;
addReplyLongLong(c,touched);
}
