
























#include "atf-c/detail/env.h"

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <errno.h>
#include <stdlib.h>

#include "atf-c/detail/sanity.h"
#include "atf-c/detail/text.h"
#include "atf-c/error.h"

const char *
atf_env_get(const char *name)
{
const char* val = getenv(name);
PRE(val != NULL);
return val;
}

const char *
atf_env_get_with_default(const char *name, const char *default_value)
{
const char* val = getenv(name);
if (val == NULL)
return default_value;
else
return val;
}

bool
atf_env_has(const char *name)
{
return getenv(name) != NULL;
}

atf_error_t
atf_env_set(const char *name, const char *val)
{
atf_error_t err;

#if defined(HAVE_SETENV)
if (setenv(name, val, 1) == -1)
err = atf_libc_error(errno, "Cannot set environment variable "
"'%s' to '%s'", name, val);
else
err = atf_no_error();
#elif defined(HAVE_PUTENV)
char *buf;

err = atf_text_format(&buf, "%s=%s", name, val);
if (!atf_is_error(err)) {
if (putenv(buf) == -1)
err = atf_libc_error(errno, "Cannot set environment variable "
"'%s' to '%s'", name, val);
free(buf);
}
#else
#error "Don't know how to set an environment variable."
#endif

return err;
}

atf_error_t
atf_env_unset(const char *name)
{
atf_error_t err;

#if defined(HAVE_UNSETENV)
unsetenv(name);
err = atf_no_error();
#elif defined(HAVE_PUTENV)
char *buf;

err = atf_text_format(&buf, "%s=", name);
if (!atf_is_error(err)) {
if (putenv(buf) == -1)
err = atf_libc_error(errno, "Cannot unset environment variable"
" '%s'", name);
free(buf);
}
#else
#error "Don't know how to unset an environment variable."
#endif

return err;
}
