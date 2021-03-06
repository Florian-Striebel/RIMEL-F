





















#include "diff.h"

#include <xalloc.h>

struct group
{
struct file_data const *file;
lin from, upto;
};

static char const *format_group (FILE *, char const *, char,
struct group const *);
static char const *do_printf_spec (FILE *, char const *,
struct file_data const *, lin,
struct group const *);
static char const *scan_char_literal (char const *, char *);
static lin groups_letter_value (struct group const *, char);
static void format_ifdef (char const *, lin, lin, lin, lin);
static void print_ifdef_hunk (struct change *);
static void print_ifdef_lines (FILE *, char const *, struct group const *);

static lin next_line0;
static lin next_line1;



void
print_ifdef_script (struct change *script)
{
next_line0 = next_line1 = - files[0].prefix_lines;
print_script (script, find_change, print_ifdef_hunk);
if (next_line0 < files[0].valid_lines
|| next_line1 < files[1].valid_lines)
{
begin_output ();
format_ifdef (group_format[UNCHANGED],
next_line0, files[0].valid_lines,
next_line1, files[1].valid_lines);
}
}





static void
print_ifdef_hunk (struct change *hunk)
{
lin first0, last0, first1, last1;


enum changes changes = analyze_hunk (hunk, &first0, &last0, &first1, &last1);
if (!changes)
return;

begin_output ();


if (next_line0 < first0 || next_line1 < first1)
format_ifdef (group_format[UNCHANGED],
next_line0, first0,
next_line1, first1);


next_line0 = last0 + 1;
next_line1 = last1 + 1;
format_ifdef (group_format[changes],
first0, next_line0,
first1, next_line1);
}





static void
format_ifdef (char const *format, lin beg0, lin end0, lin beg1, lin end1)
{
struct group groups[2];

groups[0].file = &files[0];
groups[0].from = beg0;
groups[0].upto = end0;
groups[1].file = &files[1];
groups[1].from = beg1;
groups[1].upto = end1;
format_group (outfile, format, 0, groups);
}







static char const *
format_group (register FILE *out, char const *format, char endchar,
struct group const *groups)
{
register char c;
register char const *f = format;

while ((c = *f) != endchar && c != 0)
{
char const *f1 = ++f;
if (c == '%')
switch ((c = *f++))
{
case '%':
break;

case '(':

{
int i;
uintmax_t value[2];
FILE *thenout, *elseout;

for (i = 0; i < 2; i++)
{
if (ISDIGIT (*f))
{
char *fend;
errno = 0;
value[i] = strtoumax (f, &fend, 10);
if (errno)
goto bad_format;
f = fend;
}
else
{
value[i] = groups_letter_value (groups, *f);
if (value[i] == -1)
goto bad_format;
f++;
}
if (*f++ != "=?"[i])
goto bad_format;
}
if (value[0] == value[1])
thenout = out, elseout = 0;
else
thenout = 0, elseout = out;
f = format_group (thenout, f, ':', groups);
if (*f)
{
f = format_group (elseout, f + 1, ')', groups);
if (*f)
f++;
}
}
continue;

case '<':

print_ifdef_lines (out, line_format[OLD], &groups[0]);
continue;

case '=':

print_ifdef_lines (out, line_format[UNCHANGED], &groups[0]);
continue;

case '>':

print_ifdef_lines (out, line_format[NEW], &groups[1]);
continue;

default:
f = do_printf_spec (out, f - 2, 0, 0, groups);
if (f)
continue;

bad_format:
c = '%';
f = f1;
break;
}

if (out)
putc (c, out);
}

return f;
}



static lin
groups_letter_value (struct group const *g, char letter)
{
switch (letter)
{
case 'E': letter = 'e'; g++; break;
case 'F': letter = 'f'; g++; break;
case 'L': letter = 'l'; g++; break;
case 'M': letter = 'm'; g++; break;
case 'N': letter = 'n'; g++; break;
}

switch (letter)
{
case 'e': return translate_line_number (g->file, g->from) - 1;
case 'f': return translate_line_number (g->file, g->from);
case 'l': return translate_line_number (g->file, g->upto) - 1;
case 'm': return translate_line_number (g->file, g->upto);
case 'n': return g->upto - g->from;
default: return -1;
}
}



static void
print_ifdef_lines (register FILE *out, char const *format,
struct group const *group)
{
struct file_data const *file = group->file;
char const * const *linbuf = file->linbuf;
lin from = group->from, upto = group->upto;

if (!out)
return;


if (!expand_tabs && format[0] == '%')
{
if (format[1] == 'l' && format[2] == '\n' && !format[3] && from < upto)
{
fwrite (linbuf[from], sizeof (char),
linbuf[upto] + (linbuf[upto][-1] != '\n') - linbuf[from],
out);
return;
}
if (format[1] == 'L' && !format[2])
{
fwrite (linbuf[from], sizeof (char),
linbuf[upto] - linbuf[from], out);
return;
}
}

for (; from < upto; from++)
{
register char c;
register char const *f = format;

while ((c = *f++) != 0)
{
char const *f1 = f;
if (c == '%')
switch ((c = *f++))
{
case '%':
break;

case 'l':
output_1_line (linbuf[from],
(linbuf[from + 1]
- (linbuf[from + 1][-1] == '\n')),
0, 0);
continue;

case 'L':
output_1_line (linbuf[from], linbuf[from + 1], 0, 0);
continue;

default:
f = do_printf_spec (out, f - 2, file, from, 0);
if (f)
continue;
c = '%';
f = f1;
break;
}

putc (c, out);
}
}
}

static char const *
do_printf_spec (FILE *out, char const *spec,
struct file_data const *file, lin n,
struct group const *groups)
{
char const *f = spec;
char c;
char c1;



f++;
while ((c = *f++) == '-' || c == '\'' || c == '0')
continue;
while (ISDIGIT (c))
c = *f++;
if (c == '.')
while (ISDIGIT (c = *f++))
continue;
c1 = *f++;

switch (c)
{
case 'c':
if (c1 != '\'')
return 0;
else
{
char value;
f = scan_char_literal (f, &value);
if (!f)
return 0;
if (out)
putc (value, out);
}
break;

case 'd': case 'o': case 'x': case 'X':
{
lin value;

if (file)
{
if (c1 != 'n')
return 0;
value = translate_line_number (file, n);
}
else
{
value = groups_letter_value (groups, c1);
if (value < 0)
return 0;
}

if (out)
{


long int long_value = value;
size_t spec_prefix_len = f - spec - 2;
#if HAVE_C_VARARRAYS
char format[spec_prefix_len + 3];
#else
char *format = xmalloc (spec_prefix_len + 3);
#endif
char *p = format + spec_prefix_len;
memcpy (format, spec, spec_prefix_len);
*p++ = 'l';
*p++ = c;
*p = '\0';
fprintf (out, format, long_value);
#if ! HAVE_C_VARARRAYS
free (format);
#endif
}
}
break;

default:
return 0;
}

return f;
}





static char const *
scan_char_literal (char const *lit, char *valptr)
{
register char const *p = lit;
char value;
ptrdiff_t digits;
char c = *p++;

switch (c)
{
case 0:
case '\'':
return 0;

case '\\':
value = 0;
while ((c = *p++) != '\'')
{
unsigned int digit = c - '0';
if (8 <= digit)
return 0;
value = 8 * value + digit;
}
digits = p - lit - 2;
if (! (1 <= digits && digits <= 3))
return 0;
break;

default:
value = c;
if (*p++ != '\'')
return 0;
break;
}

*valptr = value;
return p;
}
