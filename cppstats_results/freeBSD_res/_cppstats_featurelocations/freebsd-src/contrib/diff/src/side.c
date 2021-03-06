





















#include "diff.h"

static void print_sdiff_common_lines (lin, lin);
static void print_sdiff_hunk (struct change *);


static lin next0, next1;



void
print_sdiff_script (struct change *script)
{
begin_output ();

next0 = next1 = - files[0].prefix_lines;
print_script (script, find_change, print_sdiff_hunk);

print_sdiff_common_lines (files[0].valid_lines, files[1].valid_lines);
}



static size_t
tab_from_to (size_t from, size_t to)
{
FILE *out = outfile;
size_t tab;
size_t tab_size = tabsize;

if (!expand_tabs)
for (tab = from + tab_size - from % tab_size; tab <= to; tab += tab_size)
{
putc ('\t', out);
from = tab;
}
while (from++ < to)
putc (' ', out);
return to;
}





static size_t
print_half_line (char const *const *line, size_t indent, size_t out_bound)
{
FILE *out = outfile;
register size_t in_position = 0;
register size_t out_position = 0;
register char const *text_pointer = line[0];
register char const *text_limit = line[1];

while (text_pointer < text_limit)
{
register unsigned char c = *text_pointer++;

switch (c)
{
case '\t':
{
size_t spaces = tabsize - in_position % tabsize;
if (in_position == out_position)
{
size_t tabstop = out_position + spaces;
if (expand_tabs)
{
if (out_bound < tabstop)
tabstop = out_bound;
for (; out_position < tabstop; out_position++)
putc (' ', out);
}
else
if (tabstop < out_bound)
{
out_position = tabstop;
putc (c, out);
}
}
in_position += spaces;
}
break;

case '\r':
{
putc (c, out);
tab_from_to (0, indent);
in_position = out_position = 0;
}
break;

case '\b':
if (in_position != 0 && --in_position < out_bound)
{
if (out_position <= in_position)

for (; out_position < in_position; out_position++)
putc (' ', out);
else
{
out_position = in_position;
putc (c, out);
}
}
break;

case '\f':
case '\v':
control_char:
if (in_position < out_bound)
putc (c, out);
break;

default:
if (! isprint (c))
goto control_char;

case ' ':
if (in_position++ < out_bound)
{
out_position = in_position;
putc (c, out);
}
break;

case '\n':
return out_position;
}
}

return out_position;
}





static void
print_1sdiff_line (char const *const *left, char sep,
char const *const *right)
{
FILE *out = outfile;
size_t hw = sdiff_half_width;
size_t c2o = sdiff_column2_offset;
size_t col = 0;
bool put_newline = false;

if (left)
{
put_newline |= left[1][-1] == '\n';
col = print_half_line (left, 0, hw);
}

if (sep != ' ')
{
col = tab_from_to (col, (hw + c2o - 1) / 2) + 1;
if (sep == '|' && put_newline != (right[1][-1] == '\n'))
sep = put_newline ? '/' : '\\';
putc (sep, out);
}

if (right)
{
put_newline |= right[1][-1] == '\n';
if (**right != '\n')
{
col = tab_from_to (col, c2o);
print_half_line (right, col, hw);
}
}

if (put_newline)
putc ('\n', out);
}


static void
print_sdiff_common_lines (lin limit0, lin limit1)
{
lin i0 = next0, i1 = next1;

if (!suppress_common_lines && (i0 != limit0 || i1 != limit1))
{
if (sdiff_merge_assist)
{
long int len0 = limit0 - i0;
long int len1 = limit1 - i1;
fprintf (outfile, "i%ld,%ld\n", len0, len1);
}

if (!left_column)
{
while (i0 != limit0 && i1 != limit1)
print_1sdiff_line (&files[0].linbuf[i0++], ' ',
&files[1].linbuf[i1++]);
while (i1 != limit1)
print_1sdiff_line (0, ')', &files[1].linbuf[i1++]);
}
while (i0 != limit0)
print_1sdiff_line (&files[0].linbuf[i0++], '(', 0);
}

next0 = limit0;
next1 = limit1;
}





static void
print_sdiff_hunk (struct change *hunk)
{
lin first0, last0, first1, last1;
register lin i, j;


enum changes changes =
analyze_hunk (hunk, &first0, &last0, &first1, &last1);
if (!changes)
return;


print_sdiff_common_lines (first0, first1);

if (sdiff_merge_assist)
{
long int len0 = last0 - first0 + 1;
long int len1 = last1 - first1 + 1;
fprintf (outfile, "c%ld,%ld\n", len0, len1);
}


if (changes == CHANGED)
{
for (i = first0, j = first1; i <= last0 && j <= last1; i++, j++)
print_1sdiff_line (&files[0].linbuf[i], '|', &files[1].linbuf[j]);
changes = (i <= last0 ? OLD : 0) + (j <= last1 ? NEW : 0);
next0 = first0 = i;
next1 = first1 = j;
}


if (changes & NEW)
{
for (j = first1; j <= last1; ++j)
print_1sdiff_line (0, '>', &files[1].linbuf[j]);
next1 = j;
}


if (changes & OLD)
{
for (i = first0; i <= last0; ++i)
print_1sdiff_line (&files[0].linbuf[i], '<', 0);
next0 = i;
}
}
