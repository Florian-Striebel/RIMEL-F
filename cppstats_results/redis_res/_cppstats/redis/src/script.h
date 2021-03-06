#define SCRIPT_KILL 1
#define SCRIPT_CONTINUE 2
#define SCRIPT_WRITE_DIRTY (1ULL<<0)
#define SCRIPT_TIMEDOUT (1ULL<<3)
#define SCRIPT_KILLED (1ULL<<4)
#define SCRIPT_READ_ONLY (1ULL<<5)
#define SCRIPT_ALLOW_OOM (1ULL<<6)
#define SCRIPT_EVAL_MODE (1ULL<<7)
typedef struct scriptRunCtx scriptRunCtx;
struct scriptRunCtx {
const char *funcname;
client *c;
client *original_client;
int flags;
int repl_flags;
monotime start_time;
mstime_t snapshot_time;
};
#define SCRIPT_FLAG_NO_WRITES (1ULL<<0)
#define SCRIPT_FLAG_ALLOW_OOM (1ULL<<1)
#define SCRIPT_FLAG_ALLOW_STALE (1ULL<<2)
#define SCRIPT_FLAG_NO_CLUSTER (1ULL<<3)
#define SCRIPT_FLAG_EVAL_COMPAT_MODE (1ULL<<4)
typedef struct scriptFlag {
uint64_t flag;
const char *str;
} scriptFlag;
extern scriptFlag scripts_flags_def[];
int scriptPrepareForRun(scriptRunCtx *r_ctx, client *engine_client, client *caller, const char *funcname, uint64_t script_flags, int ro);
void scriptResetRun(scriptRunCtx *r_ctx);
int scriptSetResp(scriptRunCtx *r_ctx, int resp);
int scriptSetRepl(scriptRunCtx *r_ctx, int repl);
void scriptCall(scriptRunCtx *r_ctx, robj **argv, int argc, sds *err);
int scriptInterrupt(scriptRunCtx *r_ctx);
void scriptKill(client *c, int is_eval);
int scriptIsRunning();
const char* scriptCurrFunction();
int scriptIsEval();
int scriptIsTimedout();
client* scriptGetClient();
client* scriptGetCaller();
mstime_t scriptTimeSnapshot();
long long scriptRunDuration();
