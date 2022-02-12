

















#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "hard-locale.h"

#if HAVE_LOCALE_H
#include <locale.h>
#endif

#include <stdlib.h>
#include <string.h>



int
hard_locale (int category)
{
#if ! HAVE_SETLOCALE
return 0;
#else

int hard = 1;
char const *p = setlocale (category, 0);

if (p)
{
#if defined __GLIBC__ && 2 <= __GLIBC__
if (strcmp (p, "C") == 0 || strcmp (p, "POSIX") == 0)
hard = 0;
#else
char *locale = malloc (strlen (p) + 1);
if (locale)
{
strcpy (locale, p);




if (((p = setlocale (category, "C"))
&& strcmp (p, locale) == 0)
|| ((p = setlocale (category, "POSIX"))
&& strcmp (p, locale) == 0))
hard = 0;


setlocale (category, locale);
free (locale);
}
#endif
}

return hard;

#endif
}
