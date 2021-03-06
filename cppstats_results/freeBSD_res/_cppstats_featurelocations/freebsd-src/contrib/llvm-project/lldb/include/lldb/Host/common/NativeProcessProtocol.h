







#if !defined(LLDB_HOST_COMMON_NATIVEPROCESSPROTOCOL_H)
#define LLDB_HOST_COMMON_NATIVEPROCESSPROTOCOL_H

#include "NativeBreakpointList.h"
#include "NativeThreadProtocol.h"
#include "NativeWatchpointList.h"
#include "lldb/Host/Host.h"
#include "lldb/Host/MainLoop.h"
#include "lldb/Utility/ArchSpec.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/TraceGDBRemotePackets.h"
#include "lldb/Utility/UnimplementedError.h"
#include "lldb/lldb-private-forward.h"
#include "lldb/lldb-types.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
#include <mutex>
#include <unordered_map>
#include <vector>

namespace lldb_private {
LLVM_ENABLE_BITMASK_ENUMS_IN_NAMESPACE();

class MemoryRegionInfo;
class ResumeActionList;

struct SVR4LibraryInfo {
std::string name;
lldb::addr_t link_map;
lldb::addr_t base_addr;
lldb::addr_t ld_addr;
lldb::addr_t next;
};


class NativeProcessProtocol {
public:
virtual ~NativeProcessProtocol() = default;

virtual Status Resume(const ResumeActionList &resume_actions) = 0;

virtual Status Halt() = 0;

virtual Status Detach() = 0;





virtual Status Signal(int signo) = 0;









virtual Status Interrupt();

virtual Status Kill() = 0;



virtual Status IgnoreSignals(llvm::ArrayRef<int> signals);



virtual Status GetMemoryRegionInfo(lldb::addr_t load_addr,
MemoryRegionInfo &range_info);

virtual Status ReadMemory(lldb::addr_t addr, void *buf, size_t size,
size_t &bytes_read) = 0;

Status ReadMemoryWithoutTrap(lldb::addr_t addr, void *buf, size_t size,
size_t &bytes_read);

virtual Status ReadMemoryTags(int32_t type, lldb::addr_t addr, size_t len,
std::vector<uint8_t> &tags);

virtual Status WriteMemoryTags(int32_t type, lldb::addr_t addr, size_t len,
const std::vector<uint8_t> &tags);






















llvm::Expected<llvm::StringRef>
ReadCStringFromMemory(lldb::addr_t addr, char *buffer, size_t max_size,
size_t &total_bytes_read);

virtual Status WriteMemory(lldb::addr_t addr, const void *buf, size_t size,
size_t &bytes_written) = 0;

virtual llvm::Expected<lldb::addr_t> AllocateMemory(size_t size,
uint32_t permissions) {
return llvm::make_error<UnimplementedError>();
}

virtual llvm::Error DeallocateMemory(lldb::addr_t addr) {
return llvm::make_error<UnimplementedError>();
}

virtual lldb::addr_t GetSharedLibraryInfoAddress() = 0;

virtual llvm::Expected<std::vector<SVR4LibraryInfo>>
GetLoadedSVR4Libraries() {
return llvm::createStringError(llvm::inconvertibleErrorCode(),
"Not implemented");
}

virtual bool IsAlive() const;

virtual size_t UpdateThreads() = 0;

virtual const ArchSpec &GetArchitecture() const = 0;


virtual Status SetBreakpoint(lldb::addr_t addr, uint32_t size,
bool hardware) = 0;

virtual Status RemoveBreakpoint(lldb::addr_t addr, bool hardware = false);


virtual const HardwareBreakpointMap &GetHardwareBreakpointMap() const;

virtual Status SetHardwareBreakpoint(lldb::addr_t addr, size_t size);

virtual Status RemoveHardwareBreakpoint(lldb::addr_t addr);


virtual const NativeWatchpointList::WatchpointMap &GetWatchpointMap() const;

virtual llvm::Optional<std::pair<uint32_t, uint32_t>>
GetHardwareDebugSupportInfo() const;

virtual Status SetWatchpoint(lldb::addr_t addr, size_t size,
uint32_t watch_flags, bool hardware);

virtual Status RemoveWatchpoint(lldb::addr_t addr);


lldb::pid_t GetID() const { return m_pid; }

lldb::StateType GetState() const;

bool IsRunning() const {
return m_state == lldb::eStateRunning || IsStepping();
}

bool IsStepping() const { return m_state == lldb::eStateStepping; }

bool CanResume() const { return m_state == lldb::eStateStopped; }

lldb::ByteOrder GetByteOrder() const {
return GetArchitecture().GetByteOrder();
}

uint32_t GetAddressByteSize() const {
return GetArchitecture().GetAddressByteSize();
}

virtual llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
GetAuxvData() const = 0;


virtual llvm::Optional<WaitStatus> GetExitStatus();

virtual bool SetExitStatus(WaitStatus status, bool bNotifyStateChange);


NativeThreadProtocol *GetThreadAtIndex(uint32_t idx);

NativeThreadProtocol *GetThreadByID(lldb::tid_t tid);

void SetCurrentThreadID(lldb::tid_t tid) { m_current_thread_id = tid; }

lldb::tid_t GetCurrentThreadID() { return m_current_thread_id; }

NativeThreadProtocol *GetCurrentThread() {
return GetThreadByID(m_current_thread_id);
}


virtual int GetTerminalFileDescriptor() { return m_terminal_fd; }



uint32_t GetStopID() const;


class NativeDelegate {
public:
virtual ~NativeDelegate() = default;

virtual void InitializeDelegate(NativeProcessProtocol *process) = 0;

virtual void ProcessStateChanged(NativeProcessProtocol *process,
lldb::StateType state) = 0;

virtual void DidExec(NativeProcessProtocol *process) = 0;

virtual void
NewSubprocess(NativeProcessProtocol *parent_process,
std::unique_ptr<NativeProcessProtocol> child_process) = 0;
};

virtual Status GetLoadedModuleFileSpec(const char *module_path,
FileSpec &file_spec) = 0;

virtual Status GetFileLoadAddress(const llvm::StringRef &file_name,
lldb::addr_t &load_addr) = 0;



enum class Extension {
multiprocess = (1u << 0),
fork = (1u << 1),
vfork = (1u << 2),
pass_signals = (1u << 3),
auxv = (1u << 4),
libraries_svr4 = (1u << 5),
memory_tagging = (1u << 6),

LLVM_MARK_AS_BITMASK_ENUM(memory_tagging)
};

class Factory {
public:
virtual ~Factory();


















virtual llvm::Expected<std::unique_ptr<NativeProcessProtocol>>
Launch(ProcessLaunchInfo &launch_info, NativeDelegate &native_delegate,
MainLoop &mainloop) const = 0;



















virtual llvm::Expected<std::unique_ptr<NativeProcessProtocol>>
Attach(lldb::pid_t pid, NativeDelegate &native_delegate,
MainLoop &mainloop) const = 0;





virtual Extension GetSupportedExtensions() const { return {}; }
};

















virtual llvm::Error TraceStart(llvm::StringRef json_params,
llvm::StringRef type) {
return llvm::createStringError(llvm::inconvertibleErrorCode(),
"Unsupported tracing type '%s'",
type.data());
}


virtual llvm::Error TraceStop(const TraceStopRequest &request) {
return llvm::createStringError(llvm::inconvertibleErrorCode(),
"Unsupported tracing type '%s'",
request.type.data());
}


virtual llvm::Expected<llvm::json::Value>
TraceGetState(llvm::StringRef type) {
return llvm::createStringError(llvm::inconvertibleErrorCode(),
"Unsupported tracing type '%s'",
type.data());
}


virtual llvm::Expected<std::vector<uint8_t>>
TraceGetBinaryData(const TraceGetBinaryDataRequest &request) {
return llvm::createStringError(
llvm::inconvertibleErrorCode(),
"Unsupported data kind '%s' for the '%s' tracing technology",
request.kind.c_str(), request.type.c_str());
}


virtual llvm::Expected<TraceSupportedResponse> TraceSupported() {
return llvm::make_error<UnimplementedError>();
}






virtual void SetEnabledExtensions(Extension flags) {
m_enabled_extensions = flags;
}

protected:
struct SoftwareBreakpoint {
uint32_t ref_count;
llvm::SmallVector<uint8_t, 4> saved_opcodes;
llvm::ArrayRef<uint8_t> breakpoint_opcodes;
};

std::unordered_map<lldb::addr_t, SoftwareBreakpoint> m_software_breakpoints;
lldb::pid_t m_pid;

std::vector<std::unique_ptr<NativeThreadProtocol>> m_threads;
lldb::tid_t m_current_thread_id = LLDB_INVALID_THREAD_ID;
mutable std::recursive_mutex m_threads_mutex;

lldb::StateType m_state = lldb::eStateInvalid;
mutable std::recursive_mutex m_state_mutex;

llvm::Optional<WaitStatus> m_exit_status;

NativeDelegate &m_delegate;
NativeWatchpointList m_watchpoint_list;
HardwareBreakpointMap m_hw_breakpoints_map;
int m_terminal_fd;
uint32_t m_stop_id = 0;



llvm::DenseSet<int> m_signals_to_ignore;


Extension m_enabled_extensions;





NativeProcessProtocol(lldb::pid_t pid, int terminal_fd,
NativeDelegate &delegate);

void SetID(lldb::pid_t pid) { m_pid = pid; }


void SetState(lldb::StateType state, bool notify_delegates = true);





virtual void DoStopIDBumped(uint32_t newBumpId);



Status SetSoftwareBreakpoint(lldb::addr_t addr, uint32_t size_hint);
Status RemoveSoftwareBreakpoint(lldb::addr_t addr);

virtual llvm::Expected<llvm::ArrayRef<uint8_t>>
GetSoftwareBreakpointTrapOpcode(size_t size_hint);





virtual size_t GetSoftwareBreakpointPCOffset();




void FixupBreakpointPCAsNeeded(NativeThreadProtocol &thread);





void NotifyDidExec();

NativeThreadProtocol *GetThreadByIDUnlocked(lldb::tid_t tid);

private:
void SynchronouslyNotifyProcessStateChanged(lldb::StateType state);
llvm::Expected<SoftwareBreakpoint>
EnableSoftwareBreakpoint(lldb::addr_t addr, uint32_t size_hint);
};
}

#endif
