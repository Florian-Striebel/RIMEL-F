

























#include "file.h"

#if !defined(lint)
FILE_RCSID("@(#)$File: buffer.c,v 1.8 2020/02/16 15:52:49 christos Exp $")
#endif

#include "magic.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

void
buffer_init(struct buffer *b, int fd, const struct stat *st, const void *data,
size_t len)
{
b->fd = fd;
if (st)
memcpy(&b->st, st, sizeof(b->st));
else if (b->fd == -1 || fstat(b->fd, &b->st) == -1)
memset(&b->st, 0, sizeof(b->st));
b->fbuf = data;
b->flen = len;
b->eoff = 0;
b->ebuf = NULL;
b->elen = 0;
}

void
buffer_fini(struct buffer *b)
{
free(b->ebuf);
}

int
buffer_fill(const struct buffer *bb)
{
struct buffer *b = CCAST(struct buffer *, bb);

if (b->elen != 0)
return b->elen == FILE_BADSIZE ? -1 : 0;

if (!S_ISREG(b->st.st_mode))
goto out;

b->elen = CAST(size_t, b->st.st_size) < b->flen ?
CAST(size_t, b->st.st_size) : b->flen;
if ((b->ebuf = malloc(b->elen)) == NULL)
goto out;

b->eoff = b->st.st_size - b->elen;
if (pread(b->fd, b->ebuf, b->elen, b->eoff) == -1) {
free(b->ebuf);
b->ebuf = NULL;
goto out;
}

return 0;
out:
b->elen = FILE_BADSIZE;
return -1;
}
