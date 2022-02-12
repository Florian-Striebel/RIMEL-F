
#include "util-internal.h"
#if defined(_WIN32)
#include <winsock2.h>
#endif

#include "event2/util.h"

#if defined(snprintf)
#undef snprintf
#endif
#define snprintf evutil_snprintf
