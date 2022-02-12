





















#include "diff.h"

static void print_normal_hunk (struct change *);




void
print_normal_script (struct change *script)
{
print_script (script, find_change, print_normal_hunk);
}





static void
print_normal_hunk (struct change *hunk)
{
lin first0, last0, first1, last1;
register lin i;


enum changes changes = analyze_hunk (hunk, &first0, &last0, &first1, &last1);
if (!changes)
return;

begin_output ();


print_number_range (',', &files[0], first0, last0);
fprintf (outfile, "%c", change_letter[changes]);
print_number_range (',', &files[1], first1, last1);
fprintf (outfile, "\n");


if (changes & OLD)
for (i = first0; i <= last0; i++)
print_1_line ("<", &files[0].linbuf[i]);

if (changes == CHANGED)
fprintf (outfile, "---\n");


if (changes & NEW)
for (i = first1; i <= last1; i++)
print_1_line (">", &files[1].linbuf[i]);
}
