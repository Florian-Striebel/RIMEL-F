#include "fmacros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include "sds.h"
#include "sdsalloc.h"
static inline int hi_sdsHdrSize(char type) {
switch(type&HI_SDS_TYPE_MASK) {
case HI_SDS_TYPE_5:
return sizeof(struct hisdshdr5);
case HI_SDS_TYPE_8:
return sizeof(struct hisdshdr8);
case HI_SDS_TYPE_16:
return sizeof(struct hisdshdr16);
case HI_SDS_TYPE_32:
return sizeof(struct hisdshdr32);
case HI_SDS_TYPE_64:
return sizeof(struct hisdshdr64);
}
return 0;
}
static inline char hi_sdsReqType(size_t string_size) {
if (string_size < 32)
return HI_SDS_TYPE_5;
if (string_size < 0xff)
return HI_SDS_TYPE_8;
if (string_size < 0xffff)
return HI_SDS_TYPE_16;
if (string_size < 0xffffffff)
return HI_SDS_TYPE_32;
return HI_SDS_TYPE_64;
}
hisds hi_sdsnewlen(const void *init, size_t initlen) {
void *sh;
hisds s;
char type = hi_sdsReqType(initlen);
if (type == HI_SDS_TYPE_5 && initlen == 0) type = HI_SDS_TYPE_8;
int hdrlen = hi_sdsHdrSize(type);
unsigned char *fp;
sh = hi_s_malloc(hdrlen+initlen+1);
if (sh == NULL) return NULL;
if (!init)
memset(sh, 0, hdrlen+initlen+1);
s = (char*)sh+hdrlen;
fp = ((unsigned char*)s)-1;
switch(type) {
case HI_SDS_TYPE_5: {
*fp = type | (initlen << HI_SDS_TYPE_BITS);
break;
}
case HI_SDS_TYPE_8: {
HI_SDS_HDR_VAR(8,s);
sh->len = initlen;
sh->alloc = initlen;
*fp = type;
break;
}
case HI_SDS_TYPE_16: {
HI_SDS_HDR_VAR(16,s);
sh->len = initlen;
sh->alloc = initlen;
*fp = type;
break;
}
case HI_SDS_TYPE_32: {
HI_SDS_HDR_VAR(32,s);
sh->len = initlen;
sh->alloc = initlen;
*fp = type;
break;
}
case HI_SDS_TYPE_64: {
HI_SDS_HDR_VAR(64,s);
sh->len = initlen;
sh->alloc = initlen;
*fp = type;
break;
}
}
if (initlen && init)
memcpy(s, init, initlen);
s[initlen] = '\0';
return s;
}
hisds hi_sdsempty(void) {
return hi_sdsnewlen("",0);
}
hisds hi_sdsnew(const char *init) {
size_t initlen = (init == NULL) ? 0 : strlen(init);
return hi_sdsnewlen(init, initlen);
}
hisds hi_sdsdup(const hisds s) {
return hi_sdsnewlen(s, hi_sdslen(s));
}
void hi_sdsfree(hisds s) {
if (s == NULL) return;
hi_s_free((char*)s-hi_sdsHdrSize(s[-1]));
}
void hi_sdsupdatelen(hisds s) {
int reallen = strlen(s);
hi_sdssetlen(s, reallen);
}
void hi_sdsclear(hisds s) {
hi_sdssetlen(s, 0);
s[0] = '\0';
}
hisds hi_sdsMakeRoomFor(hisds s, size_t addlen) {
void *sh, *newsh;
size_t avail = hi_sdsavail(s);
size_t len, newlen;
char type, oldtype = s[-1] & HI_SDS_TYPE_MASK;
int hdrlen;
if (avail >= addlen) return s;
len = hi_sdslen(s);
sh = (char*)s-hi_sdsHdrSize(oldtype);
newlen = (len+addlen);
if (newlen < HI_SDS_MAX_PREALLOC)
newlen *= 2;
else
newlen += HI_SDS_MAX_PREALLOC;
type = hi_sdsReqType(newlen);
if (type == HI_SDS_TYPE_5) type = HI_SDS_TYPE_8;
hdrlen = hi_sdsHdrSize(type);
if (oldtype==type) {
newsh = hi_s_realloc(sh, hdrlen+newlen+1);
if (newsh == NULL) return NULL;
s = (char*)newsh+hdrlen;
} else {
newsh = hi_s_malloc(hdrlen+newlen+1);
if (newsh == NULL) return NULL;
memcpy((char*)newsh+hdrlen, s, len+1);
hi_s_free(sh);
s = (char*)newsh+hdrlen;
s[-1] = type;
hi_sdssetlen(s, len);
}
hi_sdssetalloc(s, newlen);
return s;
}
hisds hi_sdsRemoveFreeSpace(hisds s) {
void *sh, *newsh;
char type, oldtype = s[-1] & HI_SDS_TYPE_MASK;
int hdrlen;
size_t len = hi_sdslen(s);
sh = (char*)s-hi_sdsHdrSize(oldtype);
type = hi_sdsReqType(len);
hdrlen = hi_sdsHdrSize(type);
if (oldtype==type) {
newsh = hi_s_realloc(sh, hdrlen+len+1);
if (newsh == NULL) return NULL;
s = (char*)newsh+hdrlen;
} else {
newsh = hi_s_malloc(hdrlen+len+1);
if (newsh == NULL) return NULL;
memcpy((char*)newsh+hdrlen, s, len+1);
hi_s_free(sh);
s = (char*)newsh+hdrlen;
s[-1] = type;
hi_sdssetlen(s, len);
}
hi_sdssetalloc(s, len);
return s;
}
size_t hi_sdsAllocSize(hisds s) {
size_t alloc = hi_sdsalloc(s);
return hi_sdsHdrSize(s[-1])+alloc+1;
}
void *hi_sdsAllocPtr(hisds s) {
return (void*) (s-hi_sdsHdrSize(s[-1]));
}
void hi_sdsIncrLen(hisds s, int incr) {
unsigned char flags = s[-1];
size_t len;
switch(flags&HI_SDS_TYPE_MASK) {
case HI_SDS_TYPE_5: {
unsigned char *fp = ((unsigned char*)s)-1;
unsigned char oldlen = HI_SDS_TYPE_5_LEN(flags);
assert((incr > 0 && oldlen+incr < 32) || (incr < 0 && oldlen >= (unsigned int)(-incr)));
*fp = HI_SDS_TYPE_5 | ((oldlen+incr) << HI_SDS_TYPE_BITS);
len = oldlen+incr;
break;
}
case HI_SDS_TYPE_8: {
HI_SDS_HDR_VAR(8,s);
assert((incr >= 0 && sh->alloc-sh->len >= incr) || (incr < 0 && sh->len >= (unsigned int)(-incr)));
len = (sh->len += incr);
break;
}
case HI_SDS_TYPE_16: {
HI_SDS_HDR_VAR(16,s);
assert((incr >= 0 && sh->alloc-sh->len >= incr) || (incr < 0 && sh->len >= (unsigned int)(-incr)));
len = (sh->len += incr);
break;
}
case HI_SDS_TYPE_32: {
HI_SDS_HDR_VAR(32,s);
assert((incr >= 0 && sh->alloc-sh->len >= (unsigned int)incr) || (incr < 0 && sh->len >= (unsigned int)(-incr)));
len = (sh->len += incr);
break;
}
case HI_SDS_TYPE_64: {
HI_SDS_HDR_VAR(64,s);
assert((incr >= 0 && sh->alloc-sh->len >= (uint64_t)incr) || (incr < 0 && sh->len >= (uint64_t)(-incr)));
len = (sh->len += incr);
break;
}
default: len = 0;
}
s[len] = '\0';
}
hisds hi_sdsgrowzero(hisds s, size_t len) {
size_t curlen = hi_sdslen(s);
if (len <= curlen) return s;
s = hi_sdsMakeRoomFor(s,len-curlen);
if (s == NULL) return NULL;
memset(s+curlen,0,(len-curlen+1));
hi_sdssetlen(s, len);
return s;
}
hisds hi_sdscatlen(hisds s, const void *t, size_t len) {
size_t curlen = hi_sdslen(s);
s = hi_sdsMakeRoomFor(s,len);
if (s == NULL) return NULL;
memcpy(s+curlen, t, len);
hi_sdssetlen(s, curlen+len);
s[curlen+len] = '\0';
return s;
}
hisds hi_sdscat(hisds s, const char *t) {
return hi_sdscatlen(s, t, strlen(t));
}
hisds hi_sdscatsds(hisds s, const hisds t) {
return hi_sdscatlen(s, t, hi_sdslen(t));
}
hisds hi_sdscpylen(hisds s, const char *t, size_t len) {
if (hi_sdsalloc(s) < len) {
s = hi_sdsMakeRoomFor(s,len-hi_sdslen(s));
if (s == NULL) return NULL;
}
memcpy(s, t, len);
s[len] = '\0';
hi_sdssetlen(s, len);
return s;
}
hisds hi_sdscpy(hisds s, const char *t) {
return hi_sdscpylen(s, t, strlen(t));
}
#define HI_SDS_LLSTR_SIZE 21
int hi_sdsll2str(char *s, long long value) {
char *p, aux;
unsigned long long v;
size_t l;
v = (value < 0) ? -value : value;
p = s;
do {
*p++ = '0'+(v%10);
v /= 10;
} while(v);
if (value < 0) *p++ = '-';
l = p-s;
*p = '\0';
p--;
while(s < p) {
aux = *s;
*s = *p;
*p = aux;
s++;
p--;
}
return l;
}
int hi_sdsull2str(char *s, unsigned long long v) {
char *p, aux;
size_t l;
p = s;
do {
*p++ = '0'+(v%10);
v /= 10;
} while(v);
l = p-s;
*p = '\0';
p--;
while(s < p) {
aux = *s;
*s = *p;
*p = aux;
s++;
p--;
}
return l;
}
hisds hi_sdsfromlonglong(long long value) {
char buf[HI_SDS_LLSTR_SIZE];
int len = hi_sdsll2str(buf,value);
return hi_sdsnewlen(buf,len);
}
hisds hi_sdscatvprintf(hisds s, const char *fmt, va_list ap) {
va_list cpy;
char staticbuf[1024], *buf = staticbuf, *t;
size_t buflen = strlen(fmt)*2;
if (buflen > sizeof(staticbuf)) {
buf = hi_s_malloc(buflen);
if (buf == NULL) return NULL;
} else {
buflen = sizeof(staticbuf);
}
while(1) {
buf[buflen-2] = '\0';
va_copy(cpy,ap);
vsnprintf(buf, buflen, fmt, cpy);
va_end(cpy);
if (buf[buflen-2] != '\0') {
if (buf != staticbuf) hi_s_free(buf);
buflen *= 2;
buf = hi_s_malloc(buflen);
if (buf == NULL) return NULL;
continue;
}
break;
}
t = hi_sdscat(s, buf);
if (buf != staticbuf) hi_s_free(buf);
return t;
}
hisds hi_sdscatprintf(hisds s, const char *fmt, ...) {
va_list ap;
char *t;
va_start(ap, fmt);
t = hi_sdscatvprintf(s,fmt,ap);
va_end(ap);
return t;
}
hisds hi_sdscatfmt(hisds s, char const *fmt, ...) {
const char *f = fmt;
int i;
va_list ap;
va_start(ap,fmt);
i = hi_sdslen(s);
while(*f) {
char next, *str;
size_t l;
long long num;
unsigned long long unum;
if (hi_sdsavail(s)==0) {
s = hi_sdsMakeRoomFor(s,1);
if (s == NULL) goto fmt_error;
}
switch(*f) {
case '%':
next = *(f+1);
f++;
switch(next) {
case 's':
case 'S':
str = va_arg(ap,char*);
l = (next == 's') ? strlen(str) : hi_sdslen(str);
if (hi_sdsavail(s) < l) {
s = hi_sdsMakeRoomFor(s,l);
if (s == NULL) goto fmt_error;
}
memcpy(s+i,str,l);
hi_sdsinclen(s,l);
i += l;
break;
case 'i':
case 'I':
if (next == 'i')
num = va_arg(ap,int);
else
num = va_arg(ap,long long);
{
char buf[HI_SDS_LLSTR_SIZE];
l = hi_sdsll2str(buf,num);
if (hi_sdsavail(s) < l) {
s = hi_sdsMakeRoomFor(s,l);
if (s == NULL) goto fmt_error;
}
memcpy(s+i,buf,l);
hi_sdsinclen(s,l);
i += l;
}
break;
case 'u':
case 'U':
if (next == 'u')
unum = va_arg(ap,unsigned int);
else
unum = va_arg(ap,unsigned long long);
{
char buf[HI_SDS_LLSTR_SIZE];
l = hi_sdsull2str(buf,unum);
if (hi_sdsavail(s) < l) {
s = hi_sdsMakeRoomFor(s,l);
if (s == NULL) goto fmt_error;
}
memcpy(s+i,buf,l);
hi_sdsinclen(s,l);
i += l;
}
break;
default:
s[i++] = next;
hi_sdsinclen(s,1);
break;
}
break;
default:
s[i++] = *f;
hi_sdsinclen(s,1);
break;
}
f++;
}
va_end(ap);
s[i] = '\0';
return s;
fmt_error:
va_end(ap);
return NULL;
}
hisds hi_sdstrim(hisds s, const char *cset) {
char *start, *end, *sp, *ep;
size_t len;
sp = start = s;
ep = end = s+hi_sdslen(s)-1;
while(sp <= end && strchr(cset, *sp)) sp++;
while(ep > sp && strchr(cset, *ep)) ep--;
len = (sp > ep) ? 0 : ((ep-sp)+1);
if (s != sp) memmove(s, sp, len);
s[len] = '\0';
hi_sdssetlen(s,len);
return s;
}
int hi_sdsrange(hisds s, ssize_t start, ssize_t end) {
size_t newlen, len = hi_sdslen(s);
if (len > SSIZE_MAX) return -1;
if (len == 0) return 0;
if (start < 0) {
start = len+start;
if (start < 0) start = 0;
}
if (end < 0) {
end = len+end;
if (end < 0) end = 0;
}
newlen = (start > end) ? 0 : (end-start)+1;
if (newlen != 0) {
if (start >= (ssize_t)len) {
newlen = 0;
} else if (end >= (ssize_t)len) {
end = len-1;
newlen = (start > end) ? 0 : (end-start)+1;
}
} else {
start = 0;
}
if (start && newlen) memmove(s, s+start, newlen);
s[newlen] = 0;
hi_sdssetlen(s,newlen);
return 0;
}
void hi_sdstolower(hisds s) {
int len = hi_sdslen(s), j;
for (j = 0; j < len; j++) s[j] = tolower(s[j]);
}
void hi_sdstoupper(hisds s) {
int len = hi_sdslen(s), j;
for (j = 0; j < len; j++) s[j] = toupper(s[j]);
}
int hi_sdscmp(const hisds s1, const hisds s2) {
size_t l1, l2, minlen;
int cmp;
l1 = hi_sdslen(s1);
l2 = hi_sdslen(s2);
minlen = (l1 < l2) ? l1 : l2;
cmp = memcmp(s1,s2,minlen);
if (cmp == 0) return l1-l2;
return cmp;
}
hisds *hi_sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count) {
int elements = 0, slots = 5, start = 0, j;
hisds *tokens;
if (seplen < 1 || len < 0) return NULL;
tokens = hi_s_malloc(sizeof(hisds)*slots);
if (tokens == NULL) return NULL;
if (len == 0) {
*count = 0;
return tokens;
}
for (j = 0; j < (len-(seplen-1)); j++) {
if (slots < elements+2) {
hisds *newtokens;
slots *= 2;
newtokens = hi_s_realloc(tokens,sizeof(hisds)*slots);
if (newtokens == NULL) goto cleanup;
tokens = newtokens;
}
if ((seplen == 1 && *(s+j) == sep[0]) || (memcmp(s+j,sep,seplen) == 0)) {
tokens[elements] = hi_sdsnewlen(s+start,j-start);
if (tokens[elements] == NULL) goto cleanup;
elements++;
start = j+seplen;
j = j+seplen-1;
}
}
tokens[elements] = hi_sdsnewlen(s+start,len-start);
if (tokens[elements] == NULL) goto cleanup;
elements++;
*count = elements;
return tokens;
cleanup:
{
int i;
for (i = 0; i < elements; i++) hi_sdsfree(tokens[i]);
hi_s_free(tokens);
*count = 0;
return NULL;
}
}
void hi_sdsfreesplitres(hisds *tokens, int count) {
if (!tokens) return;
while(count--)
hi_sdsfree(tokens[count]);
hi_s_free(tokens);
}
hisds hi_sdscatrepr(hisds s, const char *p, size_t len) {
s = hi_sdscatlen(s,"\"",1);
while(len--) {
switch(*p) {
case '\\':
case '"':
s = hi_sdscatprintf(s,"\\%c",*p);
break;
case '\n': s = hi_sdscatlen(s,"\\n",2); break;
case '\r': s = hi_sdscatlen(s,"\\r",2); break;
case '\t': s = hi_sdscatlen(s,"\\t",2); break;
case '\a': s = hi_sdscatlen(s,"\\a",2); break;
case '\b': s = hi_sdscatlen(s,"\\b",2); break;
default:
if (isprint(*p))
s = hi_sdscatprintf(s,"%c",*p);
else
s = hi_sdscatprintf(s,"\\x%02x",(unsigned char)*p);
break;
}
p++;
}
return hi_sdscatlen(s,"\"",1);
}
static int hi_hex_digit_to_int(char c) {
switch(c) {
case '0': return 0;
case '1': return 1;
case '2': return 2;
case '3': return 3;
case '4': return 4;
case '5': return 5;
case '6': return 6;
case '7': return 7;
case '8': return 8;
case '9': return 9;
case 'a': case 'A': return 10;
case 'b': case 'B': return 11;
case 'c': case 'C': return 12;
case 'd': case 'D': return 13;
case 'e': case 'E': return 14;
case 'f': case 'F': return 15;
default: return 0;
}
}
hisds *hi_sdssplitargs(const char *line, int *argc) {
const char *p = line;
char *current = NULL;
char **vector = NULL;
*argc = 0;
while(1) {
while(*p && isspace(*p)) p++;
if (*p) {
int inq=0;
int insq=0;
int done=0;
if (current == NULL) current = hi_sdsempty();
while(!done) {
if (inq) {
if (*p == '\\' && *(p+1) == 'x' &&
isxdigit(*(p+2)) &&
isxdigit(*(p+3)))
{
unsigned char byte;
byte = (hi_hex_digit_to_int(*(p+2))*16)+
hi_hex_digit_to_int(*(p+3));
current = hi_sdscatlen(current,(char*)&byte,1);
p += 3;
} else if (*p == '\\' && *(p+1)) {
char c;
p++;
switch(*p) {
case 'n': c = '\n'; break;
case 'r': c = '\r'; break;
case 't': c = '\t'; break;
case 'b': c = '\b'; break;
case 'a': c = '\a'; break;
default: c = *p; break;
}
current = hi_sdscatlen(current,&c,1);
} else if (*p == '"') {
if (*(p+1) && !isspace(*(p+1))) goto err;
done=1;
} else if (!*p) {
goto err;
} else {
current = hi_sdscatlen(current,p,1);
}
} else if (insq) {
if (*p == '\\' && *(p+1) == '\'') {
p++;
current = hi_sdscatlen(current,"'",1);
} else if (*p == '\'') {
if (*(p+1) && !isspace(*(p+1))) goto err;
done=1;
} else if (!*p) {
goto err;
} else {
current = hi_sdscatlen(current,p,1);
}
} else {
switch(*p) {
case ' ':
case '\n':
case '\r':
case '\t':
case '\0':
done=1;
break;
case '"':
inq=1;
break;
case '\'':
insq=1;
break;
default:
current = hi_sdscatlen(current,p,1);
break;
}
}
if (*p) p++;
}
{
char **new_vector = hi_s_realloc(vector,((*argc)+1)*sizeof(char*));
if (new_vector == NULL) {
hi_s_free(vector);
return NULL;
}
vector = new_vector;
vector[*argc] = current;
(*argc)++;
current = NULL;
}
} else {
if (vector == NULL) vector = hi_s_malloc(sizeof(void*));
return vector;
}
}
err:
while((*argc)--)
hi_sdsfree(vector[*argc]);
hi_s_free(vector);
if (current) hi_sdsfree(current);
*argc = 0;
return NULL;
}
hisds hi_sdsmapchars(hisds s, const char *from, const char *to, size_t setlen) {
size_t j, i, l = hi_sdslen(s);
for (j = 0; j < l; j++) {
for (i = 0; i < setlen; i++) {
if (s[j] == from[i]) {
s[j] = to[i];
break;
}
}
}
return s;
}
hisds hi_sdsjoin(char **argv, int argc, char *sep) {
hisds join = hi_sdsempty();
int j;
for (j = 0; j < argc; j++) {
join = hi_sdscat(join, argv[j]);
if (j != argc-1) join = hi_sdscat(join,sep);
}
return join;
}
hisds hi_sdsjoinsds(hisds *argv, int argc, const char *sep, size_t seplen) {
hisds join = hi_sdsempty();
int j;
for (j = 0; j < argc; j++) {
join = hi_sdscatsds(join, argv[j]);
if (j != argc-1) join = hi_sdscatlen(join,sep,seplen);
}
return join;
}
void *hi_sds_malloc(size_t size) { return hi_s_malloc(size); }
void *hi_sds_realloc(void *ptr, size_t size) { return hi_s_realloc(ptr,size); }
void hi_sds_free(void *ptr) { hi_s_free(ptr); }
#if defined(HI_SDS_TEST_MAIN)
#include <stdio.h>
#include "testhelp.h"
#include "limits.h"
#define UNUSED(x) (void)(x)
int hi_sdsTest(void) {
{
hisds x = hi_sdsnew("foo"), y;
test_cond("Create a string and obtain the length",
hi_sdslen(x) == 3 && memcmp(x,"foo\0",4) == 0)
hi_sdsfree(x);
x = hi_sdsnewlen("foo",2);
test_cond("Create a string with specified length",
hi_sdslen(x) == 2 && memcmp(x,"fo\0",3) == 0)
x = hi_sdscat(x,"bar");
test_cond("Strings concatenation",
hi_sdslen(x) == 5 && memcmp(x,"fobar\0",6) == 0);
x = hi_sdscpy(x,"a");
test_cond("hi_sdscpy() against an originally longer string",
hi_sdslen(x) == 1 && memcmp(x,"a\0",2) == 0)
x = hi_sdscpy(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk");
test_cond("hi_sdscpy() against an originally shorter string",
hi_sdslen(x) == 33 &&
memcmp(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk\0",33) == 0)
hi_sdsfree(x);
x = hi_sdscatprintf(hi_sdsempty(),"%d",123);
test_cond("hi_sdscatprintf() seems working in the base case",
hi_sdslen(x) == 3 && memcmp(x,"123\0",4) == 0)
hi_sdsfree(x);
x = hi_sdsnew("--");
x = hi_sdscatfmt(x, "Hello %s World %I,%I--", "Hi!", LLONG_MIN,LLONG_MAX);
test_cond("hi_sdscatfmt() seems working in the base case",
hi_sdslen(x) == 60 &&
memcmp(x,"--Hello Hi! World -9223372036854775808,"
"9223372036854775807--",60) == 0)
printf("[%s]\n",x);
hi_sdsfree(x);
x = hi_sdsnew("--");
x = hi_sdscatfmt(x, "%u,%U--", UINT_MAX, ULLONG_MAX);
test_cond("hi_sdscatfmt() seems working with unsigned numbers",
hi_sdslen(x) == 35 &&
memcmp(x,"--4294967295,18446744073709551615--",35) == 0)
hi_sdsfree(x);
x = hi_sdsnew(" x ");
hi_sdstrim(x," x");
test_cond("hi_sdstrim() works when all chars match",
hi_sdslen(x) == 0)
hi_sdsfree(x);
x = hi_sdsnew(" x ");
hi_sdstrim(x," ");
test_cond("hi_sdstrim() works when a single char remains",
hi_sdslen(x) == 1 && x[0] == 'x')
hi_sdsfree(x);
x = hi_sdsnew("xxciaoyyy");
hi_sdstrim(x,"xy");
test_cond("hi_sdstrim() correctly trims characters",
hi_sdslen(x) == 4 && memcmp(x,"ciao\0",5) == 0)
y = hi_sdsdup(x);
hi_sdsrange(y,1,1);
test_cond("hi_sdsrange(...,1,1)",
hi_sdslen(y) == 1 && memcmp(y,"i\0",2) == 0)
hi_sdsfree(y);
y = hi_sdsdup(x);
hi_sdsrange(y,1,-1);
test_cond("hi_sdsrange(...,1,-1)",
hi_sdslen(y) == 3 && memcmp(y,"iao\0",4) == 0)
hi_sdsfree(y);
y = hi_sdsdup(x);
hi_sdsrange(y,-2,-1);
test_cond("hi_sdsrange(...,-2,-1)",
hi_sdslen(y) == 2 && memcmp(y,"ao\0",3) == 0)
hi_sdsfree(y);
y = hi_sdsdup(x);
hi_sdsrange(y,2,1);
test_cond("hi_sdsrange(...,2,1)",
hi_sdslen(y) == 0 && memcmp(y,"\0",1) == 0)
hi_sdsfree(y);
y = hi_sdsdup(x);
hi_sdsrange(y,1,100);
test_cond("hi_sdsrange(...,1,100)",
hi_sdslen(y) == 3 && memcmp(y,"iao\0",4) == 0)
hi_sdsfree(y);
y = hi_sdsdup(x);
hi_sdsrange(y,100,100);
test_cond("hi_sdsrange(...,100,100)",
hi_sdslen(y) == 0 && memcmp(y,"\0",1) == 0)
hi_sdsfree(y);
hi_sdsfree(x);
x = hi_sdsnew("foo");
y = hi_sdsnew("foa");
test_cond("hi_sdscmp(foo,foa)", hi_sdscmp(x,y) > 0)
hi_sdsfree(y);
hi_sdsfree(x);
x = hi_sdsnew("bar");
y = hi_sdsnew("bar");
test_cond("hi_sdscmp(bar,bar)", hi_sdscmp(x,y) == 0)
hi_sdsfree(y);
hi_sdsfree(x);
x = hi_sdsnew("aar");
y = hi_sdsnew("bar");
test_cond("hi_sdscmp(bar,bar)", hi_sdscmp(x,y) < 0)
hi_sdsfree(y);
hi_sdsfree(x);
x = hi_sdsnewlen("\a\n\0foo\r",7);
y = hi_sdscatrepr(hi_sdsempty(),x,hi_sdslen(x));
test_cond("hi_sdscatrepr(...data...)",
memcmp(y,"\"\\a\\n\\x00foo\\r\"",15) == 0)
{
unsigned int oldfree;
char *p;
int step = 10, j, i;
hi_sdsfree(x);
hi_sdsfree(y);
x = hi_sdsnew("0");
test_cond("hi_sdsnew() free/len buffers", hi_sdslen(x) == 1 && hi_sdsavail(x) == 0);
for (i = 0; i < 10; i++) {
int oldlen = hi_sdslen(x);
x = hi_sdsMakeRoomFor(x,step);
int type = x[-1]&HI_SDS_TYPE_MASK;
test_cond("sdsMakeRoomFor() len", hi_sdslen(x) == oldlen);
if (type != HI_SDS_TYPE_5) {
test_cond("hi_sdsMakeRoomFor() free", hi_sdsavail(x) >= step);
oldfree = hi_sdsavail(x);
}
p = x+oldlen;
for (j = 0; j < step; j++) {
p[j] = 'A'+j;
}
hi_sdsIncrLen(x,step);
}
test_cond("hi_sdsMakeRoomFor() content",
memcmp("0ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ",x,101) == 0);
test_cond("sdsMakeRoomFor() final length",hi_sdslen(x)==101);
hi_sdsfree(x);
}
}
test_report();
return 0;
}
#endif
#if defined(HI_SDS_TEST_MAIN)
int main(void) {
return hi_sdsTest();
}
#endif
