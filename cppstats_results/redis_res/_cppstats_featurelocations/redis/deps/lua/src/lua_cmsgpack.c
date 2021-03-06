#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "lua.h"
#include "lauxlib.h"

#define LUACMSGPACK_NAME "cmsgpack"
#define LUACMSGPACK_SAFE_NAME "cmsgpack_safe"
#define LUACMSGPACK_VERSION "lua-cmsgpack 0.4.0"
#define LUACMSGPACK_COPYRIGHT "Copyright (C) 2012, Salvatore Sanfilippo"
#define LUACMSGPACK_DESCRIPTION "MessagePack C implementation for Lua"


#if !defined(LUACMSGPACK_MAX_NESTING)
#define LUACMSGPACK_MAX_NESTING 16
#endif


#define IS_INT_TYPE_EQUIVALENT(x, T) (!isinf(x) && (T)(x) == (x))

#define IS_INT64_EQUIVALENT(x) IS_INT_TYPE_EQUIVALENT(x, int64_t)
#define IS_INT_EQUIVALENT(x) IS_INT_TYPE_EQUIVALENT(x, int)


#if UINTPTR_MAX == UINT_MAX
#define BITS_32 1
#else
#define BITS_32 0
#endif

#if BITS_32
#define lua_pushunsigned(L, n) lua_pushnumber(L, n)
#else
#define lua_pushunsigned(L, n) lua_pushinteger(L, n)
#endif






























void memrevifle(void *ptr, size_t len) {
unsigned char *p = (unsigned char *)ptr,
*e = (unsigned char *)p+len-1,
aux;
int test = 1;
unsigned char *testp = (unsigned char*) &test;

if (testp[0] == 0) return;
len /= 2;
while(len--) {
aux = *p;
*p = *e;
*e = aux;
p++;
e--;
}
}







typedef struct mp_buf {
unsigned char *b;
size_t len, free;
} mp_buf;

void *mp_realloc(lua_State *L, void *target, size_t osize,size_t nsize) {
void *(*local_realloc) (void *, void *, size_t osize, size_t nsize) = NULL;
void *ud;

local_realloc = lua_getallocf(L, &ud);

return local_realloc(ud, target, osize, nsize);
}

mp_buf *mp_buf_new(lua_State *L) {
mp_buf *buf = NULL;


buf = (mp_buf*)mp_realloc(L, NULL, 0, sizeof(*buf));

buf->b = NULL;
buf->len = buf->free = 0;
return buf;
}

void mp_buf_append(lua_State *L, mp_buf *buf, const unsigned char *s, size_t len) {
if (buf->free < len) {
size_t newsize = (buf->len+len)*2;

buf->b = (unsigned char*)mp_realloc(L, buf->b, buf->len + buf->free, newsize);
buf->free = newsize - buf->len;
}
memcpy(buf->b+buf->len,s,len);
buf->len += len;
buf->free -= len;
}

void mp_buf_free(lua_State *L, mp_buf *buf) {
mp_realloc(L, buf->b, buf->len + buf->free, 0);
mp_realloc(L, buf, sizeof(*buf), 0);
}










#define MP_CUR_ERROR_NONE 0
#define MP_CUR_ERROR_EOF 1
#define MP_CUR_ERROR_BADFMT 2

typedef struct mp_cur {
const unsigned char *p;
size_t left;
int err;
} mp_cur;

void mp_cur_init(mp_cur *cursor, const unsigned char *s, size_t len) {
cursor->p = s;
cursor->left = len;
cursor->err = MP_CUR_ERROR_NONE;
}

#define mp_cur_consume(_c,_len) do { _c->p += _len; _c->left -= _len; } while(0)




#define mp_cur_need(_c,_len) do { if (_c->left < _len) { _c->err = MP_CUR_ERROR_EOF; return; } } while(0)








void mp_encode_bytes(lua_State *L, mp_buf *buf, const unsigned char *s, size_t len) {
unsigned char hdr[5];
int hdrlen;

if (len < 32) {
hdr[0] = 0xa0 | (len&0xff);
hdrlen = 1;
} else if (len <= 0xff) {
hdr[0] = 0xd9;
hdr[1] = len;
hdrlen = 2;
} else if (len <= 0xffff) {
hdr[0] = 0xda;
hdr[1] = (len&0xff00)>>8;
hdr[2] = len&0xff;
hdrlen = 3;
} else {
hdr[0] = 0xdb;
hdr[1] = (len&0xff000000)>>24;
hdr[2] = (len&0xff0000)>>16;
hdr[3] = (len&0xff00)>>8;
hdr[4] = len&0xff;
hdrlen = 5;
}
mp_buf_append(L,buf,hdr,hdrlen);
mp_buf_append(L,buf,s,len);
}


void mp_encode_double(lua_State *L, mp_buf *buf, double d) {
unsigned char b[9];
float f = d;

assert(sizeof(f) == 4 && sizeof(d) == 8);
if (d == (double)f) {
b[0] = 0xca;
memcpy(b+1,&f,4);
memrevifle(b+1,4);
mp_buf_append(L,buf,b,5);
} else if (sizeof(d) == 8) {
b[0] = 0xcb;
memcpy(b+1,&d,8);
memrevifle(b+1,8);
mp_buf_append(L,buf,b,9);
}
}

void mp_encode_int(lua_State *L, mp_buf *buf, int64_t n) {
unsigned char b[9];
int enclen;

if (n >= 0) {
if (n <= 127) {
b[0] = n & 0x7f;
enclen = 1;
} else if (n <= 0xff) {
b[0] = 0xcc;
b[1] = n & 0xff;
enclen = 2;
} else if (n <= 0xffff) {
b[0] = 0xcd;
b[1] = (n & 0xff00) >> 8;
b[2] = n & 0xff;
enclen = 3;
} else if (n <= 0xffffffffLL) {
b[0] = 0xce;
b[1] = (n & 0xff000000) >> 24;
b[2] = (n & 0xff0000) >> 16;
b[3] = (n & 0xff00) >> 8;
b[4] = n & 0xff;
enclen = 5;
} else {
b[0] = 0xcf;
b[1] = (n & 0xff00000000000000LL) >> 56;
b[2] = (n & 0xff000000000000LL) >> 48;
b[3] = (n & 0xff0000000000LL) >> 40;
b[4] = (n & 0xff00000000LL) >> 32;
b[5] = (n & 0xff000000) >> 24;
b[6] = (n & 0xff0000) >> 16;
b[7] = (n & 0xff00) >> 8;
b[8] = n & 0xff;
enclen = 9;
}
} else {
if (n >= -32) {
b[0] = ((signed char)n);
enclen = 1;
} else if (n >= -128) {
b[0] = 0xd0;
b[1] = n & 0xff;
enclen = 2;
} else if (n >= -32768) {
b[0] = 0xd1;
b[1] = (n & 0xff00) >> 8;
b[2] = n & 0xff;
enclen = 3;
} else if (n >= -2147483648LL) {
b[0] = 0xd2;
b[1] = (n & 0xff000000) >> 24;
b[2] = (n & 0xff0000) >> 16;
b[3] = (n & 0xff00) >> 8;
b[4] = n & 0xff;
enclen = 5;
} else {
b[0] = 0xd3;
b[1] = (n & 0xff00000000000000LL) >> 56;
b[2] = (n & 0xff000000000000LL) >> 48;
b[3] = (n & 0xff0000000000LL) >> 40;
b[4] = (n & 0xff00000000LL) >> 32;
b[5] = (n & 0xff000000) >> 24;
b[6] = (n & 0xff0000) >> 16;
b[7] = (n & 0xff00) >> 8;
b[8] = n & 0xff;
enclen = 9;
}
}
mp_buf_append(L,buf,b,enclen);
}

void mp_encode_array(lua_State *L, mp_buf *buf, int64_t n) {
unsigned char b[5];
int enclen;

if (n <= 15) {
b[0] = 0x90 | (n & 0xf);
enclen = 1;
} else if (n <= 65535) {
b[0] = 0xdc;
b[1] = (n & 0xff00) >> 8;
b[2] = n & 0xff;
enclen = 3;
} else {
b[0] = 0xdd;
b[1] = (n & 0xff000000) >> 24;
b[2] = (n & 0xff0000) >> 16;
b[3] = (n & 0xff00) >> 8;
b[4] = n & 0xff;
enclen = 5;
}
mp_buf_append(L,buf,b,enclen);
}

void mp_encode_map(lua_State *L, mp_buf *buf, int64_t n) {
unsigned char b[5];
int enclen;

if (n <= 15) {
b[0] = 0x80 | (n & 0xf);
enclen = 1;
} else if (n <= 65535) {
b[0] = 0xde;
b[1] = (n & 0xff00) >> 8;
b[2] = n & 0xff;
enclen = 3;
} else {
b[0] = 0xdf;
b[1] = (n & 0xff000000) >> 24;
b[2] = (n & 0xff0000) >> 16;
b[3] = (n & 0xff00) >> 8;
b[4] = n & 0xff;
enclen = 5;
}
mp_buf_append(L,buf,b,enclen);
}



void mp_encode_lua_string(lua_State *L, mp_buf *buf) {
size_t len;
const char *s;

s = lua_tolstring(L,-1,&len);
mp_encode_bytes(L,buf,(const unsigned char*)s,len);
}

void mp_encode_lua_bool(lua_State *L, mp_buf *buf) {
unsigned char b = lua_toboolean(L,-1) ? 0xc3 : 0xc2;
mp_buf_append(L,buf,&b,1);
}


void mp_encode_lua_integer(lua_State *L, mp_buf *buf) {
#if (LUA_VERSION_NUM < 503) && BITS_32
lua_Number i = lua_tonumber(L,-1);
#else
lua_Integer i = lua_tointeger(L,-1);
#endif
mp_encode_int(L, buf, (int64_t)i);
}




void mp_encode_lua_number(lua_State *L, mp_buf *buf) {
lua_Number n = lua_tonumber(L,-1);

if (IS_INT64_EQUIVALENT(n)) {
mp_encode_lua_integer(L, buf);
} else {
mp_encode_double(L,buf,(double)n);
}
}

void mp_encode_lua_type(lua_State *L, mp_buf *buf, int level);


void mp_encode_lua_table_as_array(lua_State *L, mp_buf *buf, int level) {
#if LUA_VERSION_NUM < 502
size_t len = lua_objlen(L,-1), j;
#else
size_t len = lua_rawlen(L,-1), j;
#endif

mp_encode_array(L,buf,len);
luaL_checkstack(L, 1, "in function mp_encode_lua_table_as_array");
for (j = 1; j <= len; j++) {
lua_pushnumber(L,j);
lua_gettable(L,-2);
mp_encode_lua_type(L,buf,level+1);
}
}


void mp_encode_lua_table_as_map(lua_State *L, mp_buf *buf, int level) {
size_t len = 0;





luaL_checkstack(L, 3, "in function mp_encode_lua_table_as_map");
lua_pushnil(L);
while(lua_next(L,-2)) {
lua_pop(L,1);
len++;
}


mp_encode_map(L,buf,len);
lua_pushnil(L);
while(lua_next(L,-2)) {

lua_pushvalue(L,-2);
mp_encode_lua_type(L,buf,level+1);
mp_encode_lua_type(L,buf,level+1);
}
}




int table_is_an_array(lua_State *L) {
int count = 0, max = 0;
#if LUA_VERSION_NUM < 503
lua_Number n;
#else
lua_Integer n;
#endif


int stacktop;

stacktop = lua_gettop(L);

luaL_checkstack(L, 2, "in function table_is_an_array");
lua_pushnil(L);
while(lua_next(L,-2)) {

lua_pop(L,1);

#if LUA_VERSION_NUM < 503
if ((LUA_TNUMBER != lua_type(L,-1)) || (n = lua_tonumber(L, -1)) <= 0 ||
!IS_INT_EQUIVALENT(n))
#else
if (!lua_isinteger(L,-1) || (n = lua_tointeger(L, -1)) <= 0)
#endif
{
lua_settop(L, stacktop);
return 0;
}
max = (n > max ? n : max);
count++;
}





lua_settop(L, stacktop);
return max == count;
}




void mp_encode_lua_table(lua_State *L, mp_buf *buf, int level) {
if (table_is_an_array(L))
mp_encode_lua_table_as_array(L,buf,level);
else
mp_encode_lua_table_as_map(L,buf,level);
}

void mp_encode_lua_null(lua_State *L, mp_buf *buf) {
unsigned char b[1];

b[0] = 0xc0;
mp_buf_append(L,buf,b,1);
}

void mp_encode_lua_type(lua_State *L, mp_buf *buf, int level) {
int t = lua_type(L,-1);



if (t == LUA_TTABLE && level == LUACMSGPACK_MAX_NESTING) t = LUA_TNIL;
switch(t) {
case LUA_TSTRING: mp_encode_lua_string(L,buf); break;
case LUA_TBOOLEAN: mp_encode_lua_bool(L,buf); break;
case LUA_TNUMBER:
#if LUA_VERSION_NUM < 503
mp_encode_lua_number(L,buf); break;
#else
if (lua_isinteger(L, -1)) {
mp_encode_lua_integer(L, buf);
} else {
mp_encode_lua_number(L, buf);
}
break;
#endif
case LUA_TTABLE: mp_encode_lua_table(L,buf,level); break;
default: mp_encode_lua_null(L,buf); break;
}
lua_pop(L,1);
}





int mp_pack(lua_State *L) {
int nargs = lua_gettop(L);
int i;
mp_buf *buf;

if (nargs == 0)
return luaL_argerror(L, 0, "MessagePack pack needs input.");

if (!lua_checkstack(L, nargs))
return luaL_argerror(L, 0, "Too many arguments for MessagePack pack.");

buf = mp_buf_new(L);
for(i = 1; i <= nargs; i++) {


luaL_checkstack(L, 1, "in function mp_check");
lua_pushvalue(L, i);

mp_encode_lua_type(L,buf,0);

lua_pushlstring(L,(char*)buf->b,buf->len);




buf->free += buf->len;
buf->len = 0;
}
mp_buf_free(L, buf);


lua_concat(L, nargs);
return 1;
}



void mp_decode_to_lua_type(lua_State *L, mp_cur *c);

void mp_decode_to_lua_array(lua_State *L, mp_cur *c, size_t len) {
assert(len <= UINT_MAX);
int index = 1;

lua_newtable(L);
luaL_checkstack(L, 1, "in function mp_decode_to_lua_array");
while(len--) {
lua_pushnumber(L,index++);
mp_decode_to_lua_type(L,c);
if (c->err) return;
lua_settable(L,-3);
}
}

void mp_decode_to_lua_hash(lua_State *L, mp_cur *c, size_t len) {
assert(len <= UINT_MAX);
lua_newtable(L);
while(len--) {
mp_decode_to_lua_type(L,c);
if (c->err) return;
mp_decode_to_lua_type(L,c);
if (c->err) return;
lua_settable(L,-3);
}
}



void mp_decode_to_lua_type(lua_State *L, mp_cur *c) {
mp_cur_need(c,1);






luaL_checkstack(L, 1,
"too many return values at once; "
"use unpack_one or unpack_limit instead.");

switch(c->p[0]) {
case 0xcc:
mp_cur_need(c,2);
lua_pushunsigned(L,c->p[1]);
mp_cur_consume(c,2);
break;
case 0xd0:
mp_cur_need(c,2);
lua_pushinteger(L,(signed char)c->p[1]);
mp_cur_consume(c,2);
break;
case 0xcd:
mp_cur_need(c,3);
lua_pushunsigned(L,
(c->p[1] << 8) |
c->p[2]);
mp_cur_consume(c,3);
break;
case 0xd1:
mp_cur_need(c,3);
lua_pushinteger(L,(int16_t)
(c->p[1] << 8) |
c->p[2]);
mp_cur_consume(c,3);
break;
case 0xce:
mp_cur_need(c,5);
lua_pushunsigned(L,
((uint32_t)c->p[1] << 24) |
((uint32_t)c->p[2] << 16) |
((uint32_t)c->p[3] << 8) |
(uint32_t)c->p[4]);
mp_cur_consume(c,5);
break;
case 0xd2:
mp_cur_need(c,5);
lua_pushinteger(L,
((int32_t)c->p[1] << 24) |
((int32_t)c->p[2] << 16) |
((int32_t)c->p[3] << 8) |
(int32_t)c->p[4]);
mp_cur_consume(c,5);
break;
case 0xcf:
mp_cur_need(c,9);
lua_pushunsigned(L,
((uint64_t)c->p[1] << 56) |
((uint64_t)c->p[2] << 48) |
((uint64_t)c->p[3] << 40) |
((uint64_t)c->p[4] << 32) |
((uint64_t)c->p[5] << 24) |
((uint64_t)c->p[6] << 16) |
((uint64_t)c->p[7] << 8) |
(uint64_t)c->p[8]);
mp_cur_consume(c,9);
break;
case 0xd3:
mp_cur_need(c,9);
#if LUA_VERSION_NUM < 503
lua_pushnumber(L,
#else
lua_pushinteger(L,
#endif
((int64_t)c->p[1] << 56) |
((int64_t)c->p[2] << 48) |
((int64_t)c->p[3] << 40) |
((int64_t)c->p[4] << 32) |
((int64_t)c->p[5] << 24) |
((int64_t)c->p[6] << 16) |
((int64_t)c->p[7] << 8) |
(int64_t)c->p[8]);
mp_cur_consume(c,9);
break;
case 0xc0:
lua_pushnil(L);
mp_cur_consume(c,1);
break;
case 0xc3:
lua_pushboolean(L,1);
mp_cur_consume(c,1);
break;
case 0xc2:
lua_pushboolean(L,0);
mp_cur_consume(c,1);
break;
case 0xca:
mp_cur_need(c,5);
assert(sizeof(float) == 4);
{
float f;
memcpy(&f,c->p+1,4);
memrevifle(&f,4);
lua_pushnumber(L,f);
mp_cur_consume(c,5);
}
break;
case 0xcb:
mp_cur_need(c,9);
assert(sizeof(double) == 8);
{
double d;
memcpy(&d,c->p+1,8);
memrevifle(&d,8);
lua_pushnumber(L,d);
mp_cur_consume(c,9);
}
break;
case 0xd9:
mp_cur_need(c,2);
{
size_t l = c->p[1];
mp_cur_need(c,2+l);
lua_pushlstring(L,(char*)c->p+2,l);
mp_cur_consume(c,2+l);
}
break;
case 0xda:
mp_cur_need(c,3);
{
size_t l = (c->p[1] << 8) | c->p[2];
mp_cur_need(c,3+l);
lua_pushlstring(L,(char*)c->p+3,l);
mp_cur_consume(c,3+l);
}
break;
case 0xdb:
mp_cur_need(c,5);
{
size_t l = ((size_t)c->p[1] << 24) |
((size_t)c->p[2] << 16) |
((size_t)c->p[3] << 8) |
(size_t)c->p[4];
mp_cur_consume(c,5);
mp_cur_need(c,l);
lua_pushlstring(L,(char*)c->p,l);
mp_cur_consume(c,l);
}
break;
case 0xdc:
mp_cur_need(c,3);
{
size_t l = (c->p[1] << 8) | c->p[2];
mp_cur_consume(c,3);
mp_decode_to_lua_array(L,c,l);
}
break;
case 0xdd:
mp_cur_need(c,5);
{
size_t l = ((size_t)c->p[1] << 24) |
((size_t)c->p[2] << 16) |
((size_t)c->p[3] << 8) |
(size_t)c->p[4];
mp_cur_consume(c,5);
mp_decode_to_lua_array(L,c,l);
}
break;
case 0xde:
mp_cur_need(c,3);
{
size_t l = (c->p[1] << 8) | c->p[2];
mp_cur_consume(c,3);
mp_decode_to_lua_hash(L,c,l);
}
break;
case 0xdf:
mp_cur_need(c,5);
{
size_t l = ((size_t)c->p[1] << 24) |
((size_t)c->p[2] << 16) |
((size_t)c->p[3] << 8) |
(size_t)c->p[4];
mp_cur_consume(c,5);
mp_decode_to_lua_hash(L,c,l);
}
break;
default:
if ((c->p[0] & 0x80) == 0) {
lua_pushunsigned(L,c->p[0]);
mp_cur_consume(c,1);
} else if ((c->p[0] & 0xe0) == 0xe0) {
lua_pushinteger(L,(signed char)c->p[0]);
mp_cur_consume(c,1);
} else if ((c->p[0] & 0xe0) == 0xa0) {
size_t l = c->p[0] & 0x1f;
mp_cur_need(c,1+l);
lua_pushlstring(L,(char*)c->p+1,l);
mp_cur_consume(c,1+l);
} else if ((c->p[0] & 0xf0) == 0x90) {
size_t l = c->p[0] & 0xf;
mp_cur_consume(c,1);
mp_decode_to_lua_array(L,c,l);
} else if ((c->p[0] & 0xf0) == 0x80) {
size_t l = c->p[0] & 0xf;
mp_cur_consume(c,1);
mp_decode_to_lua_hash(L,c,l);
} else {
c->err = MP_CUR_ERROR_BADFMT;
}
}
}

int mp_unpack_full(lua_State *L, int limit, int offset) {
size_t len;
const char *s;
mp_cur c;
int cnt;
int decode_all = (!limit && !offset);

s = luaL_checklstring(L,1,&len);

if (offset < 0 || limit < 0)
return luaL_error(L,
"Invalid request to unpack with offset of %d and limit of %d.",
offset, len);
else if (offset > len)
return luaL_error(L,
"Start offset %d greater than input length %d.", offset, len);

if (decode_all) limit = INT_MAX;

mp_cur_init(&c,(const unsigned char *)s+offset,len-offset);



for(cnt = 0; c.left > 0 && cnt < limit; cnt++) {
mp_decode_to_lua_type(L,&c);

if (c.err == MP_CUR_ERROR_EOF) {
return luaL_error(L,"Missing bytes in input.");
} else if (c.err == MP_CUR_ERROR_BADFMT) {
return luaL_error(L,"Bad data format in input.");
}
}

if (!decode_all) {



int offset = len - c.left;

luaL_checkstack(L, 1, "in function mp_unpack_full");


lua_pushinteger(L, c.left == 0 ? -1 : offset);






lua_insert(L, 2);
cnt += 1;
}

return cnt;
}

int mp_unpack(lua_State *L) {
return mp_unpack_full(L, 0, 0);
}

int mp_unpack_one(lua_State *L) {
int offset = luaL_optinteger(L, 2, 0);

lua_pop(L, lua_gettop(L)-1);
return mp_unpack_full(L, 1, offset);
}

int mp_unpack_limit(lua_State *L) {
int limit = luaL_checkinteger(L, 2);
int offset = luaL_optinteger(L, 3, 0);

lua_pop(L, lua_gettop(L)-1);

return mp_unpack_full(L, limit, offset);
}

int mp_safe(lua_State *L) {
int argc, err, total_results;

argc = lua_gettop(L);



lua_pushvalue(L, lua_upvalueindex(1));
lua_insert(L, 1);

err = lua_pcall(L, argc, LUA_MULTRET, 0);
total_results = lua_gettop(L);

if (!err) {
return total_results;
} else {
lua_pushnil(L);
lua_insert(L,-2);
return 2;
}
}


const struct luaL_Reg cmds[] = {
{"pack", mp_pack},
{"unpack", mp_unpack},
{"unpack_one", mp_unpack_one},
{"unpack_limit", mp_unpack_limit},
{0}
};

int luaopen_create(lua_State *L) {
int i;


lua_newtable(L);

for (i = 0; i < (sizeof(cmds)/sizeof(*cmds) - 1); i++) {
lua_pushcfunction(L, cmds[i].func);
lua_setfield(L, -2, cmds[i].name);
}


lua_pushliteral(L, LUACMSGPACK_NAME);
lua_setfield(L, -2, "_NAME");
lua_pushliteral(L, LUACMSGPACK_VERSION);
lua_setfield(L, -2, "_VERSION");
lua_pushliteral(L, LUACMSGPACK_COPYRIGHT);
lua_setfield(L, -2, "_COPYRIGHT");
lua_pushliteral(L, LUACMSGPACK_DESCRIPTION);
lua_setfield(L, -2, "_DESCRIPTION");
return 1;
}

LUALIB_API int luaopen_cmsgpack(lua_State *L) {
luaopen_create(L);

#if LUA_VERSION_NUM < 502

lua_pushvalue(L, -1);
lua_setglobal(L, LUACMSGPACK_NAME);
#endif

return 1;
}

LUALIB_API int luaopen_cmsgpack_safe(lua_State *L) {
int i;

luaopen_cmsgpack(L);


for (i = 0; i < (sizeof(cmds)/sizeof(*cmds) - 1); i++) {
lua_getfield(L, -1, cmds[i].name);
lua_pushcclosure(L, mp_safe, 1);
lua_setfield(L, -2, cmds[i].name);
}

#if LUA_VERSION_NUM < 502

lua_pushvalue(L, -1);
lua_setglobal(L, LUACMSGPACK_SAFE_NAME);
#endif

return 1;
}























