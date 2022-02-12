




























#if !defined(__REDIS_H)
#define __REDIS_H

#include "fmacros.h"
#include "config.h"
#include "solarisfixes.h"
#include "rio.h"
#include "atomicvar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <syslog.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <lua.h>
#include <signal.h>
#include "hdr_histogram.h"

#if defined(HAVE_LIBSYSTEMD)
#include <systemd/sd-daemon.h>
#endif

typedef long long mstime_t;
typedef long long ustime_t;

#include "ae.h"
#include "sds.h"
#include "dict.h"
#include "adlist.h"
#include "zmalloc.h"
#include "anet.h"
#include "ziplist.h"
#include "intset.h"
#include "version.h"
#include "util.h"
#include "latency.h"
#include "sparkline.h"
#include "quicklist.h"

#include "rax.h"
#include "connection.h"

#define REDISMODULE_CORE 1
#include "redismodule.h"


#include "zipmap.h"
#include "sha1.h"
#include "endianconv.h"
#include "crc64.h"


#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


#define C_OK 0
#define C_ERR -1


#define CONFIG_DEFAULT_HZ 10
#define CONFIG_MIN_HZ 1
#define CONFIG_MAX_HZ 500
#define MAX_CLIENTS_PER_CLOCK_TICK 200
#define CONFIG_MAX_LINE 1024
#define CRON_DBS_PER_CALL 16
#define NET_MAX_WRITES_PER_EVENT (1024*64)
#define PROTO_SHARED_SELECT_CMDS 10
#define OBJ_SHARED_INTEGERS 10000
#define OBJ_SHARED_BULKHDR_LEN 32
#define LOG_MAX_LEN 1024
#define AOF_REWRITE_ITEMS_PER_CMD 64
#define AOF_ANNOTATION_LINE_MAX_LEN 1024
#define CONFIG_AUTHPASS_MAX_LEN 512
#define CONFIG_RUN_ID_SIZE 40
#define RDB_EOF_MARK_SIZE 40
#define CONFIG_REPL_BACKLOG_MIN_SIZE (1024*16)
#define CONFIG_BGSAVE_RETRY_DELAY 5
#define CONFIG_DEFAULT_PID_FILE "/var/run/redis.pid"
#define CONFIG_DEFAULT_BINDADDR_COUNT 2
#define CONFIG_DEFAULT_BINDADDR { "*", "-::*" }
#define NET_HOST_STR_LEN 256
#define NET_IP_STR_LEN 46
#define NET_ADDR_STR_LEN (NET_IP_STR_LEN+32)
#define NET_HOST_PORT_STR_LEN (NET_HOST_STR_LEN+32)
#define CONFIG_BINDADDR_MAX 16
#define CONFIG_MIN_RESERVED_FDS 32
#define CONFIG_DEFAULT_PROC_TITLE_TEMPLATE "{title} {listen-addr} {server-mode}"



#define CLIENT_MEM_USAGE_BUCKET_MIN_LOG 15
#define CLIENT_MEM_USAGE_BUCKET_MAX_LOG 33
#define CLIENT_MEM_USAGE_BUCKETS (1+CLIENT_MEM_USAGE_BUCKET_MAX_LOG-CLIENT_MEM_USAGE_BUCKET_MIN_LOG)

#define ACTIVE_EXPIRE_CYCLE_SLOW 0
#define ACTIVE_EXPIRE_CYCLE_FAST 1







#define SERVER_CHILD_NOERROR_RETVAL 255




#define CHILD_COW_DUTY_CYCLE 100


#define STATS_METRIC_SAMPLES 16
#define STATS_METRIC_COMMAND 0
#define STATS_METRIC_NET_INPUT 1
#define STATS_METRIC_NET_OUTPUT 2
#define STATS_METRIC_COUNT 3


#define PROTO_IOBUF_LEN (1024*16)
#define PROTO_REPLY_CHUNK_BYTES (16*1024)
#define PROTO_INLINE_MAX_SIZE (1024*64)
#define PROTO_MBULK_BIG_ARG (1024*32)
#define PROTO_RESIZE_THRESHOLD (1024*32)
#define LONG_STR_SIZE 21
#define REDIS_AUTOSYNC_BYTES (1024*1024*4)

#define LIMIT_PENDING_QUERYBUF (4*1024*1024)





#define CONFIG_FDSET_INCR (CONFIG_MIN_RESERVED_FDS+96)


#define CONFIG_OOM_MASTER 0
#define CONFIG_OOM_REPLICA 1
#define CONFIG_OOM_BGCHILD 2
#define CONFIG_OOM_COUNT 3

extern int configOOMScoreAdjValuesDefaults[CONFIG_OOM_COUNT];


#define HASHTABLE_MIN_FILL 10
#define HASHTABLE_MAX_LOAD_FACTOR 1.618



#define CMD_WRITE (1ULL<<0)
#define CMD_READONLY (1ULL<<1)
#define CMD_DENYOOM (1ULL<<2)
#define CMD_MODULE (1ULL<<3)
#define CMD_ADMIN (1ULL<<4)
#define CMD_PUBSUB (1ULL<<5)
#define CMD_NOSCRIPT (1ULL<<6)
#define CMD_BLOCKING (1ULL<<8)
#define CMD_LOADING (1ULL<<9)
#define CMD_STALE (1ULL<<10)
#define CMD_SKIP_MONITOR (1ULL<<11)
#define CMD_SKIP_SLOWLOG (1ULL<<12)
#define CMD_ASKING (1ULL<<13)
#define CMD_FAST (1ULL<<14)
#define CMD_NO_AUTH (1ULL<<15)
#define CMD_MAY_REPLICATE (1ULL<<16)
#define CMD_SENTINEL (1ULL<<17)
#define CMD_ONLY_SENTINEL (1ULL<<18)
#define CMD_NO_MANDATORY_KEYS (1ULL<<19)
#define CMD_PROTECTED (1ULL<<20)
#define CMD_MODULE_GETKEYS (1ULL<<21)
#define CMD_MODULE_NO_CLUSTER (1ULL<<22)
#define CMD_NO_ASYNC_LOADING (1ULL<<23)
#define CMD_NO_MULTI (1ULL<<24)
#define CMD_MOVABLE_KEYS (1ULL<<25)
#define CMD_ALLOW_BUSY ((1ULL<<26))


#define ACL_CATEGORY_KEYSPACE (1ULL<<0)
#define ACL_CATEGORY_READ (1ULL<<1)
#define ACL_CATEGORY_WRITE (1ULL<<2)
#define ACL_CATEGORY_SET (1ULL<<3)
#define ACL_CATEGORY_SORTEDSET (1ULL<<4)
#define ACL_CATEGORY_LIST (1ULL<<5)
#define ACL_CATEGORY_HASH (1ULL<<6)
#define ACL_CATEGORY_STRING (1ULL<<7)
#define ACL_CATEGORY_BITMAP (1ULL<<8)
#define ACL_CATEGORY_HYPERLOGLOG (1ULL<<9)
#define ACL_CATEGORY_GEO (1ULL<<10)
#define ACL_CATEGORY_STREAM (1ULL<<11)
#define ACL_CATEGORY_PUBSUB (1ULL<<12)
#define ACL_CATEGORY_ADMIN (1ULL<<13)
#define ACL_CATEGORY_FAST (1ULL<<14)
#define ACL_CATEGORY_SLOW (1ULL<<15)
#define ACL_CATEGORY_BLOCKING (1ULL<<16)
#define ACL_CATEGORY_DANGEROUS (1ULL<<17)
#define ACL_CATEGORY_CONNECTION (1ULL<<18)
#define ACL_CATEGORY_TRANSACTION (1ULL<<19)
#define ACL_CATEGORY_SCRIPTING (1ULL<<20)







#define CMD_KEY_RO (1ULL<<0)

#define CMD_KEY_RW (1ULL<<1)

#define CMD_KEY_OW (1ULL<<2)

#define CMD_KEY_RM (1ULL<<3)






#define CMD_KEY_ACCESS (1ULL<<4)

#define CMD_KEY_UPDATE (1ULL<<5)

#define CMD_KEY_INSERT (1ULL<<6)

#define CMD_KEY_DELETE (1ULL<<7)


#define CMD_KEY_CHANNEL (1ULL<<8)
#define CMD_KEY_INCOMPLETE (1ULL<<9)



#define AOF_OFF 0
#define AOF_ON 1
#define AOF_WAIT_REWRITE 2


#define AOF_OK 0
#define AOF_NOT_EXIST 1
#define AOF_EMPTY 2
#define AOF_OPEN_ERR 3
#define AOF_FAILED 4
#define AOF_TRUNCATED 5


#define CMD_DOC_NONE 0
#define CMD_DOC_DEPRECATED (1<<0)
#define CMD_DOC_SYSCMD (1<<1)


#define CLIENT_SLAVE (1<<0)
#define CLIENT_MASTER (1<<1)
#define CLIENT_MONITOR (1<<2)
#define CLIENT_MULTI (1<<3)
#define CLIENT_BLOCKED (1<<4)
#define CLIENT_DIRTY_CAS (1<<5)
#define CLIENT_CLOSE_AFTER_REPLY (1<<6)
#define CLIENT_UNBLOCKED (1<<7)

#define CLIENT_SCRIPT (1<<8)
#define CLIENT_ASKING (1<<9)
#define CLIENT_CLOSE_ASAP (1<<10)
#define CLIENT_UNIX_SOCKET (1<<11)
#define CLIENT_DIRTY_EXEC (1<<12)
#define CLIENT_MASTER_FORCE_REPLY (1<<13)
#define CLIENT_FORCE_AOF (1<<14)
#define CLIENT_FORCE_REPL (1<<15)
#define CLIENT_PRE_PSYNC (1<<16)
#define CLIENT_READONLY (1<<17)
#define CLIENT_PUBSUB (1<<18)
#define CLIENT_PREVENT_AOF_PROP (1<<19)
#define CLIENT_PREVENT_REPL_PROP (1<<20)
#define CLIENT_PREVENT_PROP (CLIENT_PREVENT_AOF_PROP|CLIENT_PREVENT_REPL_PROP)
#define CLIENT_PENDING_WRITE (1<<21)

#define CLIENT_REPLY_OFF (1<<22)
#define CLIENT_REPLY_SKIP_NEXT (1<<23)
#define CLIENT_REPLY_SKIP (1<<24)
#define CLIENT_LUA_DEBUG (1<<25)
#define CLIENT_LUA_DEBUG_SYNC (1<<26)
#define CLIENT_MODULE (1<<27)
#define CLIENT_PROTECTED (1<<28)

#define CLIENT_PENDING_COMMAND (1<<30)

#define CLIENT_TRACKING (1ULL<<31)

#define CLIENT_TRACKING_BROKEN_REDIR (1ULL<<32)
#define CLIENT_TRACKING_BCAST (1ULL<<33)
#define CLIENT_TRACKING_OPTIN (1ULL<<34)
#define CLIENT_TRACKING_OPTOUT (1ULL<<35)
#define CLIENT_TRACKING_CACHING (1ULL<<36)

#define CLIENT_TRACKING_NOLOOP (1ULL<<37)

#define CLIENT_IN_TO_TABLE (1ULL<<38)
#define CLIENT_PROTOCOL_ERROR (1ULL<<39)
#define CLIENT_CLOSE_AFTER_COMMAND (1ULL<<40)

#define CLIENT_DENY_BLOCKING (1ULL<<41)


#define CLIENT_REPL_RDBONLY (1ULL<<42)

#define CLIENT_NO_EVICT (1ULL<<43)




#define BLOCKED_NONE 0
#define BLOCKED_LIST 1
#define BLOCKED_WAIT 2
#define BLOCKED_MODULE 3
#define BLOCKED_STREAM 4
#define BLOCKED_ZSET 5
#define BLOCKED_POSTPONE 6
#define BLOCKED_SHUTDOWN 7
#define BLOCKED_NUM 8


#define PROTO_REQ_INLINE 1
#define PROTO_REQ_MULTIBULK 2



#define CLIENT_TYPE_NORMAL 0
#define CLIENT_TYPE_SLAVE 1
#define CLIENT_TYPE_PUBSUB 2
#define CLIENT_TYPE_MASTER 3
#define CLIENT_TYPE_COUNT 4
#define CLIENT_TYPE_OBUF_COUNT 3





typedef enum {
REPL_STATE_NONE = 0,
REPL_STATE_CONNECT,
REPL_STATE_CONNECTING,

REPL_STATE_RECEIVE_PING_REPLY,
REPL_STATE_SEND_HANDSHAKE,
REPL_STATE_RECEIVE_AUTH_REPLY,
REPL_STATE_RECEIVE_PORT_REPLY,
REPL_STATE_RECEIVE_IP_REPLY,
REPL_STATE_RECEIVE_CAPA_REPLY,
REPL_STATE_SEND_PSYNC,
REPL_STATE_RECEIVE_PSYNC_REPLY,

REPL_STATE_TRANSFER,
REPL_STATE_CONNECTED,
} repl_state;


typedef enum {
NO_FAILOVER = 0,
FAILOVER_WAIT_FOR_SYNC,
FAILOVER_IN_PROGRESS

} failover_state;





#define SLAVE_STATE_WAIT_BGSAVE_START 6
#define SLAVE_STATE_WAIT_BGSAVE_END 7
#define SLAVE_STATE_SEND_BULK 8
#define SLAVE_STATE_ONLINE 9


#define SLAVE_CAPA_NONE 0
#define SLAVE_CAPA_EOF (1<<0)
#define SLAVE_CAPA_PSYNC2 (1<<1)


#define SLAVE_REQ_NONE 0
#define SLAVE_REQ_RDB_EXCLUDE_DATA (1 << 0)
#define SLAVE_REQ_RDB_EXCLUDE_FUNCTIONS (1 << 1)

#define SLAVE_REQ_RDB_MASK (SLAVE_REQ_RDB_EXCLUDE_DATA | SLAVE_REQ_RDB_EXCLUDE_FUNCTIONS)


#define CONFIG_REPL_SYNCIO_TIMEOUT 5


#define REPL_BACKLOG_TRIM_BLOCKS_PER_CALL 64



#define REPL_BACKLOG_INDEX_PER_BLOCKS 64


#define LIST_HEAD 0
#define LIST_TAIL 1
#define ZSET_MIN 0
#define ZSET_MAX 1


#define SORT_OP_GET 0


#define LL_DEBUG 0
#define LL_VERBOSE 1
#define LL_NOTICE 2
#define LL_WARNING 3
#define LL_RAW (1<<10)


#define SUPERVISED_NONE 0
#define SUPERVISED_AUTODETECT 1
#define SUPERVISED_SYSTEMD 2
#define SUPERVISED_UPSTART 3


#define UNUSED(V) ((void) V)

#define ZSKIPLIST_MAXLEVEL 32
#define ZSKIPLIST_P 0.25


#define AOF_FSYNC_NO 0
#define AOF_FSYNC_ALWAYS 1
#define AOF_FSYNC_EVERYSEC 2


#define REPL_DISKLESS_LOAD_DISABLED 0
#define REPL_DISKLESS_LOAD_WHEN_DB_EMPTY 1
#define REPL_DISKLESS_LOAD_SWAPDB 2


#define TLS_CLIENT_AUTH_NO 0
#define TLS_CLIENT_AUTH_YES 1
#define TLS_CLIENT_AUTH_OPTIONAL 2


#define SANITIZE_DUMP_NO 0
#define SANITIZE_DUMP_YES 1
#define SANITIZE_DUMP_CLIENTS 2


#define PROTECTED_ACTION_ALLOWED_NO 0
#define PROTECTED_ACTION_ALLOWED_YES 1
#define PROTECTED_ACTION_ALLOWED_LOCAL 2


#define SET_OP_UNION 0
#define SET_OP_DIFF 1
#define SET_OP_INTER 2


#define OOM_SCORE_ADJ_NO 0
#define OOM_SCORE_RELATIVE 1
#define OOM_SCORE_ADJ_ABSOLUTE 2




#define MAXMEMORY_FLAG_LRU (1<<0)
#define MAXMEMORY_FLAG_LFU (1<<1)
#define MAXMEMORY_FLAG_ALLKEYS (1<<2)
#define MAXMEMORY_FLAG_NO_SHARED_INTEGERS (MAXMEMORY_FLAG_LRU|MAXMEMORY_FLAG_LFU)


#define MAXMEMORY_VOLATILE_LRU ((0<<8)|MAXMEMORY_FLAG_LRU)
#define MAXMEMORY_VOLATILE_LFU ((1<<8)|MAXMEMORY_FLAG_LFU)
#define MAXMEMORY_VOLATILE_TTL (2<<8)
#define MAXMEMORY_VOLATILE_RANDOM (3<<8)
#define MAXMEMORY_ALLKEYS_LRU ((4<<8)|MAXMEMORY_FLAG_LRU|MAXMEMORY_FLAG_ALLKEYS)
#define MAXMEMORY_ALLKEYS_LFU ((5<<8)|MAXMEMORY_FLAG_LFU|MAXMEMORY_FLAG_ALLKEYS)
#define MAXMEMORY_ALLKEYS_RANDOM ((6<<8)|MAXMEMORY_FLAG_ALLKEYS)
#define MAXMEMORY_NO_EVICTION (7<<8)


#define UNIT_SECONDS 0
#define UNIT_MILLISECONDS 1


#define SHUTDOWN_NOFLAGS 0
#define SHUTDOWN_SAVE 1

#define SHUTDOWN_NOSAVE 2
#define SHUTDOWN_NOW 4
#define SHUTDOWN_FORCE 8


#define CMD_CALL_NONE 0
#define CMD_CALL_SLOWLOG (1<<0)
#define CMD_CALL_STATS (1<<1)
#define CMD_CALL_PROPAGATE_AOF (1<<2)
#define CMD_CALL_PROPAGATE_REPL (1<<3)
#define CMD_CALL_PROPAGATE (CMD_CALL_PROPAGATE_AOF|CMD_CALL_PROPAGATE_REPL)
#define CMD_CALL_FULL (CMD_CALL_SLOWLOG | CMD_CALL_STATS | CMD_CALL_PROPAGATE)
#define CMD_CALL_FROM_MODULE (1<<4)


#define PROPAGATE_NONE 0
#define PROPAGATE_AOF 1
#define PROPAGATE_REPL 2



typedef enum {
CLIENT_PAUSE_OFF = 0,
CLIENT_PAUSE_WRITE,
CLIENT_PAUSE_ALL
} pause_type;


typedef enum {
PAUSE_BY_CLIENT_COMMAND = 0,
PAUSE_DURING_SHUTDOWN,
PAUSE_DURING_FAILOVER,
NUM_PAUSE_PURPOSES
} pause_purpose;

typedef struct {
pause_type type;
mstime_t end;
} pause_event;


typedef enum {
CLUSTER_ENDPOINT_TYPE_IP = 0,
CLUSTER_ENDPOINT_TYPE_HOSTNAME,
CLUSTER_ENDPOINT_TYPE_UNKNOWN_ENDPOINT
} cluster_endpoint_type;


#define RDB_CHILD_TYPE_NONE 0
#define RDB_CHILD_TYPE_DISK 1
#define RDB_CHILD_TYPE_SOCKET 2



#define NOTIFY_KEYSPACE (1<<0)
#define NOTIFY_KEYEVENT (1<<1)
#define NOTIFY_GENERIC (1<<2)
#define NOTIFY_STRING (1<<3)
#define NOTIFY_LIST (1<<4)
#define NOTIFY_SET (1<<5)
#define NOTIFY_HASH (1<<6)
#define NOTIFY_ZSET (1<<7)
#define NOTIFY_EXPIRED (1<<8)
#define NOTIFY_EVICTED (1<<9)
#define NOTIFY_STREAM (1<<10)
#define NOTIFY_KEY_MISS (1<<11)
#define NOTIFY_LOADED (1<<12)
#define NOTIFY_MODULE (1<<13)
#define NOTIFY_ALL (NOTIFY_GENERIC | NOTIFY_STRING | NOTIFY_LIST | NOTIFY_SET | NOTIFY_HASH | NOTIFY_ZSET | NOTIFY_EXPIRED | NOTIFY_EVICTED | NOTIFY_STREAM | NOTIFY_MODULE)




#define run_with_period(_ms_) if ((_ms_ <= 1000/server.hz) || !(server.cronloops%((_ms_)/(1000/server.hz))))


#define serverAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (_serverAssertWithInfo(_c,_o,#_e,__FILE__,__LINE__),redis_unreachable()))
#define serverAssert(_e) ((_e)?(void)0 : (_serverAssert(#_e,__FILE__,__LINE__),redis_unreachable()))
#define serverPanic(...) _serverPanic(__FILE__,__LINE__,__VA_ARGS__),redis_unreachable()


#define LATENCY_HISTOGRAM_MIN_VALUE 1L
#define LATENCY_HISTOGRAM_MAX_VALUE 1000000000L
#define LATENCY_HISTOGRAM_PRECISION 2




#define BUSY_MODULE_YIELD_NONE (0)
#define BUSY_MODULE_YIELD_EVENTS (1<<0)
#define BUSY_MODULE_YIELD_CLIENTS (1<<1)








#define OBJ_STRING 0
#define OBJ_LIST 1
#define OBJ_SET 2
#define OBJ_ZSET 3
#define OBJ_HASH 4












#define OBJ_MODULE 5
#define OBJ_STREAM 6


#define REDISMODULE_TYPE_ENCVER_BITS 10
#define REDISMODULE_TYPE_ENCVER_MASK ((1<<REDISMODULE_TYPE_ENCVER_BITS)-1)
#define REDISMODULE_TYPE_ENCVER(id) (id & REDISMODULE_TYPE_ENCVER_MASK)
#define REDISMODULE_TYPE_SIGN(id) ((id & ~((uint64_t)REDISMODULE_TYPE_ENCVER_MASK)) >>REDISMODULE_TYPE_ENCVER_BITS)


#define REDISMODULE_AUX_BEFORE_RDB (1<<0)
#define REDISMODULE_AUX_AFTER_RDB (1<<1)

struct RedisModule;
struct RedisModuleIO;
struct RedisModuleDigest;
struct RedisModuleCtx;
struct moduleLoadQueueEntry;
struct redisObject;
struct RedisModuleDefragCtx;
struct RedisModuleInfoCtx;
struct RedisModuleKeyOptCtx;





typedef void *(*moduleTypeLoadFunc)(struct RedisModuleIO *io, int encver);
typedef void (*moduleTypeSaveFunc)(struct RedisModuleIO *io, void *value);
typedef int (*moduleTypeAuxLoadFunc)(struct RedisModuleIO *rdb, int encver, int when);
typedef void (*moduleTypeAuxSaveFunc)(struct RedisModuleIO *rdb, int when);
typedef void (*moduleTypeRewriteFunc)(struct RedisModuleIO *io, struct redisObject *key, void *value);
typedef void (*moduleTypeDigestFunc)(struct RedisModuleDigest *digest, void *value);
typedef size_t (*moduleTypeMemUsageFunc)(const void *value);
typedef void (*moduleTypeFreeFunc)(void *value);
typedef size_t (*moduleTypeFreeEffortFunc)(struct redisObject *key, const void *value);
typedef void (*moduleTypeUnlinkFunc)(struct redisObject *key, void *value);
typedef void *(*moduleTypeCopyFunc)(struct redisObject *fromkey, struct redisObject *tokey, const void *value);
typedef int (*moduleTypeDefragFunc)(struct RedisModuleDefragCtx *ctx, struct redisObject *key, void **value);
typedef void (*RedisModuleInfoFunc)(struct RedisModuleInfoCtx *ctx, int for_crash_report);
typedef void (*RedisModuleDefragFunc)(struct RedisModuleDefragCtx *ctx);
typedef size_t (*moduleTypeMemUsageFunc2)(struct RedisModuleKeyOptCtx *ctx, const void *value, size_t sample_size);
typedef void (*moduleTypeFreeFunc2)(struct RedisModuleKeyOptCtx *ctx, void *value);
typedef size_t (*moduleTypeFreeEffortFunc2)(struct RedisModuleKeyOptCtx *ctx, const void *value);
typedef void (*moduleTypeUnlinkFunc2)(struct RedisModuleKeyOptCtx *ctx, void *value);
typedef void *(*moduleTypeCopyFunc2)(struct RedisModuleKeyOptCtx *ctx, const void *value);





typedef void (*RedisModuleUserChangedFunc) (uint64_t client_id, void *privdata);




typedef struct RedisModuleType {
uint64_t id;
struct RedisModule *module;
moduleTypeLoadFunc rdb_load;
moduleTypeSaveFunc rdb_save;
moduleTypeRewriteFunc aof_rewrite;
moduleTypeMemUsageFunc mem_usage;
moduleTypeDigestFunc digest;
moduleTypeFreeFunc free;
moduleTypeFreeEffortFunc free_effort;
moduleTypeUnlinkFunc unlink;
moduleTypeCopyFunc copy;
moduleTypeDefragFunc defrag;
moduleTypeAuxLoadFunc aux_load;
moduleTypeAuxSaveFunc aux_save;
moduleTypeMemUsageFunc2 mem_usage2;
moduleTypeFreeEffortFunc2 free_effort2;
moduleTypeUnlinkFunc2 unlink2;
moduleTypeCopyFunc2 copy2;
int aux_save_triggers;
char name[10];
} moduleType;
















typedef struct moduleValue {
moduleType *type;
void *value;
} moduleValue;


struct RedisModule {
void *handle;
char *name;
int ver;
int apiver;
list *types;
list *usedby;
list *using;
list *filters;
int in_call;
int in_hook;
int options;
int blocked_clients;
RedisModuleInfoFunc info_cb;
RedisModuleDefragFunc defrag_cb;
struct moduleLoadQueueEntry *loadmod;
};
typedef struct RedisModule RedisModule;




typedef struct RedisModuleIO {
size_t bytes;
rio *rio;
moduleType *type;
int error;
int ver;

struct RedisModuleCtx *ctx;
struct redisObject *key;
int dbid;
} RedisModuleIO;



#define moduleInitIOContext(iovar,mtype,rioptr,keyptr,db) do { iovar.rio = rioptr; iovar.type = mtype; iovar.bytes = 0; iovar.error = 0; iovar.ver = 0; iovar.key = keyptr; iovar.dbid = db; iovar.ctx = NULL; } while(0)















typedef struct RedisModuleDigest {
unsigned char o[20];
unsigned char x[20];
struct redisObject *key;
int dbid;
} RedisModuleDigest;


#define moduleInitDigestContext(mdvar) do { memset(mdvar.o,0,sizeof(mdvar.o)); memset(mdvar.x,0,sizeof(mdvar.x)); } while(0)







#define OBJ_ENCODING_RAW 0
#define OBJ_ENCODING_INT 1
#define OBJ_ENCODING_HT 2
#define OBJ_ENCODING_ZIPMAP 3
#define OBJ_ENCODING_LINKEDLIST 4
#define OBJ_ENCODING_ZIPLIST 5
#define OBJ_ENCODING_INTSET 6
#define OBJ_ENCODING_SKIPLIST 7
#define OBJ_ENCODING_EMBSTR 8
#define OBJ_ENCODING_QUICKLIST 9
#define OBJ_ENCODING_STREAM 10
#define OBJ_ENCODING_LISTPACK 11

#define LRU_BITS 24
#define LRU_CLOCK_MAX ((1<<LRU_BITS)-1)
#define LRU_CLOCK_RESOLUTION 1000

#define OBJ_SHARED_REFCOUNT INT_MAX
#define OBJ_STATIC_REFCOUNT (INT_MAX-1)
#define OBJ_FIRST_SPECIAL_REFCOUNT OBJ_STATIC_REFCOUNT
typedef struct redisObject {
unsigned type:4;
unsigned encoding:4;
unsigned lru:LRU_BITS;


int refcount;
void *ptr;
} robj;




char *getObjectTypeName(robj*);





#define initStaticStringObject(_var,_ptr) do { _var.refcount = OBJ_STATIC_REFCOUNT; _var.type = OBJ_STRING; _var.encoding = OBJ_ENCODING_RAW; _var.ptr = _ptr; } while(0)






struct evictionPoolEntry;



typedef struct clientReplyBlock {
size_t size, used;
char buf[];
} clientReplyBlock;




















typedef struct replBufBlock {
int refcount;
long long id;
long long repl_offset;
size_t size, used;
char buf[];
} replBufBlock;


typedef struct clusterSlotToKeyMapping clusterSlotToKeyMapping;




typedef struct redisDb {
dict *dict;
dict *expires;
dict *blocking_keys;
dict *ready_keys;
dict *watched_keys;
int id;
long long avg_ttl;
unsigned long expires_cursor;
list *defrag_later;
clusterSlotToKeyMapping *slots_to_keys;
} redisDb;


typedef struct functionsLibCtx functionsLibCtx;






typedef struct rdbLoadingCtx {
redisDb* dbarray;
functionsLibCtx* functions_lib_ctx;
}rdbLoadingCtx;


typedef struct multiCmd {
robj **argv;
int argv_len;
int argc;
struct redisCommand *cmd;
} multiCmd;

typedef struct multiState {
multiCmd *commands;
int count;
int cmd_flags;


int cmd_inv_flags;


size_t argv_len_sums;
} multiState;



typedef struct blockingState {

long count;
mstime_t timeout;



dict *keys;

robj *target;

struct blockPos {
int wherefrom;
int whereto;
} blockpos;




size_t xread_count;
robj *xread_group;
robj *xread_consumer;
int xread_group_noack;


int numreplicas;
long long reploffset;


void *module_blocked_handle;


} blockingState;












typedef struct readyList {
redisDb *db;
robj *key;
} readyList;




#define USER_COMMAND_BITS_COUNT 1024



#define USER_FLAG_ENABLED (1<<0)
#define USER_FLAG_DISABLED (1<<1)
#define USER_FLAG_NOPASS (1<<2)





#define USER_FLAG_SANITIZE_PAYLOAD (1<<3)

#define USER_FLAG_SANITIZE_PAYLOAD_SKIP (1<<4)



#define SELECTOR_FLAG_ROOT (1<<0)

#define SELECTOR_FLAG_ALLKEYS (1<<1)
#define SELECTOR_FLAG_ALLCOMMANDS (1<<2)
#define SELECTOR_FLAG_ALLCHANNELS (1<<3)


typedef struct {
sds name;
uint32_t flags;
list *passwords;
list *selectors;


} user;




#define CLIENT_ID_AOF (UINT64_MAX)









typedef struct replBacklog {
listNode *ref_repl_buf_node;

size_t unindexed_count;
rax *blocks_index;


long long histlen;
long long offset;

} replBacklog;

typedef struct {
list *clients;
size_t mem_usage_sum;
} clientMemUsageBucket;

typedef struct client {
uint64_t id;
connection *conn;
int resp;
redisDb *db;
robj *name;
sds querybuf;
size_t qb_pos;
sds pending_querybuf;



size_t querybuf_peak;
int argc;
robj **argv;
int argv_len;
int original_argc;
robj **original_argv;
size_t argv_len_sum;
struct redisCommand *cmd, *lastcmd;
user *user;


int reqtype;
int multibulklen;
long bulklen;
list *reply;
unsigned long long reply_bytes;
size_t sentlen;

time_t ctime;
long duration;
time_t lastinteraction;
time_t obuf_soft_limit_reached_time;
uint64_t flags;
int authenticated;
int replstate;
int repl_start_cmd_stream_on_ack;
int repldbfd;
off_t repldboff;
off_t repldbsize;
sds replpreamble;
long long read_reploff;
long long reploff;
long long repl_ack_off;
long long repl_ack_time;
long long repl_last_partial_write;
long long psync_initial_offset;


char replid[CONFIG_RUN_ID_SIZE+1];
int slave_listening_port;
char *slave_addr;
int slave_capa;
int slave_req;
multiState mstate;
int btype;
blockingState bpop;
long long woff;
list *watched_keys;
dict *pubsub_channels;
list *pubsub_patterns;
dict *pubsubshard_channels;
sds peerid;
sds sockname;
listNode *client_list_node;
listNode *postponed_list_node;
listNode *pending_read_list_node;
RedisModuleUserChangedFunc auth_callback;


void *auth_callback_privdata;


void *auth_module;






uint64_t client_tracking_redirection;
rax *client_tracking_prefixes;







size_t last_memory_usage;
int last_memory_type;

size_t last_memory_usage_on_bucket_update;
listNode *mem_usage_bucket_node;
clientMemUsageBucket *mem_usage_bucket;

listNode *ref_repl_buf_node;

size_t ref_block_pos;



int bufpos;
size_t buf_usable_size;





char buf[PROTO_REPLY_CHUNK_BYTES];
} client;

struct saveparam {
time_t seconds;
int changes;
};

struct moduleLoadQueueEntry {
sds path;
int argc;
robj **argv;
};

struct sentinelLoadQueueEntry {
int argc;
sds *argv;
int linenum;
sds line;
};

struct sentinelConfig {
list *pre_monitor_cfg;
list *monitor_cfg;
list *post_monitor_cfg;
};

struct sharedObjectsStruct {
robj *crlf, *ok, *err, *emptybulk, *czero, *cone, *pong, *space,
*queued, *null[4], *nullarray[4], *emptymap[4], *emptyset[4],
*emptyarray, *wrongtypeerr, *nokeyerr, *syntaxerr, *sameobjecterr,
*outofrangeerr, *noscripterr, *loadingerr,
*slowevalerr, *slowscripterr, *slowmoduleerr, *bgsaveerr,
*masterdownerr, *roslaveerr, *execaborterr, *noautherr, *noreplicaserr,
*busykeyerr, *oomerr, *plus, *messagebulk, *pmessagebulk, *subscribebulk,
*unsubscribebulk, *psubscribebulk, *punsubscribebulk, *del, *unlink,
*rpop, *lpop, *lpush, *rpoplpush, *lmove, *blmove, *zpopmin, *zpopmax,
*emptyscan, *multi, *exec, *left, *right, *hset, *srem, *xgroup, *xclaim,
*script, *replconf, *eval, *persist, *set, *pexpireat, *pexpire,
*time, *pxat, *absttl, *retrycount, *force, *justid,
*lastid, *ping, *setid, *keepttl, *load, *createconsumer,
*getack, *special_asterick, *special_equals, *default_username, *redacted,
*ssubscribebulk,*sunsubscribebulk,
*select[PROTO_SHARED_SELECT_CMDS],
*integers[OBJ_SHARED_INTEGERS],
*mbulkhdr[OBJ_SHARED_BULKHDR_LEN],
*bulkhdr[OBJ_SHARED_BULKHDR_LEN];
sds minstring, maxstring;
};


typedef struct zskiplistNode {
sds ele;
double score;
struct zskiplistNode *backward;
struct zskiplistLevel {
struct zskiplistNode *forward;
unsigned long span;
} level[];
} zskiplistNode;

typedef struct zskiplist {
struct zskiplistNode *header, *tail;
unsigned long length;
int level;
} zskiplist;

typedef struct zset {
dict *dict;
zskiplist *zsl;
} zset;

typedef struct clientBufferLimitsConfig {
unsigned long long hard_limit_bytes;
unsigned long long soft_limit_bytes;
time_t soft_limit_seconds;
} clientBufferLimitsConfig;

extern clientBufferLimitsConfig clientBufferLimitsDefaults[CLIENT_TYPE_OBUF_COUNT];







typedef struct redisOp {
robj **argv;
int argc, dbid, target;
} redisOp;








typedef struct redisOpArray {
redisOp *ops;
int numops;
int capacity;
} redisOpArray;



struct redisMemOverhead {
size_t peak_allocated;
size_t total_allocated;
size_t startup_allocated;
size_t repl_backlog;
size_t clients_slaves;
size_t clients_normal;
size_t cluster_links;
size_t aof_buffer;
size_t lua_caches;
size_t functions_caches;
size_t overhead_total;
size_t dataset;
size_t total_keys;
size_t bytes_per_key;
float dataset_perc;
float peak_perc;
float total_frag;
ssize_t total_frag_bytes;
float allocator_frag;
ssize_t allocator_frag_bytes;
float allocator_rss;
ssize_t allocator_rss_bytes;
float rss_extra;
size_t rss_extra_bytes;
size_t num_dbs;
struct {
size_t dbid;
size_t overhead_ht_main;
size_t overhead_ht_expires;
size_t overhead_ht_slot_to_keys;
} *db;
};









typedef struct rdbSaveInfo {

int repl_stream_db;


int repl_id_is_set;
char repl_id[CONFIG_RUN_ID_SIZE+1];
long long repl_offset;
} rdbSaveInfo;

#define RDB_SAVE_INFO_INIT {-1,0,"0000000000000000000000000000000000000000",-1}

struct malloc_stats {
size_t zmalloc_used;
size_t process_rss;
size_t allocator_allocated;
size_t allocator_active;
size_t allocator_resident;
};

typedef struct socketFds {
int fd[CONFIG_BINDADDR_MAX];
int count;
} socketFds;





typedef struct redisTLSContextConfig {
char *cert_file;
char *key_file;
char *key_file_pass;
char *client_cert_file;
char *client_key_file;
char *client_key_file_pass;
char *dh_params_file;
char *ca_cert_file;
char *ca_cert_dir;
char *protocols;
char *ciphers;
char *ciphersuites;
int prefer_server_ciphers;
int session_caching;
int session_cache_size;
int session_cache_timeout;
} redisTLSContextConfig;




typedef enum {
AOF_FILE_TYPE_BASE = 'b',
AOF_FILE_TYPE_HIST = 'h',
AOF_FILE_TYPE_INCR = 'i',
} aof_file_type;

typedef struct {
sds file_name;
long long file_seq;
aof_file_type file_type;
} aofInfo;

typedef struct {
aofInfo *base_aof_info;
list *incr_aof_list;
list *history_aof_list;


long long curr_base_file_seq;
long long curr_incr_file_seq;
int dirty;

} aofManifest;







#if defined(_AIX)
#undef hz
#endif

#define CHILD_TYPE_NONE 0
#define CHILD_TYPE_RDB 1
#define CHILD_TYPE_AOF 2
#define CHILD_TYPE_LDB 3
#define CHILD_TYPE_MODULE 4

typedef enum childInfoType {
CHILD_INFO_TYPE_CURRENT_INFO,
CHILD_INFO_TYPE_AOF_COW_SIZE,
CHILD_INFO_TYPE_RDB_COW_SIZE,
CHILD_INFO_TYPE_MODULE_COW_SIZE
} childInfoType;

struct redisServer {

pid_t pid;
pthread_t main_thread_id;
char *configfile;
char *executable;
char **exec_argv;
int dynamic_hz;
int config_hz;


mode_t umask;
int hz;
int in_fork_child;
redisDb *db;
dict *commands;
dict *orig_commands;
aeEventLoop *el;
rax *errors;
redisAtomic unsigned int lruclock;
volatile sig_atomic_t shutdown_asap;
mstime_t shutdown_mstime;
int shutdown_flags;
int activerehashing;
int active_defrag_running;
char *pidfile;
int arch_bits;
int cronloops;
char runid[CONFIG_RUN_ID_SIZE+1];
int sentinel_mode;
size_t initial_memory_usage;
int always_show_logo;
int in_script;
int in_exec;
int busy_module_yield_flags;
const char *busy_module_yield_reply;
int core_propagates;
int propagate_no_multi;
int module_ctx_nesting;
char *ignore_warnings;
int client_pause_in_transaction;
int thp_enabled;
size_t page_size;

dict *moduleapi;
dict *sharedapi;

list *loadmodule_queue;
int module_pipe[2];
pid_t child_pid;
int child_type;

int port;
int tls_port;
int tcp_backlog;
char *bindaddr[CONFIG_BINDADDR_MAX];
int bindaddr_count;
char *bind_source_addr;
char *unixsocket;
unsigned int unixsocketperm;
socketFds ipfd;
socketFds tlsfd;
int sofd;
socketFds cfd;
list *clients;
list *clients_to_close;
list *clients_pending_write;
list *clients_pending_read;
list *slaves, *monitors;
client *current_client;


clientMemUsageBucket client_mem_usage_buckets[CLIENT_MEM_USAGE_BUCKETS];

rax *clients_timeout_table;
long fixed_time_expire;
int in_nested_call;
rax *clients_index;
pause_type client_pause_type;
list *postponed_clients;
mstime_t client_pause_end_time;
pause_event *client_pause_per_purpose[NUM_PAUSE_PURPOSES];
char neterr[ANET_ERR_LEN];
dict *migrate_cached_sockets;
redisAtomic uint64_t next_client_id;
int protected_mode;
int io_threads_num;
int io_threads_do_reads;
int io_threads_active;
long long events_processed_while_blocked;
int enable_protected_configs;
int enable_debug_cmd;
int enable_module_cmd;


volatile sig_atomic_t loading;
volatile sig_atomic_t async_loading;
off_t loading_total_bytes;
off_t loading_rdb_used_mem;
off_t loading_loaded_bytes;
time_t loading_start_time;
off_t loading_process_events_interval_bytes;

time_t stat_starttime;
long long stat_numcommands;
long long stat_numconnections;
long long stat_expiredkeys;
double stat_expired_stale_perc;
long long stat_expired_time_cap_reached_count;
long long stat_expire_cycle_time_used;
long long stat_evictedkeys;
long long stat_evictedclients;
long long stat_total_eviction_exceeded_time;
monotime stat_last_eviction_exceeded_time;
long long stat_keyspace_hits;
long long stat_keyspace_misses;
long long stat_active_defrag_hits;
long long stat_active_defrag_misses;
long long stat_active_defrag_key_hits;
long long stat_active_defrag_key_misses;
long long stat_active_defrag_scanned;
long long stat_total_active_defrag_time;
monotime stat_last_active_defrag_time;
size_t stat_peak_memory;
long long stat_fork_time;
double stat_fork_rate;
long long stat_total_forks;
long long stat_rejected_conn;
long long stat_sync_full;
long long stat_sync_partial_ok;
long long stat_sync_partial_err;
list *slowlog;
long long slowlog_entry_id;
long long slowlog_log_slower_than;
unsigned long slowlog_max_len;
struct malloc_stats cron_malloc_stats;
redisAtomic long long stat_net_input_bytes;
redisAtomic long long stat_net_output_bytes;
size_t stat_current_cow_peak;
size_t stat_current_cow_bytes;
monotime stat_current_cow_updated;
size_t stat_current_save_keys_processed;
size_t stat_current_save_keys_total;
size_t stat_rdb_cow_bytes;
size_t stat_aof_cow_bytes;
size_t stat_module_cow_bytes;
double stat_module_progress;
redisAtomic size_t stat_clients_type_memory[CLIENT_TYPE_COUNT];
size_t stat_cluster_links_memory;
long long stat_unexpected_error_replies;
long long stat_total_error_replies;
long long stat_dump_payload_sanitizations;
long long stat_io_reads_processed;
long long stat_io_writes_processed;
redisAtomic long long stat_total_reads_processed;
redisAtomic long long stat_total_writes_processed;


struct {
long long last_sample_time;
long long last_sample_count;
long long samples[STATS_METRIC_SAMPLES];
int idx;
} inst_metric[STATS_METRIC_COUNT];

int verbosity;
int maxidletime;
int tcpkeepalive;
int active_expire_enabled;
int active_expire_effort;
int active_defrag_enabled;
int sanitize_dump_payload;
int skip_checksum_validation;
int jemalloc_bg_thread;
size_t active_defrag_ignore_bytes;
int active_defrag_threshold_lower;
int active_defrag_threshold_upper;
int active_defrag_cycle_min;
int active_defrag_cycle_max;
unsigned long active_defrag_max_scan_fields;
size_t client_max_querybuf_len;
int dbnum;
int supervised;
int supervised_mode;
int daemonize;
int set_proc_title;
char *proc_title_template;
clientBufferLimitsConfig client_obuf_limits[CLIENT_TYPE_OBUF_COUNT];
int pause_cron;
int latency_tracking_enabled;
double *latency_tracking_info_percentiles;
int latency_tracking_info_percentiles_len;

int aof_enabled;
int aof_state;
int aof_fsync;
char *aof_filename;
char *aof_dirname;
int aof_no_fsync_on_rewrite;
int aof_rewrite_perc;
off_t aof_rewrite_min_size;
off_t aof_rewrite_base_size;
off_t aof_current_size;
off_t aof_last_incr_size;
off_t aof_fsync_offset;
int aof_flush_sleep;
int aof_rewrite_scheduled;
sds aof_buf;
int aof_fd;
int aof_selected_db;
time_t aof_flush_postponed_start;
time_t aof_last_fsync;
time_t aof_rewrite_time_last;
time_t aof_rewrite_time_start;
time_t aof_cur_timestamp;
int aof_timestamp_enabled;
int aof_lastbgrewrite_status;
unsigned long aof_delayed_fsync;
int aof_rewrite_incremental_fsync;
int rdb_save_incremental_fsync;
int aof_last_write_status;
int aof_last_write_errno;
int aof_load_truncated;
int aof_use_rdb_preamble;
redisAtomic int aof_bio_fsync_status;
redisAtomic int aof_bio_fsync_errno;
aofManifest *aof_manifest;
int aof_disable_auto_gc;



long long dirty;
long long dirty_before_bgsave;
long long rdb_last_load_keys_expired;
long long rdb_last_load_keys_loaded;
struct saveparam *saveparams;
int saveparamslen;
char *rdb_filename;
int rdb_compression;
int rdb_checksum;
int rdb_del_sync_files;

time_t lastsave;
time_t lastbgsave_try;
time_t rdb_save_time_last;
time_t rdb_save_time_start;
int rdb_bgsave_scheduled;
int rdb_child_type;
int lastbgsave_status;
int stop_writes_on_bgsave_err;
int rdb_pipe_read;

int rdb_child_exit_pipe;
connection **rdb_pipe_conns;
int rdb_pipe_numconns;
int rdb_pipe_numconns_writing;
char *rdb_pipe_buff;
int rdb_pipe_bufflen;
int rdb_key_save_delay;


int key_load_delay;



int child_info_pipe[2];
int child_info_nread;

redisOpArray also_propagate;
int replication_allowed;

char *logfile;
int syslog_enabled;
char *syslog_ident;
int syslog_facility;
int crashlog_enabled;

int memcheck_enabled;
int use_exit_on_panic;


int shutdown_timeout;


char replid[CONFIG_RUN_ID_SIZE+1];
char replid2[CONFIG_RUN_ID_SIZE+1];
long long master_repl_offset;
long long second_replid_offset;
int slaveseldb;
int repl_ping_slave_period;
replBacklog *repl_backlog;
long long repl_backlog_size;
time_t repl_backlog_time_limit;

time_t repl_no_slaves_since;

int repl_min_slaves_to_write;
int repl_min_slaves_max_lag;
int repl_good_slaves_count;
int repl_diskless_sync;
int repl_diskless_load;

int repl_diskless_sync_delay;
int repl_diskless_sync_max_replicas;

size_t repl_buffer_mem;
list *repl_buffer_blocks;


char *masteruser;
sds masterauth;
char *masterhost;
int masterport;
int repl_timeout;
client *master;
client *cached_master;
int repl_syncio_timeout;
int repl_state;
off_t repl_transfer_size;
off_t repl_transfer_read;
off_t repl_transfer_last_fsync_off;
connection *repl_transfer_s;
int repl_transfer_fd;
char *repl_transfer_tmpfile;
time_t repl_transfer_lastio;
int repl_serve_stale_data;
int repl_slave_ro;
int repl_slave_ignore_maxmemory;
time_t repl_down_since;
int repl_disable_tcp_nodelay;
int slave_priority;
int replica_announced;
int slave_announce_port;
char *slave_announce_ip;



char master_replid[CONFIG_RUN_ID_SIZE+1];
long long master_initial_offset;
int repl_slave_lazy_flush;

dict *repl_scriptcache_dict;
list *repl_scriptcache_fifo;
unsigned int repl_scriptcache_size;

list *clients_waiting_acks;
int get_ack_from_slaves;

unsigned int maxclients;
unsigned long long maxmemory;
ssize_t maxmemory_clients;
int maxmemory_policy;
int maxmemory_samples;
int maxmemory_eviction_tenacity;
int lfu_log_factor;
int lfu_decay_time;
long long proto_max_bulk_len;
int oom_score_adj_values[CONFIG_OOM_COUNT];
int oom_score_adj;
int disable_thp;

unsigned int blocked_clients;
unsigned int blocked_clients_by_type[BLOCKED_NUM];
list *unblocked_clients;
list *ready_keys;

unsigned int tracking_clients;
size_t tracking_table_max_keys;
list *tracking_pending_keys;


int sort_desc;
int sort_alpha;
int sort_bypattern;
int sort_store;

size_t hash_max_listpack_entries;
size_t hash_max_listpack_value;
size_t set_max_intset_entries;
size_t zset_max_listpack_entries;
size_t zset_max_listpack_value;
size_t hll_sparse_max_bytes;
size_t stream_node_max_bytes;
long long stream_node_max_entries;

int list_max_listpack_size;
int list_compress_depth;

redisAtomic time_t unixtime;
time_t timezone;
int daylight_active;
mstime_t mstime;
ustime_t ustime;
size_t blocking_op_nesting;
long long blocked_last_cron;

dict *pubsub_channels;
dict *pubsub_patterns;
int notify_keyspace_events;

dict *pubsubshard_channels;

int cluster_enabled;
int cluster_port;
mstime_t cluster_node_timeout;
char *cluster_configfile;
struct clusterState *cluster;
int cluster_migration_barrier;
int cluster_allow_replica_migration;
int cluster_slave_validity_factor;
int cluster_require_full_coverage;

int cluster_slave_no_failover;

char *cluster_announce_ip;
char *cluster_announce_hostname;
int cluster_preferred_endpoint_type;
int cluster_announce_port;
int cluster_announce_tls_port;
int cluster_announce_bus_port;
int cluster_module_flags;



int cluster_allow_reads_when_down;

int cluster_config_file_lock_fd;
unsigned long long cluster_link_sendbuf_limit_bytes;
int cluster_drop_packet_filter;


client *script_caller;
mstime_t busy_reply_threshold;
int script_oom;
int script_disable_deny_script;

int lazyfree_lazy_eviction;
int lazyfree_lazy_expire;
int lazyfree_lazy_server_del;
int lazyfree_lazy_user_del;
int lazyfree_lazy_user_flush;

long long latency_monitor_threshold;
dict *latency_events;

char *acl_filename;
unsigned long acllog_max_len;
sds requirepass;


int acl_pubsub_default;

int watchdog_period;

size_t system_memory_size;

int tls_cluster;
int tls_replication;
int tls_auth_clients;
redisTLSContextConfig tls_ctx_config;

char *server_cpulist;
char *bio_cpulist;
char *aof_rewrite_cpulist;
char *bgsave_cpulist;

struct sentinelConfig *sentinel_config;

mstime_t failover_end_time;
int force_failover;

char *target_replica_host;

int target_replica_port;
int failover_state;
int cluster_allow_pubsubshard_when_down;

};

#define MAX_KEYS_BUFFER 256

typedef struct {
int pos;
int flags;

} keyReference;




typedef struct {
keyReference keysbuf[MAX_KEYS_BUFFER];
keyReference *keys;
int numkeys;
int size;
} getKeysResult;
#define GETKEYS_RESULT_INIT { {{0}}, NULL, 0, MAX_KEYS_BUFFER }





















typedef enum {
KSPEC_BS_INVALID = 0,
KSPEC_BS_UNKNOWN,
KSPEC_BS_INDEX,
KSPEC_BS_KEYWORD
} kspec_bs_type;


typedef enum {
KSPEC_FK_INVALID = 0,
KSPEC_FK_UNKNOWN,
KSPEC_FK_RANGE,
KSPEC_FK_KEYNUM
} kspec_fk_type;

typedef struct {

uint64_t flags;
kspec_bs_type begin_search_type;
union {
struct {

int pos;
} index;
struct {

const char *keyword;



int startfrom;
} keyword;
} bs;
kspec_fk_type find_keys_type;
union {


struct {



int lastkey;

int keystep;


int limit;
} range;
struct {

int keynumidx;


int firstkey;

int keystep;
} keynum;
} fk;
} keySpec;


#define STATIC_KEY_SPECS_NUM 4


typedef enum {
ARG_TYPE_STRING,
ARG_TYPE_INTEGER,
ARG_TYPE_DOUBLE,
ARG_TYPE_KEY,
ARG_TYPE_PATTERN,
ARG_TYPE_UNIX_TIME,
ARG_TYPE_PURE_TOKEN,
ARG_TYPE_ONEOF,
ARG_TYPE_BLOCK
} redisCommandArgType;

#define CMD_ARG_NONE (0)
#define CMD_ARG_OPTIONAL (1<<0)
#define CMD_ARG_MULTIPLE (1<<1)
#define CMD_ARG_MULTIPLE_TOKEN (1<<2)

typedef struct redisCommandArg {
const char *name;
redisCommandArgType type;
int key_spec_index;
const char *token;
const char *summary;
const char *since;
int flags;
struct redisCommandArg *subargs;

int num_args;
} redisCommandArg;


typedef enum {
RESP2_SIMPLE_STRING,
RESP2_ERROR,
RESP2_INTEGER,
RESP2_BULK_STRING,
RESP2_NULL_BULK_STRING,
RESP2_ARRAY,
RESP2_NULL_ARRAY,
} redisCommandRESP2Type;


typedef enum {
RESP3_SIMPLE_STRING,
RESP3_ERROR,
RESP3_INTEGER,
RESP3_DOUBLE,
RESP3_BULK_STRING,
RESP3_ARRAY,
RESP3_MAP,
RESP3_SET,
RESP3_BOOL,
RESP3_NULL,
} redisCommandRESP3Type;

typedef struct {
const char *since;
const char *changes;
} commandHistory;


typedef enum {
COMMAND_GROUP_GENERIC,
COMMAND_GROUP_STRING,
COMMAND_GROUP_LIST,
COMMAND_GROUP_SET,
COMMAND_GROUP_SORTED_SET,
COMMAND_GROUP_HASH,
COMMAND_GROUP_PUBSUB,
COMMAND_GROUP_TRANSACTIONS,
COMMAND_GROUP_CONNECTION,
COMMAND_GROUP_SERVER,
COMMAND_GROUP_SCRIPTING,
COMMAND_GROUP_HYPERLOGLOG,
COMMAND_GROUP_CLUSTER,
COMMAND_GROUP_SENTINEL,
COMMAND_GROUP_GEO,
COMMAND_GROUP_STREAM,
COMMAND_GROUP_BITMAP,
COMMAND_GROUP_MODULE,
} redisCommandGroup;

typedef void redisCommandProc(client *c);
typedef int redisGetKeysProc(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);























































































struct redisCommand {

const char *declared_name;

const char *summary;
const char *complexity;
const char *since;
int doc_flags;
const char *replaced_by;
const char *deprecated_since;
redisCommandGroup group;
commandHistory *history;
const char **tips;
redisCommandProc *proc;
int arity;
uint64_t flags;
uint64_t acl_categories;
keySpec key_specs_static[STATIC_KEY_SPECS_NUM];


redisGetKeysProc *getkeys_proc;

struct redisCommand *subcommands;

struct redisCommandArg *args;



long long microseconds, calls, rejected_calls, failed_calls;
int id;




sds fullname;
struct hdr_histogram* latency_histogram;
keySpec *key_specs;
keySpec legacy_range_key_spec;



int num_args;
int num_history;
int num_tips;
int key_specs_num;
int key_specs_max;
dict *subcommands_dict;

struct redisCommand *parent;
};

struct redisError {
long long count;
};

struct redisFunctionSym {
char *name;
unsigned long pointer;
};

typedef struct _redisSortObject {
robj *obj;
union {
double score;
robj *cmpobj;
} u;
} redisSortObject;

typedef struct _redisSortOperation {
int type;
robj *pattern;
} redisSortOperation;


typedef struct {
robj *subject;
unsigned char encoding;
unsigned char direction;
quicklistIter *iter;
} listTypeIterator;


typedef struct {
listTypeIterator *li;
quicklistEntry entry;
} listTypeEntry;


typedef struct {
robj *subject;
int encoding;
int ii;
dictIterator *di;
} setTypeIterator;





typedef struct {
robj *subject;
int encoding;

unsigned char *fptr, *vptr;

dictIterator *di;
dictEntry *de;
} hashTypeIterator;

#include "stream.h"

#define OBJ_HASH_KEY 1
#define OBJ_HASH_VALUE 2

#define IO_THREADS_OP_IDLE 0
#define IO_THREADS_OP_READ 1
#define IO_THREADS_OP_WRITE 2
extern int io_threads_op;





extern struct redisServer server;
extern struct sharedObjectsStruct shared;
extern dictType objectKeyPointerValueDictType;
extern dictType objectKeyHeapPointerValueDictType;
extern dictType setDictType;
extern dictType zsetDictType;
extern dictType dbDictType;
extern double R_Zero, R_PosInf, R_NegInf, R_Nan;
extern dictType hashDictType;
extern dictType replScriptCacheDictType;
extern dictType dbExpiresDictType;
extern dictType modulesDictType;
extern dictType sdsReplyDictType;
extern dict *modules;






void populateCommandLegacyRangeSpec(struct redisCommand *c);


void moduleInitModulesSystem(void);
void moduleInitModulesSystemLast(void);
void modulesCron(void);
int moduleLoad(const char *path, void **argv, int argc);
void moduleLoadFromQueue(void);
int moduleGetCommandKeysViaAPI(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
moduleType *moduleTypeLookupModuleByID(uint64_t id);
void moduleTypeNameByID(char *name, uint64_t moduleid);
const char *moduleTypeModuleName(moduleType *mt);
void moduleFreeContext(struct RedisModuleCtx *ctx);
void unblockClientFromModule(client *c);
void moduleHandleBlockedClients(void);
void moduleBlockedClientTimedOut(client *c);
void modulePipeReadable(aeEventLoop *el, int fd, void *privdata, int mask);
size_t moduleCount(void);
void moduleAcquireGIL(void);
int moduleTryAcquireGIL(void);
void moduleReleaseGIL(void);
void moduleNotifyKeyspaceEvent(int type, const char *event, robj *key, int dbid);
void moduleCallCommandFilters(client *c);
void ModuleForkDoneHandler(int exitcode, int bysignal);
int TerminateModuleForkChild(int child_pid, int wait);
ssize_t rdbSaveModulesAux(rio *rdb, int when);
int moduleAllDatatypesHandleErrors();
int moduleAllModulesHandleReplAsyncLoad();
sds modulesCollectInfo(sds info, const char *section, int for_crash_report, int sections);
void moduleFireServerEvent(uint64_t eid, int subid, void *data);
void processModuleLoadingProgressEvent(int is_aof);
int moduleTryServeClientBlockedOnKey(client *c, robj *key);
void moduleUnblockClient(client *c);
int moduleBlockedClientMayTimeout(client *c);
int moduleClientIsBlockedOnKeys(client *c);
void moduleNotifyUserChanged(client *c);
void moduleNotifyKeyUnlink(robj *key, robj *val, int dbid);
size_t moduleGetFreeEffort(robj *key, robj *val, int dbid);
size_t moduleGetMemUsage(robj *key, robj *val, size_t sample_size, int dbid);
robj *moduleTypeDupOrReply(client *c, robj *fromkey, robj *tokey, int todb, robj *value);
int moduleDefragValue(robj *key, robj *obj, long *defragged, int dbid);
int moduleLateDefrag(robj *key, robj *value, unsigned long *cursor, long long endtime, long long *defragged, int dbid);
long moduleDefragGlobals(void);
void *moduleGetHandleByName(char *modulename);
int moduleIsModuleCommand(void *module_handle, struct redisCommand *cmd);


long long ustime(void);
long long mstime(void);
void getRandomHexChars(char *p, size_t len);
void getRandomBytes(unsigned char *p, size_t len);
uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l);
void exitFromChild(int retcode);
long long redisPopcount(void *s, long count);
int redisSetProcTitle(char *title);
int validateProcTitleTemplate(const char *template);
int redisCommunicateSystemd(const char *sd_notify_msg);
void redisSetCpuAffinity(const char *cpulist);


client *createClient(connection *conn);
void freeClient(client *c);
void freeClientAsync(client *c);
void logInvalidUseAndFreeClientAsync(client *c, const char *fmt, ...);
int beforeNextClient(client *c);
void clearClientConnectionState(client *c);
void resetClient(client *c);
void freeClientOriginalArgv(client *c);
void sendReplyToClient(connection *conn);
void *addReplyDeferredLen(client *c);
void setDeferredArrayLen(client *c, void *node, long length);
void setDeferredMapLen(client *c, void *node, long length);
void setDeferredSetLen(client *c, void *node, long length);
void setDeferredAttributeLen(client *c, void *node, long length);
void setDeferredPushLen(client *c, void *node, long length);
int processInputBuffer(client *c);
void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void acceptTLSHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void acceptUnixHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void readQueryFromClient(connection *conn);
int prepareClientToWrite(client *c);
void addReplyNull(client *c);
void addReplyNullArray(client *c);
void addReplyBool(client *c, int b);
void addReplyVerbatim(client *c, const char *s, size_t len, const char *ext);
void addReplyProto(client *c, const char *s, size_t len);
void AddReplyFromClient(client *c, client *src);
void addReplyBulk(client *c, robj *obj);
void addReplyBulkCString(client *c, const char *s);
void addReplyBulkCBuffer(client *c, const void *p, size_t len);
void addReplyBulkLongLong(client *c, long long ll);
void addReply(client *c, robj *obj);
void addReplySds(client *c, sds s);
void addReplyBulkSds(client *c, sds s);
void setDeferredReplyBulkSds(client *c, void *node, sds s);
void addReplyErrorObject(client *c, robj *err);
void addReplyOrErrorObject(client *c, robj *reply);
void addReplyErrorSds(client *c, sds err);
void addReplyError(client *c, const char *err);
void addReplyErrorArity(client *c);
void addReplyErrorExpireTime(client *c);
void addReplyStatus(client *c, const char *status);
void addReplyDouble(client *c, double d);
void addReplyLongLongWithPrefix(client *c, long long ll, char prefix);
void addReplyBigNum(client *c, const char* num, size_t len);
void addReplyHumanLongDouble(client *c, long double d);
void addReplyLongLong(client *c, long long ll);
void addReplyArrayLen(client *c, long length);
void addReplyMapLen(client *c, long length);
void addReplySetLen(client *c, long length);
void addReplyAttributeLen(client *c, long length);
void addReplyPushLen(client *c, long length);
void addReplyHelp(client *c, const char **help);
void addReplySubcommandSyntaxError(client *c);
void addReplyLoadedModules(client *c);
void copyReplicaOutputBuffer(client *dst, client *src);
void addListRangeReply(client *c, robj *o, long start, long end, int reverse);
size_t sdsZmallocSize(sds s);
size_t getStringObjectSdsUsedMemory(robj *o);
void freeClientReplyValue(void *o);
void *dupClientReplyValue(void *o);
char *getClientPeerId(client *client);
char *getClientSockName(client *client);
sds catClientInfoString(sds s, client *client);
sds getAllClientsInfoString(int type);
void rewriteClientCommandVector(client *c, int argc, ...);
void rewriteClientCommandArgument(client *c, int i, robj *newval);
void replaceClientCommandVector(client *c, int argc, robj **argv);
void redactClientCommandArgument(client *c, int argc);
size_t getClientOutputBufferMemoryUsage(client *c);
size_t getClientMemoryUsage(client *c, size_t *output_buffer_mem_usage);
int freeClientsInAsyncFreeQueue(void);
int closeClientOnOutputBufferLimitReached(client *c, int async);
int getClientType(client *c);
int getClientTypeByName(char *name);
char *getClientTypeName(int class);
void flushSlavesOutputBuffers(void);
void disconnectSlaves(void);
void evictClients(void);
int listenToPort(int port, socketFds *fds);
void pauseClients(pause_purpose purpose, mstime_t end, pause_type type);
void unpauseClients(pause_purpose purpose);
int areClientsPaused(void);
int checkClientPauseTimeoutAndReturnIfPaused(void);
void unblockPostponedClients();
void processEventsWhileBlocked(void);
void whileBlockedCron();
void blockingOperationStarts();
void blockingOperationEnds();
int handleClientsWithPendingWrites(void);
int handleClientsWithPendingWritesUsingThreads(void);
int handleClientsWithPendingReadsUsingThreads(void);
int stopThreadedIOIfNeeded(void);
int clientHasPendingReplies(client *c);
int islocalClient(client *c);
int updateClientMemUsage(client *c);
void updateClientMemUsageBucket(client *c);
void unlinkClient(client *c);
int writeToClient(client *c, int handler_installed);
void linkClient(client *c);
void protectClient(client *c);
void unprotectClient(client *c);
void initThreadedIO(void);
client *lookupClientByID(uint64_t id);
int authRequired(client *c);
void clientInstallWriteHandler(client *c);

#if defined(__GNUC__)
void addReplyErrorFormat(client *c, const char *fmt, ...)
__attribute__((format(printf, 2, 3)));
void addReplyStatusFormat(client *c, const char *fmt, ...)
__attribute__((format(printf, 2, 3)));
#else
void addReplyErrorFormat(client *c, const char *fmt, ...);
void addReplyStatusFormat(client *c, const char *fmt, ...);
#endif


void enableTracking(client *c, uint64_t redirect_to, uint64_t options, robj **prefix, size_t numprefix);
void disableTracking(client *c);
void trackingRememberKeys(client *c);
void trackingInvalidateKey(client *c, robj *keyobj, int bcast);
void trackingScheduleKeyInvalidation(uint64_t client_id, robj *keyobj);
void trackingHandlePendingKeyInvalidations(void);
void trackingInvalidateKeysOnFlush(int async);
void freeTrackingRadixTree(rax *rt);
void freeTrackingRadixTreeAsync(rax *rt);
void trackingLimitUsedSlots(void);
uint64_t trackingGetTotalItems(void);
uint64_t trackingGetTotalKeys(void);
uint64_t trackingGetTotalPrefixes(void);
void trackingBroadcastInvalidationMessages(void);
int checkPrefixCollisionsOrReply(client *c, robj **prefix, size_t numprefix);


void listTypePush(robj *subject, robj *value, int where);
robj *listTypePop(robj *subject, int where);
unsigned long listTypeLength(const robj *subject);
listTypeIterator *listTypeInitIterator(robj *subject, long index, unsigned char direction);
void listTypeReleaseIterator(listTypeIterator *li);
void listTypeSetIteratorDirection(listTypeIterator *li, unsigned char direction);
int listTypeNext(listTypeIterator *li, listTypeEntry *entry);
robj *listTypeGet(listTypeEntry *entry);
void listTypeInsert(listTypeEntry *entry, robj *value, int where);
void listTypeReplace(listTypeEntry *entry, robj *value);
int listTypeEqual(listTypeEntry *entry, robj *o);
void listTypeDelete(listTypeIterator *iter, listTypeEntry *entry);
robj *listTypeDup(robj *o);
int listTypeDelRange(robj *o, long start, long stop);
void unblockClientWaitingData(client *c);
void popGenericCommand(client *c, int where);
void listElementsRemoved(client *c, robj *key, int where, robj *o, long count, int *deleted);


void unwatchAllKeys(client *c);
void initClientMultiState(client *c);
void freeClientMultiState(client *c);
void queueMultiCommand(client *c);
size_t multiStateMemOverhead(client *c);
void touchWatchedKey(redisDb *db, robj *key);
int isWatchedKeyExpired(client *c);
void touchAllWatchedKeysInDb(redisDb *emptied, redisDb *replaced_with);
void discardTransaction(client *c);
void flagTransaction(client *c);
void execCommandAbort(client *c, sds error);


void decrRefCount(robj *o);
void decrRefCountVoid(void *o);
void incrRefCount(robj *o);
robj *makeObjectShared(robj *o);
robj *resetRefCount(robj *obj);
void freeStringObject(robj *o);
void freeListObject(robj *o);
void freeSetObject(robj *o);
void freeZsetObject(robj *o);
void freeHashObject(robj *o);
void dismissObject(robj *o, size_t dump_size);
robj *createObject(int type, void *ptr);
robj *createStringObject(const char *ptr, size_t len);
robj *createRawStringObject(const char *ptr, size_t len);
robj *createEmbeddedStringObject(const char *ptr, size_t len);
robj *tryCreateRawStringObject(const char *ptr, size_t len);
robj *tryCreateStringObject(const char *ptr, size_t len);
robj *dupStringObject(const robj *o);
int isSdsRepresentableAsLongLong(sds s, long long *llval);
int isObjectRepresentableAsLongLong(robj *o, long long *llongval);
robj *tryObjectEncoding(robj *o);
robj *getDecodedObject(robj *o);
size_t stringObjectLen(robj *o);
robj *createStringObjectFromLongLong(long long value);
robj *createStringObjectFromLongLongForValue(long long value);
robj *createStringObjectFromLongDouble(long double value, int humanfriendly);
robj *createQuicklistObject(void);
robj *createSetObject(void);
robj *createIntsetObject(void);
robj *createHashObject(void);
robj *createZsetObject(void);
robj *createZsetListpackObject(void);
robj *createStreamObject(void);
robj *createModuleObject(moduleType *mt, void *value);
int getLongFromObjectOrReply(client *c, robj *o, long *target, const char *msg);
int getPositiveLongFromObjectOrReply(client *c, robj *o, long *target, const char *msg);
int getRangeLongFromObjectOrReply(client *c, robj *o, long min, long max, long *target, const char *msg);
int checkType(client *c, robj *o, int type);
int getLongLongFromObjectOrReply(client *c, robj *o, long long *target, const char *msg);
int getDoubleFromObjectOrReply(client *c, robj *o, double *target, const char *msg);
int getDoubleFromObject(const robj *o, double *target);
int getLongLongFromObject(robj *o, long long *target);
int getLongDoubleFromObject(robj *o, long double *target);
int getLongDoubleFromObjectOrReply(client *c, robj *o, long double *target, const char *msg);
int getIntFromObjectOrReply(client *c, robj *o, int *target, const char *msg);
char *strEncoding(int encoding);
int compareStringObjects(robj *a, robj *b);
int collateStringObjects(robj *a, robj *b);
int equalStringObjects(robj *a, robj *b);
unsigned long long estimateObjectIdleTime(robj *o);
void trimStringObjectIfNeeded(robj *o);
#define sdsEncodedObject(objptr) (objptr->encoding == OBJ_ENCODING_RAW || objptr->encoding == OBJ_ENCODING_EMBSTR)


ssize_t syncWrite(int fd, char *ptr, ssize_t size, long long timeout);
ssize_t syncRead(int fd, char *ptr, ssize_t size, long long timeout);
ssize_t syncReadLine(int fd, char *ptr, ssize_t size, long long timeout);


void replicationFeedSlaves(list *slaves, int dictid, robj **argv, int argc);
void replicationFeedStreamFromMasterStream(char *buf, size_t buflen);
void resetReplicationBuffer(void);
void feedReplicationBuffer(char *buf, size_t len);
void freeReplicaReferencedReplBuffer(client *replica);
void replicationFeedMonitors(client *c, list *monitors, int dictid, robj **argv, int argc);
void updateSlavesWaitingBgsave(int bgsaveerr, int type);
void replicationCron(void);
void replicationStartPendingFork(void);
void replicationHandleMasterDisconnection(void);
void replicationCacheMaster(client *c);
void resizeReplicationBacklog();
void replicationSetMaster(char *ip, int port);
void replicationUnsetMaster(void);
void refreshGoodSlavesCount(void);
void processClientsWaitingReplicas(void);
void unblockClientWaitingReplicas(client *c);
int replicationCountAcksByOffset(long long offset);
void replicationSendNewlineToMaster(void);
long long replicationGetSlaveOffset(void);
char *replicationGetSlaveName(client *c);
long long getPsyncInitialOffset(void);
int replicationSetupSlaveForFullResync(client *slave, long long offset);
void changeReplicationId(void);
void clearReplicationId2(void);
void createReplicationBacklog(void);
void freeReplicationBacklog(void);
void replicationCacheMasterUsingMyself(void);
void feedReplicationBacklog(void *ptr, size_t len);
void incrementalTrimReplicationBacklog(size_t blocks);
int canFeedReplicaReplBuffer(client *replica);
void rebaseReplicationBuffer(long long base_repl_offset);
void showLatestBacklog(void);
void rdbPipeReadHandler(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
void rdbPipeWriteHandlerConnRemoved(struct connection *conn);
void clearFailoverState(void);
void updateFailoverStatus(void);
void abortFailover(const char *err);
const char *getFailoverStateString();


void startLoadingFile(size_t size, char* filename, int rdbflags);
void startLoading(size_t size, int rdbflags, int async);
void loadingAbsProgress(off_t pos);
void loadingIncrProgress(off_t size);
void stopLoading(int success);
void updateLoadingFileName(char* filename);
void startSaving(int rdbflags);
void stopSaving(int success);
int allPersistenceDisabled(void);

#define DISK_ERROR_TYPE_AOF 1
#define DISK_ERROR_TYPE_RDB 2
#define DISK_ERROR_TYPE_NONE 0
int writeCommandsDeniedByDiskError(void);


#include "rdb.h"
void killRDBChild(void);
int bg_unlink(const char *filename);


void flushAppendOnlyFile(int force);
void feedAppendOnlyFile(int dictid, robj **argv, int argc);
void aofRemoveTempFile(pid_t childpid);
int rewriteAppendOnlyFileBackground(void);
int loadAppendOnlyFiles(aofManifest *am);
void stopAppendOnly(void);
int startAppendOnly(void);
void backgroundRewriteDoneHandler(int exitcode, int bysignal);
ssize_t aofReadDiffFromParent(void);
void killAppendOnlyChild(void);
void restartAOFAfterSYNC();
void aofLoadManifestFromDisk(void);
void aofOpenIfNeededOnServerStart(void);
void aofManifestFree(aofManifest *am);
int aofDelHistoryFiles(void);
int aofRewriteLimited(void);


void openChildInfoPipe(void);
void closeChildInfoPipe(void);
void sendChildInfoGeneric(childInfoType info_type, size_t keys, double progress, char *pname);
void sendChildCowInfo(childInfoType info_type, char *pname);
void sendChildInfo(childInfoType info_type, size_t keys, char *pname);
void receiveChildInfo(void);


int redisFork(int type);
int hasActiveChildProcess();
void resetChildState();
int isMutuallyExclusiveChildType(int type);


extern rax *Users;
extern user *DefaultUser;
void ACLInit(void);

#define ACL_OK 0
#define ACL_DENIED_CMD 1
#define ACL_DENIED_KEY 2
#define ACL_DENIED_AUTH 3
#define ACL_DENIED_CHANNEL 4


#define ACL_LOG_CTX_TOPLEVEL 0
#define ACL_LOG_CTX_LUA 1
#define ACL_LOG_CTX_MULTI 2
#define ACL_LOG_CTX_MODULE 3


#define ACL_READ_PERMISSION (1<<0)
#define ACL_WRITE_PERMISSION (1<<1)
#define ACL_ALL_PERMISSION (ACL_READ_PERMISSION|ACL_WRITE_PERMISSION)

int ACLCheckUserCredentials(robj *username, robj *password);
int ACLAuthenticateUser(client *c, robj *username, robj *password);
unsigned long ACLGetCommandID(sds cmdname);
void ACLClearCommandID(void);
user *ACLGetUserByName(const char *name, size_t namelen);
int ACLUserCheckKeyPerm(user *u, const char *key, int keylen, int flags);
int ACLUserCheckChannelPerm(user *u, sds channel, int literal);
int ACLCheckAllUserCommandPerm(user *u, struct redisCommand *cmd, robj **argv, int argc, int *idxptr);
int ACLCheckAllPerm(client *c, int *idxptr);
int ACLSetUser(user *u, const char *op, ssize_t oplen);
uint64_t ACLGetCommandCategoryFlagByName(const char *name);
int ACLAppendUserForLoading(sds *argv, int argc, int *argc_err);
const char *ACLSetUserStringError(void);
int ACLLoadConfiguredUsers(void);
sds ACLDescribeUser(user *u);
void ACLLoadUsersAtStartup(void);
void addReplyCommandCategories(client *c, struct redisCommand *cmd);
user *ACLCreateUnlinkedUser();
void ACLFreeUserAndKillClients(user *u);
void addACLLogEntry(client *c, int reason, int context, int argpos, sds username, sds object);
void ACLUpdateDefaultUserPassword(sds password);




#define ZADD_IN_NONE 0
#define ZADD_IN_INCR (1<<0)
#define ZADD_IN_NX (1<<1)
#define ZADD_IN_XX (1<<2)
#define ZADD_IN_GT (1<<3)
#define ZADD_IN_LT (1<<4)


#define ZADD_OUT_NOP (1<<0)
#define ZADD_OUT_NAN (1<<1)
#define ZADD_OUT_ADDED (1<<2)
#define ZADD_OUT_UPDATED (1<<3)


typedef struct {
double min, max;
int minex, maxex;
} zrangespec;


typedef struct {
sds min, max;
int minex, maxex;
} zlexrangespec;

zskiplist *zslCreate(void);
void zslFree(zskiplist *zsl);
zskiplistNode *zslInsert(zskiplist *zsl, double score, sds ele);
unsigned char *zzlInsert(unsigned char *zl, sds ele, double score);
int zslDelete(zskiplist *zsl, double score, sds ele, zskiplistNode **node);
zskiplistNode *zslFirstInRange(zskiplist *zsl, zrangespec *range);
zskiplistNode *zslLastInRange(zskiplist *zsl, zrangespec *range);
double zzlGetScore(unsigned char *sptr);
void zzlNext(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);
void zzlPrev(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);
unsigned char *zzlFirstInRange(unsigned char *zl, zrangespec *range);
unsigned char *zzlLastInRange(unsigned char *zl, zrangespec *range);
unsigned long zsetLength(const robj *zobj);
void zsetConvert(robj *zobj, int encoding);
void zsetConvertToListpackIfNeeded(robj *zobj, size_t maxelelen, size_t totelelen);
int zsetScore(robj *zobj, sds member, double *score);
unsigned long zslGetRank(zskiplist *zsl, double score, sds o);
int zsetAdd(robj *zobj, double score, sds ele, int in_flags, int *out_flags, double *newscore);
long zsetRank(robj *zobj, sds ele, int reverse);
int zsetDel(robj *zobj, sds ele);
robj *zsetDup(robj *o);
void genericZpopCommand(client *c, robj **keyv, int keyc, int where, int emitkey, long count, int use_nested_array, int reply_nil_when_empty, int *deleted);
sds lpGetObject(unsigned char *sptr);
int zslValueGteMin(double value, zrangespec *spec);
int zslValueLteMax(double value, zrangespec *spec);
void zslFreeLexRange(zlexrangespec *spec);
int zslParseLexRange(robj *min, robj *max, zlexrangespec *spec);
unsigned char *zzlFirstInLexRange(unsigned char *zl, zlexrangespec *range);
unsigned char *zzlLastInLexRange(unsigned char *zl, zlexrangespec *range);
zskiplistNode *zslFirstInLexRange(zskiplist *zsl, zlexrangespec *range);
zskiplistNode *zslLastInLexRange(zskiplist *zsl, zlexrangespec *range);
int zzlLexValueGteMin(unsigned char *p, zlexrangespec *spec);
int zzlLexValueLteMax(unsigned char *p, zlexrangespec *spec);
int zslLexValueGteMin(sds value, zlexrangespec *spec);
int zslLexValueLteMax(sds value, zlexrangespec *spec);


int getMaxmemoryState(size_t *total, size_t *logical, size_t *tofree, float *level);
size_t freeMemoryGetNotCountedMemory();
int overMaxmemoryAfterAlloc(size_t moremem);
int processCommand(client *c);
int processPendingCommandsAndResetClient(client *c);
void setupSignalHandlers(void);
void removeSignalHandlers(void);
int createSocketAcceptHandler(socketFds *sfd, aeFileProc *accept_handler);
int changeListenPort(int port, socketFds *sfd, aeFileProc *accept_handler);
int changeBindAddr(void);
struct redisCommand *lookupSubcommand(struct redisCommand *container, sds sub_name);
struct redisCommand *lookupCommand(robj **argv, int argc);
struct redisCommand *lookupCommandBySdsLogic(dict *commands, sds s);
struct redisCommand *lookupCommandBySds(sds s);
struct redisCommand *lookupCommandByCStringLogic(dict *commands, const char *s);
struct redisCommand *lookupCommandByCString(const char *s);
struct redisCommand *lookupCommandOrOriginal(robj **argv, int argc);
void call(client *c, int flags);
void alsoPropagate(int dbid, robj **argv, int argc, int target);
void propagatePendingCommands();
void redisOpArrayInit(redisOpArray *oa);
void redisOpArrayFree(redisOpArray *oa);
void forceCommandPropagation(client *c, int flags);
void preventCommandPropagation(client *c);
void preventCommandAOF(client *c);
void preventCommandReplication(client *c);
void slowlogPushCurrentCommand(client *c, struct redisCommand *cmd, ustime_t duration);
void updateCommandLatencyHistogram(struct hdr_histogram** latency_histogram, int64_t duration_hist);
int prepareForShutdown(int flags);
void replyToClientsBlockedOnShutdown(void);
int abortShutdown(void);
void afterCommand(client *c);
int inNestedCall(void);
#if defined(__GNUC__)
void _serverLog(int level, const char *fmt, ...)
__attribute__((format(printf, 2, 3)));
#else
void _serverLog(int level, const char *fmt, ...);
#endif
void serverLogRaw(int level, const char *msg);
void serverLogFromHandler(int level, const char *msg);
void usage(void);
void updateDictResizePolicy(void);
int htNeedsResize(dict *dict);
void populateCommandTable(void);
void resetCommandTableStats(dict* commands);
void resetErrorTableStats(void);
void adjustOpenFilesLimit(void);
void incrementErrorCount(const char *fullerr, size_t namelen);
void closeListeningSockets(int unlink_unix_socket);
void updateCachedTime(int update_daylight_info);
void resetServerStats(void);
void activeDefragCycle(void);
unsigned int getLRUClock(void);
unsigned int LRU_CLOCK(void);
const char *evictPolicyToString(void);
struct redisMemOverhead *getMemoryOverheadData(void);
void freeMemoryOverheadData(struct redisMemOverhead *mh);
void checkChildrenDone(void);
int setOOMScoreAdj(int process_class);
void rejectCommandFormat(client *c, const char *fmt, ...);
void *activeDefragAlloc(void *ptr);
robj *activeDefragStringOb(robj* ob, long *defragged);
void dismissSds(sds s);
void dismissMemory(void* ptr, size_t size_hint);
void dismissMemoryInChild(void);

#define RESTART_SERVER_NONE 0
#define RESTART_SERVER_GRACEFULLY (1<<0)
#define RESTART_SERVER_CONFIG_REWRITE (1<<1)
int restartServer(int flags, mstime_t delay);


robj *setTypeCreate(sds value);
int setTypeAdd(robj *subject, sds value);
int setTypeRemove(robj *subject, sds value);
int setTypeIsMember(robj *subject, sds value);
setTypeIterator *setTypeInitIterator(robj *subject);
void setTypeReleaseIterator(setTypeIterator *si);
int setTypeNext(setTypeIterator *si, sds *sdsele, int64_t *llele);
sds setTypeNextObject(setTypeIterator *si);
int setTypeRandomElement(robj *setobj, sds *sdsele, int64_t *llele);
unsigned long setTypeRandomElements(robj *set, unsigned long count, robj *aux_set);
unsigned long setTypeSize(const robj *subject);
void setTypeConvert(robj *subject, int enc);
robj *setTypeDup(robj *o);


#define HASH_SET_TAKE_FIELD (1<<0)
#define HASH_SET_TAKE_VALUE (1<<1)
#define HASH_SET_COPY 0

void hashTypeConvert(robj *o, int enc);
void hashTypeTryConversion(robj *subject, robj **argv, int start, int end);
int hashTypeExists(robj *o, sds key);
int hashTypeDelete(robj *o, sds key);
unsigned long hashTypeLength(const robj *o);
hashTypeIterator *hashTypeInitIterator(robj *subject);
void hashTypeReleaseIterator(hashTypeIterator *hi);
int hashTypeNext(hashTypeIterator *hi);
void hashTypeCurrentFromListpack(hashTypeIterator *hi, int what,
unsigned char **vstr,
unsigned int *vlen,
long long *vll);
sds hashTypeCurrentFromHashTable(hashTypeIterator *hi, int what);
void hashTypeCurrentObject(hashTypeIterator *hi, int what, unsigned char **vstr, unsigned int *vlen, long long *vll);
sds hashTypeCurrentObjectNewSds(hashTypeIterator *hi, int what);
robj *hashTypeLookupWriteOrCreate(client *c, robj *key);
robj *hashTypeGetValueObject(robj *o, sds field);
int hashTypeSet(robj *o, sds field, sds value, int flags);
robj *hashTypeDup(robj *o);


int pubsubUnsubscribeAllChannels(client *c, int notify);
int pubsubUnsubscribeShardAllChannels(client *c, int notify);
void pubsubUnsubscribeShardChannels(robj **channels, unsigned int count);
int pubsubUnsubscribeAllPatterns(client *c, int notify);
int pubsubPublishMessage(robj *channel, robj *message);
int pubsubPublishMessageShard(robj *channel, robj *message);
void addReplyPubsubMessage(client *c, robj *channel, robj *msg);
int serverPubsubSubscriptionCount();
int serverPubsubShardSubscriptionCount();


void notifyKeyspaceEvent(int type, char *event, robj *key, int dbid);
int keyspaceEventsStringToFlags(char *classes);
sds keyspaceEventsFlagsToString(int flags);


void loadServerConfig(char *filename, char config_from_stdin, char *options);
void appendServerSaveParams(time_t seconds, int changes);
void resetServerSaveParams(void);
struct rewriteConfigState;
void rewriteConfigRewriteLine(struct rewriteConfigState *state, const char *option, sds line, int force);
void rewriteConfigMarkAsProcessed(struct rewriteConfigState *state, const char *option);
int rewriteConfig(char *path, int force_all);
void initConfigValues();
sds getConfigDebugInfo();
int allowProtectedAction(int config, client *c);


int removeExpire(redisDb *db, robj *key);
void deleteExpiredKeyAndPropagate(redisDb *db, robj *keyobj);
void propagateDeletion(redisDb *db, robj *key, int lazy);
int keyIsExpired(redisDb *db, robj *key);
long long getExpire(redisDb *db, robj *key);
void setExpire(client *c, redisDb *db, robj *key, long long when);
int checkAlreadyExpired(long long when);
robj *lookupKeyRead(redisDb *db, robj *key);
robj *lookupKeyWrite(redisDb *db, robj *key);
robj *lookupKeyReadOrReply(client *c, robj *key, robj *reply);
robj *lookupKeyWriteOrReply(client *c, robj *key, robj *reply);
robj *lookupKeyReadWithFlags(redisDb *db, robj *key, int flags);
robj *lookupKeyWriteWithFlags(redisDb *db, robj *key, int flags);
robj *objectCommandLookup(client *c, robj *key);
robj *objectCommandLookupOrReply(client *c, robj *key, robj *reply);
int objectSetLRUOrLFU(robj *val, long long lfu_freq, long long lru_idle,
long long lru_clock, int lru_multiplier);
#define LOOKUP_NONE 0
#define LOOKUP_NOTOUCH (1<<0)
#define LOOKUP_NONOTIFY (1<<1)
#define LOOKUP_NOSTATS (1<<2)
#define LOOKUP_WRITE (1<<3)

void dbAdd(redisDb *db, robj *key, robj *val);
int dbAddRDBLoad(redisDb *db, sds key, robj *val);
void dbOverwrite(redisDb *db, robj *key, robj *val);

#define SETKEY_KEEPTTL 1
#define SETKEY_NO_SIGNAL 2
#define SETKEY_ALREADY_EXIST 4
#define SETKEY_DOESNT_EXIST 8
void setKey(client *c, redisDb *db, robj *key, robj *val, int flags);
robj *dbRandomKey(redisDb *db);
int dbSyncDelete(redisDb *db, robj *key);
int dbDelete(redisDb *db, robj *key);
robj *dbUnshareStringValue(redisDb *db, robj *key, robj *o);

#define EMPTYDB_NO_FLAGS 0
#define EMPTYDB_ASYNC (1<<0)
#define EMPTYDB_NOFUNCTIONS (1<<1)
long long emptyData(int dbnum, int flags, void(callback)(dict*));
long long emptyDbStructure(redisDb *dbarray, int dbnum, int async, void(callback)(dict*));
void flushAllDataAndResetRDB(int flags);
long long dbTotalServerKeyCount();
redisDb *initTempDb(void);
void discardTempDb(redisDb *tempDb, void(callback)(dict*));


int selectDb(client *c, int id);
void signalModifiedKey(client *c, redisDb *db, robj *key);
void signalFlushedDb(int dbid, int async);
void scanGenericCommand(client *c, robj *o, unsigned long cursor);
int parseScanCursorOrReply(client *c, robj *o, unsigned long *cursor);
int dbAsyncDelete(redisDb *db, robj *key);
void emptyDbAsync(redisDb *db);
size_t lazyfreeGetPendingObjectsCount(void);
size_t lazyfreeGetFreedObjectsCount(void);
void lazyfreeResetStats(void);
void freeObjAsync(robj *key, robj *obj, int dbid);
void freeReplicationBacklogRefMemAsync(list *blocks, rax *index);


#define GET_KEYSPEC_DEFAULT 0
#define GET_KEYSPEC_INCLUDE_CHANNELS (1<<0)
#define GET_KEYSPEC_RETURN_PARTIAL (1<<1)

int getKeysFromCommandWithSpecs(struct redisCommand *cmd, robj **argv, int argc, int search_flags, getKeysResult *result);
keyReference *getKeysPrepareResult(getKeysResult *result, int numkeys);
int getKeysFromCommand(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int doesCommandHaveKeys(struct redisCommand *cmd);
void getKeysFreeResult(getKeysResult *result);
int sintercardGetKeys(struct redisCommand *cmd,robj **argv, int argc, getKeysResult *result);
int zunionInterDiffGetKeys(struct redisCommand *cmd,robj **argv, int argc, getKeysResult *result);
int zunionInterDiffStoreGetKeys(struct redisCommand *cmd,robj **argv, int argc, getKeysResult *result);
int evalGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int functionGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int sortGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int sortROGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int migrateGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int georadiusGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int xreadGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int lmpopGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int blmpopGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int zmpopGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);
int bzmpopGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result);

unsigned short crc16(const char *buf, int len);


void initSentinelConfig(void);
void initSentinel(void);
void sentinelTimer(void);
const char *sentinelHandleConfiguration(char **argv, int argc);
void queueSentinelConfig(sds *argv, int argc, int linenum, sds line);
void loadSentinelConfigFromQueue(void);
void sentinelIsRunning(void);
void sentinelCheckConfigFile(void);
void sentinelCommand(client *c);
void sentinelInfoCommand(client *c);
void sentinelPublishCommand(client *c);
void sentinelRoleCommand(client *c);


int redis_check_rdb(char *rdbfilename, FILE *fp);
int redis_check_rdb_main(int argc, char **argv, FILE *fp);
int redis_check_aof_main(int argc, char **argv);


void scriptingInit(int setup);
int ldbRemoveChild(pid_t pid);
void ldbKillForkedSessions(void);
int ldbPendingChildren(void);
sds luaCreateFunction(client *c, robj *body);
void luaLdbLineHook(lua_State *lua, lua_Debug *ar);
void freeLuaScriptsAsync(dict *lua_scripts);
void freeFunctionsAsync(functionsLibCtx *lib_ctx);
int ldbIsEnabled();
void ldbLog(sds entry);
void ldbLogRedisReply(char *reply);
void sha1hex(char *digest, char *script, size_t len);
unsigned long evalMemory();
dict* evalScriptsDict();
unsigned long evalScriptsMemory();


void processUnblockedClients(void);
void blockClient(client *c, int btype);
void unblockClient(client *c);
void queueClientForReprocessing(client *c);
void replyToBlockedClientTimedOut(client *c);
int getTimeoutFromObjectOrReply(client *c, robj *object, mstime_t *timeout, int unit);
void disconnectAllBlockedClients(void);
void handleClientsBlockedOnKeys(void);
void signalKeyAsReady(redisDb *db, robj *key, int type);
void blockForKeys(client *c, int btype, robj **keys, int numkeys, long count, mstime_t timeout, robj *target, struct blockPos *blockpos, streamID *ids);
void updateStatsOnUnblock(client *c, long blocked_us, long reply_us);


void addClientToTimeoutTable(client *c);
void removeClientFromTimeoutTable(client *c);
void handleBlockedClientsTimeout(void);
int clientsCronHandleTimeout(client *c, mstime_t now_ms);


void activeExpireCycle(int type);
void expireSlaveKeys(void);
void rememberSlaveKeyWithExpire(redisDb *db, robj *key);
void flushSlaveKeysWithExpireList(void);
size_t getSlaveKeyWithExpireCount(void);


void evictionPoolAlloc(void);
#define LFU_INIT_VAL 5
unsigned long LFUGetTimeInMinutes(void);
uint8_t LFULogIncr(uint8_t value);
unsigned long LFUDecrAndReturn(robj *o);
#define EVICT_OK 0
#define EVICT_RUNNING 1
#define EVICT_FAIL 2
int performEvictions(void);
void startEvictionTimeProc(void);


uint64_t dictSdsHash(const void *key);
uint64_t dictSdsCaseHash(const void *key);
int dictSdsKeyCompare(dict *d, const void *key1, const void *key2);
int dictSdsKeyCaseCompare(dict *d, const void *key1, const void *key2);
void dictSdsDestructor(dict *d, void *val);
void *dictSdsDup(dict *d, const void *key);


char *redisGitSHA1(void);
char *redisGitDirty(void);
uint64_t redisBuildId(void);
char *redisBuildIdString(void);


void authCommand(client *c);
void pingCommand(client *c);
void echoCommand(client *c);
void commandCommand(client *c);
void commandCountCommand(client *c);
void commandListCommand(client *c);
void commandInfoCommand(client *c);
void commandGetKeysCommand(client *c);
void commandHelpCommand(client *c);
void commandDocsCommand(client *c);
void setCommand(client *c);
void setnxCommand(client *c);
void setexCommand(client *c);
void psetexCommand(client *c);
void getCommand(client *c);
void getexCommand(client *c);
void getdelCommand(client *c);
void delCommand(client *c);
void unlinkCommand(client *c);
void existsCommand(client *c);
void setbitCommand(client *c);
void getbitCommand(client *c);
void bitfieldCommand(client *c);
void bitfieldroCommand(client *c);
void setrangeCommand(client *c);
void getrangeCommand(client *c);
void incrCommand(client *c);
void decrCommand(client *c);
void incrbyCommand(client *c);
void decrbyCommand(client *c);
void incrbyfloatCommand(client *c);
void selectCommand(client *c);
void swapdbCommand(client *c);
void randomkeyCommand(client *c);
void keysCommand(client *c);
void scanCommand(client *c);
void dbsizeCommand(client *c);
void lastsaveCommand(client *c);
void saveCommand(client *c);
void bgsaveCommand(client *c);
void bgrewriteaofCommand(client *c);
void shutdownCommand(client *c);
void slowlogCommand(client *c);
void moveCommand(client *c);
void copyCommand(client *c);
void renameCommand(client *c);
void renamenxCommand(client *c);
void lpushCommand(client *c);
void rpushCommand(client *c);
void lpushxCommand(client *c);
void rpushxCommand(client *c);
void linsertCommand(client *c);
void lpopCommand(client *c);
void rpopCommand(client *c);
void lmpopCommand(client *c);
void llenCommand(client *c);
void lindexCommand(client *c);
void lrangeCommand(client *c);
void ltrimCommand(client *c);
void typeCommand(client *c);
void lsetCommand(client *c);
void saddCommand(client *c);
void sremCommand(client *c);
void smoveCommand(client *c);
void sismemberCommand(client *c);
void smismemberCommand(client *c);
void scardCommand(client *c);
void spopCommand(client *c);
void srandmemberCommand(client *c);
void sinterCommand(client *c);
void sinterCardCommand(client *c);
void sinterstoreCommand(client *c);
void sunionCommand(client *c);
void sunionstoreCommand(client *c);
void sdiffCommand(client *c);
void sdiffstoreCommand(client *c);
void sscanCommand(client *c);
void syncCommand(client *c);
void flushdbCommand(client *c);
void flushallCommand(client *c);
void sortCommand(client *c);
void sortroCommand(client *c);
void lremCommand(client *c);
void lposCommand(client *c);
void rpoplpushCommand(client *c);
void lmoveCommand(client *c);
void infoCommand(client *c);
void mgetCommand(client *c);
void monitorCommand(client *c);
void expireCommand(client *c);
void expireatCommand(client *c);
void pexpireCommand(client *c);
void pexpireatCommand(client *c);
void getsetCommand(client *c);
void ttlCommand(client *c);
void touchCommand(client *c);
void pttlCommand(client *c);
void expiretimeCommand(client *c);
void pexpiretimeCommand(client *c);
void persistCommand(client *c);
void replicaofCommand(client *c);
void roleCommand(client *c);
void debugCommand(client *c);
void msetCommand(client *c);
void msetnxCommand(client *c);
void zaddCommand(client *c);
void zincrbyCommand(client *c);
void zrangeCommand(client *c);
void zrangebyscoreCommand(client *c);
void zrevrangebyscoreCommand(client *c);
void zrangebylexCommand(client *c);
void zrevrangebylexCommand(client *c);
void zcountCommand(client *c);
void zlexcountCommand(client *c);
void zrevrangeCommand(client *c);
void zcardCommand(client *c);
void zremCommand(client *c);
void zscoreCommand(client *c);
void zmscoreCommand(client *c);
void zremrangebyscoreCommand(client *c);
void zremrangebylexCommand(client *c);
void zpopminCommand(client *c);
void zpopmaxCommand(client *c);
void zmpopCommand(client *c);
void bzpopminCommand(client *c);
void bzpopmaxCommand(client *c);
void bzmpopCommand(client *c);
void zrandmemberCommand(client *c);
void multiCommand(client *c);
void execCommand(client *c);
void discardCommand(client *c);
void blpopCommand(client *c);
void brpopCommand(client *c);
void blmpopCommand(client *c);
void brpoplpushCommand(client *c);
void blmoveCommand(client *c);
void appendCommand(client *c);
void strlenCommand(client *c);
void zrankCommand(client *c);
void zrevrankCommand(client *c);
void hsetCommand(client *c);
void hsetnxCommand(client *c);
void hgetCommand(client *c);
void hmgetCommand(client *c);
void hdelCommand(client *c);
void hlenCommand(client *c);
void hstrlenCommand(client *c);
void zremrangebyrankCommand(client *c);
void zunionstoreCommand(client *c);
void zinterstoreCommand(client *c);
void zdiffstoreCommand(client *c);
void zunionCommand(client *c);
void zinterCommand(client *c);
void zinterCardCommand(client *c);
void zrangestoreCommand(client *c);
void zdiffCommand(client *c);
void zscanCommand(client *c);
void hkeysCommand(client *c);
void hvalsCommand(client *c);
void hgetallCommand(client *c);
void hexistsCommand(client *c);
void hscanCommand(client *c);
void hrandfieldCommand(client *c);
void configSetCommand(client *c);
void configGetCommand(client *c);
void configResetStatCommand(client *c);
void configRewriteCommand(client *c);
void configHelpCommand(client *c);
void hincrbyCommand(client *c);
void hincrbyfloatCommand(client *c);
void subscribeCommand(client *c);
void unsubscribeCommand(client *c);
void psubscribeCommand(client *c);
void punsubscribeCommand(client *c);
void publishCommand(client *c);
void pubsubCommand(client *c);
void spublishCommand(client *c);
void ssubscribeCommand(client *c);
void sunsubscribeCommand(client *c);
void watchCommand(client *c);
void unwatchCommand(client *c);
void clusterCommand(client *c);
void restoreCommand(client *c);
void migrateCommand(client *c);
void askingCommand(client *c);
void readonlyCommand(client *c);
void readwriteCommand(client *c);
int verifyDumpPayload(unsigned char *p, size_t len, uint16_t *rdbver_ptr);
void dumpCommand(client *c);
void objectCommand(client *c);
void memoryCommand(client *c);
void clientCommand(client *c);
void helloCommand(client *c);
void evalCommand(client *c);
void evalRoCommand(client *c);
void evalShaCommand(client *c);
void evalShaRoCommand(client *c);
void scriptCommand(client *c);
void fcallCommand(client *c);
void fcallroCommand(client *c);
void functionLoadCommand(client *c);
void functionDeleteCommand(client *c);
void functionKillCommand(client *c);
void functionStatsCommand(client *c);
void functionListCommand(client *c);
void functionHelpCommand(client *c);
void functionFlushCommand(client *c);
void functionRestoreCommand(client *c);
void functionDumpCommand(client *c);
void timeCommand(client *c);
void bitopCommand(client *c);
void bitcountCommand(client *c);
void bitposCommand(client *c);
void replconfCommand(client *c);
void waitCommand(client *c);
void georadiusbymemberCommand(client *c);
void georadiusbymemberroCommand(client *c);
void georadiusCommand(client *c);
void georadiusroCommand(client *c);
void geoaddCommand(client *c);
void geohashCommand(client *c);
void geoposCommand(client *c);
void geodistCommand(client *c);
void geosearchCommand(client *c);
void geosearchstoreCommand(client *c);
void pfselftestCommand(client *c);
void pfaddCommand(client *c);
void pfcountCommand(client *c);
void pfmergeCommand(client *c);
void pfdebugCommand(client *c);
void latencyCommand(client *c);
void moduleCommand(client *c);
void securityWarningCommand(client *c);
void xaddCommand(client *c);
void xrangeCommand(client *c);
void xrevrangeCommand(client *c);
void xlenCommand(client *c);
void xreadCommand(client *c);
void xgroupCommand(client *c);
void xsetidCommand(client *c);
void xackCommand(client *c);
void xpendingCommand(client *c);
void xclaimCommand(client *c);
void xautoclaimCommand(client *c);
void xinfoCommand(client *c);
void xdelCommand(client *c);
void xtrimCommand(client *c);
void lolwutCommand(client *c);
void aclCommand(client *c);
void lcsCommand(client *c);
void quitCommand(client *c);
void resetCommand(client *c);
void failoverCommand(client *c);

#if defined(__GNUC__)
void *calloc(size_t count, size_t size) __attribute__ ((deprecated));
void free(void *ptr) __attribute__ ((deprecated));
void *malloc(size_t size) __attribute__ ((deprecated));
void *realloc(void *ptr, size_t size) __attribute__ ((deprecated));
#endif


void _serverAssertWithInfo(const client *c, const robj *o, const char *estr, const char *file, int line);
void _serverAssert(const char *estr, const char *file, int line);
#if defined(__GNUC__)
void _serverPanic(const char *file, int line, const char *msg, ...)
__attribute__ ((format (printf, 3, 4)));
#else
void _serverPanic(const char *file, int line, const char *msg, ...);
#endif
void serverLogObjectDebugInfo(const robj *o);
void sigsegvHandler(int sig, siginfo_t *info, void *secret);
const char *getSafeInfoString(const char *s, size_t len, char **tmp);
sds genRedisInfoString(const char *section);
sds genModulesInfoString(sds info);
void applyWatchdogPeriod();
void watchdogScheduleSignal(int period);
void serverLogHexDump(int level, char *descr, void *value, size_t len);
int memtest_preserving_test(unsigned long *m, size_t bytes, int passes);
void mixDigest(unsigned char *digest, const void *ptr, size_t len);
void xorDigest(unsigned char *digest, const void *ptr, size_t len);
sds catSubCommandFullname(const char *parent_name, const char *sub_name);
void commandAddSubcommand(struct redisCommand *parent, struct redisCommand *subcommand, const char *declared_name);
void populateCommandMovableKeys(struct redisCommand *cmd);
void debugDelay(int usec);
void killIOThreads(void);
void killThreads(void);
void makeThreadKillable(void);
void swapMainDbWithTempDb(redisDb *tempDb);



#define serverLog(level, ...) do {if (((level)&0xff) < server.verbosity) break;_serverLog(level, __VA_ARGS__);} while(0)





void tlsInit(void);
void tlsCleanup(void);
int tlsConfigure(redisTLSContextConfig *ctx_config);
int isTlsConfigured(void);

#define redisDebug(fmt, ...) printf("DEBUG %s:%d > " fmt "\n", __FILE__, __LINE__, __VA_ARGS__)

#define redisDebugMark() printf("-- MARK %s:%d --\n", __FILE__, __LINE__)


int iAmMaster(void);

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#endif
