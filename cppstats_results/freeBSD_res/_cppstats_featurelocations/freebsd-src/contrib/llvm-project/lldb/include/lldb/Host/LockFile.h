







#if !defined(LLDB_HOST_LOCKFILE_H)
#define LLDB_HOST_LOCKFILE_H

#if defined(_WIN32)
#include "lldb/Host/windows/LockFileWindows.h"
namespace lldb_private {
typedef LockFileWindows LockFile;
}
#else
#include "lldb/Host/posix/LockFilePosix.h"
namespace lldb_private {
typedef LockFilePosix LockFile;
}
#endif

#endif
