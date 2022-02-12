

































#include "flexdef.h"



void ccl2ecl (void)
{
int i, ich, newlen, cclp, ccls, cclmec;

for (i = 1; i <= lastccl; ++i) {







newlen = 0;
cclp = cclmap[i];

for (ccls = 0; ccls < ccllen[i]; ++ccls) {
ich = ccltbl[cclp + ccls];
cclmec = ecgroup[ich];

if (cclmec > 0) {

ccltbl[cclp + newlen] = (unsigned char) cclmec;
++newlen;
}
}

ccllen[i] = newlen;
}
}










int cre8ecs (int fwd[], int bck[], int num)
{
int i, j, numcl;

numcl = 0;






for (i = 1; i <= num; ++i)
if (bck[i] == NIL) {
bck[i] = ++numcl;
for (j = fwd[i]; j != NIL; j = fwd[j])
bck[j] = -numcl;
}

return numcl;
}

















void mkeccl (unsigned char ccls[], int lenccl, int fwd[], int bck[], int llsiz, int NUL_mapping)
{
int cclp, oldec, newec;
int cclm, i, j;
static unsigned char cclflags[CSIZE];





cclp = 0;

while (cclp < lenccl) {
cclm = ccls[cclp];

if (NUL_mapping && cclm == 0)
cclm = NUL_mapping;

oldec = bck[cclm];
newec = cclm;

j = cclp + 1;

for (i = fwd[cclm]; i != NIL && i <= llsiz; i = fwd[i]) {
for (; j < lenccl; ++j) {
int ccl_char;

if (NUL_mapping && ccls[j] == 0)
ccl_char = NUL_mapping;
else
ccl_char = ccls[j];

if (ccl_char > i)
break;

if (ccl_char == i && !cclflags[j]) {






bck[i] = newec;
fwd[newec] = i;
newec = i;

cclflags[j] = 1;



goto next_pt;
}
}





bck[i] = oldec;

if (oldec != NIL)
fwd[oldec] = i;

oldec = i;

next_pt:;
}

if (bck[cclm] != NIL || oldec != bck[cclm]) {
bck[cclm] = NIL;
fwd[oldec] = NIL;
}

fwd[newec] = NIL;



for (++cclp; cclp < lenccl && cclflags[cclp]; ++cclp) {

cclflags[cclp] = 0;
}
}
}




void mkechar (int tch, int fwd[], int bck[])
{




if (fwd[tch] != NIL)
bck[fwd[tch]] = bck[tch];

if (bck[tch] != NIL)
fwd[bck[tch]] = fwd[tch];

fwd[tch] = NIL;
bck[tch] = NIL;
}
