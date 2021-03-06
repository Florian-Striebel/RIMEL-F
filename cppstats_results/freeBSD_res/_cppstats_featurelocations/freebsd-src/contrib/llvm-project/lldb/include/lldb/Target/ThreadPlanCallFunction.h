







#if !defined(LLDB_TARGET_THREADPLANCALLFUNCTION_H)
#define LLDB_TARGET_THREADPLANCALLFUNCTION_H

#include "lldb/Target/Thread.h"
#include "lldb/Target/ThreadPlan.h"
#include "lldb/lldb-private.h"

#include "llvm/ADT/ArrayRef.h"

namespace lldb_private {

class ThreadPlanCallFunction : public ThreadPlan {



public:
ThreadPlanCallFunction(Thread &thread, const Address &function,
const CompilerType &return_type,
llvm::ArrayRef<lldb::addr_t> args,
const EvaluateExpressionOptions &options);

ThreadPlanCallFunction(Thread &thread, const Address &function,
const EvaluateExpressionOptions &options);

~ThreadPlanCallFunction() override;

void GetDescription(Stream *s, lldb::DescriptionLevel level) override;

bool ValidatePlan(Stream *error) override;

bool ShouldStop(Event *event_ptr) override;

Vote ShouldReportStop(Event *event_ptr) override;

bool StopOthers() override;

lldb::StateType GetPlanRunState() override;

void DidPush() override;

bool WillStop() override;

bool MischiefManaged() override;










lldb::ValueObjectSP GetReturnValueObject() override {
return m_return_valobj_sp;
}




lldb::addr_t GetFunctionStackPointer() { return m_function_sp; }




void WillPop() override;









virtual lldb::StopInfoSP GetRealStopInfo() {
if (m_real_stop_info_sp)
return m_real_stop_info_sp;
else
return GetPrivateStopInfo();
}

lldb::addr_t GetStopAddress() { return m_stop_address; }

void RestoreThreadState() override;

void ThreadDestroyed() override { m_takedown_done = true; }

void SetStopOthers(bool new_value) override;

protected:
void ReportRegisterState(const char *message);

bool DoPlanExplainsStop(Event *event_ptr) override;

virtual void SetReturnValue();

bool ConstructorSetup(Thread &thread, ABI *&abi,
lldb::addr_t &start_load_addr,
lldb::addr_t &function_load_addr);

virtual void DoTakedown(bool success);

void SetBreakpoints();

void ClearBreakpoints();

bool BreakpointsExplainStop();

bool m_valid;
bool m_stop_other_threads;
bool m_unwind_on_error;
bool m_ignore_breakpoints;
bool m_debug_execution;
bool m_trap_exceptions;
Address m_function_addr;
Address m_start_addr;
lldb::addr_t m_function_sp;
lldb::ThreadPlanSP m_subplan_sp;
LanguageRuntime *m_cxx_language_runtime;
LanguageRuntime *m_objc_language_runtime;
Thread::ThreadStateCheckpoint m_stored_thread_state;
lldb::StopInfoSP
m_real_stop_info_sp;



StreamString m_constructor_errors;
lldb::ValueObjectSP m_return_valobj_sp;


bool m_takedown_done;

bool m_should_clear_objc_exception_bp;
bool m_should_clear_cxx_exception_bp;
lldb::addr_t m_stop_address;


private:
CompilerType m_return_type;
ThreadPlanCallFunction(const ThreadPlanCallFunction &) = delete;
const ThreadPlanCallFunction &
operator=(const ThreadPlanCallFunction &) = delete;
};

}

#endif
