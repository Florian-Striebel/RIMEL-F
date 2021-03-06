#include <stdio.h>
#include "rio.h"
#include "server.h"
#define RDB_VERSION 10
#define RDB_6BITLEN 0
#define RDB_14BITLEN 1
#define RDB_32BITLEN 0x80
#define RDB_64BITLEN 0x81
#define RDB_ENCVAL 3
#define RDB_LENERR UINT64_MAX
#define RDB_ENC_INT8 0
#define RDB_ENC_INT16 1
#define RDB_ENC_INT32 2
#define RDB_ENC_LZF 3
#define RDB_TYPE_STRING 0
#define RDB_TYPE_LIST 1
#define RDB_TYPE_SET 2
#define RDB_TYPE_ZSET 3
#define RDB_TYPE_HASH 4
#define RDB_TYPE_ZSET_2 5
#define RDB_TYPE_MODULE 6
#define RDB_TYPE_MODULE_2 7
#define RDB_TYPE_HASH_ZIPMAP 9
#define RDB_TYPE_LIST_ZIPLIST 10
#define RDB_TYPE_SET_INTSET 11
#define RDB_TYPE_ZSET_ZIPLIST 12
#define RDB_TYPE_HASH_ZIPLIST 13
#define RDB_TYPE_LIST_QUICKLIST 14
#define RDB_TYPE_STREAM_LISTPACKS 15
#define RDB_TYPE_HASH_LISTPACK 16
#define RDB_TYPE_ZSET_LISTPACK 17
#define RDB_TYPE_LIST_QUICKLIST_2 18
#define rdbIsObjectType(t) ((t >= 0 && t <= 7) || (t >= 9 && t <= 18))
#define RDB_OPCODE_FUNCTION 246
#define RDB_OPCODE_MODULE_AUX 247
#define RDB_OPCODE_IDLE 248
#define RDB_OPCODE_FREQ 249
#define RDB_OPCODE_AUX 250
#define RDB_OPCODE_RESIZEDB 251
#define RDB_OPCODE_EXPIRETIME_MS 252
#define RDB_OPCODE_EXPIRETIME 253
#define RDB_OPCODE_SELECTDB 254
#define RDB_OPCODE_EOF 255
#define RDB_MODULE_OPCODE_EOF 0
#define RDB_MODULE_OPCODE_SINT 1
#define RDB_MODULE_OPCODE_UINT 2
#define RDB_MODULE_OPCODE_FLOAT 3
#define RDB_MODULE_OPCODE_DOUBLE 4
#define RDB_MODULE_OPCODE_STRING 5
#define RDB_LOAD_NONE 0
#define RDB_LOAD_ENC (1<<0)
#define RDB_LOAD_PLAIN (1<<1)
#define RDB_LOAD_SDS (1<<2)
#define RDBFLAGS_NONE 0
#define RDBFLAGS_AOF_PREAMBLE (1<<0)
#define RDBFLAGS_REPLICATION (1<<1)
#define RDBFLAGS_ALLOW_DUP (1<<2)
#define RDBFLAGS_FEED_REPL (1<<3)
#define RDB_LOAD_ERR_EMPTY_KEY 1
#define RDB_LOAD_ERR_OTHER 2
int rdbSaveType(rio *rdb, unsigned char type);
int rdbLoadType(rio *rdb);
time_t rdbLoadTime(rio *rdb);
int rdbSaveLen(rio *rdb, uint64_t len);
int rdbSaveMillisecondTime(rio *rdb, long long t);
long long rdbLoadMillisecondTime(rio *rdb, int rdbver);
uint64_t rdbLoadLen(rio *rdb, int *isencoded);
int rdbLoadLenByRef(rio *rdb, int *isencoded, uint64_t *lenptr);
int rdbSaveObjectType(rio *rdb, robj *o);
int rdbLoadObjectType(rio *rdb);
int rdbLoad(char *filename, rdbSaveInfo *rsi, int rdbflags);
int rdbSaveBackground(int req, char *filename, rdbSaveInfo *rsi);
int rdbSaveToSlavesSockets(int req, rdbSaveInfo *rsi);
void rdbRemoveTempFile(pid_t childpid, int from_signal);
int rdbSave(int req, char *filename, rdbSaveInfo *rsi);
ssize_t rdbSaveObject(rio *rdb, robj *o, robj *key, int dbid);
size_t rdbSavedObjectLen(robj *o, robj *key, int dbid);
robj *rdbLoadObject(int type, rio *rdb, sds key, int dbid, int *error);
void backgroundSaveDoneHandler(int exitcode, int bysignal);
int rdbSaveKeyValuePair(rio *rdb, robj *key, robj *val, long long expiretime,int dbid);
ssize_t rdbSaveSingleModuleAux(rio *rdb, int when, moduleType *mt);
robj *rdbLoadCheckModuleValue(rio *rdb, char *modulename);
robj *rdbLoadStringObject(rio *rdb);
ssize_t rdbSaveStringObject(rio *rdb, robj *obj);
ssize_t rdbSaveRawString(rio *rdb, unsigned char *s, size_t len);
void *rdbGenericLoadStringObject(rio *rdb, int flags, size_t *lenptr);
int rdbSaveBinaryDoubleValue(rio *rdb, double val);
int rdbLoadBinaryDoubleValue(rio *rdb, double *val);
int rdbSaveBinaryFloatValue(rio *rdb, float val);
int rdbLoadBinaryFloatValue(rio *rdb, float *val);
int rdbLoadRio(rio *rdb, int rdbflags, rdbSaveInfo *rsi);
int rdbLoadRioWithLoadingCtx(rio *rdb, int rdbflags, rdbSaveInfo *rsi, rdbLoadingCtx *rdb_loading_ctx);
int rdbFunctionLoad(rio *rdb, int ver, functionsLibCtx* lib_ctx, int rdbflags, sds *err);
int rdbSaveRio(int req, rio *rdb, int *error, int rdbflags, rdbSaveInfo *rsi);
ssize_t rdbSaveFunctions(rio *rdb);
rdbSaveInfo *rdbPopulateSaveInfo(rdbSaveInfo *rsi);
