







#if !defined(LLDB_HOST_MONITORINGPROCESSLAUNCHER_H)
#define LLDB_HOST_MONITORINGPROCESSLAUNCHER_H

#include <memory>
#include "lldb/Host/ProcessLauncher.h"

namespace lldb_private {

class MonitoringProcessLauncher : public ProcessLauncher {
public:
explicit MonitoringProcessLauncher(
std::unique_ptr<ProcessLauncher> delegate_launcher);




HostProcess LaunchProcess(const ProcessLaunchInfo &launch_info,
Status &error) override;

private:
std::unique_ptr<ProcessLauncher> m_delegate_launcher;
};

}

#endif
