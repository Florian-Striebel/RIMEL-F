#if !defined(STRLCPY_INTERNAL_H_INCLUDED_)
#define STRLCPY_INTERNAL_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif

#include "event2/event-config.h"
#include "evconfig-private.h"

#if !defined(EVENT__HAVE_STRLCPY)
#include <string.h>
size_t event_strlcpy_(char *dst, const char *src, size_t siz);
#define strlcpy event_strlcpy_
#endif

#if defined(__cplusplus)
}
#endif

#endif

