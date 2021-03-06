




















































#include "server.h"
#include "cluster.h"
#include "slowlog.h"
#include "rdb.h"
#include "monotonic.h"
#include "call_reply.h"
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>







typedef struct RedisModuleInfoCtx {
struct RedisModule *module;
const char *requested_section;
sds info;
int sections;
int in_section;
int in_dict_field;
} RedisModuleInfoCtx;





struct RedisModuleSharedAPI {
void *func;
RedisModule *module;
};
typedef struct RedisModuleSharedAPI RedisModuleSharedAPI;

dict *modules;



struct AutoMemEntry {
void *ptr;
int type;
};


#define REDISMODULE_AM_KEY 0
#define REDISMODULE_AM_STRING 1
#define REDISMODULE_AM_REPLY 2
#define REDISMODULE_AM_FREED 3
#define REDISMODULE_AM_DICT 4
#define REDISMODULE_AM_INFO 5














#define REDISMODULE_POOL_ALLOC_MIN_SIZE (1024*8)
#define REDISMODULE_POOL_ALLOC_ALIGN (sizeof(void*))

typedef struct RedisModulePoolAllocBlock {
uint32_t size;
uint32_t used;
struct RedisModulePoolAllocBlock *next;
char memory[];
} RedisModulePoolAllocBlock;









struct RedisModuleBlockedClient;

struct RedisModuleCtx {
void *getapifuncptr;
struct RedisModule *module;
client *client;
struct RedisModuleBlockedClient *blocked_client;

struct AutoMemEntry *amqueue;
int amqueue_len;
int amqueue_used;
int flags;
void **postponed_arrays;
int postponed_arrays_count;
void *blocked_privdata;
RedisModuleString *blocked_ready_key;




getKeysResult *keys_result;

struct RedisModulePoolAllocBlock *pa_head;
long long next_yield_time;
};
typedef struct RedisModuleCtx RedisModuleCtx;

#define REDISMODULE_CTX_NONE (0)
#define REDISMODULE_CTX_AUTO_MEMORY (1<<0)
#define REDISMODULE_CTX_KEYS_POS_REQUEST (1<<1)
#define REDISMODULE_CTX_BLOCKED_REPLY (1<<2)
#define REDISMODULE_CTX_BLOCKED_TIMEOUT (1<<3)
#define REDISMODULE_CTX_THREAD_SAFE (1<<4)
#define REDISMODULE_CTX_BLOCKED_DISCONNECTED (1<<5)
#define REDISMODULE_CTX_TEMP_CLIENT (1<<6)

#define REDISMODULE_CTX_NEW_CLIENT (1<<7)



struct RedisModuleKey {
RedisModuleCtx *ctx;
redisDb *db;
robj *key;
robj *value;
void *iter;
int mode;

union {
struct {

listTypeEntry entry;
long index;
} list;
struct {

uint32_t type;
zrangespec rs;
zlexrangespec lrs;
uint32_t start;
uint32_t end;
void *current;
int er;

} zset;
struct {

streamID currentid;
int64_t numfieldsleft;
int signalready;
} stream;
} u;
};
typedef struct RedisModuleKey RedisModuleKey;


#define REDISMODULE_ZSET_RANGE_NONE 0
#define REDISMODULE_ZSET_RANGE_LEX 1
#define REDISMODULE_ZSET_RANGE_SCORE 2
#define REDISMODULE_ZSET_RANGE_POS 3



struct RedisModuleBlockedClient;
typedef int (*RedisModuleCmdFunc) (RedisModuleCtx *ctx, void **argv, int argc);
typedef void (*RedisModuleDisconnectFunc) (RedisModuleCtx *ctx, struct RedisModuleBlockedClient *bc);


struct RedisModuleCommand {
struct RedisModule *module;
RedisModuleCmdFunc func;
struct redisCommand *rediscmd;
};
typedef struct RedisModuleCommand RedisModuleCommand;

#define REDISMODULE_REPLYFLAG_NONE 0
#define REDISMODULE_REPLYFLAG_TOPARSE (1<<0)
#define REDISMODULE_REPLYFLAG_NESTED (1<<1)





typedef struct CallReply RedisModuleCallReply;



typedef struct RedisModuleBlockedClient {
client *client;

RedisModule *module;
RedisModuleCmdFunc reply_callback;
RedisModuleCmdFunc timeout_callback;
RedisModuleDisconnectFunc disconnect_callback;
void (*free_privdata)(RedisModuleCtx*,void*);
void *privdata;


client *thread_safe_ctx_client;

client *reply_client;

int dbid;
int blocked_on_keys;
int unblocked;
monotime background_timer;
uint64_t background_duration;

} RedisModuleBlockedClient;

static pthread_mutex_t moduleUnblockedClientsMutex = PTHREAD_MUTEX_INITIALIZER;
static list *moduleUnblockedClients;





static client **moduleTempClients;
static size_t moduleTempClientCap = 0;
static size_t moduleTempClientCount = 0;
static size_t moduleTempClientMinCount = 0;




static pthread_mutex_t moduleGIL = PTHREAD_MUTEX_INITIALIZER;



typedef int (*RedisModuleNotificationFunc) (RedisModuleCtx *ctx, int type, const char *event, RedisModuleString *key);



typedef struct RedisModuleKeyspaceSubscriber {

RedisModule *module;

RedisModuleNotificationFunc notify_callback;

int event_mask;


int active;
} RedisModuleKeyspaceSubscriber;


static list *moduleKeyspaceSubscribers;


typedef struct RedisModuleDict {
rax *rax;
} RedisModuleDict;

typedef struct RedisModuleDictIter {
RedisModuleDict *dict;
raxIterator ri;
} RedisModuleDictIter;

typedef struct RedisModuleCommandFilterCtx {
RedisModuleString **argv;
int argc;
} RedisModuleCommandFilterCtx;

typedef void (*RedisModuleCommandFilterFunc) (RedisModuleCommandFilterCtx *filter);

typedef struct RedisModuleCommandFilter {

RedisModule *module;

RedisModuleCommandFilterFunc callback;

int flags;
} RedisModuleCommandFilter;


static list *moduleCommandFilters;

typedef void (*RedisModuleForkDoneHandler) (int exitcode, int bysignal, void *user_data);

static struct RedisModuleForkInfo {
RedisModuleForkDoneHandler done_handler;
void* done_handler_user_data;
} moduleForkInfo = {0};

typedef struct RedisModuleServerInfoData {
rax *rax;
} RedisModuleServerInfoData;


#define REDISMODULE_ARGV_REPLICATE (1<<0)
#define REDISMODULE_ARGV_NO_AOF (1<<1)
#define REDISMODULE_ARGV_NO_REPLICAS (1<<2)
#define REDISMODULE_ARGV_RESP_3 (1<<3)
#define REDISMODULE_ARGV_RESP_AUTO (1<<4)
#define REDISMODULE_ARGV_CHECK_ACL (1<<5)





#define SHOULD_SIGNAL_MODIFIED_KEYS(ctx) ctx->module? !(ctx->module->options & REDISMODULE_OPTION_NO_IMPLICIT_SIGNAL_MODIFIED) : 1







typedef struct RedisModuleEventListener {
RedisModule *module;
RedisModuleEvent event;
RedisModuleEventCallback callback;
} RedisModuleEventListener;

list *RedisModule_EventListeners;






typedef struct RedisModuleUser {
user *user;
int free_user;
} RedisModuleUser;


typedef struct RedisModuleKeyOptCtx {
struct redisObject *from_key, *to_key;


int from_dbid, to_dbid;


} RedisModuleKeyOptCtx;




void RM_FreeCallReply(RedisModuleCallReply *reply);
void RM_CloseKey(RedisModuleKey *key);
void autoMemoryCollect(RedisModuleCtx *ctx);
robj **moduleCreateArgvFromUserFormat(const char *cmdname, const char *fmt, int *argcp, int *argvlenp, int *flags, va_list ap);
void RM_ZsetRangeStop(RedisModuleKey *kp);
static void zsetKeyReset(RedisModuleKey *key);
static void moduleInitKeyTypeSpecific(RedisModuleKey *key);
void RM_FreeDict(RedisModuleCtx *ctx, RedisModuleDict *d);
void RM_FreeServerInfo(RedisModuleCtx *ctx, RedisModuleServerInfoData *data);












void *RM_Alloc(size_t bytes) {
return zmalloc(bytes);
}





void *RM_Calloc(size_t nmemb, size_t size) {
return zcalloc(nmemb*size);
}


void* RM_Realloc(void *ptr, size_t bytes) {
return zrealloc(ptr,bytes);
}




void RM_Free(void *ptr) {
zfree(ptr);
}


char *RM_Strdup(const char *str) {
return zstrdup(str);
}






void poolAllocRelease(RedisModuleCtx *ctx) {
RedisModulePoolAllocBlock *head = ctx->pa_head, *next;

while(head != NULL) {
next = head->next;
zfree(head);
head = next;
}
ctx->pa_head = NULL;
}













void *RM_PoolAlloc(RedisModuleCtx *ctx, size_t bytes) {
if (bytes == 0) return NULL;
RedisModulePoolAllocBlock *b = ctx->pa_head;
size_t left = b ? b->size - b->used : 0;


if (left >= bytes) {
size_t alignment = REDISMODULE_POOL_ALLOC_ALIGN;
while (bytes < alignment && alignment/2 >= bytes) alignment /= 2;
if (b->used % alignment)
b->used += alignment - (b->used % alignment);
left = (b->used > b->size) ? 0 : b->size - b->used;
}


if (left < bytes) {
size_t blocksize = REDISMODULE_POOL_ALLOC_MIN_SIZE;
if (blocksize < bytes) blocksize = bytes;
b = zmalloc(sizeof(*b) + blocksize);
b->size = blocksize;
b->used = 0;
b->next = ctx->pa_head;
ctx->pa_head = b;
}

char *retval = b->memory + b->used;
b->used += bytes;
return retval;
}





client *moduleAllocTempClient() {
client *c = NULL;

if (moduleTempClientCount > 0) {
c = moduleTempClients[--moduleTempClientCount];
if (moduleTempClientCount < moduleTempClientMinCount)
moduleTempClientMinCount = moduleTempClientCount;
} else {
c = createClient(NULL);
c->flags |= CLIENT_MODULE;
c->user = NULL;
}
return c;
}

void moduleReleaseTempClient(client *c) {
if (moduleTempClientCount == moduleTempClientCap) {
moduleTempClientCap = moduleTempClientCap ? moduleTempClientCap*2 : 32;
moduleTempClients = zrealloc(moduleTempClients, sizeof(c)*moduleTempClientCap);
}
clearClientConnectionState(c);
listEmpty(c->reply);
c->reply_bytes = 0;
resetClient(c);
c->bufpos = 0;
c->flags = CLIENT_MODULE;
c->user = NULL;
moduleTempClients[moduleTempClientCount++] = c;
}













int moduleCreateEmptyKey(RedisModuleKey *key, int type) {
robj *obj;


if (!(key->mode & REDISMODULE_WRITE) || key->value)
return REDISMODULE_ERR;

switch(type) {
case REDISMODULE_KEYTYPE_LIST:
obj = createQuicklistObject();
quicklistSetOptions(obj->ptr, server.list_max_listpack_size,
server.list_compress_depth);
break;
case REDISMODULE_KEYTYPE_ZSET:
obj = createZsetListpackObject();
break;
case REDISMODULE_KEYTYPE_HASH:
obj = createHashObject();
break;
case REDISMODULE_KEYTYPE_STREAM:
obj = createStreamObject();
break;
default: return REDISMODULE_ERR;
}
dbAdd(key->db,key->key,obj);
key->value = obj;
moduleInitKeyTypeSpecific(key);
return REDISMODULE_OK;
}


static void moduleFreeKeyIterator(RedisModuleKey *key) {
serverAssert(key->iter != NULL);
switch (key->value->type) {
case OBJ_LIST: listTypeReleaseIterator(key->iter); break;
case OBJ_STREAM: zfree(key->iter); break;
default: serverAssert(0);
}
key->iter = NULL;
}











int moduleDelKeyIfEmpty(RedisModuleKey *key) {
if (!(key->mode & REDISMODULE_WRITE) || key->value == NULL) return 0;
int isempty;
robj *o = key->value;

switch(o->type) {
case OBJ_LIST: isempty = listTypeLength(o) == 0; break;
case OBJ_SET: isempty = setTypeSize(o) == 0; break;
case OBJ_ZSET: isempty = zsetLength(o) == 0; break;
case OBJ_HASH: isempty = hashTypeLength(o) == 0; break;
case OBJ_STREAM: isempty = streamLength(o) == 0; break;
default: isempty = 0;
}

if (isempty) {
if (key->iter) moduleFreeKeyIterator(key);
dbDelete(key->db,key->key);
key->value = NULL;
return 1;
} else {
return 0;
}
}











int RM_GetApi(const char *funcname, void **targetPtrPtr) {






dictEntry *he = dictFind(server.moduleapi, funcname);
if (!he) return REDISMODULE_ERR;
*targetPtrPtr = dictGetVal(he);
return REDISMODULE_OK;
}


void moduleFreeContext(RedisModuleCtx *ctx) {
if (!(ctx->flags & REDISMODULE_CTX_THREAD_SAFE)) {


if (--server.module_ctx_nesting == 0) {
if (!server.core_propagates)
propagatePendingCommands();
if (server.busy_module_yield_flags) {
blockingOperationEnds();
server.busy_module_yield_flags = BUSY_MODULE_YIELD_NONE;
unblockPostponedClients();
}
}
}
autoMemoryCollect(ctx);
poolAllocRelease(ctx);
if (ctx->postponed_arrays) {
zfree(ctx->postponed_arrays);
ctx->postponed_arrays_count = 0;
serverLog(LL_WARNING,
"API misuse detected in module %s: "
"RedisModule_ReplyWith*(REDISMODULE_POSTPONED_LEN) "
"not matched by the same number of RedisModule_SetReply*Len() "
"calls.",
ctx->module->name);
}




if (ctx->flags & REDISMODULE_CTX_TEMP_CLIENT)
moduleReleaseTempClient(ctx->client);
else if (ctx->flags & REDISMODULE_CTX_NEW_CLIENT)
freeClient(ctx->client);
}







void moduleCreateContext(RedisModuleCtx *out_ctx, RedisModule *module, int ctx_flags) {
memset(out_ctx, 0 ,sizeof(RedisModuleCtx));
out_ctx->getapifuncptr = (void*)(unsigned long)&RM_GetApi;
out_ctx->module = module;
out_ctx->flags = ctx_flags;
if (ctx_flags & REDISMODULE_CTX_TEMP_CLIENT)
out_ctx->client = moduleAllocTempClient();
else if (ctx_flags & REDISMODULE_CTX_NEW_CLIENT)
out_ctx->client = createClient(NULL);








if (server.loading)
out_ctx->next_yield_time = getMonotonicUs() + 1000000 / server.hz;
else
out_ctx->next_yield_time = getMonotonicUs() + server.busy_reply_threshold * 1000;

if (!(ctx_flags & REDISMODULE_CTX_THREAD_SAFE)) {
server.module_ctx_nesting++;
}
}



void RedisModuleCommandDispatcher(client *c) {
RedisModuleCommand *cp = (void*)(unsigned long)c->cmd->getkeys_proc;
RedisModuleCtx ctx;
moduleCreateContext(&ctx, cp->module, REDISMODULE_CTX_NONE);

ctx.client = c;
cp->func(&ctx,(void**)c->argv,c->argc);
moduleFreeContext(&ctx);










for (int i = 0; i < c->argc; i++) {


if (c->argv[i]->refcount > 1)
trimStringObjectIfNeeded(c->argv[i]);
}
}










int moduleGetCommandKeysViaAPI(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
RedisModuleCommand *cp = (void*)(unsigned long)cmd->getkeys_proc;
RedisModuleCtx ctx;
moduleCreateContext(&ctx, cp->module, REDISMODULE_CTX_KEYS_POS_REQUEST);


getKeysPrepareResult(result, MAX_KEYS_BUFFER);
ctx.keys_result = result;

cp->func(&ctx,(void**)argv,argc);



moduleFreeContext(&ctx);
return result->numkeys;
}












int RM_IsKeysPositionRequest(RedisModuleCtx *ctx) {
return (ctx->flags & REDISMODULE_CTX_KEYS_POS_REQUEST) != 0;
}















void RM_KeyAtPos(RedisModuleCtx *ctx, int pos) {
if (!(ctx->flags & REDISMODULE_CTX_KEYS_POS_REQUEST) || !ctx->keys_result) return;
if (pos <= 0) return;

getKeysResult *res = ctx->keys_result;


if (res->numkeys == res->size) {
int newsize = res->size + (res->size > 8192 ? 8192 : res->size);
getKeysPrepareResult(res, newsize);
}

res->keys[res->numkeys++].pos = pos;
}





int64_t commandFlagsFromString(char *s) {
int count, j;
int64_t flags = 0;
sds *tokens = sdssplitlen(s,strlen(s)," ",1,&count);
for (j = 0; j < count; j++) {
char *t = tokens[j];
if (!strcasecmp(t,"write")) flags |= CMD_WRITE;
else if (!strcasecmp(t,"readonly")) flags |= CMD_READONLY;
else if (!strcasecmp(t,"admin")) flags |= CMD_ADMIN;
else if (!strcasecmp(t,"deny-oom")) flags |= CMD_DENYOOM;
else if (!strcasecmp(t,"deny-script")) flags |= CMD_NOSCRIPT;
else if (!strcasecmp(t,"allow-loading")) flags |= CMD_LOADING;
else if (!strcasecmp(t,"pubsub")) flags |= CMD_PUBSUB;
else if (!strcasecmp(t,"random")) { }
else if (!strcasecmp(t,"blocking")) flags |= CMD_BLOCKING;
else if (!strcasecmp(t,"allow-stale")) flags |= CMD_STALE;
else if (!strcasecmp(t,"no-monitor")) flags |= CMD_SKIP_MONITOR;
else if (!strcasecmp(t,"no-slowlog")) flags |= CMD_SKIP_SLOWLOG;
else if (!strcasecmp(t,"fast")) flags |= CMD_FAST;
else if (!strcasecmp(t,"no-auth")) flags |= CMD_NO_AUTH;
else if (!strcasecmp(t,"may-replicate")) flags |= CMD_MAY_REPLICATE;
else if (!strcasecmp(t,"getkeys-api")) flags |= CMD_MODULE_GETKEYS;
else if (!strcasecmp(t,"no-cluster")) flags |= CMD_MODULE_NO_CLUSTER;
else if (!strcasecmp(t,"no-mandatory-keys")) flags |= CMD_NO_MANDATORY_KEYS;
else if (!strcasecmp(t,"allow-busy")) flags |= CMD_ALLOW_BUSY;
else break;
}
sdsfreesplitres(tokens,count);
if (j != count) return -1;
return flags;
}





int64_t commandKeySpecsFlagsFromString(const char *s) {
int count, j;
int64_t flags = 0;
sds *tokens = sdssplitlen(s,strlen(s)," ",1,&count);
for (j = 0; j < count; j++) {
char *t = tokens[j];
if (!strcasecmp(t,"RO")) flags |= CMD_KEY_RO;
else if (!strcasecmp(t,"RW")) flags |= CMD_KEY_RW;
else if (!strcasecmp(t,"OW")) flags |= CMD_KEY_OW;
else if (!strcasecmp(t,"RM")) flags |= CMD_KEY_RM;
else if (!strcasecmp(t,"access")) flags |= CMD_KEY_ACCESS;
else if (!strcasecmp(t,"insert")) flags |= CMD_KEY_INSERT;
else if (!strcasecmp(t,"update")) flags |= CMD_KEY_UPDATE;
else if (!strcasecmp(t,"delete")) flags |= CMD_KEY_DELETE;
else if (!strcasecmp(t,"channel")) flags |= CMD_KEY_CHANNEL;
else if (!strcasecmp(t,"incomplete")) flags |= CMD_KEY_INCOMPLETE;
else break;
}
sdsfreesplitres(tokens,count);
if (j != count) return -1;
return flags;
}

RedisModuleCommand *moduleCreateCommandProxy(struct RedisModule *module, sds declared_name, sds fullname, RedisModuleCmdFunc cmdfunc, int64_t flags, int firstkey, int lastkey, int keystep);


























































































int RM_CreateCommand(RedisModuleCtx *ctx, const char *name, RedisModuleCmdFunc cmdfunc, const char *strflags, int firstkey, int lastkey, int keystep) {
int64_t flags = strflags ? commandFlagsFromString((char*)strflags) : 0;
if (flags == -1) return REDISMODULE_ERR;
if ((flags & CMD_MODULE_NO_CLUSTER) && server.cluster_enabled)
return REDISMODULE_ERR;


if (lookupCommandByCString(name) != NULL)
return REDISMODULE_ERR;

sds declared_name = sdsnew(name);
RedisModuleCommand *cp = moduleCreateCommandProxy(ctx->module, declared_name, sdsdup(declared_name), cmdfunc, flags, firstkey, lastkey, keystep);
cp->rediscmd->arity = cmdfunc ? -1 : -2;

serverAssert(dictAdd(server.commands, sdsdup(declared_name), cp->rediscmd) == DICT_OK);
serverAssert(dictAdd(server.orig_commands, sdsdup(declared_name), cp->rediscmd) == DICT_OK);
cp->rediscmd->id = ACLGetCommandID(declared_name);
return REDISMODULE_OK;
}








RedisModuleCommand *moduleCreateCommandProxy(struct RedisModule *module, sds declared_name, sds fullname, RedisModuleCmdFunc cmdfunc, int64_t flags, int firstkey, int lastkey, int keystep) {
struct redisCommand *rediscmd;
RedisModuleCommand *cp;








cp = zcalloc(sizeof(*cp));
cp->module = module;
cp->func = cmdfunc;
cp->rediscmd = zcalloc(sizeof(*rediscmd));
cp->rediscmd->declared_name = declared_name;
cp->rediscmd->fullname = fullname;
cp->rediscmd->group = COMMAND_GROUP_MODULE;
cp->rediscmd->proc = RedisModuleCommandDispatcher;
cp->rediscmd->flags = flags | CMD_MODULE;
cp->rediscmd->getkeys_proc = (redisGetKeysProc*)(unsigned long)cp;
cp->rediscmd->key_specs_max = STATIC_KEY_SPECS_NUM;
cp->rediscmd->key_specs = cp->rediscmd->key_specs_static;
if (firstkey != 0) {
cp->rediscmd->key_specs_num = 1;
cp->rediscmd->key_specs[0].flags = 0;
cp->rediscmd->key_specs[0].begin_search_type = KSPEC_BS_INDEX;
cp->rediscmd->key_specs[0].bs.index.pos = firstkey;
cp->rediscmd->key_specs[0].find_keys_type = KSPEC_FK_RANGE;
cp->rediscmd->key_specs[0].fk.range.lastkey = lastkey < 0 ? lastkey : (lastkey-firstkey);
cp->rediscmd->key_specs[0].fk.range.keystep = keystep;
cp->rediscmd->key_specs[0].fk.range.limit = 0;


cp->rediscmd->legacy_range_key_spec = cp->rediscmd->key_specs[0];
} else {
cp->rediscmd->key_specs_num = 0;
cp->rediscmd->legacy_range_key_spec.begin_search_type = KSPEC_BS_INVALID;
cp->rediscmd->legacy_range_key_spec.find_keys_type = KSPEC_FK_INVALID;
}
populateCommandMovableKeys(cp->rediscmd);
cp->rediscmd->microseconds = 0;
cp->rediscmd->calls = 0;
cp->rediscmd->rejected_calls = 0;
cp->rediscmd->failed_calls = 0;
return cp;
}










RedisModuleCommand *RM_GetCommand(RedisModuleCtx *ctx, const char *name) {
struct redisCommand *cmd = lookupCommandByCString(name);

if (!cmd || !(cmd->flags & CMD_MODULE))
return NULL;

RedisModuleCommand *cp = (void*)(unsigned long)cmd->getkeys_proc;
if (cp->module != ctx->module)
return NULL;

return cp;
}



























int RM_CreateSubcommand(RedisModuleCommand *parent, const char *name, RedisModuleCmdFunc cmdfunc, const char *strflags, int firstkey, int lastkey, int keystep) {
int64_t flags = strflags ? commandFlagsFromString((char*)strflags) : 0;
if (flags == -1) return REDISMODULE_ERR;
if ((flags & CMD_MODULE_NO_CLUSTER) && server.cluster_enabled)
return REDISMODULE_ERR;

struct redisCommand *parent_cmd = parent->rediscmd;

if (parent_cmd->parent)
return REDISMODULE_ERR;

RedisModuleCommand *parent_cp = (void*)(unsigned long)parent_cmd->getkeys_proc;
if (parent_cp->func)
return REDISMODULE_ERR;


sds declared_name = sdsnew(name);
if (parent_cmd->subcommands_dict && lookupSubcommand(parent_cmd, declared_name) != NULL) {
sdsfree(declared_name);
return REDISMODULE_ERR;
}

sds fullname = catSubCommandFullname(parent_cmd->fullname, name);
RedisModuleCommand *cp = moduleCreateCommandProxy(parent->module, declared_name, fullname, cmdfunc, flags, firstkey, lastkey, keystep);
cp->rediscmd->arity = -2;

commandAddSubcommand(parent_cmd, cp->rediscmd, name);
return REDISMODULE_OK;
}


void *moduleGetHandleByName(char *modulename) {
return dictFetchValue(modules,modulename);
}


int moduleIsModuleCommand(void *module_handle, struct redisCommand *cmd) {
if (cmd->proc != RedisModuleCommandDispatcher)
return 0;
if (module_handle == NULL)
return 0;
RedisModuleCommand *cp = (void*)(unsigned long)cmd->getkeys_proc;
return (cp->module == module_handle);
}

void extendKeySpecsIfNeeded(struct redisCommand *cmd) {


if (cmd->key_specs_num < cmd->key_specs_max)
return;

cmd->key_specs_max++;

if (cmd->key_specs == cmd->key_specs_static) {
cmd->key_specs = zmalloc(sizeof(keySpec) * cmd->key_specs_max);
memcpy(cmd->key_specs, cmd->key_specs_static, sizeof(keySpec) * cmd->key_specs_num);
} else {
cmd->key_specs = zrealloc(cmd->key_specs, sizeof(keySpec) * cmd->key_specs_max);
}
}

int moduleAddCommandKeySpec(RedisModuleCommand *command, const char *specflags, int *index) {
int64_t flags = specflags ? commandKeySpecsFlagsFromString(specflags) : 0;
if (flags == -1)
return REDISMODULE_ERR;

struct redisCommand *cmd = command->rediscmd;

extendKeySpecsIfNeeded(cmd);

*index = cmd->key_specs_num;
cmd->key_specs[cmd->key_specs_num].begin_search_type = KSPEC_BS_INVALID;
cmd->key_specs[cmd->key_specs_num].find_keys_type = KSPEC_FK_INVALID;
cmd->key_specs[cmd->key_specs_num].flags = flags;
cmd->key_specs_num++;
return REDISMODULE_OK;
}

int moduleSetCommandKeySpecBeginSearch(RedisModuleCommand *command, int index, keySpec *spec) {
struct redisCommand *cmd = command->rediscmd;

if (index >= cmd->key_specs_num)
return REDISMODULE_ERR;

cmd->key_specs[index].begin_search_type = spec->begin_search_type;
cmd->key_specs[index].bs = spec->bs;

return REDISMODULE_OK;
}

int moduleSetCommandKeySpecFindKeys(RedisModuleCommand *command, int index, keySpec *spec) {
struct redisCommand *cmd = command->rediscmd;

if (index >= cmd->key_specs_num)
return REDISMODULE_ERR;

cmd->key_specs[index].find_keys_type = spec->find_keys_type;
cmd->key_specs[index].fk = spec->fk;


populateCommandLegacyRangeSpec(cmd);

populateCommandMovableKeys(cmd);

return REDISMODULE_OK;
}



























































int RM_AddCommandKeySpec(RedisModuleCommand *command, const char *specflags, int *spec_id) {
return moduleAddCommandKeySpec(command, specflags, spec_id);
}







int RM_SetCommandKeySpecBeginSearchIndex(RedisModuleCommand *command, int spec_id, int index) {
keySpec spec;
spec.begin_search_type = KSPEC_BS_INDEX;
spec.bs.index.pos = index;

return moduleSetCommandKeySpecBeginSearch(command, spec_id, &spec);
}










int RM_SetCommandKeySpecBeginSearchKeyword(RedisModuleCommand *command, int spec_id, const char *keyword, int startfrom) {
keySpec spec;
spec.begin_search_type = KSPEC_BS_KEYWORD;
spec.bs.keyword.keyword = keyword;
spec.bs.keyword.startfrom = startfrom;

return moduleSetCommandKeySpecBeginSearch(command, spec_id, &spec);
}












int RM_SetCommandKeySpecFindKeysRange(RedisModuleCommand *command, int spec_id, int lastkey, int keystep, int limit) {
keySpec spec;
spec.find_keys_type = KSPEC_FK_RANGE;
spec.fk.range.lastkey = lastkey;
spec.fk.range.keystep = keystep;
spec.fk.range.limit = limit;

return moduleSetCommandKeySpecFindKeys(command, spec_id, &spec);
}











int RM_SetCommandKeySpecFindKeysKeynum(RedisModuleCommand *command, int spec_id, int keynumidx, int firstkey, int keystep) {
keySpec spec;
spec.find_keys_type = KSPEC_FK_KEYNUM;
spec.fk.keynum.keynumidx = keynumidx;
spec.fk.keynum.firstkey = firstkey;
spec.fk.keynum.keystep = keystep;

return moduleSetCommandKeySpecFindKeys(command, spec_id, &spec);
}





void RM_SetModuleAttribs(RedisModuleCtx *ctx, const char *name, int ver, int apiver) {




RedisModule *module;

if (ctx->module != NULL) return;
module = zmalloc(sizeof(*module));
module->name = sdsnew(name);
module->ver = ver;
module->apiver = apiver;
module->types = listCreate();
module->usedby = listCreate();
module->using = listCreate();
module->filters = listCreate();
module->in_call = 0;
module->in_hook = 0;
module->options = 0;
module->info_cb = 0;
module->defrag_cb = 0;
module->loadmod = NULL;
ctx->module = module;
}



int RM_IsModuleNameBusy(const char *name) {
sds modulename = sdsnew(name);
dictEntry *de = dictFind(modules,modulename);
sdsfree(modulename);
return de != NULL;
}


long long RM_Milliseconds(void) {
return mstime();
}


uint64_t RM_MonotonicMicroseconds(void) {
return getMonotonicUs();
}







int RM_BlockedClientMeasureTimeStart(RedisModuleBlockedClient *bc) {
elapsedStart(&(bc->background_timer));
return REDISMODULE_OK;
}






int RM_BlockedClientMeasureTimeEnd(RedisModuleBlockedClient *bc) {

if (!bc->background_timer)
return REDISMODULE_ERR;
bc->background_duration += elapsedUs(bc->background_timer);
return REDISMODULE_OK;
}






















void RM_Yield(RedisModuleCtx *ctx, int flags, const char *busy_reply) {
long long now = getMonotonicUs();
if (now >= ctx->next_yield_time) {



if (server.loading) {

processEventsWhileBlocked();
} else {
const char *prev_busy_module_yield_reply = server.busy_module_yield_reply;
server.busy_module_yield_reply = busy_reply;

if (!server.busy_module_yield_flags) {
server.busy_module_yield_flags = flags & REDISMODULE_YIELD_FLAG_CLIENTS ?
BUSY_MODULE_YIELD_CLIENTS : BUSY_MODULE_YIELD_EVENTS;
blockingOperationStarts();
}


processEventsWhileBlocked();

server.busy_module_yield_reply = prev_busy_module_yield_reply;



server.busy_module_yield_flags &= ~BUSY_MODULE_YIELD_CLIENTS;
}


ctx->next_yield_time = now + 1000000 / server.hz;
}
}


















void RM_SetModuleOptions(RedisModuleCtx *ctx, int options) {
ctx->module->options = options;
}








int RM_SignalModifiedKey(RedisModuleCtx *ctx, RedisModuleString *keyname) {
signalModifiedKey(ctx->client,ctx->client->db,keyname);
return REDISMODULE_OK;
}




















void RM_AutoMemory(RedisModuleCtx *ctx) {
ctx->flags |= REDISMODULE_CTX_AUTO_MEMORY;
}


void autoMemoryAdd(RedisModuleCtx *ctx, int type, void *ptr) {
if (!(ctx->flags & REDISMODULE_CTX_AUTO_MEMORY)) return;
if (ctx->amqueue_used == ctx->amqueue_len) {
ctx->amqueue_len *= 2;
if (ctx->amqueue_len < 16) ctx->amqueue_len = 16;
ctx->amqueue = zrealloc(ctx->amqueue,sizeof(struct AutoMemEntry)*ctx->amqueue_len);
}
ctx->amqueue[ctx->amqueue_used].type = type;
ctx->amqueue[ctx->amqueue_used].ptr = ptr;
ctx->amqueue_used++;
}






int autoMemoryFreed(RedisModuleCtx *ctx, int type, void *ptr) {
if (!(ctx->flags & REDISMODULE_CTX_AUTO_MEMORY)) return 0;

int count = (ctx->amqueue_used+1)/2;
for (int j = 0; j < count; j++) {
for (int side = 0; side < 2; side++) {


int i = (side == 0) ? (ctx->amqueue_used - 1 - j) : j;
if (ctx->amqueue[i].type == type &&
ctx->amqueue[i].ptr == ptr)
{
ctx->amqueue[i].type = REDISMODULE_AM_FREED;



if (i != ctx->amqueue_used-1) {
ctx->amqueue[i] = ctx->amqueue[ctx->amqueue_used-1];
}



ctx->amqueue_used--;
return 1;
}
}
}
return 0;
}


void autoMemoryCollect(RedisModuleCtx *ctx) {
if (!(ctx->flags & REDISMODULE_CTX_AUTO_MEMORY)) return;



ctx->flags &= ~REDISMODULE_CTX_AUTO_MEMORY;
int j;
for (j = 0; j < ctx->amqueue_used; j++) {
void *ptr = ctx->amqueue[j].ptr;
switch(ctx->amqueue[j].type) {
case REDISMODULE_AM_STRING: decrRefCount(ptr); break;
case REDISMODULE_AM_REPLY: RM_FreeCallReply(ptr); break;
case REDISMODULE_AM_KEY: RM_CloseKey(ptr); break;
case REDISMODULE_AM_DICT: RM_FreeDict(NULL,ptr); break;
case REDISMODULE_AM_INFO: RM_FreeServerInfo(NULL,ptr); break;
}
}
ctx->flags |= REDISMODULE_CTX_AUTO_MEMORY;
zfree(ctx->amqueue);
ctx->amqueue = NULL;
ctx->amqueue_len = 0;
ctx->amqueue_used = 0;
}















RedisModuleString *RM_CreateString(RedisModuleCtx *ctx, const char *ptr, size_t len) {
RedisModuleString *o = createStringObject(ptr,len);
if (ctx != NULL) autoMemoryAdd(ctx,REDISMODULE_AM_STRING,o);
return o;
}









RedisModuleString *RM_CreateStringPrintf(RedisModuleCtx *ctx, const char *fmt, ...) {
sds s = sdsempty();

va_list ap;
va_start(ap, fmt);
s = sdscatvprintf(s, fmt, ap);
va_end(ap);

RedisModuleString *o = createObject(OBJ_STRING, s);
if (ctx != NULL) autoMemoryAdd(ctx,REDISMODULE_AM_STRING,o);

return o;
}










RedisModuleString *RM_CreateStringFromLongLong(RedisModuleCtx *ctx, long long ll) {
char buf[LONG_STR_SIZE];
size_t len = ll2string(buf,sizeof(buf),ll);
return RM_CreateString(ctx,buf,len);
}






RedisModuleString *RM_CreateStringFromDouble(RedisModuleCtx *ctx, double d) {
char buf[128];
size_t len = d2string(buf,sizeof(buf),d);
return RM_CreateString(ctx,buf,len);
}









RedisModuleString *RM_CreateStringFromLongDouble(RedisModuleCtx *ctx, long double ld, int humanfriendly) {
char buf[MAX_LONG_DOUBLE_CHARS];
size_t len = ld2string(buf,sizeof(buf),ld,
(humanfriendly ? LD_STR_HUMAN : LD_STR_AUTO));
return RM_CreateString(ctx,buf,len);
}









RedisModuleString *RM_CreateStringFromString(RedisModuleCtx *ctx, const RedisModuleString *str) {
RedisModuleString *o = dupStringObject(str);
if (ctx != NULL) autoMemoryAdd(ctx,REDISMODULE_AM_STRING,o);
return o;
}






RedisModuleString *RM_CreateStringFromStreamID(RedisModuleCtx *ctx, const RedisModuleStreamID *id) {
streamID streamid = {id->ms, id->seq};
RedisModuleString *o = createObjectFromStreamID(&streamid);
if (ctx != NULL) autoMemoryAdd(ctx, REDISMODULE_AM_STRING, o);
return o;
}













void RM_FreeString(RedisModuleCtx *ctx, RedisModuleString *str) {
decrRefCount(str);
if (ctx != NULL) autoMemoryFreed(ctx,REDISMODULE_AM_STRING,str);
}

































void RM_RetainString(RedisModuleCtx *ctx, RedisModuleString *str) {
if (ctx == NULL || !autoMemoryFreed(ctx,REDISMODULE_AM_STRING,str)) {









incrRefCount(str);
}
}




























RedisModuleString* RM_HoldString(RedisModuleCtx *ctx, RedisModuleString *str) {
if (str->refcount == OBJ_STATIC_REFCOUNT) {
return RM_CreateStringFromString(ctx, str);
}

incrRefCount(str);
if (ctx != NULL) {



























autoMemoryAdd(ctx,REDISMODULE_AM_STRING,str);
}
return str;
}




const char *RM_StringPtrLen(const RedisModuleString *str, size_t *len) {
if (str == NULL) {
const char *errmsg = "(NULL string reply referenced in module)";
if (len) *len = strlen(errmsg);
return errmsg;
}
if (len) *len = sdslen(str->ptr);
return str->ptr;
}









int RM_StringToLongLong(const RedisModuleString *str, long long *ll) {
return string2ll(str->ptr,sdslen(str->ptr),ll) ? REDISMODULE_OK :
REDISMODULE_ERR;
}




int RM_StringToDouble(const RedisModuleString *str, double *d) {
int retval = getDoubleFromObject(str,d);
return (retval == C_OK) ? REDISMODULE_OK : REDISMODULE_ERR;
}




int RM_StringToLongDouble(const RedisModuleString *str, long double *ld) {
int retval = string2ld(str->ptr,sdslen(str->ptr),ld);
return retval ? REDISMODULE_OK : REDISMODULE_ERR;
}






int RM_StringToStreamID(const RedisModuleString *str, RedisModuleStreamID *id) {
streamID streamid;
if (streamParseID(str, &streamid) == C_OK) {
id->ms = streamid.ms;
id->seq = streamid.seq;
return REDISMODULE_OK;
} else {
return REDISMODULE_ERR;
}
}




int RM_StringCompare(RedisModuleString *a, RedisModuleString *b) {
return compareStringObjects(a,b);
}



RedisModuleString *moduleAssertUnsharedString(RedisModuleString *str) {
if (str->refcount != 1) {
serverLog(LL_WARNING,
"Module attempted to use an in-place string modify operation "
"with a string referenced multiple times. Please check the code "
"for API usage correctness.");
return NULL;
}
if (str->encoding == OBJ_ENCODING_EMBSTR) {


str->ptr = sdsnewlen(str->ptr,sdslen(str->ptr));
str->encoding = OBJ_ENCODING_RAW;
} else if (str->encoding == OBJ_ENCODING_INT) {

str->ptr = sdsfromlonglong((long)str->ptr);
str->encoding = OBJ_ENCODING_RAW;
}
return str;
}




int RM_StringAppendBuffer(RedisModuleCtx *ctx, RedisModuleString *str, const char *buf, size_t len) {
UNUSED(ctx);
str = moduleAssertUnsharedString(str);
if (str == NULL) return REDISMODULE_ERR;
str->ptr = sdscatlen(str->ptr,buf,len);
return REDISMODULE_OK;
}
























void RM_TrimStringAllocation(RedisModuleString *str) {
if (!str) return;
trimStringObjectIfNeeded(str);
}
































int RM_WrongArity(RedisModuleCtx *ctx) {
addReplyErrorFormat(ctx->client,
"wrong number of arguments for '%s' command",
(char*)ctx->client->argv[0]->ptr);
return REDISMODULE_OK;
}















client *moduleGetReplyClient(RedisModuleCtx *ctx) {
if (ctx->flags & REDISMODULE_CTX_THREAD_SAFE) {
if (ctx->blocked_client)
return ctx->blocked_client->reply_client;
else
return NULL;
} else {




return ctx->client;
}
}



int RM_ReplyWithLongLong(RedisModuleCtx *ctx, long long ll) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyLongLong(c,ll);
return REDISMODULE_OK;
}















int RM_ReplyWithError(RedisModuleCtx *ctx, const char *err) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyErrorFormat(c,"-%s",err);
return REDISMODULE_OK;
}






int RM_ReplyWithSimpleString(RedisModuleCtx *ctx, const char *msg) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyProto(c,"+",1);
addReplyProto(c,msg,strlen(msg));
addReplyProto(c,"\r\n",2);
return REDISMODULE_OK;
}

#define COLLECTION_REPLY_ARRAY 1
#define COLLECTION_REPLY_MAP 2
#define COLLECTION_REPLY_SET 3
#define COLLECTION_REPLY_ATTRIBUTE 4

int moduleReplyWithCollection(RedisModuleCtx *ctx, long len, int type) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
if (len == REDISMODULE_POSTPONED_LEN) {
ctx->postponed_arrays = zrealloc(ctx->postponed_arrays,sizeof(void*)*
(ctx->postponed_arrays_count+1));
ctx->postponed_arrays[ctx->postponed_arrays_count] =
addReplyDeferredLen(c);
ctx->postponed_arrays_count++;
} else if (len == 0) {
switch (type) {
case COLLECTION_REPLY_ARRAY:
addReply(c, shared.emptyarray);
break;
case COLLECTION_REPLY_MAP:
addReply(c, shared.emptymap[c->resp]);
break;
case COLLECTION_REPLY_SET:
addReply(c, shared.emptyset[c->resp]);
break;
case COLLECTION_REPLY_ATTRIBUTE:
addReplyAttributeLen(c,len);
break;
default:
serverPanic("Invalid module empty reply type %d", type); }
} else {
switch (type) {
case COLLECTION_REPLY_ARRAY:
addReplyArrayLen(c,len);
break;
case COLLECTION_REPLY_MAP:
addReplyMapLen(c,len);
break;
case COLLECTION_REPLY_SET:
addReplySetLen(c,len);
break;
case COLLECTION_REPLY_ATTRIBUTE:
addReplyAttributeLen(c,len);
break;
default:
serverPanic("Invalid module reply type %d", type);
}
}
return REDISMODULE_OK;
}










int RM_ReplyWithArray(RedisModuleCtx *ctx, long len) {
return moduleReplyWithCollection(ctx, len, COLLECTION_REPLY_ARRAY);
}














int RM_ReplyWithMap(RedisModuleCtx *ctx, long len) {
return moduleReplyWithCollection(ctx, len, COLLECTION_REPLY_MAP);
}














int RM_ReplyWithSet(RedisModuleCtx *ctx, long len) {
return moduleReplyWithCollection(ctx, len, COLLECTION_REPLY_SET);
}













int RM_ReplyWithAttribute(RedisModuleCtx *ctx, long len) {
if (ctx->client->resp == 2) return REDISMODULE_ERR;

return moduleReplyWithCollection(ctx, len, COLLECTION_REPLY_ATTRIBUTE);
}









int RM_ReplyWithNullArray(RedisModuleCtx *ctx) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyNullArray(c);
return REDISMODULE_OK;
}




int RM_ReplyWithEmptyArray(RedisModuleCtx *ctx) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReply(c,shared.emptyarray);
return REDISMODULE_OK;
}

void moduleReplySetCollectionLength(RedisModuleCtx *ctx, long len, int type) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return;
if (ctx->postponed_arrays_count == 0) {
serverLog(LL_WARNING,
"API misuse detected in module %s: "
"RedisModule_ReplySet*Length() called without previous "
"RedisModule_ReplyWith*(ctx,REDISMODULE_POSTPONED_LEN) "
"call.", ctx->module->name);
return;
}
ctx->postponed_arrays_count--;
switch(type) {
case COLLECTION_REPLY_ARRAY:
setDeferredArrayLen(c,ctx->postponed_arrays[ctx->postponed_arrays_count],len);
break;
case COLLECTION_REPLY_MAP:
setDeferredMapLen(c,ctx->postponed_arrays[ctx->postponed_arrays_count],len);
break;
case COLLECTION_REPLY_SET:
setDeferredSetLen(c,ctx->postponed_arrays[ctx->postponed_arrays_count],len);
break;
case COLLECTION_REPLY_ATTRIBUTE:
setDeferredAttributeLen(c,ctx->postponed_arrays[ctx->postponed_arrays_count],len);
break;
default:
serverPanic("Invalid module reply type %d", type);
}
if (ctx->postponed_arrays_count == 0) {
zfree(ctx->postponed_arrays);
ctx->postponed_arrays = NULL;
}
}



























void RM_ReplySetArrayLength(RedisModuleCtx *ctx, long len) {
moduleReplySetCollectionLength(ctx, len, COLLECTION_REPLY_ARRAY);
}





void RM_ReplySetMapLength(RedisModuleCtx *ctx, long len) {
moduleReplySetCollectionLength(ctx, len, COLLECTION_REPLY_MAP);
}



void RM_ReplySetSetLength(RedisModuleCtx *ctx, long len) {
moduleReplySetCollectionLength(ctx, len, COLLECTION_REPLY_SET);
}



void RM_ReplySetAttributeLength(RedisModuleCtx *ctx, long len) {
moduleReplySetCollectionLength(ctx, len, COLLECTION_REPLY_ATTRIBUTE);
}




int RM_ReplyWithStringBuffer(RedisModuleCtx *ctx, const char *buf, size_t len) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyBulkCBuffer(c,(char*)buf,len);
return REDISMODULE_OK;
}





int RM_ReplyWithCString(RedisModuleCtx *ctx, const char *buf) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyBulkCString(c,(char*)buf);
return REDISMODULE_OK;
}




int RM_ReplyWithString(RedisModuleCtx *ctx, RedisModuleString *str) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyBulk(c,str);
return REDISMODULE_OK;
}




int RM_ReplyWithEmptyString(RedisModuleCtx *ctx) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReply(c,shared.emptybulk);
return REDISMODULE_OK;
}





int RM_ReplyWithVerbatimStringType(RedisModuleCtx *ctx, const char *buf, size_t len, const char *ext) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyVerbatim(c, buf, len, ext);
return REDISMODULE_OK;
}





int RM_ReplyWithVerbatimString(RedisModuleCtx *ctx, const char *buf, size_t len) {
return RM_ReplyWithVerbatimStringType(ctx, buf, len, "txt");
}




int RM_ReplyWithNull(RedisModuleCtx *ctx) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyNull(c);
return REDISMODULE_OK;
}








int RM_ReplyWithBool(RedisModuleCtx *ctx, int b) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyBool(c,b);
return REDISMODULE_OK;
}














int RM_ReplyWithCallReply(RedisModuleCtx *ctx, RedisModuleCallReply *reply) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
if (c->resp == 2 && callReplyIsResp3(reply)) {


return REDISMODULE_ERR;
}
size_t proto_len;
const char *proto = callReplyGetProto(reply, &proto_len);
addReplyProto(c, proto, proto_len);
return REDISMODULE_OK;
}













int RM_ReplyWithDouble(RedisModuleCtx *ctx, double d) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyDouble(c,d);
return REDISMODULE_OK;
}









int RM_ReplyWithBigNumber(RedisModuleCtx *ctx, const char *bignum, size_t len) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyBigNum(c, bignum, len);
return REDISMODULE_OK;
}









int RM_ReplyWithLongDouble(RedisModuleCtx *ctx, long double ld) {
client *c = moduleGetReplyClient(ctx);
if (c == NULL) return REDISMODULE_OK;
addReplyHumanLongDouble(c, ld);
return REDISMODULE_OK;
}










































int RM_Replicate(RedisModuleCtx *ctx, const char *cmdname, const char *fmt, ...) {
struct redisCommand *cmd;
robj **argv = NULL;
int argc = 0, flags = 0, j;
va_list ap;

cmd = lookupCommandByCString((char*)cmdname);
if (!cmd) return REDISMODULE_ERR;


va_start(ap, fmt);
argv = moduleCreateArgvFromUserFormat(cmdname,fmt,&argc,NULL,&flags,ap);
va_end(ap);
if (argv == NULL) return REDISMODULE_ERR;




int target = 0;
if (!(flags & REDISMODULE_ARGV_NO_AOF)) target |= PROPAGATE_AOF;
if (!(flags & REDISMODULE_ARGV_NO_REPLICAS)) target |= PROPAGATE_REPL;

alsoPropagate(ctx->client->db->id,argv,argc,target);


for (j = 0; j < argc; j++) decrRefCount(argv[j]);
zfree(argv);
server.dirty++;
return REDISMODULE_OK;
}












int RM_ReplicateVerbatim(RedisModuleCtx *ctx) {
alsoPropagate(ctx->client->db->id,
ctx->client->argv,ctx->client->argc,
PROPAGATE_AOF|PROPAGATE_REPL);
server.dirty++;
return REDISMODULE_OK;
}
























unsigned long long RM_GetClientId(RedisModuleCtx *ctx) {
if (ctx->client == NULL) return 0;
return ctx->client->id;
}





RedisModuleString *RM_GetClientUserNameById(RedisModuleCtx *ctx, uint64_t id) {
client *client = lookupClientByID(id);
if (client == NULL) {
errno = ENOENT;
return NULL;
}

if (client->user == NULL) {
errno = ENOTSUP;
return NULL;
}

sds name = sdsnew(client->user->name);
robj *str = createObject(OBJ_STRING, name);
autoMemoryAdd(ctx, REDISMODULE_AM_STRING, str);
return str;
}







int modulePopulateClientInfoStructure(void *ci, client *client, int structver) {
if (structver != 1) return REDISMODULE_ERR;

RedisModuleClientInfoV1 *ci1 = ci;
memset(ci1,0,sizeof(*ci1));
ci1->version = structver;
if (client->flags & CLIENT_MULTI)
ci1->flags |= REDISMODULE_CLIENTINFO_FLAG_MULTI;
if (client->flags & CLIENT_PUBSUB)
ci1->flags |= REDISMODULE_CLIENTINFO_FLAG_PUBSUB;
if (client->flags & CLIENT_UNIX_SOCKET)
ci1->flags |= REDISMODULE_CLIENTINFO_FLAG_UNIXSOCKET;
if (client->flags & CLIENT_TRACKING)
ci1->flags |= REDISMODULE_CLIENTINFO_FLAG_TRACKING;
if (client->flags & CLIENT_BLOCKED)
ci1->flags |= REDISMODULE_CLIENTINFO_FLAG_BLOCKED;
if (connGetType(client->conn) == CONN_TYPE_TLS)
ci1->flags |= REDISMODULE_CLIENTINFO_FLAG_SSL;

int port;
connPeerToString(client->conn,ci1->addr,sizeof(ci1->addr),&port);
ci1->port = port;
ci1->db = client->db->id;
ci1->id = client->id;
return REDISMODULE_OK;
}






int modulePopulateReplicationInfoStructure(void *ri, int structver) {
if (structver != 1) return REDISMODULE_ERR;

RedisModuleReplicationInfoV1 *ri1 = ri;
memset(ri1,0,sizeof(*ri1));
ri1->version = structver;
ri1->master = server.masterhost==NULL;
ri1->masterhost = server.masterhost? server.masterhost: "";
ri1->masterport = server.masterport;
ri1->replid1 = server.replid;
ri1->replid2 = server.replid2;
ri1->repl1_offset = server.master_repl_offset;
ri1->repl2_offset = server.second_replid_offset;
return REDISMODULE_OK;
}











































int RM_GetClientInfoById(void *ci, uint64_t id) {
client *client = lookupClientByID(id);
if (client == NULL) return REDISMODULE_ERR;
if (ci == NULL) return REDISMODULE_OK;


uint64_t structver = ((uint64_t*)ci)[0];
return modulePopulateClientInfoStructure(ci,client,structver);
}


int RM_PublishMessage(RedisModuleCtx *ctx, RedisModuleString *channel, RedisModuleString *message) {
UNUSED(ctx);
int receivers = pubsubPublishMessage(channel, message);
if (server.cluster_enabled)
clusterPropagatePublish(channel, message);
return receivers;
}


int RM_GetSelectedDb(RedisModuleCtx *ctx) {
return ctx->client->db->id;
}





































































int RM_GetContextFlags(RedisModuleCtx *ctx) {
int flags = 0;


if (ctx) {
if (ctx->client) {
if (ctx->client->flags & CLIENT_DENY_BLOCKING)
flags |= REDISMODULE_CTX_FLAGS_DENY_BLOCKING;

if (ctx->client->flags & CLIENT_MASTER)
flags |= REDISMODULE_CTX_FLAGS_REPLICATED;
if (ctx->client->resp == 3) {
flags |= REDISMODULE_CTX_FLAGS_RESP3;
}
}


client *c = ctx->blocked_client ? ctx->blocked_client->client : ctx->client;
if (c && (c->flags & (CLIENT_DIRTY_CAS|CLIENT_DIRTY_EXEC))) {
flags |= REDISMODULE_CTX_FLAGS_MULTI_DIRTY;
}
}

if (server.in_script)
flags |= REDISMODULE_CTX_FLAGS_LUA;

if (server.in_exec)
flags |= REDISMODULE_CTX_FLAGS_MULTI;

if (server.cluster_enabled)
flags |= REDISMODULE_CTX_FLAGS_CLUSTER;

if (server.async_loading)
flags |= REDISMODULE_CTX_FLAGS_ASYNC_LOADING;
else if (server.loading)
flags |= REDISMODULE_CTX_FLAGS_LOADING;


if (server.maxmemory > 0 && (!server.masterhost || !server.repl_slave_ignore_maxmemory)) {
flags |= REDISMODULE_CTX_FLAGS_MAXMEMORY;

if (server.maxmemory_policy != MAXMEMORY_NO_EVICTION)
flags |= REDISMODULE_CTX_FLAGS_EVICT;
}


if (server.aof_state != AOF_OFF)
flags |= REDISMODULE_CTX_FLAGS_AOF;
if (server.saveparamslen > 0)
flags |= REDISMODULE_CTX_FLAGS_RDB;


if (server.masterhost == NULL) {
flags |= REDISMODULE_CTX_FLAGS_MASTER;
} else {
flags |= REDISMODULE_CTX_FLAGS_SLAVE;
if (server.repl_slave_ro)
flags |= REDISMODULE_CTX_FLAGS_READONLY;


if (server.repl_state == REPL_STATE_CONNECT ||
server.repl_state == REPL_STATE_CONNECTING)
{
flags |= REDISMODULE_CTX_FLAGS_REPLICA_IS_CONNECTING;
} else if (server.repl_state == REPL_STATE_TRANSFER) {
flags |= REDISMODULE_CTX_FLAGS_REPLICA_IS_TRANSFERRING;
} else if (server.repl_state == REPL_STATE_CONNECTED) {
flags |= REDISMODULE_CTX_FLAGS_REPLICA_IS_ONLINE;
}

if (server.repl_state != REPL_STATE_CONNECTED)
flags |= REDISMODULE_CTX_FLAGS_REPLICA_IS_STALE;
}


float level;
int retval = getMaxmemoryState(NULL,NULL,NULL,&level);
if (retval == C_ERR) flags |= REDISMODULE_CTX_FLAGS_OOM;
if (level > 0.75) flags |= REDISMODULE_CTX_FLAGS_OOM_WARNING;


if (hasActiveChildProcess()) flags |= REDISMODULE_CTX_FLAGS_ACTIVE_CHILD;
if (server.in_fork_child) flags |= REDISMODULE_CTX_FLAGS_IS_CHILD;

return flags;
}





















int RM_AvoidReplicaTraffic() {
return checkClientPauseTimeoutAndReturnIfPaused();
}











int RM_SelectDb(RedisModuleCtx *ctx, int newid) {
int retval = selectDb(ctx->client,newid);
return (retval == C_OK) ? REDISMODULE_OK : REDISMODULE_ERR;
}







int RM_KeyExists(RedisModuleCtx *ctx, robj *keyname) {
robj *value = lookupKeyReadWithFlags(ctx->client->db, keyname, LOOKUP_NOTOUCH);
return (value != NULL);
}


static void moduleInitKey(RedisModuleKey *kp, RedisModuleCtx *ctx, robj *keyname, robj *value, int mode){
kp->ctx = ctx;
kp->db = ctx->client->db;
kp->key = keyname;
incrRefCount(keyname);
kp->value = value;
kp->iter = NULL;
kp->mode = mode;
if (kp->value) moduleInitKeyTypeSpecific(kp);
}


static void moduleInitKeyTypeSpecific(RedisModuleKey *key) {
switch (key->value->type) {
case OBJ_ZSET: zsetKeyReset(key); break;
case OBJ_STREAM: key->u.stream.signalready = 0; break;
}
}















void *RM_OpenKey(RedisModuleCtx *ctx, robj *keyname, int mode) {
RedisModuleKey *kp;
robj *value;
int flags = mode & REDISMODULE_OPEN_KEY_NOTOUCH? LOOKUP_NOTOUCH: 0;

if (mode & REDISMODULE_WRITE) {
value = lookupKeyWriteWithFlags(ctx->client->db,keyname, flags);
} else {
value = lookupKeyReadWithFlags(ctx->client->db,keyname, flags);
if (value == NULL) {
return NULL;
}
}


kp = zmalloc(sizeof(*kp));
moduleInitKey(kp, ctx, keyname, value, mode);
autoMemoryAdd(ctx,REDISMODULE_AM_KEY,kp);
return (void*)kp;
}


static void moduleCloseKey(RedisModuleKey *key) {
int signal = SHOULD_SIGNAL_MODIFIED_KEYS(key->ctx);
if ((key->mode & REDISMODULE_WRITE) && signal)
signalModifiedKey(key->ctx->client,key->db,key->key);
if (key->value) {
if (key->iter) moduleFreeKeyIterator(key);
switch (key->value->type) {
case OBJ_ZSET:
RM_ZsetRangeStop(key);
break;
case OBJ_STREAM:
if (key->u.stream.signalready)

signalKeyAsReady(key->db, key->key, OBJ_STREAM);
break;
}
}
serverAssert(key->iter == NULL);
decrRefCount(key->key);
}


void RM_CloseKey(RedisModuleKey *key) {
if (key == NULL) return;
moduleCloseKey(key);
autoMemoryFreed(key->ctx,REDISMODULE_AM_KEY,key);
zfree(key);
}



int RM_KeyType(RedisModuleKey *key) {
if (key == NULL || key->value == NULL) return REDISMODULE_KEYTYPE_EMPTY;


switch(key->value->type) {
case OBJ_STRING: return REDISMODULE_KEYTYPE_STRING;
case OBJ_LIST: return REDISMODULE_KEYTYPE_LIST;
case OBJ_SET: return REDISMODULE_KEYTYPE_SET;
case OBJ_ZSET: return REDISMODULE_KEYTYPE_ZSET;
case OBJ_HASH: return REDISMODULE_KEYTYPE_HASH;
case OBJ_MODULE: return REDISMODULE_KEYTYPE_MODULE;
case OBJ_STREAM: return REDISMODULE_KEYTYPE_STREAM;
default: return REDISMODULE_KEYTYPE_EMPTY;
}
}






size_t RM_ValueLength(RedisModuleKey *key) {
if (key == NULL || key->value == NULL) return 0;
switch(key->value->type) {
case OBJ_STRING: return stringObjectLen(key->value);
case OBJ_LIST: return listTypeLength(key->value);
case OBJ_SET: return setTypeSize(key->value);
case OBJ_ZSET: return zsetLength(key->value);
case OBJ_HASH: return hashTypeLength(key->value);
case OBJ_STREAM: return streamLength(key->value);
default: return 0;
}
}





int RM_DeleteKey(RedisModuleKey *key) {
if (!(key->mode & REDISMODULE_WRITE)) return REDISMODULE_ERR;
if (key->value) {
dbDelete(key->db,key->key);
key->value = NULL;
}
return REDISMODULE_OK;
}






int RM_UnlinkKey(RedisModuleKey *key) {
if (!(key->mode & REDISMODULE_WRITE)) return REDISMODULE_ERR;
if (key->value) {
dbAsyncDelete(key->db,key->key);
key->value = NULL;
}
return REDISMODULE_OK;
}




mstime_t RM_GetExpire(RedisModuleKey *key) {
mstime_t expire = getExpire(key->db,key->key);
if (expire == -1 || key->value == NULL)
return REDISMODULE_NO_EXPIRE;
expire -= mstime();
return expire >= 0 ? expire : 0;
}










int RM_SetExpire(RedisModuleKey *key, mstime_t expire) {
if (!(key->mode & REDISMODULE_WRITE) || key->value == NULL || (expire < 0 && expire != REDISMODULE_NO_EXPIRE))
return REDISMODULE_ERR;
if (expire != REDISMODULE_NO_EXPIRE) {
expire += mstime();
setExpire(key->ctx->client,key->db,key->key,expire);
} else {
removeExpire(key->db,key->key);
}
return REDISMODULE_OK;
}




mstime_t RM_GetAbsExpire(RedisModuleKey *key) {
mstime_t expire = getExpire(key->db,key->key);
if (expire == -1 || key->value == NULL)
return REDISMODULE_NO_EXPIRE;
return expire;
}










int RM_SetAbsExpire(RedisModuleKey *key, mstime_t expire) {
if (!(key->mode & REDISMODULE_WRITE) || key->value == NULL || (expire < 0 && expire != REDISMODULE_NO_EXPIRE))
return REDISMODULE_ERR;
if (expire != REDISMODULE_NO_EXPIRE) {
setExpire(key->ctx->client,key->db,key->key,expire);
} else {
removeExpire(key->db,key->key);
}
return REDISMODULE_OK;
}





void RM_ResetDataset(int restart_aof, int async) {
if (restart_aof && server.aof_state != AOF_OFF) stopAppendOnly();
flushAllDataAndResetRDB(async? EMPTYDB_ASYNC: EMPTYDB_NO_FLAGS);
if (server.aof_enabled && restart_aof) restartAOFAfterSYNC();
}


unsigned long long RM_DbSize(RedisModuleCtx *ctx) {
return dictSize(ctx->client->db->dict);
}


RedisModuleString *RM_RandomKey(RedisModuleCtx *ctx) {
robj *key = dbRandomKey(ctx->client->db);
autoMemoryAdd(ctx,REDISMODULE_AM_STRING,key);
return key;
}


const RedisModuleString *RM_GetKeyNameFromOptCtx(RedisModuleKeyOptCtx *ctx) {
return ctx->from_key;
}


const RedisModuleString *RM_GetToKeyNameFromOptCtx(RedisModuleKeyOptCtx *ctx) {
return ctx->to_key;
}


int RM_GetDbIdFromOptCtx(RedisModuleKeyOptCtx *ctx) {
return ctx->from_dbid;
}


int RM_GetToDbIdFromOptCtx(RedisModuleKeyOptCtx *ctx) {
return ctx->to_dbid;
}










int RM_StringSet(RedisModuleKey *key, RedisModuleString *str) {
if (!(key->mode & REDISMODULE_WRITE) || key->iter) return REDISMODULE_ERR;
RM_DeleteKey(key);
setKey(key->ctx->client,key->db,key->key,str,SETKEY_NO_SIGNAL);
key->value = str;
return REDISMODULE_OK;
}






























char *RM_StringDMA(RedisModuleKey *key, size_t *len, int mode) {




char *emptystring = "<dma-empty-string>";
if (key->value == NULL) {
*len = 0;
return emptystring;
}

if (key->value->type != OBJ_STRING) return NULL;



if ((mode & REDISMODULE_WRITE) || key->value->encoding != OBJ_ENCODING_RAW)
key->value = dbUnshareStringValue(key->db, key->key, key->value);

*len = sdslen(key->value->ptr);
return key->value->ptr;
}













int RM_StringTruncate(RedisModuleKey *key, size_t newlen) {
if (!(key->mode & REDISMODULE_WRITE)) return REDISMODULE_ERR;
if (key->value && key->value->type != OBJ_STRING) return REDISMODULE_ERR;
if (newlen > 512*1024*1024) return REDISMODULE_ERR;



if (key->value == NULL && newlen == 0) return REDISMODULE_OK;

if (key->value == NULL) {

robj *o = createObject(OBJ_STRING,sdsnewlen(NULL, newlen));
setKey(key->ctx->client,key->db,key->key,o,SETKEY_NO_SIGNAL);
key->value = o;
decrRefCount(o);
} else {

key->value = dbUnshareStringValue(key->db, key->key, key->value);
size_t curlen = sdslen(key->value->ptr);
if (newlen > curlen) {
key->value->ptr = sdsgrowzero(key->value->ptr,newlen);
} else if (newlen < curlen) {
sdssubstr(key->value->ptr,0,newlen);

if (sdslen(key->value->ptr) < sdsavail(key->value->ptr))
key->value->ptr = sdsRemoveFreeSpace(key->value->ptr);
}
}
return REDISMODULE_OK;
}

































int moduleListIteratorSeek(RedisModuleKey *key, long index, int mode) {
if (!key) {
errno = EINVAL;
return 0;
} else if (!key->value || key->value->type != OBJ_LIST) {
errno = ENOTSUP;
return 0;
} if (!(key->mode & mode)) {
errno = EBADF;
return 0;
}

long length = listTypeLength(key->value);
if (index < -length || index >= length) {
errno = EDOM;
return 0;
}

if (key->iter == NULL) {

key->iter = listTypeInitIterator(key->value, index, LIST_TAIL);
serverAssert(key->iter != NULL);
serverAssert(listTypeNext(key->iter, &key->u.list.entry));
key->u.list.index = index;
return 1;
}



if (index < 0 && key->u.list.index >= 0) index += length;
else if (index >= 0 && key->u.list.index < 0) index -= length;

if (index == key->u.list.index) return 1;


unsigned char dir = key->u.list.index < index ? LIST_TAIL : LIST_HEAD;
listTypeSetIteratorDirection(key->iter, dir);
while (key->u.list.index != index) {
serverAssert(listTypeNext(key->iter, &key->u.list.entry));
key->u.list.index += dir == LIST_HEAD ? -1 : 1;
}
return 1;
}












int RM_ListPush(RedisModuleKey *key, int where, RedisModuleString *ele) {
if (!key || !ele) {
errno = EINVAL;
return REDISMODULE_ERR;
} else if (key->value != NULL && key->value->type != OBJ_LIST) {
errno = ENOTSUP;
return REDISMODULE_ERR;
} if (!(key->mode & REDISMODULE_WRITE)) {
errno = EBADF;
return REDISMODULE_ERR;
}

if (!(key->mode & REDISMODULE_WRITE)) return REDISMODULE_ERR;
if (key->value && key->value->type != OBJ_LIST) return REDISMODULE_ERR;
if (key->iter) {
listTypeReleaseIterator(key->iter);
key->iter = NULL;
}
if (key->value == NULL) moduleCreateEmptyKey(key,REDISMODULE_KEYTYPE_LIST);
listTypePush(key->value, ele,
(where == REDISMODULE_LIST_HEAD) ? LIST_HEAD : LIST_TAIL);
return REDISMODULE_OK;
}













RedisModuleString *RM_ListPop(RedisModuleKey *key, int where) {
if (!key) {
errno = EINVAL;
return NULL;
} else if (key->value == NULL || key->value->type != OBJ_LIST) {
errno = ENOTSUP;
return NULL;
} else if (!(key->mode & REDISMODULE_WRITE)) {
errno = EBADF;
return NULL;
}
if (key->iter) {
listTypeReleaseIterator(key->iter);
key->iter = NULL;
}
robj *ele = listTypePop(key->value,
(where == REDISMODULE_LIST_HEAD) ? LIST_HEAD : LIST_TAIL);
robj *decoded = getDecodedObject(ele);
decrRefCount(ele);
moduleDelKeyIfEmpty(key);
autoMemoryAdd(key->ctx,REDISMODULE_AM_STRING,decoded);
return decoded;
}


















RedisModuleString *RM_ListGet(RedisModuleKey *key, long index) {
if (moduleListIteratorSeek(key, index, REDISMODULE_READ)) {
robj *elem = listTypeGet(&key->u.list.entry);
robj *decoded = getDecodedObject(elem);
decrRefCount(elem);
autoMemoryAdd(key->ctx, REDISMODULE_AM_STRING, decoded);
return decoded;
} else {
return NULL;
}
}
















int RM_ListSet(RedisModuleKey *key, long index, RedisModuleString *value) {
if (!value) {
errno = EINVAL;
return REDISMODULE_ERR;
}
if (moduleListIteratorSeek(key, index, REDISMODULE_WRITE)) {
listTypeReplace(&key->u.list.entry, value);


listTypeReleaseIterator(key->iter);
key->iter = NULL;
return REDISMODULE_OK;
} else {
return REDISMODULE_ERR;
}
}
















int RM_ListInsert(RedisModuleKey *key, long index, RedisModuleString *value) {
if (!value) {
errno = EINVAL;
return REDISMODULE_ERR;
} else if (key != NULL && key->value == NULL &&
(index == 0 || index == -1)) {

return RM_ListPush(key, REDISMODULE_LIST_TAIL, value);
} else if (key != NULL && key->value != NULL &&
key->value->type == OBJ_LIST &&
(index == (long)listTypeLength(key->value) || index == -1)) {

return RM_ListPush(key, REDISMODULE_LIST_TAIL, value);
} else if (key != NULL && key->value != NULL &&
key->value->type == OBJ_LIST &&
(index == 0 || index == -(long)listTypeLength(key->value) - 1)) {

return RM_ListPush(key, REDISMODULE_LIST_HEAD, value);
}
if (moduleListIteratorSeek(key, index, REDISMODULE_WRITE)) {
int where = index < 0 ? LIST_TAIL : LIST_HEAD;
listTypeInsert(&key->u.list.entry, value, where);

listTypeReleaseIterator(key->iter);
key->iter = NULL;
return REDISMODULE_OK;
} else {
return REDISMODULE_ERR;
}
}












int RM_ListDelete(RedisModuleKey *key, long index) {
if (moduleListIteratorSeek(key, index, REDISMODULE_WRITE)) {
listTypeDelete(key->iter, &key->u.list.entry);
moduleDelKeyIfEmpty(key);
return REDISMODULE_OK;
} else {
return REDISMODULE_ERR;
}
}









int moduleZsetAddFlagsToCoreFlags(int flags) {
int retflags = 0;
if (flags & REDISMODULE_ZADD_XX) retflags |= ZADD_IN_XX;
if (flags & REDISMODULE_ZADD_NX) retflags |= ZADD_IN_NX;
if (flags & REDISMODULE_ZADD_GT) retflags |= ZADD_IN_GT;
if (flags & REDISMODULE_ZADD_LT) retflags |= ZADD_IN_LT;
return retflags;
}


int moduleZsetAddFlagsFromCoreFlags(int flags) {
int retflags = 0;
if (flags & ZADD_OUT_ADDED) retflags |= REDISMODULE_ZADD_ADDED;
if (flags & ZADD_OUT_UPDATED) retflags |= REDISMODULE_ZADD_UPDATED;
if (flags & ZADD_OUT_NOP) retflags |= REDISMODULE_ZADD_NOP;
return retflags;
}

































int RM_ZsetAdd(RedisModuleKey *key, double score, RedisModuleString *ele, int *flagsptr) {
int in_flags = 0, out_flags = 0;
if (!(key->mode & REDISMODULE_WRITE)) return REDISMODULE_ERR;
if (key->value && key->value->type != OBJ_ZSET) return REDISMODULE_ERR;
if (key->value == NULL) moduleCreateEmptyKey(key,REDISMODULE_KEYTYPE_ZSET);
if (flagsptr) in_flags = moduleZsetAddFlagsToCoreFlags(*flagsptr);
if (zsetAdd(key->value,score,ele->ptr,in_flags,&out_flags,NULL) == 0) {
if (flagsptr) *flagsptr = 0;
return REDISMODULE_ERR;
}
if (flagsptr) *flagsptr = moduleZsetAddFlagsFromCoreFlags(out_flags);
return REDISMODULE_OK;
}














int RM_ZsetIncrby(RedisModuleKey *key, double score, RedisModuleString *ele, int *flagsptr, double *newscore) {
int in_flags = 0, out_flags = 0;
if (!(key->mode & REDISMODULE_WRITE)) return REDISMODULE_ERR;
if (key->value && key->value->type != OBJ_ZSET) return REDISMODULE_ERR;
if (key->value == NULL) moduleCreateEmptyKey(key,REDISMODULE_KEYTYPE_ZSET);
if (flagsptr) in_flags = moduleZsetAddFlagsToCoreFlags(*flagsptr);
in_flags |= ZADD_IN_INCR;
if (zsetAdd(key->value,score,ele->ptr,in_flags,&out_flags,newscore) == 0) {
if (flagsptr) *flagsptr = 0;
return REDISMODULE_ERR;
}
if (flagsptr) *flagsptr = moduleZsetAddFlagsFromCoreFlags(out_flags);
return REDISMODULE_OK;
}



















int RM_ZsetRem(RedisModuleKey *key, RedisModuleString *ele, int *deleted) {
if (!(key->mode & REDISMODULE_WRITE)) return REDISMODULE_ERR;
if (key->value && key->value->type != OBJ_ZSET) return REDISMODULE_ERR;
if (key->value != NULL && zsetDel(key->value,ele->ptr)) {
if (deleted) *deleted = 1;
moduleDelKeyIfEmpty(key);
} else {
if (deleted) *deleted = 0;
}
return REDISMODULE_OK;
}









int RM_ZsetScore(RedisModuleKey *key, RedisModuleString *ele, double *score) {
if (key->value == NULL) return REDISMODULE_ERR;
if (key->value->type != OBJ_ZSET) return REDISMODULE_ERR;
if (zsetScore(key->value,ele->ptr,score) == C_ERR) return REDISMODULE_ERR;
return REDISMODULE_OK;
}





void zsetKeyReset(RedisModuleKey *key) {
key->u.zset.type = REDISMODULE_ZSET_RANGE_NONE;
key->u.zset.current = NULL;
key->u.zset.er = 1;
}


void RM_ZsetRangeStop(RedisModuleKey *key) {
if (!key->value || key->value->type != OBJ_ZSET) return;

if (key->u.zset.type == REDISMODULE_ZSET_RANGE_LEX)
zslFreeLexRange(&key->u.zset.lrs);



zsetKeyReset(key);
}


int RM_ZsetRangeEndReached(RedisModuleKey *key) {
if (!key->value || key->value->type != OBJ_ZSET) return 1;
return key->u.zset.er;
}







int zsetInitScoreRange(RedisModuleKey *key, double min, double max, int minex, int maxex, int first) {
if (!key->value || key->value->type != OBJ_ZSET) return REDISMODULE_ERR;

RM_ZsetRangeStop(key);
key->u.zset.type = REDISMODULE_ZSET_RANGE_SCORE;
key->u.zset.er = 0;



zrangespec *zrs = &key->u.zset.rs;
zrs->min = min;
zrs->max = max;
zrs->minex = minex;
zrs->maxex = maxex;

if (key->value->encoding == OBJ_ENCODING_LISTPACK) {
key->u.zset.current = first ? zzlFirstInRange(key->value->ptr,zrs) :
zzlLastInRange(key->value->ptr,zrs);
} else if (key->value->encoding == OBJ_ENCODING_SKIPLIST) {
zset *zs = key->value->ptr;
zskiplist *zsl = zs->zsl;
key->u.zset.current = first ? zslFirstInRange(zsl,zrs) :
zslLastInRange(zsl,zrs);
} else {
serverPanic("Unsupported zset encoding");
}
if (key->u.zset.current == NULL) key->u.zset.er = 1;
return REDISMODULE_OK;
}
















int RM_ZsetFirstInScoreRange(RedisModuleKey *key, double min, double max, int minex, int maxex) {
return zsetInitScoreRange(key,min,max,minex,maxex,1);
}



int RM_ZsetLastInScoreRange(RedisModuleKey *key, double min, double max, int minex, int maxex) {
return zsetInitScoreRange(key,min,max,minex,maxex,0);
}










int zsetInitLexRange(RedisModuleKey *key, RedisModuleString *min, RedisModuleString *max, int first) {
if (!key->value || key->value->type != OBJ_ZSET) return REDISMODULE_ERR;

RM_ZsetRangeStop(key);
key->u.zset.er = 0;



zlexrangespec *zlrs = &key->u.zset.lrs;
if (zslParseLexRange(min, max, zlrs) == C_ERR) return REDISMODULE_ERR;



key->u.zset.type = REDISMODULE_ZSET_RANGE_LEX;

if (key->value->encoding == OBJ_ENCODING_LISTPACK) {
key->u.zset.current = first ? zzlFirstInLexRange(key->value->ptr,zlrs) :
zzlLastInLexRange(key->value->ptr,zlrs);
} else if (key->value->encoding == OBJ_ENCODING_SKIPLIST) {
zset *zs = key->value->ptr;
zskiplist *zsl = zs->zsl;
key->u.zset.current = first ? zslFirstInLexRange(zsl,zlrs) :
zslLastInLexRange(zsl,zlrs);
} else {
serverPanic("Unsupported zset encoding");
}
if (key->u.zset.current == NULL) key->u.zset.er = 1;

return REDISMODULE_OK;
}













int RM_ZsetFirstInLexRange(RedisModuleKey *key, RedisModuleString *min, RedisModuleString *max) {
return zsetInitLexRange(key,min,max,1);
}



int RM_ZsetLastInLexRange(RedisModuleKey *key, RedisModuleString *min, RedisModuleString *max) {
return zsetInitLexRange(key,min,max,0);
}




RedisModuleString *RM_ZsetRangeCurrentElement(RedisModuleKey *key, double *score) {
RedisModuleString *str;

if (!key->value || key->value->type != OBJ_ZSET) return NULL;
if (key->u.zset.current == NULL) return NULL;
if (key->value->encoding == OBJ_ENCODING_LISTPACK) {
unsigned char *eptr, *sptr;
eptr = key->u.zset.current;
sds ele = lpGetObject(eptr);
if (score) {
sptr = lpNext(key->value->ptr,eptr);
*score = zzlGetScore(sptr);
}
str = createObject(OBJ_STRING,ele);
} else if (key->value->encoding == OBJ_ENCODING_SKIPLIST) {
zskiplistNode *ln = key->u.zset.current;
if (score) *score = ln->score;
str = createStringObject(ln->ele,sdslen(ln->ele));
} else {
serverPanic("Unsupported zset encoding");
}
autoMemoryAdd(key->ctx,REDISMODULE_AM_STRING,str);
return str;
}




int RM_ZsetRangeNext(RedisModuleKey *key) {
if (!key->value || key->value->type != OBJ_ZSET) return 0;
if (!key->u.zset.type || !key->u.zset.current) return 0;

if (key->value->encoding == OBJ_ENCODING_LISTPACK) {
unsigned char *zl = key->value->ptr;
unsigned char *eptr = key->u.zset.current;
unsigned char *next;
next = lpNext(zl,eptr);
if (next) next = lpNext(zl,next);
if (next == NULL) {
key->u.zset.er = 1;
return 0;
} else {

if (key->u.zset.type == REDISMODULE_ZSET_RANGE_SCORE) {


unsigned char *saved_next = next;
next = lpNext(zl,next);
double score = zzlGetScore(next);
if (!zslValueLteMax(score,&key->u.zset.rs)) {
key->u.zset.er = 1;
return 0;
}
next = saved_next;
} else if (key->u.zset.type == REDISMODULE_ZSET_RANGE_LEX) {
if (!zzlLexValueLteMax(next,&key->u.zset.lrs)) {
key->u.zset.er = 1;
return 0;
}
}
key->u.zset.current = next;
return 1;
}
} else if (key->value->encoding == OBJ_ENCODING_SKIPLIST) {
zskiplistNode *ln = key->u.zset.current, *next = ln->level[0].forward;
if (next == NULL) {
key->u.zset.er = 1;
return 0;
} else {

if (key->u.zset.type == REDISMODULE_ZSET_RANGE_SCORE &&
!zslValueLteMax(next->score,&key->u.zset.rs))
{
key->u.zset.er = 1;
return 0;
} else if (key->u.zset.type == REDISMODULE_ZSET_RANGE_LEX) {
if (!zslLexValueLteMax(next->ele,&key->u.zset.lrs)) {
key->u.zset.er = 1;
return 0;
}
}
key->u.zset.current = next;
return 1;
}
} else {
serverPanic("Unsupported zset encoding");
}
}




int RM_ZsetRangePrev(RedisModuleKey *key) {
if (!key->value || key->value->type != OBJ_ZSET) return 0;
if (!key->u.zset.type || !key->u.zset.current) return 0;

if (key->value->encoding == OBJ_ENCODING_LISTPACK) {
unsigned char *zl = key->value->ptr;
unsigned char *eptr = key->u.zset.current;
unsigned char *prev;
prev = lpPrev(zl,eptr);
if (prev) prev = lpPrev(zl,prev);
if (prev == NULL) {
key->u.zset.er = 1;
return 0;
} else {

if (key->u.zset.type == REDISMODULE_ZSET_RANGE_SCORE) {


unsigned char *saved_prev = prev;
prev = lpNext(zl,prev);
double score = zzlGetScore(prev);
if (!zslValueGteMin(score,&key->u.zset.rs)) {
key->u.zset.er = 1;
return 0;
}
prev = saved_prev;
} else if (key->u.zset.type == REDISMODULE_ZSET_RANGE_LEX) {
if (!zzlLexValueGteMin(prev,&key->u.zset.lrs)) {
key->u.zset.er = 1;
return 0;
}
}
key->u.zset.current = prev;
return 1;
}
} else if (key->value->encoding == OBJ_ENCODING_SKIPLIST) {
zskiplistNode *ln = key->u.zset.current, *prev = ln->backward;
if (prev == NULL) {
key->u.zset.er = 1;
return 0;
} else {

if (key->u.zset.type == REDISMODULE_ZSET_RANGE_SCORE &&
!zslValueGteMin(prev->score,&key->u.zset.rs))
{
key->u.zset.er = 1;
return 0;
} else if (key->u.zset.type == REDISMODULE_ZSET_RANGE_LEX) {
if (!zslLexValueGteMin(prev->ele,&key->u.zset.lrs)) {
key->u.zset.er = 1;
return 0;
}
}
key->u.zset.current = prev;
return 1;
}
} else {
serverPanic("Unsupported zset encoding");
}
}










































































int RM_HashSet(RedisModuleKey *key, int flags, ...) {
va_list ap;
if (!key || (flags & ~(REDISMODULE_HASH_NX |
REDISMODULE_HASH_XX |
REDISMODULE_HASH_CFIELDS |
REDISMODULE_HASH_COUNT_ALL))) {
errno = EINVAL;
return 0;
} else if (key->value && key->value->type != OBJ_HASH) {
errno = ENOTSUP;
return 0;
} else if (!(key->mode & REDISMODULE_WRITE)) {
errno = EBADF;
return 0;
}
if (key->value == NULL) moduleCreateEmptyKey(key,REDISMODULE_KEYTYPE_HASH);

int count = 0;
va_start(ap, flags);
while(1) {
RedisModuleString *field, *value;

if (flags & REDISMODULE_HASH_CFIELDS) {
char *cfield = va_arg(ap,char*);
if (cfield == NULL) break;
field = createRawStringObject(cfield,strlen(cfield));
} else {
field = va_arg(ap,RedisModuleString*);
if (field == NULL) break;
}
value = va_arg(ap,RedisModuleString*);


if (flags & (REDISMODULE_HASH_XX|REDISMODULE_HASH_NX)) {
int exists = hashTypeExists(key->value, field->ptr);
if (((flags & REDISMODULE_HASH_XX) && !exists) ||
((flags & REDISMODULE_HASH_NX) && exists))
{
if (flags & REDISMODULE_HASH_CFIELDS) decrRefCount(field);
continue;
}
}


if (value == REDISMODULE_HASH_DELETE) {
count += hashTypeDelete(key->value, field->ptr);
if (flags & REDISMODULE_HASH_CFIELDS) decrRefCount(field);
continue;
}

int low_flags = HASH_SET_COPY;



if (flags & REDISMODULE_HASH_CFIELDS)
low_flags |= HASH_SET_TAKE_FIELD;

robj *argv[2] = {field,value};
hashTypeTryConversion(key->value,argv,0,1);
int updated = hashTypeSet(key->value, field->ptr, value->ptr, low_flags);
count += (flags & REDISMODULE_HASH_COUNT_ALL) ? 1 : updated;



if (flags & REDISMODULE_HASH_CFIELDS) {
field->ptr = NULL;
decrRefCount(field);
}
}
va_end(ap);
moduleDelKeyIfEmpty(key);
if (count == 0) errno = ENOENT;
return count;
}










































int RM_HashGet(RedisModuleKey *key, int flags, ...) {
va_list ap;
if (key->value && key->value->type != OBJ_HASH) return REDISMODULE_ERR;

va_start(ap, flags);
while(1) {
RedisModuleString *field, **valueptr;
int *existsptr;

if (flags & REDISMODULE_HASH_CFIELDS) {
char *cfield = va_arg(ap,char*);
if (cfield == NULL) break;
field = createRawStringObject(cfield,strlen(cfield));
} else {
field = va_arg(ap,RedisModuleString*);
if (field == NULL) break;
}


if (flags & REDISMODULE_HASH_EXISTS) {
existsptr = va_arg(ap,int*);
if (key->value)
*existsptr = hashTypeExists(key->value,field->ptr);
else
*existsptr = 0;
} else {
valueptr = va_arg(ap,RedisModuleString**);
if (key->value) {
*valueptr = hashTypeGetValueObject(key->value,field->ptr);
if (*valueptr) {
robj *decoded = getDecodedObject(*valueptr);
decrRefCount(*valueptr);
*valueptr = decoded;
}
if (*valueptr)
autoMemoryAdd(key->ctx,REDISMODULE_AM_STRING,*valueptr);
} else {
*valueptr = NULL;
}
}


if (flags & REDISMODULE_HASH_CFIELDS) decrRefCount(field);
}
va_end(ap);
return REDISMODULE_OK;
}










































int RM_StreamAdd(RedisModuleKey *key, int flags, RedisModuleStreamID *id, RedisModuleString **argv, long numfields) {

if (!key || (numfields != 0 && !argv) ||
(flags & ~(REDISMODULE_STREAM_ADD_AUTOID)) ||
(!(flags & REDISMODULE_STREAM_ADD_AUTOID) && !id)) {
errno = EINVAL;
return REDISMODULE_ERR;
} else if (key->value && key->value->type != OBJ_STREAM) {
errno = ENOTSUP;
return REDISMODULE_ERR;
} else if (!(key->mode & REDISMODULE_WRITE)) {
errno = EBADF;
return REDISMODULE_ERR;
} else if (!(flags & REDISMODULE_STREAM_ADD_AUTOID) &&
id->ms == 0 && id->seq == 0) {
errno = EDOM;
return REDISMODULE_ERR;
}


int created = 0;
if (key->value == NULL) {
moduleCreateEmptyKey(key, REDISMODULE_KEYTYPE_STREAM);
created = 1;
}

stream *s = key->value->ptr;
if (s->last_id.ms == UINT64_MAX && s->last_id.seq == UINT64_MAX) {

errno = EFBIG;
return REDISMODULE_ERR;
}

streamID added_id;
streamID use_id;
streamID *use_id_ptr = NULL;
if (!(flags & REDISMODULE_STREAM_ADD_AUTOID)) {
use_id.ms = id->ms;
use_id.seq = id->seq;
use_id_ptr = &use_id;
}

if (streamAppendItem(s,argv,numfields,&added_id,use_id_ptr,1) == C_ERR) {



return REDISMODULE_ERR;
}


if (!created) key->u.stream.signalready = 1;

if (id != NULL) {
id->ms = added_id.ms;
id->seq = added_id.seq;
}

return REDISMODULE_OK;
}



















int RM_StreamDelete(RedisModuleKey *key, RedisModuleStreamID *id) {
if (!key || !id) {
errno = EINVAL;
return REDISMODULE_ERR;
} else if (!key->value || key->value->type != OBJ_STREAM) {
errno = ENOTSUP;
return REDISMODULE_ERR;
} else if (!(key->mode & REDISMODULE_WRITE) ||
key->iter != NULL) {
errno = EBADF;
return REDISMODULE_ERR;
}
stream *s = key->value->ptr;
streamID streamid = {id->ms, id->seq};
if (streamDeleteItem(s, &streamid)) {
return REDISMODULE_OK;
} else {
errno = ENOENT;
return REDISMODULE_ERR;
}
}


















































int RM_StreamIteratorStart(RedisModuleKey *key, int flags, RedisModuleStreamID *start, RedisModuleStreamID *end) {

if (!key ||
(flags & ~(REDISMODULE_STREAM_ITERATOR_EXCLUSIVE |
REDISMODULE_STREAM_ITERATOR_REVERSE))) {
errno = EINVAL;
return REDISMODULE_ERR;
} else if (!key->value || key->value->type != OBJ_STREAM) {
errno = ENOTSUP;
return REDISMODULE_ERR;
} else if (key->iter) {
errno = EBADF;
return REDISMODULE_ERR;
}


streamID lower, upper;
if (start) lower = (streamID){start->ms, start->seq};
if (end) upper = (streamID){end->ms, end->seq};
if (flags & REDISMODULE_STREAM_ITERATOR_EXCLUSIVE) {
if ((start && streamIncrID(&lower) != C_OK) ||
(end && streamDecrID(&upper) != C_OK)) {
errno = EDOM;
return REDISMODULE_ERR;
}
}


stream *s = key->value->ptr;
int rev = flags & REDISMODULE_STREAM_ITERATOR_REVERSE;
streamIterator *si = zmalloc(sizeof(*si));
streamIteratorStart(si, s, start ? &lower : NULL, end ? &upper : NULL, rev);
key->iter = si;
key->u.stream.currentid.ms = 0;
key->u.stream.currentid.seq = 0;
key->u.stream.numfieldsleft = 0;
return REDISMODULE_OK;
}













int RM_StreamIteratorStop(RedisModuleKey *key) {
if (!key) {
errno = EINVAL;
return REDISMODULE_ERR;
} else if (!key->value || key->value->type != OBJ_STREAM) {
errno = ENOTSUP;
return REDISMODULE_ERR;
} else if (!key->iter) {
errno = EBADF;
return REDISMODULE_ERR;
}
zfree(key->iter);
key->iter = NULL;
return REDISMODULE_OK;
}


























int RM_StreamIteratorNextID(RedisModuleKey *key, RedisModuleStreamID *id, long *numfields) {
if (!key) {
errno = EINVAL;
return REDISMODULE_ERR;
} else if (!key->value || key->value->type != OBJ_STREAM) {
errno = ENOTSUP;
return REDISMODULE_ERR;
} else if (!key->iter) {
errno = EBADF;
return REDISMODULE_ERR;
}
streamIterator *si = key->iter;
int64_t *num_ptr = &key->u.stream.numfieldsleft;
streamID *streamid_ptr = &key->u.stream.currentid;
if (streamIteratorGetID(si, streamid_ptr, num_ptr)) {
if (id) {
id->ms = streamid_ptr->ms;
id->seq = streamid_ptr->seq;
}
if (numfields) *numfields = *num_ptr;
return REDISMODULE_OK;
} else {

key->u.stream.currentid.ms = 0;
key->u.stream.currentid.seq = 0;
key->u.stream.numfieldsleft = 0;
errno = ENOENT;
return REDISMODULE_ERR;
}
}


























int RM_StreamIteratorNextField(RedisModuleKey *key, RedisModuleString **field_ptr, RedisModuleString **value_ptr) {
if (!key) {
errno = EINVAL;
return REDISMODULE_ERR;
} else if (!key->value || key->value->type != OBJ_STREAM) {
errno = ENOTSUP;
return REDISMODULE_ERR;
} else if (!key->iter) {
errno = EBADF;
return REDISMODULE_ERR;
} else if (key->u.stream.numfieldsleft <= 0) {
errno = ENOENT;
return REDISMODULE_ERR;
}
streamIterator *si = key->iter;
unsigned char *field, *value;
int64_t field_len, value_len;
streamIteratorGetField(si, &field, &value, &field_len, &value_len);
if (field_ptr) {
*field_ptr = createRawStringObject((char *)field, field_len);
autoMemoryAdd(key->ctx, REDISMODULE_AM_STRING, *field_ptr);
}
if (value_ptr) {
*value_ptr = createRawStringObject((char *)value, value_len);
autoMemoryAdd(key->ctx, REDISMODULE_AM_STRING, *value_ptr);
}
key->u.stream.numfieldsleft--;
return REDISMODULE_OK;
}














int RM_StreamIteratorDelete(RedisModuleKey *key) {
if (!key) {
errno = EINVAL;
return REDISMODULE_ERR;
} else if (!key->value || key->value->type != OBJ_STREAM) {
errno = ENOTSUP;
return REDISMODULE_ERR;
} else if (!(key->mode & REDISMODULE_WRITE) || !key->iter) {
errno = EBADF;
return REDISMODULE_ERR;
} else if (key->u.stream.currentid.ms == 0 &&
key->u.stream.currentid.seq == 0) {
errno = ENOENT;
return REDISMODULE_ERR;
}
streamIterator *si = key->iter;
streamIteratorRemoveEntry(si, &key->u.stream.currentid);
key->u.stream.currentid.ms = 0;
key->u.stream.currentid.seq = 0;
key->u.stream.numfieldsleft = 0;
return REDISMODULE_OK;
}
















long long RM_StreamTrimByLength(RedisModuleKey *key, int flags, long long length) {
if (!key || (flags & ~(REDISMODULE_STREAM_TRIM_APPROX)) || length < 0) {
errno = EINVAL;
return -1;
} else if (!key->value || key->value->type != OBJ_STREAM) {
errno = ENOTSUP;
return -1;
} else if (!(key->mode & REDISMODULE_WRITE)) {
errno = EBADF;
return -1;
}
int approx = flags & REDISMODULE_STREAM_TRIM_APPROX ? 1 : 0;
return streamTrimByLength((stream *)key->value->ptr, length, approx);
}
















long long RM_StreamTrimByID(RedisModuleKey *key, int flags, RedisModuleStreamID *id) {
if (!key || (flags & ~(REDISMODULE_STREAM_TRIM_APPROX)) || !id) {
errno = EINVAL;
return -1;
} else if (!key->value || key->value->type != OBJ_STREAM) {
errno = ENOTSUP;
return -1;
} else if (!(key->mode & REDISMODULE_WRITE)) {
errno = EBADF;
return -1;
}
int approx = flags & REDISMODULE_STREAM_TRIM_APPROX ? 1 : 0;
streamID minid = (streamID){id->ms, id->seq};
return streamTrimByID((stream *)key->value->ptr, minid, approx);
}








void moduleParseCallReply_Int(RedisModuleCallReply *reply);
void moduleParseCallReply_BulkString(RedisModuleCallReply *reply);
void moduleParseCallReply_SimpleString(RedisModuleCallReply *reply);
void moduleParseCallReply_Array(RedisModuleCallReply *reply);






void RM_FreeCallReply(RedisModuleCallReply *reply) {



RedisModuleCtx *ctx = callReplyGetPrivateData(reply);
freeCallReply(reply);
autoMemoryFreed(ctx,REDISMODULE_AM_REPLY,reply);
}
















int RM_CallReplyType(RedisModuleCallReply *reply) {
return callReplyType(reply);
}


size_t RM_CallReplyLength(RedisModuleCallReply *reply) {
return callReplyGetLen(reply);
}



RedisModuleCallReply *RM_CallReplyArrayElement(RedisModuleCallReply *reply, size_t idx) {
return callReplyGetArrayElement(reply, idx);
}


long long RM_CallReplyInteger(RedisModuleCallReply *reply) {
return callReplyGetLongLong(reply);
}


double RM_CallReplyDouble(RedisModuleCallReply *reply) {
return callReplyGetDouble(reply);
}


const char *RM_CallReplyBigNumber(RedisModuleCallReply *reply, size_t *len) {
return callReplyGetBigNumber(reply, len);
}



const char *RM_CallReplyVerbatim(RedisModuleCallReply *reply, size_t *len, const char **format) {
return callReplyGetVerbatim(reply, len, format);
}


int RM_CallReplyBool(RedisModuleCallReply *reply) {
return callReplyGetBool(reply);
}



RedisModuleCallReply *RM_CallReplySetElement(RedisModuleCallReply *reply, size_t idx) {
return callReplyGetSetElement(reply, idx);
}









int RM_CallReplyMapElement(RedisModuleCallReply *reply, size_t idx, RedisModuleCallReply **key, RedisModuleCallReply **val) {
if (callReplyGetMapElement(reply, idx, key, val) == C_OK){
return REDISMODULE_OK;
}
return REDISMODULE_ERR;
}


RedisModuleCallReply *RM_CallReplyAttribute(RedisModuleCallReply *reply) {
return callReplyGetAttribute(reply);
}









int RM_CallReplyAttributeElement(RedisModuleCallReply *reply, size_t idx, RedisModuleCallReply **key, RedisModuleCallReply **val) {
if (callReplyGetAttributeElement(reply, idx, key, val) == C_OK){
return REDISMODULE_OK;
}
return REDISMODULE_ERR;
}


const char *RM_CallReplyStringPtr(RedisModuleCallReply *reply, size_t *len) {
size_t private_len;
if (!len) len = &private_len;
return callReplyGetString(reply, len);
}



RedisModuleString *RM_CreateStringFromCallReply(RedisModuleCallReply *reply) {
RedisModuleCtx* ctx = callReplyGetPrivateData(reply);
size_t len;
const char *str;
switch(callReplyType(reply)) {
case REDISMODULE_REPLY_STRING:
case REDISMODULE_REPLY_ERROR:
str = callReplyGetString(reply, &len);
return RM_CreateString(ctx, str, len);
case REDISMODULE_REPLY_INTEGER: {
char buf[64];
int len = ll2string(buf,sizeof(buf),callReplyGetLongLong(reply));
return RM_CreateString(ctx ,buf,len);
}
default:
return NULL;
}
}

















robj **moduleCreateArgvFromUserFormat(const char *cmdname, const char *fmt, int *argcp, int *argvlenp, int *flags, va_list ap) {
int argc = 0, argv_size, j;
robj **argv = NULL;



argv_size = strlen(fmt)+1;
argv = zrealloc(argv,sizeof(robj*)*argv_size);


argv[0] = createStringObject(cmdname,strlen(cmdname));
argc++;


const char *p = fmt;
while(*p) {
if (*p == 'c') {
char *cstr = va_arg(ap,char*);
argv[argc++] = createStringObject(cstr,strlen(cstr));
} else if (*p == 's') {
robj *obj = va_arg(ap,void*);
if (obj->refcount == OBJ_STATIC_REFCOUNT)
obj = createStringObject(obj->ptr,sdslen(obj->ptr));
else
incrRefCount(obj);
argv[argc++] = obj;
} else if (*p == 'b') {
char *buf = va_arg(ap,char*);
size_t len = va_arg(ap,size_t);
argv[argc++] = createStringObject(buf,len);
} else if (*p == 'l') {
long long ll = va_arg(ap,long long);
argv[argc++] = createObject(OBJ_STRING,sdsfromlonglong(ll));
} else if (*p == 'v') {

robj **v = va_arg(ap, void*);
size_t vlen = va_arg(ap, size_t);




argv_size += vlen-1;
argv = zrealloc(argv,sizeof(robj*)*argv_size);

size_t i = 0;
for (i = 0; i < vlen; i++) {
incrRefCount(v[i]);
argv[argc++] = v[i];
}
} else if (*p == '!') {
if (flags) (*flags) |= REDISMODULE_ARGV_REPLICATE;
} else if (*p == 'A') {
if (flags) (*flags) |= REDISMODULE_ARGV_NO_AOF;
} else if (*p == 'R') {
if (flags) (*flags) |= REDISMODULE_ARGV_NO_REPLICAS;
} else if (*p == '3') {
if (flags) (*flags) |= REDISMODULE_ARGV_RESP_3;
} else if (*p == '0') {
if (flags) (*flags) |= REDISMODULE_ARGV_RESP_AUTO;
} else if (*p == 'C') {
if (flags) (*flags) |= REDISMODULE_ARGV_CHECK_ACL;
} else {
goto fmterr;
}
p++;
}
if (argcp) *argcp = argc;
if (argvlenp) *argvlenp = argv_size;
return argv;

fmterr:
for (j = 0; j < argc; j++)
decrRefCount(argv[j]);
zfree(argv);
return NULL;
}

















































RedisModuleCallReply *RM_Call(RedisModuleCtx *ctx, const char *cmdname, const char *fmt, ...) {
struct redisCommand *cmd;
client *c = NULL;
robj **argv = NULL;
int argc = 0, argv_len = 0, flags = 0;
va_list ap;
RedisModuleCallReply *reply = NULL;
int replicate = 0;


va_start(ap, fmt);
argv = moduleCreateArgvFromUserFormat(cmdname,fmt,&argc,&argv_len,&flags,ap);
replicate = flags & REDISMODULE_ARGV_REPLICATE;
va_end(ap);

c = moduleAllocTempClient();


c->flags |= CLIENT_DENY_BLOCKING;
c->db = ctx->client->db;
c->argv = argv;
c->argc = argc;
c->argv_len = argv_len;
c->resp = 2;
if (flags & REDISMODULE_ARGV_RESP_3) {
c->resp = 3;
} else if (flags & REDISMODULE_ARGV_RESP_AUTO) {

c->resp = ctx->client->resp;
}
if (ctx->module) ctx->module->in_call++;



if (argv == NULL) {
errno = EBADF;
goto cleanup;
}


moduleCallCommandFilters(c);




cmd = lookupCommand(c->argv,c->argc);
if (!cmd) {
errno = ENOENT;
goto cleanup;
}
c->cmd = c->lastcmd = cmd;


if ((cmd->arity > 0 && cmd->arity != argc) || (argc < -cmd->arity)) {
errno = EINVAL;
goto cleanup;
}



if (flags & REDISMODULE_ARGV_CHECK_ACL) {
int acl_errpos;
int acl_retval;

if (ctx->client->user == NULL) {
errno = ENOTSUP;
goto cleanup;
}
acl_retval = ACLCheckAllUserCommandPerm(ctx->client->user,c->cmd,c->argv,c->argc,&acl_errpos);
if (acl_retval != ACL_OK) {
sds object = (acl_retval == ACL_DENIED_CMD) ? sdsdup(c->cmd->fullname) : sdsdup(c->argv[acl_errpos]->ptr);
addACLLogEntry(ctx->client, acl_retval, ACL_LOG_CTX_MODULE, -1, ctx->client->user->name, object);
errno = EACCES;
goto cleanup;
}
}




if (server.cluster_enabled && !(ctx->client->flags & CLIENT_MASTER)) {
int error_code;

c->flags &= ~(CLIENT_READONLY|CLIENT_ASKING);
c->flags |= ctx->client->flags & (CLIENT_READONLY|CLIENT_ASKING);
if (getNodeByQuery(c,c->cmd,c->argv,c->argc,NULL,&error_code) !=
server.cluster->myself)
{
if (error_code == CLUSTER_REDIR_DOWN_RO_STATE) {
errno = EROFS;
} else if (error_code == CLUSTER_REDIR_DOWN_STATE) {
errno = ENETDOWN;
} else {
errno = EPERM;
}
goto cleanup;
}
}







int prev_replication_allowed = server.replication_allowed;
server.replication_allowed = replicate && server.replication_allowed;


int call_flags = CMD_CALL_SLOWLOG | CMD_CALL_STATS | CMD_CALL_FROM_MODULE;
if (replicate) {
if (!(flags & REDISMODULE_ARGV_NO_AOF))
call_flags |= CMD_CALL_PROPAGATE_AOF;
if (!(flags & REDISMODULE_ARGV_NO_REPLICAS))
call_flags |= CMD_CALL_PROPAGATE_REPL;
}

client *old_client = server.current_client;
server.current_client = c;
call(c,call_flags);
server.current_client = old_client;
server.replication_allowed = prev_replication_allowed;

serverAssert((c->flags & CLIENT_BLOCKED) == 0);


sds proto = sdsnewlen(c->buf,c->bufpos);
c->bufpos = 0;
while(listLength(c->reply)) {
clientReplyBlock *o = listNodeValue(listFirst(c->reply));

proto = sdscatlen(proto,o->buf,o->used);
listDelNode(c->reply,listFirst(c->reply));
}
reply = callReplyCreate(proto, ctx);
autoMemoryAdd(ctx,REDISMODULE_AM_REPLY,reply);

cleanup:
if (ctx->module) ctx->module->in_call--;
moduleReleaseTempClient(c);
return reply;
}



const char *RM_CallReplyProto(RedisModuleCallReply *reply, size_t *len) {
return callReplyGetProto(reply, len);
}


































const char *ModuleTypeNameCharSet =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789-_";

uint64_t moduleTypeEncodeId(const char *name, int encver) {


const char *cset = ModuleTypeNameCharSet;
if (strlen(name) != 9) return 0;
if (encver < 0 || encver > 1023) return 0;

uint64_t id = 0;
for (int j = 0; j < 9; j++) {
char *p = strchr(cset,name[j]);
if (!p) return 0;
unsigned long pos = p-cset;
id = (id << 6) | pos;
}
id = (id << 10) | encver;
return id;
}




moduleType *moduleTypeLookupModuleByName(const char *name) {
dictIterator *di = dictGetIterator(modules);
dictEntry *de;

while ((de = dictNext(di)) != NULL) {
struct RedisModule *module = dictGetVal(de);
listIter li;
listNode *ln;

listRewind(module->types,&li);
while((ln = listNext(&li))) {
moduleType *mt = ln->value;
if (memcmp(name,mt->name,sizeof(mt->name)) == 0) {
dictReleaseIterator(di);
return mt;
}
}
}
dictReleaseIterator(di);
return NULL;
}




#define MODULE_LOOKUP_CACHE_SIZE 3

moduleType *moduleTypeLookupModuleByID(uint64_t id) {
static struct {
uint64_t id;
moduleType *mt;
} cache[MODULE_LOOKUP_CACHE_SIZE];


int j;
for (j = 0; j < MODULE_LOOKUP_CACHE_SIZE && cache[j].mt != NULL; j++)
if (cache[j].id == id) return cache[j].mt;


moduleType *mt = NULL;
dictIterator *di = dictGetIterator(modules);
dictEntry *de;

while ((de = dictNext(di)) != NULL && mt == NULL) {
struct RedisModule *module = dictGetVal(de);
listIter li;
listNode *ln;

listRewind(module->types,&li);
while((ln = listNext(&li))) {
moduleType *this_mt = ln->value;


if (this_mt->id >> 10 == id >> 10) {
mt = this_mt;
break;
}
}
}
dictReleaseIterator(di);


if (mt && j < MODULE_LOOKUP_CACHE_SIZE) {
cache[j].id = id;
cache[j].mt = mt;
}
return mt;
}





void moduleTypeNameByID(char *name, uint64_t moduleid) {
const char *cset = ModuleTypeNameCharSet;

name[9] = '\0';
char *p = name+8;
moduleid >>= 10;
for (int j = 0; j < 9; j++) {
*p-- = cset[moduleid & 63];
moduleid >>= 6;
}
}


const char *moduleTypeModuleName(moduleType *mt) {
if (!mt || !mt->module) return NULL;
return mt->module->name;
}




robj *moduleTypeDupOrReply(client *c, robj *fromkey, robj *tokey, int todb, robj *value) {
moduleValue *mv = value->ptr;
moduleType *mt = mv->type;
if (!mt->copy && !mt->copy2) {
addReplyError(c, "not supported for this module key");
return NULL;
}
void *newval = NULL;
if (mt->copy2 != NULL) {
RedisModuleKeyOptCtx ctx = {fromkey, tokey, c->db->id, todb};
newval = mt->copy2(&ctx, mv->value);
} else {
newval = mt->copy(fromkey, tokey, mv->value);
}

if (!newval) {
addReplyError(c, "module key failed to copy");
return NULL;
}
return createModuleObject(mt, newval);
}






















































































































moduleType *RM_CreateDataType(RedisModuleCtx *ctx, const char *name, int encver, void *typemethods_ptr) {
uint64_t id = moduleTypeEncodeId(name,encver);
if (id == 0) return NULL;
if (moduleTypeLookupModuleByName(name) != NULL) return NULL;

long typemethods_version = ((long*)typemethods_ptr)[0];
if (typemethods_version == 0) return NULL;

struct typemethods {
uint64_t version;
moduleTypeLoadFunc rdb_load;
moduleTypeSaveFunc rdb_save;
moduleTypeRewriteFunc aof_rewrite;
moduleTypeMemUsageFunc mem_usage;
moduleTypeDigestFunc digest;
moduleTypeFreeFunc free;
struct {
moduleTypeAuxLoadFunc aux_load;
moduleTypeAuxSaveFunc aux_save;
int aux_save_triggers;
} v2;
struct {
moduleTypeFreeEffortFunc free_effort;
moduleTypeUnlinkFunc unlink;
moduleTypeCopyFunc copy;
moduleTypeDefragFunc defrag;
} v3;
struct {
moduleTypeMemUsageFunc2 mem_usage2;
moduleTypeFreeEffortFunc2 free_effort2;
moduleTypeUnlinkFunc2 unlink2;
moduleTypeCopyFunc2 copy2;
} v4;
} *tms = (struct typemethods*) typemethods_ptr;

moduleType *mt = zcalloc(sizeof(*mt));
mt->id = id;
mt->module = ctx->module;
mt->rdb_load = tms->rdb_load;
mt->rdb_save = tms->rdb_save;
mt->aof_rewrite = tms->aof_rewrite;
mt->mem_usage = tms->mem_usage;
mt->digest = tms->digest;
mt->free = tms->free;
if (tms->version >= 2) {
mt->aux_load = tms->v2.aux_load;
mt->aux_save = tms->v2.aux_save;
mt->aux_save_triggers = tms->v2.aux_save_triggers;
}
if (tms->version >= 3) {
mt->free_effort = tms->v3.free_effort;
mt->unlink = tms->v3.unlink;
mt->copy = tms->v3.copy;
mt->defrag = tms->v3.defrag;
}
if (tms->version >= 4) {
mt->mem_usage2 = tms->v4.mem_usage2;
mt->unlink2 = tms->v4.unlink2;
mt->free_effort2 = tms->v4.free_effort2;
mt->copy2 = tms->v4.copy2;
}
memcpy(mt->name,name,sizeof(mt->name));
listAddNodeTail(ctx->module->types,mt);
return mt;
}





int RM_ModuleTypeSetValue(RedisModuleKey *key, moduleType *mt, void *value) {
if (!(key->mode & REDISMODULE_WRITE) || key->iter) return REDISMODULE_ERR;
RM_DeleteKey(key);
robj *o = createModuleObject(mt,value);
setKey(key->ctx->client,key->db,key->key,o,SETKEY_NO_SIGNAL);
decrRefCount(o);
key->value = o;
return REDISMODULE_OK;
}






moduleType *RM_ModuleTypeGetType(RedisModuleKey *key) {
if (key == NULL ||
key->value == NULL ||
RM_KeyType(key) != REDISMODULE_KEYTYPE_MODULE) return NULL;
moduleValue *mv = key->value->ptr;
return mv->type;
}







void *RM_ModuleTypeGetValue(RedisModuleKey *key) {
if (key == NULL ||
key->value == NULL ||
RM_KeyType(key) != REDISMODULE_KEYTYPE_MODULE) return NULL;
moduleValue *mv = key->value->ptr;
return mv->value;
}








void moduleRDBLoadError(RedisModuleIO *io) {
if (io->type->module->options & REDISMODULE_OPTIONS_HANDLE_IO_ERRORS) {
io->error = 1;
return;
}
serverPanic(
"Error loading data from RDB (short read or EOF). "
"Read performed by module '%s' about type '%s' "
"after reading '%llu' bytes of a value "
"for key named: '%s'.",
io->type->module->name,
io->type->name,
(unsigned long long)io->bytes,
io->key? (char*)io->key->ptr: "(null)");
}




int moduleAllDatatypesHandleErrors() {
dictIterator *di = dictGetIterator(modules);
dictEntry *de;

while ((de = dictNext(di)) != NULL) {
struct RedisModule *module = dictGetVal(de);
if (listLength(module->types) &&
!(module->options & REDISMODULE_OPTIONS_HANDLE_IO_ERRORS))
{
dictReleaseIterator(di);
return 0;
}
}
dictReleaseIterator(di);
return 1;
}




int moduleAllModulesHandleReplAsyncLoad() {
dictIterator *di = dictGetIterator(modules);
dictEntry *de;

while ((de = dictNext(di)) != NULL) {
struct RedisModule *module = dictGetVal(de);
if (!(module->options & REDISMODULE_OPTIONS_HANDLE_REPL_ASYNC_LOAD)) {
dictReleaseIterator(di);
return 0;
}
}
dictReleaseIterator(di);
return 1;
}




int RM_IsIOError(RedisModuleIO *io) {
return io->error;
}




void RM_SaveUnsigned(RedisModuleIO *io, uint64_t value) {
if (io->error) return;

int retval = rdbSaveLen(io->rio, RDB_MODULE_OPCODE_UINT);
if (retval == -1) goto saveerr;
io->bytes += retval;

retval = rdbSaveLen(io->rio, value);
if (retval == -1) goto saveerr;
io->bytes += retval;
return;

saveerr:
io->error = 1;
}




uint64_t RM_LoadUnsigned(RedisModuleIO *io) {
if (io->error) return 0;
if (io->ver == 2) {
uint64_t opcode = rdbLoadLen(io->rio,NULL);
if (opcode != RDB_MODULE_OPCODE_UINT) goto loaderr;
}
uint64_t value;
int retval = rdbLoadLenByRef(io->rio, NULL, &value);
if (retval == -1) goto loaderr;
return value;

loaderr:
moduleRDBLoadError(io);
return 0;
}


void RM_SaveSigned(RedisModuleIO *io, int64_t value) {
union {uint64_t u; int64_t i;} conv;
conv.i = value;
RM_SaveUnsigned(io,conv.u);
}


int64_t RM_LoadSigned(RedisModuleIO *io) {
union {uint64_t u; int64_t i;} conv;
conv.u = RM_LoadUnsigned(io);
return conv.i;
}







void RM_SaveString(RedisModuleIO *io, RedisModuleString *s) {
if (io->error) return;

ssize_t retval = rdbSaveLen(io->rio, RDB_MODULE_OPCODE_STRING);
if (retval == -1) goto saveerr;
io->bytes += retval;

retval = rdbSaveStringObject(io->rio, s);
if (retval == -1) goto saveerr;
io->bytes += retval;
return;

saveerr:
io->error = 1;
}



void RM_SaveStringBuffer(RedisModuleIO *io, const char *str, size_t len) {
if (io->error) return;

ssize_t retval = rdbSaveLen(io->rio, RDB_MODULE_OPCODE_STRING);
if (retval == -1) goto saveerr;
io->bytes += retval;

retval = rdbSaveRawString(io->rio, (unsigned char*)str,len);
if (retval == -1) goto saveerr;
io->bytes += retval;
return;

saveerr:
io->error = 1;
}


void *moduleLoadString(RedisModuleIO *io, int plain, size_t *lenptr) {
if (io->error) return NULL;
if (io->ver == 2) {
uint64_t opcode = rdbLoadLen(io->rio,NULL);
if (opcode != RDB_MODULE_OPCODE_STRING) goto loaderr;
}
void *s = rdbGenericLoadStringObject(io->rio,
plain ? RDB_LOAD_PLAIN : RDB_LOAD_NONE, lenptr);
if (s == NULL) goto loaderr;
return s;

loaderr:
moduleRDBLoadError(io);
return NULL;
}










RedisModuleString *RM_LoadString(RedisModuleIO *io) {
return moduleLoadString(io,0,NULL);
}








char *RM_LoadStringBuffer(RedisModuleIO *io, size_t *lenptr) {
return moduleLoadString(io,1,lenptr);
}




void RM_SaveDouble(RedisModuleIO *io, double value) {
if (io->error) return;

int retval = rdbSaveLen(io->rio, RDB_MODULE_OPCODE_DOUBLE);
if (retval == -1) goto saveerr;
io->bytes += retval;

retval = rdbSaveBinaryDoubleValue(io->rio, value);
if (retval == -1) goto saveerr;
io->bytes += retval;
return;

saveerr:
io->error = 1;
}



double RM_LoadDouble(RedisModuleIO *io) {
if (io->error) return 0;
if (io->ver == 2) {
uint64_t opcode = rdbLoadLen(io->rio,NULL);
if (opcode != RDB_MODULE_OPCODE_DOUBLE) goto loaderr;
}
double value;
int retval = rdbLoadBinaryDoubleValue(io->rio, &value);
if (retval == -1) goto loaderr;
return value;

loaderr:
moduleRDBLoadError(io);
return 0;
}




void RM_SaveFloat(RedisModuleIO *io, float value) {
if (io->error) return;

int retval = rdbSaveLen(io->rio, RDB_MODULE_OPCODE_FLOAT);
if (retval == -1) goto saveerr;
io->bytes += retval;

retval = rdbSaveBinaryFloatValue(io->rio, value);
if (retval == -1) goto saveerr;
io->bytes += retval;
return;

saveerr:
io->error = 1;
}



float RM_LoadFloat(RedisModuleIO *io) {
if (io->error) return 0;
if (io->ver == 2) {
uint64_t opcode = rdbLoadLen(io->rio,NULL);
if (opcode != RDB_MODULE_OPCODE_FLOAT) goto loaderr;
}
float value;
int retval = rdbLoadBinaryFloatValue(io->rio, &value);
if (retval == -1) goto loaderr;
return value;

loaderr:
moduleRDBLoadError(io);
return 0;
}




void RM_SaveLongDouble(RedisModuleIO *io, long double value) {
if (io->error) return;
char buf[MAX_LONG_DOUBLE_CHARS];


size_t len = ld2string(buf,sizeof(buf),value,LD_STR_HEX);
RM_SaveStringBuffer(io,buf,len);
}



long double RM_LoadLongDouble(RedisModuleIO *io) {
if (io->error) return 0;
long double value;
size_t len;
char* str = RM_LoadStringBuffer(io,&len);
if (!str) return 0;
string2ld(str,len,&value);
RM_Free(str);
return value;
}



ssize_t rdbSaveModulesAux(rio *rdb, int when) {
size_t total_written = 0;
dictIterator *di = dictGetIterator(modules);
dictEntry *de;

while ((de = dictNext(di)) != NULL) {
struct RedisModule *module = dictGetVal(de);
listIter li;
listNode *ln;

listRewind(module->types,&li);
while((ln = listNext(&li))) {
moduleType *mt = ln->value;
if (!mt->aux_save || !(mt->aux_save_triggers & when))
continue;
ssize_t ret = rdbSaveSingleModuleAux(rdb, when, mt);
if (ret==-1) {
dictReleaseIterator(di);
return -1;
}
total_written += ret;
}
}

dictReleaseIterator(di);
return total_written;
}











































void RM_DigestAddStringBuffer(RedisModuleDigest *md, const char *ele, size_t len) {
mixDigest(md->o,ele,len);
}



void RM_DigestAddLongLong(RedisModuleDigest *md, long long ll) {
char buf[LONG_STR_SIZE];
size_t len = ll2string(buf,sizeof(buf),ll);
mixDigest(md->o,buf,len);
}


void RM_DigestEndSequence(RedisModuleDigest *md) {
xorDigest(md->x,md->o,sizeof(md->o));
memset(md->o,0,sizeof(md->o));
}















void *RM_LoadDataTypeFromStringEncver(const RedisModuleString *str, const moduleType *mt, int encver) {
rio payload;
RedisModuleIO io;
void *ret;

rioInitWithBuffer(&payload, str->ptr);
moduleInitIOContext(io,(moduleType *)mt,&payload,NULL,-1);




io.ver = 2;
ret = mt->rdb_load(&io,encver);
if (io.ctx) {
moduleFreeContext(io.ctx);
zfree(io.ctx);
}
return ret;
}




void *RM_LoadDataTypeFromString(const RedisModuleString *str, const moduleType *mt) {
return RM_LoadDataTypeFromStringEncver(str, mt, 0);
}








RedisModuleString *RM_SaveDataTypeToString(RedisModuleCtx *ctx, void *data, const moduleType *mt) {
rio payload;
RedisModuleIO io;

rioInitWithBuffer(&payload,sdsempty());
moduleInitIOContext(io,(moduleType *)mt,&payload,NULL,-1);
mt->rdb_save(&io,data);
if (io.ctx) {
moduleFreeContext(io.ctx);
zfree(io.ctx);
}
if (io.error) {
return NULL;
} else {
robj *str = createObject(OBJ_STRING,payload.io.buffer.ptr);
if (ctx != NULL) autoMemoryAdd(ctx,REDISMODULE_AM_STRING,str);
return str;
}
}


const RedisModuleString *RM_GetKeyNameFromDigest(RedisModuleDigest *dig) {
return dig->key;
}


int RM_GetDbIdFromDigest(RedisModuleDigest *dig) {
return dig->dbid;
}









void RM_EmitAOF(RedisModuleIO *io, const char *cmdname, const char *fmt, ...) {
if (io->error) return;
struct redisCommand *cmd;
robj **argv = NULL;
int argc = 0, flags = 0, j;
va_list ap;

cmd = lookupCommandByCString((char*)cmdname);
if (!cmd) {
serverLog(LL_WARNING,
"Fatal: AOF method for module data type '%s' tried to "
"emit unknown command '%s'",
io->type->name, cmdname);
io->error = 1;
errno = EINVAL;
return;
}


va_start(ap, fmt);
argv = moduleCreateArgvFromUserFormat(cmdname,fmt,&argc,NULL,&flags,ap);
va_end(ap);
if (argv == NULL) {
serverLog(LL_WARNING,
"Fatal: AOF method for module data type '%s' tried to "
"call RedisModule_EmitAOF() with wrong format specifiers '%s'",
io->type->name, fmt);
io->error = 1;
errno = EINVAL;
return;
}


if (!io->error && rioWriteBulkCount(io->rio,'*',argc) == 0)
io->error = 1;


for (j = 0; j < argc; j++) {
if (!io->error && rioWriteBulkObject(io->rio,argv[j]) == 0)
io->error = 1;
decrRefCount(argv[j]);
}
zfree(argv);
return;
}





RedisModuleCtx *RM_GetContextFromIO(RedisModuleIO *io) {
if (io->ctx) return io->ctx;
io->ctx = zmalloc(sizeof(RedisModuleCtx));
moduleCreateContext(io->ctx, io->type->module, REDISMODULE_CTX_NONE);
return io->ctx;
}




const RedisModuleString *RM_GetKeyNameFromIO(RedisModuleIO *io) {
return io->key;
}


const RedisModuleString *RM_GetKeyNameFromModuleKey(RedisModuleKey *key) {
return key ? key->key : NULL;
}


int RM_GetDbIdFromModuleKey(RedisModuleKey *key) {
return key ? key->db->id : -1;
}




int RM_GetDbIdFromIO(RedisModuleIO *io) {
return io->dbid;
}











void moduleLogRaw(RedisModule *module, const char *levelstr, const char *fmt, va_list ap) {
char msg[LOG_MAX_LEN];
size_t name_len;
int level;

if (!strcasecmp(levelstr,"debug")) level = LL_DEBUG;
else if (!strcasecmp(levelstr,"verbose")) level = LL_VERBOSE;
else if (!strcasecmp(levelstr,"notice")) level = LL_NOTICE;
else if (!strcasecmp(levelstr,"warning")) level = LL_WARNING;
else level = LL_VERBOSE;

if (level < server.verbosity) return;

name_len = snprintf(msg, sizeof(msg),"<%s> ", module? module->name: "module");
vsnprintf(msg + name_len, sizeof(msg) - name_len, fmt, ap);
serverLogRaw(level,msg);
}



















void RM_Log(RedisModuleCtx *ctx, const char *levelstr, const char *fmt, ...) {
va_list ap;
va_start(ap, fmt);
moduleLogRaw(ctx? ctx->module: NULL,levelstr,fmt,ap);
va_end(ap);
}






void RM_LogIOError(RedisModuleIO *io, const char *levelstr, const char *fmt, ...) {
va_list ap;
va_start(ap, fmt);
moduleLogRaw(io->type->module,levelstr,fmt,ap);
va_end(ap);
}









void RM__Assert(const char *estr, const char *file, int line) {
_serverAssert(estr, file, line);
}




void RM_LatencyAddSample(const char *event, mstime_t latency) {
if (latency >= server.latency_monitor_threshold)
latencyAddSample(event, latency);
}




















void unblockClientFromModule(client *c) {
RedisModuleBlockedClient *bc = c->bpop.module_blocked_handle;





if (bc->disconnect_callback) {
RedisModuleCtx ctx;
moduleCreateContext(&ctx, bc->module, REDISMODULE_CTX_NONE);
ctx.blocked_privdata = bc->privdata;
ctx.client = bc->client;
bc->disconnect_callback(&ctx,bc);
moduleFreeContext(&ctx);
}
















if (bc->blocked_on_keys && !bc->unblocked)
moduleUnblockClient(c);

bc->client = NULL;
}





















RedisModuleBlockedClient *moduleBlockClient(RedisModuleCtx *ctx, RedisModuleCmdFunc reply_callback, RedisModuleCmdFunc timeout_callback, void (*free_privdata)(RedisModuleCtx*,void*), long long timeout_ms, RedisModuleString **keys, int numkeys, void *privdata) {
client *c = ctx->client;
int islua = server.in_script;
int ismulti = server.in_exec;

c->bpop.module_blocked_handle = zmalloc(sizeof(RedisModuleBlockedClient));
RedisModuleBlockedClient *bc = c->bpop.module_blocked_handle;
ctx->module->blocked_clients++;





mstime_t timeout = timeout_ms ? (mstime()+timeout_ms) : 0;
bc->client = (islua || ismulti) ? NULL : c;
bc->module = ctx->module;
bc->reply_callback = reply_callback;
bc->timeout_callback = timeout_callback;
bc->disconnect_callback = NULL;
bc->free_privdata = free_privdata;
bc->privdata = privdata;
bc->reply_client = moduleAllocTempClient();
bc->thread_safe_ctx_client = moduleAllocTempClient();
if (bc->client)
bc->reply_client->resp = bc->client->resp;
bc->dbid = c->db->id;
bc->blocked_on_keys = keys != NULL;
bc->unblocked = 0;
bc->background_timer = 0;
bc->background_duration = 0;
c->bpop.timeout = timeout;

if (islua || ismulti) {
c->bpop.module_blocked_handle = NULL;
addReplyError(c, islua ?
"Blocking module command called from Lua script" :
"Blocking module command called from transaction");
} else {
if (keys) {
blockForKeys(c,BLOCKED_MODULE,keys,numkeys,-1,timeout,NULL,NULL,NULL);
} else {
blockClient(c,BLOCKED_MODULE);
}
}
return bc;
}








int moduleTryServeClientBlockedOnKey(client *c, robj *key) {
int served = 0;
RedisModuleBlockedClient *bc = c->bpop.module_blocked_handle;




if (bc->unblocked) return 0;

RedisModuleCtx ctx;
moduleCreateContext(&ctx, bc->module, REDISMODULE_CTX_BLOCKED_REPLY);
ctx.blocked_ready_key = key;
ctx.blocked_privdata = bc->privdata;
ctx.client = bc->client;
ctx.blocked_client = bc;
if (bc->reply_callback(&ctx,(void**)c->argv,c->argc) == REDISMODULE_OK)
served = 1;
moduleFreeContext(&ctx);
return served;
}








































RedisModuleBlockedClient *RM_BlockClient(RedisModuleCtx *ctx, RedisModuleCmdFunc reply_callback, RedisModuleCmdFunc timeout_callback, void (*free_privdata)(RedisModuleCtx*,void*), long long timeout_ms) {
return moduleBlockClient(ctx,reply_callback,timeout_callback,free_privdata,timeout_ms, NULL,0,NULL);
}



























































RedisModuleBlockedClient *RM_BlockClientOnKeys(RedisModuleCtx *ctx, RedisModuleCmdFunc reply_callback, RedisModuleCmdFunc timeout_callback, void (*free_privdata)(RedisModuleCtx*,void*), long long timeout_ms, RedisModuleString **keys, int numkeys, void *privdata) {
return moduleBlockClient(ctx,reply_callback,timeout_callback,free_privdata,timeout_ms, keys,numkeys,privdata);
}






void RM_SignalKeyAsReady(RedisModuleCtx *ctx, RedisModuleString *key) {
signalKeyAsReady(ctx->client->db, key, OBJ_MODULE);
}


int moduleUnblockClientByHandle(RedisModuleBlockedClient *bc, void *privdata) {
pthread_mutex_lock(&moduleUnblockedClientsMutex);
if (!bc->blocked_on_keys) bc->privdata = privdata;
bc->unblocked = 1;
if (listLength(moduleUnblockedClients) == 0) {
if (write(server.module_pipe[1],"A",1) != 1) {

}
}
listAddNodeTail(moduleUnblockedClients,bc);
pthread_mutex_unlock(&moduleUnblockedClientsMutex);
return REDISMODULE_OK;
}



void moduleUnblockClient(client *c) {
RedisModuleBlockedClient *bc = c->bpop.module_blocked_handle;
moduleUnblockClientByHandle(bc,NULL);
}



int moduleClientIsBlockedOnKeys(client *c) {
RedisModuleBlockedClient *bc = c->bpop.module_blocked_handle;
return bc->blocked_on_keys;
}




















int RM_UnblockClient(RedisModuleBlockedClient *bc, void *privdata) {
if (bc->blocked_on_keys) {


if (bc->timeout_callback == NULL) return REDISMODULE_ERR;
if (bc->unblocked) return REDISMODULE_OK;
if (bc->client) moduleBlockedClientTimedOut(bc->client);
}
moduleUnblockClientByHandle(bc,privdata);
return REDISMODULE_OK;
}



int RM_AbortBlock(RedisModuleBlockedClient *bc) {
bc->reply_callback = NULL;
bc->disconnect_callback = NULL;
return RM_UnblockClient(bc,NULL);
}

















void RM_SetDisconnectCallback(RedisModuleBlockedClient *bc, RedisModuleDisconnectFunc callback) {
bc->disconnect_callback = callback;
}









void moduleHandleBlockedClients(void) {
listNode *ln;
RedisModuleBlockedClient *bc;

pthread_mutex_lock(&moduleUnblockedClientsMutex);
while (listLength(moduleUnblockedClients)) {
ln = listFirst(moduleUnblockedClients);
bc = ln->value;
client *c = bc->client;
listDelNode(moduleUnblockedClients,ln);
pthread_mutex_unlock(&moduleUnblockedClientsMutex);









uint64_t reply_us = 0;
if (c && !bc->blocked_on_keys && bc->reply_callback) {
RedisModuleCtx ctx;
moduleCreateContext(&ctx, bc->module, REDISMODULE_CTX_BLOCKED_REPLY);
ctx.blocked_privdata = bc->privdata;
ctx.blocked_ready_key = NULL;
ctx.client = bc->client;
ctx.blocked_client = bc;
monotime replyTimer;
elapsedStart(&replyTimer);
bc->reply_callback(&ctx,(void**)c->argv,c->argc);
reply_us = elapsedUs(replyTimer);
moduleFreeContext(&ctx);
}




if (c && !bc->blocked_on_keys) {
updateStatsOnUnblock(c, bc->background_duration, reply_us);
}


if (bc->privdata && bc->free_privdata) {
RedisModuleCtx ctx;
int ctx_flags = c == NULL ? REDISMODULE_CTX_BLOCKED_DISCONNECTED : REDISMODULE_CTX_NONE;
moduleCreateContext(&ctx, bc->module, ctx_flags);
ctx.blocked_privdata = bc->privdata;
ctx.client = bc->client;
bc->free_privdata(&ctx,bc->privdata);
moduleFreeContext(&ctx);
}





if (c) AddReplyFromClient(c, bc->reply_client);
moduleReleaseTempClient(bc->reply_client);
moduleReleaseTempClient(bc->thread_safe_ctx_client);

if (c != NULL) {



bc->disconnect_callback = NULL;
unblockClient(c);



if (clientHasPendingReplies(c) &&
!(c->flags & CLIENT_PENDING_WRITE))
{
c->flags |= CLIENT_PENDING_WRITE;
listAddNodeHead(server.clients_pending_write,c);
}
}




bc->module->blocked_clients--;
zfree(bc);


pthread_mutex_lock(&moduleUnblockedClientsMutex);
}
pthread_mutex_unlock(&moduleUnblockedClientsMutex);
}




int moduleBlockedClientMayTimeout(client *c) {
if (c->btype != BLOCKED_MODULE)
return 1;

RedisModuleBlockedClient *bc = c->bpop.module_blocked_handle;
return (bc && bc->timeout_callback != NULL);
}





void moduleBlockedClientTimedOut(client *c) {
RedisModuleBlockedClient *bc = c->bpop.module_blocked_handle;




if (bc->unblocked) return;

RedisModuleCtx ctx;
moduleCreateContext(&ctx, bc->module, REDISMODULE_CTX_BLOCKED_TIMEOUT);
ctx.client = bc->client;
ctx.blocked_client = bc;
ctx.blocked_privdata = bc->privdata;
bc->timeout_callback(&ctx,(void**)c->argv,c->argc);
moduleFreeContext(&ctx);
if (!bc->blocked_on_keys) {
updateStatsOnUnblock(c, bc->background_duration, 0);
}



bc->disconnect_callback = NULL;
}



int RM_IsBlockedReplyRequest(RedisModuleCtx *ctx) {
return (ctx->flags & REDISMODULE_CTX_BLOCKED_REPLY) != 0;
}



int RM_IsBlockedTimeoutRequest(RedisModuleCtx *ctx) {
return (ctx->flags & REDISMODULE_CTX_BLOCKED_TIMEOUT) != 0;
}


void *RM_GetBlockedClientPrivateData(RedisModuleCtx *ctx) {
return ctx->blocked_privdata;
}



RedisModuleString *RM_GetBlockedClientReadyKey(RedisModuleCtx *ctx) {
return ctx->blocked_ready_key;
}





RedisModuleBlockedClient *RM_GetBlockedClientHandle(RedisModuleCtx *ctx) {
return ctx->blocked_client;
}




int RM_BlockedClientDisconnected(RedisModuleCtx *ctx) {
return (ctx->flags & REDISMODULE_CTX_BLOCKED_DISCONNECTED) != 0;
}

























RedisModuleCtx *RM_GetThreadSafeContext(RedisModuleBlockedClient *bc) {
RedisModuleCtx *ctx = zmalloc(sizeof(*ctx));
RedisModule *module = bc ? bc->module : NULL;
int flags = REDISMODULE_CTX_THREAD_SAFE;











if (!bc) flags |= REDISMODULE_CTX_NEW_CLIENT;
moduleCreateContext(ctx, module, flags);




if (bc) {
ctx->blocked_client = bc;
ctx->client = bc->thread_safe_ctx_client;
selectDb(ctx->client,bc->dbid);
if (bc->client) {
ctx->client->id = bc->client->id;
ctx->client->resp = bc->client->resp;
}
}
return ctx;
}






RedisModuleCtx *RM_GetDetachedThreadSafeContext(RedisModuleCtx *ctx) {
RedisModuleCtx *new_ctx = zmalloc(sizeof(*new_ctx));


moduleCreateContext(new_ctx, ctx->module,
REDISMODULE_CTX_THREAD_SAFE|REDISMODULE_CTX_NEW_CLIENT);
return new_ctx;
}


void RM_FreeThreadSafeContext(RedisModuleCtx *ctx) {
moduleFreeContext(ctx);
zfree(ctx);
}

void moduleGILAfterLock() {


serverAssert(server.module_ctx_nesting == 0);


server.module_ctx_nesting++;
}




void RM_ThreadSafeContextLock(RedisModuleCtx *ctx) {
UNUSED(ctx);
moduleAcquireGIL();
moduleGILAfterLock();
}







int RM_ThreadSafeContextTryLock(RedisModuleCtx *ctx) {
UNUSED(ctx);

int res = moduleTryAcquireGIL();
if(res != 0) {
errno = res;
return REDISMODULE_ERR;
}
moduleGILAfterLock();
return REDISMODULE_OK;
}

void moduleGILBeforeUnlock() {



serverAssert(server.module_ctx_nesting == 1);



server.module_ctx_nesting--;
propagatePendingCommands();

if (server.busy_module_yield_flags) {
blockingOperationEnds();
server.busy_module_yield_flags = BUSY_MODULE_YIELD_NONE;
unblockPostponedClients();
}
}


void RM_ThreadSafeContextUnlock(RedisModuleCtx *ctx) {
UNUSED(ctx);
moduleGILBeforeUnlock();
moduleReleaseGIL();
}

void moduleAcquireGIL(void) {
pthread_mutex_lock(&moduleGIL);
}

int moduleTryAcquireGIL(void) {
return pthread_mutex_trylock(&moduleGIL);
}

void moduleReleaseGIL(void) {
pthread_mutex_unlock(&moduleGIL);
}






























































int RM_SubscribeToKeyspaceEvents(RedisModuleCtx *ctx, int types, RedisModuleNotificationFunc callback) {
RedisModuleKeyspaceSubscriber *sub = zmalloc(sizeof(*sub));
sub->module = ctx->module;
sub->event_mask = types;
sub->notify_callback = callback;
sub->active = 0;

listAddNodeTail(moduleKeyspaceSubscribers, sub);
return REDISMODULE_OK;
}



int RM_GetNotifyKeyspaceEvents() {
return server.notify_keyspace_events;
}


int RM_NotifyKeyspaceEvent(RedisModuleCtx *ctx, int type, const char *event, RedisModuleString *key) {
if (!ctx || !ctx->client)
return REDISMODULE_ERR;
notifyKeyspaceEvent(type, (char *)event, key, ctx->client->db->id);
return REDISMODULE_OK;
}




void moduleNotifyKeyspaceEvent(int type, const char *event, robj *key, int dbid) {

if (listLength(moduleKeyspaceSubscribers) == 0) return;

listIter li;
listNode *ln;
listRewind(moduleKeyspaceSubscribers,&li);


type &= ~(NOTIFY_KEYEVENT | NOTIFY_KEYSPACE);

while((ln = listNext(&li))) {
RedisModuleKeyspaceSubscriber *sub = ln->value;


if ((sub->event_mask & type) && sub->active == 0) {
RedisModuleCtx ctx;
moduleCreateContext(&ctx, sub->module, REDISMODULE_CTX_TEMP_CLIENT);
selectDb(ctx.client, dbid);




sub->active = 1;
sub->notify_callback(&ctx, type, event, key);
sub->active = 0;
moduleFreeContext(&ctx);
}
}
}


void moduleUnsubscribeNotifications(RedisModule *module) {
listIter li;
listNode *ln;
listRewind(moduleKeyspaceSubscribers,&li);
while((ln = listNext(&li))) {
RedisModuleKeyspaceSubscriber *sub = ln->value;
if (sub->module == module) {
listDelNode(moduleKeyspaceSubscribers, ln);
zfree(sub);
}
}
}






typedef void (*RedisModuleClusterMessageReceiver)(RedisModuleCtx *ctx, const char *sender_id, uint8_t type, const unsigned char *payload, uint32_t len);




typedef struct moduleClusterReceiver {
uint64_t module_id;
RedisModuleClusterMessageReceiver callback;
struct RedisModule *module;
struct moduleClusterReceiver *next;
} moduleClusterReceiver;

typedef struct moduleClusterNodeInfo {
int flags;
char ip[NET_IP_STR_LEN];
int port;
char master_id[40];
} mdouleClusterNodeInfo;



static moduleClusterReceiver *clusterReceivers[UINT8_MAX];


void moduleCallClusterReceivers(const char *sender_id, uint64_t module_id, uint8_t type, const unsigned char *payload, uint32_t len) {
moduleClusterReceiver *r = clusterReceivers[type];
while(r) {
if (r->module_id == module_id) {
RedisModuleCtx ctx;
moduleCreateContext(&ctx, r->module, REDISMODULE_CTX_TEMP_CLIENT);
selectDb(ctx.client, 0);
r->callback(&ctx,sender_id,type,payload,len);
moduleFreeContext(&ctx);
return;
}
r = r->next;
}
}






void RM_RegisterClusterMessageReceiver(RedisModuleCtx *ctx, uint8_t type, RedisModuleClusterMessageReceiver callback) {
if (!server.cluster_enabled) return;

uint64_t module_id = moduleTypeEncodeId(ctx->module->name,0);
moduleClusterReceiver *r = clusterReceivers[type], *prev = NULL;
while(r) {
if (r->module_id == module_id) {

if (callback) {
r->callback = callback;
} else {



if (prev)
prev->next = r->next;
else
clusterReceivers[type]->next = r->next;
zfree(r);
}
return;
}
prev = r;
r = r->next;
}


if (callback) {
r = zmalloc(sizeof(*r));
r->module_id = module_id;
r->module = ctx->module;
r->callback = callback;
r->next = clusterReceivers[type];
clusterReceivers[type] = r;
}
}








int RM_SendClusterMessage(RedisModuleCtx *ctx, const char *target_id, uint8_t type, const char *msg, uint32_t len) {
if (!server.cluster_enabled) return REDISMODULE_ERR;
uint64_t module_id = moduleTypeEncodeId(ctx->module->name,0);
if (clusterSendModuleMessageToTarget(target_id,module_id,type,msg,len) == C_OK)
return REDISMODULE_OK;
else
return REDISMODULE_ERR;
}























char **RM_GetClusterNodesList(RedisModuleCtx *ctx, size_t *numnodes) {
UNUSED(ctx);

if (!server.cluster_enabled) return NULL;
size_t count = dictSize(server.cluster->nodes);
char **ids = zmalloc((count+1)*REDISMODULE_NODE_ID_LEN);
dictIterator *di = dictGetIterator(server.cluster->nodes);
dictEntry *de;
int j = 0;
while((de = dictNext(di)) != NULL) {
clusterNode *node = dictGetVal(de);
if (node->flags & (CLUSTER_NODE_NOADDR|CLUSTER_NODE_HANDSHAKE)) continue;
ids[j] = zmalloc(REDISMODULE_NODE_ID_LEN);
memcpy(ids[j],node->name,REDISMODULE_NODE_ID_LEN);
j++;
}
*numnodes = j;
ids[j] = NULL;

dictReleaseIterator(di);
return ids;
}


void RM_FreeClusterNodesList(char **ids) {
if (ids == NULL) return;
for (int j = 0; ids[j]; j++) zfree(ids[j]);
zfree(ids);
}



const char *RM_GetMyClusterID(void) {
if (!server.cluster_enabled) return NULL;
return server.cluster->myself->name;
}





size_t RM_GetClusterSize(void) {
if (!server.cluster_enabled) return 0;
return dictSize(server.cluster->nodes);
}




















int RM_GetClusterNodeInfo(RedisModuleCtx *ctx, const char *id, char *ip, char *master_id, int *port, int *flags) {
UNUSED(ctx);

clusterNode *node = clusterLookupNode(id);
if (node == NULL ||
node->flags & (CLUSTER_NODE_NOADDR|CLUSTER_NODE_HANDSHAKE))
{
return REDISMODULE_ERR;
}

if (ip) strncpy(ip,node->ip,NET_IP_STR_LEN);

if (master_id) {



if (node->flags & CLUSTER_NODE_SLAVE && node->slaveof)
memcpy(master_id,node->slaveof->name,REDISMODULE_NODE_ID_LEN);
else
memset(master_id,0,REDISMODULE_NODE_ID_LEN);
}
if (port) *port = node->port;



if (flags) {
*flags = 0;
if (node->flags & CLUSTER_NODE_MYSELF) *flags |= REDISMODULE_NODE_MYSELF;
if (node->flags & CLUSTER_NODE_MASTER) *flags |= REDISMODULE_NODE_MASTER;
if (node->flags & CLUSTER_NODE_SLAVE) *flags |= REDISMODULE_NODE_SLAVE;
if (node->flags & CLUSTER_NODE_PFAIL) *flags |= REDISMODULE_NODE_PFAIL;
if (node->flags & CLUSTER_NODE_FAIL) *flags |= REDISMODULE_NODE_FAIL;
if (node->flags & CLUSTER_NODE_NOFAILOVER) *flags |= REDISMODULE_NODE_NOFAILOVER;
}
return REDISMODULE_OK;
}



















void RM_SetClusterFlags(RedisModuleCtx *ctx, uint64_t flags) {
UNUSED(ctx);
if (flags & REDISMODULE_CLUSTER_FLAG_NO_FAILOVER)
server.cluster_module_flags |= CLUSTER_MODULE_FLAG_NO_FAILOVER;
if (flags & REDISMODULE_CLUSTER_FLAG_NO_REDIRECTION)
server.cluster_module_flags |= CLUSTER_MODULE_FLAG_NO_REDIRECTION;
}




















static rax *Timers;
long long aeTimer = -1;

typedef void (*RedisModuleTimerProc)(RedisModuleCtx *ctx, void *data);


typedef struct RedisModuleTimer {
RedisModule *module;
RedisModuleTimerProc callback;
void *data;
int dbid;
} RedisModuleTimer;



int moduleTimerHandler(struct aeEventLoop *eventLoop, long long id, void *clientData) {
UNUSED(eventLoop);
UNUSED(id);
UNUSED(clientData);


raxIterator ri;
raxStart(&ri,Timers);
uint64_t now = ustime();
long long next_period = 0;
while(1) {
raxSeek(&ri,"^",NULL,0);
if (!raxNext(&ri)) break;
uint64_t expiretime;
memcpy(&expiretime,ri.key,sizeof(expiretime));
expiretime = ntohu64(expiretime);
if (now >= expiretime) {
RedisModuleTimer *timer = ri.data;
RedisModuleCtx ctx;
moduleCreateContext(&ctx,timer->module,REDISMODULE_CTX_TEMP_CLIENT);
selectDb(ctx.client, timer->dbid);
timer->callback(&ctx,timer->data);
moduleFreeContext(&ctx);
raxRemove(Timers,(unsigned char*)ri.key,ri.key_len,NULL);
zfree(timer);
} else {






next_period = ((long long)expiretime-ustime())/1000;
break;
}
}
raxStop(&ri);


if (next_period <= 0) next_period = 1;
if (raxSize(Timers) > 0) {
return next_period;
} else {
aeTimer = -1;
return AE_NOMORE;
}
}













RedisModuleTimerID RM_CreateTimer(RedisModuleCtx *ctx, mstime_t period, RedisModuleTimerProc callback, void *data) {
RedisModuleTimer *timer = zmalloc(sizeof(*timer));
timer->module = ctx->module;
timer->callback = callback;
timer->data = data;
timer->dbid = ctx->client ? ctx->client->db->id : 0;
uint64_t expiretime = ustime()+period*1000;
uint64_t key;

while(1) {
key = htonu64(expiretime);
if (raxFind(Timers, (unsigned char*)&key,sizeof(key)) == raxNotFound) {
raxInsert(Timers,(unsigned char*)&key,sizeof(key),timer,NULL);
break;
} else {
expiretime++;
}
}





if (aeTimer != -1) {
raxIterator ri;
raxStart(&ri,Timers);
raxSeek(&ri,"^",NULL,0);
raxNext(&ri);
if (memcmp(ri.key,&key,sizeof(key)) == 0) {


aeDeleteTimeEvent(server.el,aeTimer);
aeTimer = -1;
}
raxStop(&ri);
}



if (aeTimer == -1)
aeTimer = aeCreateTimeEvent(server.el,period,moduleTimerHandler,NULL,NULL);

return key;
}





int RM_StopTimer(RedisModuleCtx *ctx, RedisModuleTimerID id, void **data) {
RedisModuleTimer *timer = raxFind(Timers,(unsigned char*)&id,sizeof(id));
if (timer == raxNotFound || timer->module != ctx->module)
return REDISMODULE_ERR;
if (data) *data = timer->data;
raxRemove(Timers,(unsigned char*)&id,sizeof(id),NULL);
zfree(timer);
return REDISMODULE_OK;
}







int RM_GetTimerInfo(RedisModuleCtx *ctx, RedisModuleTimerID id, uint64_t *remaining, void **data) {
RedisModuleTimer *timer = raxFind(Timers,(unsigned char*)&id,sizeof(id));
if (timer == raxNotFound || timer->module != ctx->module)
return REDISMODULE_ERR;
if (remaining) {
int64_t rem = ntohu64(id)-ustime();
if (rem < 0) rem = 0;
*remaining = rem/1000;
}
if (data) *data = timer->data;
return REDISMODULE_OK;
}





typedef struct EventLoopData {
RedisModuleEventLoopFunc rFunc;
RedisModuleEventLoopFunc wFunc;
void *user_data;
} EventLoopData;

typedef struct EventLoopOneShot {
RedisModuleEventLoopOneShotFunc func;
void *user_data;
} EventLoopOneShot;

list *moduleEventLoopOneShots;
static pthread_mutex_t moduleEventLoopMutex = PTHREAD_MUTEX_INITIALIZER;

static int eventLoopToAeMask(int mask) {
int aeMask = 0;
if (mask & REDISMODULE_EVENTLOOP_READABLE)
aeMask |= AE_READABLE;
if (mask & REDISMODULE_EVENTLOOP_WRITABLE)
aeMask |= AE_WRITABLE;
return aeMask;
}

static int eventLoopFromAeMask(int ae_mask) {
int mask = 0;
if (ae_mask & AE_READABLE)
mask |= REDISMODULE_EVENTLOOP_READABLE;
if (ae_mask & AE_WRITABLE)
mask |= REDISMODULE_EVENTLOOP_WRITABLE;
return mask;
}

static void eventLoopCbReadable(struct aeEventLoop *ae, int fd, void *user_data, int ae_mask) {
UNUSED(ae);
EventLoopData *data = user_data;
data->rFunc(fd, data->user_data, eventLoopFromAeMask(ae_mask));
}

static void eventLoopCbWritable(struct aeEventLoop *ae, int fd, void *user_data, int ae_mask) {
UNUSED(ae);
EventLoopData *data = user_data;
data->wFunc(fd, data->user_data, eventLoopFromAeMask(ae_mask));
}


























int RM_EventLoopAdd(int fd, int mask, RedisModuleEventLoopFunc func, void *user_data) {
if (fd < 0 || fd >= aeGetSetSize(server.el)) {
errno = ERANGE;
return REDISMODULE_ERR;
}

if (!func || mask & ~(REDISMODULE_EVENTLOOP_READABLE |
REDISMODULE_EVENTLOOP_WRITABLE)) {
errno = EINVAL;
return REDISMODULE_ERR;
}











EventLoopData *data = aeGetFileClientData(server.el, fd);
if (!data)
data = zcalloc(sizeof(*data));

aeFileProc *aeProc;
if (mask & REDISMODULE_EVENTLOOP_READABLE)
aeProc = eventLoopCbReadable;
else
aeProc = eventLoopCbWritable;

int aeMask = eventLoopToAeMask(mask);

if (aeCreateFileEvent(server.el, fd, aeMask, aeProc, data) != AE_OK) {
if (aeGetFileEvents(server.el, fd) == AE_NONE)
zfree(data);
return REDISMODULE_ERR;
}

data->user_data = user_data;
if (mask & REDISMODULE_EVENTLOOP_READABLE)
data->rFunc = func;
if (mask & REDISMODULE_EVENTLOOP_WRITABLE)
data->wFunc = func;

errno = 0;
return REDISMODULE_OK;
}















int RM_EventLoopDel(int fd, int mask) {
if (fd < 0 || fd >= aeGetSetSize(server.el)) {
errno = ERANGE;
return REDISMODULE_ERR;
}

if (mask & ~(REDISMODULE_EVENTLOOP_READABLE |
REDISMODULE_EVENTLOOP_WRITABLE)) {
errno = EINVAL;
return REDISMODULE_ERR;
}



EventLoopData *data = aeGetFileClientData(server.el, fd);
aeDeleteFileEvent(server.el, fd, eventLoopToAeMask(mask));
if (aeGetFileEvents(server.el, fd) == AE_NONE)
zfree(data);

errno = 0;
return REDISMODULE_OK;
}





int RM_EventLoopAddOneShot(RedisModuleEventLoopOneShotFunc func, void *user_data) {
if (!func) {
errno = EINVAL;
return REDISMODULE_ERR;
}

EventLoopOneShot *oneshot = zmalloc(sizeof(*oneshot));
oneshot->func = func;
oneshot->user_data = user_data;

pthread_mutex_lock(&moduleEventLoopMutex);
if (!moduleEventLoopOneShots) moduleEventLoopOneShots = listCreate();
listAddNodeTail(moduleEventLoopOneShots, oneshot);
pthread_mutex_unlock(&moduleEventLoopMutex);

if (write(server.module_pipe[1],"A",1) != 1) {

}

errno = 0;
return REDISMODULE_OK;
}



static void eventLoopHandleOneShotEvents() {
pthread_mutex_lock(&moduleEventLoopMutex);
if (moduleEventLoopOneShots) {
while (listLength(moduleEventLoopOneShots)) {
listNode *ln = listFirst(moduleEventLoopOneShots);
EventLoopOneShot *oneshot = ln->value;
listDelNode(moduleEventLoopOneShots, ln);


pthread_mutex_unlock(&moduleEventLoopMutex);
oneshot->func(oneshot->user_data);
zfree(oneshot);

pthread_mutex_lock(&moduleEventLoopMutex);
}
}
pthread_mutex_unlock(&moduleEventLoopMutex);
}













void moduleNotifyUserChanged(client *c) {
if (c->auth_callback) {
c->auth_callback(c->id, c->auth_callback_privdata);




c->auth_callback = NULL;
c->auth_callback_privdata = NULL;
c->auth_module = NULL;
}
}

void revokeClientAuthentication(client *c) {





moduleNotifyUserChanged(c);

c->user = DefaultUser;
c->authenticated = 0;


if (c == server.current_client) {
c->flags |= CLIENT_CLOSE_AFTER_COMMAND;
} else {
freeClientAsync(c);
}
}




static void moduleFreeAuthenticatedClients(RedisModule *module) {
listIter li;
listNode *ln;
listRewind(server.clients,&li);
while ((ln = listNext(&li)) != NULL) {
client *c = listNodeValue(ln);
if (!c->auth_module) continue;

RedisModule *auth_module = (RedisModule *) c->auth_module;
if (auth_module == module) {
revokeClientAuthentication(c);
}
}
}





















RedisModuleUser *RM_CreateModuleUser(const char *name) {
RedisModuleUser *new_user = zmalloc(sizeof(RedisModuleUser));
new_user->user = ACLCreateUnlinkedUser();
new_user->free_user = 1;


sdsfree(new_user->user->name);
new_user->user->name = sdsnew(name);
return new_user;
}



int RM_FreeModuleUser(RedisModuleUser *user) {
if (user->free_user)
ACLFreeUserAndKillClients(user->user);
zfree(user);
return REDISMODULE_OK;
}








int RM_SetModuleUserACL(RedisModuleUser *user, const char* acl) {
return ACLSetUser(user->user, acl, -1);
}







RedisModuleString *RM_GetCurrentUserName(RedisModuleCtx *ctx) {
return RM_CreateString(ctx,ctx->client->user->name,sdslen(ctx->client->user->name));
}












RedisModuleUser *RM_GetModuleUserFromUserName(RedisModuleString *name) {

user *acl_user = ACLGetUserByName(name->ptr, sdslen(name->ptr));
if (acl_user == NULL) {
return NULL;
}

RedisModuleUser *new_user = zmalloc(sizeof(RedisModuleUser));
new_user->user = acl_user;
new_user->free_user = 0;
return new_user;
}









int RM_ACLCheckCommandPermissions(RedisModuleUser *user, RedisModuleString **argv, int argc) {
int keyidxptr;
struct redisCommand *cmd;


if ((cmd = lookupCommand(argv, argc)) == NULL) {
errno = ENOENT;
return REDISMODULE_ERR;
}

if (ACLCheckAllUserCommandPerm(user->user, cmd, argv, argc, &keyidxptr) != ACL_OK) {
errno = EACCES;
return REDISMODULE_ERR;
}

return REDISMODULE_OK;
}













int RM_ACLCheckKeyPermissions(RedisModuleUser *user, RedisModuleString *key, int flags) {
int acl_flags = 0;
if (flags & REDISMODULE_KEY_PERMISSION_READ) acl_flags |= ACL_READ_PERMISSION;
if (flags & REDISMODULE_KEY_PERMISSION_WRITE) acl_flags |= ACL_WRITE_PERMISSION;
if (!acl_flags || ((flags & REDISMODULE_KEY_PERMISSION_ALL) != flags)) {
errno = EINVAL;
return REDISMODULE_ERR;
}

if (ACLUserCheckKeyPerm(user->user, key->ptr, sdslen(key->ptr), acl_flags) != ACL_OK) {
errno = EACCES;
return REDISMODULE_ERR;
}

return REDISMODULE_OK;
}







int RM_ACLCheckChannelPermissions(RedisModuleUser *user, RedisModuleString *ch, int literal) {
if (ACLUserCheckChannelPerm(user->user, ch->ptr, literal) != ACL_OK)
return REDISMODULE_ERR;

return REDISMODULE_OK;
}





void RM_ACLAddLogEntry(RedisModuleCtx *ctx, RedisModuleUser *user, RedisModuleString *object) {
addACLLogEntry(ctx->client, 0, ACL_LOG_CTX_MODULE, -1, user->user->name, sdsdup(object->ptr));
}





















static int authenticateClientWithUser(RedisModuleCtx *ctx, user *user, RedisModuleUserChangedFunc callback, void *privdata, uint64_t *client_id) {
if (user->flags & USER_FLAG_DISABLED) {
return REDISMODULE_ERR;
}


if (!ctx->client || (ctx->client->flags & CLIENT_MODULE)) {
return REDISMODULE_ERR;
}

moduleNotifyUserChanged(ctx->client);

ctx->client->user = user;
ctx->client->authenticated = 1;

if (callback) {
ctx->client->auth_callback = callback;
ctx->client->auth_callback_privdata = privdata;
ctx->client->auth_module = ctx->module;
}

if (client_id) {
*client_id = ctx->client->id;
}

return REDISMODULE_OK;
}







int RM_AuthenticateClientWithUser(RedisModuleCtx *ctx, RedisModuleUser *module_user, RedisModuleUserChangedFunc callback, void *privdata, uint64_t *client_id) {
return authenticateClientWithUser(ctx, module_user->user, callback, privdata, client_id);
}






int RM_AuthenticateClientWithACLUser(RedisModuleCtx *ctx, const char *name, size_t len, RedisModuleUserChangedFunc callback, void *privdata, uint64_t *client_id) {
user *acl_user = ACLGetUserByName(name, len);

if (!acl_user) {
return REDISMODULE_ERR;
}
return authenticateClientWithUser(ctx, acl_user, callback, privdata, client_id);
}













int RM_DeauthenticateAndCloseClient(RedisModuleCtx *ctx, uint64_t client_id) {
UNUSED(ctx);
client *c = lookupClientByID(client_id);
if (c == NULL) return REDISMODULE_ERR;


revokeClientAuthentication(c);
return REDISMODULE_OK;
}













RedisModuleString *RM_GetClientCertificate(RedisModuleCtx *ctx, uint64_t client_id) {
client *c = lookupClientByID(client_id);
if (c == NULL) return NULL;

sds cert = connTLSGetPeerCert(c->conn);
if (!cert) return NULL;

RedisModuleString *s = createObject(OBJ_STRING, cert);
if (ctx != NULL) autoMemoryAdd(ctx, REDISMODULE_AM_STRING, s);

return s;
}






















RedisModuleDict *RM_CreateDict(RedisModuleCtx *ctx) {
struct RedisModuleDict *d = zmalloc(sizeof(*d));
d->rax = raxNew();
if (ctx != NULL) autoMemoryAdd(ctx,REDISMODULE_AM_DICT,d);
return d;
}




void RM_FreeDict(RedisModuleCtx *ctx, RedisModuleDict *d) {
if (ctx != NULL) autoMemoryFreed(ctx,REDISMODULE_AM_DICT,d);
raxFree(d->rax);
zfree(d);
}


uint64_t RM_DictSize(RedisModuleDict *d) {
return raxSize(d->rax);
}





int RM_DictSetC(RedisModuleDict *d, void *key, size_t keylen, void *ptr) {
int retval = raxTryInsert(d->rax,key,keylen,ptr,NULL);
return (retval == 1) ? REDISMODULE_OK : REDISMODULE_ERR;
}



int RM_DictReplaceC(RedisModuleDict *d, void *key, size_t keylen, void *ptr) {
int retval = raxInsert(d->rax,key,keylen,ptr,NULL);
return (retval == 1) ? REDISMODULE_OK : REDISMODULE_ERR;
}


int RM_DictSet(RedisModuleDict *d, RedisModuleString *key, void *ptr) {
return RM_DictSetC(d,key->ptr,sdslen(key->ptr),ptr);
}


int RM_DictReplace(RedisModuleDict *d, RedisModuleString *key, void *ptr) {
return RM_DictReplaceC(d,key->ptr,sdslen(key->ptr),ptr);
}






void *RM_DictGetC(RedisModuleDict *d, void *key, size_t keylen, int *nokey) {
void *res = raxFind(d->rax,key,keylen);
if (nokey) *nokey = (res == raxNotFound);
return (res == raxNotFound) ? NULL : res;
}


void *RM_DictGet(RedisModuleDict *d, RedisModuleString *key, int *nokey) {
return RM_DictGetC(d,key->ptr,sdslen(key->ptr),nokey);
}








int RM_DictDelC(RedisModuleDict *d, void *key, size_t keylen, void *oldval) {
int retval = raxRemove(d->rax,key,keylen,oldval);
return retval ? REDISMODULE_OK : REDISMODULE_ERR;
}


int RM_DictDel(RedisModuleDict *d, RedisModuleString *key, void *oldval) {
return RM_DictDelC(d,key->ptr,sdslen(key->ptr),oldval);
}





















RedisModuleDictIter *RM_DictIteratorStartC(RedisModuleDict *d, const char *op, void *key, size_t keylen) {
RedisModuleDictIter *di = zmalloc(sizeof(*di));
di->dict = d;
raxStart(&di->ri,d->rax);
raxSeek(&di->ri,op,key,keylen);
return di;
}



RedisModuleDictIter *RM_DictIteratorStart(RedisModuleDict *d, const char *op, RedisModuleString *key) {
return RM_DictIteratorStartC(d,op,key->ptr,sdslen(key->ptr));
}



void RM_DictIteratorStop(RedisModuleDictIter *di) {
raxStop(&di->ri);
zfree(di);
}








int RM_DictIteratorReseekC(RedisModuleDictIter *di, const char *op, void *key, size_t keylen) {
return raxSeek(&di->ri,op,key,keylen);
}



int RM_DictIteratorReseek(RedisModuleDictIter *di, const char *op, RedisModuleString *key) {
return RM_DictIteratorReseekC(di,op,key->ptr,sdslen(key->ptr));
}


























void *RM_DictNextC(RedisModuleDictIter *di, size_t *keylen, void **dataptr) {
if (!raxNext(&di->ri)) return NULL;
if (keylen) *keylen = di->ri.key_len;
if (dataptr) *dataptr = di->ri.data;
return di->ri.key;
}




void *RM_DictPrevC(RedisModuleDictIter *di, size_t *keylen, void **dataptr) {
if (!raxPrev(&di->ri)) return NULL;
if (keylen) *keylen = di->ri.key_len;
if (dataptr) *dataptr = di->ri.data;
return di->ri.key;
}








RedisModuleString *RM_DictNext(RedisModuleCtx *ctx, RedisModuleDictIter *di, void **dataptr) {
size_t keylen;
void *key = RM_DictNextC(di,&keylen,dataptr);
if (key == NULL) return NULL;
return RM_CreateString(ctx,key,keylen);
}




RedisModuleString *RM_DictPrev(RedisModuleCtx *ctx, RedisModuleDictIter *di, void **dataptr) {
size_t keylen;
void *key = RM_DictPrevC(di,&keylen,dataptr);
if (key == NULL) return NULL;
return RM_CreateString(ctx,key,keylen);
}













int RM_DictCompareC(RedisModuleDictIter *di, const char *op, void *key, size_t keylen) {
if (raxEOF(&di->ri)) return REDISMODULE_ERR;
int res = raxCompare(&di->ri,op,key,keylen);
return res ? REDISMODULE_OK : REDISMODULE_ERR;
}



int RM_DictCompare(RedisModuleDictIter *di, const char *op, RedisModuleString *key) {
if (raxEOF(&di->ri)) return REDISMODULE_ERR;
int res = raxCompare(&di->ri,op,key->ptr,sdslen(key->ptr));
return res ? REDISMODULE_OK : REDISMODULE_ERR;
}








int RM_InfoEndDictField(RedisModuleInfoCtx *ctx);





int RM_InfoAddSection(RedisModuleInfoCtx *ctx, const char *name) {
sds full_name = sdsdup(ctx->module->name);
if (name != NULL && strlen(name) > 0)
full_name = sdscatfmt(full_name, "_%s", name);


if (ctx->in_dict_field)
RM_InfoEndDictField(ctx);





if (ctx->requested_section) {
if (strcasecmp(ctx->requested_section, full_name) &&
strcasecmp(ctx->requested_section, ctx->module->name)) {
sdsfree(full_name);
ctx->in_section = 0;
return REDISMODULE_ERR;
}
}
if (ctx->sections++) ctx->info = sdscat(ctx->info,"\r\n");
ctx->info = sdscatfmt(ctx->info, "#%S\r\n", full_name);
ctx->in_section = 1;
sdsfree(full_name);
return REDISMODULE_OK;
}




int RM_InfoBeginDictField(RedisModuleInfoCtx *ctx, const char *name) {
if (!ctx->in_section)
return REDISMODULE_ERR;

if (ctx->in_dict_field)
RM_InfoEndDictField(ctx);
char *tmpmodname, *tmpname;
ctx->info = sdscatfmt(ctx->info,
"%s_%s:",
getSafeInfoString(ctx->module->name, strlen(ctx->module->name), &tmpmodname),
getSafeInfoString(name, strlen(name), &tmpname));
if (tmpmodname != NULL) zfree(tmpmodname);
if (tmpname != NULL) zfree(tmpname);
ctx->in_dict_field = 1;
return REDISMODULE_OK;
}


int RM_InfoEndDictField(RedisModuleInfoCtx *ctx) {
if (!ctx->in_dict_field)
return REDISMODULE_ERR;

if (ctx->info[sdslen(ctx->info)-1]==',')
sdsIncrLen(ctx->info, -1);
ctx->info = sdscat(ctx->info, "\r\n");
ctx->in_dict_field = 0;
return REDISMODULE_OK;
}




int RM_InfoAddFieldString(RedisModuleInfoCtx *ctx, const char *field, RedisModuleString *value) {
if (!ctx->in_section)
return REDISMODULE_ERR;
if (ctx->in_dict_field) {
ctx->info = sdscatfmt(ctx->info,
"%s=%S,",
field,
(sds)value->ptr);
return REDISMODULE_OK;
}
ctx->info = sdscatfmt(ctx->info,
"%s_%s:%S\r\n",
ctx->module->name,
field,
(sds)value->ptr);
return REDISMODULE_OK;
}


int RM_InfoAddFieldCString(RedisModuleInfoCtx *ctx, const char *field, const char *value) {
if (!ctx->in_section)
return REDISMODULE_ERR;
if (ctx->in_dict_field) {
ctx->info = sdscatfmt(ctx->info,
"%s=%s,",
field,
value);
return REDISMODULE_OK;
}
ctx->info = sdscatfmt(ctx->info,
"%s_%s:%s\r\n",
ctx->module->name,
field,
value);
return REDISMODULE_OK;
}


int RM_InfoAddFieldDouble(RedisModuleInfoCtx *ctx, const char *field, double value) {
if (!ctx->in_section)
return REDISMODULE_ERR;
if (ctx->in_dict_field) {
ctx->info = sdscatprintf(ctx->info,
"%s=%.17g,",
field,
value);
return REDISMODULE_OK;
}
ctx->info = sdscatprintf(ctx->info,
"%s_%s:%.17g\r\n",
ctx->module->name,
field,
value);
return REDISMODULE_OK;
}


int RM_InfoAddFieldLongLong(RedisModuleInfoCtx *ctx, const char *field, long long value) {
if (!ctx->in_section)
return REDISMODULE_ERR;
if (ctx->in_dict_field) {
ctx->info = sdscatfmt(ctx->info,
"%s=%I,",
field,
value);
return REDISMODULE_OK;
}
ctx->info = sdscatfmt(ctx->info,
"%s_%s:%I\r\n",
ctx->module->name,
field,
value);
return REDISMODULE_OK;
}


int RM_InfoAddFieldULongLong(RedisModuleInfoCtx *ctx, const char *field, unsigned long long value) {
if (!ctx->in_section)
return REDISMODULE_ERR;
if (ctx->in_dict_field) {
ctx->info = sdscatfmt(ctx->info,
"%s=%U,",
field,
value);
return REDISMODULE_OK;
}
ctx->info = sdscatfmt(ctx->info,
"%s_%s:%U\r\n",
ctx->module->name,
field,
value);
return REDISMODULE_OK;
}



int RM_RegisterInfoFunc(RedisModuleCtx *ctx, RedisModuleInfoFunc cb) {
ctx->module->info_cb = cb;
return REDISMODULE_OK;
}

sds modulesCollectInfo(sds info, const char *section, int for_crash_report, int sections) {
dictIterator *di = dictGetIterator(modules);
dictEntry *de;

while ((de = dictNext(di)) != NULL) {
struct RedisModule *module = dictGetVal(de);
if (!module->info_cb)
continue;
RedisModuleInfoCtx info_ctx = {module, section, info, sections, 0, 0};
module->info_cb(&info_ctx, for_crash_report);

if (info_ctx.in_dict_field)
RM_InfoEndDictField(&info_ctx);
info = info_ctx.info;
sections = info_ctx.sections;
}
dictReleaseIterator(di);
return info;
}







RedisModuleServerInfoData *RM_GetServerInfo(RedisModuleCtx *ctx, const char *section) {
struct RedisModuleServerInfoData *d = zmalloc(sizeof(*d));
d->rax = raxNew();
if (ctx != NULL) autoMemoryAdd(ctx,REDISMODULE_AM_INFO,d);
sds info = genRedisInfoString(section);
int totlines, i;
sds *lines = sdssplitlen(info, sdslen(info), "\r\n", 2, &totlines);
for(i=0; i<totlines; i++) {
sds line = lines[i];
if (line[0]=='#') continue;
char *sep = strchr(line, ':');
if (!sep) continue;
unsigned char *key = (unsigned char*)line;
size_t keylen = (intptr_t)sep-(intptr_t)line;
sds val = sdsnewlen(sep+1,sdslen(line)-((intptr_t)sep-(intptr_t)line)-1);
if (!raxTryInsert(d->rax,key,keylen,val,NULL))
sdsfree(val);
}
sdsfree(info);
sdsfreesplitres(lines,totlines);
return d;
}




void RM_FreeServerInfo(RedisModuleCtx *ctx, RedisModuleServerInfoData *data) {
if (ctx != NULL) autoMemoryFreed(ctx,REDISMODULE_AM_INFO,data);
raxFreeWithCallback(data->rax, (void(*)(void*))sdsfree);
zfree(data);
}





RedisModuleString *RM_ServerInfoGetField(RedisModuleCtx *ctx, RedisModuleServerInfoData *data, const char* field) {
sds val = raxFind(data->rax, (unsigned char *)field, strlen(field));
if (val == raxNotFound) return NULL;
RedisModuleString *o = createStringObject(val,sdslen(val));
if (ctx != NULL) autoMemoryAdd(ctx,REDISMODULE_AM_STRING,o);
return o;
}


const char *RM_ServerInfoGetFieldC(RedisModuleServerInfoData *data, const char* field) {
sds val = raxFind(data->rax, (unsigned char *)field, strlen(field));
if (val == raxNotFound) return NULL;
return val;
}




long long RM_ServerInfoGetFieldSigned(RedisModuleServerInfoData *data, const char* field, int *out_err) {
long long ll;
sds val = raxFind(data->rax, (unsigned char *)field, strlen(field));
if (val == raxNotFound) {
if (out_err) *out_err = REDISMODULE_ERR;
return 0;
}
if (!string2ll(val,sdslen(val),&ll)) {
if (out_err) *out_err = REDISMODULE_ERR;
return 0;
}
if (out_err) *out_err = REDISMODULE_OK;
return ll;
}




unsigned long long RM_ServerInfoGetFieldUnsigned(RedisModuleServerInfoData *data, const char* field, int *out_err) {
unsigned long long ll;
sds val = raxFind(data->rax, (unsigned char *)field, strlen(field));
if (val == raxNotFound) {
if (out_err) *out_err = REDISMODULE_ERR;
return 0;
}
if (!string2ull(val,&ll)) {
if (out_err) *out_err = REDISMODULE_ERR;
return 0;
}
if (out_err) *out_err = REDISMODULE_OK;
return ll;
}




double RM_ServerInfoGetFieldDouble(RedisModuleServerInfoData *data, const char* field, int *out_err) {
double dbl;
sds val = raxFind(data->rax, (unsigned char *)field, strlen(field));
if (val == raxNotFound) {
if (out_err) *out_err = REDISMODULE_ERR;
return 0;
}
if (!string2d(val,sdslen(val),&dbl)) {
if (out_err) *out_err = REDISMODULE_ERR;
return 0;
}
if (out_err) *out_err = REDISMODULE_OK;
return dbl;
}









void RM_GetRandomBytes(unsigned char *dst, size_t len) {
getRandomBytes(dst,len);
}




void RM_GetRandomHexChars(char *dst, size_t len) {
getRandomHexChars(dst,len);
}

















int RM_ExportSharedAPI(RedisModuleCtx *ctx, const char *apiname, void *func) {
RedisModuleSharedAPI *sapi = zmalloc(sizeof(*sapi));
sapi->module = ctx->module;
sapi->func = func;
if (dictAdd(server.sharedapi, (char*)apiname, sapi) != DICT_OK) {
zfree(sapi);
return REDISMODULE_ERR;
}
return REDISMODULE_OK;
}


































void *RM_GetSharedAPI(RedisModuleCtx *ctx, const char *apiname) {
dictEntry *de = dictFind(server.sharedapi, apiname);
if (de == NULL) return NULL;
RedisModuleSharedAPI *sapi = dictGetVal(de);
if (listSearchKey(sapi->module->usedby,ctx->module) == NULL) {
listAddNodeTail(sapi->module->usedby,ctx->module);
listAddNodeTail(ctx->module->using,sapi->module);
}
return sapi->func;
}







int moduleUnregisterSharedAPI(RedisModule *module) {
int count = 0;
dictIterator *di = dictGetSafeIterator(server.sharedapi);
dictEntry *de;
while ((de = dictNext(di)) != NULL) {
const char *apiname = dictGetKey(de);
RedisModuleSharedAPI *sapi = dictGetVal(de);
if (sapi->module == module) {
dictDelete(server.sharedapi,apiname);
zfree(sapi);
count++;
}
}
dictReleaseIterator(di);
return count;
}





int moduleUnregisterUsedAPI(RedisModule *module) {
listIter li;
listNode *ln;
int count = 0;

listRewind(module->using,&li);
while((ln = listNext(&li))) {
RedisModule *used = ln->value;
listNode *ln = listSearchKey(used->usedby,module);
if (ln) {
listDelNode(used->usedby,ln);
count++;
}
}
return count;
}





int moduleUnregisterFilters(RedisModule *module) {
listIter li;
listNode *ln;
int count = 0;

listRewind(module->filters,&li);
while((ln = listNext(&li))) {
RedisModuleCommandFilter *filter = ln->value;
listNode *ln = listSearchKey(moduleCommandFilters,filter);
if (ln) {
listDelNode(moduleCommandFilters,ln);
count++;
}
zfree(filter);
}
return count;
}
























































RedisModuleCommandFilter *RM_RegisterCommandFilter(RedisModuleCtx *ctx, RedisModuleCommandFilterFunc callback, int flags) {
RedisModuleCommandFilter *filter = zmalloc(sizeof(*filter));
filter->module = ctx->module;
filter->callback = callback;
filter->flags = flags;

listAddNodeTail(moduleCommandFilters, filter);
listAddNodeTail(ctx->module->filters, filter);
return filter;
}



int RM_UnregisterCommandFilter(RedisModuleCtx *ctx, RedisModuleCommandFilter *filter) {
listNode *ln;


if (filter->module != ctx->module) return REDISMODULE_ERR;

ln = listSearchKey(moduleCommandFilters,filter);
if (!ln) return REDISMODULE_ERR;
listDelNode(moduleCommandFilters,ln);

ln = listSearchKey(ctx->module->filters,filter);
if (!ln) return REDISMODULE_ERR;
listDelNode(ctx->module->filters,ln);

zfree(filter);

return REDISMODULE_OK;
}

void moduleCallCommandFilters(client *c) {
if (listLength(moduleCommandFilters) == 0) return;

listIter li;
listNode *ln;
listRewind(moduleCommandFilters,&li);

RedisModuleCommandFilterCtx filter = {
.argv = c->argv,
.argc = c->argc
};

while((ln = listNext(&li))) {
RedisModuleCommandFilter *f = ln->value;




if ((f->flags & REDISMODULE_CMDFILTER_NOSELF) && f->module->in_call) continue;


f->callback(&filter);
}

c->argv = filter.argv;
c->argc = filter.argc;
}




int RM_CommandFilterArgsCount(RedisModuleCommandFilterCtx *fctx)
{
return fctx->argc;
}




RedisModuleString *RM_CommandFilterArgGet(RedisModuleCommandFilterCtx *fctx, int pos)
{
if (pos < 0 || pos >= fctx->argc) return NULL;
return fctx->argv[pos];
}






int RM_CommandFilterArgInsert(RedisModuleCommandFilterCtx *fctx, int pos, RedisModuleString *arg)
{
int i;

if (pos < 0 || pos > fctx->argc) return REDISMODULE_ERR;

fctx->argv = zrealloc(fctx->argv, (fctx->argc+1)*sizeof(RedisModuleString *));
for (i = fctx->argc; i > pos; i--) {
fctx->argv[i] = fctx->argv[i-1];
}
fctx->argv[pos] = arg;
fctx->argc++;

return REDISMODULE_OK;
}






int RM_CommandFilterArgReplace(RedisModuleCommandFilterCtx *fctx, int pos, RedisModuleString *arg)
{
if (pos < 0 || pos >= fctx->argc) return REDISMODULE_ERR;

decrRefCount(fctx->argv[pos]);
fctx->argv[pos] = arg;

return REDISMODULE_OK;
}




int RM_CommandFilterArgDelete(RedisModuleCommandFilterCtx *fctx, int pos)
{
int i;
if (pos < 0 || pos >= fctx->argc) return REDISMODULE_ERR;

decrRefCount(fctx->argv[pos]);
for (i = pos; i < fctx->argc-1; i++) {
fctx->argv[i] = fctx->argv[i+1];
}
fctx->argc--;

return REDISMODULE_OK;
}







size_t RM_MallocSize(void* ptr){
return zmalloc_size(ptr);
}









float RM_GetUsedMemoryRatio(){
float level;
getMaxmemoryState(NULL, NULL, NULL, &level);
return level;
}





typedef void (*RedisModuleScanCB)(RedisModuleCtx *ctx, RedisModuleString *keyname, RedisModuleKey *key, void *privdata);
typedef struct {
RedisModuleCtx *ctx;
void* user_data;
RedisModuleScanCB fn;
} ScanCBData;

typedef struct RedisModuleScanCursor{
int cursor;
int done;
}RedisModuleScanCursor;

static void moduleScanCallback(void *privdata, const dictEntry *de) {
ScanCBData *data = privdata;
sds key = dictGetKey(de);
robj* val = dictGetVal(de);
RedisModuleString *keyname = createObject(OBJ_STRING,sdsdup(key));


RedisModuleKey kp = {0};
moduleInitKey(&kp, data->ctx, keyname, val, REDISMODULE_READ);

data->fn(data->ctx, keyname, &kp, data->user_data);

moduleCloseKey(&kp);
decrRefCount(keyname);
}


RedisModuleScanCursor *RM_ScanCursorCreate() {
RedisModuleScanCursor* cursor = zmalloc(sizeof(*cursor));
cursor->cursor = 0;
cursor->done = 0;
return cursor;
}


void RM_ScanCursorRestart(RedisModuleScanCursor *cursor) {
cursor->cursor = 0;
cursor->done = 0;
}


void RM_ScanCursorDestroy(RedisModuleScanCursor *cursor) {
zfree(cursor);
}

























































int RM_Scan(RedisModuleCtx *ctx, RedisModuleScanCursor *cursor, RedisModuleScanCB fn, void *privdata) {
if (cursor->done) {
errno = ENOENT;
return 0;
}
int ret = 1;
ScanCBData data = { ctx, privdata, fn };
cursor->cursor = dictScan(ctx->client->db->dict, cursor->cursor, moduleScanCallback, NULL, &data);
if (cursor->cursor == 0) {
cursor->done = 1;
ret = 0;
}
errno = 0;
return ret;
}

typedef void (*RedisModuleScanKeyCB)(RedisModuleKey *key, RedisModuleString *field, RedisModuleString *value, void *privdata);
typedef struct {
RedisModuleKey *key;
void* user_data;
RedisModuleScanKeyCB fn;
} ScanKeyCBData;

static void moduleScanKeyCallback(void *privdata, const dictEntry *de) {
ScanKeyCBData *data = privdata;
sds key = dictGetKey(de);
robj *o = data->key->value;
robj *field = createStringObject(key, sdslen(key));
robj *value = NULL;
if (o->type == OBJ_SET) {
value = NULL;
} else if (o->type == OBJ_HASH) {
sds val = dictGetVal(de);
value = createStringObject(val, sdslen(val));
} else if (o->type == OBJ_ZSET) {
double *val = (double*)dictGetVal(de);
value = createStringObjectFromLongDouble(*val, 0);
}

data->fn(data->key, field, value, data->user_data);
decrRefCount(field);
if (value) decrRefCount(value);
}

















































int RM_ScanKey(RedisModuleKey *key, RedisModuleScanCursor *cursor, RedisModuleScanKeyCB fn, void *privdata) {
if (key == NULL || key->value == NULL) {
errno = EINVAL;
return 0;
}
dict *ht = NULL;
robj *o = key->value;
if (o->type == OBJ_SET) {
if (o->encoding == OBJ_ENCODING_HT)
ht = o->ptr;
} else if (o->type == OBJ_HASH) {
if (o->encoding == OBJ_ENCODING_HT)
ht = o->ptr;
} else if (o->type == OBJ_ZSET) {
if (o->encoding == OBJ_ENCODING_SKIPLIST)
ht = ((zset *)o->ptr)->dict;
} else {
errno = EINVAL;
return 0;
}
if (cursor->done) {
errno = ENOENT;
return 0;
}
int ret = 1;
if (ht) {
ScanKeyCBData data = { key, privdata, fn };
cursor->cursor = dictScan(ht, cursor->cursor, moduleScanKeyCallback, NULL, &data);
if (cursor->cursor == 0) {
cursor->done = 1;
ret = 0;
}
} else if (o->type == OBJ_SET && o->encoding == OBJ_ENCODING_INTSET) {
int pos = 0;
int64_t ll;
while(intsetGet(o->ptr,pos++,&ll)) {
robj *field = createObject(OBJ_STRING,sdsfromlonglong(ll));
fn(key, field, NULL, privdata);
decrRefCount(field);
}
cursor->cursor = 1;
cursor->done = 1;
ret = 0;
} else if (o->type == OBJ_ZSET) {
unsigned char *p = lpSeek(o->ptr,0);
unsigned char *vstr;
unsigned int vlen;
long long vll;
while(p) {
vstr = lpGetValue(p,&vlen,&vll);
robj *field = (vstr != NULL) ?
createStringObject((char*)vstr,vlen) :
createObject(OBJ_STRING,sdsfromlonglong(vll));
p = lpNext(o->ptr,p);
vstr = lpGetValue(p,&vlen,&vll);
robj *value = (vstr != NULL) ?
createStringObject((char*)vstr,vlen) :
createObject(OBJ_STRING,sdsfromlonglong(vll));
fn(key, field, value, privdata);
p = lpNext(o->ptr,p);
decrRefCount(field);
decrRefCount(value);
}
cursor->cursor = 1;
cursor->done = 1;
ret = 0;
} else if (o->type == OBJ_HASH) {
unsigned char *p = lpFirst(o->ptr);
unsigned char *vstr;
int64_t vlen;
unsigned char intbuf[LP_INTBUF_SIZE];
while(p) {
vstr = lpGet(p,&vlen,intbuf);
robj *field = createStringObject((char*)vstr,vlen);
p = lpNext(o->ptr,p);
vstr = lpGet(p,&vlen,intbuf);
robj *value = createStringObject((char*)vstr,vlen);
fn(key, field, value, privdata);
p = lpNext(o->ptr,p);
decrRefCount(field);
decrRefCount(value);
}
cursor->cursor = 1;
cursor->done = 1;
ret = 0;
}
errno = 0;
return ret;
}

















int RM_Fork(RedisModuleForkDoneHandler cb, void *user_data) {
pid_t childpid;

if ((childpid = redisFork(CHILD_TYPE_MODULE)) == 0) {

redisSetProcTitle("redis-module-fork");
} else if (childpid == -1) {
serverLog(LL_WARNING,"Can't fork for module: %s", strerror(errno));
} else {

moduleForkInfo.done_handler = cb;
moduleForkInfo.done_handler_user_data = user_data;
serverLog(LL_VERBOSE, "Module fork started pid: %ld ", (long) childpid);
}
return childpid;
}





void RM_SendChildHeartbeat(double progress) {
sendChildInfoGeneric(CHILD_INFO_TYPE_CURRENT_INFO, 0, progress, "Module fork");
}




int RM_ExitFromChild(int retcode) {
sendChildCowInfo(CHILD_INFO_TYPE_MODULE_COW_SIZE, "Module fork");
exitFromChild(retcode);
return REDISMODULE_OK;
}




int TerminateModuleForkChild(int child_pid, int wait) {

if (server.child_type != CHILD_TYPE_MODULE ||
server.child_pid != child_pid) return C_ERR;

int statloc;
serverLog(LL_VERBOSE,"Killing running module fork child: %ld",
(long) server.child_pid);
if (kill(server.child_pid,SIGUSR1) != -1 && wait) {
while(waitpid(server.child_pid, &statloc, 0) !=
server.child_pid);
}

resetChildState();
moduleForkInfo.done_handler = NULL;
moduleForkInfo.done_handler_user_data = NULL;
return C_OK;
}



int RM_KillForkChild(int child_pid) {

if (TerminateModuleForkChild(child_pid,1) == C_OK)
return REDISMODULE_OK;
else
return REDISMODULE_ERR;
}

void ModuleForkDoneHandler(int exitcode, int bysignal) {
serverLog(LL_NOTICE,
"Module fork exited pid: %ld, retcode: %d, bysignal: %d",
(long) server.child_pid, exitcode, bysignal);
if (moduleForkInfo.done_handler) {
moduleForkInfo.done_handler(exitcode, bysignal,
moduleForkInfo.done_handler_user_data);
}

moduleForkInfo.done_handler = NULL;
moduleForkInfo.done_handler_user_data = NULL;
}









static uint64_t moduleEventVersions[] = {
REDISMODULE_REPLICATIONINFO_VERSION,
-1,
REDISMODULE_FLUSHINFO_VERSION,
-1,
REDISMODULE_CLIENTINFO_VERSION,
-1,
-1,
-1,
REDISMODULE_CRON_LOOP_VERSION,
REDISMODULE_MODULE_CHANGE_VERSION,
REDISMODULE_LOADING_PROGRESS_VERSION,
REDISMODULE_SWAPDBINFO_VERSION,
-1,
-1,
-1,
-1,
};






































































































































































































































































int RM_SubscribeToServerEvent(RedisModuleCtx *ctx, RedisModuleEvent event, RedisModuleEventCallback callback) {
RedisModuleEventListener *el;


if (ctx->module == NULL) return REDISMODULE_ERR;
if (event.id >= _REDISMODULE_EVENT_NEXT) return REDISMODULE_ERR;
if (event.dataver > moduleEventVersions[event.id]) return REDISMODULE_ERR;


listIter li;
listNode *ln;
listRewind(RedisModule_EventListeners,&li);
while((ln = listNext(&li))) {
el = ln->value;
if (el->module == ctx->module && el->event.id == event.id)
break;
}


if (ln) {
if (callback == NULL) {
listDelNode(RedisModule_EventListeners,ln);
zfree(el);
} else {
el->callback = callback;
}
return REDISMODULE_OK;
}


el = zmalloc(sizeof(*el));
el->module = ctx->module;
el->event = event;
el->callback = callback;
listAddNodeTail(RedisModule_EventListeners,el);
return REDISMODULE_OK;
}





int RM_IsSubEventSupported(RedisModuleEvent event, int64_t subevent) {
switch (event.id) {
case REDISMODULE_EVENT_REPLICATION_ROLE_CHANGED:
return subevent < _REDISMODULE_EVENT_REPLROLECHANGED_NEXT;
case REDISMODULE_EVENT_PERSISTENCE:
return subevent < _REDISMODULE_SUBEVENT_PERSISTENCE_NEXT;
case REDISMODULE_EVENT_FLUSHDB:
return subevent < _REDISMODULE_SUBEVENT_FLUSHDB_NEXT;
case REDISMODULE_EVENT_LOADING:
return subevent < _REDISMODULE_SUBEVENT_LOADING_NEXT;
case REDISMODULE_EVENT_CLIENT_CHANGE:
return subevent < _REDISMODULE_SUBEVENT_CLIENT_CHANGE_NEXT;
case REDISMODULE_EVENT_SHUTDOWN:
return subevent < _REDISMODULE_SUBEVENT_SHUTDOWN_NEXT;
case REDISMODULE_EVENT_REPLICA_CHANGE:
return subevent < _REDISMODULE_EVENT_REPLROLECHANGED_NEXT;
case REDISMODULE_EVENT_MASTER_LINK_CHANGE:
return subevent < _REDISMODULE_SUBEVENT_MASTER_NEXT;
case REDISMODULE_EVENT_CRON_LOOP:
return subevent < _REDISMODULE_SUBEVENT_CRON_LOOP_NEXT;
case REDISMODULE_EVENT_MODULE_CHANGE:
return subevent < _REDISMODULE_SUBEVENT_MODULE_NEXT;
case REDISMODULE_EVENT_LOADING_PROGRESS:
return subevent < _REDISMODULE_SUBEVENT_LOADING_PROGRESS_NEXT;
case REDISMODULE_EVENT_SWAPDB:
return subevent < _REDISMODULE_SUBEVENT_SWAPDB_NEXT;
case REDISMODULE_EVENT_REPL_ASYNC_LOAD:
return subevent < _REDISMODULE_SUBEVENT_REPL_ASYNC_LOAD_NEXT;
case REDISMODULE_EVENT_FORK_CHILD:
return subevent < _REDISMODULE_SUBEVENT_FORK_CHILD_NEXT;
case REDISMODULE_EVENT_EVENTLOOP:
return subevent < _REDISMODULE_SUBEVENT_EVENTLOOP_NEXT;
default:
break;
}
return 0;
}








void moduleFireServerEvent(uint64_t eid, int subid, void *data) {



if (listLength(RedisModule_EventListeners) == 0) return;

listIter li;
listNode *ln;
listRewind(RedisModule_EventListeners,&li);
while((ln = listNext(&li))) {
RedisModuleEventListener *el = ln->value;
if (el->event.id == eid) {
RedisModuleCtx ctx;
if (eid == REDISMODULE_EVENT_CLIENT_CHANGE) {





moduleCreateContext(&ctx,el->module,REDISMODULE_CTX_NONE);
ctx.client = (client *) data;
} else {
moduleCreateContext(&ctx,el->module,REDISMODULE_CTX_TEMP_CLIENT);
}

void *moduledata = NULL;
RedisModuleClientInfoV1 civ1;
RedisModuleReplicationInfoV1 riv1;
RedisModuleModuleChangeV1 mcv1;




selectDb(ctx.client, 0);


if (eid == REDISMODULE_EVENT_CLIENT_CHANGE) {
serverAssert(modulePopulateClientInfoStructure(&civ1,data, el->event.dataver) == REDISMODULE_OK);
moduledata = &civ1;
} else if (eid == REDISMODULE_EVENT_REPLICATION_ROLE_CHANGED) {
serverAssert(modulePopulateReplicationInfoStructure(&riv1,el->event.dataver) == REDISMODULE_OK);
moduledata = &riv1;
} else if (eid == REDISMODULE_EVENT_FLUSHDB) {
moduledata = data;
RedisModuleFlushInfoV1 *fi = data;
if (fi->dbnum != -1)
selectDb(ctx.client, fi->dbnum);
} else if (eid == REDISMODULE_EVENT_MODULE_CHANGE) {
RedisModule *m = data;
if (m == el->module) {
moduleFreeContext(&ctx);
continue;
}
mcv1.version = REDISMODULE_MODULE_CHANGE_VERSION;
mcv1.module_name = m->name;
mcv1.module_version = m->ver;
moduledata = &mcv1;
} else if (eid == REDISMODULE_EVENT_LOADING_PROGRESS) {
moduledata = data;
} else if (eid == REDISMODULE_EVENT_CRON_LOOP) {
moduledata = data;
} else if (eid == REDISMODULE_EVENT_SWAPDB) {
moduledata = data;
}

el->module->in_hook++;
el->callback(&ctx,el->event,subid,moduledata);
el->module->in_hook--;

moduleFreeContext(&ctx);
}
}
}



void moduleUnsubscribeAllServerEvents(RedisModule *module) {
RedisModuleEventListener *el;
listIter li;
listNode *ln;
listRewind(RedisModule_EventListeners,&li);

while((ln = listNext(&li))) {
el = ln->value;
if (el->module == module) {
listDelNode(RedisModule_EventListeners,ln);
zfree(el);
}
}
}

void processModuleLoadingProgressEvent(int is_aof) {
long long now = server.ustime;
static long long next_event = 0;
if (now >= next_event) {

int progress = -1;
if (server.loading_total_bytes)
progress = (server.loading_loaded_bytes<<10) / server.loading_total_bytes;
RedisModuleLoadingProgressV1 fi = {REDISMODULE_LOADING_PROGRESS_VERSION,
server.hz,
progress};
moduleFireServerEvent(REDISMODULE_EVENT_LOADING_PROGRESS,
is_aof?
REDISMODULE_SUBEVENT_LOADING_PROGRESS_AOF:
REDISMODULE_SUBEVENT_LOADING_PROGRESS_RDB,
&fi);

next_event = now + 1000000 / server.hz;
}
}



void moduleNotifyKeyUnlink(robj *key, robj *val, int dbid) {
if (val->type == OBJ_MODULE) {
moduleValue *mv = val->ptr;
moduleType *mt = mv->type;

if (mt->unlink2 != NULL) {
RedisModuleKeyOptCtx ctx = {key, NULL, dbid, -1};
mt->unlink2(&ctx,mv->value);
} else if (mt->unlink != NULL) {
mt->unlink(key,mv->value);
}
}
}




size_t moduleGetFreeEffort(robj *key, robj *val, int dbid) {
moduleValue *mv = val->ptr;
moduleType *mt = mv->type;
size_t effort = 1;

if (mt->free_effort2 != NULL) {
RedisModuleKeyOptCtx ctx = {key, NULL, dbid, -1};
effort = mt->free_effort2(&ctx,mv->value);
} else if (mt->free_effort != NULL) {
effort = mt->free_effort(key,mv->value);
}

return effort;
}



size_t moduleGetMemUsage(robj *key, robj *val, size_t sample_size, int dbid) {
moduleValue *mv = val->ptr;
moduleType *mt = mv->type;
size_t size = 0;

if (mt->mem_usage2 != NULL) {
RedisModuleKeyOptCtx ctx = {key, NULL, dbid, -1};
size = mt->mem_usage2(&ctx, mv->value, sample_size);
} else if (mt->mem_usage != NULL) {
size = mt->mem_usage(mv->value);
}

return size;
}








uint64_t dictCStringKeyHash(const void *key) {
return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

int dictCStringKeyCompare(dict *d, const void *key1, const void *key2) {
UNUSED(d);
return strcmp(key1,key2) == 0;
}

dictType moduleAPIDictType = {
dictCStringKeyHash,
NULL,
NULL,
dictCStringKeyCompare,
NULL,
NULL,
NULL
};

int moduleRegisterApi(const char *funcname, void *funcptr) {
return dictAdd(server.moduleapi, (char*)funcname, funcptr);
}

#define REGISTER_API(name) moduleRegisterApi("RedisModule_" #name, (void *)(unsigned long)RM_ ##name)



void moduleRegisterCoreAPI(void);





void moduleInitModulesSystemLast(void) {
}

void moduleInitModulesSystem(void) {
moduleUnblockedClients = listCreate();
server.loadmodule_queue = listCreate();
modules = dictCreate(&modulesDictType);


moduleKeyspaceSubscribers = listCreate();


moduleCommandFilters = listCreate();

moduleRegisterCoreAPI();






if (anetPipe(server.module_pipe, O_CLOEXEC|O_NONBLOCK, O_CLOEXEC|O_NONBLOCK) == -1) {
serverLog(LL_WARNING,
"Can't create the pipe for module threads: %s", strerror(errno));
exit(1);
}


Timers = raxNew();


RedisModule_EventListeners = listCreate();


serverAssert(sizeof(moduleEventVersions)/sizeof(moduleEventVersions[0]) == _REDISMODULE_EVENT_NEXT);



pthread_mutex_lock(&moduleGIL);
}

void modulesCron(void) {






int iteration = 50;



const unsigned int min_client = 8;
while (iteration > 0 && moduleTempClientCount > 0 && moduleTempClientMinCount > min_client) {
client *c = moduleTempClients[--moduleTempClientCount];
freeClient(c);
iteration--;
moduleTempClientMinCount--;
}
moduleTempClientMinCount = moduleTempClientCount;


if (moduleTempClientCap > 32 && moduleTempClientCap > moduleTempClientCount * 4) {
moduleTempClientCap /= 4;
moduleTempClients = zrealloc(moduleTempClients,sizeof(client*)*moduleTempClientCap);
}
}

void moduleLoadQueueEntryFree(struct moduleLoadQueueEntry *loadmod) {
if (!loadmod) return;
sdsfree(loadmod->path);
for (int i = 0; i < loadmod->argc; i++) {
decrRefCount(loadmod->argv[i]);
}
zfree(loadmod->argv);
zfree(loadmod);
}










void moduleLoadFromQueue(void) {
listIter li;
listNode *ln;

listRewind(server.loadmodule_queue,&li);
while((ln = listNext(&li))) {
struct moduleLoadQueueEntry *loadmod = ln->value;
if (moduleLoad(loadmod->path,(void **)loadmod->argv,loadmod->argc)
== C_ERR)
{
serverLog(LL_WARNING,
"Can't load module from %s: server aborting",
loadmod->path);
exit(1);
}
moduleLoadQueueEntryFree(loadmod);
listDelNode(server.loadmodule_queue, ln);
}
}

void moduleFreeModuleStructure(struct RedisModule *module) {
listRelease(module->types);
listRelease(module->filters);
listRelease(module->usedby);
listRelease(module->using);
sdsfree(module->name);
moduleLoadQueueEntryFree(module->loadmod);
zfree(module);
}







int moduleFreeCommand(struct RedisModule *module, struct redisCommand *cmd) {
if (cmd->proc != RedisModuleCommandDispatcher)
return C_ERR;

RedisModuleCommand *cp = (void*)(unsigned long)cmd->getkeys_proc;
if (cp->module != module)
return C_ERR;


if (cmd->key_specs != cmd->key_specs_static)
zfree(cmd->key_specs);
for (int j = 0; cmd->tips && cmd->tips[j]; j++)
sdsfree((sds)cmd->tips[j]);
for (int j = 0; cmd->history && cmd->history[j].since; j++) {
sdsfree((sds)cmd->history[j].since);
sdsfree((sds)cmd->history[j].changes);
}
sdsfree((sds)cmd->summary);
sdsfree((sds)cmd->since);
sdsfree((sds)cmd->complexity);
if (cmd->latency_histogram) {
hdr_close(cmd->latency_histogram);
cmd->latency_histogram = NULL;
}
zfree(cmd->args);
zfree(cp);

if (cmd->subcommands_dict) {
dictEntry *de;
dictIterator *di = dictGetSafeIterator(cmd->subcommands_dict);
while ((de = dictNext(di)) != NULL) {
struct redisCommand *sub = dictGetVal(de);
if (moduleFreeCommand(module, sub) != C_OK) continue;

serverAssert(dictDelete(cmd->subcommands_dict, sub->declared_name) == DICT_OK);
sdsfree((sds)sub->declared_name);
sdsfree(sub->fullname);
zfree(sub);
}
dictReleaseIterator(di);
dictRelease(cmd->subcommands_dict);
}

return C_OK;
}

void moduleUnregisterCommands(struct RedisModule *module) {

dictIterator *di = dictGetSafeIterator(server.commands);
dictEntry *de;
while ((de = dictNext(di)) != NULL) {
struct redisCommand *cmd = dictGetVal(de);
if (moduleFreeCommand(module, cmd) != C_OK) continue;

serverAssert(dictDelete(server.commands, cmd->fullname) == DICT_OK);
serverAssert(dictDelete(server.orig_commands, cmd->fullname) == DICT_OK);
sdsfree((sds)cmd->declared_name);
sdsfree(cmd->fullname);
zfree(cmd);
}
dictReleaseIterator(di);
}



int moduleLoad(const char *path, void **module_argv, int module_argc) {
int (*onload)(void *, void **, int);
void *handle;

struct stat st;
if (stat(path, &st) == 0)
{
if (!(st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
serverLog(LL_WARNING, "Module %s failed to load: It does not have execute permissions.", path);
return C_ERR;
}
}

handle = dlopen(path,RTLD_NOW|RTLD_LOCAL);
if (handle == NULL) {
serverLog(LL_WARNING, "Module %s failed to load: %s", path, dlerror());
return C_ERR;
}
onload = (int (*)(void *, void **, int))(unsigned long) dlsym(handle,"RedisModule_OnLoad");
if (onload == NULL) {
dlclose(handle);
serverLog(LL_WARNING,
"Module %s does not export RedisModule_OnLoad() "
"symbol. Module not loaded.",path);
return C_ERR;
}
RedisModuleCtx ctx;
moduleCreateContext(&ctx, NULL, REDISMODULE_CTX_TEMP_CLIENT);
selectDb(ctx.client, 0);
if (onload((void*)&ctx,module_argv,module_argc) == REDISMODULE_ERR) {
if (ctx.module) {
moduleUnregisterCommands(ctx.module);
moduleUnregisterSharedAPI(ctx.module);
moduleUnregisterUsedAPI(ctx.module);
moduleFreeModuleStructure(ctx.module);
}
moduleFreeContext(&ctx);
dlclose(handle);
serverLog(LL_WARNING,
"Module %s initialization failed. Module not loaded",path);
return C_ERR;
}


dictAdd(modules,ctx.module->name,ctx.module);
ctx.module->blocked_clients = 0;
ctx.module->handle = handle;
ctx.module->loadmod = zmalloc(sizeof(struct moduleLoadQueueEntry));
ctx.module->loadmod->path = sdsnew(path);
ctx.module->loadmod->argv = module_argc ? zmalloc(sizeof(robj*)*module_argc) : NULL;
ctx.module->loadmod->argc = module_argc;
for (int i = 0; i < module_argc; i++) {
ctx.module->loadmod->argv[i] = module_argv[i];
incrRefCount(ctx.module->loadmod->argv[i]);
}

serverLog(LL_NOTICE,"Module '%s' loaded from %s",ctx.module->name,path);

moduleFireServerEvent(REDISMODULE_EVENT_MODULE_CHANGE,
REDISMODULE_SUBEVENT_MODULE_LOADED,
ctx.module);
moduleFreeContext(&ctx);
return C_OK;
}











int moduleUnload(sds name) {
struct RedisModule *module = dictFetchValue(modules,name);

if (module == NULL) {
errno = ENOENT;
return C_ERR;
} else if (listLength(module->types)) {
errno = EBUSY;
return C_ERR;
} else if (listLength(module->usedby)) {
errno = EPERM;
return C_ERR;
} else if (module->blocked_clients) {
errno = EAGAIN;
return C_ERR;
}


int (*onunload)(void *);
onunload = (int (*)(void *))(unsigned long) dlsym(module->handle, "RedisModule_OnUnload");
if (onunload) {
RedisModuleCtx ctx;
moduleCreateContext(&ctx, module, REDISMODULE_CTX_TEMP_CLIENT);
int unload_status = onunload((void*)&ctx);
moduleFreeContext(&ctx);

if (unload_status == REDISMODULE_ERR) {
serverLog(LL_WARNING, "Module %s OnUnload failed. Unload canceled.", name);
errno = ECANCELED;
return C_ERR;
}
}

moduleFreeAuthenticatedClients(module);
moduleUnregisterCommands(module);
moduleUnregisterSharedAPI(module);
moduleUnregisterUsedAPI(module);
moduleUnregisterFilters(module);


moduleUnsubscribeNotifications(module);
moduleUnsubscribeAllServerEvents(module);


if (dlclose(module->handle) == -1) {
char *error = dlerror();
if (error == NULL) error = "Unknown error";
serverLog(LL_WARNING,"Error when trying to close the %s module: %s",
module->name, error);
}


moduleFireServerEvent(REDISMODULE_EVENT_MODULE_CHANGE,
REDISMODULE_SUBEVENT_MODULE_UNLOADED,
module);


serverLog(LL_NOTICE,"Module %s unloaded",module->name);
dictDelete(modules,module->name);
module->name = NULL;
moduleFreeModuleStructure(module);

return C_OK;
}

void modulePipeReadable(aeEventLoop *el, int fd, void *privdata, int mask) {
UNUSED(el);
UNUSED(fd);
UNUSED(mask);
UNUSED(privdata);

char buf[128];
while (read(fd, buf, sizeof(buf)) == sizeof(buf));


eventLoopHandleOneShotEvents();
}



void addReplyLoadedModules(client *c) {
dictIterator *di = dictGetIterator(modules);
dictEntry *de;

addReplyArrayLen(c,dictSize(modules));
while ((de = dictNext(di)) != NULL) {
sds name = dictGetKey(de);
struct RedisModule *module = dictGetVal(de);
sds path = module->loadmod->path;
addReplyMapLen(c,4);
addReplyBulkCString(c,"name");
addReplyBulkCBuffer(c,name,sdslen(name));
addReplyBulkCString(c,"ver");
addReplyLongLong(c,module->ver);
addReplyBulkCString(c,"path");
addReplyBulkCBuffer(c,path,sdslen(path));
addReplyBulkCString(c,"args");
addReplyArrayLen(c,module->loadmod->argc);
for (int i = 0; i < module->loadmod->argc; i++) {
addReplyBulk(c,module->loadmod->argv[i]);
}
}
dictReleaseIterator(di);
}



sds genModulesInfoStringRenderModulesList(list *l) {
listIter li;
listNode *ln;
listRewind(l,&li);
sds output = sdsnew("[");
while((ln = listNext(&li))) {
RedisModule *module = ln->value;
output = sdscat(output,module->name);
if (ln != listLast(l))
output = sdscat(output,"|");
}
output = sdscat(output,"]");
return output;
}


sds genModulesInfoStringRenderModuleOptions(struct RedisModule *module) {
sds output = sdsnew("[");
if (module->options & REDISMODULE_OPTIONS_HANDLE_IO_ERRORS)
output = sdscat(output,"handle-io-errors|");
if (module->options & REDISMODULE_OPTIONS_HANDLE_REPL_ASYNC_LOAD)
output = sdscat(output,"handle-repl-async-load|");
output = sdstrim(output,"|");
output = sdscat(output,"]");
return output;
}







sds genModulesInfoString(sds info) {
dictIterator *di = dictGetIterator(modules);
dictEntry *de;

while ((de = dictNext(di)) != NULL) {
sds name = dictGetKey(de);
struct RedisModule *module = dictGetVal(de);

sds usedby = genModulesInfoStringRenderModulesList(module->usedby);
sds using = genModulesInfoStringRenderModulesList(module->using);
sds options = genModulesInfoStringRenderModuleOptions(module);
info = sdscatfmt(info,
"module:name=%S,ver=%i,api=%i,filters=%i,"
"usedby=%S,using=%S,options=%S\r\n",
name, module->ver, module->apiver,
(int)listLength(module->filters), usedby, using, options);
sdsfree(usedby);
sdsfree(using);
sdsfree(options);
}
dictReleaseIterator(di);
return info;
}







void moduleCommand(client *c) {
char *subcmd = c->argv[1]->ptr;

if (c->argc == 2 && !strcasecmp(subcmd,"help")) {
const char *help[] = {
"LIST",
" Return a list of loaded modules.",
"LOAD <path> [<arg> ...]",
" Load a module library from <path>, passing to it any optional arguments.",
"UNLOAD <name>",
" Unload a module.",
NULL
};
addReplyHelp(c, help);
} else if (!strcasecmp(subcmd,"load") && c->argc >= 3) {
robj **argv = NULL;
int argc = 0;

if (c->argc > 3) {
argc = c->argc - 3;
argv = &c->argv[3];
}

if (moduleLoad(c->argv[2]->ptr,(void **)argv,argc) == C_OK)
addReply(c,shared.ok);
else
addReplyError(c,
"Error loading the extension. Please check the server logs.");
} else if (!strcasecmp(subcmd,"unload") && c->argc == 3) {
if (moduleUnload(c->argv[2]->ptr) == C_OK)
addReply(c,shared.ok);
else {
char *errmsg;
switch(errno) {
case ENOENT:
errmsg = "no such module with that name";
break;
case EBUSY:
errmsg = "the module exports one or more module-side data "
"types, can't unload";
break;
case EPERM:
errmsg = "the module exports APIs used by other modules. "
"Please unload them first and try again";
break;
case EAGAIN:
errmsg = "the module has blocked clients. "
"Please wait them unblocked and try again";
break;
default:
errmsg = "operation not possible.";
break;
}
addReplyErrorFormat(c,"Error unloading module: %s",errmsg);
}
} else if (!strcasecmp(subcmd,"list") && c->argc == 2) {
addReplyLoadedModules(c);
} else {
addReplySubcommandSyntaxError(c);
return;
}
}


size_t moduleCount(void) {
return dictSize(modules);
}








int RM_SetLRU(RedisModuleKey *key, mstime_t lru_idle) {
if (!key->value)
return REDISMODULE_ERR;
if (objectSetLRUOrLFU(key->value, -1, lru_idle, lru_idle>=0 ? LRU_CLOCK() : 0, 1))
return REDISMODULE_OK;
return REDISMODULE_ERR;
}





int RM_GetLRU(RedisModuleKey *key, mstime_t *lru_idle) {
*lru_idle = -1;
if (!key->value)
return REDISMODULE_ERR;
if (server.maxmemory_policy & MAXMEMORY_FLAG_LFU)
return REDISMODULE_OK;
*lru_idle = estimateObjectIdleTime(key->value);
return REDISMODULE_OK;
}






int RM_SetLFU(RedisModuleKey *key, long long lfu_freq) {
if (!key->value)
return REDISMODULE_ERR;
if (objectSetLRUOrLFU(key->value, lfu_freq, -1, 0, 1))
return REDISMODULE_OK;
return REDISMODULE_ERR;
}




int RM_GetLFU(RedisModuleKey *key, long long *lfu_freq) {
*lfu_freq = -1;
if (!key->value)
return REDISMODULE_ERR;
if (server.maxmemory_policy & MAXMEMORY_FLAG_LFU)
*lfu_freq = LFUDecrAndReturn(key->value);
return REDISMODULE_OK;
}


















int RM_GetContextFlagsAll() {
return _REDISMODULE_CTX_FLAGS_NEXT - 1;
}














int RM_GetKeyspaceNotificationFlagsAll() {
return _REDISMODULE_NOTIFY_NEXT - 1;
}





int RM_GetServerVersion() {
return REDIS_VERSION_NUM;
}






int RM_GetTypeMethodVersion() {
return REDISMODULE_TYPE_METHOD_VERSION;
}


















int RM_ModuleTypeReplaceValue(RedisModuleKey *key, moduleType *mt, void *new_value, void **old_value) {
if (!(key->mode & REDISMODULE_WRITE) || key->iter)
return REDISMODULE_ERR;
if (!key->value || key->value->type != OBJ_MODULE)
return REDISMODULE_ERR;

moduleValue *mv = key->value->ptr;
if (mv->type != mt)
return REDISMODULE_ERR;

if (old_value)
*old_value = mv->value;
mv->value = new_value;

return REDISMODULE_OK;
}
















int *RM_GetCommandKeys(RedisModuleCtx *ctx, RedisModuleString **argv, int argc, int *num_keys) {
UNUSED(ctx);
struct redisCommand *cmd;
int *res = NULL;


if ((cmd = lookupCommand(argv,argc)) == NULL) {
errno = ENOENT;
return NULL;
}


if (cmd->getkeys_proc == NULL && cmd->key_specs_num == 0) {
errno = 0;
return NULL;
}

if ((cmd->arity > 0 && cmd->arity != argc) || (argc < -cmd->arity)) {
errno = EINVAL;
return NULL;
}

getKeysResult result = GETKEYS_RESULT_INIT;
getKeysFromCommand(cmd, argv, argc, &result);

*num_keys = result.numkeys;
if (!result.numkeys) {
errno = 0;
getKeysFreeResult(&result);
return NULL;
}


unsigned long int size = sizeof(int) * result.numkeys;
res = zmalloc(size);
for (int i = 0; i < result.numkeys; i++) {
res[i] = result.keys[i].pos;
}

return res;
}


const char *RM_GetCurrentCommandName(RedisModuleCtx *ctx) {
if (!ctx || !ctx->client || !ctx->client->cmd)
return NULL;

return (const char*)ctx->client->cmd->fullname;
}








typedef struct RedisModuleDefragCtx {
long defragged;
long long int endtime;
unsigned long *cursor;
struct redisObject *key;
int dbid;
} RedisModuleDefragCtx;




int RM_RegisterDefragFunc(RedisModuleCtx *ctx, RedisModuleDefragFunc cb) {
ctx->module->defrag_cb = cb;
return REDISMODULE_OK;
}















int RM_DefragShouldStop(RedisModuleDefragCtx *ctx) {
return (ctx->endtime != 0 && ctx->endtime < ustime());
}























int RM_DefragCursorSet(RedisModuleDefragCtx *ctx, unsigned long cursor) {
if (!ctx->cursor)
return REDISMODULE_ERR;

*ctx->cursor = cursor;
return REDISMODULE_OK;
}







int RM_DefragCursorGet(RedisModuleDefragCtx *ctx, unsigned long *cursor) {
if (!ctx->cursor)
return REDISMODULE_ERR;

*cursor = *ctx->cursor;
return REDISMODULE_OK;
}












void *RM_DefragAlloc(RedisModuleDefragCtx *ctx, void *ptr) {
void *newptr = activeDefragAlloc(ptr);
if (newptr)
ctx->defragged++;

return newptr;
}











RedisModuleString *RM_DefragRedisModuleString(RedisModuleDefragCtx *ctx, RedisModuleString *str) {
return activeDefragStringOb(str, &ctx->defragged);
}







int moduleLateDefrag(robj *key, robj *value, unsigned long *cursor, long long endtime, long long *defragged, int dbid) {
moduleValue *mv = value->ptr;
moduleType *mt = mv->type;

RedisModuleDefragCtx defrag_ctx = { 0, endtime, cursor, key, dbid};




int ret = 0;
if (mt->defrag)
ret = mt->defrag(&defrag_ctx, key, &mv->value);

*defragged += defrag_ctx.defragged;
if (!ret) {
*cursor = 0;
return 0;
}

return 1;
}







int moduleDefragValue(robj *key, robj *value, long *defragged, int dbid) {
moduleValue *mv = value->ptr;
moduleType *mt = mv->type;




moduleValue *newmv = activeDefragAlloc(mv);
if (newmv) {
(*defragged)++;
value->ptr = mv = newmv;
}

if (!mt->defrag)
return 1;





size_t effort = moduleGetFreeEffort(key, value, dbid);
if (!effort)
effort = SIZE_MAX;
if (effort > server.active_defrag_max_scan_fields) {
return 0;
}

RedisModuleDefragCtx defrag_ctx = { 0, 0, NULL, key, dbid};
mt->defrag(&defrag_ctx, key, &mv->value);
(*defragged) += defrag_ctx.defragged;
return 1;
}


long moduleDefragGlobals(void) {
dictIterator *di = dictGetIterator(modules);
dictEntry *de;
long defragged = 0;

while ((de = dictNext(di)) != NULL) {
struct RedisModule *module = dictGetVal(de);
if (!module->defrag_cb)
continue;
RedisModuleDefragCtx defrag_ctx = { 0, 0, NULL, NULL, -1};
module->defrag_cb(&defrag_ctx);
defragged += defrag_ctx.defragged;
}
dictReleaseIterator(di);

return defragged;
}




const RedisModuleString *RM_GetKeyNameFromDefragCtx(RedisModuleDefragCtx *ctx) {
return ctx->key;
}




int RM_GetDbIdFromDefragCtx(RedisModuleDefragCtx *ctx) {
return ctx->dbid;
}



void moduleRegisterCoreAPI(void) {
server.moduleapi = dictCreate(&moduleAPIDictType);
server.sharedapi = dictCreate(&moduleAPIDictType);
REGISTER_API(Alloc);
REGISTER_API(Calloc);
REGISTER_API(Realloc);
REGISTER_API(Free);
REGISTER_API(Strdup);
REGISTER_API(CreateCommand);
REGISTER_API(GetCommand);
REGISTER_API(CreateSubcommand);
REGISTER_API(SetModuleAttribs);
REGISTER_API(IsModuleNameBusy);
REGISTER_API(WrongArity);
REGISTER_API(ReplyWithLongLong);
REGISTER_API(ReplyWithError);
REGISTER_API(ReplyWithSimpleString);
REGISTER_API(ReplyWithArray);
REGISTER_API(ReplyWithMap);
REGISTER_API(ReplyWithSet);
REGISTER_API(ReplyWithAttribute);
REGISTER_API(ReplyWithNullArray);
REGISTER_API(ReplyWithEmptyArray);
REGISTER_API(ReplySetArrayLength);
REGISTER_API(ReplySetMapLength);
REGISTER_API(ReplySetSetLength);
REGISTER_API(ReplySetAttributeLength);
REGISTER_API(ReplyWithString);
REGISTER_API(ReplyWithEmptyString);
REGISTER_API(ReplyWithVerbatimString);
REGISTER_API(ReplyWithVerbatimStringType);
REGISTER_API(ReplyWithStringBuffer);
REGISTER_API(ReplyWithCString);
REGISTER_API(ReplyWithNull);
REGISTER_API(ReplyWithBool);
REGISTER_API(ReplyWithCallReply);
REGISTER_API(ReplyWithDouble);
REGISTER_API(ReplyWithBigNumber);
REGISTER_API(ReplyWithLongDouble);
REGISTER_API(GetSelectedDb);
REGISTER_API(SelectDb);
REGISTER_API(KeyExists);
REGISTER_API(OpenKey);
REGISTER_API(CloseKey);
REGISTER_API(KeyType);
REGISTER_API(ValueLength);
REGISTER_API(ListPush);
REGISTER_API(ListPop);
REGISTER_API(ListGet);
REGISTER_API(ListSet);
REGISTER_API(ListInsert);
REGISTER_API(ListDelete);
REGISTER_API(StringToLongLong);
REGISTER_API(StringToDouble);
REGISTER_API(StringToLongDouble);
REGISTER_API(StringToStreamID);
REGISTER_API(Call);
REGISTER_API(CallReplyProto);
REGISTER_API(FreeCallReply);
REGISTER_API(CallReplyInteger);
REGISTER_API(CallReplyDouble);
REGISTER_API(CallReplyBigNumber);
REGISTER_API(CallReplyVerbatim);
REGISTER_API(CallReplyBool);
REGISTER_API(CallReplySetElement);
REGISTER_API(CallReplyMapElement);
REGISTER_API(CallReplyAttributeElement);
REGISTER_API(CallReplyAttribute);
REGISTER_API(CallReplyType);
REGISTER_API(CallReplyLength);
REGISTER_API(CallReplyArrayElement);
REGISTER_API(CallReplyStringPtr);
REGISTER_API(CreateStringFromCallReply);
REGISTER_API(CreateString);
REGISTER_API(CreateStringFromLongLong);
REGISTER_API(CreateStringFromDouble);
REGISTER_API(CreateStringFromLongDouble);
REGISTER_API(CreateStringFromString);
REGISTER_API(CreateStringFromStreamID);
REGISTER_API(CreateStringPrintf);
REGISTER_API(FreeString);
REGISTER_API(StringPtrLen);
REGISTER_API(AutoMemory);
REGISTER_API(Replicate);
REGISTER_API(ReplicateVerbatim);
REGISTER_API(DeleteKey);
REGISTER_API(UnlinkKey);
REGISTER_API(StringSet);
REGISTER_API(StringDMA);
REGISTER_API(StringTruncate);
REGISTER_API(SetExpire);
REGISTER_API(GetExpire);
REGISTER_API(ResetDataset);
REGISTER_API(DbSize);
REGISTER_API(RandomKey);
REGISTER_API(ZsetAdd);
REGISTER_API(ZsetIncrby);
REGISTER_API(ZsetScore);
REGISTER_API(ZsetRem);
REGISTER_API(ZsetRangeStop);
REGISTER_API(ZsetFirstInScoreRange);
REGISTER_API(ZsetLastInScoreRange);
REGISTER_API(ZsetFirstInLexRange);
REGISTER_API(ZsetLastInLexRange);
REGISTER_API(ZsetRangeCurrentElement);
REGISTER_API(ZsetRangeNext);
REGISTER_API(ZsetRangePrev);
REGISTER_API(ZsetRangeEndReached);
REGISTER_API(HashSet);
REGISTER_API(HashGet);
REGISTER_API(StreamAdd);
REGISTER_API(StreamDelete);
REGISTER_API(StreamIteratorStart);
REGISTER_API(StreamIteratorStop);
REGISTER_API(StreamIteratorNextID);
REGISTER_API(StreamIteratorNextField);
REGISTER_API(StreamIteratorDelete);
REGISTER_API(StreamTrimByLength);
REGISTER_API(StreamTrimByID);
REGISTER_API(IsKeysPositionRequest);
REGISTER_API(KeyAtPos);
REGISTER_API(GetClientId);
REGISTER_API(GetClientUserNameById);
REGISTER_API(GetContextFlags);
REGISTER_API(AvoidReplicaTraffic);
REGISTER_API(PoolAlloc);
REGISTER_API(CreateDataType);
REGISTER_API(ModuleTypeSetValue);
REGISTER_API(ModuleTypeReplaceValue);
REGISTER_API(ModuleTypeGetType);
REGISTER_API(ModuleTypeGetValue);
REGISTER_API(IsIOError);
REGISTER_API(SetModuleOptions);
REGISTER_API(SignalModifiedKey);
REGISTER_API(SaveUnsigned);
REGISTER_API(LoadUnsigned);
REGISTER_API(SaveSigned);
REGISTER_API(LoadSigned);
REGISTER_API(SaveString);
REGISTER_API(SaveStringBuffer);
REGISTER_API(LoadString);
REGISTER_API(LoadStringBuffer);
REGISTER_API(SaveDouble);
REGISTER_API(LoadDouble);
REGISTER_API(SaveFloat);
REGISTER_API(LoadFloat);
REGISTER_API(SaveLongDouble);
REGISTER_API(LoadLongDouble);
REGISTER_API(SaveDataTypeToString);
REGISTER_API(LoadDataTypeFromString);
REGISTER_API(LoadDataTypeFromStringEncver);
REGISTER_API(EmitAOF);
REGISTER_API(Log);
REGISTER_API(LogIOError);
REGISTER_API(_Assert);
REGISTER_API(LatencyAddSample);
REGISTER_API(StringAppendBuffer);
REGISTER_API(TrimStringAllocation);
REGISTER_API(RetainString);
REGISTER_API(HoldString);
REGISTER_API(StringCompare);
REGISTER_API(GetContextFromIO);
REGISTER_API(GetKeyNameFromIO);
REGISTER_API(GetKeyNameFromModuleKey);
REGISTER_API(GetDbIdFromModuleKey);
REGISTER_API(GetDbIdFromIO);
REGISTER_API(GetKeyNameFromOptCtx);
REGISTER_API(GetToKeyNameFromOptCtx);
REGISTER_API(GetDbIdFromOptCtx);
REGISTER_API(GetToDbIdFromOptCtx);
REGISTER_API(GetKeyNameFromDefragCtx);
REGISTER_API(GetDbIdFromDefragCtx);
REGISTER_API(GetKeyNameFromDigest);
REGISTER_API(GetDbIdFromDigest);
REGISTER_API(BlockClient);
REGISTER_API(UnblockClient);
REGISTER_API(IsBlockedReplyRequest);
REGISTER_API(IsBlockedTimeoutRequest);
REGISTER_API(GetBlockedClientPrivateData);
REGISTER_API(AbortBlock);
REGISTER_API(Milliseconds);
REGISTER_API(MonotonicMicroseconds);
REGISTER_API(BlockedClientMeasureTimeStart);
REGISTER_API(BlockedClientMeasureTimeEnd);
REGISTER_API(GetThreadSafeContext);
REGISTER_API(GetDetachedThreadSafeContext);
REGISTER_API(FreeThreadSafeContext);
REGISTER_API(ThreadSafeContextLock);
REGISTER_API(ThreadSafeContextTryLock);
REGISTER_API(ThreadSafeContextUnlock);
REGISTER_API(DigestAddStringBuffer);
REGISTER_API(DigestAddLongLong);
REGISTER_API(DigestEndSequence);
REGISTER_API(NotifyKeyspaceEvent);
REGISTER_API(GetNotifyKeyspaceEvents);
REGISTER_API(SubscribeToKeyspaceEvents);
REGISTER_API(RegisterClusterMessageReceiver);
REGISTER_API(SendClusterMessage);
REGISTER_API(GetClusterNodeInfo);
REGISTER_API(GetClusterNodesList);
REGISTER_API(FreeClusterNodesList);
REGISTER_API(CreateTimer);
REGISTER_API(StopTimer);
REGISTER_API(GetTimerInfo);
REGISTER_API(GetMyClusterID);
REGISTER_API(GetClusterSize);
REGISTER_API(GetRandomBytes);
REGISTER_API(GetRandomHexChars);
REGISTER_API(BlockedClientDisconnected);
REGISTER_API(SetDisconnectCallback);
REGISTER_API(GetBlockedClientHandle);
REGISTER_API(SetClusterFlags);
REGISTER_API(CreateDict);
REGISTER_API(FreeDict);
REGISTER_API(DictSize);
REGISTER_API(DictSetC);
REGISTER_API(DictReplaceC);
REGISTER_API(DictSet);
REGISTER_API(DictReplace);
REGISTER_API(DictGetC);
REGISTER_API(DictGet);
REGISTER_API(DictDelC);
REGISTER_API(DictDel);
REGISTER_API(DictIteratorStartC);
REGISTER_API(DictIteratorStart);
REGISTER_API(DictIteratorStop);
REGISTER_API(DictIteratorReseekC);
REGISTER_API(DictIteratorReseek);
REGISTER_API(DictNextC);
REGISTER_API(DictPrevC);
REGISTER_API(DictNext);
REGISTER_API(DictPrev);
REGISTER_API(DictCompareC);
REGISTER_API(DictCompare);
REGISTER_API(ExportSharedAPI);
REGISTER_API(GetSharedAPI);
REGISTER_API(RegisterCommandFilter);
REGISTER_API(UnregisterCommandFilter);
REGISTER_API(CommandFilterArgsCount);
REGISTER_API(CommandFilterArgGet);
REGISTER_API(CommandFilterArgInsert);
REGISTER_API(CommandFilterArgReplace);
REGISTER_API(CommandFilterArgDelete);
REGISTER_API(Fork);
REGISTER_API(SendChildHeartbeat);
REGISTER_API(ExitFromChild);
REGISTER_API(KillForkChild);
REGISTER_API(RegisterInfoFunc);
REGISTER_API(InfoAddSection);
REGISTER_API(InfoBeginDictField);
REGISTER_API(InfoEndDictField);
REGISTER_API(InfoAddFieldString);
REGISTER_API(InfoAddFieldCString);
REGISTER_API(InfoAddFieldDouble);
REGISTER_API(InfoAddFieldLongLong);
REGISTER_API(InfoAddFieldULongLong);
REGISTER_API(GetServerInfo);
REGISTER_API(FreeServerInfo);
REGISTER_API(ServerInfoGetField);
REGISTER_API(ServerInfoGetFieldC);
REGISTER_API(ServerInfoGetFieldSigned);
REGISTER_API(ServerInfoGetFieldUnsigned);
REGISTER_API(ServerInfoGetFieldDouble);
REGISTER_API(GetClientInfoById);
REGISTER_API(PublishMessage);
REGISTER_API(SubscribeToServerEvent);
REGISTER_API(SetLRU);
REGISTER_API(GetLRU);
REGISTER_API(SetLFU);
REGISTER_API(GetLFU);
REGISTER_API(BlockClientOnKeys);
REGISTER_API(SignalKeyAsReady);
REGISTER_API(GetBlockedClientReadyKey);
REGISTER_API(GetUsedMemoryRatio);
REGISTER_API(MallocSize);
REGISTER_API(ScanCursorCreate);
REGISTER_API(ScanCursorDestroy);
REGISTER_API(ScanCursorRestart);
REGISTER_API(Scan);
REGISTER_API(ScanKey);
REGISTER_API(CreateModuleUser);
REGISTER_API(SetModuleUserACL);
REGISTER_API(GetCurrentUserName);
REGISTER_API(GetModuleUserFromUserName);
REGISTER_API(ACLCheckCommandPermissions);
REGISTER_API(ACLCheckKeyPermissions);
REGISTER_API(ACLCheckChannelPermissions);
REGISTER_API(ACLAddLogEntry);
REGISTER_API(FreeModuleUser);
REGISTER_API(DeauthenticateAndCloseClient);
REGISTER_API(AuthenticateClientWithACLUser);
REGISTER_API(AuthenticateClientWithUser);
REGISTER_API(GetContextFlagsAll);
REGISTER_API(GetKeyspaceNotificationFlagsAll);
REGISTER_API(IsSubEventSupported);
REGISTER_API(GetServerVersion);
REGISTER_API(GetClientCertificate);
REGISTER_API(GetCommandKeys);
REGISTER_API(GetCurrentCommandName);
REGISTER_API(GetTypeMethodVersion);
REGISTER_API(RegisterDefragFunc);
REGISTER_API(DefragAlloc);
REGISTER_API(DefragRedisModuleString);
REGISTER_API(DefragShouldStop);
REGISTER_API(DefragCursorSet);
REGISTER_API(DefragCursorGet);
#if defined(INCLUDE_UNRELEASED_KEYSPEC_API)
REGISTER_API(AddCommandKeySpec);
REGISTER_API(SetCommandKeySpecBeginSearchIndex);
REGISTER_API(SetCommandKeySpecBeginSearchKeyword);
REGISTER_API(SetCommandKeySpecFindKeysRange);
REGISTER_API(SetCommandKeySpecFindKeysKeynum);
#endif
REGISTER_API(EventLoopAdd);
REGISTER_API(EventLoopDel);
REGISTER_API(EventLoopAddOneShot);
REGISTER_API(Yield);
}
