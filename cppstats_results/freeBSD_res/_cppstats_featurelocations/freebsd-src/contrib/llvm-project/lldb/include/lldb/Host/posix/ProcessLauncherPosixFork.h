







#if !defined(LLDB_HOST_POSIX_PROCESSLAUNCHERPOSIXFORK_H)
#define LLDB_HOST_POSIX_PROCESSLAUNCHERPOSIXFORK_H

#include "lldb/Host/ProcessLauncher.h"

namespace lldb_private {

class ProcessLauncherPosixFork : public ProcessLauncher {
public:
HostProcess LaunchProcess(const ProcessLaunchInfo &launch_info,
Status &error) override;
};

}

#endif
