







#if !defined(LLDB_TARGET_THREADPLANSTEPINRANGE_H)
#define LLDB_TARGET_THREADPLANSTEPINRANGE_H

#include "lldb/Core/AddressRange.h"
#include "lldb/Target/StackID.h"
#include "lldb/Target/Thread.h"
#include "lldb/Target/ThreadPlanShouldStopHere.h"
#include "lldb/Target/ThreadPlanStepRange.h"

namespace lldb_private {

class ThreadPlanStepInRange : public ThreadPlanStepRange,
public ThreadPlanShouldStopHere {
public:
ThreadPlanStepInRange(Thread &thread, const AddressRange &range,
const SymbolContext &addr_context,
const char *step_into_target, lldb::RunMode stop_others,
LazyBool step_in_avoids_code_without_debug_info,
LazyBool step_out_avoids_code_without_debug_info);

~ThreadPlanStepInRange() override;

void GetDescription(Stream *s, lldb::DescriptionLevel level) override;

bool ShouldStop(Event *event_ptr) override;

void SetAvoidRegexp(const char *name);

static void SetDefaultFlagValue(uint32_t new_value);

bool IsVirtualStep() override;



static uint32_t GetDefaultFlagsValue() {
return s_default_flag_values;
}

protected:
static bool DefaultShouldStopHereCallback(ThreadPlan *current_plan,
Flags &flags,
lldb::FrameComparison operation,
Status &status, void *baton);

bool DoWillResume(lldb::StateType resume_state, bool current_plan) override;

bool DoPlanExplainsStop(Event *event_ptr) override;

void SetFlagsToDefault() override {
GetFlags().Set(ThreadPlanStepInRange::s_default_flag_values);
}

void SetCallbacks() {
ThreadPlanShouldStopHere::ThreadPlanShouldStopHereCallbacks callbacks(
ThreadPlanStepInRange::DefaultShouldStopHereCallback, nullptr);
SetShouldStopHereCallbacks(&callbacks, nullptr);
}

bool FrameMatchesAvoidCriteria();

private:
void SetupAvoidNoDebug(LazyBool step_in_avoids_code_without_debug_info,
LazyBool step_out_avoids_code_without_debug_info);



static uint32_t s_default_flag_values;

lldb::ThreadPlanSP m_sub_plan_sp;

std::unique_ptr<RegularExpression> m_avoid_regexp_up;
bool m_step_past_prologue;


bool m_virtual_step;

ConstString m_step_into_target;
ThreadPlanStepInRange(const ThreadPlanStepInRange &) = delete;
const ThreadPlanStepInRange &
operator=(const ThreadPlanStepInRange &) = delete;
};

}

#endif
