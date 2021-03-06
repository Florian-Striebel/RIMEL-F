






























#include "flexdef.h"
#include "tables.h"



void dump_associated_rules(FILE *, int);
void dump_transitions(FILE *, int[]);
void sympartition(int[], int, int[], int[]);
int symfollowset(int[], int, int, int[]);











void check_for_backing_up (int ds, int state[])
{
if ((reject && !dfaacc[ds].dfaacc_set) || (!reject && !dfaacc[ds].dfaacc_state)) {
++num_backing_up;

if (backing_up_report) {
fprintf (backing_up_file,
_("State #%d is non-accepting -\n"), ds);


dump_associated_rules (backing_up_file, ds);




dump_transitions (backing_up_file, state);

putc ('\n', backing_up_file);
}
}
}
























void check_trailing_context (int *nfa_states, int num_states, int *accset, int nacc)
{
int i, j;

for (i = 1; i <= num_states; ++i) {
int ns = nfa_states[i];
int type = state_type[ns];
int ar = assoc_rule[ns];

if (type == STATE_NORMAL || rule_type[ar] != RULE_VARIABLE) {
}

else if (type == STATE_TRAILING_CONTEXT) {






for (j = 1; j <= nacc; ++j)
if (accset[j] & YY_TRAILING_HEAD_MASK) {
line_warning (_
("dangerous trailing context"),
rule_linenum[ar]);
return;
}
}
}
}









void dump_associated_rules (FILE *file, int ds)
{
int i, j;
int num_associated_rules = 0;
int rule_set[MAX_ASSOC_RULES + 1];
int *dset = dss[ds];
int size = dfasiz[ds];

for (i = 1; i <= size; ++i) {
int rule_num = rule_linenum[assoc_rule[dset[i]]];

for (j = 1; j <= num_associated_rules; ++j)
if (rule_num == rule_set[j])
break;

if (j > num_associated_rules) {
if (num_associated_rules < MAX_ASSOC_RULES)
rule_set[++num_associated_rules] =
rule_num;
}
}

qsort (&rule_set [1], (size_t) num_associated_rules, sizeof (rule_set [1]), intcmp);

fprintf (file, _(" associated rule line numbers:"));

for (i = 1; i <= num_associated_rules; ++i) {
if (i % 8 == 1)
putc ('\n', file);

fprintf (file, "\t%d", rule_set[i]);
}

putc ('\n', file);
}













void dump_transitions (FILE *file, int state[])
{
int i, ec;
int out_char_set[CSIZE];

for (i = 0; i < csize; ++i) {
ec = ABS (ecgroup[i]);
out_char_set[i] = state[ec];
}

fprintf (file, _(" out-transitions: "));

list_character_set (file, out_char_set);


for (i = 0; i < csize; ++i)
out_char_set[i] = !out_char_set[i];

fprintf (file, _("\n jam-transitions: EOF "));

list_character_set (file, out_char_set);

putc ('\n', file);
}






















int *epsclosure (int *t, int *ns_addr, int accset[], int *nacc_addr, int *hv_addr)
{
int stkpos, ns, tsp;
int numstates = *ns_addr, nacc, hashval, transsym, nfaccnum;
int stkend, nstate;
static int did_stk_init = false, *stk;

#define MARK_STATE(state) do{ trans1[state] = trans1[state] - MARKER_DIFFERENCE;} while(0)


#define IS_MARKED(state) (trans1[state] < 0)

#define UNMARK_STATE(state) do{ trans1[state] = trans1[state] + MARKER_DIFFERENCE;} while(0)


#define CHECK_ACCEPT(state) do{ nfaccnum = accptnum[state]; if ( nfaccnum != NIL ) accset[++nacc] = nfaccnum; }while(0)






#define DO_REALLOCATION() do { current_max_dfa_size += MAX_DFA_SIZE_INCREMENT; ++num_reallocs; t = reallocate_integer_array( t, current_max_dfa_size ); stk = reallocate_integer_array( stk, current_max_dfa_size ); }while(0)







#define PUT_ON_STACK(state) do { if ( ++stkend >= current_max_dfa_size ) DO_REALLOCATION(); stk[stkend] = state; MARK_STATE(state); }while(0)







#define ADD_STATE(state) do { if ( ++numstates >= current_max_dfa_size ) DO_REALLOCATION(); t[numstates] = state; hashval += state; }while(0)







#define STACK_STATE(state) do { PUT_ON_STACK(state); CHECK_ACCEPT(state); if ( nfaccnum != NIL || transchar[state] != SYM_EPSILON ) ADD_STATE(state); }while(0)








if (!did_stk_init) {
stk = allocate_integer_array (current_max_dfa_size);
did_stk_init = true;
}

nacc = stkend = hashval = 0;

for (nstate = 1; nstate <= numstates; ++nstate) {
ns = t[nstate];




if (!IS_MARKED (ns)) {
PUT_ON_STACK (ns);
CHECK_ACCEPT (ns);
hashval += ns;
}
}

for (stkpos = 1; stkpos <= stkend; ++stkpos) {
ns = stk[stkpos];
transsym = transchar[ns];

if (transsym == SYM_EPSILON) {
tsp = trans1[ns] + MARKER_DIFFERENCE;

if (tsp != NO_TRANSITION) {
if (!IS_MARKED (tsp))
STACK_STATE (tsp);

tsp = trans2[ns];

if (tsp != NO_TRANSITION
&& !IS_MARKED (tsp))
STACK_STATE (tsp);
}
}
}



for (stkpos = 1; stkpos <= stkend; ++stkpos) {
if (IS_MARKED (stk[stkpos]))
UNMARK_STATE (stk[stkpos]);
else
flexfatal (_
("consistency check failed in epsclosure()"));
}

*ns_addr = numstates;
*hv_addr = hashval;
*nacc_addr = nacc;

return t;
}




void increase_max_dfas (void)
{
current_max_dfas += MAX_DFAS_INCREMENT;

++num_reallocs;

base = reallocate_integer_array (base, current_max_dfas);
def = reallocate_integer_array (def, current_max_dfas);
dfasiz = reallocate_integer_array (dfasiz, current_max_dfas);
accsiz = reallocate_integer_array (accsiz, current_max_dfas);
dhash = reallocate_integer_array (dhash, current_max_dfas);
dss = reallocate_int_ptr_array (dss, current_max_dfas);
dfaacc = reallocate_dfaacc_union (dfaacc, current_max_dfas);

if (nultrans)
nultrans =
reallocate_integer_array (nultrans,
current_max_dfas);
}








void ntod (void)
{
int *accset, ds, nacc, newds;
int sym, hashval, numstates, dsize;
int num_full_table_rows=0;
int *nset, *dset;
int targptr, totaltrans, i, comstate, comfreq, targ;
int symlist[CSIZE + 1];
int num_start_states;
int todo_head, todo_next;

struct yytbl_data *yynxt_tbl = 0;
flex_int32_t *yynxt_data = 0, yynxt_curr = 0;








int duplist[CSIZE + 1], state[CSIZE + 1];
int targfreq[CSIZE + 1] = {0}, targstate[CSIZE + 1];




accset = allocate_integer_array ((num_rules + 1) * 2);
nset = allocate_integer_array (current_max_dfa_size);







todo_head = todo_next = 0;

for (i = 0; i <= csize; ++i) {
duplist[i] = NIL;
symlist[i] = false;
}

for (i = 0; i <= num_rules; ++i)
accset[i] = NIL;

if (trace) {
dumpnfa (scset[1]);
fputs (_("\n\nDFA Dump:\n\n"), stderr);
}

inittbl ();

































if (!fullspd && ecgroup[0] == numecs) {



int use_NUL_table = (numecs == csize);

if (fulltbl && !use_NUL_table) {



if (numecs <= csize && is_power_of_2(numecs)) {
use_NUL_table = true;
}
}

if (use_NUL_table)
nultrans =
allocate_integer_array (current_max_dfas);




}


if (fullspd) {
for (i = 0; i <= numecs; ++i)
state[i] = 0;

place_state (state, 0, 0);
dfaacc[0].dfaacc_state = 0;
}

else if (fulltbl) {
if (nultrans)



num_full_table_rows = numecs;

else




num_full_table_rows = numecs + 1;







yynxt_tbl = calloc(1, sizeof (struct yytbl_data));

yytbl_data_init (yynxt_tbl, YYTD_ID_NXT);
yynxt_tbl->td_hilen = 1;
yynxt_tbl->td_lolen = (flex_uint32_t) num_full_table_rows;
yynxt_tbl->td_data = yynxt_data =
calloc(yynxt_tbl->td_lolen *
yynxt_tbl->td_hilen,
sizeof (flex_int32_t));
yynxt_curr = 0;

buf_prints (&yydmap_buf,
"\t{YYTD_ID_NXT, (void**)&yy_nxt, sizeof(%s)},\n",
long_align ? "flex_int32_t" : "flex_int16_t");




if (gentables)
out_str_dec
("static const %s yy_nxt[][%d] =\n {\n",
long_align ? "flex_int32_t" : "flex_int16_t",
num_full_table_rows);
else {
out_dec ("#undef YY_NXT_LOLEN\n#define YY_NXT_LOLEN (%d)\n", num_full_table_rows);
out_str ("static const %s *yy_nxt =0;\n",
long_align ? "flex_int32_t" : "flex_int16_t");
}


if (gentables)
outn (" {");


for (i = 0; i < num_full_table_rows; ++i) {
mk2data (0);
yynxt_data[yynxt_curr++] = 0;
}

dataflush ();
if (gentables)
outn (" },\n");
}



num_start_states = lastsc * 2;

for (i = 1; i <= num_start_states; ++i) {
numstates = 1;





if (i % 2 == 1)
nset[numstates] = scset[(i / 2) + 1];
else
nset[numstates] =
mkbranch (scbol[i / 2], scset[i / 2]);

nset = epsclosure (nset, &numstates, accset, &nacc,
&hashval);

if (snstods (nset, numstates, accset, nacc, hashval, &ds)) {
numas += nacc;
totnst += numstates;
++todo_next;

if (variable_trailing_context_rules && nacc > 0)
check_trailing_context (nset, numstates,
accset, nacc);
}
}

if (!fullspd) {
if (!snstods (nset, 0, accset, 0, 0, &end_of_buffer_state))
flexfatal (_
("could not create unique end-of-buffer state"));

++numas;
++num_start_states;
++todo_next;
}


while (todo_head < todo_next) {
targptr = 0;
totaltrans = 0;

for (i = 1; i <= numecs; ++i)
state[i] = 0;

ds = ++todo_head;

dset = dss[ds];
dsize = dfasiz[ds];

if (trace)
fprintf (stderr, _("state #%d:\n"), ds);

sympartition (dset, dsize, symlist, duplist);

for (sym = 1; sym <= numecs; ++sym) {
if (symlist[sym]) {
symlist[sym] = 0;

if (duplist[sym] == NIL) {

numstates =
symfollowset (dset, dsize,
sym, nset);
nset = epsclosure (nset,
&numstates,
accset, &nacc,
&hashval);

if (snstods
(nset, numstates, accset, nacc,
hashval, &newds)) {
totnst = totnst +
numstates;
++todo_next;
numas += nacc;

if (variable_trailing_context_rules && nacc > 0)
check_trailing_context
(nset,
numstates,
accset,
nacc);
}

state[sym] = newds;

if (trace)
fprintf (stderr,
"\t%d\t%d\n", sym,
newds);

targfreq[++targptr] = 1;
targstate[targptr] = newds;
++numuniq;
}

else {




targ = state[duplist[sym]];
state[sym] = targ;

if (trace)
fprintf (stderr,
"\t%d\t%d\n", sym,
targ);





i = 0;
while (targstate[++i] != targ) ;

++targfreq[i];
++numdup;
}

++totaltrans;
duplist[sym] = NIL;
}
}


numsnpairs += totaltrans;

if (ds > num_start_states)
check_for_backing_up (ds, state);

if (nultrans) {
nultrans[ds] = state[NUL_ec];
state[NUL_ec] = 0;
}

if (fulltbl) {


yynxt_tbl->td_hilen++;
yynxt_tbl->td_data = yynxt_data =
realloc (yynxt_data,
yynxt_tbl->td_hilen *
yynxt_tbl->td_lolen *
sizeof (flex_int32_t));


if (gentables)
outn (" {");


if (ds == end_of_buffer_state) {
mk2data (-end_of_buffer_state);
yynxt_data[yynxt_curr++] =
-end_of_buffer_state;
}
else {
mk2data (end_of_buffer_state);
yynxt_data[yynxt_curr++] =
end_of_buffer_state;
}

for (i = 1; i < num_full_table_rows; ++i) {



mk2data (state[i] ? state[i] : -ds);
yynxt_data[yynxt_curr++] =
state[i] ? state[i] : -ds;
}

dataflush ();
if (gentables)
outn (" },\n");
}

else if (fullspd)
place_state (state, ds, totaltrans);

else if (ds == end_of_buffer_state)



stack1 (ds, 0, 0, JAMSTATE);

else {





comfreq = 0;
comstate = 0;

for (i = 1; i <= targptr; ++i)
if (targfreq[i] > comfreq) {
comfreq = targfreq[i];
comstate = targstate[i];
}

bldtbl (state, ds, totaltrans, comstate, comfreq);
}
}

if (fulltbl) {
dataend ();
if (tablesext) {
yytbl_data_compress (yynxt_tbl);
if (yytbl_data_fwrite (&tableswr, yynxt_tbl) < 0)
flexerror (_
("Could not write yynxt_tbl[][]"));
}
if (yynxt_tbl) {
yytbl_data_destroy (yynxt_tbl);
yynxt_tbl = 0;
}
}

else if (!fullspd) {
cmptmps ();




while (onesp > 0) {
mk1tbl (onestate[onesp], onesym[onesp],
onenext[onesp], onedef[onesp]);
--onesp;
}

mkdeftbl ();
}

free(accset);
free(nset);
}












int snstods (int sns[], int numstates, int accset[], int nacc, int hashval, int *newds_addr)
{
int didsort = 0;
int i, j;
int newds, *oldsns;

for (i = 1; i <= lastdfa; ++i)
if (hashval == dhash[i]) {
if (numstates == dfasiz[i]) {
oldsns = dss[i];

if (!didsort) {



qsort (&sns [1], (size_t) numstates, sizeof (sns [1]), intcmp);
didsort = 1;
}

for (j = 1; j <= numstates; ++j)
if (sns[j] != oldsns[j])
break;

if (j > numstates) {
++dfaeql;
*newds_addr = i;
return 0;
}

++hshcol;
}

else
++hshsave;
}



if (++lastdfa >= current_max_dfas)
increase_max_dfas ();

newds = lastdfa;

dss[newds] = allocate_integer_array (numstates + 1);





if (!didsort)
qsort (&sns [1], (size_t) numstates, sizeof (sns [1]), intcmp);

for (i = 1; i <= numstates; ++i)
dss[newds][i] = sns[i];

dfasiz[newds] = numstates;
dhash[newds] = hashval;

if (nacc == 0) {
if (reject)
dfaacc[newds].dfaacc_set = NULL;
else
dfaacc[newds].dfaacc_state = 0;

accsiz[newds] = 0;
}

else if (reject) {





qsort (&accset [1], (size_t) nacc, sizeof (accset [1]), intcmp);

dfaacc[newds].dfaacc_set =
allocate_integer_array (nacc + 1);


for (i = 1; i <= nacc; ++i) {
dfaacc[newds].dfaacc_set[i] = accset[i];

if (accset[i] <= num_rules)



rule_useful[accset[i]] = true;
}

accsiz[newds] = nacc;
}

else {



j = num_rules + 1;

for (i = 1; i <= nacc; ++i)
if (accset[i] < j)
j = accset[i];

dfaacc[newds].dfaacc_state = j;

if (j <= num_rules)
rule_useful[j] = true;
}

*newds_addr = newds;

return 1;
}









int symfollowset (int ds[], int dsize, int transsym, int nset[])
{
int ns, tsp, sym, i, j, lenccl, ch, numstates, ccllist;

numstates = 0;

for (i = 1; i <= dsize; ++i) {
ns = ds[i];
sym = transchar[ns];
tsp = trans1[ns];

if (sym < 0) {
sym = -sym;
ccllist = cclmap[sym];
lenccl = ccllen[sym];

if (cclng[sym]) {
for (j = 0; j < lenccl; ++j) {



ch = ccltbl[ccllist + j];

if (ch == 0)
ch = NUL_ec;

if (ch > transsym)



break;

else if (ch == transsym)

goto bottom;
}


nset[++numstates] = tsp;
}

else
for (j = 0; j < lenccl; ++j) {
ch = ccltbl[ccllist + j];

if (ch == 0)
ch = NUL_ec;

if (ch > transsym)
break;
else if (ch == transsym) {
nset[++numstates] = tsp;
break;
}
}
}

else if (sym == SYM_EPSILON) {
}

else if (ABS (ecgroup[sym]) == transsym)
nset[++numstates] = tsp;

bottom:;
}

return numstates;
}









void sympartition (int ds[], int numstates, int symlist[], int duplist[])
{
int tch, i, j, k, ns, dupfwd[CSIZE + 1], lenccl, cclp, ich;






for (i = 1; i <= numecs; ++i) {
duplist[i] = i - 1;
dupfwd[i] = i + 1;
}

duplist[1] = NIL;
dupfwd[numecs] = NIL;

for (i = 1; i <= numstates; ++i) {
ns = ds[i];
tch = transchar[ns];

if (tch != SYM_EPSILON) {
if (tch < -lastccl || tch >= csize) {
flexfatal (_
("bad transition character detected in sympartition()"));
}

if (tch >= 0) {
int ec = ecgroup[tch];

mkechar (ec, dupfwd, duplist);
symlist[ec] = 1;
}

else {
tch = -tch;

lenccl = ccllen[tch];
cclp = cclmap[tch];
mkeccl (ccltbl + cclp, lenccl, dupfwd,
duplist, numecs, NUL_ec);

if (cclng[tch]) {
j = 0;

for (k = 0; k < lenccl; ++k) {
ich = ccltbl[cclp + k];

if (ich == 0)
ich = NUL_ec;

for (++j; j < ich; ++j)
symlist[j] = 1;
}

for (++j; j <= numecs; ++j)
symlist[j] = 1;
}

else
for (k = 0; k < lenccl; ++k) {
ich = ccltbl[cclp + k];

if (ich == 0)
ich = NUL_ec;

symlist[ich] = 1;
}
}
}
}
}
