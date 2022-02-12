
































#include "flexdef.h"




void mkentry(int *, int, int, int, int);
void mkprot(int[], int, int);
void mktemplate(int[], int, int);
void mv2front(int);
int tbldiff(int[], int, int[]);





































void bldtbl (int state[], int statenum, int totaltrans, int comstate, int comfreq)
{
int extptr, extrct[2][CSIZE + 1];
int mindiff, minprot, i, d;











extptr = 0;





if ((totaltrans * 100) < (numecs * PROTO_SIZE_PERCENTAGE))
mkentry (state, numecs, statenum, JAMSTATE, totaltrans);

else {



int checkcom =

comfreq * 100 > totaltrans * CHECK_COM_PERCENTAGE;

minprot = firstprot;
mindiff = totaltrans;

if (checkcom) {

for (i = firstprot; i != NIL; i = protnext[i])
if (protcomst[i] == comstate) {
minprot = i;
mindiff = tbldiff (state, minprot,
extrct[extptr]);
break;
}
}

else {






comstate = 0;

if (firstprot != NIL) {
minprot = firstprot;
mindiff = tbldiff (state, minprot,
extrct[extptr]);
}
}







if (mindiff * 100 >
totaltrans * FIRST_MATCH_DIFF_PERCENTAGE) {



for (i = minprot; i != NIL; i = protnext[i]) {
d = tbldiff (state, i, extrct[1 - extptr]);
if (d < mindiff) {
extptr = 1 - extptr;
mindiff = d;
minprot = i;
}
}
}





if (mindiff * 100 >
totaltrans * ACCEPTABLE_DIFF_PERCENTAGE) {





if (comfreq * 100 >=
totaltrans * TEMPLATE_SAME_PERCENTAGE)
mktemplate (state, statenum,
comstate);

else {
mkprot (state, statenum, comstate);
mkentry (state, numecs, statenum,
JAMSTATE, totaltrans);
}
}

else {
mkentry (extrct[extptr], numecs, statenum,
prottbl[minprot], mindiff);





if (mindiff * 100 >=
totaltrans * NEW_PROTO_DIFF_PERCENTAGE)
mkprot (state, statenum, comstate);











mv2front (minprot);
}
}
}










void cmptmps (void)
{
int tmpstorage[CSIZE + 1];
int *tmp = tmpstorage, i, j;
int totaltrans, trans;

peakpairs = numtemps * numecs + tblend;

if (usemecs) {



nummecs = cre8ecs (tecfwd, tecbck, numecs);
}

else
nummecs = numecs;

while (lastdfa + numtemps + 1 >= current_max_dfas)
increase_max_dfas ();



for (i = 1; i <= numtemps; ++i) {

totaltrans = 0;

for (j = 1; j <= numecs; ++j) {
trans = tnxt[numecs * i + j];

if (usemecs) {




if (tecbck[j] > 0) {
tmp[tecbck[j]] = trans;

if (trans > 0)
++totaltrans;
}
}

else {
tmp[j] = trans;

if (trans > 0)
++totaltrans;
}
}









mkentry (tmp, nummecs, lastdfa + i + 1, JAMSTATE,
totaltrans);
}
}





void expand_nxt_chk (void)
{
int old_max = current_max_xpairs;

current_max_xpairs += MAX_XPAIRS_INCREMENT;

++num_reallocs;

nxt = reallocate_integer_array (nxt, current_max_xpairs);
chk = reallocate_integer_array (chk, current_max_xpairs);

memset(chk + old_max, 0, MAX_XPAIRS_INCREMENT * sizeof(int));
}





















int find_table_space (int *state, int numtrans)
{



int i;
int *state_ptr, *chk_ptr;
int *ptr_to_last_entry_in_state;




if (numtrans > MAX_XTIONS_FULL_INTERIOR_FIT) {



if (tblend < 2)
return 1;




i = tblend - numecs;
}

else




i = firstfree;

while (1) {
while (i + numecs >= current_max_xpairs)
expand_nxt_chk ();




while (1) {

if (chk[i - 1] == 0) {

if (chk[i] == 0)
break;

else





i += 2;
}

else
++i;

while (i + numecs >= current_max_xpairs)
expand_nxt_chk ();
}




if (numtrans <= MAX_XTIONS_FULL_INTERIOR_FIT)
firstfree = i + 1;





state_ptr = &state[1];
ptr_to_last_entry_in_state = &chk[i + numecs + 1];

for (chk_ptr = &chk[i + 1];
chk_ptr != ptr_to_last_entry_in_state; ++chk_ptr)
if (*(state_ptr++) != 0 && *chk_ptr != 0)
break;

if (chk_ptr == ptr_to_last_entry_in_state)
return i;

else
++i;
}
}







void inittbl (void)
{
int i;

memset(chk, 0, (size_t) current_max_xpairs * sizeof(int));

tblend = 0;
firstfree = tblend + 1;
numtemps = 0;

if (usemecs) {





tecbck[1] = NIL;

for (i = 2; i <= numecs; ++i) {
tecbck[i] = i - 1;
tecfwd[i - 1] = i;
}

tecfwd[numecs] = NIL;
}
}




void mkdeftbl (void)
{
int i;

jamstate = lastdfa + 1;

++tblend;

while (tblend + numecs >= current_max_xpairs)
expand_nxt_chk ();


nxt[tblend] = end_of_buffer_state;
chk[tblend] = jamstate;

for (i = 1; i <= numecs; ++i) {
nxt[tblend + i] = 0;
chk[tblend + i] = jamstate;
}

jambase = tblend;

base[jamstate] = jambase;
def[jamstate] = 0;

tblend += numecs;
++numtemps;
}





















void mkentry (int *state, int numchars, int statenum, int deflink,
int totaltrans)
{
int minec, maxec, i, baseaddr;
int tblbase, tbllast;

if (totaltrans == 0) {
if (deflink == JAMSTATE)
base[statenum] = JAMSTATE;
else
base[statenum] = 0;

def[statenum] = deflink;
return;
}

for (minec = 1; minec <= numchars; ++minec) {
if (state[minec] != SAME_TRANS)
if (state[minec] != 0 || deflink != JAMSTATE)
break;
}

if (totaltrans == 1) {



stack1 (statenum, minec, state[minec], deflink);
return;
}

for (maxec = numchars; maxec > 0; --maxec) {
if (state[maxec] != SAME_TRANS)
if (state[maxec] != 0 || deflink != JAMSTATE)
break;
}











if (totaltrans * 100 <= numchars * INTERIOR_FIT_PERCENTAGE) {

baseaddr = firstfree;

while (baseaddr < minec) {



for (++baseaddr; chk[baseaddr] != 0; ++baseaddr) ;
}

while (baseaddr + maxec - minec + 1 >= current_max_xpairs)
expand_nxt_chk ();

for (i = minec; i <= maxec; ++i)
if (state[i] != SAME_TRANS &&
(state[i] != 0 || deflink != JAMSTATE) &&
chk[baseaddr + i - minec] != 0) {
for (++baseaddr;
baseaddr < current_max_xpairs &&
chk[baseaddr] != 0; ++baseaddr) ;

while (baseaddr + maxec - minec + 1 >=
current_max_xpairs)
expand_nxt_chk ();





i = minec - 1;
}
}

else {



baseaddr = MAX (tblend + 1, minec);
}

tblbase = baseaddr - minec;
tbllast = tblbase + maxec;

while (tbllast + 1 >= current_max_xpairs)
expand_nxt_chk ();

base[statenum] = tblbase;
def[statenum] = deflink;

for (i = minec; i <= maxec; ++i)
if (state[i] != SAME_TRANS)
if (state[i] != 0 || deflink != JAMSTATE) {
nxt[tblbase + i] = state[i];
chk[tblbase + i] = statenum;
}

if (baseaddr == firstfree)

for (++firstfree; chk[firstfree] != 0; ++firstfree) ;

tblend = MAX (tblend, tbllast);
}






void mk1tbl (int state, int sym, int onenxt, int onedef)
{
if (firstfree < sym)
firstfree = sym;

while (chk[firstfree] != 0)
if (++firstfree >= current_max_xpairs)
expand_nxt_chk ();

base[state] = firstfree - sym;
def[state] = onedef;
chk[firstfree] = state;
nxt[firstfree] = onenxt;

if (firstfree > tblend) {
tblend = firstfree++;

if (firstfree >= current_max_xpairs)
expand_nxt_chk ();
}
}




void mkprot (int state[], int statenum, int comstate)
{
int i, slot, tblbase;

if (++numprots >= MSP || numecs * numprots >= PROT_SAVE_SIZE) {



slot = lastprot;
lastprot = protprev[lastprot];
protnext[lastprot] = NIL;
}

else
slot = numprots;

protnext[slot] = firstprot;

if (firstprot != NIL)
protprev[firstprot] = slot;

firstprot = slot;
prottbl[slot] = statenum;
protcomst[slot] = comstate;


tblbase = numecs * (slot - 1);

for (i = 1; i <= numecs; ++i)
protsave[tblbase + i] = state[i];
}






void mktemplate (int state[], int statenum, int comstate)
{
int i, numdiff, tmpbase, tmp[CSIZE + 1];
unsigned char transset[CSIZE + 1];
int tsptr;

++numtemps;

tsptr = 0;






tmpbase = numtemps * numecs;

if (tmpbase + numecs >= current_max_template_xpairs) {
current_max_template_xpairs +=
MAX_TEMPLATE_XPAIRS_INCREMENT;

++num_reallocs;

tnxt = reallocate_integer_array (tnxt,
current_max_template_xpairs);
}

for (i = 1; i <= numecs; ++i)
if (state[i] == 0)
tnxt[tmpbase + i] = 0;
else {

transset[tsptr++] = (unsigned char) i;
tnxt[tmpbase + i] = comstate;
}

if (usemecs)
mkeccl (transset, tsptr, tecfwd, tecbck, numecs, 0);

mkprot (tnxt + tmpbase, -numtemps, comstate);





numdiff = tbldiff (state, firstprot, tmp);
mkentry (tmp, numecs, statenum, -numtemps, numdiff);
}




void mv2front (int qelm)
{
if (firstprot != qelm) {
if (qelm == lastprot)
lastprot = protprev[lastprot];

protnext[protprev[qelm]] = protnext[qelm];

if (protnext[qelm] != NIL)
protprev[protnext[qelm]] = protprev[qelm];

protprev[qelm] = NIL;
protnext[qelm] = firstprot;
protprev[firstprot] = qelm;
firstprot = qelm;
}
}









void place_state (int *state, int statenum, int transnum)
{
int i;
int *state_ptr;
int position = find_table_space (state, transnum);


base[statenum] = position;






chk[position - 1] = 1;




chk[position] = 1;


state_ptr = &state[1];

for (i = 1; i <= numecs; ++i, ++state_ptr)
if (*state_ptr != 0) {
chk[position + i] = i;
nxt[position + i] = *state_ptr;
}

if (position + numecs > tblend)
tblend = position + numecs;
}









void stack1 (int statenum, int sym, int nextstate, int deflink)
{
if (onesp >= ONE_STACK_SIZE - 1)
mk1tbl (statenum, sym, nextstate, deflink);

else {
++onesp;
onestate[onesp] = statenum;
onesym[onesp] = sym;
onenext[onesp] = nextstate;
onedef[onesp] = deflink;
}
}
















int tbldiff (int state[], int pr, int ext[])
{
int i, *sp = state, *ep = ext, *protp;
int numdiff = 0;

protp = &protsave[numecs * (pr - 1)];

for (i = numecs; i > 0; --i) {
if (*++protp == *++sp)
*++ep = SAME_TRANS;
else {
*++ep = *sp;
++numdiff;
}
}

return numdiff;
}
