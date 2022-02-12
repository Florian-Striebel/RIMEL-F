





















#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdbool.h>

#include <ctype.h>
#include <errno.h>
#if !defined(errno)
extern int errno;
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exclude.h"
#include "fnmatch.h"
#include "unlocked-io.h"
#include "xalloc.h"

#if STDC_HEADERS || (! defined isascii && ! HAVE_ISASCII)
#define IN_CTYPE_DOMAIN(c) true
#else
#define IN_CTYPE_DOMAIN(c) isascii (c)
#endif

static inline bool
is_space (unsigned char c)
{
return IN_CTYPE_DOMAIN (c) && isspace (c);
}


#define verify(name, assertion) struct name { char a[(assertion) ? 1 : -1]; }


#if !defined(FNM_CASEFOLD)
#define FNM_CASEFOLD 0
#endif
#if !defined(FNM_LEADING_DIR)
#define FNM_LEADING_DIR 0
#endif

verify (EXCLUDE_macros_do_not_collide_with_FNM_macros,
(((EXCLUDE_ANCHORED | EXCLUDE_INCLUDE | EXCLUDE_WILDCARDS)
& (FNM_PATHNAME | FNM_NOESCAPE | FNM_PERIOD | FNM_LEADING_DIR
| FNM_CASEFOLD))
== 0));




struct patopts
{
char const *pattern;
int options;
};



struct exclude
{
struct patopts *exclude;
size_t exclude_alloc;
size_t exclude_count;
};



struct exclude *
new_exclude (void)
{
return xzalloc (sizeof *new_exclude ());
}



void
free_exclude (struct exclude *ex)
{
free (ex->exclude);
free (ex);
}




static int
fnmatch_no_wildcards (char const *pattern, char const *f, int options)
{
if (! (options & FNM_LEADING_DIR))
return ((options & FNM_CASEFOLD)
? strcasecmp (pattern, f)
: strcmp (pattern, f));
else
{
size_t patlen = strlen (pattern);
int r = ((options & FNM_CASEFOLD)
? strncasecmp (pattern, f, patlen)
: strncmp (pattern, f, patlen));
if (! r)
{
r = f[patlen];
if (r == '/')
r = 0;
}
return r;
}
}



bool
excluded_filename (struct exclude const *ex, char const *f)
{
size_t exclude_count = ex->exclude_count;


if (exclude_count == 0)
return false;
else
{
struct patopts const *exclude = ex->exclude;
size_t i;


bool excluded = !! (exclude[0].options & EXCLUDE_INCLUDE);



for (i = 0; i < exclude_count; i++)
{
char const *pattern = exclude[i].pattern;
int options = exclude[i].options;
if (excluded == !! (options & EXCLUDE_INCLUDE))
{
int (*matcher) (char const *, char const *, int) =
(options & EXCLUDE_WILDCARDS
? fnmatch
: fnmatch_no_wildcards);
bool matched = ((*matcher) (pattern, f, options) == 0);
char const *p;

if (! (options & EXCLUDE_ANCHORED))
for (p = f; *p && ! matched; p++)
if (*p == '/' && p[1] != '/')
matched = ((*matcher) (pattern, p + 1, options) == 0);

excluded ^= matched;
}
}

return excluded;
}
}



void
add_exclude (struct exclude *ex, char const *pattern, int options)
{
struct patopts *patopts;

if (ex->exclude_count == ex->exclude_alloc)
ex->exclude = x2nrealloc (ex->exclude, &ex->exclude_alloc,
sizeof *ex->exclude);

patopts = &ex->exclude[ex->exclude_count++];
patopts->pattern = pattern;
patopts->options = options;
}






int
add_exclude_file (void (*add_func) (struct exclude *, char const *, int),
struct exclude *ex, char const *filename, int options,
char line_end)
{
bool use_stdin = filename[0] == '-' && !filename[1];
FILE *in;
char *buf = NULL;
char *p;
char const *pattern;
char const *lim;
size_t buf_alloc = 0;
size_t buf_count = 0;
int c;
int e = 0;

if (use_stdin)
in = stdin;
else if (! (in = fopen (filename, "r")))
return -1;

while ((c = getc (in)) != EOF)
{
if (buf_count == buf_alloc)
buf = x2realloc (buf, &buf_alloc);
buf[buf_count++] = c;
}

if (ferror (in))
e = errno;

if (!use_stdin && fclose (in) != 0)
e = errno;

buf = xrealloc (buf, buf_count + 1);
buf[buf_count] = line_end;
lim = buf + buf_count + ! (buf_count == 0 || buf[buf_count - 1] == line_end);
pattern = buf;

for (p = buf; p < lim; p++)
if (*p == line_end)
{
char *pattern_end = p;

if (is_space (line_end))
{
for (; ; pattern_end--)
if (pattern_end == pattern)
goto next_pattern;
else if (! is_space (pattern_end[-1]))
break;
}

*pattern_end = '\0';
(*add_func) (ex, pattern, options);

next_pattern:
pattern = p + 1;
}

errno = e;
return e ? -1 : 0;
}
