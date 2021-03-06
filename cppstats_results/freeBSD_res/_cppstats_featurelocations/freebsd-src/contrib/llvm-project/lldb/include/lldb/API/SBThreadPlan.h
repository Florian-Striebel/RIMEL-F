







#if !defined(LLDB_API_SBTHREADPLAN_H)
#define LLDB_API_SBTHREADPLAN_H

#include "lldb/API/SBDefines.h"

#include <cstdio>

namespace lldb {

class LLDB_API SBThreadPlan {

public:
SBThreadPlan();

SBThreadPlan(const lldb::SBThreadPlan &threadPlan);

SBThreadPlan(const lldb::ThreadPlanSP &lldb_object_sp);

SBThreadPlan(lldb::SBThread &thread, const char *class_name);

SBThreadPlan(lldb::SBThread &thread, const char *class_name,
lldb::SBStructuredData &args_data);

~SBThreadPlan();

explicit operator bool() const;

bool IsValid() const;

void Clear();

lldb::StopReason GetStopReason();



size_t GetStopReasonDataCount();




















uint64_t GetStopReasonDataAtIndex(uint32_t idx);

SBThread GetThread() const;

const lldb::SBThreadPlan &operator=(const lldb::SBThreadPlan &rhs);

bool GetDescription(lldb::SBStream &description) const;

void SetPlanComplete(bool success);

bool IsPlanComplete();

bool IsPlanStale();

bool IsValid();

bool GetStopOthers();

void SetStopOthers(bool stop_others);



SBThreadPlan QueueThreadPlanForStepOverRange(SBAddress &start_address,
lldb::addr_t range_size);
SBThreadPlan QueueThreadPlanForStepOverRange(SBAddress &start_address,
lldb::addr_t range_size,
SBError &error);

SBThreadPlan QueueThreadPlanForStepInRange(SBAddress &start_address,
lldb::addr_t range_size);
SBThreadPlan QueueThreadPlanForStepInRange(SBAddress &start_address,
lldb::addr_t range_size,
SBError &error);

SBThreadPlan QueueThreadPlanForStepOut(uint32_t frame_idx_to_step_to,
bool first_insn = false);
SBThreadPlan QueueThreadPlanForStepOut(uint32_t frame_idx_to_step_to,
bool first_insn, SBError &error);

SBThreadPlan QueueThreadPlanForRunToAddress(SBAddress address);
SBThreadPlan QueueThreadPlanForRunToAddress(SBAddress address,
SBError &error);

SBThreadPlan QueueThreadPlanForStepScripted(const char *script_class_name);
SBThreadPlan QueueThreadPlanForStepScripted(const char *script_class_name,
SBError &error);
SBThreadPlan QueueThreadPlanForStepScripted(const char *script_class_name,
lldb::SBStructuredData &args_data,
SBError &error);

private:
friend class SBBreakpoint;
friend class SBBreakpointLocation;
friend class SBFrame;
friend class SBProcess;
friend class SBDebugger;
friend class SBValue;
friend class lldb_private::QueueImpl;
friend class SBQueueItem;

lldb::ThreadPlanSP GetSP() const { return m_opaque_wp.lock(); }
lldb_private::ThreadPlan *get() const { return GetSP().get(); }
void SetThreadPlan(const lldb::ThreadPlanSP &lldb_object_sp);

lldb::ThreadPlanWP m_opaque_wp;
};

}

#endif
