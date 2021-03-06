







#if !defined(LLDB_TARGET_THREADPLANSHOULDSTOPHERE_H)
#define LLDB_TARGET_THREADPLANSHOULDSTOPHERE_H

#include "lldb/Target/ThreadPlan.h"

namespace lldb_private {



















class ThreadPlanShouldStopHere {
public:
struct ThreadPlanShouldStopHereCallbacks {
ThreadPlanShouldStopHereCallbacks() {
should_stop_here_callback = nullptr;
step_from_here_callback = nullptr;
}

ThreadPlanShouldStopHereCallbacks(
ThreadPlanShouldStopHereCallback should_stop,
ThreadPlanStepFromHereCallback step_from_here) {
should_stop_here_callback = should_stop;
step_from_here_callback = step_from_here;
}

void Clear() {
should_stop_here_callback = nullptr;
step_from_here_callback = nullptr;
}

ThreadPlanShouldStopHereCallback should_stop_here_callback;
ThreadPlanStepFromHereCallback step_from_here_callback;
};

enum {
eNone = 0,
eAvoidInlines = (1 << 0),
eStepInAvoidNoDebug = (1 << 1),
eStepOutAvoidNoDebug = (1 << 2)
};


ThreadPlanShouldStopHere(ThreadPlan *owner);

ThreadPlanShouldStopHere(ThreadPlan *owner,
const ThreadPlanShouldStopHereCallbacks *callbacks,
void *baton = nullptr);
virtual ~ThreadPlanShouldStopHere();






void
SetShouldStopHereCallbacks(const ThreadPlanShouldStopHereCallbacks *callbacks,
void *baton) {
if (callbacks) {
m_callbacks = *callbacks;
if (!m_callbacks.should_stop_here_callback)
m_callbacks.should_stop_here_callback =
ThreadPlanShouldStopHere::DefaultShouldStopHereCallback;
if (!m_callbacks.step_from_here_callback)
m_callbacks.step_from_here_callback =
ThreadPlanShouldStopHere::DefaultStepFromHereCallback;
} else {
ClearShouldStopHereCallbacks();
}
m_baton = baton;
}

void ClearShouldStopHereCallbacks() { m_callbacks.Clear(); }

bool InvokeShouldStopHereCallback(lldb::FrameComparison operation,
Status &status);

lldb::ThreadPlanSP
CheckShouldStopHereAndQueueStepOut(lldb::FrameComparison operation,
Status &status);

lldb_private::Flags &GetFlags() { return m_flags; }

const lldb_private::Flags &GetFlags() const { return m_flags; }

protected:
static bool DefaultShouldStopHereCallback(ThreadPlan *current_plan,
Flags &flags,
lldb::FrameComparison operation,
Status &status, void *baton);

static lldb::ThreadPlanSP
DefaultStepFromHereCallback(ThreadPlan *current_plan, Flags &flags,
lldb::FrameComparison operation, Status &status,
void *baton);

virtual lldb::ThreadPlanSP
QueueStepOutFromHerePlan(Flags &flags, lldb::FrameComparison operation,
Status &status);



virtual void SetFlagsToDefault() = 0;

ThreadPlanShouldStopHereCallbacks m_callbacks;
void *m_baton;
ThreadPlan *m_owner;
lldb_private::Flags m_flags;

private:
ThreadPlanShouldStopHere(const ThreadPlanShouldStopHere &) = delete;
const ThreadPlanShouldStopHere &
operator=(const ThreadPlanShouldStopHere &) = delete;
};

}

#endif
