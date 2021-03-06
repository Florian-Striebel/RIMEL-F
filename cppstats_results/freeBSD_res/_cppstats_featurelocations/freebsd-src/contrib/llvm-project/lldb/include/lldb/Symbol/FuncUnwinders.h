#if !defined(LLDB_SYMBOL_FUNCUNWINDERS_H)
#define LLDB_SYMBOL_FUNCUNWINDERS_H

#include "lldb/Core/AddressRange.h"
#include "lldb/lldb-private-enumerations.h"
#include <mutex>
#include <vector>

namespace lldb_private {

class UnwindTable;

class FuncUnwinders {
public:



















FuncUnwinders(lldb_private::UnwindTable &unwind_table, AddressRange range);

~FuncUnwinders();

lldb::UnwindPlanSP GetUnwindPlanAtCallSite(Target &target, Thread &thread);

lldb::UnwindPlanSP GetUnwindPlanAtNonCallSite(Target &target,
lldb_private::Thread &thread);

lldb::UnwindPlanSP GetUnwindPlanFastUnwind(Target &target,
lldb_private::Thread &thread);

lldb::UnwindPlanSP
GetUnwindPlanArchitectureDefault(lldb_private::Thread &thread);

lldb::UnwindPlanSP
GetUnwindPlanArchitectureDefaultAtFunctionEntry(lldb_private::Thread &thread);

Address &GetFirstNonPrologueInsn(Target &target);

const Address &GetFunctionStartAddress() const;

bool ContainsAddress(const Address &addr) const {
return m_range.ContainsFileAddress(addr);
}






Address GetLSDAAddress(Target &target);





Address GetPersonalityRoutinePtrAddress(Target &target);





lldb::UnwindPlanSP GetAssemblyUnwindPlan(Target &target, Thread &thread);

lldb::UnwindPlanSP GetObjectFileUnwindPlan(Target &target);

lldb::UnwindPlanSP GetObjectFileAugmentedUnwindPlan(Target &target,
Thread &thread);

lldb::UnwindPlanSP GetEHFrameUnwindPlan(Target &target);

lldb::UnwindPlanSP GetEHFrameAugmentedUnwindPlan(Target &target,
Thread &thread);

lldb::UnwindPlanSP GetDebugFrameUnwindPlan(Target &target);

lldb::UnwindPlanSP GetDebugFrameAugmentedUnwindPlan(Target &target,
Thread &thread);

lldb::UnwindPlanSP GetCompactUnwindUnwindPlan(Target &target);

lldb::UnwindPlanSP GetArmUnwindUnwindPlan(Target &target);

lldb::UnwindPlanSP GetSymbolFileUnwindPlan(Thread &thread);

lldb::UnwindPlanSP GetArchDefaultUnwindPlan(Thread &thread);

lldb::UnwindPlanSP GetArchDefaultAtFuncEntryUnwindPlan(Thread &thread);

private:
lldb::UnwindAssemblySP GetUnwindAssemblyProfiler(Target &target);






lldb_private::LazyBool CompareUnwindPlansForIdenticalInitialPCLocation(
Thread &thread, const lldb::UnwindPlanSP &a, const lldb::UnwindPlanSP &b);

UnwindTable &m_unwind_table;
AddressRange m_range;

std::recursive_mutex m_mutex;

lldb::UnwindPlanSP m_unwind_plan_assembly_sp;
lldb::UnwindPlanSP m_unwind_plan_object_file_sp;
lldb::UnwindPlanSP m_unwind_plan_eh_frame_sp;
lldb::UnwindPlanSP m_unwind_plan_debug_frame_sp;


lldb::UnwindPlanSP m_unwind_plan_object_file_augmented_sp;
lldb::UnwindPlanSP m_unwind_plan_eh_frame_augmented_sp;
lldb::UnwindPlanSP m_unwind_plan_debug_frame_augmented_sp;

std::vector<lldb::UnwindPlanSP> m_unwind_plan_compact_unwind;
lldb::UnwindPlanSP m_unwind_plan_arm_unwind_sp;
lldb::UnwindPlanSP m_unwind_plan_symbol_file_sp;
lldb::UnwindPlanSP m_unwind_plan_fast_sp;
lldb::UnwindPlanSP m_unwind_plan_arch_default_sp;
lldb::UnwindPlanSP m_unwind_plan_arch_default_at_func_entry_sp;



bool m_tried_unwind_plan_assembly : 1, m_tried_unwind_plan_eh_frame : 1,
m_tried_unwind_plan_object_file : 1,
m_tried_unwind_plan_debug_frame : 1,
m_tried_unwind_plan_object_file_augmented : 1,
m_tried_unwind_plan_eh_frame_augmented : 1,
m_tried_unwind_plan_debug_frame_augmented : 1,
m_tried_unwind_plan_compact_unwind : 1,
m_tried_unwind_plan_arm_unwind : 1, m_tried_unwind_plan_symbol_file : 1,
m_tried_unwind_fast : 1, m_tried_unwind_arch_default : 1,
m_tried_unwind_arch_default_at_func_entry : 1;

Address m_first_non_prologue_insn;

FuncUnwinders(const FuncUnwinders &) = delete;
const FuncUnwinders &operator=(const FuncUnwinders &) = delete;

};

}

#endif
