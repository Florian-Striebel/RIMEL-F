

































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)output.c 8.2 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");











#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

#include "shell.h"
#include "syntax.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "var.h"


#define OUTBUFSIZ BUFSIZ
#define MEM_OUT -2
#define OUTPUT_ERR 01

static int doformat_wr(void *, const char *, int);

struct output output = {NULL, NULL, NULL, OUTBUFSIZ, 1, 0};
struct output errout = {NULL, NULL, NULL, 256, 2, 0};
struct output memout = {NULL, NULL, NULL, 64, MEM_OUT, 0};
struct output *out1 = &output;
struct output *out2 = &errout;

void
outcslow(int c, struct output *file)
{
outc(c, file);
}

void
out1str(const char *p)
{
outstr(p, out1);
}

void
out1qstr(const char *p)
{
outqstr(p, out1);
}

void
out2str(const char *p)
{
outstr(p, out2);
}

void
out2qstr(const char *p)
{
outqstr(p, out2);
}

void
outstr(const char *p, struct output *file)
{
outbin(p, strlen(p), file);
}

static void
byteseq(int ch, struct output *file)
{
char seq[4];

seq[0] = '\\';
seq[1] = (ch >> 6 & 0x3) + '0';
seq[2] = (ch >> 3 & 0x7) + '0';
seq[3] = (ch & 0x7) + '0';
outbin(seq, 4, file);
}

static void
outdqstr(const char *p, struct output *file)
{
const char *end;
mbstate_t mbs;
size_t clen;
wchar_t wc;

memset(&mbs, '\0', sizeof(mbs));
end = p + strlen(p);
outstr("$'", file);
while ((clen = mbrtowc(&wc, p, end - p + 1, &mbs)) != 0) {
if (clen == (size_t)-2) {
while (p < end)
byteseq(*p++, file);
break;
}
if (clen == (size_t)-1) {
memset(&mbs, '\0', sizeof(mbs));
byteseq(*p++, file);
continue;
}
if (wc == L'\n')
outcslow('\n', file), p++;
else if (wc == L'\r')
outstr("\\r", file), p++;
else if (wc == L'\t')
outstr("\\t", file), p++;
else if (!iswprint(wc)) {
for (; clen > 0; clen--)
byteseq(*p++, file);
} else {
if (wc == L'\'' || wc == L'\\')
outcslow('\\', file);
outbin(p, clen, file);
p += clen;
}
}
outcslow('\'', file);
}


void
outqstr(const char *p, struct output *file)
{
int i;

if (p[0] == '\0') {
outstr("''", file);
return;
}
for (i = 0; p[i] != '\0'; i++) {
if ((p[i] > '\0' && p[i] < ' ' && p[i] != '\n') ||
(p[i] & 0x80) != 0 || p[i] == '\'') {
outdqstr(p, file);
return;
}
}

if (p[strcspn(p, "|&;<>()$`\\\" \n*?[~#=")] == '\0' ||
strcmp(p, "[") == 0) {
outstr(p, file);
return;
}

outcslow('\'', file);
outstr(p, file);
outcslow('\'', file);
}

void
outbin(const void *data, size_t len, struct output *file)
{
const char *p;

p = data;
while (len-- > 0)
outc(*p++, file);
}

void
emptyoutbuf(struct output *dest)
{
int offset, newsize;

if (dest->buf == NULL) {
INTOFF;
dest->buf = ckmalloc(dest->bufsize);
dest->nextc = dest->buf;
dest->bufend = dest->buf + dest->bufsize;
INTON;
} else if (dest->fd == MEM_OUT) {
offset = dest->nextc - dest->buf;
newsize = dest->bufsize << 1;
INTOFF;
dest->buf = ckrealloc(dest->buf, newsize);
dest->bufsize = newsize;
dest->bufend = dest->buf + newsize;
dest->nextc = dest->buf + offset;
INTON;
} else {
flushout(dest);
}
}


void
flushall(void)
{
flushout(&output);
flushout(&errout);
}


void
flushout(struct output *dest)
{

if (dest->buf == NULL || dest->nextc == dest->buf || dest->fd < 0)
return;
if (xwrite(dest->fd, dest->buf, dest->nextc - dest->buf) < 0)
dest->flags |= OUTPUT_ERR;
dest->nextc = dest->buf;
}


void
freestdout(void)
{
output.nextc = output.buf;
}


int
outiserror(struct output *file)
{
return (file->flags & OUTPUT_ERR);
}


void
outclearerror(struct output *file)
{
file->flags &= ~OUTPUT_ERR;
}


void
outfmt(struct output *file, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
doformat(file, fmt, ap);
va_end(ap);
}


void
out1fmt(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
doformat(out1, fmt, ap);
va_end(ap);
}

void
out2fmt_flush(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
doformat(out2, fmt, ap);
va_end(ap);
flushout(out2);
}

void
fmtstr(char *outbuf, int length, const char *fmt, ...)
{
va_list ap;

INTOFF;
va_start(ap, fmt);
vsnprintf(outbuf, length, fmt, ap);
va_end(ap);
INTON;
}

static int
doformat_wr(void *cookie, const char *buf, int len)
{
struct output *o;

o = (struct output *)cookie;
outbin(buf, len, o);

return (len);
}

void
doformat(struct output *dest, const char *f, va_list ap)
{
FILE *fp;

if ((fp = fwopen(dest, doformat_wr)) != NULL) {
vfprintf(fp, f, ap);
fclose(fp);
}
}

FILE *
out1fp(void)
{
return fwopen(out1, doformat_wr);
}





int
xwrite(int fd, const char *buf, int nbytes)
{
int ntry;
int i;
int n;

n = nbytes;
ntry = 0;
for (;;) {
i = write(fd, buf, n);
if (i > 0) {
if ((n -= i) <= 0)
return nbytes;
buf += i;
ntry = 0;
} else if (i == 0) {
if (++ntry > 10)
return nbytes - n;
} else if (errno != EINTR) {
return -1;
}
}
}
