






































#include "file.h"

#if !defined(lint)
FILE_RCSID("@(#)$File: is_tar.c,v 1.44 2019/02/20 02:35:27 christos Exp $")
#endif

#include "magic.h"
#include <string.h>
#include <ctype.h>
#include "tar.h"

#define isodigit(c) ( ((c) >= '0') && ((c) <= '7') )

private int is_tar(const unsigned char *, size_t);
private int from_oct(const char *, size_t);

static const char tartype[][32] = {
"tar archive",
"POSIX tar archive",
"POSIX tar archive (GNU)",
};

protected int
file_is_tar(struct magic_set *ms, const struct buffer *b)
{
const unsigned char *buf = CAST(const unsigned char *, b->fbuf);
size_t nbytes = b->flen;




int tar;
int mime = ms->flags & MAGIC_MIME;

if ((ms->flags & (MAGIC_APPLE|MAGIC_EXTENSION)) != 0)
return 0;

tar = is_tar(buf, nbytes);
if (tar < 1 || tar > 3)
return 0;

if (mime == MAGIC_MIME_ENCODING)
return 1;

if (file_printf(ms, "%s", mime ? "application/x-tar" :
tartype[tar - 1]) == -1)
return -1;

return 1;
}








private int
is_tar(const unsigned char *buf, size_t nbytes)
{
const union record *header = RCAST(const union record *,
RCAST(const void *, buf));
size_t i;
int sum, recsum;
const unsigned char *p, *ep;

if (nbytes < sizeof(*header))
return 0;

recsum = from_oct(header->header.chksum, sizeof(header->header.chksum));

sum = 0;
p = header->charptr;
ep = header->charptr + sizeof(*header);
while (p < ep)
sum += *p++;


for (i = 0; i < sizeof(header->header.chksum); i++)
sum -= header->header.chksum[i];
sum += ' ' * sizeof(header->header.chksum);

if (sum != recsum)
return 0;

if (strncmp(header->header.magic, GNUTMAGIC,
sizeof(header->header.magic)) == 0)
return 3;

if (strncmp(header->header.magic, TMAGIC,
sizeof(header->header.magic)) == 0)
return 2;

return 1;
}







private int
from_oct(const char *where, size_t digs)
{
int value;

if (digs == 0)
return -1;

while (isspace(CAST(unsigned char, *where))) {
where++;
if (digs-- == 0)
return -1;
}
value = 0;
while (digs > 0 && isodigit(*where)) {
value = (value << 3) | (*where++ - '0');
digs--;
}

if (digs > 0 && *where && !isspace(CAST(unsigned char, *where)))
return -1;

return value;
}
