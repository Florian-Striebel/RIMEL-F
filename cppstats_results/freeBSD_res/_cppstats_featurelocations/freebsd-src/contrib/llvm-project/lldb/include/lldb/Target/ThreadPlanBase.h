







#if !defined(LLDB_TARGET_THREADPLANBASE_H)
#define LLDB_TARGET_THREADPLANBASE_H

#include "lldb/Target/Process.h"
#include "lldb/Target/Thread.h"
#include "lldb/Target/ThreadPlan.h"

namespace lldb_private {






class ThreadPlanBase : public ThreadPlan {
friend class Process;
public:
~ThreadPlanBase() override;

void GetDescription(Stream *s, lldb::DescriptionLevel level) override;
bool ValidatePlan(Stream *error) override;
bool ShouldStop(Event *event_ptr) override;
Vote ShouldReportStop(Event *event_ptr) override;
bool StopOthers() override;
lldb::StateType GetPlanRunState() override;
bool WillStop() override;
bool MischiefManaged() override;

bool OkayToDiscard() override { return false; }

bool IsBasePlan() override { return true; }

protected:
bool DoWillResume(lldb::StateType resume_state, bool current_plan) override;
bool DoPlanExplainsStop(Event *event_ptr) override;
ThreadPlanBase(Thread &thread);

private:
friend lldb::ThreadPlanSP Thread::QueueBasePlan(bool abort_other_plans);

ThreadPlanBase(const ThreadPlanBase &) = delete;
const ThreadPlanBase &operator=(const ThreadPlanBase &) = delete;
};

}

#endif
