







#if !defined(LLDB_TARGET_THREADPLANCALLONFUNCTIONEXIT_H)
#define LLDB_TARGET_THREADPLANCALLONFUNCTIONEXIT_H

#include "lldb/Target/ThreadPlan.h"

#include <functional>

namespace lldb_private {





class ThreadPlanCallOnFunctionExit : public ThreadPlan {
public:


using Callback = std::function<void()>;

ThreadPlanCallOnFunctionExit(Thread &thread, const Callback &callback);

void DidPush() override;



void GetDescription(Stream *s, lldb::DescriptionLevel level) override;

bool ValidatePlan(Stream *error) override;

bool ShouldStop(Event *event_ptr) override;

bool WillStop() override;

protected:
bool DoPlanExplainsStop(Event *event_ptr) override;

lldb::StateType GetPlanRunState() override;

private:
Callback m_callback;
lldb::ThreadPlanSP m_step_out_threadplan_sp;
};
}

#endif
