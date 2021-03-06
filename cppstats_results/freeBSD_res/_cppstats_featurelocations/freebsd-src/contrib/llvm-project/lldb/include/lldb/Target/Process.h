







#if !defined(LLDB_TARGET_PROCESS_H)
#define LLDB_TARGET_PROCESS_H

#include "lldb/Host/Config.h"

#include <climits>

#include <chrono>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#include "lldb/Breakpoint/BreakpointSiteList.h"
#include "lldb/Core/Communication.h"
#include "lldb/Core/LoadedModuleInfoList.h"
#include "lldb/Core/PluginInterface.h"
#include "lldb/Core/ThreadSafeValue.h"
#include "lldb/Core/UserSettingsController.h"
#include "lldb/Host/HostThread.h"
#include "lldb/Host/ProcessLaunchInfo.h"
#include "lldb/Host/ProcessRunLock.h"
#include "lldb/Symbol/ObjectFile.h"
#include "lldb/Target/ExecutionContextScope.h"
#include "lldb/Target/InstrumentationRuntime.h"
#include "lldb/Target/Memory.h"
#include "lldb/Target/MemoryTagManager.h"
#include "lldb/Target/QueueList.h"
#include "lldb/Target/ThreadList.h"
#include "lldb/Target/ThreadPlanStack.h"
#include "lldb/Target/Trace.h"
#include "lldb/Utility/ArchSpec.h"
#include "lldb/Utility/Broadcaster.h"
#include "lldb/Utility/Event.h"
#include "lldb/Utility/Listener.h"
#include "lldb/Utility/NameMatches.h"
#include "lldb/Utility/ProcessInfo.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/StructuredData.h"
#include "lldb/Utility/TraceGDBRemotePackets.h"
#include "lldb/Utility/UnimplementedError.h"
#include "lldb/Utility/UserIDResolver.h"
#include "lldb/lldb-private.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Threading.h"
#include "llvm/Support/VersionTuple.h"

namespace lldb_private {

template <typename B, typename S> struct Range;

class ProcessExperimentalProperties : public Properties {
public:
ProcessExperimentalProperties();
};

class ProcessProperties : public Properties {
public:


ProcessProperties(lldb_private::Process *process);

~ProcessProperties() override;

bool GetDisableMemoryCache() const;
uint64_t GetMemoryCacheLineSize() const;
Args GetExtraStartupCommands() const;
void SetExtraStartupCommands(const Args &args);
FileSpec GetPythonOSPluginPath() const;
uint32_t GetVirtualAddressableBits() const;
void SetVirtualAddressableBits(uint32_t bits);
void SetPythonOSPluginPath(const FileSpec &file);
bool GetIgnoreBreakpointsInExpressions() const;
void SetIgnoreBreakpointsInExpressions(bool ignore);
bool GetUnwindOnErrorInExpressions() const;
void SetUnwindOnErrorInExpressions(bool ignore);
bool GetStopOnSharedLibraryEvents() const;
void SetStopOnSharedLibraryEvents(bool stop);
bool GetDisableLangRuntimeUnwindPlans() const;
void SetDisableLangRuntimeUnwindPlans(bool disable);
bool GetDetachKeepsStopped() const;
void SetDetachKeepsStopped(bool keep_stopped);
bool GetWarningsOptimization() const;
bool GetWarningsUnsupportedLanguage() const;
bool GetStopOnExec() const;
std::chrono::seconds GetUtilityExpressionTimeout() const;
std::chrono::seconds GetInterruptTimeout() const;
bool GetOSPluginReportsAllThreads() const;
void SetOSPluginReportsAllThreads(bool does_report);
bool GetSteppingRunsAllThreads() const;

protected:
Process *m_process;
std::unique_ptr<ProcessExperimentalProperties> m_experimental_properties_up;
};

typedef std::shared_ptr<ProcessProperties> ProcessPropertiesSP;





class ProcessAttachInfo : public ProcessInstanceInfo {
public:
ProcessAttachInfo()
: ProcessInstanceInfo(), m_listener_sp(), m_hijack_listener_sp(),
m_plugin_name() {}

ProcessAttachInfo(const ProcessLaunchInfo &launch_info)
: ProcessInstanceInfo(), m_listener_sp(), m_hijack_listener_sp(),
m_plugin_name(), m_resume_count(0), m_wait_for_launch(false),
m_ignore_existing(true), m_continue_once_attached(false),
m_detach_on_error(true), m_async(false) {
ProcessInfo::operator=(launch_info);
SetProcessPluginName(launch_info.GetProcessPluginName());
SetResumeCount(launch_info.GetResumeCount());
SetListener(launch_info.GetListener());
SetHijackListener(launch_info.GetHijackListener());
m_detach_on_error = launch_info.GetDetachOnError();
}

bool GetWaitForLaunch() const { return m_wait_for_launch; }

void SetWaitForLaunch(bool b) { m_wait_for_launch = b; }

bool GetAsync() const { return m_async; }

void SetAsync(bool b) { m_async = b; }

bool GetIgnoreExisting() const { return m_ignore_existing; }

void SetIgnoreExisting(bool b) { m_ignore_existing = b; }

bool GetContinueOnceAttached() const { return m_continue_once_attached; }

void SetContinueOnceAttached(bool b) { m_continue_once_attached = b; }

uint32_t GetResumeCount() const { return m_resume_count; }

void SetResumeCount(uint32_t c) { m_resume_count = c; }

const char *GetProcessPluginName() const {
return (m_plugin_name.empty() ? nullptr : m_plugin_name.c_str());
}

void SetProcessPluginName(llvm::StringRef plugin) {
m_plugin_name = std::string(plugin);
}

void Clear() {
ProcessInstanceInfo::Clear();
m_plugin_name.clear();
m_resume_count = 0;
m_wait_for_launch = false;
m_ignore_existing = true;
m_continue_once_attached = false;
}

bool ProcessInfoSpecified() const {
if (GetExecutableFile())
return true;
if (GetProcessID() != LLDB_INVALID_PROCESS_ID)
return true;
if (GetParentProcessID() != LLDB_INVALID_PROCESS_ID)
return true;
return false;
}

lldb::ListenerSP GetHijackListener() const { return m_hijack_listener_sp; }

void SetHijackListener(const lldb::ListenerSP &listener_sp) {
m_hijack_listener_sp = listener_sp;
}

bool GetDetachOnError() const { return m_detach_on_error; }

void SetDetachOnError(bool enable) { m_detach_on_error = enable; }


lldb::ListenerSP GetListener() const { return m_listener_sp; }

void SetListener(const lldb::ListenerSP &listener_sp) {
m_listener_sp = listener_sp;
}

lldb::ListenerSP GetListenerForProcess(Debugger &debugger);

protected:
lldb::ListenerSP m_listener_sp;
lldb::ListenerSP m_hijack_listener_sp;
std::string m_plugin_name;
uint32_t m_resume_count = 0;
bool m_wait_for_launch = false;
bool m_ignore_existing = true;
bool m_continue_once_attached = false;


bool m_detach_on_error =
true;

bool m_async =
false;


};






class ProcessModID {
friend bool operator==(const ProcessModID &lhs, const ProcessModID &rhs);

public:
ProcessModID() = default;

ProcessModID(const ProcessModID &rhs)
: m_stop_id(rhs.m_stop_id), m_memory_id(rhs.m_memory_id) {}

const ProcessModID &operator=(const ProcessModID &rhs) {
if (this != &rhs) {
m_stop_id = rhs.m_stop_id;
m_memory_id = rhs.m_memory_id;
}
return *this;
}

~ProcessModID() = default;

void BumpStopID() {
m_stop_id++;
if (!IsLastResumeForUserExpression())
m_last_natural_stop_id++;
}

void BumpMemoryID() { m_memory_id++; }

void BumpResumeID() {
m_resume_id++;
if (m_running_user_expression > 0)
m_last_user_expression_resume = m_resume_id;
}

bool IsRunningUtilityFunction() const {
return m_running_utility_function > 0;
}

uint32_t GetStopID() const { return m_stop_id; }
uint32_t GetLastNaturalStopID() const { return m_last_natural_stop_id; }
uint32_t GetMemoryID() const { return m_memory_id; }
uint32_t GetResumeID() const { return m_resume_id; }
uint32_t GetLastUserExpressionResumeID() const {
return m_last_user_expression_resume;
}

bool MemoryIDEqual(const ProcessModID &compare) const {
return m_memory_id == compare.m_memory_id;
}

bool StopIDEqual(const ProcessModID &compare) const {
return m_stop_id == compare.m_stop_id;
}

void SetInvalid() { m_stop_id = UINT32_MAX; }

bool IsValid() const { return m_stop_id != UINT32_MAX; }

bool IsLastResumeForUserExpression() const {


if (m_resume_id == 0)
return false;

return m_resume_id == m_last_user_expression_resume;
}

void SetRunningUserExpression(bool on) {
if (on)
m_running_user_expression++;
else
m_running_user_expression--;
}

void SetRunningUtilityFunction(bool on) {
if (on)
m_running_utility_function++;
else {
assert(m_running_utility_function > 0 &&
"Called SetRunningUtilityFunction(false) without calling "
"SetRunningUtilityFunction(true) before?");
m_running_utility_function--;
}
}

void SetStopEventForLastNaturalStopID(lldb::EventSP event_sp) {
m_last_natural_stop_event = std::move(event_sp);
}

lldb::EventSP GetStopEventForStopID(uint32_t stop_id) const {
if (stop_id == m_last_natural_stop_id)
return m_last_natural_stop_event;
return lldb::EventSP();
}

private:
uint32_t m_stop_id = 0;
uint32_t m_last_natural_stop_id = 0;
uint32_t m_resume_id = 0;
uint32_t m_memory_id = 0;
uint32_t m_last_user_expression_resume = 0;
uint32_t m_running_user_expression = false;
uint32_t m_running_utility_function = 0;
lldb::EventSP m_last_natural_stop_event;
};

inline bool operator==(const ProcessModID &lhs, const ProcessModID &rhs) {
if (lhs.StopIDEqual(rhs) && lhs.MemoryIDEqual(rhs))
return true;
else
return false;
}

inline bool operator!=(const ProcessModID &lhs, const ProcessModID &rhs) {
return (!lhs.StopIDEqual(rhs) || !lhs.MemoryIDEqual(rhs));
}



class Process : public std::enable_shared_from_this<Process>,
public ProcessProperties,
public Broadcaster,
public ExecutionContextScope,
public PluginInterface {
friend class FunctionCaller;
friend class Debugger;
friend class DynamicLoader;
friend class ProcessEventData;
friend class StopInfo;
friend class Target;
friend class ThreadList;

public:

enum {
eBroadcastBitStateChanged = (1 << 0),
eBroadcastBitInterrupt = (1 << 1),
eBroadcastBitSTDOUT = (1 << 2),
eBroadcastBitSTDERR = (1 << 3),
eBroadcastBitProfileData = (1 << 4),
eBroadcastBitStructuredData = (1 << 5),
};

enum {
eBroadcastInternalStateControlStop = (1 << 0),
eBroadcastInternalStateControlPause = (1 << 1),
eBroadcastInternalStateControlResume = (1 << 2)
};


enum Warnings { eWarningsOptimization = 1, eWarningsUnsupportedLanguage = 2 };

typedef Range<lldb::addr_t, lldb::addr_t> LoadRange;






typedef ProcessRunLock::ProcessRunLocker StopLocker;



static ConstString &GetStaticBroadcasterClass();

ConstString &GetBroadcasterClass() const override {
return GetStaticBroadcasterClass();
}






typedef struct {
void *baton;
void (*initialize)(void *baton, Process *process);
void (*process_state_changed)(void *baton, Process *process,
lldb::StateType state);
} Notifications;

class ProcessEventData : public EventData {
friend class Process;

public:
ProcessEventData();
ProcessEventData(const lldb::ProcessSP &process, lldb::StateType state);

~ProcessEventData() override;

static ConstString GetFlavorString();

ConstString GetFlavor() const override;

lldb::ProcessSP GetProcessSP() const { return m_process_wp.lock(); }

lldb::StateType GetState() const { return m_state; }
bool GetRestarted() const { return m_restarted; }

size_t GetNumRestartedReasons() { return m_restarted_reasons.size(); }

const char *GetRestartedReasonAtIndex(size_t idx) {
return ((idx < m_restarted_reasons.size())
? m_restarted_reasons[idx].c_str()
: nullptr);
}

bool GetInterrupted() const { return m_interrupted; }

void Dump(Stream *s) const override;

virtual bool ShouldStop(Event *event_ptr, bool &found_valid_stopinfo);

void DoOnRemoval(Event *event_ptr) override;

static const Process::ProcessEventData *
GetEventDataFromEvent(const Event *event_ptr);

static lldb::ProcessSP GetProcessFromEvent(const Event *event_ptr);

static lldb::StateType GetStateFromEvent(const Event *event_ptr);

static bool GetRestartedFromEvent(const Event *event_ptr);

static size_t GetNumRestartedReasons(const Event *event_ptr);

static const char *GetRestartedReasonAtIndex(const Event *event_ptr,
size_t idx);

static void AddRestartedReason(Event *event_ptr, const char *reason);

static void SetRestartedInEvent(Event *event_ptr, bool new_value);

static bool GetInterruptedFromEvent(const Event *event_ptr);

static void SetInterruptedInEvent(Event *event_ptr, bool new_value);

static bool SetUpdateStateOnRemoval(Event *event_ptr);

private:
void SetUpdateStateOnRemoval() { m_update_state++; }

void SetRestarted(bool new_value) { m_restarted = new_value; }

void SetInterrupted(bool new_value) { m_interrupted = new_value; }

void AddRestartedReason(const char *reason) {
m_restarted_reasons.push_back(reason);
}

lldb::ProcessWP m_process_wp;
lldb::StateType m_state = lldb::eStateInvalid;
std::vector<std::string> m_restarted_reasons;
bool m_restarted = false;

int m_update_state = 0;
bool m_interrupted = false;

ProcessEventData(const ProcessEventData &) = delete;
const ProcessEventData &operator=(const ProcessEventData &) = delete;
};



Process(lldb::TargetSP target_sp, lldb::ListenerSP listener_sp);



Process(lldb::TargetSP target_sp, lldb::ListenerSP listener_sp,
const lldb::UnixSignalsSP &unix_signals_sp);





~Process() override;

static void SettingsInitialize();

static void SettingsTerminate();

static const ProcessPropertiesSP &GetGlobalProperties();









static lldb::ProcessSP FindPlugin(lldb::TargetSP target_sp,
llvm::StringRef plugin_name,
lldb::ListenerSP listener_sp,
const FileSpec *crash_file_path,
bool can_connect);











static bool
SetProcessExitStatus(lldb::pid_t pid,
bool exited,
int signo,
int status);

lldb::ByteOrder GetByteOrder() const;

uint32_t GetAddressByteSize() const;




lldb::pid_t GetID() const { return m_pid; }



void SetID(lldb::pid_t new_pid) { m_pid = new_pid; }

uint32_t GetUniqueID() const { return m_process_unique_id; }










virtual bool CanDebug(lldb::TargetSP target,
bool plugin_specified_by_name) = 0;





virtual void Finalize();






bool IsValid() const { return !m_finalizing; }













virtual CommandObject *GetPluginCommandObject() { return nullptr; }




















virtual Status Launch(ProcessLaunchInfo &launch_info);

virtual Status LoadCore();

virtual Status DoLoadCore() {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support loading core files.",
GetPluginName().GetCString());
return error;
}




























UtilityFunction *GetLoadImageUtilityFunction(
Platform *platform,
llvm::function_ref<std::unique_ptr<UtilityFunction>()> factory);








virtual DynamicLoader *GetDynamicLoader();







virtual DataExtractor GetAuxvData();









virtual llvm::Error LoadModules() {
return llvm::make_error<llvm::StringError>("Not implemented.",
llvm::inconvertibleErrorCode());
}




virtual llvm::Expected<LoadedModuleInfoList> GetLoadedModuleList() {
return llvm::createStringError(llvm::inconvertibleErrorCode(),
"Not implemented");
}

protected:
virtual JITLoaderList &GetJITLoaders();

public:





virtual SystemRuntime *GetSystemRuntime();















virtual Status Attach(ProcessAttachInfo &attach_info);








virtual Status ConnectRemote(llvm::StringRef remote_url);

bool GetShouldDetach() const { return m_should_detach; }

void SetShouldDetach(bool b) { m_should_detach = b; }





const std::vector<lldb::addr_t>& GetImageTokens() { return m_image_tokens; }













virtual lldb::addr_t GetImageInfoAddress();













virtual void WillPublicStop() {}











void RegisterNotificationCallbacks(const Process::Notifications &callbacks);















bool UnregisterNotificationCallbacks(const Process::Notifications &callbacks);





























Status Resume();

Status ResumeSynchronous(Stream *stream);


















Status Halt(bool clear_thread_plans = false, bool use_run_lock = true);










Status Detach(bool keep_stopped);
















Status Destroy(bool force_kill);







Status Signal(int signal);

void SetUnixSignals(lldb::UnixSignalsSP &&signals_sp);

const lldb::UnixSignalsSP &GetUnixSignals();











virtual Status WillAttachToProcessWithID(lldb::pid_t pid) { return Status(); }







virtual Status WillAttachToProcessWithName(const char *process_name,
bool wait_for_launch) {
return Status();
}








virtual Status DoConnectRemote(llvm::StringRef remote_url) {
Status error;
error.SetErrorString("remote connections are not supported");
return error;
}















virtual Status DoAttachToProcessWithID(lldb::pid_t pid,
const ProcessAttachInfo &attach_info) {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support attaching to a process by pid",
GetPluginName().GetCString());
return error;
}














virtual Status
DoAttachToProcessWithName(const char *process_name,
const ProcessAttachInfo &attach_info) {
Status error;
error.SetErrorString("attach by name is not supported");
return error;
}









virtual void DidAttach(ArchSpec &process_arch) { process_arch.Clear(); }







virtual void DidExec();



virtual void DoDidExec() {}







virtual Status WillLaunch(Module *module) { return Status(); }


















virtual Status DoLaunch(Module *exe_module, ProcessLaunchInfo &launch_info) {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support launching processes",
GetPluginName().GetCString());
return error;
}




virtual void DidLaunch() {}







virtual Status WillResume() { return Status(); }
















virtual Status DoResume() {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support resuming processes",
GetPluginName().GetCString());
return error;
}




virtual void DidResume() {}







virtual Status WillHalt() { return Status(); }
















virtual Status DoHalt(bool &caused_stop) {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support halting processes",
GetPluginName().GetCString());
return error;
}




virtual void DidHalt() {}








virtual Status WillDetach() { return Status(); }






virtual Status DoDetach(bool keep_stopped) {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support detaching from processes",
GetPluginName().GetCString());
return error;
}





virtual void DidDetach() {}

virtual bool DetachRequiresHalt() { return false; }










virtual Status WillSignal() { return Status(); }





virtual Status DoSignal(int signal) {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support sending signals to processes",
GetPluginName().GetCString());
return error;
}

virtual Status WillDestroy() { return Status(); }

virtual Status DoDestroy() = 0;

virtual void DidDestroy() {}

virtual bool DestroyRequiresHalt() { return true; }





virtual void DidSignal() {}











virtual void RefreshStateAfterStop() = 0;












virtual llvm::VersionTuple GetHostOSVersion() { return llvm::VersionTuple(); }


virtual llvm::VersionTuple GetHostMacCatalystVersion() { return {}; }






Target &GetTarget() { return *m_target_wp.lock(); }






const Target &GetTarget() const { return *m_target_wp.lock(); }









void Flush();







lldb::StateType GetState();

lldb::ExpressionResults
RunThreadPlan(ExecutionContext &exe_ctx, lldb::ThreadPlanSP &thread_plan_sp,
const EvaluateExpressionOptions &options,
DiagnosticManager &diagnostic_manager);

static const char *ExecutionResultAsCString(lldb::ExpressionResults result);

void GetStatus(Stream &ostrm);

size_t GetThreadStatus(Stream &ostrm, bool only_threads_with_stop_reason,
uint32_t start_frame, uint32_t num_frames,
uint32_t num_frames_with_source,
bool stop_format);

void SendAsyncInterrupt();





virtual void ModulesDidLoad(ModuleList &module_list);



























virtual lldb_private::StructuredData::ObjectSP
GetLoadedDynamicLibrariesInfos(lldb::addr_t image_list_address,
lldb::addr_t image_count) {
return StructuredData::ObjectSP();
}



virtual lldb_private::StructuredData::ObjectSP
GetLoadedDynamicLibrariesInfos() {
return StructuredData::ObjectSP();
}



virtual lldb_private::StructuredData::ObjectSP GetLoadedDynamicLibrariesInfos(
const std::vector<lldb::addr_t> &load_addresses) {
return StructuredData::ObjectSP();
}







virtual lldb_private::StructuredData::ObjectSP GetSharedCacheInfo() {
return StructuredData::ObjectSP();
}










void PrintWarningOptimization(const SymbolContext &sc);





void PrintWarningUnsupportedLanguage(const SymbolContext &sc);

virtual bool GetProcessInfo(ProcessInstanceInfo &info);






int GetExitStatus();






const char *GetExitDescription();

virtual void DidExit() {}

lldb::addr_t GetCodeAddressMask();
lldb::addr_t GetDataAddressMask();

void SetCodeAddressMask(lldb::addr_t code_address_mask) {
m_code_address_mask = code_address_mask;
}

void SetDataAddressMask(lldb::addr_t data_address_mask) {
m_data_address_mask = data_address_mask;
}





ProcessModID GetModID() const { return m_mod_id; }

const ProcessModID &GetModIDRef() const { return m_mod_id; }

uint32_t GetStopID() const { return m_mod_id.GetStopID(); }

uint32_t GetResumeID() const { return m_mod_id.GetResumeID(); }

uint32_t GetLastUserExpressionResumeID() const {
return m_mod_id.GetLastUserExpressionResumeID();
}

uint32_t GetLastNaturalStopID() const {
return m_mod_id.GetLastNaturalStopID();
}

lldb::EventSP GetStopEventForStopID(uint32_t stop_id) const {
return m_mod_id.GetStopEventForStopID(stop_id);
}














virtual bool SetExitStatus(int exit_status, const char *cstr);






virtual bool IsAlive();

virtual bool IsLiveDebugSession() const { return true; };










virtual bool WarnBeforeDetach() const { return true; }

































virtual size_t ReadMemory(lldb::addr_t vm_addr, void *buf, size_t size,
Status &error);





























size_t ReadMemoryFromInferior(lldb::addr_t vm_addr, void *buf, size_t size,
Status &error);




























size_t ReadStringFromMemory(lldb::addr_t vm_addr, char *str, size_t max_bytes,
Status &error, size_t type_width = 1);








size_t ReadCStringFromMemory(lldb::addr_t vm_addr, char *cstr,
size_t cstr_max_len, Status &error);

size_t ReadCStringFromMemory(lldb::addr_t vm_addr, std::string &out_str,
Status &error);


























uint64_t ReadUnsignedIntegerFromMemory(lldb::addr_t load_addr,
size_t byte_size, uint64_t fail_value,
Status &error);

int64_t ReadSignedIntegerFromMemory(lldb::addr_t load_addr, size_t byte_size,
int64_t fail_value, Status &error);

lldb::addr_t ReadPointerFromMemory(lldb::addr_t vm_addr, Status &error);

bool WritePointerToMemory(lldb::addr_t vm_addr, lldb::addr_t ptr_value,
Status &error);



















virtual size_t DoWriteMemory(lldb::addr_t vm_addr, const void *buf,
size_t size, Status &error) {
error.SetErrorStringWithFormat(
"error: %s does not support writing to processes",
GetPluginName().GetCString());
return 0;
}






























size_t WriteScalarToMemory(lldb::addr_t vm_addr, const Scalar &scalar,
size_t size, Status &error);

size_t ReadScalarIntegerFromMemory(lldb::addr_t addr, uint32_t byte_size,
bool is_signed, Scalar &scalar,
Status &error);

























size_t WriteMemory(lldb::addr_t vm_addr, const void *buf, size_t size,
Status &error);














virtual lldb::addr_t DoAllocateMemory(size_t size, uint32_t permissions,
Status &error) {
error.SetErrorStringWithFormat(
"error: %s does not support allocating in the debug process",
GetPluginName().GetCString());
return LLDB_INVALID_ADDRESS;
}

virtual Status WriteObjectFile(std::vector<ObjectFile::LoadableData> entries);






















lldb::addr_t AllocateMemory(size_t size, uint32_t permissions, Status &error);

























lldb::addr_t CallocateMemory(size_t size, uint32_t permissions,
Status &error);







llvm::Expected<const MemoryTagManager *> GetMemoryTagManager();


















llvm::Expected<std::vector<lldb::addr_t>> ReadMemoryTags(lldb::addr_t addr,
size_t len);





















Status WriteMemoryTags(lldb::addr_t addr, size_t len,
const std::vector<lldb::addr_t> &tags);












virtual lldb::addr_t ResolveIndirectFunction(const Address *address,
Status &error);

























virtual Status GetMemoryRegionInfo(lldb::addr_t load_addr,
MemoryRegionInfo &range_info) {
Status error;
error.SetErrorString("Process::GetMemoryRegionInfo() not supported");
return error;
}









virtual Status
GetMemoryRegions(lldb_private::MemoryRegionInfos &region_list);

virtual Status GetWatchpointSupportInfo(uint32_t &num) {
Status error;
num = 0;
error.SetErrorString("Process::GetWatchpointSupportInfo() not supported");
return error;
}

virtual Status GetWatchpointSupportInfo(uint32_t &num, bool &after) {
Status error;
num = 0;
after = true;
error.SetErrorString("Process::GetWatchpointSupportInfo() not supported");
return error;
}

lldb::ModuleSP ReadModuleFromMemory(const FileSpec &file_spec,
lldb::addr_t header_addr,
size_t size_to_read = 512);




















virtual bool GetLoadAddressPermissions(lldb::addr_t load_addr,
uint32_t &permissions);






bool CanJIT();





void SetCanJIT(bool can_jit);






bool CanInterpretFunctionCalls() { return m_can_interpret_function_calls; }






void SetCanInterpretFunctionCalls(bool can_interpret_function_calls) {
m_can_interpret_function_calls = can_interpret_function_calls;
}






void SetCanRunCode(bool can_run_code);












virtual Status DoDeallocateMemory(lldb::addr_t ptr) {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support deallocating in the debug process",
GetPluginName().GetCString());
return error;
}












Status DeallocateMemory(lldb::addr_t ptr);




























virtual size_t GetSTDOUT(char *buf, size_t buf_size, Status &error);




























virtual size_t GetSTDERR(char *buf, size_t buf_size, Status &error);



















virtual size_t PutSTDIN(const char *buf, size_t buf_size, Status &error) {
error.SetErrorString("stdin unsupported");
return 0;
}














virtual size_t GetAsyncProfileData(char *buf, size_t buf_size, Status &error);


size_t GetSoftwareBreakpointTrapOpcode(BreakpointSite *bp_site);

virtual Status EnableBreakpointSite(BreakpointSite *bp_site) {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support enabling breakpoints",
GetPluginName().GetCString());
return error;
}

virtual Status DisableBreakpointSite(BreakpointSite *bp_site) {
Status error;
error.SetErrorStringWithFormat(
"error: %s does not support disabling breakpoints",
GetPluginName().GetCString());
return error;
}





virtual Status EnableSoftwareBreakpoint(BreakpointSite *bp_site);





virtual Status DisableSoftwareBreakpoint(BreakpointSite *bp_site);

BreakpointSiteList &GetBreakpointSiteList();

const BreakpointSiteList &GetBreakpointSiteList() const;

void DisableAllBreakpointSites();

Status ClearBreakpointSiteByID(lldb::user_id_t break_id);

lldb::break_id_t CreateBreakpointSite(const lldb::BreakpointLocationSP &owner,
bool use_hardware);

Status DisableBreakpointSiteByID(lldb::user_id_t break_id);

Status EnableBreakpointSiteByID(lldb::user_id_t break_id);



void RemoveOwnerFromBreakpointSite(lldb::user_id_t owner_id,
lldb::user_id_t owner_loc_id,
lldb::BreakpointSiteSP &bp_site_sp);


virtual Status EnableWatchpoint(Watchpoint *wp, bool notify = true);

virtual Status DisableWatchpoint(Watchpoint *wp, bool notify = true);











bool UpdateThreadList(ThreadList &old_thread_list,
ThreadList &new_thread_list);

void UpdateThreadListIfNeeded();

ThreadList &GetThreadList() { return m_thread_list; }





ThreadList &GetExtendedThreadList() { return m_extended_thread_list; }

ThreadList::ThreadIterable Threads() { return m_thread_list.Threads(); }

uint32_t GetNextThreadIndexID(uint64_t thread_id);

lldb::ThreadSP CreateOSPluginThread(lldb::tid_t tid, lldb::addr_t context);


bool HasAssignedIndexIDToThread(uint64_t sb_thread_id);




uint32_t AssignIndexIDToThread(uint64_t thread_id);



void UpdateQueueListIfNeeded();

QueueList &GetQueueList() {
UpdateQueueListIfNeeded();
return m_queue_list;
}

QueueList::QueueIterable Queues() {
UpdateQueueListIfNeeded();
return m_queue_list.Queues();
}


lldb::StateType GetNextEvent(lldb::EventSP &event_sp);







lldb::StateType
WaitForProcessToStop(const Timeout<std::micro> &timeout,
lldb::EventSP *event_sp_ptr = nullptr,
bool wait_always = true,
lldb::ListenerSP hijack_listener = lldb::ListenerSP(),
Stream *stream = nullptr, bool use_run_lock = true);

uint32_t GetIOHandlerID() const { return m_iohandler_sync.GetValue(); }









void SyncIOHandler(uint32_t iohandler_id, const Timeout<std::micro> &timeout);

lldb::StateType GetStateChangedEvents(
lldb::EventSP &event_sp, const Timeout<std::micro> &timeout,
lldb::ListenerSP
hijack_listener);




















static bool HandleProcessStateChangedEvent(const lldb::EventSP &event_sp,
Stream *stream,
bool &pop_process_io_handler);

Event *PeekAtStateChangedEvents();

class ProcessEventHijacker {
public:
ProcessEventHijacker(Process &process, lldb::ListenerSP listener_sp)
: m_process(process) {
m_process.HijackProcessEvents(std::move(listener_sp));
}

~ProcessEventHijacker() { m_process.RestoreProcessEvents(); }

private:
Process &m_process;
};

friend class ProcessEventHijacker;
friend class ProcessProperties;













bool HijackProcessEvents(lldb::ListenerSP listener_sp);



void RestoreProcessEvents();

bool StateChangedIsHijackedForSynchronousResume();

bool StateChangedIsExternallyHijacked();

const lldb::ABISP &GetABI();

OperatingSystem *GetOperatingSystem() { return m_os_up.get(); }

std::vector<LanguageRuntime *> GetLanguageRuntimes();

LanguageRuntime *GetLanguageRuntime(lldb::LanguageType language);

bool IsPossibleDynamicValue(ValueObject &in_value);

bool IsRunning() const;

DynamicCheckerFunctions *GetDynamicCheckers() {
return m_dynamic_checkers_up.get();
}

void SetDynamicCheckers(DynamicCheckerFunctions *dynamic_checkers);








bool PruneThreadPlansForTID(lldb::tid_t tid);


void PruneThreadPlans();








ThreadPlanStack *FindThreadPlans(lldb::tid_t tid);
























bool DumpThreadPlansForTID(Stream &strm, lldb::tid_t tid,
lldb::DescriptionLevel desc_level, bool internal,
bool condense_trivial, bool skip_unreported_plans);


















void DumpThreadPlans(Stream &strm, lldb::DescriptionLevel desc_level,
bool internal, bool condense_trivial,
bool skip_unreported_plans);











virtual bool StartNoticingNewThreads() { return true; }






virtual bool StopNoticingNewThreads() { return true; }

void SetRunningUserExpression(bool on);
void SetRunningUtilityFunction(bool on);


lldb::TargetSP CalculateTarget() override;

lldb::ProcessSP CalculateProcess() override { return shared_from_this(); }

lldb::ThreadSP CalculateThread() override { return lldb::ThreadSP(); }

lldb::StackFrameSP CalculateStackFrame() override {
return lldb::StackFrameSP();
}

void CalculateExecutionContext(ExecutionContext &exe_ctx) override;

void SetSTDIOFileDescriptor(int file_descriptor);





void AddInvalidMemoryRegion(const LoadRange &region);



bool RemoveInvalidMemoryRange(const LoadRange &region);








typedef bool(PreResumeActionCallback)(void *);

void AddPreResumeAction(PreResumeActionCallback callback, void *baton);

bool RunPreResumeActions();

void ClearPreResumeActions();

void ClearPreResumeAction(PreResumeActionCallback callback, void *baton);

ProcessRunLock &GetRunLock();

bool CurrentThreadIsPrivateStateThread();

virtual Status SendEventData(const char *data) {
Status return_error("Sending an event is not supported for this process.");
return return_error;
}

lldb::ThreadCollectionSP GetHistoryThreads(lldb::addr_t addr);

lldb::InstrumentationRuntimeSP
GetInstrumentationRuntime(lldb::InstrumentationRuntimeType type);



















virtual bool GetModuleSpec(const FileSpec &module_file_spec,
const ArchSpec &arch, ModuleSpec &module_spec);

virtual void PrefetchModuleSpecs(llvm::ArrayRef<FileSpec> module_file_specs,
const llvm::Triple &triple) {}















virtual Status GetFileLoadAddress(const FileSpec &file, bool &is_loaded,
lldb::addr_t &load_addr) {
return Status("Not supported");
}

size_t AddImageToken(lldb::addr_t image_ptr);

lldb::addr_t GetImagePtrFromToken(size_t token) const;

void ResetImageToken(size_t token);






















Address AdvanceAddressToNextBranchInstruction(Address default_stop_addr,
AddressRange range_bounds);

























virtual Status
ConfigureStructuredData(ConstString type_name,
const StructuredData::ObjectSP &config_sp);














void BroadcastStructuredData(const StructuredData::ObjectSP &object_sp,
const lldb::StructuredDataPluginSP &plugin_sp);











lldb::StructuredDataPluginSP
GetStructuredDataPlugin(ConstString type_name) const;

protected:
friend class Trace;







virtual llvm::Expected<TraceSupportedResponse> TraceSupported();











virtual llvm::Error TraceStart(const llvm::json::Value &request) {
return llvm::make_error<UnimplementedError>();
}









virtual llvm::Error TraceStop(const TraceStopRequest &request) {
return llvm::make_error<UnimplementedError>();
}









virtual llvm::Expected<std::string> TraceGetState(llvm::StringRef type) {
return llvm::make_error<UnimplementedError>();
}









virtual llvm::Expected<std::vector<uint8_t>>
TraceGetBinaryData(const TraceGetBinaryDataRequest &request) {
return llvm::make_error<UnimplementedError>();
}


bool CallVoidArgVoidPtrReturn(const Address *address,
lldb::addr_t &returned_func,
bool trap_exceptions = false);







virtual bool DoUpdateThreadList(ThreadList &old_thread_list,
ThreadList &new_thread_list) = 0;



























virtual size_t DoReadMemory(lldb::addr_t vm_addr, void *buf, size_t size,
Status &error) = 0;

lldb::StateType GetPrivateState();






Status PrivateResume();


void CompleteAttach();



























void PrintWarning(uint64_t warning_type, const void *repeat_key,
const char *fmt, ...) __attribute__((format(printf, 4, 5)));









class NextEventAction {
public:
enum EventActionResult {
eEventActionSuccess,
eEventActionRetry,
eEventActionExit
};

NextEventAction(Process *process) : m_process(process) {}

virtual ~NextEventAction() = default;

virtual EventActionResult PerformAction(lldb::EventSP &event_sp) = 0;
virtual void HandleBeingUnshipped() {}
virtual EventActionResult HandleBeingInterrupted() = 0;
virtual const char *GetExitString() = 0;
void RequestResume() { m_process->m_resume_requested = true; }

protected:
Process *m_process;
};

void SetNextEventAction(Process::NextEventAction *next_event_action) {
if (m_next_event_action_up.get())
m_next_event_action_up->HandleBeingUnshipped();

m_next_event_action_up.reset(next_event_action);
}


class AttachCompletionHandler : public NextEventAction {
public:
AttachCompletionHandler(Process *process, uint32_t exec_count);

~AttachCompletionHandler() override = default;

EventActionResult PerformAction(lldb::EventSP &event_sp) override;
EventActionResult HandleBeingInterrupted() override;
const char *GetExitString() override;

private:
uint32_t m_exec_count;
std::string m_exit_string;
};

bool PrivateStateThreadIsValid() const {
lldb::StateType state = m_private_state.GetValue();
return state != lldb::eStateInvalid && state != lldb::eStateDetached &&
state != lldb::eStateExited && m_private_state_thread.IsJoinable();
}

void ForceNextEventDelivery() { m_force_next_event_delivery = true; }

















void MapSupportedStructuredDataPlugins(
const StructuredData::Array &supported_type_names);























bool RouteAsyncStructuredData(const StructuredData::ObjectSP object_sp);






virtual bool SupportsMemoryTagging() { return false; }

















virtual llvm::Expected<std::vector<uint8_t>>
DoReadMemoryTags(lldb::addr_t addr, size_t len, int32_t type) {
return llvm::createStringError(llvm::inconvertibleErrorCode(),
"%s does not support reading memory tags",
GetPluginName().GetCString());
}



















virtual Status DoWriteMemoryTags(lldb::addr_t addr, size_t len, int32_t type,
const std::vector<uint8_t> &tags) {
return Status("%s does not support writing memory tags",
GetPluginName().GetCString());
}


typedef std::map<lldb::LanguageType, lldb::LanguageRuntimeSP>
LanguageRuntimeCollection;
typedef std::unordered_set<const void *> WarningsPointerSet;
typedef std::map<uint64_t, WarningsPointerSet> WarningsCollection;

struct PreResumeCallbackAndBaton {
bool (*callback)(void *);
void *baton;
PreResumeCallbackAndBaton(PreResumeActionCallback in_callback,
void *in_baton)
: callback(in_callback), baton(in_baton) {}
bool operator== (const PreResumeCallbackAndBaton &rhs) {
return callback == rhs.callback && baton == rhs.baton;
}
};

using StructuredDataPluginMap =
std::map<ConstString, lldb::StructuredDataPluginSP>;


std::weak_ptr<Target> m_target_wp;
lldb::pid_t m_pid = LLDB_INVALID_PROCESS_ID;
ThreadSafeValue<lldb::StateType> m_public_state;
ThreadSafeValue<lldb::StateType>
m_private_state;
Broadcaster m_private_state_broadcaster;


Broadcaster m_private_state_control_broadcaster;



lldb::ListenerSP m_private_state_listener_sp;

HostThread m_private_state_thread;

ProcessModID m_mod_id;

uint32_t m_process_unique_id;


uint32_t m_thread_index_id;

std::map<uint64_t, uint32_t> m_thread_id_to_index_id_map;
int m_exit_status;
std::string m_exit_string;
std::mutex m_exit_status_mutex;

std::recursive_mutex m_thread_mutex;
ThreadList m_thread_list_real;

ThreadList m_thread_list;



ThreadPlanStackMap m_thread_plans;



ThreadList m_extended_thread_list;

uint32_t m_extended_thread_stop_id;

QueueList
m_queue_list;
uint32_t m_queue_list_stop_id;

std::vector<Notifications> m_notifications;

std::vector<lldb::addr_t> m_image_tokens;
lldb::ListenerSP m_listener_sp;

BreakpointSiteList m_breakpoint_site_list;


lldb::DynamicLoaderUP m_dyld_up;
lldb::JITLoaderListUP m_jit_loaders_up;
lldb::DynamicCheckerFunctionsUP m_dynamic_checkers_up;




lldb::OperatingSystemUP m_os_up;
lldb::SystemRuntimeUP m_system_runtime_up;
lldb::UnixSignalsSP
m_unix_signals_sp;
lldb::ABISP m_abi_sp;
lldb::IOHandlerSP m_process_input_reader;
Communication m_stdio_communication;
std::recursive_mutex m_stdio_communication_mutex;
bool m_stdin_forward;

std::string m_stdout_data;
std::string m_stderr_data;
std::recursive_mutex m_profile_data_comm_mutex;
std::vector<std::string> m_profile_data;
Predicate<uint32_t> m_iohandler_sync;
MemoryCache m_memory_cache;
AllocatedMemoryCache m_allocated_memory_cache;
bool m_should_detach;

LanguageRuntimeCollection m_language_runtimes;
std::recursive_mutex m_language_runtimes_mutex;
InstrumentationRuntimeCollection m_instrumentation_runtimes;
std::unique_ptr<NextEventAction> m_next_event_action_up;
std::vector<PreResumeCallbackAndBaton> m_pre_resume_actions;
ProcessRunLock m_public_run_lock;
ProcessRunLock m_private_run_lock;
bool m_currently_handling_do_on_removals;
bool m_resume_requested;






std::atomic<bool> m_finalizing;





lldb::addr_t m_code_address_mask = 0;
lldb::addr_t m_data_address_mask = 0;


bool m_clear_thread_plans_on_stop;
bool m_force_next_event_delivery;
lldb::StateType m_last_broadcast_state;


std::map<lldb::addr_t, lldb::addr_t> m_resolved_indirect_addresses;
bool m_destroy_in_process;
bool m_can_interpret_function_calls;


WarningsCollection m_warnings_issued;

std::mutex m_run_thread_plan_lock;
StructuredDataPluginMap m_structured_data_plugin_map;

enum { eCanJITDontKnow = 0, eCanJITYes, eCanJITNo } m_can_jit;

std::unique_ptr<UtilityFunction> m_dlopen_utility_func_up;
llvm::once_flag m_dlopen_utility_func_flag_once;

size_t RemoveBreakpointOpcodesFromBuffer(lldb::addr_t addr, size_t size,
uint8_t *buf) const;

void SynchronouslyNotifyStateChanged(lldb::StateType state);

void SetPublicState(lldb::StateType new_state, bool restarted);

void SetPrivateState(lldb::StateType state);

bool StartPrivateStateThread(bool is_secondary_thread = false);

void StopPrivateStateThread();

void PausePrivateStateThread();

void ResumePrivateStateThread();

private:
struct PrivateStateThreadArgs {
PrivateStateThreadArgs(Process *p, bool s)
: process(p), is_secondary_thread(s){};
Process *process;
bool is_secondary_thread;
};



static lldb::thread_result_t PrivateStateThread(void *arg);






lldb::thread_result_t RunPrivateStateThread(bool is_secondary_thread);

protected:
void HandlePrivateEvent(lldb::EventSP &event_sp);

Status HaltPrivate();

lldb::StateType WaitForProcessStopPrivate(lldb::EventSP &event_sp,
const Timeout<std::micro> &timeout);




bool GetEventsPrivate(lldb::EventSP &event_sp,
const Timeout<std::micro> &timeout, bool control_only);

lldb::StateType
GetStateChangedEventsPrivate(lldb::EventSP &event_sp,
const Timeout<std::micro> &timeout);

size_t WriteMemoryPrivate(lldb::addr_t addr, const void *buf, size_t size,
Status &error);

void AppendSTDOUT(const char *s, size_t len);

void AppendSTDERR(const char *s, size_t len);

void BroadcastAsyncProfileData(const std::string &one_profile_data);

static void STDIOReadThreadBytesReceived(void *baton, const void *src,
size_t src_len);

bool PushProcessIOHandler();

bool PopProcessIOHandler();

bool ProcessIOHandlerIsActive();

bool ProcessIOHandlerExists() const {
return static_cast<bool>(m_process_input_reader);
}

Status StopForDestroyOrDetach(lldb::EventSP &exit_event_sp);

virtual Status UpdateAutomaticSignalFiltering();

void LoadOperatingSystemPlugin(bool flush);

private:
Status DestroyImpl(bool force_kill);














bool ShouldBroadcastEvent(Event *event_ptr);

void ControlPrivateStateThread(uint32_t signal);

Process(const Process &) = delete;
const Process &operator=(const Process &) = delete;
};



class UtilityFunctionScope {
Process *m_process;

public:
UtilityFunctionScope(Process *p) : m_process(p) {
if (m_process)
m_process->SetRunningUtilityFunction(true);
}
~UtilityFunctionScope() {
if (m_process)
m_process->SetRunningUtilityFunction(false);
}
};

}

#endif
