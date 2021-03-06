








































































#include <limits.h>
#include "make.h"


MAKE_RCSID("$NetBSD: buf.c,v 1.53 2021/11/28 22:48:06 rillig Exp $");


void
Buf_Expand(Buffer *buf)
{
buf->cap += buf->cap > 16 ? buf->cap : 16;
buf->data = bmake_realloc(buf->data, buf->cap);
}


void
Buf_AddBytes(Buffer *buf, const char *bytes, size_t bytes_len)
{
size_t old_len = buf->len;
char *end;

if (old_len + bytes_len >= buf->cap) {
size_t minIncr = bytes_len + 16;
buf->cap += buf->cap > minIncr ? buf->cap : minIncr;
buf->data = bmake_realloc(buf->data, buf->cap);
}

end = buf->data + old_len;
buf->len = old_len + bytes_len;
memcpy(end, bytes, bytes_len);
end[bytes_len] = '\0';
}


void
Buf_AddBytesBetween(Buffer *buf, const char *start, const char *end)
{
Buf_AddBytes(buf, start, (size_t)(end - start));
}


void
Buf_AddStr(Buffer *buf, const char *str)
{
Buf_AddBytes(buf, str, strlen(str));
}


void
Buf_AddInt(Buffer *buf, int n)
{
char str[sizeof(int) * CHAR_BIT + 1];

size_t len = (size_t)snprintf(str, sizeof str, "%d", n);
Buf_AddBytes(buf, str, len);
}

void
Buf_AddFlag(Buffer *buf, bool flag, const char *name)
{
if (flag) {
if (buf->len > 0)
Buf_AddByte(buf, '|');
Buf_AddBytes(buf, name, strlen(name));
}
}


void
Buf_Empty(Buffer *buf)
{
buf->len = 0;
buf->data[0] = '\0';
}


void
Buf_InitSize(Buffer *buf, size_t cap)
{
buf->cap = cap;
buf->len = 0;
buf->data = bmake_malloc(cap);
buf->data[0] = '\0';
}

void
Buf_Init(Buffer *buf)
{
Buf_InitSize(buf, 256);
}





void
Buf_Done(Buffer *buf)
{
free(buf->data);

#if defined(CLEANUP)
buf->cap = 0;
buf->len = 0;
buf->data = NULL;
#endif
}





char *
Buf_DoneData(Buffer *buf)
{
char *data = buf->data;

#if defined(CLEANUP)
buf->cap = 0;
buf->len = 0;
buf->data = NULL;
#endif

return data;
}

#if !defined(BUF_COMPACT_LIMIT)
#define BUF_COMPACT_LIMIT 128
#endif








char *
Buf_DoneDataCompact(Buffer *buf)
{
#if BUF_COMPACT_LIMIT > 0
if (buf->cap - buf->len >= BUF_COMPACT_LIMIT) {

char *data = bmake_realloc(buf->data, buf->len + 1);
data[buf->len] = '\0';
Buf_DoneData(buf);
return data;
}
#endif
return Buf_DoneData(buf);
}
