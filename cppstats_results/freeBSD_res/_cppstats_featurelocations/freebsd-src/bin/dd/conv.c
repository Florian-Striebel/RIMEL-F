


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)conv.c 8.3 (Berkeley) 4/2/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>

#include <err.h>
#include <inttypes.h>
#include <string.h>

#include "dd.h"
#include "extern.h"







void
def(void)
{
u_char *inp;
const u_char *t;
size_t cnt;

if ((t = ctab) != NULL)
for (inp = in.dbp - (cnt = in.dbrcnt); cnt--; ++inp)
*inp = t[*inp];


out.dbp = in.dbp;
out.dbcnt = in.dbcnt;

if (in.dbcnt >= out.dbsz) {

dd_out(0);






in.dbp = out.dbp;
in.dbcnt = out.dbcnt;
}
}

void
def_close(void)
{

if (in.dbcnt)
out.dbcnt = in.dbcnt;
}








void
block(void)
{
u_char *inp, *outp;
const u_char *t;
size_t cnt, maxlen;
static int intrunc;
int ch;







if (intrunc) {
for (inp = in.db, cnt = in.dbrcnt; cnt && *inp++ != '\n'; --cnt)
;
if (!cnt) {
in.dbcnt = 0;
in.dbp = in.db;
return;
}
intrunc = 0;

in.dbcnt = cnt - 1;
in.dbp = inp + cnt - 1;
}





ch = 0;
for (inp = in.dbp - in.dbcnt, outp = out.dbp; in.dbcnt;) {
maxlen = MIN(cbsz, (size_t)in.dbcnt);
if ((t = ctab) != NULL)
for (cnt = 0; cnt < maxlen && (ch = *inp++) != '\n';
++cnt)
*outp++ = t[ch];
else
for (cnt = 0; cnt < maxlen && (ch = *inp++) != '\n';
++cnt)
*outp++ = ch;




if (ch != '\n' && (size_t)in.dbcnt < cbsz) {
(void)memmove(in.db, in.dbp - in.dbcnt, in.dbcnt);
break;
}


in.dbcnt -= cnt;
if (ch == '\n')
--in.dbcnt;


if (cnt < cbsz)
(void)memset(outp, ctab ? ctab[' '] : ' ', cbsz - cnt);
else {




if (!in.dbcnt || *inp != '\n')
++st.trunc;


for (; in.dbcnt && *inp++ != '\n'; --in.dbcnt)
;
if (!in.dbcnt)
intrunc = 1;
else
--in.dbcnt;
}


out.dbp += cbsz;
if ((out.dbcnt += cbsz) >= out.dbsz)
dd_out(0);
outp = out.dbp;
}
in.dbp = in.db + in.dbcnt;
}

void
block_close(void)
{








if (in.dbcnt) {
++st.trunc;
(void)memmove(out.dbp, in.dbp - in.dbcnt, in.dbcnt);
(void)memset(out.dbp + in.dbcnt, ctab ? ctab[' '] : ' ',
cbsz - in.dbcnt);
out.dbcnt += cbsz;
}
}








void
unblock(void)
{
u_char *inp;
const u_char *t;
size_t cnt;


if ((t = ctab) != NULL)
for (inp = in.dbp - (cnt = in.dbrcnt); cnt--; ++inp)
*inp = t[*inp];





for (inp = in.db; (size_t)in.dbcnt >= cbsz; inp += cbsz, in.dbcnt -= cbsz) {
for (t = inp + cbsz - 1; t >= inp && *t == ' '; --t)
;
if (t >= inp) {
cnt = t - inp + 1;
(void)memmove(out.dbp, inp, cnt);
out.dbp += cnt;
out.dbcnt += cnt;
}
*out.dbp++ = '\n';
if (++out.dbcnt >= out.dbsz)
dd_out(0);
}
if (in.dbcnt)
(void)memmove(in.db, in.dbp - in.dbcnt, in.dbcnt);
in.dbp = in.db + in.dbcnt;
}

void
unblock_close(void)
{
u_char *t;
size_t cnt;

if (in.dbcnt) {
warnx("%s: short input record", in.name);
for (t = in.db + in.dbcnt - 1; t >= in.db && *t == ' '; --t)
;
if (t >= in.db) {
cnt = t - in.db + 1;
(void)memmove(out.dbp, in.db, cnt);
out.dbp += cnt;
out.dbcnt += cnt;
}
++out.dbcnt;
*out.dbp++ = '\n';
}
}
