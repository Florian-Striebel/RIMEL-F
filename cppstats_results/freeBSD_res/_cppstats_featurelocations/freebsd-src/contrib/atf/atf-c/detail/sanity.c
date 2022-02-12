
























#include "atf-c/detail/sanity.h"

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <err.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static
void
fail(const char *fmt, ...)
{
va_list ap;
char buf[4096];

va_start(ap, fmt);
vsnprintf(buf, sizeof(buf), fmt, ap);
va_end(ap);
warnx("%s", buf);
warnx("%s", "");
warnx("This is probably a bug in this application or one of the "
"libraries it uses. If you believe this problem is caused "
"by, or is related to " PACKAGE_STRING ", please report it "
"to " PACKAGE_BUGREPORT " and provide as many details as "
"possible describing how you got to this condition.");

abort();
}

void
atf_sanity_inv(const char *file, int line, const char *cond)
{
fail("Invariant check failed at %s:%d: %s", file, line, cond);
}

void
atf_sanity_pre(const char *file, int line, const char *cond)
{
fail("Precondition check failed at %s:%d: %s", file, line, cond);
}

void
atf_sanity_post(const char *file, int line, const char *cond)
{
fail("Postcondition check failed at %s:%d: %s", file, line, cond);
}
