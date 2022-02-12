







#if !defined(LLDB_HOST_POSIX_HOSTTHREADPOSIX_H)
#define LLDB_HOST_POSIX_HOSTTHREADPOSIX_H

#include "lldb/Host/HostNativeThreadBase.h"

namespace lldb_private {

class HostThreadPosix : public HostNativeThreadBase {
HostThreadPosix(const HostThreadPosix &) = delete;
const HostThreadPosix &operator=(const HostThreadPosix &) = delete;

public:
HostThreadPosix();
HostThreadPosix(lldb::thread_t thread);
~HostThreadPosix() override;

Status Join(lldb::thread_result_t *result) override;
Status Cancel() override;

Status Detach();
};

}

#endif
