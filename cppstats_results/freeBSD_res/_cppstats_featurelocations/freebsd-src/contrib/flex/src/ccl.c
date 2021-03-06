
































#include "flexdef.h"


static bool
ccl_contains (const int cclp, const int ch)
{
int ind, len, i;

len = ccllen[cclp];
ind = cclmap[cclp];

for (i = 0; i < len; ++i)
if (ccltbl[ind + i] == ch)
return !cclng[cclp];

return cclng[cclp];
}




void ccladd (int cclp, int ch)
{
int ind, len, newpos, i;

check_char (ch);

len = ccllen[cclp];
ind = cclmap[cclp];



for (i = 0; i < len; ++i)
if (ccltbl[ind + i] == ch)
return;


if (ch == nlch)
ccl_has_nl[cclp] = true;

newpos = ind + len;

if (newpos >= current_max_ccl_tbl_size) {
current_max_ccl_tbl_size += MAX_CCL_TBL_SIZE_INCREMENT;

++num_reallocs;

ccltbl = reallocate_Character_array (ccltbl,
current_max_ccl_tbl_size);
}

ccllen[cclp] = len + 1;
ccltbl[newpos] = (unsigned char) ch;
}



static void dump_cclp (FILE* file, int cclp)
{
int i;

putc ('[', file);

for (i = 0; i < csize; ++i) {
if (ccl_contains(cclp, i)){
int start_char = i;

putc (' ', file);

fputs (readable_form (i), file);

while (++i < csize && ccl_contains(cclp,i)) ;

if (i - 1 > start_char)

fprintf (file, "-%s",
readable_form (i - 1));

putc (' ', file);
}
}

putc (']', file);
}




int
ccl_set_diff (int a, int b)
{
int d, ch;


d = cclinit();





for ( ch = 0; ch < csize; ++ch )
if (ccl_contains (a, ch) && !ccl_contains(b, ch))
ccladd (d, ch);


if (0){
fprintf(stderr, "ccl_set_diff (");
fprintf(stderr, "\n ");
dump_cclp (stderr, a);
fprintf(stderr, "\n ");
dump_cclp (stderr, b);
fprintf(stderr, "\n ");
dump_cclp (stderr, d);
fprintf(stderr, "\n)\n");
}
return d;
}


int
ccl_set_union (int a, int b)
{
int d, i;


d = cclinit();


for (i = 0; i < ccllen[a]; ++i)
ccladd (d, ccltbl[cclmap[a] + i]);


for (i = 0; i < ccllen[b]; ++i)
ccladd (d, ccltbl[cclmap[b] + i]);


if (0){
fprintf(stderr, "ccl_set_union (%d + %d = %d", a, b, d);
fprintf(stderr, "\n ");
dump_cclp (stderr, a);
fprintf(stderr, "\n ");
dump_cclp (stderr, b);
fprintf(stderr, "\n ");
dump_cclp (stderr, d);
fprintf(stderr, "\n)\n");
}
return d;
}




int cclinit (void)
{
if (++lastccl >= current_maxccls) {
current_maxccls += MAX_CCLS_INCREMENT;

++num_reallocs;

cclmap =
reallocate_integer_array (cclmap, current_maxccls);
ccllen =
reallocate_integer_array (ccllen, current_maxccls);
cclng = reallocate_integer_array (cclng, current_maxccls);
ccl_has_nl =
reallocate_bool_array (ccl_has_nl,
current_maxccls);
}

if (lastccl == 1)

cclmap[lastccl] = 0;

else





cclmap[lastccl] =
cclmap[lastccl - 1] + ccllen[lastccl - 1];

ccllen[lastccl] = 0;
cclng[lastccl] = 0;
ccl_has_nl[lastccl] = false;

return lastccl;
}




void cclnegate (int cclp)
{
cclng[cclp] = 1;
ccl_has_nl[cclp] = !ccl_has_nl[cclp];
}









void list_character_set (FILE *file, int cset[])
{
int i;

putc ('[', file);

for (i = 0; i < csize; ++i) {
if (cset[i]) {
int start_char = i;

putc (' ', file);

fputs (readable_form (i), file);

while (++i < csize && cset[i]) ;

if (i - 1 > start_char)

fprintf (file, "-%s",
readable_form (i - 1));

putc (' ', file);
}
}

putc (']', file);
}












bool range_covers_case (int c1, int c2)
{
int i, o;

for (i = c1; i <= c2; i++) {
if (has_case (i)) {
o = reverse_case (i);
if (o < c1 || c2 < o)
return false;
}
}
return true;
}




int reverse_case (int c)
{
return isupper (c) ? tolower (c) : (islower (c) ? toupper (c) : c);
}


bool has_case (int c)
{
return (isupper (c) || islower (c)) ? true : false;
}
