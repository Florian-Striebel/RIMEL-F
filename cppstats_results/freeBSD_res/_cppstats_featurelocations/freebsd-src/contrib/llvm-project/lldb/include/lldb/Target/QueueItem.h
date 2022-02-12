







#if !defined(LLDB_TARGET_QUEUEITEM_H)
#define LLDB_TARGET_QUEUEITEM_H

#include <memory>
#include <string>
#include <vector>

#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private.h"

#include "lldb/Core/Address.h"
#include "lldb/Utility/ConstString.h"

namespace lldb_private {








class QueueItem : public std::enable_shared_from_this<QueueItem> {
public:
QueueItem(lldb::QueueSP queue_sp, lldb::ProcessSP process_sp,
lldb::addr_t item_ref, lldb_private::Address address);

~QueueItem();






lldb::QueueItemKind GetKind();





void SetKind(lldb::QueueItemKind item_kind);











lldb_private::Address &GetAddress();






void SetAddress(lldb_private::Address addr);








bool IsValid() { return m_queue_wp.lock() != nullptr; }













lldb::ThreadSP GetExtendedBacktraceThread(ConstString type);

void SetItemThatEnqueuedThis(lldb::addr_t address_of_item) {
m_item_that_enqueued_this_ref = address_of_item;
}

lldb::addr_t GetItemThatEnqueuedThis();

void SetEnqueueingThreadID(lldb::tid_t tid) { m_enqueueing_thread_id = tid; }

lldb::tid_t GetEnqueueingThreadID();

void SetEnqueueingQueueID(lldb::queue_id_t qid) {
m_enqueueing_queue_id = qid;
}

lldb::queue_id_t GetEnqueueingQueueID();

void SetTargetQueueID(lldb::queue_id_t qid) { m_target_queue_id = qid; }

void SetStopID(uint32_t stop_id) { m_stop_id = stop_id; }

uint32_t GetStopID();

void SetEnqueueingBacktrace(std::vector<lldb::addr_t> backtrace) {
m_backtrace = backtrace;
}

std::vector<lldb::addr_t> &GetEnqueueingBacktrace();

void SetThreadLabel(std::string thread_name) { m_thread_label = thread_name; }

std::string GetThreadLabel();

void SetQueueLabel(std::string queue_name) { m_queue_label = queue_name; }

std::string GetQueueLabel();

void SetTargetQueueLabel(std::string queue_name) {
m_target_queue_label = queue_name;
}

lldb::ProcessSP GetProcessSP();

protected:
void FetchEntireItem();

lldb::QueueWP m_queue_wp;
lldb::ProcessWP m_process_wp;

lldb::addr_t m_item_ref;

lldb_private::Address m_address;
bool m_have_fetched_entire_item;

lldb::QueueItemKind m_kind;
lldb::addr_t m_item_that_enqueued_this_ref;


lldb::tid_t m_enqueueing_thread_id;
lldb::queue_id_t
m_enqueueing_queue_id;
lldb::queue_id_t m_target_queue_id;
uint32_t m_stop_id;
std::vector<lldb::addr_t> m_backtrace;
std::string m_thread_label;
std::string m_queue_label;
std::string m_target_queue_label;

private:
QueueItem(const QueueItem &) = delete;
const QueueItem &operator=(const QueueItem &) = delete;
};

}

#endif
