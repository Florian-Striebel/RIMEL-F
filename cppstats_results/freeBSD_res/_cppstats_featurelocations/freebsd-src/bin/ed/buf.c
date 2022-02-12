



























#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/file.h>
#include <sys/stat.h>

#include "ed.h"


static FILE *sfp;
static off_t sfseek;
static int seek_write;
static line_t buffer_head;



char *
get_sbuf_line(line_t *lp)
{
static char *sfbuf = NULL;
static size_t sfbufsz;

size_t len;

if (lp == &buffer_head)
return NULL;
seek_write = 1;

if (sfseek != lp->seek) {
sfseek = lp->seek;
if (fseeko(sfp, sfseek, SEEK_SET) < 0) {
fprintf(stderr, "%s\n", strerror(errno));
errmsg = "cannot seek temp file";
return NULL;
}
}
len = lp->len;
REALLOC(sfbuf, sfbufsz, len + 1, NULL);
if (fread(sfbuf, sizeof(char), len, sfp) != len) {
fprintf(stderr, "%s\n", strerror(errno));
errmsg = "cannot read temp file";
return NULL;
}
sfseek += len;
sfbuf[len] = '\0';
return sfbuf;
}




const char *
put_sbuf_line(const char *cs)
{
line_t *lp;
size_t len;
const char *s;

if ((lp = (line_t *) malloc(sizeof(line_t))) == NULL) {
fprintf(stderr, "%s\n", strerror(errno));
errmsg = "out of memory";
return NULL;
}

for (s = cs; *s != '\n'; s++)
;
if (s - cs >= LINECHARS) {
errmsg = "line too long";
free(lp);
return NULL;
}
len = s - cs;

if (seek_write) {
if (fseeko(sfp, (off_t)0, SEEK_END) < 0) {
fprintf(stderr, "%s\n", strerror(errno));
errmsg = "cannot seek temp file";
free(lp);
return NULL;
}
sfseek = ftello(sfp);
seek_write = 0;
}

if (fwrite(cs, sizeof(char), len, sfp) != len) {
sfseek = -1;
fprintf(stderr, "%s\n", strerror(errno));
errmsg = "cannot write temp file";
free(lp);
return NULL;
}
lp->len = len;
lp->seek = sfseek;
add_line_node(lp);
sfseek += len;
return ++s;
}



void
add_line_node(line_t *lp)
{
line_t *cp;

cp = get_addressed_line_node(current_addr);
INSQUE(lp, cp);
addr_last++;
current_addr++;
}



long
get_line_node_addr(line_t *lp)
{
line_t *cp = &buffer_head;
long n = 0;

while (cp != lp && (cp = cp->q_forw) != &buffer_head)
n++;
if (n && cp == &buffer_head) {
errmsg = "invalid address";
return ERR;
}
return n;
}



line_t *
get_addressed_line_node(long n)
{
static line_t *lp = &buffer_head;
static long on = 0;

SPL1();
if (n > on)
if (n <= (on + addr_last) >> 1)
for (; on < n; on++)
lp = lp->q_forw;
else {
lp = buffer_head.q_back;
for (on = addr_last; on > n; on--)
lp = lp->q_back;
}
else
if (n >= on >> 1)
for (; on > n; on--)
lp = lp->q_back;
else {
lp = &buffer_head;
for (on = 0; on < n; on++)
lp = lp->q_forw;
}
SPL0();
return lp;
}

static char sfn[15] = "";


int
open_sbuf(void)
{
int fd;
int u;

isbinary = newline_added = 0;
u = umask(077);
strcpy(sfn, "/tmp/ed.XXXXXX");
if ((fd = mkstemp(sfn)) == -1 ||
(sfp = fdopen(fd, "w+")) == NULL) {
if (fd != -1)
close(fd);
perror(sfn);
errmsg = "cannot open temp file";
umask(u);
return ERR;
}
umask(u);
return 0;
}



int
close_sbuf(void)
{
if (sfp) {
if (fclose(sfp) < 0) {
fprintf(stderr, "%s: %s\n", sfn, strerror(errno));
errmsg = "cannot close temp file";
return ERR;
}
sfp = NULL;
unlink(sfn);
}
sfseek = seek_write = 0;
return 0;
}



void
quit(int n)
{
if (sfp) {
fclose(sfp);
unlink(sfn);
}
exit(n);
}


static unsigned char ctab[256];


void
init_buffers(void)
{
int i = 0;







setbuffer(stdin, stdinbuf, 1);



setvbuf(stdout, NULL, _IOLBF, 0);
if (open_sbuf() < 0)
quit(2);
REQUE(&buffer_head, &buffer_head);
for (i = 0; i < 256; i++)
ctab[i] = i;
}



char *
translit_text(char *s, int len, int from, int to)
{
static int i = 0;

unsigned char *us;

ctab[i] = i;
ctab[i = from] = to;
for (us = (unsigned char *) s; len-- > 0; us++)
*us = ctab[*us];
return s;
}
