







#if !defined(LLDB_HOST_PIPE_H)
#define LLDB_HOST_PIPE_H

#if defined(_WIN32)
#include "lldb/Host/windows/PipeWindows.h"
namespace lldb_private {
typedef PipeWindows Pipe;
}
#else
#include "lldb/Host/posix/PipePosix.h"
namespace lldb_private {
typedef PipePosix Pipe;
}
#endif

#endif
