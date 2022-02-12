#include "functions.h"
#include "sds.h"
#include "dict.h"
#include "adlist.h"
#include "atomicvar.h"
typedef enum {
restorePolicy_Flush, restorePolicy_Append, restorePolicy_Replace
} restorePolicy;
static size_t engine_cache_memory = 0;
static void engineFunctionDispose(dict *d, void *obj);
static void engineStatsDispose(dict *d, void *obj);
static void engineLibraryDispose(dict *d, void *obj);
static int functionsVerifyName(sds name);
typedef struct functionsLibEngineStats {
size_t n_lib;
size_t n_functions;
} functionsLibEngineStats;
struct functionsLibCtx {
dict *libraries;
dict *functions;
size_t cache_memory;
dict *engines_stats;
};
dictType engineDictType = {
dictSdsCaseHash,
dictSdsDup,
NULL,
dictSdsKeyCaseCompare,
dictSdsDestructor,
NULL,
NULL
};
dictType functionDictType = {
dictSdsCaseHash,
dictSdsDup,
NULL,
dictSdsKeyCaseCompare,
dictSdsDestructor,
NULL,
NULL
};
dictType engineStatsDictType = {
dictSdsCaseHash,
dictSdsDup,
NULL,
dictSdsKeyCaseCompare,
dictSdsDestructor,
engineStatsDispose,
NULL
};
dictType libraryFunctionDictType = {
dictSdsHash,
dictSdsDup,
NULL,
dictSdsKeyCompare,
dictSdsDestructor,
engineFunctionDispose,
NULL
};
dictType librariesDictType = {
dictSdsHash,
dictSdsDup,
NULL,
dictSdsKeyCompare,
dictSdsDestructor,
engineLibraryDispose,
NULL
};
static dict *engines = NULL;
static functionsLibCtx *curr_functions_lib_ctx = NULL;
static size_t functionMallocSize(functionInfo *fi) {
return zmalloc_size(fi) + sdsZmallocSize(fi->name)
+ (fi->desc ? sdsZmallocSize(fi->desc) : 0)
+ fi->li->ei->engine->get_function_memory_overhead(fi->function);
}
static size_t libraryMallocSize(functionLibInfo *li) {
return zmalloc_size(li) + sdsZmallocSize(li->name)
+ (li->desc ? sdsZmallocSize(li->desc) : 0)
+ sdsZmallocSize(li->code);
}
static void engineStatsDispose(dict *d, void *obj) {
UNUSED(d);
functionsLibEngineStats *stats = obj;
zfree(stats);
}
static void engineFunctionDispose(dict *d, void *obj) {
UNUSED(d);
if (!obj) {
return;
}
functionInfo *fi = obj;
sdsfree(fi->name);
if (fi->desc) {
sdsfree(fi->desc);
}
engine *engine = fi->li->ei->engine;
engine->free_function(engine->engine_ctx, fi->function);
zfree(fi);
}
static void engineLibraryFree(functionLibInfo* li) {
if (!li) {
return;
}
dictRelease(li->functions);
sdsfree(li->name);
sdsfree(li->code);
if (li->desc) sdsfree(li->desc);
zfree(li);
}
static void engineLibraryDispose(dict *d, void *obj) {
UNUSED(d);
engineLibraryFree(obj);
}
void functionsLibCtxClear(functionsLibCtx *lib_ctx) {
dictEmpty(lib_ctx->functions, NULL);
dictEmpty(lib_ctx->libraries, NULL);
dictIterator *iter = dictGetIterator(lib_ctx->engines_stats);
dictEntry *entry = NULL;
while ((entry = dictNext(iter))) {
functionsLibEngineStats *stats = dictGetVal(entry);
stats->n_functions = 0;
stats->n_lib = 0;
}
dictReleaseIterator(iter);
curr_functions_lib_ctx->cache_memory = 0;
}
void functionsLibCtxClearCurrent(int async) {
if (async) {
functionsLibCtx *old_l_ctx = curr_functions_lib_ctx;
curr_functions_lib_ctx = functionsLibCtxCreate();
freeFunctionsAsync(old_l_ctx);
} else {
functionsLibCtxClear(curr_functions_lib_ctx);
}
}
void functionsLibCtxFree(functionsLibCtx *functions_lib_ctx) {
functionsLibCtxClear(functions_lib_ctx);
dictRelease(functions_lib_ctx->functions);
dictRelease(functions_lib_ctx->libraries);
dictRelease(functions_lib_ctx->engines_stats);
zfree(functions_lib_ctx);
}
void functionsLibCtxSwapWithCurrent(functionsLibCtx *new_lib_ctx) {
functionsLibCtxFree(curr_functions_lib_ctx);
curr_functions_lib_ctx = new_lib_ctx;
}
functionsLibCtx* functionsLibCtxGetCurrent() {
return curr_functions_lib_ctx;
}
functionsLibCtx* functionsLibCtxCreate() {
functionsLibCtx *ret = zmalloc(sizeof(functionsLibCtx));
ret->libraries = dictCreate(&librariesDictType);
ret->functions = dictCreate(&functionDictType);
ret->engines_stats = dictCreate(&engineStatsDictType);
dictIterator *iter = dictGetIterator(engines);
dictEntry *entry = NULL;
while ((entry = dictNext(iter))) {
engineInfo *ei = dictGetVal(entry);
functionsLibEngineStats *stats = zcalloc(sizeof(*stats));
dictAdd(ret->engines_stats, ei->name, stats);
}
dictReleaseIterator(iter);
ret->cache_memory = 0;
return ret;
}
int functionLibCreateFunction(sds name, void *function, functionLibInfo *li, sds desc, uint64_t f_flags, sds *err) {
if (functionsVerifyName(name) != C_OK) {
*err = sdsnew("Function names can only contain letters and numbers and must be at least one character long");
return C_ERR;
}
if (dictFetchValue(li->functions, name)) {
*err = sdsnew("Function already exists in the library");
return C_ERR;
}
functionInfo *fi = zmalloc(sizeof(*fi));
*fi = (functionInfo) {
.name = name,
.function = function,
.li = li,
.desc = desc,
.f_flags = f_flags,
};
int res = dictAdd(li->functions, fi->name, fi);
serverAssert(res == DICT_OK);
return C_OK;
}
static functionLibInfo* engineLibraryCreate(sds name, engineInfo *ei, sds desc, sds code) {
functionLibInfo *li = zmalloc(sizeof(*li));
*li = (functionLibInfo) {
.name = sdsdup(name),
.functions = dictCreate(&libraryFunctionDictType),
.ei = ei,
.code = sdsdup(code),
.desc = desc ? sdsdup(desc) : NULL,
};
return li;
}
static void libraryUnlink(functionsLibCtx *lib_ctx, functionLibInfo* li) {
dictIterator *iter = dictGetIterator(li->functions);
dictEntry *entry = NULL;
while ((entry = dictNext(iter))) {
functionInfo *fi = dictGetVal(entry);
int ret = dictDelete(lib_ctx->functions, fi->name);
serverAssert(ret == DICT_OK);
lib_ctx->cache_memory -= functionMallocSize(fi);
}
dictReleaseIterator(iter);
entry = dictUnlink(lib_ctx->libraries, li->name);
dictSetVal(lib_ctx->libraries, entry, NULL);
dictFreeUnlinkedEntry(lib_ctx->libraries, entry);
lib_ctx->cache_memory += libraryMallocSize(li);
functionsLibEngineStats *stats = dictFetchValue(lib_ctx->engines_stats, li->ei->name);
serverAssert(stats);
stats->n_lib--;
stats->n_functions -= dictSize(li->functions);
}
static void libraryLink(functionsLibCtx *lib_ctx, functionLibInfo* li) {
dictIterator *iter = dictGetIterator(li->functions);
dictEntry *entry = NULL;
while ((entry = dictNext(iter))) {
functionInfo *fi = dictGetVal(entry);
dictAdd(lib_ctx->functions, fi->name, fi);
lib_ctx->cache_memory += functionMallocSize(fi);
}
dictReleaseIterator(iter);
dictAdd(lib_ctx->libraries, li->name, li);
lib_ctx->cache_memory += libraryMallocSize(li);
functionsLibEngineStats *stats = dictFetchValue(lib_ctx->engines_stats, li->ei->name);
serverAssert(stats);
stats->n_lib++;
stats->n_functions += dictSize(li->functions);
}
static int libraryJoin(functionsLibCtx *functions_lib_ctx_dst, functionsLibCtx *functions_lib_ctx_src, int replace, sds *err) {
int ret = C_ERR;
dictIterator *iter = NULL;
list *old_libraries_list = NULL;
dictEntry *entry = NULL;
iter = dictGetIterator(functions_lib_ctx_src->libraries);
while ((entry = dictNext(iter))) {
functionLibInfo *li = dictGetVal(entry);
functionLibInfo *old_li = dictFetchValue(functions_lib_ctx_dst->libraries, li->name);
if (old_li) {
if (!replace) {
*err = sdscatfmt(sdsempty(), "Library %s already exists", li->name);
goto done;
} else {
if (!old_libraries_list) {
old_libraries_list = listCreate();
listSetFreeMethod(old_libraries_list, (void (*)(void*))engineLibraryFree);
}
libraryUnlink(functions_lib_ctx_dst, old_li);
listAddNodeTail(old_libraries_list, old_li);
}
}
}
dictReleaseIterator(iter);
iter = NULL;
iter = dictGetIterator(functions_lib_ctx_src->functions);
while ((entry = dictNext(iter))) {
functionInfo *fi = dictGetVal(entry);
if (dictFetchValue(functions_lib_ctx_dst->functions, fi->name)) {
*err = sdscatfmt(sdsempty(), "Function %s already exists", fi->name);
goto done;
}
}
dictReleaseIterator(iter);
iter = NULL;
iter = dictGetIterator(functions_lib_ctx_src->libraries);
while ((entry = dictNext(iter))) {
functionLibInfo *li = dictGetVal(entry);
libraryLink(functions_lib_ctx_dst, li);
dictSetVal(functions_lib_ctx_src->libraries, entry, NULL);
}
dictReleaseIterator(iter);
iter = NULL;
functionsLibCtxClear(functions_lib_ctx_src);
if (old_libraries_list) {
listRelease(old_libraries_list);
old_libraries_list = NULL;
}
ret = C_OK;
done:
if (iter) dictReleaseIterator(iter);
if (old_libraries_list) {
while (listLength(old_libraries_list) > 0) {
listNode *head = listFirst(old_libraries_list);
functionLibInfo *li = listNodeValue(head);
listNodeValue(head) = NULL;
libraryLink(functions_lib_ctx_dst, li);
listDelNode(old_libraries_list, head);
}
listRelease(old_libraries_list);
}
return ret;
}
int functionsRegisterEngine(const char *engine_name, engine *engine) {
sds engine_name_sds = sdsnew(engine_name);
if (dictFetchValue(engines, engine_name_sds)) {
serverLog(LL_WARNING, "Same engine was registered twice");
sdsfree(engine_name_sds);
return C_ERR;
}
client *c = createClient(NULL);
c->flags |= (CLIENT_DENY_BLOCKING | CLIENT_SCRIPT);
engineInfo *ei = zmalloc(sizeof(*ei));
*ei = (engineInfo ) { .name = engine_name_sds, .engine = engine, .c = c,};
dictAdd(engines, engine_name_sds, ei);
engine_cache_memory += zmalloc_size(ei) + sdsZmallocSize(ei->name) +
zmalloc_size(engine) +
engine->get_engine_memory_overhead(engine->engine_ctx);
return C_OK;
}
void functionStatsCommand(client *c) {
if (scriptIsRunning() && scriptIsEval()) {
addReplyErrorObject(c, shared.slowevalerr);
return;
}
addReplyMapLen(c, 2);
addReplyBulkCString(c, "running_script");
if (!scriptIsRunning()) {
addReplyNull(c);
} else {
addReplyMapLen(c, 3);
addReplyBulkCString(c, "name");
addReplyBulkCString(c, scriptCurrFunction());
addReplyBulkCString(c, "command");
client *script_client = scriptGetCaller();
addReplyArrayLen(c, script_client->argc);
for (int i = 0 ; i < script_client->argc ; ++i) {
addReplyBulkCBuffer(c, script_client->argv[i]->ptr, sdslen(script_client->argv[i]->ptr));
}
addReplyBulkCString(c, "duration_ms");
addReplyLongLong(c, scriptRunDuration());
}
addReplyBulkCString(c, "engines");
addReplyMapLen(c, dictSize(engines));
dictIterator *iter = dictGetIterator(engines);
dictEntry *entry = NULL;
while ((entry = dictNext(iter))) {
engineInfo *ei = dictGetVal(entry);
addReplyBulkCString(c, ei->name);
addReplyMapLen(c, 2);
functionsLibEngineStats *e_stats = dictFetchValue(curr_functions_lib_ctx->engines_stats, ei->name);
addReplyBulkCString(c, "libraries_count");
addReplyLongLong(c, e_stats->n_lib);
addReplyBulkCString(c, "functions_count");
addReplyLongLong(c, e_stats->n_functions);
}
dictReleaseIterator(iter);
}
static void functionListReplyFlags(client *c, functionInfo *fi) {
int flagcount = 0;
for (scriptFlag *flag = scripts_flags_def; flag->str ; ++flag) {
if (fi->f_flags & flag->flag) {
++flagcount;
}
}
addReplySetLen(c, flagcount);
for (scriptFlag *flag = scripts_flags_def; flag->str ; ++flag) {
if (fi->f_flags & flag->flag) {
addReplyStatus(c, flag->str);
}
}
}
void functionListCommand(client *c) {
int with_code = 0;
sds library_name = NULL;
for (int i = 2 ; i < c->argc ; ++i) {
robj *next_arg = c->argv[i];
if (!with_code && !strcasecmp(next_arg->ptr, "withcode")) {
with_code = 1;
continue;
}
if (!library_name && !strcasecmp(next_arg->ptr, "libraryname")) {
if (i >= c->argc - 1) {
addReplyError(c, "library name argument was not given");
return;
}
library_name = c->argv[++i]->ptr;
continue;
}
addReplyErrorSds(c, sdscatfmt(sdsempty(), "Unknown argument %s", next_arg->ptr));
return;
}
size_t reply_len = 0;
void *len_ptr = NULL;
if (library_name) {
len_ptr = addReplyDeferredLen(c);
} else {
addReplyArrayLen(c, dictSize(curr_functions_lib_ctx->libraries));
}
dictIterator *iter = dictGetIterator(curr_functions_lib_ctx->libraries);
dictEntry *entry = NULL;
while ((entry = dictNext(iter))) {
functionLibInfo *li = dictGetVal(entry);
if (library_name) {
if (!stringmatchlen(library_name, sdslen(library_name), li->name, sdslen(li->name), 1)) {
continue;
}
}
++reply_len;
addReplyMapLen(c, with_code? 5 : 4);
addReplyBulkCString(c, "library_name");
addReplyBulkCBuffer(c, li->name, sdslen(li->name));
addReplyBulkCString(c, "engine");
addReplyBulkCBuffer(c, li->ei->name, sdslen(li->ei->name));
addReplyBulkCString(c, "description");
if (li->desc) {
addReplyBulkCBuffer(c, li->desc, sdslen(li->desc));
} else {
addReplyNull(c);
}
addReplyBulkCString(c, "functions");
addReplyArrayLen(c, dictSize(li->functions));
dictIterator *functions_iter = dictGetIterator(li->functions);
dictEntry *function_entry = NULL;
while ((function_entry = dictNext(functions_iter))) {
functionInfo *fi = dictGetVal(function_entry);
addReplyMapLen(c, 3);
addReplyBulkCString(c, "name");
addReplyBulkCBuffer(c, fi->name, sdslen(fi->name));
addReplyBulkCString(c, "description");
if (fi->desc) {
addReplyBulkCBuffer(c, fi->desc, sdslen(fi->desc));
} else {
addReplyNull(c);
}
addReplyBulkCString(c, "flags");
functionListReplyFlags(c, fi);
}
dictReleaseIterator(functions_iter);
if (with_code) {
addReplyBulkCString(c, "library_code");
addReplyBulkCBuffer(c, li->code, sdslen(li->code));
}
}
dictReleaseIterator(iter);
if (len_ptr) {
setDeferredArrayLen(c, len_ptr, reply_len);
}
}
void functionDeleteCommand(client *c) {
robj *function_name = c->argv[2];
functionLibInfo *li = dictFetchValue(curr_functions_lib_ctx->libraries, function_name->ptr);
if (!li) {
addReplyError(c, "Library not found");
return;
}
libraryUnlink(curr_functions_lib_ctx, li);
engineLibraryFree(li);
server.dirty++;
addReply(c, shared.ok);
}
void functionKillCommand(client *c) {
scriptKill(c, 0);
}
static void fcallCommandGeneric(client *c, int ro) {
robj *function_name = c->argv[1];
functionInfo *fi = dictFetchValue(curr_functions_lib_ctx->functions, function_name->ptr);
if (!fi) {
addReplyError(c, "Function not found");
return;
}
engine *engine = fi->li->ei->engine;
long long numkeys;
if (getLongLongFromObject(c->argv[2], &numkeys) != C_OK) {
addReplyError(c, "Bad number of keys provided");
return;
}
if (numkeys > (c->argc - 3)) {
addReplyError(c, "Number of keys can't be greater than number of args");
return;
} else if (numkeys < 0) {
addReplyError(c, "Number of keys can't be negative");
return;
}
scriptRunCtx run_ctx;
if (scriptPrepareForRun(&run_ctx, fi->li->ei->c, c, fi->name, fi->f_flags, ro) != C_OK)
return;
engine->call(&run_ctx, engine->engine_ctx, fi->function, c->argv + 3, numkeys,
c->argv + 3 + numkeys, c->argc - 3 - numkeys);
scriptResetRun(&run_ctx);
}
void fcallCommand(client *c) {
fcallCommandGeneric(c, 0);
}
void fcallroCommand(client *c) {
fcallCommandGeneric(c, 1);
}
void functionDumpCommand(client *c) {
unsigned char buf[2];
uint64_t crc;
rio payload;
rioInitWithBuffer(&payload, sdsempty());
rdbSaveFunctions(&payload);
buf[0] = RDB_VERSION & 0xff;
buf[1] = (RDB_VERSION >> 8) & 0xff;
payload.io.buffer.ptr = sdscatlen(payload.io.buffer.ptr, buf, 2);
crc = crc64(0, (unsigned char*) payload.io.buffer.ptr,
sdslen(payload.io.buffer.ptr));
memrev64ifbe(&crc);
payload.io.buffer.ptr = sdscatlen(payload.io.buffer.ptr, &crc, 8);
addReplyBulkSds(c, payload.io.buffer.ptr);
}
void functionRestoreCommand(client *c) {
if (c->argc > 4) {
addReplySubcommandSyntaxError(c);
return;
}
restorePolicy restore_replicy = restorePolicy_Append;
sds data = c->argv[2]->ptr;
size_t data_len = sdslen(data);
rio payload;
sds err = NULL;
if (c->argc == 4) {
const char *restore_policy_str = c->argv[3]->ptr;
if (!strcasecmp(restore_policy_str, "append")) {
restore_replicy = restorePolicy_Append;
} else if (!strcasecmp(restore_policy_str, "replace")) {
restore_replicy = restorePolicy_Replace;
} else if (!strcasecmp(restore_policy_str, "flush")) {
restore_replicy = restorePolicy_Flush;
} else {
addReplyError(c, "Wrong restore policy given, value should be either FLUSH, APPEND or REPLACE.");
return;
}
}
uint16_t rdbver;
if (verifyDumpPayload((unsigned char*)data, data_len, &rdbver) != C_OK) {
addReplyError(c, "DUMP payload version or checksum are wrong");
return;
}
functionsLibCtx *functions_lib_ctx = functionsLibCtxCreate();
rioInitWithBuffer(&payload, data);
while (data_len - payload.io.buffer.pos > 10) {
int type;
if ((type = rdbLoadType(&payload)) == -1) {
err = sdsnew("can not read data type");
goto load_error;
}
if (type != RDB_OPCODE_FUNCTION) {
err = sdsnew("given type is not a function");
goto load_error;
}
if (rdbFunctionLoad(&payload, rdbver, functions_lib_ctx, RDBFLAGS_NONE, &err) != C_OK) {
if (!err) {
err = sdsnew("failed loading the given functions payload");
}
goto load_error;
}
}
if (restore_replicy == restorePolicy_Flush) {
functionsLibCtxSwapWithCurrent(functions_lib_ctx);
functions_lib_ctx = NULL;
} else {
if (libraryJoin(curr_functions_lib_ctx, functions_lib_ctx, restore_replicy == restorePolicy_Replace, &err) != C_OK) {
goto load_error;
}
}
server.dirty++;
load_error:
if (err) {
addReplyErrorSds(c, err);
} else {
addReply(c, shared.ok);
}
if (functions_lib_ctx) {
functionsLibCtxFree(functions_lib_ctx);
}
}
void functionFlushCommand(client *c) {
if (c->argc > 3) {
addReplySubcommandSyntaxError(c);
return;
}
int async = 0;
if (c->argc == 3 && !strcasecmp(c->argv[2]->ptr,"sync")) {
async = 0;
} else if (c->argc == 3 && !strcasecmp(c->argv[2]->ptr,"async")) {
async = 1;
} else if (c->argc == 2) {
async = server.lazyfree_lazy_user_flush ? 1 : 0;
} else {
addReplyError(c,"FUNCTION FLUSH only supports SYNC|ASYNC option");
return;
}
functionsLibCtxClearCurrent(async);
server.dirty++;
addReply(c,shared.ok);
}
void functionHelpCommand(client *c) {
const char *help[] = {
"LOAD <ENGINE NAME> <LIBRARY NAME> [REPLACE] [DESC <LIBRARY DESCRIPTION>] <LIBRARY CODE>",
" Create a new library with the given library name and code.",
"DELETE <LIBRARY NAME>",
" Delete the given library.",
"LIST [LIBRARYNAME PATTERN] [WITHCODE]",
" Return general information on all the libraries:",
" * Library name",
" * The engine used to run the Library",
" * Library description",
" * Functions list",
" * Library code (if WITHCODE is given)",
" It also possible to get only function that matches a pattern using LIBRARYNAME argument.",
"STATS",
" Return information about the current function running:",
" * Function name",
" * Command used to run the function",
" * Duration in MS that the function is running",
" If no function is running, return nil",
" In addition, returns a list of available engines.",
"KILL",
" Kill the current running function.",
"FLUSH [ASYNC|SYNC]",
" Delete all the libraries.",
" When called without the optional mode argument, the behavior is determined by the",
" lazyfree-lazy-user-flush configuration directive. Valid modes are:",
" * ASYNC: Asynchronously flush the libraries.",
" * SYNC: Synchronously flush the libraries.",
"DUMP",
" Returns a serialized payload representing the current libraries, can be restored using FUNCTION RESTORE command",
"RESTORE <PAYLOAD> [FLUSH|APPEND|REPLACE]",
" Restore the libraries represented by the given payload, it is possible to give a restore policy to",
" control how to handle existing libraries (default APPEND):",
" * FLUSH: delete all existing libraries.",
" * APPEND: appends the restored libraries to the existing libraries. On collision, abort.",
" * REPLACE: appends the restored libraries to the existing libraries, On collision, replace the old",
" libraries with the new libraries (notice that even on this option there is a chance of failure",
" in case of functions name collision with another library).",
NULL };
addReplyHelp(c, help);
}
static int functionsVerifyName(sds name) {
if (sdslen(name) == 0) {
return C_ERR;
}
for (size_t i = 0 ; i < sdslen(name) ; ++i) {
char curr_char = name[i];
if ((curr_char >= 'a' && curr_char <= 'z') ||
(curr_char >= 'A' && curr_char <= 'Z') ||
(curr_char >= '0' && curr_char <= '9') ||
(curr_char == '_'))
{
continue;
}
return C_ERR;
}
return C_OK;
}
int functionsCreateWithLibraryCtx(sds lib_name,sds engine_name, sds desc, sds code,
int replace, sds* err, functionsLibCtx *lib_ctx) {
dictIterator *iter = NULL;
dictEntry *entry = NULL;
if (functionsVerifyName(lib_name)) {
*err = sdsnew("Library names can only contain letters and numbers and must be at least one character long");
return C_ERR;
}
engineInfo *ei = dictFetchValue(engines, engine_name);
if (!ei) {
*err = sdsnew("Engine not found");
return C_ERR;
}
engine *engine = ei->engine;
functionLibInfo *old_li = dictFetchValue(lib_ctx->libraries, lib_name);
if (old_li && !replace) {
*err = sdsnew("Library already exists");
return C_ERR;
}
if (old_li) {
libraryUnlink(lib_ctx, old_li);
}
functionLibInfo *new_li = engineLibraryCreate(lib_name, ei, desc, code);
if (engine->create(engine->engine_ctx, new_li, code, err) != C_OK) {
goto error;
}
if (dictSize(new_li->functions) == 0) {
*err = sdsnew("No functions registered");
goto error;
}
iter = dictGetIterator(new_li->functions);
while ((entry = dictNext(iter))) {
functionInfo *fi = dictGetVal(entry);
if (dictFetchValue(lib_ctx->functions, fi->name)) {
*err = sdscatfmt(sdsempty(), "Function %s already exists", fi->name);
goto error;
}
}
dictReleaseIterator(iter);
iter = NULL;
libraryLink(lib_ctx, new_li);
if (old_li) {
engineLibraryFree(old_li);
}
return C_OK;
error:
if (iter) dictReleaseIterator(iter);
engineLibraryFree(new_li);
if (old_li) {
libraryLink(lib_ctx, old_li);
}
return C_ERR;
}
void functionLoadCommand(client *c) {
robj *engine_name = c->argv[2];
robj *library_name = c->argv[3];
int replace = 0;
int argc_pos = 4;
sds desc = NULL;
while (argc_pos < c->argc - 1) {
robj *next_arg = c->argv[argc_pos++];
if (!strcasecmp(next_arg->ptr, "replace")) {
replace = 1;
continue;
}
if (!strcasecmp(next_arg->ptr, "description")) {
if (argc_pos >= c->argc) {
addReplyError(c, "Bad function description");
return;
}
desc = c->argv[argc_pos++]->ptr;
continue;
}
addReplyErrorFormat(c, "Unknown option given: %s", (char*)next_arg->ptr);
return;
}
if (argc_pos >= c->argc) {
addReplyError(c, "Function code is missing");
return;
}
robj *code = c->argv[argc_pos];
sds err = NULL;
if (functionsCreateWithLibraryCtx(library_name->ptr, engine_name->ptr,
desc, code->ptr, replace, &err, curr_functions_lib_ctx) != C_OK)
{
addReplyErrorSds(c, err);
return;
}
server.dirty++;
addReply(c, shared.ok);
}
unsigned long functionsMemory() {
dictIterator *iter = dictGetIterator(engines);
dictEntry *entry = NULL;
size_t engines_nemory = 0;
while ((entry = dictNext(iter))) {
engineInfo *ei = dictGetVal(entry);
engine *engine = ei->engine;
engines_nemory += engine->get_used_memory(engine->engine_ctx);
}
dictReleaseIterator(iter);
return engines_nemory;
}
unsigned long functionsMemoryOverhead() {
size_t memory_overhead = dictSize(engines) * sizeof(dictEntry) +
dictSlots(engines) * sizeof(dictEntry*);
memory_overhead += dictSize(curr_functions_lib_ctx->functions) * sizeof(dictEntry) +
dictSlots(curr_functions_lib_ctx->functions) * sizeof(dictEntry*) + sizeof(functionsLibCtx);
memory_overhead += curr_functions_lib_ctx->cache_memory;
memory_overhead += engine_cache_memory;
return memory_overhead;
}
unsigned long functionsNum() {
return dictSize(curr_functions_lib_ctx->functions);
}
unsigned long functionsLibNum() {
return dictSize(curr_functions_lib_ctx->libraries);
}
dict* functionsLibGet() {
return curr_functions_lib_ctx->libraries;
}
size_t functionsLibCtxfunctionsLen(functionsLibCtx *functions_ctx) {
return dictSize(functions_ctx->functions);
}
int functionsInit() {
engines = dictCreate(&engineDictType);
if (luaEngineInitEngine() != C_OK) {
return C_ERR;
}
curr_functions_lib_ctx = functionsLibCtxCreate();
return C_OK;
}
