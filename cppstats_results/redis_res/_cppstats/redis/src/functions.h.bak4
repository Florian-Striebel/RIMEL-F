




























#if !defined(__FUNCTIONS_H_)
#define __FUNCTIONS_H_














#include "server.h"
#include "script.h"
#include "redismodule.h"

typedef struct functionLibInfo functionLibInfo;

typedef struct engine {

void *engine_ctx;



int (*create)(void *engine_ctx, functionLibInfo *li, sds code, sds *err);






void (*call)(scriptRunCtx *r_ctx, void *engine_ctx, void *compiled_function,
robj **keys, size_t nkeys, robj **args, size_t nargs);


size_t (*get_used_memory)(void *engine_ctx);




size_t (*get_function_memory_overhead)(void *compiled_function);


size_t (*get_engine_memory_overhead)(void *engine_ctx);


void (*free_function)(void *engine_ctx, void *compiled_function);
} engine;



typedef struct engineInfo {
sds name;
engine *engine;
client *c;
} engineInfo;



typedef struct functionInfo {
sds name;
void *function;

functionLibInfo* li;
sds desc;
uint64_t f_flags;
} functionInfo;



struct functionLibInfo {
sds name;
dict *functions;
engineInfo *ei;
sds code;
sds desc;
};

int functionsRegisterEngine(const char *engine_name, engine *engine_ctx);
int functionsCreateWithLibraryCtx(sds lib_name, sds engine_name, sds desc, sds code,
int replace, sds* err, functionsLibCtx *lib_ctx);
unsigned long functionsMemory();
unsigned long functionsMemoryOverhead();
unsigned long functionsNum();
unsigned long functionsLibNum();
dict* functionsLibGet();
size_t functionsLibCtxfunctionsLen(functionsLibCtx *functions_ctx);
functionsLibCtx* functionsLibCtxGetCurrent();
functionsLibCtx* functionsLibCtxCreate();
void functionsLibCtxClearCurrent(int async);
void functionsLibCtxFree(functionsLibCtx *lib_ctx);
void functionsLibCtxClear(functionsLibCtx *lib_ctx);
void functionsLibCtxSwapWithCurrent(functionsLibCtx *lib_ctx);

int functionLibCreateFunction(sds name, void *function, functionLibInfo *li, sds desc, uint64_t f_flags, sds *err);

int luaEngineInitEngine();
int functionsInit();

#endif
