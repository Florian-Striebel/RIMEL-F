







#if !defined(LLDB_HOST_HOST_H)
#define LLDB_HOST_HOST_H

#include "lldb/Host/File.h"
#include "lldb/Host/HostThread.h"
#include "lldb/Utility/Environment.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/Timeout.h"
#include "lldb/lldb-private-forward.h"
#include "lldb/lldb-private.h"
#include <cerrno>
#include <cstdarg>
#include <map>
#include <string>
#include <type_traits>

namespace lldb_private {

class FileAction;
class ProcessLaunchInfo;
class ProcessInstanceInfo;
class ProcessInstanceInfoMatch;
typedef std::vector<ProcessInstanceInfo> ProcessInstanceInfoList;


struct WaitStatus {
enum Type : uint8_t {
Exit,

Signal,

Stop,

};

Type type;
uint8_t status;

WaitStatus(Type type, uint8_t status) : type(type), status(status) {}

static WaitStatus Decode(int wstatus);
};

inline bool operator==(WaitStatus a, WaitStatus b) {
return a.type == b.type && a.status == b.status;
}

inline bool operator!=(WaitStatus a, WaitStatus b) { return !(a == b); }





class Host {
public:
typedef std::function<bool(
lldb::pid_t pid, bool exited,
int signal,
int status)>
MonitorChildProcessCallback;

































static llvm::Expected<HostThread>
StartMonitoringChildProcess(const MonitorChildProcessCallback &callback,
lldb::pid_t pid, bool monitor_signals);

enum SystemLogType { eSystemLogWarning, eSystemLogError };

static void SystemLog(SystemLogType type, const char *format, ...)
__attribute__((format(printf, 2, 3)));

static void SystemLog(SystemLogType type, const char *format, va_list args);





static lldb::pid_t GetCurrentProcessID();

static void Kill(lldb::pid_t pid, int signo);






static lldb::thread_t GetCurrentThread();

static const char *GetSignalAsCString(int signo);














static FileSpec GetModuleFileSpecForHostAddress(const void *host_addr);
















static bool GetBundleDirectory(const FileSpec &file,
FileSpec &bundle_directory);













static bool ResolveExecutableInBundle(FileSpec &file);

static uint32_t FindProcesses(const ProcessInstanceInfoMatch &match_info,
ProcessInstanceInfoList &proc_infos);

typedef std::map<lldb::pid_t, bool> TidMap;
typedef std::pair<lldb::pid_t, bool> TidPair;
static bool FindProcessThreads(const lldb::pid_t pid, TidMap &tids_to_attach);

static bool GetProcessInfo(lldb::pid_t pid, ProcessInstanceInfo &proc_info);




static Status LaunchProcess(ProcessLaunchInfo &launch_info);






static Status ShellExpandArguments(ProcessLaunchInfo &launch_info);









static Status RunShellCommand(llvm::StringRef command,
const FileSpec &working_dir, int *status_ptr,
int *signo_ptr, std::string *command_output,
const Timeout<std::micro> &timeout,
bool run_in_shell = true,
bool hide_stderr = false);










static Status RunShellCommand(llvm::StringRef shell, llvm::StringRef command,
const FileSpec &working_dir, int *status_ptr,
int *signo_ptr, std::string *command_output,
const Timeout<std::micro> &timeout,
bool run_in_shell = true,
bool hide_stderr = false);








static Status RunShellCommand(const Args &args, const FileSpec &working_dir,
int *status_ptr, int *signo_ptr,
std::string *command_output,
const Timeout<std::micro> &timeout,
bool run_in_shell = true,
bool hide_stderr = false);










static Status RunShellCommand(llvm::StringRef shell, const Args &args,
const FileSpec &working_dir, int *status_ptr,
int *signo_ptr, std::string *command_output,
const Timeout<std::micro> &timeout,
bool run_in_shell = true,
bool hide_stderr = false);

static bool OpenFileInExternalEditor(const FileSpec &file_spec,
uint32_t line_no);

static Environment GetEnvironment();

static std::unique_ptr<Connection>
CreateDefaultConnection(llvm::StringRef url);

protected:
static uint32_t FindProcessesImpl(const ProcessInstanceInfoMatch &match_info,
ProcessInstanceInfoList &proc_infos);
};

}

namespace llvm {
template <> struct format_provider<lldb_private::WaitStatus> {


static void format(const lldb_private::WaitStatus &WS, raw_ostream &OS,
llvm::StringRef Options);
};
}

#endif
