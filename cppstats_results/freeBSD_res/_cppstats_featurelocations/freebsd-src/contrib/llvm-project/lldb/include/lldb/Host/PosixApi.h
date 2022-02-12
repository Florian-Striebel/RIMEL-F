







#if !defined(LLDB_HOST_POSIXAPI_H)
#define LLDB_HOST_POSIXAPI_H





#if defined(_WIN32)
#include "lldb/Host/windows/PosixApi.h"
#else
#include <unistd.h>
#include <csignal>
#endif

#endif
