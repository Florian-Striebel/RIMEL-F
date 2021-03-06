











































































#if !defined(MAKE_BUF_H)
#define MAKE_BUF_H

#include <stddef.h>


typedef struct Buffer {
size_t cap;
size_t len;
char *data;
} Buffer;

void Buf_Expand(Buffer *);


MAKE_INLINE void
Buf_AddByte(Buffer *buf, char byte)
{
size_t old_len = buf->len++;
char *end;
if (old_len + 1 >= buf->cap)
Buf_Expand(buf);
end = buf->data + old_len;
end[0] = byte;
end[1] = '\0';
}

MAKE_INLINE bool
Buf_EndsWith(const Buffer *buf, char ch)
{
return buf->len > 0 && buf->data[buf->len - 1] == ch;
}

void Buf_AddBytes(Buffer *, const char *, size_t);
void Buf_AddBytesBetween(Buffer *, const char *, const char *);
void Buf_AddStr(Buffer *, const char *);
void Buf_AddInt(Buffer *, int);
void Buf_AddFlag(Buffer *, bool, const char *);
void Buf_Empty(Buffer *);
void Buf_Init(Buffer *);
void Buf_InitSize(Buffer *, size_t);
void Buf_Done(Buffer *);
char *Buf_DoneData(Buffer *);
char *Buf_DoneDataCompact(Buffer *);

#endif
