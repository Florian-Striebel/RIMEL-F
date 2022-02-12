
































#include "flexdef.h"




int dupmachine(int);
void mkxtion(int, int);







void add_accept (int mach, int accepting_number)
{






if (transchar[finalst[mach]] == SYM_EPSILON)
accptnum[finalst[mach]] = accepting_number;

else {
int astate = mkstate (SYM_EPSILON);

accptnum[astate] = accepting_number;
(void) link_machines (mach, astate);
}
}













int copysingl (int singl, int num)
{
int copy, i;

copy = mkstate (SYM_EPSILON);

for (i = 1; i <= num; ++i)
copy = link_machines (copy, dupmachine (singl));

return copy;
}




void dumpnfa (int state1)
{
int sym, tsp1, tsp2, anum, ns;

fprintf (stderr,
_
("\n\n********** beginning dump of nfa with start state %d\n"),
state1);








for (ns = 1; ns <= lastnfa; ++ns) {
fprintf (stderr, _("state #%4d\t"), ns);

sym = transchar[ns];
tsp1 = trans1[ns];
tsp2 = trans2[ns];
anum = accptnum[ns];

fprintf (stderr, "%3d: %4d, %4d", sym, tsp1, tsp2);

if (anum != NIL)
fprintf (stderr, " [%d]", anum);

fprintf (stderr, "\n");
}

fprintf (stderr, _("********** end of dump\n"));
}



















int dupmachine (int mach)
{
int i, init, state_offset;
int state = 0;
int last = lastst[mach];

for (i = firstst[mach]; i <= last; ++i) {
state = mkstate (transchar[i]);

if (trans1[i] != NO_TRANSITION) {
mkxtion (finalst[state], trans1[i] + state - i);

if (transchar[i] == SYM_EPSILON &&
trans2[i] != NO_TRANSITION)
mkxtion (finalst[state],
trans2[i] + state - i);
}

accptnum[state] = accptnum[i];
}

if (state == 0)
flexfatal (_("empty machine in dupmachine()"));

state_offset = state - i + 1;

init = mach + state_offset;
firstst[init] = firstst[mach] + state_offset;
finalst[init] = finalst[mach] + state_offset;
lastst[init] = lastst[mach] + state_offset;

return init;
}














void finish_rule (int mach, int variable_trail_rule, int headcnt, int trailcnt,
int pcont_act)
{
char action_text[MAXLINE];

add_accept (mach, num_rules);




rule_linenum[num_rules] = linenum;




if (continued_action)
--rule_linenum[num_rules];





if (pcont_act && rule_has_nl[num_rules - 1])
rule_has_nl[num_rules] = true;

snprintf (action_text, sizeof(action_text), "case %d:\n", num_rules);
add_action (action_text);
if (rule_has_nl[num_rules]) {
snprintf (action_text, sizeof(action_text), "/* rule %d can match eol */\n",
num_rules);
add_action (action_text);
}


if (variable_trail_rule) {
rule_type[num_rules] = RULE_VARIABLE;

if (performance_report > 0)
fprintf (stderr,
_
("Variable trailing context rule at line %d\n"),
rule_linenum[num_rules]);

variable_trailing_context_rules = true;
}

else {
rule_type[num_rules] = RULE_NORMAL;

if (headcnt > 0 || trailcnt > 0) {



char *scanner_cp = "YY_G(yy_c_buf_p) = yy_cp";
char *scanner_bp = "yy_bp";

add_action
("*yy_cp = YY_G(yy_hold_char); /* undo effects of setting up yytext */\n");

if (headcnt > 0) {
if (rule_has_nl[num_rules]) {
snprintf (action_text, sizeof(action_text),
"YY_LINENO_REWIND_TO(%s + %d);\n", scanner_bp, headcnt);
add_action (action_text);
}
snprintf (action_text, sizeof(action_text), "%s = %s + %d;\n",
scanner_cp, scanner_bp, headcnt);
add_action (action_text);
}

else {
if (rule_has_nl[num_rules]) {
snprintf (action_text, sizeof(action_text),
"YY_LINENO_REWIND_TO(yy_cp - %d);\n", trailcnt);
add_action (action_text);
}

snprintf (action_text, sizeof(action_text), "%s -= %d;\n",
scanner_cp, trailcnt);
add_action (action_text);
}

add_action
("YY_DO_BEFORE_ACTION; /* set up yytext again */\n");
}
}






if (!continued_action)
add_action ("YY_RULE_SETUP\n");

line_directive_out(NULL, 1);
add_action("[[");
}


















int link_machines (int first, int last)
{
if (first == NIL)
return last;

else if (last == NIL)
return first;

else {
mkxtion (finalst[first], last);
finalst[first] = finalst[last];
lastst[first] = MAX (lastst[first], lastst[last]);
firstst[first] = MIN (firstst[first], firstst[last]);

return first;
}
}









void mark_beginning_as_normal (int mach)
{
switch (state_type[mach]) {
case STATE_NORMAL:

return;

case STATE_TRAILING_CONTEXT:
state_type[mach] = STATE_NORMAL;

if (transchar[mach] == SYM_EPSILON) {
if (trans1[mach] != NO_TRANSITION)
mark_beginning_as_normal (trans1[mach]);

if (trans2[mach] != NO_TRANSITION)
mark_beginning_as_normal (trans2[mach]);
}
break;

default:
flexerror (_
("bad state type in mark_beginning_as_normal()"));
break;
}
}
















int mkbranch (int first, int second)
{
int eps;

if (first == NO_TRANSITION)
return second;

else if (second == NO_TRANSITION)
return first;

eps = mkstate (SYM_EPSILON);

mkxtion (eps, first);
mkxtion (eps, second);

return eps;
}










int mkclos (int state)
{
return mkopt (mkposcl (state));
}
















int mkopt (int mach)
{
int eps;

if (!SUPER_FREE_EPSILON (finalst[mach])) {
eps = mkstate (SYM_EPSILON);
mach = link_machines (mach, eps);
}





eps = mkstate (SYM_EPSILON);
mach = link_machines (eps, mach);

mkxtion (mach, finalst[mach]);

return mach;
}
















int mkor (int first, int second)
{
int eps, orend;

if (first == NIL)
return second;

else if (second == NIL)
return first;

else {



eps = mkstate (SYM_EPSILON);

first = link_machines (eps, first);

mkxtion (first, second);

if (SUPER_FREE_EPSILON (finalst[first]) &&
accptnum[finalst[first]] == NIL) {
orend = finalst[first];
mkxtion (finalst[second], orend);
}

else if (SUPER_FREE_EPSILON (finalst[second]) &&
accptnum[finalst[second]] == NIL) {
orend = finalst[second];
mkxtion (finalst[first], orend);
}

else {
eps = mkstate (SYM_EPSILON);

first = link_machines (first, eps);
orend = finalst[first];

mkxtion (finalst[second], orend);
}
}

finalst[first] = orend;
return first;
}










int mkposcl (int state)
{
int eps;

if (SUPER_FREE_EPSILON (finalst[state])) {
mkxtion (finalst[state], state);
return state;
}

else {
eps = mkstate (SYM_EPSILON);
mkxtion (eps, state);
return link_machines (state, eps);
}
}














int mkrep (int mach, int lb, int ub)
{
int base_mach, tail, copy, i;

base_mach = copysingl (mach, lb - 1);

if (ub == INFINITE_REPEAT) {
copy = dupmachine (mach);
mach = link_machines (mach,
link_machines (base_mach,
mkclos (copy)));
}

else {
tail = mkstate (SYM_EPSILON);

for (i = lb; i < ub; ++i) {
copy = dupmachine (mach);
tail = mkopt (link_machines (copy, tail));
}

mach =
link_machines (mach,
link_machines (base_mach, tail));
}

return mach;
}


















int mkstate (int sym)
{
if (++lastnfa >= current_mns) {
if ((current_mns += MNS_INCREMENT) >= maximum_mns)
lerr(_
("input rules are too complicated (>= %d NFA states)"),
current_mns);

++num_reallocs;

firstst = reallocate_integer_array (firstst, current_mns);
lastst = reallocate_integer_array (lastst, current_mns);
finalst = reallocate_integer_array (finalst, current_mns);
transchar =
reallocate_integer_array (transchar, current_mns);
trans1 = reallocate_integer_array (trans1, current_mns);
trans2 = reallocate_integer_array (trans2, current_mns);
accptnum =
reallocate_integer_array (accptnum, current_mns);
assoc_rule =
reallocate_integer_array (assoc_rule, current_mns);
state_type =
reallocate_integer_array (state_type, current_mns);
}

firstst[lastnfa] = lastnfa;
finalst[lastnfa] = lastnfa;
lastst[lastnfa] = lastnfa;
transchar[lastnfa] = sym;
trans1[lastnfa] = NO_TRANSITION;
trans2[lastnfa] = NO_TRANSITION;
accptnum[lastnfa] = NIL;
assoc_rule[lastnfa] = num_rules;
state_type[lastnfa] = current_state_type;










if (sym < 0) {




}

else if (sym == SYM_EPSILON)
++numeps;

else {
check_char (sym);

if (useecs)

mkechar (sym ? sym : csize, nextecm, ecgroup);
}

return lastnfa;
}












void mkxtion (int statefrom, int stateto)
{
if (trans1[statefrom] == NO_TRANSITION)
trans1[statefrom] = stateto;

else if ((transchar[statefrom] != SYM_EPSILON) ||
(trans2[statefrom] != NO_TRANSITION))
flexfatal (_("found too many transitions in mkxtion()"));

else {
++eps2;
trans2[statefrom] = stateto;
}
}



void new_rule (void)
{
if (++num_rules >= current_max_rules) {
++num_reallocs;
current_max_rules += MAX_RULES_INCREMENT;
rule_type = reallocate_integer_array (rule_type,
current_max_rules);
rule_linenum = reallocate_integer_array (rule_linenum,
current_max_rules);
rule_useful = reallocate_integer_array (rule_useful,
current_max_rules);
rule_has_nl = reallocate_bool_array (rule_has_nl,
current_max_rules);
}

if (num_rules > MAX_RULE)
lerr (_("too many rules (> %d)!"), MAX_RULE);

rule_linenum[num_rules] = linenum;
rule_useful[num_rules] = false;
rule_has_nl[num_rules] = false;
}
