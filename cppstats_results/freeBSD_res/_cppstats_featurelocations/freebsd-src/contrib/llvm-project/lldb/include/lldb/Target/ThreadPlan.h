







#if !defined(LLDB_TARGET_THREADPLAN_H)
#define LLDB_TARGET_THREADPLAN_H

#include <mutex>
#include <string>

#include "lldb/Target/Process.h"
#include "lldb/Target/StopInfo.h"
#include "lldb/Target/Target.h"
#include "lldb/Target/Thread.h"
#include "lldb/Target/ThreadPlanTracer.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"

namespace lldb_private {


































































































































































































































































class ThreadPlan : public std::enable_shared_from_this<ThreadPlan>,
public UserID {
public:


enum ThreadPlanKind {
eKindGeneric,
eKindNull,
eKindBase,
eKindCallFunction,
eKindPython,
eKindStepInstruction,
eKindStepOut,
eKindStepOverBreakpoint,
eKindStepOverRange,
eKindStepInRange,
eKindRunToAddress,
eKindStepThrough,
eKindStepUntil
};

virtual ~ThreadPlan();





const char *GetName() const { return m_name.c_str(); }





Thread &GetThread();

Target &GetTarget();

const Target &GetTarget() const;





void ClearThreadCache();












virtual void GetDescription(Stream *s, lldb::DescriptionLevel level) = 0;










virtual bool ValidatePlan(Stream *error) = 0;

bool TracerExplainsStop() {
if (!m_tracer_sp)
return false;
else
return m_tracer_sp->TracerExplainsStop();
}

lldb::StateType RunState();

bool PlanExplainsStop(Event *event_ptr);

virtual bool ShouldStop(Event *event_ptr) = 0;







virtual bool ShouldAutoContinue(Event *event_ptr) { return false; }




virtual Vote ShouldReportStop(Event *event_ptr);

Vote ShouldReportRun(Event *event_ptr);

virtual void SetStopOthers(bool new_value);

virtual bool StopOthers();



bool WillResume(lldb::StateType resume_state, bool current_plan);

virtual bool WillStop() = 0;

bool IsMasterPlan() { return m_is_master_plan; }

bool SetIsMasterPlan(bool value) {
bool old_value = m_is_master_plan;
m_is_master_plan = value;
return old_value;
}

virtual bool OkayToDiscard();

void SetOkayToDiscard(bool value) { m_okay_to_discard = value; }



virtual bool MischiefManaged();

virtual void ThreadDestroyed() {


}

bool GetPrivate() { return m_plan_private; }

void SetPrivate(bool input) { m_plan_private = input; }

virtual void DidPush();

virtual void WillPop();

ThreadPlanKind GetKind() const { return m_kind; }

bool IsPlanComplete();

void SetPlanComplete(bool success = true);

virtual bool IsPlanStale() { return false; }

bool PlanSucceeded() { return m_plan_succeeded; }

virtual bool IsBasePlan() { return false; }

lldb::ThreadPlanTracerSP &GetThreadPlanTracer() { return m_tracer_sp; }

void SetThreadPlanTracer(lldb::ThreadPlanTracerSP new_tracer_sp) {
m_tracer_sp = new_tracer_sp;
}

void DoTraceLog() {
if (m_tracer_sp && m_tracer_sp->TracingEnabled())
m_tracer_sp->Log();
}






virtual lldb::ValueObjectSP GetReturnValueObject() {
return lldb::ValueObjectSP();
}








virtual lldb::ExpressionVariableSP GetExpressionVariable() {
return lldb::ExpressionVariableSP();
}





virtual void RestoreThreadState() {}

virtual bool IsVirtualStep() { return false; }

bool SetIterationCount(size_t count) {
if (m_takes_iteration_count) {

if (count == 0)
return false;
m_iteration_count = count;
}
return m_takes_iteration_count;
}

protected:

ThreadPlan(ThreadPlanKind kind, const char *name, Thread &thread,
Vote report_stop_vote, Vote report_run_vote);



virtual bool DoWillResume(lldb::StateType resume_state, bool current_plan) {
return true;
}

virtual bool DoPlanExplainsStop(Event *event_ptr) = 0;




void PushPlan(lldb::ThreadPlanSP &thread_plan_sp) {
GetThread().PushPlan(thread_plan_sp);
thread_plan_sp->SetPrivate(true);
thread_plan_sp->SetIsMasterPlan(false);
}





ThreadPlan *GetPreviousPlan() { return GetThread().GetPreviousPlan(this); }




lldb::StopInfoSP GetPrivateStopInfo() {
return GetThread().GetPrivateStopInfo();
}

void SetStopInfo(lldb::StopInfoSP stop_reason_sp) {
GetThread().SetStopInfo(stop_reason_sp);
}

virtual lldb::StateType GetPlanRunState() = 0;

bool IsUsuallyUnexplainedStopReason(lldb::StopReason);

Status m_status;
Process &m_process;
lldb::tid_t m_tid;
Vote m_report_stop_vote;
Vote m_report_run_vote;
bool m_takes_iteration_count;
bool m_could_not_resolve_hw_bp;
int32_t m_iteration_count = 1;

private:
void CachePlanExplainsStop(bool does_explain) {
m_cached_plan_explains_stop = does_explain ? eLazyBoolYes : eLazyBoolNo;
}


static lldb::user_id_t GetNextID();

Thread *m_thread;


ThreadPlanKind m_kind;
std::string m_name;
std::recursive_mutex m_plan_complete_mutex;
LazyBool m_cached_plan_explains_stop;
bool m_plan_complete;
bool m_plan_private;
bool m_okay_to_discard;
bool m_is_master_plan;
bool m_plan_succeeded;

lldb::ThreadPlanTracerSP m_tracer_sp;

ThreadPlan(const ThreadPlan &) = delete;
const ThreadPlan &operator=(const ThreadPlan &) = delete;
};







class ThreadPlanNull : public ThreadPlan {
public:
ThreadPlanNull(Thread &thread);
~ThreadPlanNull() override;

void GetDescription(Stream *s, lldb::DescriptionLevel level) override;

bool ValidatePlan(Stream *error) override;

bool ShouldStop(Event *event_ptr) override;

bool MischiefManaged() override;

bool WillStop() override;

bool IsBasePlan() override { return true; }

bool OkayToDiscard() override { return false; }

const Status &GetStatus() { return m_status; }

protected:
bool DoPlanExplainsStop(Event *event_ptr) override;

lldb::StateType GetPlanRunState() override;

ThreadPlanNull(const ThreadPlanNull &) = delete;
const ThreadPlanNull &operator=(const ThreadPlanNull &) = delete;
};

}

#endif
