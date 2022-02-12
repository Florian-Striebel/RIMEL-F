





























#include <sys/cdefs.h>
#include <sys/types.h>
#include <stdio.h>

#include "unwind.h"
#include "execinfo.h"

struct tracer_context {
void **arr;
size_t len;
size_t n;
};

static _Unwind_Reason_Code
tracer(struct _Unwind_Context *ctx, void *arg)
{
struct tracer_context *t = arg;
if (t->n == (size_t)~0) {

t->n = 0;
return 0;
}
if (t->n < t->len)
t->arr[t->n++] = (void *)_Unwind_GetIP(ctx);
else
return _URC_END_OF_STACK;
return _URC_NO_REASON;
}

size_t
backtrace(void **arr, size_t len)
{
struct tracer_context ctx;

ctx.arr = arr;
ctx.len = len;
ctx.n = (size_t)~0;

_Unwind_Backtrace(tracer, &ctx);
if (ctx.n == (size_t)~0)
ctx.n = 0;
else if (ctx.n > 0)
ctx.arr[--ctx.n] = NULL;

return ctx.n;
}
