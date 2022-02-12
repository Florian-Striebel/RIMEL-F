







#if !defined(LLDB_TARGET_THREADPLANSTEPOUT_H)
#define LLDB_TARGET_THREADPLANSTEPOUT_H

#include "lldb/Target/Thread.h"
#include "lldb/Target/ThreadPlan.h"
#include "lldb/Target/ThreadPlanShouldStopHere.h"

namespace lldb_private {

class ThreadPlanStepOut : public ThreadPlan, public ThreadPlanShouldStopHere {
public:
ThreadPlanStepOut(Thread &thread, SymbolContext *addr_context,
bool first_insn, bool stop_others, Vote report_stop_vote,
Vote report_run_vote, uint32_t frame_idx,
LazyBool step_out_avoids_code_without_debug_info,
bool continue_to_next_branch = false,
bool gather_return_value = true);

~ThreadPlanStepOut() override;

void GetDescription(Stream *s, lldb::DescriptionLevel level) override;
bool ValidatePlan(Stream *error) override;
bool ShouldStop(Event *event_ptr) override;
bool StopOthers() override;
lldb::StateType GetPlanRunState() override;
bool WillStop() override;
bool MischiefManaged() override;
void DidPush() override;
bool IsPlanStale() override;

lldb::ValueObjectSP GetReturnValueObject() override {
return m_return_valobj_sp;
}

protected:
void SetFlagsToDefault() override {
GetFlags().Set(ThreadPlanStepOut::s_default_flag_values);
}

bool DoPlanExplainsStop(Event *event_ptr) override;
bool DoWillResume(lldb::StateType resume_state, bool current_plan) override;
bool QueueInlinedStepPlan(bool queue_now);

private:
static uint32_t s_default_flag_values;


lldb::addr_t m_step_from_insn;
StackID m_step_out_to_id;
StackID m_immediate_step_from_id;
lldb::break_id_t m_return_bp_id;
lldb::addr_t m_return_addr;
bool m_stop_others;
lldb::ThreadPlanSP m_step_out_to_inline_plan_sp;



lldb::ThreadPlanSP m_step_through_inline_plan_sp;

lldb::ThreadPlanSP m_step_out_further_plan_sp;


Function *m_immediate_step_from_function;
std::vector<lldb::StackFrameSP> m_stepped_past_frames;
lldb::ValueObjectSP m_return_valobj_sp;
bool m_calculate_return_value;
StreamString m_constructor_errors;

friend lldb::ThreadPlanSP Thread::QueueThreadPlanForStepOut(
bool abort_other_plans, SymbolContext *addr_context, bool first_insn,
bool stop_others, Vote report_stop_vote, Vote report_run_vote,
uint32_t frame_idx, Status &status,
LazyBool step_out_avoids_code_without_debug_info);

void SetupAvoidNoDebug(LazyBool step_out_avoids_code_without_debug_info);



void CalculateReturnValue();

ThreadPlanStepOut(const ThreadPlanStepOut &) = delete;
const ThreadPlanStepOut &operator=(const ThreadPlanStepOut &) = delete;
};

}

#endif
