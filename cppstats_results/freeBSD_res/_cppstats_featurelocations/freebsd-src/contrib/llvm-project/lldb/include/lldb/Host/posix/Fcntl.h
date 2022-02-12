









#if !defined(liblldb_Host_posix_Fcntl_h_)
#define liblldb_Host_posix_Fcntl_h_

#if defined(__ANDROID__)
#include <android/api-level.h>
#endif

#include <fcntl.h>

#if defined(__ANDROID_API__) && __ANDROID_API__ < 21
#define F_DUPFD_CLOEXEC (F_LINUX_SPECIFIC_BASE + 6)
#endif

#endif
