
























































#include "resp_parser.h"
#include "server.h"

static int parseBulk(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
long long bulklen;
parser->curr_location = p + 2;

string2ll(proto+1,p-proto-1,&bulklen);
if (bulklen == -1) {
parser->callbacks.null_bulk_string_callback(p_ctx, proto, parser->curr_location - proto);
} else {
const char *str = parser->curr_location;
parser->curr_location += bulklen;
parser->curr_location += 2;
parser->callbacks.bulk_string_callback(p_ctx, str, bulklen, proto, parser->curr_location - proto);
}

return C_OK;
}

static int parseSimpleString(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
parser->curr_location = p + 2;
parser->callbacks.simple_str_callback(p_ctx, proto+1, p-proto-1, proto, parser->curr_location - proto);
return C_OK;
}

static int parseError(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
parser->curr_location = p + 2;
parser->callbacks.error_callback(p_ctx, proto+1, p-proto-1, proto, parser->curr_location - proto);
return C_OK;
}

static int parseLong(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
parser->curr_location = p + 2;
long long val;
string2ll(proto+1,p-proto-1,&val);
parser->callbacks.long_callback(p_ctx, val, proto, parser->curr_location - proto);
return C_OK;
}

static int parseAttributes(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
long long len;
string2ll(proto+1,p-proto-1,&len);
p += 2;
parser->curr_location = p;
parser->callbacks.attribute_callback(parser, p_ctx, len, proto);
return C_OK;
}

static int parseVerbatimString(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
long long bulklen;
parser->curr_location = p + 2;
string2ll(proto+1,p-proto-1,&bulklen);
const char *format = parser->curr_location;
parser->curr_location += bulklen;
parser->curr_location += 2;
parser->callbacks.verbatim_string_callback(p_ctx, format, format + 4, bulklen - 4, proto, parser->curr_location - proto);
return C_OK;
}

static int parseBigNumber(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
parser->curr_location = p + 2;
parser->callbacks.big_number_callback(p_ctx, proto+1, p-proto-1, proto, parser->curr_location - proto);
return C_OK;
}

static int parseNull(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
parser->curr_location = p + 2;
parser->callbacks.null_callback(p_ctx, proto, parser->curr_location - proto);
return C_OK;
}

static int parseDouble(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
parser->curr_location = p + 2;
char buf[MAX_LONG_DOUBLE_CHARS+1];
size_t len = p-proto-1;
double d;
if (len <= MAX_LONG_DOUBLE_CHARS) {
memcpy(buf,proto+1,len);
buf[len] = '\0';
d = strtod(buf,NULL);
} else {
d = 0;
}
parser->callbacks.double_callback(p_ctx, d, proto, parser->curr_location - proto);
return C_OK;
}

static int parseBool(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
parser->curr_location = p + 2;
parser->callbacks.bool_callback(p_ctx, proto[1] == 't', proto, parser->curr_location - proto);
return C_OK;
}

static int parseArray(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
long long len;
string2ll(proto+1,p-proto-1,&len);
p += 2;
parser->curr_location = p;
if (len == -1) {
parser->callbacks.null_array_callback(p_ctx, proto, parser->curr_location - proto);
} else {
parser->callbacks.array_callback(parser, p_ctx, len, proto);
}
return C_OK;
}

static int parseSet(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
long long len;
string2ll(proto+1,p-proto-1,&len);
p += 2;
parser->curr_location = p;
parser->callbacks.set_callback(parser, p_ctx, len, proto);
return C_OK;
}

static int parseMap(ReplyParser *parser, void *p_ctx) {
const char *proto = parser->curr_location;
char *p = strchr(proto+1,'\r');
long long len;
string2ll(proto+1,p-proto-1,&len);
p += 2;
parser->curr_location = p;
parser->callbacks.map_callback(parser, p_ctx, len, proto);
return C_OK;
}


int parseReply(ReplyParser *parser, void *p_ctx) {
switch (parser->curr_location[0]) {
case '$': return parseBulk(parser, p_ctx);
case '+': return parseSimpleString(parser, p_ctx);
case '-': return parseError(parser, p_ctx);
case ':': return parseLong(parser, p_ctx);
case '*': return parseArray(parser, p_ctx);
case '~': return parseSet(parser, p_ctx);
case '%': return parseMap(parser, p_ctx);
case '#': return parseBool(parser, p_ctx);
case ',': return parseDouble(parser, p_ctx);
case '_': return parseNull(parser, p_ctx);
case '(': return parseBigNumber(parser, p_ctx);
case '=': return parseVerbatimString(parser, p_ctx);
case '|': return parseAttributes(parser, p_ctx);
default: if (parser->callbacks.error) parser->callbacks.error(p_ctx);
}
return C_ERR;
}
