







#if !defined(LLDB_TARGET_THREADPLANSTACK_H)
#define LLDB_TARGET_THREADPLANSTACK_H

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "lldb/Target/Target.h"
#include "lldb/Target/Thread.h"
#include "lldb/lldb-private-forward.h"
#include "lldb/lldb-private.h"

namespace lldb_private {








class ThreadPlanStack {
friend class lldb_private::Thread;

public:
ThreadPlanStack(const Thread &thread, bool make_empty = false);
~ThreadPlanStack() = default;

using PlanStack = std::vector<lldb::ThreadPlanSP>;

void DumpThreadPlans(Stream &s, lldb::DescriptionLevel desc_level,
bool include_internal) const;

size_t CheckpointCompletedPlans();

void RestoreCompletedPlanCheckpoint(size_t checkpoint);

void DiscardCompletedPlanCheckpoint(size_t checkpoint);

void ThreadDestroyed(Thread *thread);

void PushPlan(lldb::ThreadPlanSP new_plan_sp);

lldb::ThreadPlanSP PopPlan();

lldb::ThreadPlanSP DiscardPlan();



void DiscardPlansUpToPlan(ThreadPlan *up_to_plan_ptr);

void DiscardAllPlans();

void DiscardConsultingMasterPlans();

lldb::ThreadPlanSP GetCurrentPlan() const;

lldb::ThreadPlanSP GetCompletedPlan(bool skip_private = true) const;

lldb::ThreadPlanSP GetPlanByIndex(uint32_t plan_idx,
bool skip_private = true) const;

lldb::ValueObjectSP GetReturnValueObject() const;

lldb::ExpressionVariableSP GetExpressionVariable() const;

bool AnyPlans() const;

bool AnyCompletedPlans() const;

bool AnyDiscardedPlans() const;

bool IsPlanDone(ThreadPlan *plan) const;

bool WasPlanDiscarded(ThreadPlan *plan) const;

ThreadPlan *GetPreviousPlan(ThreadPlan *current_plan) const;

ThreadPlan *GetInnermostExpression() const;

void WillResume();





void ClearThreadCache();

private:
void PrintOneStack(Stream &s, llvm::StringRef stack_name,
const PlanStack &stack, lldb::DescriptionLevel desc_level,
bool include_internal) const;

PlanStack m_plans;
PlanStack m_completed_plans;


PlanStack m_discarded_plans;


size_t m_completed_plan_checkpoint = 0;

std::unordered_map<size_t, PlanStack> m_completed_plan_store;
mutable std::recursive_mutex m_stack_mutex;
};

class ThreadPlanStackMap {
public:
ThreadPlanStackMap(Process &process) : m_process(process) {}
~ThreadPlanStackMap() = default;


void Update(ThreadList &current_threads, bool delete_missing,
bool check_for_new = true);

void AddThread(Thread &thread) {
lldb::tid_t tid = thread.GetID();
m_plans_list.emplace(tid, thread);
}

bool RemoveTID(lldb::tid_t tid) {
auto result = m_plans_list.find(tid);
if (result == m_plans_list.end())
return false;
result->second.ThreadDestroyed(nullptr);
m_plans_list.erase(result);
return true;
}

ThreadPlanStack *Find(lldb::tid_t tid) {
auto result = m_plans_list.find(tid);
if (result == m_plans_list.end())
return nullptr;
else
return &result->second;
}





void ClearThreadCache() {
for (auto &plan_list : m_plans_list)
plan_list.second.ClearThreadCache();
}

void Clear() {
for (auto &plan : m_plans_list)
plan.second.ThreadDestroyed(nullptr);
m_plans_list.clear();
}


void DumpPlans(Stream &strm, lldb::DescriptionLevel desc_level, bool internal,
bool ignore_boring, bool skip_unreported);


bool DumpPlansForTID(Stream &strm, lldb::tid_t tid,
lldb::DescriptionLevel desc_level, bool internal,
bool ignore_boring, bool skip_unreported);

bool PrunePlansForTID(lldb::tid_t tid);

private:
Process &m_process;
using PlansList = std::unordered_map<lldb::tid_t, ThreadPlanStack>;
PlansList m_plans_list;
};

}

#endif
