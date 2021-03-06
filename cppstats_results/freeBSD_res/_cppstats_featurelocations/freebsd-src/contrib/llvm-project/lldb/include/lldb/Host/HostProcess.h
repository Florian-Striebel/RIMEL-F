







#if !defined(LLDB_HOST_HOSTPROCESS_H)
#define LLDB_HOST_HOSTPROCESS_H

#include "lldb/Host/Host.h"
#include "lldb/lldb-types.h"














namespace lldb_private {

class HostNativeProcessBase;
class HostThread;

class HostProcess {
public:
HostProcess();
HostProcess(lldb::process_t process);
~HostProcess();

Status Terminate();
Status GetMainModule(FileSpec &file_spec) const;

lldb::pid_t GetProcessId() const;
bool IsRunning() const;

llvm::Expected<HostThread>
StartMonitoring(const Host::MonitorChildProcessCallback &callback,
bool monitor_signals);

HostNativeProcessBase &GetNativeProcess();
const HostNativeProcessBase &GetNativeProcess() const;

private:
std::shared_ptr<HostNativeProcessBase> m_native_process;
};
}

#endif
