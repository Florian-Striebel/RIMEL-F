




#include "openbsd-compat.h"

#if !defined(HAVE_EXPLICIT_BZERO) && defined(_WIN32)

#include <windows.h>
#include <string.h>

void
explicit_bzero(void *buf, size_t len)
{
SecureZeroMemory(buf, len);
}

#endif
