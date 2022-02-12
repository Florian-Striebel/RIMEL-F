







#if !defined(LLDB_HOST_PROCESSLAUNCHER_H)
#define LLDB_HOST_PROCESSLAUNCHER_H

namespace lldb_private {

class ProcessLaunchInfo;
class Status;
class HostProcess;

class ProcessLauncher {
public:
virtual ~ProcessLauncher() = default;
virtual HostProcess LaunchProcess(const ProcessLaunchInfo &launch_info,
Status &error) = 0;
};
}

#endif
