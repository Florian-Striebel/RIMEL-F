







#if !defined(LLDB_TARGET_THREADPLANSTEPRANGE_H)
#define LLDB_TARGET_THREADPLANSTEPRANGE_H

#include "lldb/Core/AddressRange.h"
#include "lldb/Target/StackID.h"
#include "lldb/Target/Thread.h"
#include "lldb/Target/ThreadPlan.h"
#include "lldb/Target/ThreadPlanShouldStopHere.h"

namespace lldb_private {

class ThreadPlanStepRange : public ThreadPlan {
public:
ThreadPlanStepRange(ThreadPlanKind kind, const char *name, Thread &thread,
const AddressRange &range,
const SymbolContext &addr_context,
lldb::RunMode stop_others,
bool given_ranges_only = false);

~ThreadPlanStepRange() override;

void GetDescription(Stream *s, lldb::DescriptionLevel level) override = 0;
bool ValidatePlan(Stream *error) override;
bool ShouldStop(Event *event_ptr) override = 0;
Vote ShouldReportStop(Event *event_ptr) override;
bool StopOthers() override;
lldb::StateType GetPlanRunState() override;
bool WillStop() override;
bool MischiefManaged() override;
void DidPush() override;
bool IsPlanStale() override;

void AddRange(const AddressRange &new_range);

protected:
bool InRange();
lldb::FrameComparison CompareCurrentFrameToStartFrame();
bool InSymbol();
void DumpRanges(Stream *s);

Disassembler *GetDisassembler();

InstructionList *GetInstructionsForAddress(lldb::addr_t addr,
size_t &range_index,
size_t &insn_offset);





bool SetNextBranchBreakpoint();

void ClearNextBranchBreakpoint();

bool NextRangeBreakpointExplainsStop(lldb::StopInfoSP stop_info_sp);

SymbolContext m_addr_context;
std::vector<AddressRange> m_address_ranges;
lldb::RunMode m_stop_others;
StackID m_stack_id;
StackID m_parent_stack_id;

bool m_no_more_plans;


bool m_first_run_event;

lldb::BreakpointSP m_next_branch_bp_sp;
bool m_use_fast_step;
bool m_given_ranges_only;
bool m_found_calls = false;






private:
std::vector<lldb::DisassemblerSP> m_instruction_ranges;

ThreadPlanStepRange(const ThreadPlanStepRange &) = delete;
const ThreadPlanStepRange &operator=(const ThreadPlanStepRange &) = delete;
};

}

#endif
