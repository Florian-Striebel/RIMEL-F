







#if !defined(LLDB_TARGET_EXECUTIONCONTEXT_H)
#define LLDB_TARGET_EXECUTIONCONTEXT_H

#include <mutex>

#include "lldb/Target/StackID.h"
#include "lldb/lldb-private.h"

namespace lldb_private {






















































class ExecutionContextRef {
public:

ExecutionContextRef();


ExecutionContextRef(const ExecutionContextRef &rhs);






ExecutionContextRef(const ExecutionContext *exe_ctx_ptr);




ExecutionContextRef(const ExecutionContext &exe_ctx);





ExecutionContextRef(Target *target, bool adopt_selected);











ExecutionContextRef(ExecutionContextScope *exe_scope);










ExecutionContextRef(ExecutionContextScope &exe_scope);

~ExecutionContextRef();




ExecutionContextRef &operator=(const ExecutionContextRef &rhs);




ExecutionContextRef &operator=(const ExecutionContext &exe_ctx);





void Clear();














void SetTargetSP(const lldb::TargetSP &target_sp);














void SetProcessSP(const lldb::ProcessSP &process_sp);














void SetThreadSP(const lldb::ThreadSP &thread_sp);














void SetFrameSP(const lldb::StackFrameSP &frame_sp);

void SetTargetPtr(Target *target, bool adopt_selected);

void SetProcessPtr(Process *process);

void SetThreadPtr(Thread *thread);

void SetFramePtr(StackFrame *frame);






lldb::TargetSP GetTargetSP() const;






lldb::ProcessSP GetProcessSP() const;






lldb::ThreadSP GetThreadSP() const;






lldb::StackFrameSP GetFrameSP() const;








ExecutionContext Lock(bool thread_and_frame_only_if_stopped) const;




bool HasThreadRef() const { return m_tid != LLDB_INVALID_THREAD_ID; }




bool HasFrameRef() const { return m_stack_id.IsValid(); }

void ClearThread() {
m_thread_wp.reset();
m_tid = LLDB_INVALID_THREAD_ID;
}

void ClearFrame() { m_stack_id.Clear(); }

protected:

lldb::TargetWP m_target_wp;
lldb::ProcessWP m_process_wp;
mutable lldb::ThreadWP m_thread_wp;
lldb::tid_t m_tid = LLDB_INVALID_THREAD_ID;


StackID m_stack_id;

};




















class ExecutionContext {
public:

ExecutionContext();


ExecutionContext(const ExecutionContext &rhs);


ExecutionContext(Target *t, bool fill_current_process_thread_frame = true);


ExecutionContext(const lldb::TargetSP &target_sp, bool get_process);
ExecutionContext(const lldb::ProcessSP &process_sp);
ExecutionContext(const lldb::ThreadSP &thread_sp);
ExecutionContext(const lldb::StackFrameSP &frame_sp);


ExecutionContext(const lldb::TargetWP &target_wp, bool get_process);
ExecutionContext(const lldb::ProcessWP &process_wp);
ExecutionContext(const lldb::ThreadWP &thread_wp);
ExecutionContext(const lldb::StackFrameWP &frame_wp);
ExecutionContext(const ExecutionContextRef &exe_ctx_ref);
ExecutionContext(const ExecutionContextRef *exe_ctx_ref,
bool thread_and_frame_only_if_stopped = false);



ExecutionContext(const ExecutionContextRef &exe_ctx_ref,
std::unique_lock<std::recursive_mutex> &locker);
ExecutionContext(const ExecutionContextRef *exe_ctx_ref,
std::unique_lock<std::recursive_mutex> &locker);

ExecutionContext(ExecutionContextScope *exe_scope);
ExecutionContext(ExecutionContextScope &exe_scope);













ExecutionContext(Process *process, Thread *thread = nullptr,
StackFrame *frame = nullptr);

~ExecutionContext();

ExecutionContext &operator=(const ExecutionContext &rhs);

bool operator==(const ExecutionContext &rhs) const;

bool operator!=(const ExecutionContext &rhs) const;





void Clear();

RegisterContext *GetRegisterContext() const;

ExecutionContextScope *GetBestExecutionContextScope() const;

uint32_t GetAddressByteSize() const;

lldb::ByteOrder GetByteOrder() const;












Target *GetTargetPtr() const;











Process *GetProcessPtr() const;










Thread *GetThreadPtr() const { return m_thread_sp.get(); }








StackFrame *GetFramePtr() const { return m_frame_sp.get(); }











Target &GetTargetRef() const;










Process &GetProcessRef() const;









Thread &GetThreadRef() const;







StackFrame &GetFrameRef() const;




const lldb::TargetSP &GetTargetSP() const { return m_target_sp; }




const lldb::ProcessSP &GetProcessSP() const { return m_process_sp; }




const lldb::ThreadSP &GetThreadSP() const { return m_thread_sp; }




const lldb::StackFrameSP &GetFrameSP() const { return m_frame_sp; }


void SetTargetSP(const lldb::TargetSP &target_sp);


void SetProcessSP(const lldb::ProcessSP &process_sp);


void SetThreadSP(const lldb::ThreadSP &thread_sp);


void SetFrameSP(const lldb::StackFrameSP &frame_sp);



void SetTargetPtr(Target *target);



void SetProcessPtr(Process *process);



void SetThreadPtr(Thread *thread);


void SetFramePtr(StackFrame *frame);






void SetContext(const lldb::TargetSP &target_sp, bool get_process);






void SetContext(const lldb::ProcessSP &process_sp);






void SetContext(const lldb::ThreadSP &thread_sp);





void SetContext(const lldb::StackFrameSP &frame_sp);






bool HasTargetScope() const;








bool HasProcessScope() const;








bool HasThreadScope() const;








bool HasFrameScope() const;

protected:

lldb::TargetSP m_target_sp;
lldb::ProcessSP m_process_sp;
lldb::ThreadSP m_thread_sp;
lldb::StackFrameSP m_frame_sp;
};

}

#endif
