







#if !defined(LLDB_TARGET_PLATFORM_H)
#define LLDB_TARGET_PLATFORM_H

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "lldb/Core/PluginInterface.h"
#include "lldb/Core/UserSettingsController.h"
#include "lldb/Host/File.h"
#include "lldb/Interpreter/Options.h"
#include "lldb/Utility/ArchSpec.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/StructuredData.h"
#include "lldb/Utility/Timeout.h"
#include "lldb/Utility/UserIDResolver.h"
#include "lldb/lldb-private-forward.h"
#include "lldb/lldb-public.h"
#include "llvm/Support/VersionTuple.h"

namespace lldb_private {

class ProcessInstanceInfo;
class ProcessInstanceInfoMatch;
typedef std::vector<ProcessInstanceInfo> ProcessInstanceInfoList;

class ModuleCache;
enum MmapFlags { eMmapFlagsPrivate = 1, eMmapFlagsAnon = 2 };

class PlatformProperties : public Properties {
public:
PlatformProperties();

static ConstString GetSettingName();

bool GetUseModuleCache() const;
bool SetUseModuleCache(bool use_module_cache);

FileSpec GetModuleCacheDirectory() const;
bool SetModuleCacheDirectory(const FileSpec &dir_spec);

private:
void SetDefaultModuleCacheDirectory(const FileSpec &dir_spec);
};

typedef std::shared_ptr<PlatformProperties> PlatformPropertiesSP;
typedef llvm::SmallVector<lldb::addr_t, 6> MmapArgList;












class Platform : public PluginInterface {
public:

Platform(bool is_host_platform);





~Platform() override;

static void Initialize();

static void Terminate();

static const PlatformPropertiesSP &GetGlobalPlatformProperties();










static lldb::PlatformSP GetHostPlatform();

static lldb::PlatformSP
GetPlatformForArchitecture(const ArchSpec &arch, ArchSpec *platform_arch_ptr);

static const char *GetHostPlatformName();

static void SetHostPlatform(const lldb::PlatformSP &platform_sp);


static lldb::PlatformSP Find(ConstString name);

static lldb::PlatformSP Create(ConstString name, Status &error);

static lldb::PlatformSP Create(const ArchSpec &arch,
ArchSpec *platform_arch_ptr, Status &error);



static ArchSpec GetAugmentedArchSpec(Platform *platform,
llvm::StringRef triple);































virtual Status ResolveExecutable(const ModuleSpec &module_spec,
lldb::ModuleSP &module_sp,
const FileSpecList *module_search_paths_ptr);














































virtual Status ResolveSymbolFile(Target &target, const ModuleSpec &sym_spec,
FileSpec &sym_file);



virtual bool ResolveRemotePath(const FileSpec &platform_path,
FileSpec &resolved_platform_path);








virtual llvm::VersionTuple GetOSVersion(Process *process = nullptr);

bool SetOSVersion(llvm::VersionTuple os_version);

bool GetOSBuildString(std::string &s);

bool GetOSKernelDescription(std::string &s);


ConstString GetName();

virtual const char *GetHostname();

virtual ConstString GetFullNameForDylib(ConstString basename);

virtual const char *GetDescription() = 0;







virtual void GetStatus(Stream &strm);






virtual bool GetRemoteOSVersion() { return false; }

virtual bool GetRemoteOSBuildString(std::string &s) {
s.clear();
return false;
}

virtual bool GetRemoteOSKernelDescription(std::string &s) {
s.clear();
return false;
}


virtual ArchSpec GetRemoteSystemArchitecture() {
return ArchSpec();
}

virtual FileSpec GetRemoteWorkingDirectory() { return m_working_dir; }

virtual bool SetRemoteWorkingDirectory(const FileSpec &working_dir);

virtual UserIDResolver &GetUserIDResolver() = 0;



























virtual Status GetFileWithUUID(const FileSpec &platform_file,
const UUID *uuid_ptr, FileSpec &local_file);





virtual FileSpecList
LocateExecutableScriptingResources(Target *target, Module &module,
Stream *feedback_stream);

virtual Status GetSharedModule(
const ModuleSpec &module_spec, Process *process,
lldb::ModuleSP &module_sp, const FileSpecList *module_search_paths_ptr,
llvm::SmallVectorImpl<lldb::ModuleSP> *old_modules, bool *did_create_ptr);

virtual bool GetModuleSpec(const FileSpec &module_file_spec,
const ArchSpec &arch, ModuleSpec &module_spec);

virtual Status ConnectRemote(Args &args);

virtual Status DisconnectRemote();














virtual bool GetSupportedArchitectureAtIndex(uint32_t idx,
ArchSpec &arch) = 0;

virtual size_t GetSoftwareBreakpointTrapOpcode(Target &target,
BreakpointSite *bp_site);



virtual Status LaunchProcess(ProcessLaunchInfo &launch_info);






virtual Status ShellExpandArguments(ProcessLaunchInfo &launch_info);


virtual Status KillProcess(const lldb::pid_t pid);



virtual bool IsCompatibleArchitecture(const ArchSpec &arch,
bool exact_arch_match,
ArchSpec *compatible_arch_ptr);





virtual bool CanDebugProcess() { return true; }






virtual lldb::ProcessSP
DebugProcess(ProcessLaunchInfo &launch_info, Debugger &debugger,
Target *target,

Status &error);

virtual lldb::ProcessSP ConnectProcess(llvm::StringRef connect_url,
llvm::StringRef plugin_name,
Debugger &debugger, Target *target,
Status &error);

virtual lldb::ProcessSP
ConnectProcessSynchronous(llvm::StringRef connect_url,
llvm::StringRef plugin_name, Debugger &debugger,
Stream &stream, Target *target, Status &error);














virtual lldb::ProcessSP Attach(ProcessAttachInfo &attach_info,
Debugger &debugger,
Target *target,


Status &error) = 0;






















virtual uint32_t FindProcesses(const ProcessInstanceInfoMatch &match_info,
ProcessInstanceInfoList &proc_infos);

virtual bool GetProcessInfo(lldb::pid_t pid, ProcessInstanceInfo &proc_info);




virtual lldb::BreakpointSP SetThreadCreationBreakpoint(Target &target);



virtual lldb_private::ConstString
GetSDKDirectory(lldb_private::Target &target) {
return lldb_private::ConstString();
}

const std::string &GetRemoteURL() const { return m_remote_url; }

bool IsHost() const {
return m_is_host;
}

bool IsRemote() const { return !m_is_host; }

virtual bool IsConnected() const {

return IsHost();
}

const ArchSpec &GetSystemArchitecture();

void SetSystemArchitecture(const ArchSpec &arch) {
m_system_arch = arch;
if (IsHost())
m_os_version_set_while_connected = m_system_arch.IsValid();
}




ArchSpec GetAugmentedArchSpec(llvm::StringRef triple);


size_t GetMaxUserIDNameLength() const { return m_max_uid_name_len; }


size_t GetMaxGroupIDNameLength() const { return m_max_gid_name_len; }

ConstString GetSDKRootDirectory() const { return m_sdk_sysroot; }

void SetSDKRootDirectory(ConstString dir) { m_sdk_sysroot = dir; }

ConstString GetSDKBuild() const { return m_sdk_build; }

void SetSDKBuild(ConstString sdk_build) { m_sdk_build = sdk_build; }




virtual bool SupportsModules() { return false; }



virtual void
AddClangModuleCompilationOptions(Target *target,
std::vector<std::string> &options);

FileSpec GetWorkingDirectory();

bool SetWorkingDirectory(const FileSpec &working_dir);





virtual bool
ModuleIsExcludedForUnconstrainedSearches(Target &target,
const lldb::ModuleSP &module_sp) {
return false;
}

virtual Status MakeDirectory(const FileSpec &file_spec, uint32_t permissions);

virtual Status GetFilePermissions(const FileSpec &file_spec,
uint32_t &file_permissions);

virtual Status SetFilePermissions(const FileSpec &file_spec,
uint32_t file_permissions);

virtual lldb::user_id_t OpenFile(const FileSpec &file_spec,
File::OpenOptions flags, uint32_t mode,
Status &error) {
return UINT64_MAX;
}

virtual bool CloseFile(lldb::user_id_t fd, Status &error) { return false; }

virtual lldb::user_id_t GetFileSize(const FileSpec &file_spec) {
return UINT64_MAX;
}

virtual void AutoCompleteDiskFileOrDirectory(CompletionRequest &request,
bool only_dir) {}

virtual uint64_t ReadFile(lldb::user_id_t fd, uint64_t offset, void *dst,
uint64_t dst_len, Status &error) {
error.SetErrorStringWithFormat(
"Platform::ReadFile() is not supported in the %s platform",
GetName().GetCString());
return -1;
}

virtual uint64_t WriteFile(lldb::user_id_t fd, uint64_t offset,
const void *src, uint64_t src_len, Status &error) {
error.SetErrorStringWithFormat(
"Platform::WriteFile() is not supported in the %s platform",
GetName().GetCString());
return -1;
}

virtual Status GetFile(const FileSpec &source, const FileSpec &destination);

virtual Status PutFile(const FileSpec &source, const FileSpec &destination,
uint32_t uid = UINT32_MAX, uint32_t gid = UINT32_MAX);

virtual Status
CreateSymlink(const FileSpec &src,
const FileSpec &dst);























virtual Status Install(const FileSpec &src, const FileSpec &dst);

virtual Environment GetEnvironment();

virtual bool GetFileExists(const lldb_private::FileSpec &file_spec);

virtual Status Unlink(const FileSpec &file_spec);

virtual MmapArgList GetMmapArgumentList(const ArchSpec &arch,
lldb::addr_t addr,
lldb::addr_t length,
unsigned prot, unsigned flags,
lldb::addr_t fd, lldb::addr_t offset);

virtual bool GetSupportsRSync() { return m_supports_rsync; }

virtual void SetSupportsRSync(bool flag) { m_supports_rsync = flag; }

virtual const char *GetRSyncOpts() { return m_rsync_opts.c_str(); }

virtual void SetRSyncOpts(const char *opts) { m_rsync_opts.assign(opts); }

virtual const char *GetRSyncPrefix() { return m_rsync_prefix.c_str(); }

virtual void SetRSyncPrefix(const char *prefix) {
m_rsync_prefix.assign(prefix);
}

virtual bool GetSupportsSSH() { return m_supports_ssh; }

virtual void SetSupportsSSH(bool flag) { m_supports_ssh = flag; }

virtual const char *GetSSHOpts() { return m_ssh_opts.c_str(); }

virtual void SetSSHOpts(const char *opts) { m_ssh_opts.assign(opts); }

virtual bool GetIgnoresRemoteHostname() { return m_ignores_remote_hostname; }

virtual void SetIgnoresRemoteHostname(bool flag) {
m_ignores_remote_hostname = flag;
}

virtual lldb_private::OptionGroupOptions *
GetConnectionOptions(CommandInterpreter &interpreter) {
return nullptr;
}

virtual lldb_private::Status RunShellCommand(
llvm::StringRef command,
const FileSpec &working_dir,

int *status_ptr,
int *signo_ptr,

std::string
*command_output,
const Timeout<std::micro> &timeout);

virtual lldb_private::Status RunShellCommand(
llvm::StringRef shell, llvm::StringRef command,
const FileSpec &working_dir,

int *status_ptr,
int *signo_ptr,

std::string
*command_output,
const Timeout<std::micro> &timeout);

virtual void SetLocalCacheDirectory(const char *local);

virtual const char *GetLocalCacheDirectory();

virtual std::string GetPlatformSpecificConnectionInformation() { return ""; }

virtual bool CalculateMD5(const FileSpec &file_spec, uint64_t &low,
uint64_t &high);

virtual uint32_t GetResumeCountForLaunchInfo(ProcessLaunchInfo &launch_info) {
return 1;
}

virtual const lldb::UnixSignalsSP &GetRemoteUnixSignals();

lldb::UnixSignalsSP GetUnixSignals();




















virtual std::string
GetQueueNameForThreadQAddress(Process *process, lldb::addr_t dispatch_qaddr) {
return "";
}



















virtual lldb::queue_id_t
GetQueueIDForThreadQAddress(Process *process, lldb::addr_t dispatch_qaddr) {
return LLDB_INVALID_QUEUE_ID;
}




















virtual const std::vector<ConstString> &GetTrapHandlerSymbolNames();














virtual FileSpec LocateExecutable(const char *basename) { return FileSpec(); }




virtual uint32_t GetDefaultMemoryCacheLineSize() { return 0; }
































uint32_t LoadImage(lldb_private::Process *process,
const lldb_private::FileSpec &local_file,
const lldb_private::FileSpec &remote_file,
lldb_private::Status &error);































uint32_t LoadImageUsingPaths(lldb_private::Process *process,
const lldb_private::FileSpec &library_name,
const std::vector<std::string> &paths,
lldb_private::Status &error,
lldb_private::FileSpec *loaded_path);

virtual uint32_t DoLoadImage(lldb_private::Process *process,
const lldb_private::FileSpec &remote_file,
const std::vector<std::string> *paths,
lldb_private::Status &error,
lldb_private::FileSpec *loaded_path = nullptr);

virtual Status UnloadImage(lldb_private::Process *process,
uint32_t image_token);















virtual size_t ConnectToWaitingProcesses(lldb_private::Debugger &debugger,
lldb_private::Status &error);
















virtual llvm::Expected<StructuredData::DictionarySP>
FetchExtendedCrashInformation(lldb_private::Process &process) {
return nullptr;
}

protected:


lldb::ProcessSP DoConnectProcess(llvm::StringRef connect_url,
llvm::StringRef plugin_name,
Debugger &debugger, Stream *stream,
Target *target, Status &error);
bool m_is_host;





bool m_os_version_set_while_connected;
bool m_system_arch_set_while_connected;
ConstString
m_sdk_sysroot;
ConstString m_sdk_build;
FileSpec m_working_dir;

std::string m_remote_url;
std::string m_name;
llvm::VersionTuple m_os_version;
ArchSpec
m_system_arch;
typedef std::map<uint32_t, ConstString> IDToNameMap;


std::mutex m_mutex;
size_t m_max_uid_name_len;
size_t m_max_gid_name_len;
bool m_supports_rsync;
std::string m_rsync_opts;
std::string m_rsync_prefix;
bool m_supports_ssh;
std::string m_ssh_opts;
bool m_ignores_remote_hostname;
std::string m_local_cache_directory;
std::vector<ConstString> m_trap_handlers;
bool m_calculated_trap_handlers;
const std::unique_ptr<ModuleCache> m_module_cache;












virtual void CalculateTrapHandlerSymbolNames() = 0;

Status GetCachedExecutable(ModuleSpec &module_spec, lldb::ModuleSP &module_sp,
const FileSpecList *module_search_paths_ptr,
Platform &remote_platform);

virtual Status DownloadModuleSlice(const FileSpec &src_file_spec,
const uint64_t src_offset,
const uint64_t src_size,
const FileSpec &dst_file_spec);

virtual Status DownloadSymbolFile(const lldb::ModuleSP &module_sp,
const FileSpec &dst_file_spec);

virtual const char *GetCacheHostname();

private:
typedef std::function<Status(const ModuleSpec &)> ModuleResolver;

Status GetRemoteSharedModule(const ModuleSpec &module_spec, Process *process,
lldb::ModuleSP &module_sp,
const ModuleResolver &module_resolver,
bool *did_create_ptr);

bool GetCachedSharedModule(const ModuleSpec &module_spec,
lldb::ModuleSP &module_sp, bool *did_create_ptr);

Status LoadCachedExecutable(const ModuleSpec &module_spec,
lldb::ModuleSP &module_sp,
const FileSpecList *module_search_paths_ptr,
Platform &remote_platform);

FileSpec GetModuleCacheRoot();
};

class PlatformList {
public:
PlatformList() : m_mutex(), m_platforms(), m_selected_platform_sp() {}

~PlatformList() = default;

void Append(const lldb::PlatformSP &platform_sp, bool set_selected) {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
m_platforms.push_back(platform_sp);
if (set_selected)
m_selected_platform_sp = m_platforms.back();
}

size_t GetSize() {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
return m_platforms.size();
}

lldb::PlatformSP GetAtIndex(uint32_t idx) {
lldb::PlatformSP platform_sp;
{
std::lock_guard<std::recursive_mutex> guard(m_mutex);
if (idx < m_platforms.size())
platform_sp = m_platforms[idx];
}
return platform_sp;
}







lldb::PlatformSP GetSelectedPlatform() {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
if (!m_selected_platform_sp && !m_platforms.empty())
m_selected_platform_sp = m_platforms.front();

return m_selected_platform_sp;
}

void SetSelectedPlatform(const lldb::PlatformSP &platform_sp) {
if (platform_sp) {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
const size_t num_platforms = m_platforms.size();
for (size_t idx = 0; idx < num_platforms; ++idx) {
if (m_platforms[idx].get() == platform_sp.get()) {
m_selected_platform_sp = m_platforms[idx];
return;
}
}
m_platforms.push_back(platform_sp);
m_selected_platform_sp = m_platforms.back();
}
}

protected:
typedef std::vector<lldb::PlatformSP> collection;
mutable std::recursive_mutex m_mutex;
collection m_platforms;
lldb::PlatformSP m_selected_platform_sp;

private:
PlatformList(const PlatformList &) = delete;
const PlatformList &operator=(const PlatformList &) = delete;
};

class OptionGroupPlatformRSync : public lldb_private::OptionGroup {
public:
OptionGroupPlatformRSync() = default;

~OptionGroupPlatformRSync() override = default;

lldb_private::Status
SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override;



bool m_rsync;
std::string m_rsync_opts;
std::string m_rsync_prefix;
bool m_ignores_remote_hostname;

private:
OptionGroupPlatformRSync(const OptionGroupPlatformRSync &) = delete;
const OptionGroupPlatformRSync &
operator=(const OptionGroupPlatformRSync &) = delete;
};

class OptionGroupPlatformSSH : public lldb_private::OptionGroup {
public:
OptionGroupPlatformSSH() = default;

~OptionGroupPlatformSSH() override = default;

lldb_private::Status
SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override;



bool m_ssh;
std::string m_ssh_opts;

private:
OptionGroupPlatformSSH(const OptionGroupPlatformSSH &) = delete;
const OptionGroupPlatformSSH &
operator=(const OptionGroupPlatformSSH &) = delete;
};

class OptionGroupPlatformCaching : public lldb_private::OptionGroup {
public:
OptionGroupPlatformCaching() = default;

~OptionGroupPlatformCaching() override = default;

lldb_private::Status
SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override;



std::string m_cache_dir;

private:
OptionGroupPlatformCaching(const OptionGroupPlatformCaching &) = delete;
const OptionGroupPlatformCaching &
operator=(const OptionGroupPlatformCaching &) = delete;
};

}

#endif
