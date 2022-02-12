






















#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "ucl.h"
#include "ucl_internal.h"
#include "ucl_chartable.h"

struct ucl_emitter_streamline_stack {
bool is_array;
bool empty;
const ucl_object_t *obj;
struct ucl_emitter_streamline_stack *next;
};

struct ucl_emitter_context_streamline {


const char *name;

int id;

const struct ucl_emitter_functions *func;

const struct ucl_emitter_operations *ops;

unsigned int indent;

const ucl_object_t *top;

const ucl_object_t *comments;


struct ucl_emitter_streamline_stack *containers;
};

#define TO_STREAMLINE(ctx) (struct ucl_emitter_context_streamline *)(ctx)

struct ucl_emitter_context*
ucl_object_emit_streamline_new (const ucl_object_t *obj,
enum ucl_emitter emit_type,
struct ucl_emitter_functions *emitter)
{
const struct ucl_emitter_context *ctx;
struct ucl_emitter_context_streamline *sctx;

ctx = ucl_emit_get_standard_context (emit_type);
if (ctx == NULL) {
return NULL;
}

sctx = calloc (1, sizeof (*sctx));
if (sctx == NULL) {
return NULL;
}

memcpy (sctx, ctx, sizeof (*ctx));
sctx->func = emitter;
sctx->top = obj;

ucl_object_emit_streamline_start_container ((struct ucl_emitter_context *)sctx,
obj);

return (struct ucl_emitter_context *)sctx;
}

void
ucl_object_emit_streamline_start_container (struct ucl_emitter_context *ctx,
const ucl_object_t *obj)
{
struct ucl_emitter_context_streamline *sctx = TO_STREAMLINE(ctx);
struct ucl_emitter_streamline_stack *st, *top;
bool print_key = false;


if (sctx->top == NULL) {
sctx->top = obj;
}

top = sctx->containers;
st = malloc (sizeof (*st));
if (st != NULL) {
if (top != NULL && !top->is_array) {
print_key = true;
}
st->empty = true;
st->obj = obj;
if (obj != NULL && obj->type == UCL_ARRAY) {
st->is_array = true;
sctx->ops->ucl_emitter_start_array (ctx, obj, print_key);
}
else {
st->is_array = false;
sctx->ops->ucl_emitter_start_object (ctx, obj, print_key);
}
LL_PREPEND (sctx->containers, st);
}
}

void
ucl_object_emit_streamline_add_object (
struct ucl_emitter_context *ctx, const ucl_object_t *obj)
{
struct ucl_emitter_context_streamline *sctx = TO_STREAMLINE(ctx);
bool is_array = false, is_first = false;

if (sctx->containers != NULL) {
if (sctx->containers->is_array) {
is_array = true;
}
if (sctx->containers->empty) {
is_first = true;
sctx->containers->empty = false;
}
}

sctx->ops->ucl_emitter_write_elt (ctx, obj, is_first, !is_array);
}

void
ucl_object_emit_streamline_end_container (struct ucl_emitter_context *ctx)
{
struct ucl_emitter_context_streamline *sctx = TO_STREAMLINE(ctx);
struct ucl_emitter_streamline_stack *st;

if (sctx->containers != NULL) {
st = sctx->containers;

if (st->is_array) {
sctx->ops->ucl_emitter_end_array (ctx, st->obj);
}
else {
sctx->ops->ucl_emitter_end_object (ctx, st->obj);
}
sctx->containers = st->next;
free (st);
}
}

void
ucl_object_emit_streamline_finish (struct ucl_emitter_context *ctx)
{
struct ucl_emitter_context_streamline *sctx = TO_STREAMLINE(ctx);

while (sctx->containers != NULL) {
ucl_object_emit_streamline_end_container (ctx);
}

free (sctx);
}
