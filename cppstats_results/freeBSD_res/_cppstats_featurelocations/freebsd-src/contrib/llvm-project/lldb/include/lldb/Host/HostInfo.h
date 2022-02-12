







#if !defined(LLDB_HOST_HOSTINFO_H)
#define LLDB_HOST_HOSTINFO_H
























#if defined(_WIN32)
#include "lldb/Host/windows/HostInfoWindows.h"
#define HOST_INFO_TYPE HostInfoWindows
#elif defined(__linux__) || defined(__EMSCRIPTEN__)
#if defined(__ANDROID__)
#include "lldb/Host/android/HostInfoAndroid.h"
#define HOST_INFO_TYPE HostInfoAndroid
#else
#include "lldb/Host/linux/HostInfoLinux.h"
#define HOST_INFO_TYPE HostInfoLinux
#endif
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#include "lldb/Host/freebsd/HostInfoFreeBSD.h"
#define HOST_INFO_TYPE HostInfoFreeBSD
#elif defined(__NetBSD__)
#include "lldb/Host/netbsd/HostInfoNetBSD.h"
#define HOST_INFO_TYPE HostInfoNetBSD
#elif defined(__OpenBSD__)
#include "lldb/Host/openbsd/HostInfoOpenBSD.h"
#define HOST_INFO_TYPE HostInfoOpenBSD
#elif defined(__APPLE__)
#include "lldb/Host/macosx/HostInfoMacOSX.h"
#define HOST_INFO_TYPE HostInfoMacOSX
#else
#include "lldb/Host/posix/HostInfoPosix.h"
#define HOST_INFO_TYPE HostInfoPosix
#endif

namespace lldb_private {
typedef HOST_INFO_TYPE HostInfo;
}

#undef HOST_INFO_TYPE

#endif
