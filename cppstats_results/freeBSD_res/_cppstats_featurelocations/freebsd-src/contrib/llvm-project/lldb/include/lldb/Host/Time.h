









#if !defined(LLDB_HOST_TIME_H)
#define LLDB_HOST_TIME_H

#if defined(__ANDROID__)
#include <android/api-level.h>
#endif

#if defined(__ANDROID_API__) && __ANDROID_API__ < 21
#include <time64.h>
extern time_t timegm(struct tm *t);
#else
#include <ctime>
#endif

#endif
