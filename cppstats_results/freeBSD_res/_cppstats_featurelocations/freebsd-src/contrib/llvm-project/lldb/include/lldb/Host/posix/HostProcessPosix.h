







#if !defined(LLDB_HOST_POSIX_HOSTPROCESSPOSIX_H)
#define LLDB_HOST_POSIX_HOSTPROCESSPOSIX_H

#include "lldb/Host/HostNativeProcessBase.h"
#include "lldb/Utility/Status.h"
#include "lldb/lldb-types.h"

namespace lldb_private {

class FileSpec;

class HostProcessPosix : public HostNativeProcessBase {
public:
HostProcessPosix();
HostProcessPosix(lldb::process_t process);
~HostProcessPosix() override;

virtual Status Signal(int signo) const;
static Status Signal(lldb::process_t process, int signo);

Status Terminate() override;
Status GetMainModule(FileSpec &file_spec) const override;

lldb::pid_t GetProcessId() const override;
bool IsRunning() const override;

llvm::Expected<HostThread>
StartMonitoring(const Host::MonitorChildProcessCallback &callback,
bool monitor_signals) override;
};

}

#endif
