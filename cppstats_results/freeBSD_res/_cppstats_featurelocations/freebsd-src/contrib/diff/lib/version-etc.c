


















#if HAVE_CONFIG_H
#include <config.h>
#endif


#include "version-etc.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "unlocked-io.h"

#include "gettext.h"
#define _(msgid) gettext (msgid)



const char* version_etc_copyright =

"Copyright (C) 2004 Free Software Foundation, Inc.";




void
version_etc_va (FILE *stream,
const char *command_name, const char *package,
const char *version, va_list authors)
{
unsigned int n_authors;


{
va_list tmp_authors;

#if defined(__va_copy)
__va_copy (tmp_authors, authors);
#else
tmp_authors = authors;
#endif

n_authors = 0;
while (va_arg (tmp_authors, const char *) != NULL)
++n_authors;
}

if (command_name)
fprintf (stream, "%s (%s) %s\n", command_name, package, version);
else
fprintf (stream, "%s %s\n", package, version);

switch (n_authors)
{
case 0:

abort ();
case 1:

vfprintf (stream, _("Written by %s.\n"), authors);
break;
case 2:

vfprintf (stream, _("Written by %s and %s.\n"), authors);
break;
case 3:

vfprintf (stream, _("Written by %s, %s, and %s.\n"), authors);
break;
case 4:



vfprintf (stream, _("Written by %s, %s, %s,\nand %s.\n"), authors);
break;
case 5:



vfprintf (stream, _("Written by %s, %s, %s,\n%s, and %s.\n"), authors);
break;
case 6:



vfprintf (stream, _("Written by %s, %s, %s,\n%s, %s, and %s.\n"),
authors);
break;
case 7:



vfprintf (stream, _("Written by %s, %s, %s,\n%s, %s, %s, and %s.\n"),
authors);
break;
case 8:



vfprintf (stream, _("\
Written by %s, %s, %s,\n%s, %s, %s, %s,\nand %s.\n"),
authors);
break;
case 9:



vfprintf (stream, _("\
Written by %s, %s, %s,\n%s, %s, %s, %s,\n%s, and %s.\n"),
authors);
break;
default:





vfprintf (stream, _("\
Written by %s, %s, %s,\n%s, %s, %s, %s,\n%s, %s, and others.\n"),
authors);
break;
}
va_end (authors);
putc ('\n', stream);

fputs (version_etc_copyright, stream);
putc ('\n', stream);

fputs (_("\
This is free software; see the source for copying conditions. There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"),
stream);
}















void
version_etc (FILE *stream,
const char *command_name, const char *package,
const char *version, ...)
{
va_list authors;

va_start (authors, version);
version_etc_va (stream, command_name, package, version, authors);
}
