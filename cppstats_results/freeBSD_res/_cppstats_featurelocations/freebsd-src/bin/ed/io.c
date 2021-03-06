


























#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "ed.h"


long
read_file(char *fn, long n)
{
FILE *fp;
long size;
int cs;

fp = (*fn == '!') ? popen(fn + 1, "r") : fopen(strip_escapes(fn), "r");
if (fp == NULL) {
fprintf(stderr, "%s: %s\n", fn, strerror(errno));
errmsg = "cannot open input file";
return ERR;
}
if ((size = read_stream(fp, n)) < 0) {
fprintf(stderr, "%s: %s\n", fn, strerror(errno));
errmsg = "error reading input file";
}
if ((cs = (*fn == '!') ? pclose(fp) : fclose(fp)) < 0) {
fprintf(stderr, "%s: %s\n", fn, strerror(errno));
errmsg = "cannot close input file";
}
if (size < 0 || cs < 0)
return ERR;
if (!scripted)
fprintf(stdout, "%lu\n", size);
return current_addr - n;
}

static char *sbuf;
static int sbufsz;
int newline_added;


long
read_stream(FILE *fp, long n)
{
line_t *lp = get_addressed_line_node(n);
undo_t *up = NULL;
unsigned long size = 0;
int o_newline_added = newline_added;
int o_isbinary = isbinary;
int appended = (n == addr_last);
int len;

isbinary = newline_added = 0;
for (current_addr = n; (len = get_stream_line(fp)) > 0; size += len) {
SPL1();
if (put_sbuf_line(sbuf) == NULL) {
SPL0();
return ERR;
}
lp = lp->q_forw;
if (up)
up->t = lp;
else if ((up = push_undo_stack(UADD, current_addr,
current_addr)) == NULL) {
SPL0();
return ERR;
}
SPL0();
}
if (len < 0)
return ERR;
if (appended && size && o_isbinary && o_newline_added)
fputs("newline inserted\n", stderr);
else if (newline_added && (!appended || (!isbinary && !o_isbinary)))
fputs("newline appended\n", stderr);
if (isbinary && newline_added && !appended)
size += 1;
if (!size)
newline_added = 1;
newline_added = appended ? newline_added : o_newline_added;
isbinary = isbinary | o_isbinary;
return size;
}



int
get_stream_line(FILE *fp)
{
int c;
int i = 0;

while (((c = getc(fp)) != EOF || (!feof(fp) && !ferror(fp))) &&
c != '\n') {
REALLOC(sbuf, sbufsz, i + 1, ERR);
if (!(sbuf[i++] = c))
isbinary = 1;
}
REALLOC(sbuf, sbufsz, i + 2, ERR);
if (c == '\n')
sbuf[i++] = c;
else if (ferror(fp)) {
fprintf(stderr, "%s\n", strerror(errno));
errmsg = "cannot read input file";
return ERR;
} else if (i) {
sbuf[i++] = '\n';
newline_added = 1;
}
sbuf[i] = '\0';
return (isbinary && newline_added && i) ? --i : i;
}



long
write_file(char *fn, const char *mode, long n, long m)
{
FILE *fp;
long size;
int cs;

fp = (*fn == '!') ? popen(fn+1, "w") : fopen(strip_escapes(fn), mode);
if (fp == NULL) {
fprintf(stderr, "%s: %s\n", fn, strerror(errno));
errmsg = "cannot open output file";
return ERR;
}
if ((size = write_stream(fp, n, m)) < 0) {
fprintf(stderr, "%s: %s\n", fn, strerror(errno));
errmsg = "error writing output file";
}
if ((cs = (*fn == '!') ? pclose(fp) : fclose(fp)) < 0) {
fprintf(stderr, "%s: %s\n", fn, strerror(errno));
errmsg = "cannot close output file";
}
if (size < 0 || cs < 0)
return ERR;
if (!scripted)
fprintf(stdout, "%lu\n", size);
return n ? m - n + 1 : 0;
}



long
write_stream(FILE *fp, long n, long m)
{
line_t *lp = get_addressed_line_node(n);
unsigned long size = 0;
char *s;
int len;

for (; n && n <= m; n++, lp = lp->q_forw) {
if ((s = get_sbuf_line(lp)) == NULL)
return ERR;
len = lp->len;
if (n != addr_last || !isbinary || !newline_added)
s[len++] = '\n';
if (put_stream_line(fp, s, len) < 0)
return ERR;
size += len;
}
return size;
}



int
put_stream_line(FILE *fp, const char *s, int len)
{
while (len--)
if (fputc(*s++, fp) < 0) {
fprintf(stderr, "%s\n", strerror(errno));
errmsg = "cannot write file";
return ERR;
}
return 0;
}


char *
get_extended_line(int *sizep, int nonl)
{
static char *cvbuf = NULL;
static int cvbufsz = 0;

int l, n;
char *t = ibufp;

while (*t++ != '\n')
;
if ((l = t - ibufp) < 2 || !has_trailing_escape(ibufp, ibufp + l - 1)) {
*sizep = l;
return ibufp;
}
*sizep = -1;
REALLOC(cvbuf, cvbufsz, l, NULL);
memcpy(cvbuf, ibufp, l);
*(cvbuf + --l - 1) = '\n';
if (nonl) l--;
for (;;) {
if ((n = get_tty_line()) < 0)
return NULL;
else if (n == 0 || ibuf[n - 1] != '\n') {
errmsg = "unexpected end-of-file";
return NULL;
}
REALLOC(cvbuf, cvbufsz, l + n, NULL);
memcpy(cvbuf + l, ibuf, n);
l += n;
if (n < 2 || !has_trailing_escape(cvbuf, cvbuf + l - 1))
break;
*(cvbuf + --l - 1) = '\n';
if (nonl) l--;
}
REALLOC(cvbuf, cvbufsz, l + 1, NULL);
cvbuf[l] = '\0';
*sizep = l;
return cvbuf;
}



int
get_tty_line(void)
{
int oi = 0;
int i = 0;
int c;

for (;;)
switch (c = getchar()) {
default:
oi = 0;
REALLOC(ibuf, ibufsz, i + 2, ERR);
if (!(ibuf[i++] = c)) isbinary = 1;
if (c != '\n')
continue;
lineno++;
ibuf[i] = '\0';
ibufp = ibuf;
return i;
case EOF:
if (ferror(stdin)) {
fprintf(stderr, "stdin: %s\n", strerror(errno));
errmsg = "cannot read stdin";
clearerr(stdin);
ibufp = NULL;
return ERR;
} else {
clearerr(stdin);
if (i != oi) {
oi = i;
continue;
} else if (i)
ibuf[i] = '\0';
ibufp = ibuf;
return i;
}
}
}



#define ESCAPES "\a\b\f\n\r\t\v\\"
#define ESCCHARS "abfnrtv\\"


int
put_tty_line(const char *s, int l, long n, int gflag)
{
int col = 0;
int lc = 0;
char *cp;

if (gflag & GNP) {
printf("%ld\t", n);
col = 8;
}
for (; l--; s++) {
if ((gflag & GLS) && ++col > cols) {
fputs("\\\n", stdout);
col = 1;
#if !defined(BACKWARDS)
if (!scripted && !isglobal && ++lc > rows) {
lc = 0;
fputs("Press <RETURN> to continue... ", stdout);
fflush(stdout);
if (get_tty_line() < 0)
return ERR;
}
#endif
}
if (gflag & GLS) {
if (31 < *s && *s < 127 && *s != '\\')
putchar(*s);
else {
putchar('\\');
col++;
if (*s && (cp = strchr(ESCAPES, *s)) != NULL)
putchar(ESCCHARS[cp - ESCAPES]);
else {
putchar((((unsigned char) *s & 0300) >> 6) + '0');
putchar((((unsigned char) *s & 070) >> 3) + '0');
putchar(((unsigned char) *s & 07) + '0');
col += 2;
}
}

} else
putchar(*s);
}
#if !defined(BACKWARDS)
if (gflag & GLS)
putchar('$');
#endif
putchar('\n');
return 0;
}
