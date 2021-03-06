







#if !defined(LLDB_TARGET_STOPINFO_H)
#define LLDB_TARGET_STOPINFO_H

#include <string>

#include "lldb/Target/Process.h"
#include "lldb/Utility/StructuredData.h"
#include "lldb/lldb-public.h"

namespace lldb_private {

class StopInfo {
friend class Process::ProcessEventData;
friend class ThreadPlanBase;

public:

StopInfo(Thread &thread, uint64_t value);

virtual ~StopInfo() = default;

bool IsValid() const;

void SetThread(const lldb::ThreadSP &thread_sp) { m_thread_wp = thread_sp; }

lldb::ThreadSP GetThread() const { return m_thread_wp.lock(); }










uint64_t GetValue() const { return m_value; }

virtual lldb::StopReason GetStopReason() const = 0;





virtual bool ShouldStopSynchronous(Event *event_ptr) { return true; }

void OverrideShouldNotify(bool override_value) {
m_override_should_notify = override_value ? eLazyBoolYes : eLazyBoolNo;
}


virtual bool ShouldNotify(Event *event_ptr) {
if (m_override_should_notify == eLazyBoolCalculate)
return DoShouldNotify(event_ptr);
else
return m_override_should_notify == eLazyBoolYes;
}

virtual void WillResume(lldb::StateType resume_state) {

}

virtual const char *GetDescription() { return m_description.c_str(); }

virtual void SetDescription(const char *desc_cstr) {
if (desc_cstr && desc_cstr[0])
m_description.assign(desc_cstr);
else
m_description.clear();
}

virtual bool IsValidForOperatingSystemThread(Thread &thread) { return true; }








void OverrideShouldStop(bool override_value) {
m_override_should_stop = override_value ? eLazyBoolYes : eLazyBoolNo;
}

bool GetOverrideShouldStop() {
return m_override_should_stop != eLazyBoolCalculate;
}

bool GetOverriddenShouldStopValue() {
return m_override_should_stop == eLazyBoolYes;
}

StructuredData::ObjectSP GetExtendedInfo() { return m_extended_info; }

static lldb::StopInfoSP
CreateStopReasonWithBreakpointSiteID(Thread &thread,
lldb::break_id_t break_id);



static lldb::StopInfoSP CreateStopReasonWithBreakpointSiteID(
Thread &thread, lldb::break_id_t break_id, bool should_stop);

static lldb::StopInfoSP CreateStopReasonWithWatchpointID(
Thread &thread, lldb::break_id_t watch_id,
lldb::addr_t watch_hit_addr = LLDB_INVALID_ADDRESS);

static lldb::StopInfoSP
CreateStopReasonWithSignal(Thread &thread, int signo,
const char *description = nullptr);

static lldb::StopInfoSP CreateStopReasonToTrace(Thread &thread);

static lldb::StopInfoSP
CreateStopReasonWithPlan(lldb::ThreadPlanSP &plan,
lldb::ValueObjectSP return_valobj_sp,
lldb::ExpressionVariableSP expression_variable_sp);

static lldb::StopInfoSP
CreateStopReasonWithException(Thread &thread, const char *description);

static lldb::StopInfoSP CreateStopReasonWithExec(Thread &thread);

static lldb::StopInfoSP
CreateStopReasonProcessorTrace(Thread &thread, const char *description);

static lldb::ValueObjectSP
GetReturnValueObject(lldb::StopInfoSP &stop_info_sp);

static lldb::ExpressionVariableSP
GetExpressionVariable(lldb::StopInfoSP &stop_info_sp);

static lldb::ValueObjectSP
GetCrashingDereference(lldb::StopInfoSP &stop_info_sp,
lldb::addr_t *crashing_address = nullptr);

protected:




virtual void PerformAction(Event *event_ptr) {}

virtual bool DoShouldNotify(Event *event_ptr) { return false; }








virtual bool ShouldStop(Event *event_ptr) { return true; }


lldb::ThreadWP m_thread_wp;
uint32_t m_stop_id;
uint32_t m_resume_id;
uint64_t m_value;

std::string m_description;
LazyBool m_override_should_notify;
LazyBool m_override_should_stop;

StructuredData::ObjectSP
m_extended_info;



bool HasTargetRunSinceMe();





void MakeStopInfoValid();

private:
friend class Thread;

StopInfo(const StopInfo &) = delete;
const StopInfo &operator=(const StopInfo &) = delete;
};

}

#endif
