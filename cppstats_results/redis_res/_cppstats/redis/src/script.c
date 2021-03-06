#include "server.h"
#include "script.h"
#include "cluster.h"
scriptFlag scripts_flags_def[] = {
{.flag = SCRIPT_FLAG_NO_WRITES, .str = "no-writes"},
{.flag = SCRIPT_FLAG_ALLOW_OOM, .str = "allow-oom"},
{.flag = SCRIPT_FLAG_ALLOW_STALE, .str = "allow-stale"},
{.flag = SCRIPT_FLAG_NO_CLUSTER, .str = "no-cluster"},
{.flag = 0, .str = NULL},
};
static scriptRunCtx *curr_run_ctx = NULL;
static void exitScriptTimedoutMode(scriptRunCtx *run_ctx) {
serverAssert(run_ctx == curr_run_ctx);
serverAssert(scriptIsTimedout());
run_ctx->flags &= ~SCRIPT_TIMEDOUT;
blockingOperationEnds();
if (server.masterhost && server.master) queueClientForReprocessing(server.master);
}
static void enterScriptTimedoutMode(scriptRunCtx *run_ctx) {
serverAssert(run_ctx == curr_run_ctx);
serverAssert(!scriptIsTimedout());
run_ctx->flags |= SCRIPT_TIMEDOUT;
blockingOperationStarts();
}
int scriptIsTimedout() {
return scriptIsRunning() && (curr_run_ctx->flags & SCRIPT_TIMEDOUT);
}
client* scriptGetClient() {
serverAssert(scriptIsRunning());
return curr_run_ctx->c;
}
client* scriptGetCaller() {
serverAssert(scriptIsRunning());
return curr_run_ctx->original_client;
}
int scriptInterrupt(scriptRunCtx *run_ctx) {
if (run_ctx->flags & SCRIPT_TIMEDOUT) {
processEventsWhileBlocked();
return (run_ctx->flags & SCRIPT_KILLED) ? SCRIPT_KILL : SCRIPT_CONTINUE;
}
long long elapsed = elapsedMs(run_ctx->start_time);
if (elapsed < server.busy_reply_threshold) {
return SCRIPT_CONTINUE;
}
serverLog(LL_WARNING,
"Slow script detected: still in execution after %lld milliseconds. "
"You can try killing the script using the %s command.",
elapsed, (run_ctx->flags & SCRIPT_EVAL_MODE) ? "SCRIPT KILL" : "FUNCTION KILL");
enterScriptTimedoutMode(run_ctx);
protectClient(run_ctx->original_client);
processEventsWhileBlocked();
return (run_ctx->flags & SCRIPT_KILLED) ? SCRIPT_KILL : SCRIPT_CONTINUE;
}
int scriptPrepareForRun(scriptRunCtx *run_ctx, client *engine_client, client *caller, const char *funcname, uint64_t script_flags, int ro) {
serverAssert(!curr_run_ctx);
int running_stale = server.masterhost &&
server.repl_state != REPL_STATE_CONNECTED &&
server.repl_serve_stale_data == 0;
if (!(script_flags & SCRIPT_FLAG_EVAL_COMPAT_MODE)) {
if ((script_flags & SCRIPT_FLAG_NO_CLUSTER) && server.cluster_enabled) {
addReplyError(caller, "Can not run script on cluster, 'no-cluster' flag is set.");
return C_ERR;
}
if (!(script_flags & SCRIPT_FLAG_ALLOW_OOM) && server.script_oom && server.maxmemory) {
addReplyError(caller, "-OOM allow-oom flag is not set on the script, "
"can not run it when used memory > 'maxmemory'");
return C_ERR;
}
if (running_stale && !(script_flags & SCRIPT_FLAG_ALLOW_STALE)) {
addReplyError(caller, "-MASTERDOWN Link with MASTER is down, "
"replica-serve-stale-data is set to 'no' "
"and 'allow-stale' flag is not set on the script.");
return C_ERR;
}
if (!(script_flags & SCRIPT_FLAG_NO_WRITES)) {
if (server.masterhost && server.repl_slave_ro && caller->id != CLIENT_ID_AOF
&& !(caller->flags & CLIENT_MASTER))
{
addReplyError(caller, "Can not run script with write flag on readonly replica");
return C_ERR;
}
int deny_write_type = writeCommandsDeniedByDiskError();
if (deny_write_type != DISK_ERROR_TYPE_NONE && server.masterhost == NULL) {
if (deny_write_type == DISK_ERROR_TYPE_RDB)
addReplyError(caller, "-MISCONF Redis is configured to save RDB snapshots, "
"but it's currently unable to persist to disk. "
"Writable scripts are blocked. Use 'no-writes' flag for read only scripts.");
else
addReplyErrorFormat(caller, "-MISCONF Redis is configured to persist data to AOF, "
"but it's currently unable to persist to disk. "
"Writable scripts are blocked. Use 'no-writes' flag for read only scripts. "
"AOF error: %s", strerror(server.aof_last_write_errno));
return C_ERR;
}
if (ro) {
addReplyError(caller, "Can not execute a script with write flag using *_ro command.");
return C_ERR;
}
}
} else {
if (running_stale) {
addReplyErrorObject(caller, shared.masterdownerr);
return C_ERR;
}
}
run_ctx->c = engine_client;
run_ctx->original_client = caller;
run_ctx->funcname = funcname;
client *script_client = run_ctx->c;
client *curr_client = run_ctx->original_client;
server.script_caller = curr_client;
selectDb(script_client, curr_client->db->id);
script_client->resp = 2;
if (curr_client->flags & CLIENT_MULTI) {
script_client->flags |= CLIENT_MULTI;
}
server.in_script = 1;
run_ctx->start_time = getMonotonicUs();
run_ctx->snapshot_time = mstime();
run_ctx->flags = 0;
run_ctx->repl_flags = PROPAGATE_AOF | PROPAGATE_REPL;
if (ro || (!(script_flags & SCRIPT_FLAG_EVAL_COMPAT_MODE) && (script_flags & SCRIPT_FLAG_NO_WRITES))) {
run_ctx->flags |= SCRIPT_READ_ONLY;
}
if (!(script_flags & SCRIPT_FLAG_EVAL_COMPAT_MODE) && (script_flags & SCRIPT_FLAG_ALLOW_OOM)) {
run_ctx->flags |= SCRIPT_ALLOW_OOM;
}
curr_run_ctx = run_ctx;
return C_OK;
}
void scriptResetRun(scriptRunCtx *run_ctx) {
serverAssert(curr_run_ctx);
run_ctx->c->flags &= ~CLIENT_MULTI;
server.in_script = 0;
server.script_caller = NULL;
if (scriptIsTimedout()) {
exitScriptTimedoutMode(run_ctx);
unprotectClient(run_ctx->original_client);
}
preventCommandPropagation(run_ctx->original_client);
curr_run_ctx = NULL;
}
int scriptIsRunning() {
return curr_run_ctx != NULL;
}
const char* scriptCurrFunction() {
serverAssert(scriptIsRunning());
return curr_run_ctx->funcname;
}
int scriptIsEval() {
serverAssert(scriptIsRunning());
return curr_run_ctx->flags & SCRIPT_EVAL_MODE;
}
void scriptKill(client *c, int is_eval) {
if (!curr_run_ctx) {
addReplyError(c, "-NOTBUSY No scripts in execution right now.");
return;
}
if (curr_run_ctx->original_client->flags & CLIENT_MASTER) {
addReplyError(c,
"-UNKILLABLE The busy script was sent by a master instance in the context of replication and cannot be killed.");
}
if (curr_run_ctx->flags & SCRIPT_WRITE_DIRTY) {
addReplyError(c,
"-UNKILLABLE Sorry the script already executed write "
"commands against the dataset. You can either wait the "
"script termination or kill the server in a hard way "
"using the SHUTDOWN NOSAVE command.");
return;
}
if (is_eval && !(curr_run_ctx->flags & SCRIPT_EVAL_MODE)) {
addReplyErrorObject(c, shared.slowscripterr);
return;
}
if (!is_eval && (curr_run_ctx->flags & SCRIPT_EVAL_MODE)) {
addReplyErrorObject(c, shared.slowevalerr);
return;
}
curr_run_ctx->flags |= SCRIPT_KILLED;
addReply(c, shared.ok);
}
static int scriptVerifyCommandArity(struct redisCommand *cmd, int argc, sds *err) {
if (!cmd || ((cmd->arity > 0 && cmd->arity != argc) || (argc < -cmd->arity))) {
if (cmd)
*err = sdsnew("Wrong number of args calling Redis command from script");
else
*err = sdsnew("Unknown Redis command called from script");
return C_ERR;
}
return C_OK;
}
static int scriptVerifyACL(client *c, sds *err) {
int acl_errpos;
int acl_retval = ACLCheckAllPerm(c, &acl_errpos);
if (acl_retval != ACL_OK) {
addACLLogEntry(c,acl_retval,ACL_LOG_CTX_LUA,acl_errpos,NULL,NULL);
switch (acl_retval) {
case ACL_DENIED_CMD:
*err = sdsnew("The user executing the script can't run this "
"command or subcommand");
break;
case ACL_DENIED_KEY:
*err = sdsnew("The user executing the script can't access "
"at least one of the keys mentioned in the "
"command arguments");
break;
case ACL_DENIED_CHANNEL:
*err = sdsnew("The user executing the script can't publish "
"to the channel mentioned in the command");
break;
default:
*err = sdsnew("The user executing the script is lacking the "
"permissions for the command");
break;
}
return C_ERR;
}
return C_OK;
}
static int scriptVerifyWriteCommandAllow(scriptRunCtx *run_ctx, char **err) {
if (!(run_ctx->c->cmd->flags & CMD_WRITE)) {
return C_OK;
}
if (run_ctx->flags & SCRIPT_READ_ONLY) {
*err = sdsnew("Write commands are not allowed from read-only scripts.");
return C_ERR;
}
int deny_write_type = writeCommandsDeniedByDiskError();
if (server.masterhost && server.repl_slave_ro && run_ctx->original_client->id != CLIENT_ID_AOF
&& !(run_ctx->original_client->flags & CLIENT_MASTER))
{
*err = sdsdup(shared.roslaveerr->ptr);
return C_ERR;
}
if (deny_write_type != DISK_ERROR_TYPE_NONE) {
if (deny_write_type == DISK_ERROR_TYPE_RDB) {
*err = sdsdup(shared.bgsaveerr->ptr);
} else {
*err = sdsempty();
*err = sdscatfmt(*err,
"-MISCONF Errors writing to the AOF file: %s\r\n",
strerror(server.aof_last_write_errno));
}
return C_ERR;
}
return C_OK;
}
static int scriptVerifyOOM(scriptRunCtx *run_ctx, char **err) {
if (run_ctx->flags & SCRIPT_ALLOW_OOM) {
return C_OK;
}
if (server.maxmemory &&
run_ctx->original_client->id != CLIENT_ID_AOF &&
!server.masterhost &&
!(run_ctx->flags & SCRIPT_WRITE_DIRTY) &&
server.script_oom &&
(run_ctx->c->cmd->flags & CMD_DENYOOM))
{
*err = sdsdup(shared.oomerr->ptr);
return C_ERR;
}
return C_OK;
}
static int scriptVerifyClusterState(client *c, client *original_c, sds *err) {
if (!server.cluster_enabled || original_c->id == CLIENT_ID_AOF || (original_c->flags & CLIENT_MASTER)) {
return C_OK;
}
int error_code;
c->flags &= ~(CLIENT_READONLY | CLIENT_ASKING);
c->flags |= original_c->flags & (CLIENT_READONLY | CLIENT_ASKING);
if (getNodeByQuery(c, c->cmd, c->argv, c->argc, NULL, &error_code) != server.cluster->myself) {
if (error_code == CLUSTER_REDIR_DOWN_RO_STATE) {
*err = sdsnew(
"Script attempted to execute a write command while the "
"cluster is down and readonly");
} else if (error_code == CLUSTER_REDIR_DOWN_STATE) {
*err = sdsnew("Script attempted to execute a command while the "
"cluster is down");
} else {
*err = sdsnew("Script attempted to access a non local key in a "
"cluster node");
}
return C_ERR;
}
return C_OK;
}
int scriptSetResp(scriptRunCtx *run_ctx, int resp) {
if (resp != 2 && resp != 3) {
return C_ERR;
}
run_ctx->c->resp = resp;
return C_OK;
}
int scriptSetRepl(scriptRunCtx *run_ctx, int repl) {
if ((repl & ~(PROPAGATE_AOF | PROPAGATE_REPL)) != 0) {
return C_ERR;
}
run_ctx->repl_flags = repl;
return C_OK;
}
static int scriptVerifyAllowStale(client *c, sds *err) {
if (!server.masterhost) {
return C_OK;
}
if (server.repl_state == REPL_STATE_CONNECTED) {
return C_OK;
}
if (server.repl_serve_stale_data == 1) {
return C_OK;
}
if (c->cmd->flags & CMD_STALE) {
return C_OK;
}
*err = sdsnew("Can not execute the command on a stale replica");
return C_ERR;
}
void scriptCall(scriptRunCtx *run_ctx, robj* *argv, int argc, sds *err) {
client *c = run_ctx->c;
c->argv = argv;
c->argc = argc;
c->user = run_ctx->original_client->user;
moduleCallCommandFilters(c);
argv = c->argv;
argc = c->argc;
struct redisCommand *cmd = lookupCommand(argv, argc);
if (scriptVerifyCommandArity(cmd, argc, err) != C_OK) {
return;
}
c->cmd = c->lastcmd = cmd;
if (!server.script_disable_deny_script && (cmd->flags & CMD_NOSCRIPT)) {
*err = sdsnew("This Redis command is not allowed from script");
return;
}
if (scriptVerifyAllowStale(c, err) != C_OK) {
return;
}
if (scriptVerifyACL(c, err) != C_OK) {
return;
}
if (scriptVerifyWriteCommandAllow(run_ctx, err) != C_OK) {
return;
}
if (scriptVerifyOOM(run_ctx, err) != C_OK) {
return;
}
if (cmd->flags & CMD_WRITE) {
run_ctx->flags |= SCRIPT_WRITE_DIRTY;
}
if (scriptVerifyClusterState(c, run_ctx->original_client, err) != C_OK) {
return;
}
int call_flags = CMD_CALL_SLOWLOG | CMD_CALL_STATS;
if (run_ctx->repl_flags & PROPAGATE_AOF) {
call_flags |= CMD_CALL_PROPAGATE_AOF;
}
if (run_ctx->repl_flags & PROPAGATE_REPL) {
call_flags |= CMD_CALL_PROPAGATE_REPL;
}
call(c, call_flags);
serverAssert((c->flags & CLIENT_BLOCKED) == 0);
}
mstime_t scriptTimeSnapshot() {
serverAssert(curr_run_ctx);
return curr_run_ctx->snapshot_time;
}
long long scriptRunDuration() {
serverAssert(scriptIsRunning());
return elapsedMs(curr_run_ctx->start_time);
}
