





























#if !defined(TEST)
#include "file.h"

#if !defined(lint)
FILE_RCSID("@(#)$File: is_csv.c,v 1.6 2020/08/09 16:43:36 christos Exp $")
#endif

#include <string.h>
#include "magic.h"
#else
#include <sys/types.h>
#endif


#if defined(DEBUG)
#include <stdio.h>
#define DPRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DPRINTF(fmt, ...)
#endif









#if !defined(CSV_LINES)
#define CSV_LINES 10
#endif

static int csv_parse(const unsigned char *, const unsigned char *);

static const unsigned char *
eatquote(const unsigned char *uc, const unsigned char *ue)
{
int quote = 0;

while (uc < ue) {
unsigned char c = *uc++;
if (c != '"') {

if (quote) {
return --uc;
}
continue;
}
if (quote) {

quote = 0;
continue;
}

quote = 1;
}
return ue;
}

static int
csv_parse(const unsigned char *uc, const unsigned char *ue)
{
size_t nf = 0, tf = 0, nl = 0;

while (uc < ue) {
switch (*uc++) {
case '"':

uc = eatquote(uc, ue);
break;
case ',':
nf++;
break;
case '\n':
DPRINTF("%zu %zu %zu\n", nl, nf, tf);
nl++;
#if CSV_LINES
if (nl == CSV_LINES)
return tf != 0 && tf == nf;
#endif
if (tf == 0) {

if (nf == 0)
return 0;

tf = nf;
} else if (tf != nf) {

return 0;
}
nf = 0;
break;
default:
break;
}
}
return tf && nl > 2;
}

#if !defined(TEST)
int
file_is_csv(struct magic_set *ms, const struct buffer *b, int looks_text)
{
const unsigned char *uc = CAST(const unsigned char *, b->fbuf);
const unsigned char *ue = uc + b->flen;
int mime = ms->flags & MAGIC_MIME;

if (!looks_text)
return 0;

if ((ms->flags & (MAGIC_APPLE|MAGIC_EXTENSION)) != 0)
return 0;

if (!csv_parse(uc, ue))
return 0;

if (mime == MAGIC_MIME_ENCODING)
return 1;

if (mime) {
if (file_printf(ms, "text/csv") == -1)
return -1;
return 1;
}

if (file_printf(ms, "CSV text") == -1)
return -1;

return 1;
}

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

int
main(int argc, char *argv[])
{
int fd, rv;
struct stat st;
unsigned char *p;

if ((fd = open(argv[1], O_RDONLY)) == -1)
err(EXIT_FAILURE, "Can't open `%s'", argv[1]);

if (fstat(fd, &st) == -1)
err(EXIT_FAILURE, "Can't stat `%s'", argv[1]);

if ((p = malloc(st.st_size)) == NULL)
err(EXIT_FAILURE, "Can't allocate %jd bytes",
(intmax_t)st.st_size);
if (read(fd, p, st.st_size) != st.st_size)
err(EXIT_FAILURE, "Can't read %jd bytes",
(intmax_t)st.st_size);
printf("is csv %d\n", csv_parse(p, p + st.st_size));
return 0;
}
#endif
