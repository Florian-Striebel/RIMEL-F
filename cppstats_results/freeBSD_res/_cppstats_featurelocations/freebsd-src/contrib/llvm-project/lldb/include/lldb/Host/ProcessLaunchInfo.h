







#if !defined(LLDB_HOST_PROCESSLAUNCHINFO_H)
#define LLDB_HOST_PROCESSLAUNCHINFO_H


#include <string>


#include "lldb/Utility/Flags.h"

#include "lldb/Host/FileAction.h"
#include "lldb/Host/Host.h"
#include "lldb/Host/PseudoTerminal.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/ProcessInfo.h"
#include "lldb/Utility/StructuredData.h"

namespace lldb_private {





class ProcessLaunchInfo : public ProcessInfo {
public:
ProcessLaunchInfo();

ProcessLaunchInfo(const FileSpec &stdin_file_spec,
const FileSpec &stdout_file_spec,
const FileSpec &stderr_file_spec,
const FileSpec &working_dir, uint32_t launch_flags);

void AppendFileAction(const FileAction &info) {
m_file_actions.push_back(info);
}

bool AppendCloseFileAction(int fd);

bool AppendDuplicateFileAction(int fd, int dup_fd);

bool AppendOpenFileAction(int fd, const FileSpec &file_spec, bool read,
bool write);

bool AppendSuppressFileAction(int fd, bool read, bool write);




llvm::Error SetUpPtyRedirection();

size_t GetNumFileActions() const { return m_file_actions.size(); }

const FileAction *GetFileActionAtIndex(size_t idx) const;

const FileAction *GetFileActionForFD(int fd) const;

Flags &GetFlags() { return m_flags; }

const Flags &GetFlags() const { return m_flags; }

const FileSpec &GetWorkingDirectory() const;

void SetWorkingDirectory(const FileSpec &working_dir);

const char *GetProcessPluginName() const;

void SetProcessPluginName(llvm::StringRef plugin);

const FileSpec &GetShell() const;

void SetShell(const FileSpec &shell);

uint32_t GetResumeCount() const { return m_resume_count; }

void SetResumeCount(uint32_t c) { m_resume_count = c; }

bool GetLaunchInSeparateProcessGroup() const {
return m_flags.Test(lldb::eLaunchFlagLaunchInSeparateProcessGroup);
}

void SetLaunchInSeparateProcessGroup(bool separate);

bool GetShellExpandArguments() const {
return m_flags.Test(lldb::eLaunchFlagShellExpandArguments);
}

void SetShellExpandArguments(bool expand);

void Clear();

bool ConvertArgumentsForLaunchingInShell(Status &error, bool will_debug,
bool first_arg_is_full_shell_command,
uint32_t num_resumes);

void
SetMonitorProcessCallback(const Host::MonitorChildProcessCallback &callback,
bool monitor_signals);

Host::MonitorChildProcessCallback GetMonitorProcessCallback() const {
return m_monitor_callback;
}




static bool NoOpMonitorCallback(lldb::pid_t pid, bool exited, int signal,
int status);

bool GetMonitorSignals() const { return m_monitor_signals; }






bool MonitorProcess() const;

PseudoTerminal &GetPTY() { return *m_pty; }


lldb::ListenerSP GetListener() const { return m_listener_sp; }

void SetListener(const lldb::ListenerSP &listener_sp) {
m_listener_sp = listener_sp;
}

lldb::ListenerSP GetHijackListener() const { return m_hijack_listener_sp; }

void SetHijackListener(const lldb::ListenerSP &listener_sp) {
m_hijack_listener_sp = listener_sp;
}

void SetLaunchEventData(const char *data) { m_event_data.assign(data); }

const char *GetLaunchEventData() const { return m_event_data.c_str(); }

void SetDetachOnError(bool enable);

bool GetDetachOnError() const {
return m_flags.Test(lldb::eLaunchFlagDetachOnError);
}

bool IsScriptedProcess() const {
return !m_scripted_process_class_name.empty();
}

std::string GetScriptedProcessClassName() const {
return m_scripted_process_class_name;
}

void SetScriptedProcessClassName(std::string name) {
m_scripted_process_class_name = name;
}

lldb_private::StructuredData::DictionarySP
GetScriptedProcessDictionarySP() const {
return m_scripted_process_dictionary_sp;
}

void SetScriptedProcessDictionarySP(
lldb_private::StructuredData::DictionarySP dictionary_sp) {
m_scripted_process_dictionary_sp = dictionary_sp;
}

protected:
FileSpec m_working_dir;
std::string m_plugin_name;
FileSpec m_shell;
Flags m_flags;
std::vector<FileAction> m_file_actions;
std::shared_ptr<PseudoTerminal> m_pty;
uint32_t m_resume_count = 0;
Host::MonitorChildProcessCallback m_monitor_callback;
void *m_monitor_callback_baton = nullptr;
bool m_monitor_signals = false;
std::string m_event_data;

lldb::ListenerSP m_listener_sp;
lldb::ListenerSP m_hijack_listener_sp;
std::string m_scripted_process_class_name;

StructuredData::DictionarySP
m_scripted_process_dictionary_sp;

};
}

#endif
