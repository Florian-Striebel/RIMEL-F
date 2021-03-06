







#if !defined(LLDB_TARGET_THREAD_H)
#define LLDB_TARGET_THREAD_H

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "lldb/Core/UserSettingsController.h"
#include "lldb/Target/ExecutionContextScope.h"
#include "lldb/Target/RegisterCheckpoint.h"
#include "lldb/Target/StackFrameList.h"
#include "lldb/Utility/Broadcaster.h"
#include "lldb/Utility/CompletionRequest.h"
#include "lldb/Utility/Event.h"
#include "lldb/Utility/StructuredData.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"

#define LLDB_THREAD_MAX_STOP_EXC_DATA 8

namespace lldb_private {

class ThreadPlanStack;

class ThreadProperties : public Properties {
public:
ThreadProperties(bool is_global);

~ThreadProperties() override;








const RegularExpression *GetSymbolsToAvoidRegexp();

FileSpecList GetLibrariesToAvoid() const;

bool GetTraceEnabledState() const;

bool GetStepInAvoidsNoDebug() const;

bool GetStepOutAvoidsNoDebug() const;

uint64_t GetMaxBacktraceDepth() const;
};

typedef std::shared_ptr<ThreadProperties> ThreadPropertiesSP;

class Thread : public std::enable_shared_from_this<Thread>,
public ThreadProperties,
public UserID,
public ExecutionContextScope,
public Broadcaster {
public:

enum {
eBroadcastBitStackChanged = (1 << 0),
eBroadcastBitThreadSuspended = (1 << 1),
eBroadcastBitThreadResumed = (1 << 2),
eBroadcastBitSelectedFrameChanged = (1 << 3),
eBroadcastBitThreadSelected = (1 << 4)
};

static ConstString &GetStaticBroadcasterClass();

ConstString &GetBroadcasterClass() const override {
return GetStaticBroadcasterClass();
}

class ThreadEventData : public EventData {
public:
ThreadEventData(const lldb::ThreadSP thread_sp);

ThreadEventData(const lldb::ThreadSP thread_sp, const StackID &stack_id);

ThreadEventData();

~ThreadEventData() override;

static ConstString GetFlavorString();

ConstString GetFlavor() const override {
return ThreadEventData::GetFlavorString();
}

void Dump(Stream *s) const override;

static const ThreadEventData *GetEventDataFromEvent(const Event *event_ptr);

static lldb::ThreadSP GetThreadFromEvent(const Event *event_ptr);

static StackID GetStackIDFromEvent(const Event *event_ptr);

static lldb::StackFrameSP GetStackFrameFromEvent(const Event *event_ptr);

lldb::ThreadSP GetThread() const { return m_thread_sp; }

StackID GetStackID() const { return m_stack_id; }

private:
lldb::ThreadSP m_thread_sp;
StackID m_stack_id;

ThreadEventData(const ThreadEventData &) = delete;
const ThreadEventData &operator=(const ThreadEventData &) = delete;
};

struct ThreadStateCheckpoint {
uint32_t orig_stop_id;

lldb::StopInfoSP stop_info_sp;

size_t m_completed_plan_checkpoint;
lldb::RegisterCheckpointSP
register_backup_sp;
uint32_t current_inlined_depth;
lldb::addr_t current_inlined_pc;
};












Thread(Process &process, lldb::tid_t tid, bool use_invalid_index_id = false);

~Thread() override;

static void SettingsInitialize();

static void SettingsTerminate();

static const ThreadPropertiesSP &GetGlobalProperties();

lldb::ProcessSP GetProcess() const { return m_process_wp.lock(); }

int GetResumeSignal() const { return m_resume_signal; }

void SetResumeSignal(int signal) { m_resume_signal = signal; }

lldb::StateType GetState() const;

void SetState(lldb::StateType state);





















void SetResumeState(lldb::StateType state, bool override_suspend = false) {
if (m_resume_state == lldb::eStateSuspended && !override_suspend)
return;
m_resume_state = state;
}









lldb::StateType GetResumeState() const { return m_resume_state; }





void SetupForResume();


bool ShouldResume(lldb::StateType resume_state);


virtual void WillResume(lldb::StateType resume_state) {}



virtual void DidResume();


virtual void DidStop();

virtual void RefreshStateAfterStop() = 0;

void SelectMostRelevantFrame();

std::string GetStopDescription();

std::string GetStopDescriptionRaw();

void WillStop();

bool ShouldStop(Event *event_ptr);

Vote ShouldReportStop(Event *event_ptr);

Vote ShouldReportRun(Event *event_ptr);

void Flush();






virtual bool MatchesSpec(const ThreadSpec *spec);

lldb::StopInfoSP GetStopInfo();

lldb::StopReason GetStopReason();

bool StopInfoIsUpToDate() const;




void SetStopInfoToNothing();

bool ThreadStoppedForAReason();

static std::string RunModeAsString(lldb::RunMode mode);

static std::string StopReasonAsString(lldb::StopReason reason);

virtual const char *GetInfo() { return nullptr; }











StructuredData::ObjectSP GetExtendedInfo() {
if (!m_extended_info_fetched) {
m_extended_info = FetchThreadExtendedInfo();
m_extended_info_fetched = true;
}
return m_extended_info;
}

virtual const char *GetName() { return nullptr; }

virtual void SetName(const char *name) {}

















virtual lldb_private::LazyBool GetAssociatedWithLibdispatchQueue() {
return eLazyBoolNo;
}

virtual void SetAssociatedWithLibdispatchQueue(
lldb_private::LazyBool associated_with_libdispatch_queue) {}














virtual lldb::queue_id_t GetQueueID() { return LLDB_INVALID_QUEUE_ID; }

virtual void SetQueueID(lldb::queue_id_t new_val) {}









virtual const char *GetQueueName() { return nullptr; }

virtual void SetQueueName(const char *name) {}











virtual lldb::QueueKind GetQueueKind() { return lldb::eQueueKindUnknown; }

virtual void SetQueueKind(lldb::QueueKind kind) {}









virtual lldb::QueueSP GetQueue() { return lldb::QueueSP(); }















virtual lldb::addr_t GetQueueLibdispatchQueueAddress() {
return LLDB_INVALID_ADDRESS;
}

virtual void SetQueueLibdispatchQueueAddress(lldb::addr_t dispatch_queue_t) {}













virtual bool ThreadHasQueueInformation() const { return false; }

virtual uint32_t GetStackFrameCount() {
return GetStackFrameList()->GetNumFrames();
}

virtual lldb::StackFrameSP GetStackFrameAtIndex(uint32_t idx) {
return GetStackFrameList()->GetFrameAtIndex(idx);
}

virtual lldb::StackFrameSP
GetFrameWithConcreteFrameIndex(uint32_t unwind_idx);

bool DecrementCurrentInlinedDepth() {
return GetStackFrameList()->DecrementCurrentInlinedDepth();
}

uint32_t GetCurrentInlinedDepth() {
return GetStackFrameList()->GetCurrentInlinedDepth();
}

Status ReturnFromFrameWithIndex(uint32_t frame_idx,
lldb::ValueObjectSP return_value_sp,
bool broadcast = false);

Status ReturnFromFrame(lldb::StackFrameSP frame_sp,
lldb::ValueObjectSP return_value_sp,
bool broadcast = false);

Status JumpToLine(const FileSpec &file, uint32_t line,
bool can_leave_function, std::string *warnings = nullptr);

virtual lldb::StackFrameSP GetFrameWithStackID(const StackID &stack_id) {
if (stack_id.IsValid())
return GetStackFrameList()->GetFrameWithStackID(stack_id);
return lldb::StackFrameSP();
}

uint32_t GetSelectedFrameIndex() {
return GetStackFrameList()->GetSelectedFrameIndex();
}

lldb::StackFrameSP GetSelectedFrame();

uint32_t SetSelectedFrame(lldb_private::StackFrame *frame,
bool broadcast = false);

bool SetSelectedFrameByIndex(uint32_t frame_idx, bool broadcast = false);

bool SetSelectedFrameByIndexNoisily(uint32_t frame_idx,
Stream &output_stream);

void SetDefaultFileAndLineToSelectedFrame() {
GetStackFrameList()->SetDefaultFileAndLineToSelectedFrame();
}

virtual lldb::RegisterContextSP GetRegisterContext() = 0;

virtual lldb::RegisterContextSP
CreateRegisterContextForFrame(StackFrame *frame) = 0;

virtual void ClearStackFrames();

virtual bool SetBackingThread(const lldb::ThreadSP &thread_sp) {
return false;
}

virtual lldb::ThreadSP GetBackingThread() const { return lldb::ThreadSP(); }

virtual void ClearBackingThread() {







}
















void DumpTraceInstructions(Stream &s, size_t count,
size_t start_position = 0) const;



void DumpUsingSettingsFormat(Stream &strm, uint32_t frame_idx,
bool stop_format);

bool GetDescription(Stream &s, lldb::DescriptionLevel level,
bool print_json_thread, bool print_json_stopinfo);





















virtual Status
StepIn(bool source_step,
LazyBool step_in_avoids_code_without_debug_info = eLazyBoolCalculate,
LazyBool step_out_avoids_code_without_debug_info = eLazyBoolCalculate);












virtual Status StepOver(
bool source_step,
LazyBool step_out_avoids_code_without_debug_info = eLazyBoolCalculate);








virtual Status StepOut();








virtual lldb::addr_t GetThreadPointer();












virtual lldb::addr_t GetThreadLocalData(const lldb::ModuleSP module,
lldb::addr_t tls_file_addr);










virtual bool SafeToCallFunctions();






































lldb::ThreadPlanSP QueueBasePlan(bool abort_other_plans);





















virtual lldb::ThreadPlanSP QueueThreadPlanForStepSingleInstruction(
bool step_over, bool abort_other_plans, bool stop_other_threads,
Status &status);







































virtual lldb::ThreadPlanSP QueueThreadPlanForStepOverRange(
bool abort_other_plans, const AddressRange &range,
const SymbolContext &addr_context, lldb::RunMode stop_other_threads,
Status &status,
LazyBool step_out_avoids_code_without_debug_info = eLazyBoolCalculate);




virtual lldb::ThreadPlanSP QueueThreadPlanForStepOverRange(
bool abort_other_plans, const LineEntry &line_entry,
const SymbolContext &addr_context, lldb::RunMode stop_other_threads,
Status &status,
LazyBool step_out_avoids_code_without_debug_info = eLazyBoolCalculate);
















































virtual lldb::ThreadPlanSP QueueThreadPlanForStepInRange(
bool abort_other_plans, const AddressRange &range,
const SymbolContext &addr_context, const char *step_in_target,
lldb::RunMode stop_other_threads, Status &status,
LazyBool step_in_avoids_code_without_debug_info = eLazyBoolCalculate,
LazyBool step_out_avoids_code_without_debug_info = eLazyBoolCalculate);




virtual lldb::ThreadPlanSP QueueThreadPlanForStepInRange(
bool abort_other_plans, const LineEntry &line_entry,
const SymbolContext &addr_context, const char *step_in_target,
lldb::RunMode stop_other_threads, Status &status,
LazyBool step_in_avoids_code_without_debug_info = eLazyBoolCalculate,
LazyBool step_out_avoids_code_without_debug_info = eLazyBoolCalculate);









































virtual lldb::ThreadPlanSP QueueThreadPlanForStepOut(
bool abort_other_plans, SymbolContext *addr_context, bool first_insn,
bool stop_other_threads, Vote report_stop_vote, Vote report_run_vote,
uint32_t frame_idx, Status &status,
LazyBool step_out_avoids_code_without_debug_info = eLazyBoolCalculate);



























































virtual lldb::ThreadPlanSP QueueThreadPlanForStepOutNoShouldStop(
bool abort_other_plans, SymbolContext *addr_context, bool first_insn,
bool stop_other_threads, Vote report_stop_vote, Vote report_run_vote,
uint32_t frame_idx, Status &status, bool continue_to_next_branch = false);























virtual lldb::ThreadPlanSP
QueueThreadPlanForStepThrough(StackID &return_stack_id,
bool abort_other_plans, bool stop_other_threads,
Status &status);






















virtual lldb::ThreadPlanSP
QueueThreadPlanForRunToAddress(bool abort_other_plans, Address &target_addr,
bool stop_other_threads, Status &status);

virtual lldb::ThreadPlanSP QueueThreadPlanForStepUntil(
bool abort_other_plans, lldb::addr_t *address_list, size_t num_addresses,
bool stop_others, uint32_t frame_idx, Status &status);

virtual lldb::ThreadPlanSP
QueueThreadPlanForStepScripted(bool abort_other_plans, const char *class_name,
StructuredData::ObjectSP extra_args_sp,
bool stop_other_threads, Status &status);







void AutoCompleteThreadPlans(CompletionRequest &request) const;





ThreadPlan *GetCurrentPlan() const;







Status UnwindInnermostExpression();






lldb::ThreadPlanSP GetCompletedPlan() const;






lldb::ValueObjectSP GetReturnValueObject() const;







lldb::ExpressionVariableSP GetExpressionVariable() const;










bool IsThreadPlanDone(ThreadPlan *plan) const;










bool WasThreadPlanDiscarded(ThreadPlan *plan) const;






bool CompletedPlanOverridesBreakpoint() const;













Status QueueThreadPlan(lldb::ThreadPlanSP &plan_sp, bool abort_other_plans);





void DiscardThreadPlans(bool force);







void DiscardThreadPlansUpToPlan(lldb::ThreadPlanSP &up_to_plan_sp);

void DiscardThreadPlansUpToPlan(ThreadPlan *up_to_plan_ptr);













bool DiscardUserThreadPlansUpToIndex(uint32_t thread_index);

virtual bool CheckpointThreadState(ThreadStateCheckpoint &saved_state);

virtual bool
RestoreRegisterStateFromCheckpoint(ThreadStateCheckpoint &saved_state);

void RestoreThreadStateFromCheckpoint(ThreadStateCheckpoint &saved_state);





uint32_t GetIndexID() const;










virtual uint32_t GetExtendedBacktraceOriginatingIndexID() {
return GetIndexID();
}





virtual lldb::user_id_t GetProtocolID() const { return GetID(); }


lldb::TargetSP CalculateTarget() override;

lldb::ProcessSP CalculateProcess() override;

lldb::ThreadSP CalculateThread() override;

lldb::StackFrameSP CalculateStackFrame() override;

void CalculateExecutionContext(ExecutionContext &exe_ctx) override;

lldb::StackFrameSP
GetStackFrameSPForStackFramePtr(StackFrame *stack_frame_ptr);

size_t GetStatus(Stream &strm, uint32_t start_frame, uint32_t num_frames,
uint32_t num_frames_with_source, bool stop_format,
bool only_stacks = false);

size_t GetStackFrameStatus(Stream &strm, uint32_t first_frame,
uint32_t num_frames, bool show_frame_info,
uint32_t num_frames_with_source);





bool IsValid() const { return !m_destroy_called; }














virtual lldb::StopInfoSP GetPrivateStopInfo();










void CalculatePublicStopInfo();








virtual bool CalculateStopInfo() = 0;














lldb::StateType GetTemporaryResumeState() const {
return m_temporary_resume_state;
}

void SetStopInfo(const lldb::StopInfoSP &stop_info_sp);

void ResetStopInfo();

void SetShouldReportStop(Vote vote);







virtual void SetExtendedBacktraceToken(uint64_t token) {}









virtual uint64_t GetExtendedBacktraceToken() { return LLDB_INVALID_ADDRESS; }

lldb::ValueObjectSP GetCurrentException();

lldb::ThreadSP GetCurrentExceptionBacktrace();

protected:
friend class ThreadPlan;
friend class ThreadList;
friend class ThreadEventData;
friend class StackFrameList;
friend class StackFrame;
friend class OperatingSystem;




virtual void DestroyThread();

ThreadPlanStack &GetPlans() const;

void PushPlan(lldb::ThreadPlanSP plan_sp);

void PopPlan();

void DiscardPlan();

ThreadPlan *GetPreviousPlan(ThreadPlan *plan) const;

virtual Unwind &GetUnwinder();



virtual bool IsStillAtLastBreakpointHit();





virtual bool IsOperatingSystemPluginThread() const { return false; }



virtual lldb_private::StructuredData::ObjectSP FetchThreadExtendedInfo() {
return StructuredData::ObjectSP();
}

lldb::StackFrameListSP GetStackFrameList();

void SetTemporaryResumeState(lldb::StateType new_state) {
m_temporary_resume_state = new_state;
}

void FrameSelectedCallback(lldb_private::StackFrame *frame);


lldb::ProcessWP m_process_wp;
lldb::StopInfoSP m_stop_info_sp;
uint32_t m_stop_info_stop_id;



uint32_t m_stop_info_override_stop_id;


const uint32_t m_index_id;

lldb::RegisterContextSP m_reg_context_sp;

lldb::StateType m_state;
mutable std::recursive_mutex
m_state_mutex;
mutable std::recursive_mutex
m_frame_mutex;
lldb::StackFrameListSP m_curr_frames_sp;

lldb::StackFrameListSP m_prev_frames_sp;

int m_resume_signal;

lldb::StateType m_resume_state;


lldb::StateType m_temporary_resume_state;




std::unique_ptr<lldb_private::Unwind> m_unwinder_up;
bool m_destroy_called;

LazyBool m_override_should_notify;
mutable std::unique_ptr<ThreadPlanStack> m_null_plan_stack_up;

private:
bool m_extended_info_fetched;

StructuredData::ObjectSP m_extended_info;

void BroadcastSelectedFrameChange(StackID &new_frame_id);

Thread(const Thread &) = delete;
const Thread &operator=(const Thread &) = delete;
};

}

#endif
