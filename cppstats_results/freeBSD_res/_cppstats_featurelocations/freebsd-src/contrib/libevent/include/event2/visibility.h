

























#if !defined(EVENT2_VISIBILITY_H_INCLUDED_)
#define EVENT2_VISIBILITY_H_INCLUDED_

#include <event2/event-config.h>

#if defined(event_EXPORTS) || defined(event_extra_EXPORTS) || defined(event_core_EXPORTS)
#if defined (__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#define EVENT2_EXPORT_SYMBOL __global
#elif defined __GNUC__
#define EVENT2_EXPORT_SYMBOL __attribute__ ((visibility("default")))
#elif defined(_MSC_VER)
#define EVENT2_EXPORT_SYMBOL extern __declspec(dllexport)
#else
#define EVENT2_EXPORT_SYMBOL
#endif
#else
#if defined(EVENT__NEED_DLLIMPORT) && defined(_MSC_VER) && !defined(EVENT_BUILDING_REGRESS_TEST)
#define EVENT2_EXPORT_SYMBOL extern __declspec(dllimport)
#else
#define EVENT2_EXPORT_SYMBOL
#endif
#endif

#endif
