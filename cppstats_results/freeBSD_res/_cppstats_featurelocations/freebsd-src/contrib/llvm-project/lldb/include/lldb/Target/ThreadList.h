







#if !defined(LLDB_TARGET_THREADLIST_H)
#define LLDB_TARGET_THREADLIST_H

#include <mutex>
#include <vector>

#include "lldb/Target/Thread.h"
#include "lldb/Target/ThreadCollection.h"
#include "lldb/Utility/Iterable.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"

namespace lldb_private {




class ThreadList : public ThreadCollection {
friend class Process;

public:
ThreadList(Process *process);

ThreadList(const ThreadList &rhs);

~ThreadList() override;

const ThreadList &operator=(const ThreadList &rhs);

uint32_t GetSize(bool can_update = true);



lldb::ThreadSP GetSelectedThread();




class ExpressionExecutionThreadPusher {
public:
ExpressionExecutionThreadPusher(ThreadList &thread_list, lldb::tid_t tid)
: m_thread_list(&thread_list), m_tid(tid) {
m_thread_list->PushExpressionExecutionThread(m_tid);
}

ExpressionExecutionThreadPusher(lldb::ThreadSP thread_sp);

~ExpressionExecutionThreadPusher() {
if (m_thread_list && m_tid != LLDB_INVALID_THREAD_ID)
m_thread_list->PopExpressionExecutionThread(m_tid);
}

private:
ThreadList *m_thread_list;
lldb::tid_t m_tid;
};

lldb::ThreadSP GetExpressionExecutionThread();

protected:
void PushExpressionExecutionThread(lldb::tid_t tid);

void PopExpressionExecutionThread(lldb::tid_t tid);

public:
bool SetSelectedThreadByID(lldb::tid_t tid, bool notify = false);

bool SetSelectedThreadByIndexID(uint32_t index_id, bool notify = false);

void Clear();

void Flush();

void Destroy();




lldb::ThreadSP GetThreadAtIndex(uint32_t idx, bool can_update = true);

lldb::ThreadSP FindThreadByID(lldb::tid_t tid, bool can_update = true);

lldb::ThreadSP FindThreadByProtocolID(lldb::tid_t tid,
bool can_update = true);

lldb::ThreadSP RemoveThreadByID(lldb::tid_t tid, bool can_update = true);

lldb::ThreadSP RemoveThreadByProtocolID(lldb::tid_t tid,
bool can_update = true);

lldb::ThreadSP FindThreadByIndexID(uint32_t index_id, bool can_update = true);

lldb::ThreadSP GetThreadSPForThreadPtr(Thread *thread_ptr);

lldb::ThreadSP GetBackingThread(const lldb::ThreadSP &real_thread);

bool ShouldStop(Event *event_ptr);

Vote ShouldReportStop(Event *event_ptr);

Vote ShouldReportRun(Event *event_ptr);

void RefreshStateAfterStop();












bool WillResume();

void DidResume();

void DidStop();

void DiscardThreadPlans();

uint32_t GetStopID() const;

void SetStopID(uint32_t stop_id);

std::recursive_mutex &GetMutex() const override;

void Update(ThreadList &rhs);

protected:
void SetShouldReportStop(Vote vote);

void NotifySelectedThreadChanged(lldb::tid_t tid);


Process *m_process;
uint32_t
m_stop_id;
lldb::tid_t
m_selected_tid;
std::vector<lldb::tid_t> m_expression_tid_stack;

private:
ThreadList() = delete;
};

}

#endif
