







#if !defined(LLDB_API_SBPROCESS_H)
#define LLDB_API_SBPROCESS_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBError.h"
#include "lldb/API/SBProcessInfo.h"
#include "lldb/API/SBQueue.h"
#include "lldb/API/SBTarget.h"
#include <cstdio>

namespace lldb {

class SBEvent;

class LLDB_API SBProcess {
public:

FLAGS_ANONYMOUS_ENUM(){eBroadcastBitStateChanged = (1 << 0),
eBroadcastBitInterrupt = (1 << 1),
eBroadcastBitSTDOUT = (1 << 2),
eBroadcastBitSTDERR = (1 << 3),
eBroadcastBitProfileData = (1 << 4),
eBroadcastBitStructuredData = (1 << 5)};

SBProcess();

SBProcess(const lldb::SBProcess &rhs);

const lldb::SBProcess &operator=(const lldb::SBProcess &rhs);

SBProcess(const lldb::ProcessSP &process_sp);

~SBProcess();

static const char *GetBroadcasterClassName();

const char *GetPluginName();


const char *GetShortPluginName();

void Clear();

explicit operator bool() const;

bool IsValid() const;

lldb::SBTarget GetTarget() const;

lldb::ByteOrder GetByteOrder() const;

size_t PutSTDIN(const char *src, size_t src_len);

size_t GetSTDOUT(char *dst, size_t dst_len) const;

size_t GetSTDERR(char *dst, size_t dst_len) const;

size_t GetAsyncProfileData(char *dst, size_t dst_len) const;

void ReportEventState(const lldb::SBEvent &event, FILE *out) const;

void ReportEventState(const lldb::SBEvent &event, SBFile file) const;

void ReportEventState(const lldb::SBEvent &event, FileSP file) const;

void AppendEventStateReport(const lldb::SBEvent &event,
lldb::SBCommandReturnObject &result);




bool RemoteAttachToProcessWithID(lldb::pid_t pid, lldb::SBError &error);

bool RemoteLaunch(char const **argv, char const **envp,
const char *stdin_path, const char *stdout_path,
const char *stderr_path, const char *working_directory,
uint32_t launch_flags, bool stop_at_entry,
lldb::SBError &error);


uint32_t GetNumThreads();

lldb::SBThread GetThreadAtIndex(size_t index);

lldb::SBThread GetThreadByID(lldb::tid_t sb_thread_id);

lldb::SBThread GetThreadByIndexID(uint32_t index_id);

lldb::SBThread GetSelectedThread() const;





lldb::SBThread CreateOSPluginThread(lldb::tid_t tid, lldb::addr_t context);

bool SetSelectedThread(const lldb::SBThread &thread);

bool SetSelectedThreadByID(lldb::tid_t tid);

bool SetSelectedThreadByIndexID(uint32_t index_id);


uint32_t GetNumQueues();

lldb::SBQueue GetQueueAtIndex(size_t index);



lldb::StateType GetState();

int GetExitStatus();

const char *GetExitDescription();













lldb::pid_t GetProcessID();












uint32_t GetUniqueID();

uint32_t GetAddressByteSize() const;

lldb::SBError Destroy();

lldb::SBError Continue();

lldb::SBError Stop();

lldb::SBError Kill();

lldb::SBError Detach();

lldb::SBError Detach(bool keep_stopped);

lldb::SBError Signal(int signal);

lldb::SBUnixSignals GetUnixSignals();

void SendAsyncInterrupt();

uint32_t GetStopID(bool include_expression_stops = false);











lldb::SBEvent GetStopEventForStopID(uint32_t stop_id);

size_t ReadMemory(addr_t addr, void *buf, size_t size, lldb::SBError &error);

size_t WriteMemory(addr_t addr, const void *buf, size_t size,
lldb::SBError &error);

size_t ReadCStringFromMemory(addr_t addr, void *buf, size_t size,
lldb::SBError &error);

uint64_t ReadUnsignedFromMemory(addr_t addr, uint32_t byte_size,
lldb::SBError &error);

lldb::addr_t ReadPointerFromMemory(addr_t addr, lldb::SBError &error);


static lldb::StateType GetStateFromEvent(const lldb::SBEvent &event);

static bool GetRestartedFromEvent(const lldb::SBEvent &event);

static size_t GetNumRestartedReasonsFromEvent(const lldb::SBEvent &event);

static const char *
GetRestartedReasonAtIndexFromEvent(const lldb::SBEvent &event, size_t idx);

static lldb::SBProcess GetProcessFromEvent(const lldb::SBEvent &event);

static bool GetInterruptedFromEvent(const lldb::SBEvent &event);

static lldb::SBStructuredData
GetStructuredDataFromEvent(const lldb::SBEvent &event);

static bool EventIsProcessEvent(const lldb::SBEvent &event);

static bool EventIsStructuredDataEvent(const lldb::SBEvent &event);

lldb::SBBroadcaster GetBroadcaster() const;

static const char *GetBroadcasterClass();

bool GetDescription(lldb::SBStream &description);

SBStructuredData GetExtendedCrashInformation();

uint32_t GetNumSupportedHardwareWatchpoints(lldb::SBError &error) const;
















uint32_t LoadImage(lldb::SBFileSpec &remote_image_spec, lldb::SBError &error);

























uint32_t LoadImage(const lldb::SBFileSpec &local_image_spec,
const lldb::SBFileSpec &remote_image_spec,
lldb::SBError &error);




























uint32_t LoadImageUsingPaths(const lldb::SBFileSpec &image_spec,
SBStringList &paths,
lldb::SBFileSpec &loaded_path,
lldb::SBError &error);

lldb::SBError UnloadImage(uint32_t image_token);

lldb::SBError SendEventData(const char *data);












uint32_t GetNumExtendedBacktraceTypes();












const char *GetExtendedBacktraceTypeAtIndex(uint32_t idx);

lldb::SBThreadCollection GetHistoryThreads(addr_t addr);

bool IsInstrumentationRuntimePresent(InstrumentationRuntimeType type);


lldb::SBError SaveCore(const char *file_name);















lldb::SBError GetMemoryRegionInfo(lldb::addr_t load_addr,
lldb::SBMemoryRegionInfo &region_info);





lldb::SBMemoryRegionInfoList GetMemoryRegions();






lldb::SBProcessInfo GetProcessInfo();






















lldb::addr_t AllocateMemory(size_t size, uint32_t permissions,
lldb::SBError &error);














lldb::SBError DeallocateMemory(lldb::addr_t ptr);

protected:
friend class SBAddress;
friend class SBBreakpoint;
friend class SBBreakpointLocation;
friend class SBCommandInterpreter;
friend class SBDebugger;
friend class SBExecutionContext;
friend class SBFunction;
friend class SBModule;
friend class SBTarget;
friend class SBThread;
friend class SBValue;
friend class lldb_private::QueueImpl;

lldb::ProcessSP GetSP() const;

void SetSP(const lldb::ProcessSP &process_sp);

lldb::ProcessWP m_opaque_wp;
};

}

#endif
