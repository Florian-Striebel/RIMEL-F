







#if !defined(LLDB_HOST_HOSTNATIVEPROCESS_H)
#define LLDB_HOST_HOSTNATIVEPROCESS_H

#if defined(_WIN32)
#include "lldb/Host/windows/HostProcessWindows.h"
namespace lldb_private {
typedef HostProcessWindows HostNativeProcess;
}
#else
#include "lldb/Host/posix/HostProcessPosix.h"
namespace lldb_private {
typedef HostProcessPosix HostNativeProcess;
}
#endif

#endif
