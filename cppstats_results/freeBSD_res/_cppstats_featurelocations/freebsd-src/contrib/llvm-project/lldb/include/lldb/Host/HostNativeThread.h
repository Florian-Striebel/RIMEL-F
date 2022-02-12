







#if !defined(LLDB_HOST_HOSTNATIVETHREAD_H)
#define LLDB_HOST_HOSTNATIVETHREAD_H

#include "HostNativeThreadForward.h"

#if defined(_WIN32)
#include "lldb/Host/windows/HostThreadWindows.h"
#elif defined(__APPLE__)
#include "lldb/Host/macosx/HostThreadMacOSX.h"
#else
#include "lldb/Host/posix/HostThreadPosix.h"
#endif

#endif
