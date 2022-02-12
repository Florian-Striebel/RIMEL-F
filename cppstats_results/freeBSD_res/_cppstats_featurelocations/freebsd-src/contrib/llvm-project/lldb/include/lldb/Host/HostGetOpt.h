







#if !defined(LLDB_HOST_HOSTGETOPT_H)
#define LLDB_HOST_HOSTGETOPT_H

#if !defined(_MSC_VER) && !defined(__NetBSD__)

#if defined(_WIN32)
#define _BSD_SOURCE
#endif

#include <getopt.h>
#include <unistd.h>

#else

#include <lldb/Host/common/GetOptInc.h>

#endif

#endif
