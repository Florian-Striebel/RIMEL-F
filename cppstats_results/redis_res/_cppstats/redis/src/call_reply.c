#include "server.h"
#include "call_reply.h"
#define REPLY_FLAG_ROOT (1<<0)
#define REPLY_FLAG_PARSED (1<<1)
#define REPLY_FLAG_RESP3 (1<<2)
struct CallReply {
void *private_data;
sds original_proto;
const char *proto;
size_t proto_len;
int type;
int flags;
size_t len;
union {
const char *str;
struct {
const char *str;
const char *format;
} verbatim_str;
long long ll;
double d;
struct CallReply *array;
} val;
struct CallReply *attribute;
};
static void callReplySetSharedData(CallReply *rep, int type, const char *proto, size_t proto_len, int extra_flags) {
rep->type = type;
rep->proto = proto;
rep->proto_len = proto_len;
rep->flags |= extra_flags;
}
static void callReplyNull(void *ctx, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_NULL, proto, proto_len, REPLY_FLAG_RESP3);
}
static void callReplyNullBulkString(void *ctx, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_NULL, proto, proto_len, 0);
}
static void callReplyNullArray(void *ctx, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_NULL, proto, proto_len, 0);
}
static void callReplyBulkString(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_STRING, proto, proto_len, 0);
rep->len = len;
rep->val.str = str;
}
static void callReplyError(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_ERROR, proto, proto_len, 0);
rep->len = len;
rep->val.str = str;
}
static void callReplySimpleStr(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_STRING, proto, proto_len, 0);
rep->len = len;
rep->val.str = str;
}
static void callReplyLong(void *ctx, long long val, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_INTEGER, proto, proto_len, 0);
rep->val.ll = val;
}
static void callReplyDouble(void *ctx, double val, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_DOUBLE, proto, proto_len, REPLY_FLAG_RESP3);
rep->val.d = val;
}
static void callReplyVerbatimString(void *ctx, const char *format, const char *str, size_t len, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_VERBATIM_STRING, proto, proto_len, REPLY_FLAG_RESP3);
rep->len = len;
rep->val.verbatim_str.str = str;
rep->val.verbatim_str.format = format;
}
static void callReplyBigNumber(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_BIG_NUMBER, proto, proto_len, REPLY_FLAG_RESP3);
rep->len = len;
rep->val.str = str;
}
static void callReplyBool(void *ctx, int val, const char *proto, size_t proto_len) {
CallReply *rep = ctx;
callReplySetSharedData(rep, REDISMODULE_REPLY_BOOL, proto, proto_len, REPLY_FLAG_RESP3);
rep->val.ll = val;
}
static void callReplyParseCollection(ReplyParser *parser, CallReply *rep, size_t len, const char *proto, size_t elements_per_entry) {
rep->len = len;
rep->val.array = zcalloc(elements_per_entry * len * sizeof(CallReply));
for (size_t i = 0; i < len * elements_per_entry; i += elements_per_entry) {
for (size_t j = 0 ; j < elements_per_entry ; ++j) {
rep->val.array[i + j].private_data = rep->private_data;
parseReply(parser, rep->val.array + i + j);
rep->val.array[i + j].flags |= REPLY_FLAG_PARSED;
if (rep->val.array[i + j].flags & REPLY_FLAG_RESP3) {
rep->flags |= REPLY_FLAG_RESP3;
}
}
}
rep->proto = proto;
rep->proto_len = parser->curr_location - proto;
}
static void callReplyAttribute(ReplyParser *parser, void *ctx, size_t len, const char *proto) {
CallReply *rep = ctx;
rep->attribute = zcalloc(sizeof(CallReply));
rep->attribute->len = len;
rep->attribute->type = REDISMODULE_REPLY_ATTRIBUTE;
callReplyParseCollection(parser, rep->attribute, len, proto, 2);
rep->attribute->flags |= REPLY_FLAG_PARSED | REPLY_FLAG_RESP3;
rep->attribute->private_data = rep->private_data;
parseReply(parser, rep);
rep->proto = proto;
rep->proto_len = parser->curr_location - proto;
rep->flags |= REPLY_FLAG_RESP3;
}
static void callReplyArray(ReplyParser *parser, void *ctx, size_t len, const char *proto) {
CallReply *rep = ctx;
rep->type = REDISMODULE_REPLY_ARRAY;
callReplyParseCollection(parser, rep, len, proto, 1);
}
static void callReplySet(ReplyParser *parser, void *ctx, size_t len, const char *proto) {
CallReply *rep = ctx;
rep->type = REDISMODULE_REPLY_SET;
callReplyParseCollection(parser, rep, len, proto, 1);
rep->flags |= REPLY_FLAG_RESP3;
}
static void callReplyMap(ReplyParser *parser, void *ctx, size_t len, const char *proto) {
CallReply *rep = ctx;
rep->type = REDISMODULE_REPLY_MAP;
callReplyParseCollection(parser, rep, len, proto, 2);
rep->flags |= REPLY_FLAG_RESP3;
}
static void callReplyParseError(void *ctx) {
CallReply *rep = ctx;
rep->type = REDISMODULE_REPLY_UNKNOWN;
}
static void freeCallReplyInternal(CallReply *rep) {
if (rep->type == REDISMODULE_REPLY_ARRAY || rep->type == REDISMODULE_REPLY_SET) {
for (size_t i = 0 ; i < rep->len ; ++i) {
freeCallReplyInternal(rep->val.array + i);
}
zfree(rep->val.array);
}
if (rep->type == REDISMODULE_REPLY_MAP || rep->type == REDISMODULE_REPLY_ATTRIBUTE) {
for (size_t i = 0 ; i < rep->len ; ++i) {
freeCallReplyInternal(rep->val.array + i * 2);
freeCallReplyInternal(rep->val.array + i * 2 + 1);
}
zfree(rep->val.array);
}
if (rep->attribute) {
freeCallReplyInternal(rep->attribute);
zfree(rep->attribute);
}
}
void freeCallReply(CallReply *rep) {
if (!(rep->flags & REPLY_FLAG_ROOT)) {
return;
}
if (rep->flags & REPLY_FLAG_PARSED) {
freeCallReplyInternal(rep);
}
sdsfree(rep->original_proto);
zfree(rep);
}
static const ReplyParserCallbacks DefaultParserCallbacks = {
.null_callback = callReplyNull,
.bulk_string_callback = callReplyBulkString,
.null_bulk_string_callback = callReplyNullBulkString,
.null_array_callback = callReplyNullArray,
.error_callback = callReplyError,
.simple_str_callback = callReplySimpleStr,
.long_callback = callReplyLong,
.array_callback = callReplyArray,
.set_callback = callReplySet,
.map_callback = callReplyMap,
.double_callback = callReplyDouble,
.bool_callback = callReplyBool,
.big_number_callback = callReplyBigNumber,
.verbatim_string_callback = callReplyVerbatimString,
.attribute_callback = callReplyAttribute,
.error = callReplyParseError,
};
static void callReplyParse(CallReply *rep) {
if (rep->flags & REPLY_FLAG_PARSED) {
return;
}
ReplyParser parser = {.curr_location = rep->proto, .callbacks = DefaultParserCallbacks};
parseReply(&parser, rep);
rep->flags |= REPLY_FLAG_PARSED;
}
int callReplyType(CallReply *rep) {
if (!rep) return REDISMODULE_REPLY_UNKNOWN;
callReplyParse(rep);
return rep->type;
}
const char *callReplyGetString(CallReply *rep, size_t *len) {
callReplyParse(rep);
if (rep->type != REDISMODULE_REPLY_STRING &&
rep->type != REDISMODULE_REPLY_ERROR) return NULL;
if (len) *len = rep->len;
return rep->val.str;
}
long long callReplyGetLongLong(CallReply *rep) {
callReplyParse(rep);
if (rep->type != REDISMODULE_REPLY_INTEGER) return LLONG_MIN;
return rep->val.ll;
}
double callReplyGetDouble(CallReply *rep) {
callReplyParse(rep);
if (rep->type != REDISMODULE_REPLY_DOUBLE) return LLONG_MIN;
return rep->val.d;
}
int callReplyGetBool(CallReply *rep) {
callReplyParse(rep);
if (rep->type != REDISMODULE_REPLY_BOOL) return INT_MIN;
return rep->val.ll;
}
size_t callReplyGetLen(CallReply *rep) {
callReplyParse(rep);
switch(rep->type) {
case REDISMODULE_REPLY_STRING:
case REDISMODULE_REPLY_ERROR:
case REDISMODULE_REPLY_ARRAY:
case REDISMODULE_REPLY_SET:
case REDISMODULE_REPLY_MAP:
case REDISMODULE_REPLY_ATTRIBUTE:
return rep->len;
default:
return 0;
}
}
static CallReply *callReplyGetCollectionElement(CallReply *rep, size_t idx, int elements_per_entry) {
if (idx >= rep->len * elements_per_entry) return NULL;
return rep->val.array+idx;
}
CallReply *callReplyGetArrayElement(CallReply *rep, size_t idx) {
callReplyParse(rep);
if (rep->type != REDISMODULE_REPLY_ARRAY) return NULL;
return callReplyGetCollectionElement(rep, idx, 1);
}
CallReply *callReplyGetSetElement(CallReply *rep, size_t idx) {
callReplyParse(rep);
if (rep->type != REDISMODULE_REPLY_SET) return NULL;
return callReplyGetCollectionElement(rep, idx, 1);
}
static int callReplyGetMapElementInternal(CallReply *rep, size_t idx, CallReply **key, CallReply **val, int type) {
callReplyParse(rep);
if (rep->type != type) return C_ERR;
if (idx >= rep->len) return C_ERR;
if (key) *key = callReplyGetCollectionElement(rep, idx * 2, 2);
if (val) *val = callReplyGetCollectionElement(rep, idx * 2 + 1, 2);
return C_OK;
}
int callReplyGetMapElement(CallReply *rep, size_t idx, CallReply **key, CallReply **val) {
return callReplyGetMapElementInternal(rep, idx, key, val, REDISMODULE_REPLY_MAP);
}
CallReply *callReplyGetAttribute(CallReply *rep) {
return rep->attribute;
}
int callReplyGetAttributeElement(CallReply *rep, size_t idx, CallReply **key, CallReply **val) {
return callReplyGetMapElementInternal(rep, idx, key, val, REDISMODULE_REPLY_MAP);
}
const char *callReplyGetBigNumber(CallReply *rep, size_t *len) {
callReplyParse(rep);
if (rep->type != REDISMODULE_REPLY_BIG_NUMBER) return NULL;
*len = rep->len;
return rep->val.str;
}
const char *callReplyGetVerbatim(CallReply *rep, size_t *len, const char **format){
callReplyParse(rep);
if (rep->type != REDISMODULE_REPLY_VERBATIM_STRING) return NULL;
*len = rep->len;
if (format) *format = rep->val.verbatim_str.format;
return rep->val.verbatim_str.str;
}
const char *callReplyGetProto(CallReply *rep, size_t *proto_len) {
*proto_len = rep->proto_len;
return rep->proto;
}
void *callReplyGetPrivateData(CallReply *rep) {
return rep->private_data;
}
int callReplyIsResp3(CallReply *rep) {
return rep->flags & REPLY_FLAG_RESP3;
}
CallReply *callReplyCreate(sds reply, void *private_data) {
CallReply *res = zmalloc(sizeof(*res));
res->flags = REPLY_FLAG_ROOT;
res->original_proto = reply;
res->proto = reply;
res->proto_len = sdslen(reply);
res->private_data = private_data;
res->attribute = NULL;
return res;
}
