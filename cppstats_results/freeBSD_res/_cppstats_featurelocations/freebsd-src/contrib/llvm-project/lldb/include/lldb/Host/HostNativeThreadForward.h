







#if !defined(LLDB_HOST_HOSTNATIVETHREADFORWARD_H)
#define LLDB_HOST_HOSTNATIVETHREADFORWARD_H

namespace lldb_private {
#if defined(_WIN32)
class HostThreadWindows;
typedef HostThreadWindows HostNativeThread;
#elif defined(__APPLE__)
class HostThreadMacOSX;
typedef HostThreadMacOSX HostNativeThread;
#else
class HostThreadPosix;
typedef HostThreadPosix HostNativeThread;
#endif
}

#endif
